#include "process_pool.h"

#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "exit_codes.h"
#include "util.h"

struct process_pool_s {
  size_t max_procs;
  size_t capacity;
  long child_max;
  size_t count;
  int status;
  uint8_t halt;
  pid_t* pids;
};

static volatile sig_atomic_t sigusr1_count = 0;
static volatile sig_atomic_t sigusr2_count = 0;
static sig_atomic_t applied_sigusr1 = 0;
static sig_atomic_t applied_sigusr2 = 0;

static void on_sigusr1(int sig) {
  (void) sig;
  ++sigusr1_count;
}

static void on_sigusr2(int sig) {
  (void) sig;
  ++sigusr2_count;
}

static int severity(int phxargs_status) {
  switch (phxargs_status) {
    case PHXARGS_STATUS_NOT_FOUND:
      return 6;
    case PHXARGS_STATUS_NOT_EXECUTABLE:
      return 5;
    case PHXARGS_STATUS_SIGNALLED:
      return 4;
    case PHXARGS_STATUS_HALT:
      return 3;
    case PHXARGS_STATUS_CHILD_FAILED:
      return 2;
    default: /* success */
      return 0;
  }
}

static void accumulate_status(process_pool* pool, int new_status) {
  if (severity(new_status) > severity(pool->status)) {
    pool->status = new_status;
  }
}

static int child_exit_status(int raw_status, uint8_t* halt) {
  if (WIFEXITED(raw_status)) {
    int code = WEXITSTATUS(raw_status);

    if (code == 255) {
      fprintf(stderr, "phxargs: child exited with status 255 -- halting\n");
      *halt = 1;
      return PHXARGS_STATUS_HALT;
    }

    if (code == 0 || code == 126 || code == 127) {
      return code;
    }

    return PHXARGS_STATUS_CHILD_FAILED;
  }

  if (WIFSIGNALED(raw_status)) {
    return PHXARGS_STATUS_SIGNALLED;
  }

  assert(!"reachable");
  exit(EXIT_FAILURE);
}

static void reap_one(process_pool* pool) {
  int raw_status;
  pid_t pid;

  do {
    pid = waitpid(-1, &raw_status, 0);
  } while (pid == -1 && errno == EINTR);

  if (pid == -1) {
    perror("phxargs: waitpid");
    exit(EXIT_FAILURE);
  }

  size_t i;
  for (i = 0; i < pool->count; ++i) {
    if (pool->pids[i] == pid) {
      pool->pids[i] = pool->pids[pool->count - 1];
      break;
    }
  }
  assert(i < pool->count);

  --pool->count;

  accumulate_status(pool, child_exit_status(raw_status, &pool->halt));
}

static void apply_signal_adjustments(process_pool* pool) {
  const sig_atomic_t u1 = sigusr1_count - applied_sigusr1;
  const sig_atomic_t u2 = sigusr2_count - applied_sigusr2;
  if (u1 == 0 && u2 == 0) {
    return;
  }

  applied_sigusr1 = sigusr1_count;
  applied_sigusr2 = sigusr2_count;

  if (pool->max_procs == 0) {
    return;
  }

  long new_max = (long) pool->max_procs + (long) u1 - (long) u2;
  if (new_max < 1) {
    new_max = 1;
  }
  if (pool->child_max > 0 && new_max > pool->child_max) {
    new_max = pool->child_max;
  }

  if ((size_t) new_max > pool->capacity) {
    pool->pids = phxargs_realloc(pool->pids, (size_t) new_max * sizeof(pid_t));
    pool->capacity = (size_t) new_max;
  }

  pool->max_procs = (size_t) new_max;
}

process_pool* process_pool_create(size_t max_procs) {
  process_pool* pool = phxargs_malloc(sizeof(process_pool));

  pool->max_procs = max_procs;
  pool->capacity = max_procs == 0 ? 16 : max_procs;
  pool->child_max = phxargs_sysconf(_SC_CHILD_MAX);
  pool->count = 0;
  pool->status = 0;
  pool->halt = 0;
  pool->pids = phxargs_calloc(pool->capacity, sizeof(pid_t));

  return pool;
}

void process_pool_install_signal_handlers(void) {
  signal(SIGCHLD, SIG_DFL);

  struct sigaction sa;

  sa.sa_flags = SA_RESTART;
  sigemptyset(&sa.sa_mask);
  sa.sa_handler = on_sigusr1;
  sigaction(SIGUSR1, &sa, NULL);
  sa.sa_handler = on_sigusr2;
  sigaction(SIGUSR2, &sa, NULL);
}

bool process_pool_halted(const process_pool* pool) {
  return pool->halt;
}

void process_pool_wait_if_full(process_pool* pool) {
  apply_signal_adjustments(pool);

  while (pool->max_procs != 0 && pool->count >= pool->max_procs) {
    reap_one(pool);
  }
}

void process_pool_submit(process_pool* pool, pid_t pid) {
  if (pool->count == pool->capacity) {
    pool->capacity *= 2;
    pool->pids = phxargs_realloc(pool->pids, pool->capacity * sizeof(pid_t));
  }

  pool->pids[pool->count++] = pid;
}

int process_pool_drain(process_pool* pool) {
  while (pool->count > 0) {
    reap_one(pool);
  }

  return pool->status;
}

void process_pool_destroy(process_pool* pool) {
  free(pool->pids);
  free(pool);
}

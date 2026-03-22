#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "process_pool.h"
#include "util.h"

struct _process_pool {
  size_t max_procs;
  size_t count;
  int status;
  uint8_t halt;
  pid_t* pids;
};

process_pool* process_pool_create(size_t max_procs) {
  process_pool* pool = safe_malloc(sizeof(process_pool));
  pool->max_procs = max_procs;
  pool->count = 0;
  pool->status = 0;
  pool->halt = 0;
  pool->pids = safe_calloc(max_procs, sizeof(pid_t));
  return pool;
}

static int severity(int phxargs_status) {
  switch (phxargs_status) {
    case 127: return 6;
    case 126: return 5;
    case 125: return 4;
    case 124: return 3;
    case 123: return 2;
    case 1: return 1;
    default: return 0;
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
      return 124;
    }
    if (code >= 1 && code <= 125) return 123;
    return code;
  }
  if (WIFSIGNALED(raw_status)) {
    return 125;
  }
  return 1;
}

static void reap_one(process_pool* pool) {
  int raw_status;
  pid_t pid = waitpid(pool->pids[0], &raw_status, 0);

  if (pid == -1) {
    perror("phxargs: waitpid");
    exit(EXIT_FAILURE);
  }

  pool->pids[0] = pool->pids[pool->count - 1];
  --pool->count;

  accumulate_status(pool, child_exit_status(raw_status, &pool->halt));
}

uint8_t process_pool_halted(process_pool* pool) {
  return pool->halt;
}

void process_pool_wait_if_full(process_pool* pool) {
  while (pool->count >= pool->max_procs) {
    reap_one(pool);
  }
}

void process_pool_submit(process_pool* pool, pid_t pid) {
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

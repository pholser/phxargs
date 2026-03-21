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
  pid_t* pids;
};

process_pool* process_pool_create(size_t max_procs) {
  process_pool* pool = safe_malloc(sizeof(process_pool));
  pool->max_procs = max_procs;
  pool->count = 0;
  pool->status = 0;
  pool->pids = safe_calloc(max_procs, sizeof(pid_t));
  return pool;
}

static int reap_one(process_pool* pool) {
  int status;
  pid_t pid = waitpid(pool->pids[0], &status, 0);

  if (pid == -1) {
    perror("phxargs: waitpid");
    exit(EXIT_FAILURE);
  }

  pool->pids[0] = pool->pids[pool->count - 1];
  --pool->count;

  if (WIFEXITED(status)) {
    return WEXITSTATUS(status);
  }
  if (WIFSIGNALED(status)) {
    return WTERMSIG(status);
  }

  return EXIT_FAILURE;
}

void process_pool_wait_if_full(process_pool* pool) {
  while (pool->count >= pool->max_procs) {
    pool->status |= reap_one(pool);
  }
}

void process_pool_submit(process_pool* pool, pid_t pid) {
  pool->pids[pool->count++] = pid;
}

int process_pool_drain(process_pool* pool) {
  while (pool->count > 0) {
    pool->status |= reap_one(pool);
  }

  return pool->status;
}

void process_pool_destroy(process_pool* pool) {
  free(pool->pids);
  free(pool);
}

#ifndef PHXARGS_PROCESS_POOL_H
#define PHXARGS_PROCESS_POOL_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct process_pool_s process_pool;

void process_pool_install_signal_handlers(void);

process_pool* process_pool_create(size_t max_procs);

bool process_pool_halted(const process_pool* pool);

void process_pool_wait_if_full(process_pool* pool);

void process_pool_submit(process_pool* pool, pid_t pid);

int process_pool_drain(process_pool* pool);

void process_pool_destroy(process_pool* pool);

#endif // PHXARGS_PROCESS_POOL_H

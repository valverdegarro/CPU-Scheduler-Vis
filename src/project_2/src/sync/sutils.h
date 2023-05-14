#ifndef SUTILS_H
#define SUTILS_H

#include <semaphore.h>
#include "types.h"

int timed_sem_wait(sem_t *sem, int *time_sum);

char *proc_type_to_string(proc_t ptype);

char *op_code_to_str(msg_code code);

void print_timeval(struct timeval tv, char *str, int size);

#endif // SUTILS_H
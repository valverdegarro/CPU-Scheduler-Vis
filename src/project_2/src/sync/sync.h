#ifndef SYNC_H
#define SYNC_H

#include "types.h"

#include <stdbool.h>

/* Basic read functions */

int read_producer_count(int *val);

int read_consumer_count(int *val);

int get_halted(bool *value);

int get_sem_wait_time();

/* Memory allocation functions */

int init_shr_mem(char *name, int buff_size);

int free_shr_mem();

int connect_shr_mem(char *name, proc_t proc_type);

int disconnect_shr_mem(proc_t proc_type);

/* Buffer functions */

// Writes msg into buffer, fires an event with proc_type, and returns the
// index that was written to via modified_idx.
int write_buff_msg(proc_t proc_type, buff_msg *msg, int *modified_idx);

// Reads a message from the buffer into msg, fires an event, and returns the
// index that was read from via modified_idx.
int read_buff_msg(buff_msg *msg, int *modified_idx);

// NOTE: using these in the creator might cause deadlocks, only uncomment if
// absolutely neccesary

// int get_buff_entry(int *value);

// Print function to be used when a message is sent
void print_msg(buff_msg *msg, int index, bool is_sender);
char *proc_type_to_string(proc_t ptype);

/* Event system functions */

int wait_event(event_t *event, buff_msg *buffer, int buff_size);
int event_continue();

/* Finalization functions */

int finalize();

#endif // SYNC_H

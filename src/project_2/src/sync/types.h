/*
 * types.h
 *
 * Defines types that are shared among all four categories
 * of processes in the system. Anything defined here will
 * be visible to everyone.
 *
 */
#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <sys/time.h>
#include <unistd.h>

#define OK 0
#define FAIL 1

typedef enum proc_t { CREATOR, PRODUCER, CONSUMER, FINALIZER } proc_t;

typedef enum msg_code { NORMAL, HALT } msg_code;

typedef struct event_t {
  proc_t proc_type;
  int pid;
  bool last;

  int prod_count;
  int cons_count;

  bool is_halted;

  int modified_idx;
  int buff_entry;
  int buff_exit;
  int buff_count;

} event_t;

typedef struct buff_msg {
  bool valid;
  pid_t pid;
  proc_t proc_type;
  struct timeval ts; // Timestamp
  int key;           // Random key between 0-99
  msg_code op_code;

} buff_msg;

// Metadata neccesary for circular buffer operations.
typedef struct cb_metadata {
  int head;
  int tail;
  int size;
  int count;

} cb_metadata;

// Non-mmapable struct which abstracts a circular buffer.
typedef struct c_buffer {
  buff_msg *buffer;
  cb_metadata *cbd;

} c_buffer;

#endif // TYPES_H
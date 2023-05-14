#include "types.h"
#include "events.h"
#include "sutils.h"
#include "sync.h"

#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

#define SEM_E_RECV_NAME "SEM_E_RECV"
#define SEM_E_SEND_NAME "SEM_E_SEND"

sem_t *sem_recv;
sem_t *sem_send;
sem_t *sem_read;

// Pointers to shared memory
event_t *local_shared_event = NULL;
buff_msg *local_buffer = NULL;

// Time spent waiting on event semaphores (microseconds)
int sem_e_wait_usec = 0;

int get_esem_wait_time() { return sem_e_wait_usec; }

int init_events(event_t *event_mem, buff_msg *buffer) {
  sem_unlink(SEM_E_RECV_NAME);
  sem_unlink(SEM_E_SEND_NAME);
  sem_unlink(SEM_E_SEND_NAME);

  sem_recv = sem_open(SEM_E_RECV_NAME, O_CREAT, 0644, 0);
  sem_send = sem_open(SEM_E_SEND_NAME, O_CREAT, 0644, 1);
  sem_read = sem_open(SEM_E_SEND_NAME, O_CREAT, 0644, 0);
  local_shared_event = event_mem;
  local_buffer = buffer;

  return OK;
}

int attach_events(event_t *event_mem, buff_msg *buffer) {
  sem_recv = sem_open(SEM_E_RECV_NAME, 0);
  sem_send = sem_open(SEM_E_SEND_NAME, 0);
  sem_read = sem_open(SEM_E_SEND_NAME, 0);
  local_shared_event = event_mem;
  local_buffer = buffer;

  return OK;
}

int send_event(event_t *event) {
  int status;
  status = timed_sem_wait(sem_send, &sem_e_wait_usec);
  if (status != 0) {
    printf("[send_event] failed waiting on sem_send \n");
    return FAIL;
  }

  // put the event in the memory
  *local_shared_event = *event;

  // Post the event on the receiver
  status = sem_post(sem_recv);
  if (status != 0) {
    printf("[send_event] failed posting sem_recv \n");
    return FAIL;
  }

  // Wait until the receiver processes the event
  status = timed_sem_wait(sem_read, &sem_e_wait_usec);
  if (status != 0) {
    printf("[send_event] failed waiting sem_read \n");
    return FAIL;
  }

  // Release the sem_send so other process can send an event
  status = sem_post(sem_send);
  if (status != 0) {
    printf("[send_event] failed posting on sem_send \n");
    return FAIL;
  }

  return OK;
}

// return value depends on the operaion
int wait_event(event_t *event, buff_msg *buffer, int buff_size) {
  int status;

  // wait for an event
  status = timed_sem_wait(sem_recv, &sem_e_wait_usec);
  if (status != 0) {
    printf("[wait_event] failed waiting sem_recv \n");
    return FAIL;
  }

  // read event and current buffer from memory
  *event = *local_shared_event;
  memcpy(buffer, local_buffer, sizeof(buff_msg) * buff_size);

  return OK;
}

int event_continue() {
    int status;

    // notify the sender that event was already read
    status = sem_post(sem_read);
    if (status != 0) {
        printf("[wait_event] failed posting sem_read \n");
        return FAIL;
    }
    return OK;
}

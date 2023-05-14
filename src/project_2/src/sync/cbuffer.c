#include "cbuffer.h"
#include "sutils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MSG_LINE_SIZE 100
#define TIMESTAMP_STR_SIZE 20

c_buffer *init_cbuffer(cb_metadata *cbd, buff_msg *buffer, int size) {
  c_buffer *cb = (c_buffer *)malloc(sizeof(c_buffer));

  cb->cbd = cbd;
  cb->buffer = buffer;

  cb->cbd->size = size;
  cb->cbd->head = 0;
  cb->cbd->tail = 0;
  cb->cbd->count = 0;

  for (int i = 0; i < size; i++) {
    cb->buffer[i].valid = false;
  }

  return cb;
}

c_buffer *construct_cbuffer(cb_metadata *cbd, buff_msg *buffer) {
  c_buffer *cb = (c_buffer *)malloc(sizeof(c_buffer));

  cb->cbd = cbd;
  cb->buffer = buffer;

  return cb;
}

int get_head(c_buffer *cb) { return cb->cbd->head; }

int get_tail(c_buffer *cb) { return cb->cbd->tail; }

int get_count(c_buffer *cb) { return cb->cbd->count; }

bool is_buff_empty(c_buffer *cb) { return cb->cbd->count == 0; }

bool is_buff_full(c_buffer *cb) { return cb->cbd->count == cb->cbd->size; }

int get_msg(c_buffer *cb, buff_msg *msg) {
  if (is_buff_empty(cb)) {
    printf("Read error: buffer is empty!\n");
    return FAIL;
  }

  cb->buffer[cb->cbd->head].valid = false;
  *msg = cb->buffer[cb->cbd->head];

  cb->cbd->head = (cb->cbd->head + 1) % cb->cbd->size;
  cb->cbd->count--;

  return OK;
}

int put_msg(c_buffer *cb, buff_msg *msg) {
  if (is_buff_full(cb)) {
    printf("Write error: buffer is full!\n");
    return FAIL;
  }

  cb->buffer[cb->cbd->tail] = *msg;
  cb->buffer[cb->cbd->tail].valid = true;

  cb->cbd->tail = (cb->cbd->tail + 1) % cb->cbd->size;
  cb->cbd->count++;

  return OK;
}

int get_buff_bytes(int buff_size) { return (buff_size) * sizeof(buff_msg); }

void print_buff_msg(buff_msg *msg, char *line, int size, int index, char *tag) {
  char msg_line[MSG_LINE_SIZE];
  char *ptype_str;
	char *op_code_str;

	ptype_str = proc_type_to_string(msg->proc_type);
	op_code_str = op_code_to_str(msg->op_code);


  snprintf(
    msg_line, MSG_LINE_SIZE,
    "[%d]\t|\t%s (PID: %d)\t%s \n",
    index, ptype_str, msg->pid, tag
  );
  strcat(line, msg_line);


  if (msg->proc_type == PRODUCER) {
    snprintf(
      msg_line, MSG_LINE_SIZE,
      "\t|\tKey: %d \n",
      msg->key
    );
    strcat(line, msg_line);

  } else {
    snprintf(
      msg_line, MSG_LINE_SIZE,
      "\t|\t%s\n",
      op_code_str
    );
    strcat(line, msg_line);
  }


  char timestamp[TIMESTAMP_STR_SIZE];
  print_timeval(msg->ts, timestamp, TIMESTAMP_STR_SIZE);

  snprintf(
    msg_line, MSG_LINE_SIZE,
    "\t|\t%s\n",
    timestamp
  );
  strcat(line, msg_line);

  strcat(line, "\t---------------------------------------------\n\n");
}

#ifndef CBUFFER_H
#define CBUFFER_H

#include "types.h"

#include <stdbool.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>


// Returns a pointer to a new cbuffer with default values (Creator use only)
c_buffer *init_cbuffer(cb_metadata *cbd, buff_msg *buffer, int size);

// Returns a cbuffer with the pre-initialized parameters
c_buffer *construct_cbuffer(cb_metadata *cbd, buff_msg *buffer);

int get_head(c_buffer *cb);
int get_tail(c_buffer *cb);

int get_count(c_buffer *cb);

bool is_buff_empty(c_buffer *cb);
bool is_buff_full(c_buffer *cb);

int get_msg(c_buffer *cb, buff_msg *msg);
int put_msg(c_buffer *cb, buff_msg *msg);

int get_buff_bytes(int buff_size);

void print_buff_msg(buff_msg *msg, char *line, int size, int index, char *tag);

#endif // CBUFFER_H
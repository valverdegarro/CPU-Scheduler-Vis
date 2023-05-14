#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <semaphore.h>

#include "../sync/cbuffer.h"
#include "../sync/sync.h"
#include "../sync/types.h"
#include "parameter_parser.h"
#include "ui.h"

char *buffer_name;
int number_of_messages;
bool executing = true;

int main(int argc, char *argv[]) {
  read_creator_parameters(argc, argv, &buffer_name, &number_of_messages);
  return gtk_initialization(argc, argv);
}

void windows_start_show() {
  int status = OK;
  event_t event;
  buff_msg *buffer;

  buffer_info_t buff_info;
  buff_info.buffer_name = malloc(strlen(buffer_name));
  memcpy(buff_info.buffer_name, buffer_name, strlen(buffer_name));
  buff_info.init_end = 0;
  buff_info.init_start = 0;
  buff_info.size = number_of_messages;

  g_idle_add((GSourceFunc)gtk_set_buffer_info, &buff_info);
  g_idle_add((GSourceFunc)gtk_set_executing, &executing);

  status = init_shr_mem(buffer_name, number_of_messages);
  if (status != OK) {
    printf("FAILED %d\n", status);
  }

  do {
    int con_ctr = 0;
    status = read_consumer_count(&con_ctr);
    if (status != OK) {
      printf("Failed reading counter\n");
      break;
    }

    int prod_ctr = 0;
    status = read_producer_count(&prod_ctr);
    if (status != OK) {
      printf("Failed reading counter\n");
      break;
    }

    buffer = malloc(number_of_messages * sizeof(buff_msg));

    status = wait_event(&event, buffer, number_of_messages);
    if (status != OK) {
      printf("Failed waiting for event\n");
      break;
    }

    if (!event.last) {
      event_t *current_event = malloc(
          sizeof(event_t)); // This will be free'd inside update_ui_labels
      memcpy(current_event, &event, sizeof(event_t));
      g_idle_add((GSourceFunc)gtk_update_ui_labels, current_event);

      cb_metadata *cbd = malloc(sizeof(cb_metadata));

      cbd->head = event.buff_exit;
      cbd->tail = event.buff_entry;
      cbd->count = event.buff_count;
      cbd->size = number_of_messages;

      c_buffer *cbuffer = construct_cbuffer(cbd, buffer);

      g_idle_add((GSourceFunc)gtk_set_buffer_view, cbuffer);

      current_event =
          malloc(sizeof(event_t)); // This will be free'd inside set_log_message
      memcpy(current_event, &event, sizeof(event_t));
      g_idle_add((GSourceFunc)gtk_set_log_message, current_event);
    }

    status = event_continue();
    if (status != OK) {
      printf("Failed contininuing event\n");
      break;
    }

  } while (!event.last);

  executing = false;
  gtk_set_executing(&executing);

  event_t *current_event =
      malloc(sizeof(event_t)); // This will be free'd inside update_ui_labels
  memcpy(current_event, &event, sizeof(event_t));
  g_idle_add((GSourceFunc)gtk_update_ui_labels, current_event);
}

void exit_app() { gtk_main_quit(); }
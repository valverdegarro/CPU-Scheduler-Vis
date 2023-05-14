#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>
#include <stdbool.h>

#include "../sync/types.h"

typedef struct buffer_info_t {
    
    char *buffer_name;
    int size;
    int init_start;
    int init_end;

} buffer_info_t;

int gtk_initialization(int argc, char **argv);

//void gtk_set_buffer_info(char *buffer_name, int size, int init_start, int init_end);
void gtk_set_buffer_info(gpointer data);

void gtk_set_executing(bool *data);

void gtk_update_ui_labels(gpointer data);
void gtk_set_buffer_view(gpointer data);
void gtk_set_log_message(gpointer data);

#endif // UI_H
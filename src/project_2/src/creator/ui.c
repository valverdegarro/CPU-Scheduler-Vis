#include "ui.h"

#include "../sync/types.h"
#include "../sync/sync.h"
#include "../sync/cbuffer.h"
#include "../sync/sutils.h"
#include "../utils/utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024

GtkBuilder *builder;
GtkWidget *window;
GtkPaned *panel;
GtkLabel *buffer_name_label;
GtkLabel *buffer_size_label;
GtkLabel *buffer_start_label;
GtkLabel *buffer_end_label;
GtkLabel *consumer_qty_label;
GtkLabel *producer_qty_label;
GtkLabel *on_execution_qty_label;
GtkGrid *app_var_display_grid;

// Buffer window widgets
GtkScrolledWindow *buffer_scrolled_window;
GtkTextView *buffer_text_view;
GtkTextBuffer *buffer_obj;


// Log window widgets
GtkScrolledWindow *log_scrolled_window;
GtkTextView *log_text_view;
GtkTextBuffer *log_obj;

void init_glade_structures(GtkBuilder*);
void windows_start_show();

int gtk_initialization(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/creator/ui.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "app_window"));
    if(NULL == window)
    {
        fprintf(stderr, "Unable to file object with id \"app_window\" \n");
        return 0;
    }

    init_glade_structures(builder);

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show_all(window);

    g_thread_new(NULL, (GThreadFunc)windows_start_show, NULL);

    gtk_main(); // This blocks forever

    return 0;
}

void gtk_set_buffer_pos(int start, int end)
{
    char int_as_char[10];

    sprintf(int_as_char, "%d", start);
    gtk_label_set_text(buffer_start_label, int_as_char);

    sprintf(int_as_char, "%d", end);
    gtk_label_set_text(buffer_end_label, int_as_char);
}

void gtk_set_buffer_info(gpointer data)
{
    buffer_info_t *buff_info = data;

    char int_as_char[10];
    gtk_label_set_text(buffer_name_label, buff_info->buffer_name);

    sprintf(int_as_char, "%d", buff_info->size);
    gtk_label_set_text(buffer_size_label, int_as_char);

    gtk_set_buffer_pos(buff_info->init_start, buff_info->init_end);
}

void gtk_set_identity_qty(int consumers, int producers)
{
    char int_as_char[10];
    sprintf(int_as_char, "%d", consumers);
    gtk_label_set_text(consumer_qty_label, int_as_char);

    sprintf(int_as_char, "%d", producers);
    gtk_label_set_text(producer_qty_label, int_as_char);
}

void gtk_set_executing(bool *data)
{
    bool execute = *data;
    char *executing = execute ? "Si" : "No";
    gtk_label_set_text(on_execution_qty_label, executing);
}

void init_glade_structures(GtkBuilder* builder)
{
    panel = GTK_PANED(gtk_builder_get_object(builder, "app_panel"));
    app_var_display_grid = GTK_GRID(gtk_builder_get_object(builder, "app_var_display"));

    buffer_name_label = GTK_LABEL(gtk_builder_get_object(builder, "buffer_name_label"));
    buffer_size_label = GTK_LABEL(gtk_builder_get_object(builder, "buffer_size_label"));
    buffer_start_label = GTK_LABEL(gtk_builder_get_object(builder, "buffer_start_label"));
    buffer_end_label = GTK_LABEL(gtk_builder_get_object(builder, "buffer_end_label"));
    consumer_qty_label = GTK_LABEL(gtk_builder_get_object(builder, "consumer_qty_label"));
    producer_qty_label = GTK_LABEL(gtk_builder_get_object(builder, "producer_qty_label"));
    on_execution_qty_label = GTK_LABEL(gtk_builder_get_object(builder, "on_execution_qty_label"));

    GTK_PANED(gtk_builder_get_object(builder, "second_paned")); // Previously saved in global variable 'panel'

    buffer_scrolled_window = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "buffer_scrolled_window"));
    buffer_text_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "buffer_text_view"));
    gtk_text_view_set_editable(buffer_text_view, FALSE);
    buffer_obj = gtk_text_view_get_buffer(buffer_text_view);


    log_scrolled_window = GTK_SCROLLED_WINDOW(gtk_builder_get_object(builder, "log_scrolled_window"));
    log_text_view = GTK_TEXT_VIEW(gtk_builder_get_object(builder, "log_text_view"));
    gtk_text_view_set_editable(log_text_view, FALSE);
    log_obj = gtk_text_view_get_buffer(log_text_view);
}


void gtk_update_ui_labels(gpointer data) {
    event_t *event = data;

    gtk_set_identity_qty(event->cons_count, event->prod_count);
	gtk_set_buffer_pos(event->buff_entry, event->buff_exit);

    free(data);
}


void gtk_set_buffer_view(gpointer data)
{
    c_buffer *cbuffer = data;
    char line[BUFFER_SIZE];

    // Delete current textbuffer contents
    GtkTextIter iter_start;
    gtk_text_buffer_get_iter_at_line(buffer_obj, &iter_start, 0);

    GtkTextIter iter_end;
    gtk_text_buffer_get_end_iter(buffer_obj, &iter_end);

    gtk_text_buffer_delete(buffer_obj, &iter_start, &iter_end); // Deletes the entire textbuffer content

    
    // Iteratively print circular buffer to a string and show each message in the textbuffer
    buff_msg *msg;
    char msg_tag[15]; // To indicate if head or tail point here

    for (int i = 0; i < cbuffer->cbd->size; i++) {
        
        memset(line, 0, BUFFER_SIZE);
        memset(msg_tag, 0, 15);

        msg = &cbuffer->buffer[i];

        if (i == get_head(cbuffer)) {
            strcat(msg_tag, "[EXIT] ");
        }

        if (i == get_tail(cbuffer)) {
            strcat(msg_tag, "[ENTRY]");
        }

        if (msg->valid) {
            print_buff_msg(msg, line, BUFFER_SIZE, i, msg_tag);
        
        } else{
            snprintf(
                line, BUFFER_SIZE, 
                "[%d]\t|\tEMPTY\t\t\t\t\t\t%s\n\t---------------------------------------------\n\n", i, msg_tag
                );
        }
        
        gtk_text_buffer_insert_at_cursor(buffer_obj, line, -1);
    }
    

    // Free malloc'd structs to avoid memory leaks
    free(cbuffer->cbd);
    free(cbuffer->buffer);
    free(data);
}


void gtk_set_log_message(gpointer data)
{
    event_t *event = data;
    char buffer[BUFFER_SIZE];
    char *action = "writes";
    char *process = proc_type_to_string(event->proc_type);
    char *time =  get_current_timestamp();


    if (event->proc_type == CONSUMER) {
        action = "reads";
    }

    snprintf(buffer, BUFFER_SIZE, "[%s] %s (pid=%d) %s at buffer index: %d \n\n", time, process, event->pid, action, event->modified_idx);

    // Insert event printout at the start of the textbuffer
    GtkTextIter iter_start;
    gtk_text_buffer_get_iter_at_line(log_obj, &iter_start, 0);
    gtk_text_buffer_place_cursor(log_obj, &iter_start);
    gtk_text_buffer_insert_at_cursor(log_obj, buffer, -1);

    free(data);
}

#include "ui.h"
#include "console.h"
#include "types.h"
#include "ui_grid.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_THREADS 40
#define MAX_GRID_ITEM_INDEX_LENGTH 6

GtkWidget *panel;
GtkWidget* controls_fixed;
GtkToggleButton* expropiative_mode_on_button;
GtkToggleButton* expropiative_mode_off_button;
GtkLabel* operation_mode_label;
GtkSpinButton* thread_quantity_button;
GtkAdjustment* thread_quantity_adjustment;
GtkSpinButton* quantum_button;
GtkAdjustment* quantum_adjustment;

// Control variables
enum expropiative_mode operation_mode = OFF;
int thread_quantity = MAX_THREADS;
int quantum = 10;

void init_glade_structures(GtkBuilder* builder)
{
    panel = GTK_WIDGET(gtk_builder_get_object(builder, "main_panel"));
    controls_fixed = GTK_WIDGET(gtk_builder_get_object(builder, "controls_fixed"));

    operation_mode_label = GTK_LABEL(gtk_builder_get_object(builder, "operation_mode"));
    expropiative_mode_on_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "expropiative_mode_on_button"));
    expropiative_mode_off_button = GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder, "expropiative_mode_off_button"));

    thread_quantity_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "thread_quantity_button"));
    thread_quantity_adjustment = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "thread_quantity_adjustment"));

    quantum_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, "quantum_button"));
    quantum_adjustment = GTK_ADJUSTMENT(gtk_builder_get_object(builder, "quantum_adjustment"));

    char label[MAX_GRID_ITEM_INDEX_LENGTH];
    init_grid_items(builder);
}

void on_thread_quantity_adjustment_changed(GtkAdjustment* adjustment)
{
}

void on_quantum_adjustment_changed(GtkAdjustment* adjustment)
{
}

void on_thread_quantity_value_changed(GtkSpinButton* button)
{
    thread_quantity = gtk_spin_button_get_value_as_int(button);
    enable_grid_items(thread_quantity);
    printf("thread_quantity changed to=[%d]\n", thread_quantity);
}

void on_quantum_value_changed(GtkSpinButton* button)
{
    quantum = gtk_spin_button_get_value_as_int(button);
    printf("quantum changed to=[%d]\n", quantum);
}

void on_expropiative_mode_off_button_toggled(GtkToggleButton* button)
{
    gboolean result = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
    if (result) {
        operation_mode = OFF;
        console_print("operation_mode OFF");
    } else {
        operation_mode = ON;
        console_print("operation_mode ON");
    }
}

void on_expropiative_mode_on_button_toggled(GtkToggleButton* button)
{
    gboolean result = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button));
    if (result) {
        operation_mode = ON;
        console_print("operation_mode ON");
    } else {
        operation_mode = OFF;
        console_print("operation_mode OFF");
    }
}

void start_set_sensitive()
{
    gtk_widget_set_sensitive(controls_fixed, false);
    set_grid_item_controls_sensitive(false);   
}

void stop_set_sensitive()
{
    gtk_widget_set_sensitive(controls_fixed, true);
    set_grid_item_controls_sensitive(true);
}

int get_quantum_value()
{
    return quantum;
}

int get_number_of_threads_value()
{
    return thread_quantity;
}

void get_thread_config(int id, thread_config* configs)
{
    get_grid_item_config(id, configs);
}

expropiative_mode get_operation_mode()
{
    return operation_mode;
}

void set_pi_progress(int id, long double pi_val, float prog)
{
    set_grid_item_result(id, pi_val);
    set_grid_item_progress(id, prog);
}

void exit_app()
{
    gtk_main_quit();
}
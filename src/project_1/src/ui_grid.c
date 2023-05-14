#include "ui_grid.h"
#include "console.h"
#include "types.h"
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>

#define MAX_GRID_ITEM_INDEX_LENGTH 6

typedef enum {
    Label,
    Box,
    ProgressBar,
    TicketSpinButton,
    WorkloadSpinButton,
    TicketAdjustmentButton,
    WorkloadAdjustmentButton
} ItemType;

typedef struct grid_item {
    bool enabled;
    GtkLabel* label;
    GtkBox* box;
    GtkProgressBar* progress_bar;
    GtkSpinButton* ticket;
    GtkSpinButton* workload;
    GtkAdjustment* ticket_adjustment;
    GtkAdjustment* workload_adjustment;
} grid_item;

int total_enabled_items = (int)MAX_THREADS;
grid_item grid_items[MAX_THREADS];
GtkWidget* grid;

void print_init_message(size_t i, bool enabled)
{
    return;
    char* message;
    if (enabled) {
        asprintf(&message, "Grid: Enabling grid item=[%d]", i);
        console_print(message);
    } else {
        asprintf(&message, "Grid: Disabling grid item=[%d]", i);
        console_print(message);
    }
    free(message);
}

void get_prefix(ItemType type, char output[MAX_GRID_ITEM_INDEX_LENGTH])
{
    char prefix[MAX_GRID_ITEM_INDEX_LENGTH];
    switch (type) {
    case Label:
        strcpy(prefix, "rst_");
        break;
    case Box:
        strcpy(prefix, "box_");
        break;
    case ProgressBar:
        strcpy(prefix, "pgr_");
        break;
    case TicketSpinButton:
        strcpy(prefix, "tck_");
        break;
    case WorkloadSpinButton:
        strcpy(prefix, "wld_");
        break;
    case TicketAdjustmentButton:
        strcpy(prefix, "tca_");
        break;
    case WorkloadAdjustmentButton:
        strcpy(prefix, "wda_");
        break;
    default:
        break;
    }
    for (int i = 0; i < MAX_GRID_ITEM_INDEX_LENGTH; i++) {
        output[i] = prefix[i];
    }
}

void get_grid_item_name(size_t value, ItemType type, char output[MAX_GRID_ITEM_INDEX_LENGTH])
{
    const int max_value_length = 3;
    char value_as_char[max_value_length];
    snprintf(value_as_char, sizeof value_as_char, "%zu", value);
    char label[MAX_GRID_ITEM_INDEX_LENGTH];
    get_prefix(type, label);
    strcat(label, value_as_char);
    for (int i = 0; i < MAX_GRID_ITEM_INDEX_LENGTH; i++) {
        output[i] = label[i];
    }
}

bool get_grid_item_config(int index, thread_config* config)
{
    if (index < total_enabled_items) {
        if (!grid_items[index].enabled) {
            config->number_of_tickets = (int)DEFAULT_NUMBER_OF_TICKETS;
            config->workload = (int)DEFAULT_WORKLOAD;
            return true;
        }

        const int ticket = gtk_spin_button_get_value_as_int(grid_items[index].ticket);
        const int workload = gtk_spin_button_get_value_as_int(grid_items[index].workload);
        config->number_of_tickets = ticket;
        config->workload = workload;
        return true;
    }
    return false;
}

bool set_grid_item_result(int index, long double result)
{
    if (index < total_enabled_items && grid_items[index].enabled) {
        char* result_text;
        asprintf(&result_text, "Pi: %0.16Lf", result);
        gtk_label_set_text(grid_items[index].label, result_text);
        free(result_text);
        return true;
    } else {
        return false;
    }
}

bool set_grid_item_progress(int index, float progress)
{
    float fraction;
    if (progress <= 1.0 && progress >= 0.0) {
        fraction = progress;
    } else {
        fraction = progress / 100.0;
    }
    if (index < total_enabled_items && grid_items[index].enabled) {
        /*if (fraction < 1.0) {
            printf("Setting fraction: %f \n", fraction);
        }*/
        gtk_progress_bar_set_fraction(grid_items[index].progress_bar, fraction);
        return true;
    } else {
        return false;
    }
}

void init_grid_item(grid_item* item, GtkLabel* label, GtkBox* box,
    GtkProgressBar* pgr_bar, GtkSpinButton* tckt,
    GtkSpinButton* wload, GtkAdjustment* ticket_adjustment,
    GtkAdjustment* workload_adjustment)
{
    item->enabled = true;
    item->box = box;
    item->label = label;
    item->progress_bar = pgr_bar;
    item->ticket = tckt;
    item->workload = wload;
    item->ticket_adjustment = ticket_adjustment;
    item->workload_adjustment = workload_adjustment;
}

void init_grid_items(GtkBuilder* builder)
{
    char name[MAX_GRID_ITEM_INDEX_LENGTH];
    grid = GTK_WIDGET(gtk_builder_get_object(builder, "thread_grid"));
    for (size_t i = 0; i < (int)MAX_THREADS; i++) {
        get_grid_item_name(i, Box, name);
        GtkBox* gtk_box = GTK_BOX(gtk_builder_get_object(builder, name));
        get_grid_item_name(i, Label, name);
        GtkLabel* gtk_label = GTK_LABEL(gtk_builder_get_object(builder, name));
        get_grid_item_name(i, ProgressBar, name);
        GtkProgressBar* gtk_progress_bar = GTK_PROGRESS_BAR(gtk_builder_get_object(builder, name));
        get_grid_item_name(i, TicketSpinButton, name);
        GtkSpinButton* gtk_ticket_spin_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, name));
        get_grid_item_name(i, WorkloadSpinButton, name);
        GtkSpinButton* gtk_workload_spin_button = GTK_SPIN_BUTTON(gtk_builder_get_object(builder, name));
        get_grid_item_name(i, TicketAdjustmentButton, name);
        GtkAdjustment* ticket_adjustment = GTK_ADJUSTMENT(gtk_builder_get_object(builder, name));
        get_grid_item_name(i, WorkloadAdjustmentButton, name);
        GtkAdjustment* workload_adjustment = GTK_ADJUSTMENT(gtk_builder_get_object(builder, name));

        grid_item new_item;
        init_grid_item(&new_item, gtk_label, gtk_box, gtk_progress_bar,
            gtk_ticket_spin_button, gtk_workload_spin_button,
            ticket_adjustment, workload_adjustment);
        grid_items[i] = new_item;
        print_init_message(i, true);
    }
}

void enable_grid_items(int items_qty)
{
    total_enabled_items = items_qty;
    for (size_t i = 0; i < (int)MAX_THREADS; i++) {
        bool enable = i < items_qty;
        grid_items[i].enabled = enable;
        print_init_message(i, enable);
        if (!enable) {
            gtk_widget_hide(GTK_WIDGET(grid_items[i].box));
        } else {
            gtk_widget_show(GTK_WIDGET(grid_items[i].box));
        }
    }
}

void set_grid_item_controls_sensitive(bool value)
{
    for (size_t i = 0; i < (int)MAX_THREADS; i++) {
        gtk_widget_set_sensitive(GTK_WIDGET(grid_items[i].ticket), value);
        gtk_widget_set_sensitive(GTK_WIDGET(grid_items[i].workload), value);
    }
}

void ticket_adj_changed(GtkToggleButton* button)
{
    console_print("Ticket value has changed\n");
}

void workload_adj_changed(GtkToggleButton* button)
{
    console_print("Workload value has changed\n");
}
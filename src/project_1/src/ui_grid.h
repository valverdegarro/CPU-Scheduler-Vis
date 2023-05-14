#ifndef UI_GRID_H
#define UI_GRID_H

#include "types.h"
#include <gtk/gtk.h>
#include <stdbool.h>

#define DEFAULT_NUMBER_OF_TICKETS 2
#define DEFAULT_WORKLOAD 1

int get_total_enabled_grid_items();
void init_grid_items(GtkBuilder* builder);
void enable_grid_items(int items_qty);
void set_grid_item_controls_sensitive(bool value);
bool set_grid_item_result(int index, long double result);
bool set_grid_item_progress(int index, float progress);
bool get_grid_item_config(int index, thread_config* config);

// Only declaired methods
void ticket_adj_changed(GtkToggleButton* button);
void workload_adj_changed(GtkToggleButton* button);

#endif /* UI_GRID_H */
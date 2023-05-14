#include <gtk/gtk.h>
#include <stdbool.h>

#include "types.h"

void init_glade_structures(GtkBuilder* builder);

void start_set_sensitive();
void stop_set_sensitive();

int get_quantum_value();
int get_number_of_threads_value();
void get_thread_config(int id, thread_config* configs);
expropiative_mode get_operation_mode();
void set_pi_progress(int id, long double pi_val, float prog);
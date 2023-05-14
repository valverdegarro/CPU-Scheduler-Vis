#include <signal.h>
#include <stdlib.h>
#include <time.h>
#include <gtk/gtk.h>
#include <stdio.h>

#include "pi_approximator.h"
#include "scheduler.h"
#include "types.h"
#include "ui.h"
#include "s_control.h"

GtkBuilder *builder;
GtkWidget *window;
bool is_running = false;


int main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "src/ui.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "app_window"));
    init_glade_structures(builder);

    gtk_builder_connect_signals(builder, NULL);
    g_object_unref(builder);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

void on_start_button_clicked(GtkButton *button)
{
    if (!is_running)
    {
        // Important: do not move this code!!
        // =================================
        start_set_sensitive();
        is_running = true;
        // =================================

        // Reading all configuration made by the user
        const int quantum = get_quantum_value();
        const int n_threads = get_number_of_threads_value();
        const expropiative_mode exp_mode = get_operation_mode();
        thread_config thread_configs[n_threads];
        for (size_t i = 0; i < n_threads; i++)
        {
            get_thread_config(i, &thread_configs[i]);
        }

        // Init and configure scheduler
        set_scheduler_op(quantum, exp_mode);
        create_pi_threads(n_threads, thread_configs);

        // Run non-blocking scheduler
        run_scheduler();
    }
}

void on_stop_button_clicked(GtkButton *button)
{
    // Important: do not move this code!!
    // =================================
    stop_set_sensitive();
    is_running = false;
    // =================================
}

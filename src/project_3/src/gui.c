#include "gui.h"
#include "common.h"

#include <gtk/gtk.h>


// Global vars
GtkWidget *in_numberic;
GtkApplication *app;


int grab_int_value (GtkSpinButton *button)
{
  return gtk_spin_button_get_value_as_int(button);
}

gui_config *prepare_config() {
    gui_config *config = (gui_config*) malloc(sizeof(gui_config));
    config->task_config = (task_config_t*) malloc(sizeof(task_config_t) * );

    return config;
}

void gen_pdf() {
    // prepare config


}


void open_second_window(GtkWidget *widget, gpointer   user_data) {
    GtkWidget *previous_window  = (GtkWidget*)user_data;
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *num_input;
    gchar *text;
    GtkWidget *button_rm;
    GtkWidget *button_edf;
    GtkWidget *button_llf;
    GtkWidget *button_ss;
    GtkWidget *button_ms ;
    GtkWidget *button_gen_pdf;
    GtkAdjustment *adjustment;
    int number_of_tasks = grab_int_value(GTK_SPIN_BUTTON(in_numberic));

    // Build the new window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Proyecto 3");
    gtk_window_set_default_size(GTK_WINDOW(window), 30, 10);

    // Build the grid that contains the rest of components
    grid = gtk_grid_new ();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 2);
    gtk_grid_set_row_spacing(GTK_GRID (grid), 2);
    gtk_container_add(GTK_CONTAINER (window), grid);

    /*
     * The header
    */
    label = gtk_label_new("ID de la tarea");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 10, 1);

    label = gtk_label_new("Periodo");
    gtk_grid_attach(GTK_GRID(grid), label, 10, 0, 10, 1);

    label = gtk_label_new("Tiempo de ejecucion");
    gtk_grid_attach(GTK_GRID(grid), label, 20, 0, 10, 1);


    /*
     * The data for each task
    */
    for (int task_id = 0; task_id < number_of_tasks; task_id++) {
        // The column with the label of the task id
        text = g_strdup_printf("%d", task_id);
        label = gtk_label_new(text);
        gtk_grid_attach(GTK_GRID(grid), label, 0, task_id + 1, 10, 1);

        // Execution time
        adjustment = gtk_adjustment_new(1.0, 1.0, 10.0, 1.0, 5.0, 0.0);
        num_input = gtk_spin_button_new(adjustment, 1.0, 0);
        gtk_grid_attach(GTK_GRID (grid), num_input, 10, task_id + 1, 10, 1);

        // Period
        adjustment = gtk_adjustment_new(1.0, 1.0, 10.0, 1.0, 5.0, 0.0);
        num_input = gtk_spin_button_new(adjustment, 1.0, 0);
        gtk_grid_attach(GTK_GRID(grid), num_input, 20, task_id + 1, 10, 1);
    }

    /*
     * Buttons at the bottom
    */

    // Algorithms buttons
    button_rm = gtk_check_button_new_with_label ("RM");
    //g_signal_connect (button_rm, "toggled", G_CALLBACK (cb_toggle_bit), rm);
    gtk_grid_attach(GTK_GRID (grid), button_rm , 0, number_of_tasks+1, 10, 1);

    button_edf = gtk_check_button_new_with_label ("EDF");
    //g_signal_connect (button_edf, "toggled", G_CALLBACK (cb_toggle_bit), edf);
    gtk_grid_attach(GTK_GRID(grid), button_edf, 10, number_of_tasks+1, 10, 1);

    button_llf = gtk_check_button_new_with_label ("LLF");
    //g_signal_connect (button_llf, "toggled", G_CALLBACK (cb_toggle_bit), llf);
    gtk_grid_attach(GTK_GRID (grid), button_llf, 20, number_of_tasks+1, 10, 1);


    // radio buttons for slides
    button_ms = gtk_radio_button_new_with_label_from_widget (NULL, "Multiples Slides");
    gtk_grid_attach (GTK_GRID (grid), button_ms, 0, number_of_tasks + 2, 10, 1);
    button_ss = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(button_ms), "Unico Slide");
    gtk_grid_attach (GTK_GRID (grid), button_ss, 15, number_of_tasks + 2, 15, 1);
    //g_signal_connect (button_ms, "clicked", G_CALLBACK (cb_toggle_bit), multiple_slides);

    // gen pdf button
    button_gen_pdf = gtk_button_new_with_label ("Generar .pdf");
    //g_signal_connect (button_gen_pdf, "clicked", G_CALLBACK (cb_generate_pdf), config);
    gtk_grid_attach (GTK_GRID (grid), button_gen_pdf, 0, number_of_tasks + 3, 15, 1);

    // Show new window
    gtk_widget_show_all(window);

    // Hide previous window
    gtk_application_add_window(app, GTK_WINDOW(window));
    gtk_application_remove_window(app, GTK_WINDOW(previous_window));
    gtk_widget_hide(previous_window);
}

void open_first_window() {
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *label;
    GtkWidget *button;
    GtkAdjustment *adjustment; // for the spin button

    // Build the window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Proyecto 3");
    gtk_window_set_default_size(GTK_WINDOW(window), 20, 30);

    // Build the grid
    grid = gtk_grid_new ();
    gtk_grid_set_column_homogeneous(GTK_GRID (grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID (grid), 5);
    gtk_grid_set_row_homogeneous(GTK_GRID (grid), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID (grid), 1);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // The label asking for the number of tasks
    label = gtk_label_new("Seleccione la cantidad de tareas");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 9, 6, 1);

    // An input the number of tasks to create as a sping button
    adjustment = gtk_adjustment_new(1.0, 1.0, 10.0, 1.0, 5.0, 0.0);
    in_numberic = gtk_spin_button_new(adjustment, 1.0, 0);
    gtk_grid_attach(GTK_GRID (grid), in_numberic, 2, 11, 2, 1);

    // The continue button
    button = gtk_button_new_with_label ("Continuar");
    g_signal_connect(button, "clicked", G_CALLBACK (open_second_window), window);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 12, 1, 1);

    // Show elements of the window
    gtk_widget_show_all(window);
}

int init_gui(int argc, char **argv) {
    int status;
    
    app = gtk_application_new("gui.project3", G_APPLICATION_FLAGS_NONE);
    
    g_signal_connect(app, "activate", G_CALLBACK (open_first_window), NULL);
    status = g_application_run(G_APPLICATION (app), argc, argv);
    g_object_unref(app);
    return status;
}
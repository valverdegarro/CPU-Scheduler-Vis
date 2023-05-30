#include "gui.h"
#include "common.h"
#include "latex.h"

#include <gtk/gtk.h>

// Tweak the following if you need changes in the numberic inputs
#define MAX_PERIOD 1000
#define MAX_EXEC_TIME 1000

// Global vars
GtkWidget *in_numberic;
GtkApplication *app;

typedef struct {
    int num_tasks;
    GtkWidget *parent_window;
    GtkWidget **exec_time; // array with pointers to the execution time widget for each task
    GtkWidget **period; // array with pointers to the period time widget for each task
    GtkWidget *button_rm; //single element to the rm check component
    GtkWidget *button_edf; // single element to edf component
    GtkWidget *button_llf; // single element to llf component
    GtkWidget *button_ss; // single element to ss (single slide radio button) component
} widgets_t;


int grab_int_value (GtkSpinButton *button)
{
  return gtk_spin_button_get_value_as_int(button);
}

gui_config *prepare_config(widgets_t *w) {
    gui_config *config = (gui_config*) malloc(sizeof(gui_config));
    config->num_tasks = w->num_tasks;
    config->task_config = (task_config_t*) malloc(sizeof(task_config_t) * w->num_tasks);
    config->rm_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->button_rm));
    config->edf_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->button_edf));
    config->llf_enabled = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->button_llf));
    config->single_slide = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->button_ss));

    for (int i = 0; i < w->num_tasks; i++) {
        config->task_config[i].execution = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w->exec_time[i]));
        config->task_config[i].period = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(w->period[i]));
    }

    return config;
}

bool valid_config(gui_config *config, int *invalid_task) {
    for (int i = 0; i < config->num_tasks; i++) {
        if (config->task_config[i].period <  config->task_config[i].execution) {
            *invalid_task = i;
            return false;
        }
    }

    return true;
}

// INSERT YOUR CODE HERE: add the actual function that generates the pdf here
void execute(gui_config *config) {
    int status;

    printf("ss enabled %d\n", config->single_slide);
    printf("rm enabled %d\n", config->rm_enabled);
    printf("edf enabled %d\n", config->edf_enabled);
    printf("llf enabled %d\n\n", config->llf_enabled);

    printf("Values for each task\n");
    for (int i = 0; i < config->num_tasks; i++) {
        printf("Task %d -period: %d  -exec_time: %d \n", i, config->task_config[i].period, config->task_config[i].execution);
    }

    status = latex_execute(config);
    if (status != OK) {
        printf("ERROR GENERATING PDF FILE\n");
    }


    printf("-------------------------------------\n");
}

void show_popup_error(GtkWidget* pWindow, int invalid_task) {
    GtkWidget *popup_window;
    gchar *text;
    GtkWidget *grid;
    GtkWidget *label;

    popup_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW (popup_window), "ERROR");
    gtk_container_set_border_width (GTK_CONTAINER (popup_window), 10);
    gtk_window_set_resizable(GTK_WINDOW (popup_window), FALSE);
    gtk_widget_set_size_request(popup_window, 150, 150);
    gtk_window_set_transient_for(GTK_WINDOW (popup_window),GTK_WINDOW (pWindow));
    gtk_window_set_position(GTK_WINDOW (popup_window),GTK_WIN_POS_CENTER);

    // Build the grid
    grid = gtk_grid_new ();
    gtk_grid_set_column_homogeneous(GTK_GRID (grid), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID (grid), 5);
    gtk_grid_set_row_homogeneous(GTK_GRID (grid), TRUE);
    gtk_grid_set_row_spacing(GTK_GRID (grid), 1);
    gtk_container_add(GTK_CONTAINER(popup_window), grid);

    // The label indicating the task with the error
    text = g_strdup_printf("El periodo de la tarea %d es menor que su tiempo de ejecucion", invalid_task);
    label = gtk_label_new(text);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 9, 6, 1);


    gtk_widget_show_all(popup_window);
    gtk_widget_grab_focus (popup_window);
}

void gen_pdf(GtkWidget *widget, gpointer user_data) {
    int invalid_task;
    widgets_t *w = (widgets_t*)user_data;

    // prepare config
    gui_config *config = prepare_config(w);

    // check if config is valid
    bool is_valid = valid_config(config, &invalid_task);
    if (!is_valid) {
        // show pop window with error
        show_popup_error(w->parent_window, invalid_task);
        return;
    }

    // replace the code in the following function for the actual code
    execute(config);
}


void open_second_window(GtkWidget *widget, gpointer   user_data) {
    GtkWidget *previous_window  = (GtkWidget*)user_data;
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *label;
    gchar *text;
    GtkWidget *button_ms ;
    GtkWidget *button_gen_pdf;
    GtkAdjustment *adjustment;
    int number_of_tasks = grab_int_value(GTK_SPIN_BUTTON(in_numberic));

    widgets_t *w = (widgets_t*) malloc(sizeof(widgets_t));
    w->exec_time = (GtkWidget **) malloc(sizeof(GtkWidget*) * number_of_tasks);
    w->period = (GtkWidget **) malloc(sizeof(GtkWidget*) * number_of_tasks);
    w->num_tasks = number_of_tasks;

    // Build the new window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Proyecto 3");
    gtk_window_set_default_size(GTK_WINDOW(window), 30, 10);
    w->parent_window = window;

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

        // Period time
        adjustment = gtk_adjustment_new(1.0, 1.0, MAX_PERIOD, 1.0, 5.0, 0.0);
        w->period[task_id] = gtk_spin_button_new(adjustment, 1.0, 0);
        gtk_grid_attach(GTK_GRID (grid), w->period[task_id], 10, task_id + 1, 10, 1);

        // Execution time
        adjustment = gtk_adjustment_new(1.0, 1.0, MAX_EXEC_TIME, 1.0, 5.0, 0.0);
        w->exec_time[task_id] = gtk_spin_button_new(adjustment, 1.0, 0);
        gtk_grid_attach(GTK_GRID(grid), w->exec_time[task_id], 20, task_id + 1, 10, 1);
    }

    /*
     * Buttons at the bottom
    */

    // Algorithms buttons
    w->button_rm = gtk_check_button_new_with_label ("RM");
    gtk_grid_attach(GTK_GRID (grid), w->button_rm , 0, number_of_tasks+1, 10, 1);

    w->button_edf = gtk_check_button_new_with_label ("EDF");
    gtk_grid_attach(GTK_GRID(grid), w->button_edf, 10, number_of_tasks+1, 10, 1);

    w->button_llf = gtk_check_button_new_with_label ("LLF");
    gtk_grid_attach(GTK_GRID (grid), w->button_llf, 20, number_of_tasks+1, 10, 1);


    // radio buttons for slides
    button_ms = gtk_radio_button_new_with_label_from_widget (NULL, "Multiples Slides");
    gtk_grid_attach (GTK_GRID (grid), button_ms, 0, number_of_tasks + 2, 10, 1);
    w->button_ss = gtk_radio_button_new_with_label_from_widget (GTK_RADIO_BUTTON(button_ms), "Unico Slide");
    gtk_grid_attach (GTK_GRID (grid), w->button_ss, 15, number_of_tasks + 2, 15, 1);

    // gen pdf button
    button_gen_pdf = gtk_button_new_with_label ("Generar .pdf");
    g_signal_connect(button_gen_pdf, "clicked", G_CALLBACK (gen_pdf), w);
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
    adjustment = gtk_adjustment_new(1.0, 1.0, MAX_TASKS, 1.0, 5.0, 0.0);
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
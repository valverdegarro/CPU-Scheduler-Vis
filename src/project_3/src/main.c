#include <stdio.h>
#include "gui.h"

// Set the following define to 1 if you want to compile with the GUI, 0 otherwise
// non GUI mode is used for development (faster than using the GUI to execute desired code)
#define ENABLE_GUI 0

#if ENABLE_GUI==0

#include "common.h" // remove it
#include <stdlib.h> // remove it
#include "latex.h"  //remove it

#endif

int main(int argc, char **argv)
{
    #if ENABLE_GUI==0
    
    
    int num_tasks = 2;

    task_config_t *tasks = (task_config_t*) malloc(sizeof(task_config_t) * num_tasks);

    for (int i = 0; i < num_tasks; i++) {
        tasks[i].execution = i + 1;
        tasks[i].period = (i+1)*2;
    }

    gui_config config;
    config.single_slide = true;
    config.rm_enabled = true;
    config.edf_enabled = false;
    config.llf_enabled = true;
    config.num_tasks = num_tasks;
    config.task_config = tasks;

    return latex_execute(&config);

    #else
    return init_gui(argc, argv);
    #endif

}
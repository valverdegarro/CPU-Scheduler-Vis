#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gui.h"


// Set the following define to 1 if you want to compile with the GUI, 0 otherwise
// non GUI mode is used for development (faster than using the GUI to execute desired code)
#define ENABLE_GUI 0

#if ENABLE_GUI==0

#include "common.h" // remove it
#include <stdlib.h> // remove it
#include "latex.h"  //remove it
#include "ttables/ttables.h" // remove it

#endif

int main(int argc, char **argv)
{
    #if ENABLE_GUI==0
    
    
    int num_tasks = 3;

    task_config_t *tasks = (task_config_t*) malloc(sizeof(task_config_t) * num_tasks);

    // This page generates graphs for RM and EDF: http://ddmills.com/schedule-vis/

    // Data from https://www.geeksforgeeks.org/rate-monotonic-scheduling/
    // tasks[0].execution = 3;
    // tasks[0].period = 20;
    // tasks[1].execution = 2;
    // tasks[1].period = 5;
    // tasks[2].execution = 2;
    // tasks[2].period = 10;

    // RM data with deadline miss
    // tasks[0].execution = 3;
    // tasks[0].period = 4;
    // tasks[1].execution = 3;
    // tasks[1].period = 5;
    // tasks[2].execution = 2;
    // tasks[2].period = 8;

    // EDF data from https://microcontrollerslab.com/earliest-deadline-first-scheduling/
    //tasks[0].execution = 1;
    //tasks[0].period = 4;
    //tasks[1].execution = 2;
    //tasks[1].period = 6;
    //tasks[2].execution = 3;
    //tasks[2].period = 8;

    // LLF data from https://microcontrollerslab.com/least-laxity-first-llf/
    // NOTE: the lcm for this set is 120 so it will generate a lot of slides
    tasks[0].execution = 2;
    tasks[0].period = 6;
    tasks[1].execution = 2;
    tasks[1].period = 8;
    tasks[2].execution = 3;
    tasks[2].period = 10;


    // Test data for more than 100% CPU utilization
    //tasks[0].execution = 1;
    //tasks[0].period = 1;
    //tasks[1].execution = 2;
    //tasks[1].period = 50;
    //tasks[2].execution = 3;
    //tasks[2].period = 50;

    gui_config config;
    config.single_slide = false;
    config.rm_enabled = true;
    config.edf_enabled = true;
    config.llf_enabled = true;

    config.num_tasks = num_tasks;
    config.task_config = tasks;

    return latex_execute(&config);

    #else
    return init_gui(argc, argv);
    #endif

}
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

#endif

int main(int argc, char **argv)
{
    #if ENABLE_GUI==0
    
    
    int num_tasks = 3;

    task_config_t *tasks = (task_config_t*) malloc(sizeof(task_config_t) * num_tasks);

    // Data from https://www.geeksforgeeks.org/rate-monotonic-scheduling/
    tasks[0].execution = 3;
    tasks[0].period = 20;

    tasks[1].execution = 2;
    tasks[1].period = 5;

    tasks[2].execution = 2;
    tasks[2].period = 10;

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

    /*
    char buffer[4096];
    memset(buffer, 0, 4096);
    
    // Ugly code to initialize timeslots array manually
    int tasks[MCM] = {0,1,1,2,2,2,0,2,2,1,1,2,0,-1,-1,-1,-1,-1,0,1,1,2,2,2,0,2};
    //int tasks[MCM] = {0,1,2,3,4,5,6,7,8,9,1,2,0,4,9,6,3,-1,0,-1,-1,2,5,2,7,2}; // 10 tasks example

    timeslot_t *ts_array = malloc(MCM * sizeof(timeslot_t));

    for (int i = 0; i < MCM; i++) {
        ts_array[i].task_id = tasks[i];

        ts_array[i].deadlines = malloc(NTASKS * sizeof(bool));
        //memcpy(ts_array[i].deadlines, deadlines, NTASKS);

        for (int j = 0; j < NTASKS; j++) {
            ts_array[i].deadlines[j] = false;
        }
    }

    // Manually set some deadlines
    // Task 1
    ts_array[0].deadlines[0] = true;
    ts_array[6].deadlines[0] = true;
    ts_array[12].deadlines[0] = true;
    ts_array[18].deadlines[0] = true;
    ts_array[24].deadlines[0] = true;

    // Task 2
    ts_array[0].deadlines[1] = true;
    ts_array[9].deadlines[1] = true;
    ts_array[18].deadlines[1] = true;

    // Task 3
    ts_array[0].deadlines[2] = true;
    ts_array[18].deadlines[2] = true;
    

    write_ttable(buffer, ts_array, MCM, NTASKS);
    printf("%s\n", buffer);

    //return init_gui(argc, argv);
    */

}
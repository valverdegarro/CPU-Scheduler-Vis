#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

#define FAIL -1
#define OK 0

typedef struct {
    int execution;
    int period;
} task_config_t;

// This structs contains everything that should be used for later use in the actual gen pdf function
typedef struct gui_config {
    bool single_slide; // if true, generate everything in a single slide
    bool rm_enabled;
    bool edf_enabled;
    bool llf_enabled;
    int num_tasks;
    task_config_t *task_config;
} gui_config;


// This struct is shared between the simulator and time table generator to create the gantt charts
typedef struct timeslot_t {
    int task_id;
    bool *deadlines; // Array of size num_tasks, indicates task deadlines for this timeslot
    //bool missed;
} timeslot_t;

#endif // COMMON_H
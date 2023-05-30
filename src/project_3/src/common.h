#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

#define FAIL -1
#define OK 0

#define MAX_TASKS 10 // if this variable is changed, remember to update table_colors in latex.c
#define N_ALGORITHMS 3


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
} timeslot_t;

typedef struct sim_data_t {
    timeslot_t *ts;
    int ts_size;
    bool *misses;
    int miss_idx;
} sim_data_t;


// Contains all the parameters neccesary which are necessary to draw a time table and that can be different
// between executions.
// The idea with this is to create an array which contains 3 of these structs to generate the slides
typedef struct ttable_params {
    bool enabled;
    timeslot_t *ts;
    bool *misses;
    int miss_idx;
} ttable_params;

#endif // COMMON_H
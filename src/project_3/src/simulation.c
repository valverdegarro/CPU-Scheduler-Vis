#include "common.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    task_config_t data;
    int time_left;
    int task_id;
} runtime_t;

int lcm(int a, int b)
{
    int m = 1;

    while(m%a || m%b) m++;

    return m;
}

int get_lcm_from_array(int size, int *array) {
    int result = array[0];
    for (int i=0; i < size; i++) {
        result = lcm(result, array[i]);
    }
    
    return result;
}

int get_deadline(bool *deadlines, int size) {
    for (int i =0; i < size; i++) {
        if (deadlines[i]) {
            return i;
        }
    }
    return -1;
}

int period_compare(const void *a, const void *b) {
    runtime_t *t_a = (runtime_t *) a;
    runtime_t *t_b = (runtime_t *) b;
    return (t_a->data.period - t_b->data.period);
}

int check_highest_priority(int current_task, int num_tasks, runtime_t *runtime) {
    if (current_task == -1 ) {
        return -1;
    }
    int current_task_index = 0;
    for (int i=0; i < num_tasks; i++) {
        if (current_task == runtime[i].task_id) {
            current_task_index = i;
        }
    }


    for (int i =0; i <= current_task_index; i++) {
        if (runtime[i].time_left != 0) {
            return runtime[i].task_id;
        }
    }
    return -1;
}

sim_data_t *simulate_rm(gui_config *config) {
    int running = -1; // the task that is running
    sim_data_t *result  = (sim_data_t*)malloc(sizeof(sim_data_t));
    result->miss_idx = -1;
    result->misses = (bool*)malloc(sizeof(bool) * config->num_tasks);
    for (int i =0; i < config->num_tasks; i++) {
        result->misses[i] = false;
    }

    runtime_t *runtime = (runtime_t*) malloc(sizeof(runtime_t) * config->num_tasks);

    for (int i = 0; i < config->num_tasks; i++) {
        runtime[i].data = config->task_config[i];
        runtime[i].task_id = i;
        runtime[i].time_left = config->task_config[i].execution;
    }

    // Sort tasks by period
    qsort(runtime, config->num_tasks, sizeof(runtime_t), period_compare);

    int *periods = (int*) malloc(sizeof(int) * config->num_tasks);
    for (int i =0; i < config->num_tasks; i++) {
        periods[i] = config->task_config[i].period;
    }

    int lcm = get_lcm_from_array(config->num_tasks, periods); // get lowest common multiple


    // initialize the structure with simulated data
    timeslot_t *sim_data = (timeslot_t *) malloc(sizeof(timeslot_t) * lcm);
    for (int i = 0; i < lcm; i++) {
        sim_data[i].task_id = -1;
        sim_data[i].deadlines = (bool *) malloc(sizeof(bool) * config->num_tasks);
        for (int j = 0; j < config->num_tasks; j++) {
            sim_data[i].deadlines[j] = false;
        }
    }
    
    // Fill the deadlines for each task
    for (int i=0; i < config->num_tasks; i++) {
        for (int j=0; j < lcm; j += config->task_config[i].period) {
            //printf("i: %d j: %d \n", i, j);
            sim_data[j].deadlines[i] = true;
        }
    }

    // Start simulating
    sim_data[0].task_id = runtime[0].task_id; // first slice always starts with highgest priority task
    runtime[0].time_left -= 1; // the first slot is taken
    if (runtime[0].time_left != 0) {
        running = sim_data[0].task_id;
    }

    bool run = true;
    for(int i=1; i < lcm && run; i++) {
        //printf("-------------------------------- slot: %d -------------------------------- \n", i);
        // update time left based on deadline information
        for (int j =0; j < config->num_tasks; j++) {
            if (sim_data[i].deadlines[j]) {
                for (int k = 0; k < config->num_tasks; k++) {
                    if (j == runtime[k].task_id) {
                        if (runtime[k].time_left != 0) {
                            result->miss_idx = i; // this timeslot missed
                            result->misses[runtime[k].task_id] = true;
                            run = false;
                        } else {
                            // When this update happens runtime[k].time_left must be zero, if not then a deadline miss ocurred
                            runtime[k].time_left = config->task_config[j].execution;
                        }
                        break;
                    }
                }
            }
        }

        if (running == -1) {
            running = runtime[config->num_tasks - 1].task_id; // Look for all tasks
        }

        // Next task must be one with highest priority or the task it self
        running = check_highest_priority(running, config->num_tasks, runtime); // it is possible that the highest priority is the task itself
        
        if (running != -1) { // there is a task to run
            sim_data[i].task_id = running;
            for (int j=0; j<config->num_tasks; j++) {
                if (running == runtime[j].task_id ) {
                    runtime[j].time_left -= 1;
                    if (runtime[j].time_left != 0) {
                        running = runtime[j].task_id;
                    } else {
                        running = -1;
                    }
                    break;
                }
            }

        } else { // empty slot
            running = -1; // redundant
            sim_data[i].task_id = -1;
        }

        //printf("-------------------------------- END SLOT %d -------------------------------- \n\n\n", i);
    }

    printf("execution results\n");

    for (int i =0; i < lcm; i++) {
        printf("slot: %d ----> task: %d \n", i, sim_data[i].task_id+1); // 0 means empty slot when printing
    }
    printf("MISS IDX: %d\n", result->miss_idx);
    printf("Failed: [ ");
    for(int i =0; i < config->num_tasks; i++) {
        printf("%d ", result->misses[i]);
    }
    printf("]\n");

    result->ts = sim_data;
    result->ts_size = lcm;
    return result;
}


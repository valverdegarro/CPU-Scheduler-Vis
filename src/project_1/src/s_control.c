#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "s_control.h"
#include "scheduler.h"
#include "ui.h"
#include "pi_approximator.h"

scheduler_op op;
float yield_percent;

// Arrays to hold current pi values and task progress for each thread
long double pi_values[MAX_THREADS] = {0.0};
float pi_task_progress[MAX_THREADS] = {0.0};


// PRIVATE FUNCTIONS //////////////////////////////////////////////////
// Wrapper function to pass a task to the scheduler
void pi_task(void *args)
    {
        ptask_params *pi_task_params = args;
        run_pi_task(pi_task_params);
    }


void update_ui_loop(){
    for (int i = 0; i < MAX_THREADS; i++){
        set_pi_progress(i, pi_values[i], pi_task_progress[i]);
    }
}

void update_pi_arrays(int index, long double pi_val, float prog){
    pi_values[index] = pi_val;
    pi_task_progress[index] = prog;
}

void clean_pi_arrays(){
    memset(pi_values, 0, sizeof(pi_values));
    memset(pi_task_progress, 0, sizeof(pi_task_progress));
}

// PUBLIC FUNCTIONS ///////////////////////////////////////////////////
void set_scheduler_op(int quantum, expropiative_mode ex_mode) {
    if (ex_mode == ON) {
        op.mode = PREEMPTIVE;
        op.quantum = quantum;
        yield_percent = -1.0;
    } else {
        op.mode = NONPREEMPTIVE;
        yield_percent = (float) quantum; // Quantum is actually a percentage in this case
    }
}

void create_pi_threads(int n_threads, thread_config configs[]){
    for (int i = 0; i < n_threads; i++){
        ptask_params *args = malloc(sizeof(*args));

        args->task_id = i;
        args->terms = configs[i].workload * WORK_UNIT_VALUE;
        args->update_f = update_pi_arrays;
        args->yield_percent = yield_percent;

        if (op.mode == NONPREEMPTIVE){
            args->yield_f = yield_cpu;
        }

        if (create_thread(pi_task, args, configs[i].number_of_tickets) != OK){
            printf("ERROR: Failed to create thread with ID %d\n", i);
            return;
        }
    }
    
}

void run_scheduler(){
    int finished = 0;

    while(finished == 0) {
        finished = start_scheduler(op);
        update_ui_loop();
        while (gtk_events_pending()) {
	    	gtk_main_iteration();
	    }
    }
    
    clean_pi_arrays();
}


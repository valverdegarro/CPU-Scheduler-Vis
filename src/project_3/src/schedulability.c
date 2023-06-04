#include "common.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <string.h>

sche_data_t *testability_rm(gui_config *config) {
    sche_data_t *result  = (sche_data_t*)malloc(sizeof(sche_data_t));

    int num_tasks = config -> num_tasks;
    float mu = 0;
    float u_n_tasks;
    result -> task_config = config -> task_config;

    for (int i = 0; i < num_tasks; i++) {
        int exec_time = config -> task_config[i].execution;
        int period = config -> task_config[i].period;
        float div_exec_time_by_period = (float) exec_time / period;
        mu += div_exec_time_by_period;
    }

    result -> mu = mu;

    u_n_tasks = num_tasks * (pow(2, 1.0 / num_tasks) - 1);

    result -> u_n_tasks = u_n_tasks;

    if (mu <= u_n_tasks) {
        result -> message = (char*)malloc(sizeof(char) * (strlen("It approved the test! The tasks are schedulable!") + 1));
        strcpy(result -> message, "It approved the test! The tasks are schedulable!");
    } else {
        result -> message = (char*)malloc(sizeof(char) * (strlen("It didn't approve the test! Maybe try to schedule those tasks, are not a good idea!") + 1));
        strcpy(result -> message, "It didn't approve the test! Maybe try to schedule those tasks, are not a good idea!");
    }

    return result;
}

sche_data_t *testability_edf(gui_config *config) {
    sche_data_t *result  = (sche_data_t*)malloc(sizeof(sche_data_t));

    int num_tasks = config -> num_tasks;
    float mu = 0;
    float u_n_tasks;
    result -> task_config = config -> task_config;

    for (int i = 0; i < num_tasks; i++) {
        int exec_time = config -> task_config[i].execution;
        int period = config -> task_config[i].period;
        float div_exec_time_by_period = (float) exec_time / period;
        mu += div_exec_time_by_period;
    }

    result -> mu = mu;

    u_n_tasks = 1.0;

    result -> u_n_tasks = u_n_tasks;

    if (mu <= u_n_tasks) {
        result -> message = (char*)malloc(sizeof(char) * (strlen("It approved the test! The tasks are schedulable!\n") + 1));
        strcpy(result -> message, "It approved the test! The tasks are schedulable!\n");
    } else {
        result -> message = (char*)malloc(sizeof(char) * (strlen("It didn't approve the test! Maybe try to schedule those tasks, are not a good idea!\n") + 1));
        strcpy(result -> message, "It didn't approve the test! Maybe try to schedule those tasks, are not a good idea!\n");
    }

    return result;
}

sim_data_t *testability_llf(gui_config *config) {
    sim_data_t *result  = (sim_data_t*)malloc(sizeof(sim_data_t));
    return result;
}
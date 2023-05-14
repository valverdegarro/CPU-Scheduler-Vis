#include <math.h>
#include <stdio.h>

#include "pi_approximator.h"
#include "ui.h"

// PRIVATE FUNCTIONS //////////////////////////////////////////////////
int calculate_yield_step(int terms, float yield_percent)
{
    float f_yield_step = terms / 100.0 * yield_percent;
    int yield_step = (int)ceil(f_yield_step);

    if (yield_step == 0) { // Avoid possible division by 0, although this case should never occur
        return 1;
    } else {
        return yield_step;
    }
}

void print_pi_val(long double pi_val, int id)
{
    printf("Task %d - Pi Val: %0.16Lf\n", id, pi_val);
}

// PUBLIC FUNCTIONS ///////////////////////////////////////////////////
long double run_pi_task(ptask_params* params)
{
    long double arctan_val = 0.0;
    long double divisor = 1.0;
    long double sign = 1.0;

    int yield_step = calculate_yield_step(params->terms, params->yield_percent);

    long double pi_val = 0.0;
    float progress = 0.0;

    printf("Initiating pi_task (ID: %d, Terms: %d)\n", params->task_id, params->terms);

    // Approximate arctan(1) using Gregory's formula
    for (int i = 0; i < params->terms; i++) {
        arctan_val += sign / divisor;
        divisor += 2;
        sign = -sign;

        if ((i + 1) % UPDATE_INTERVAL == 0) {
            pi_val = 4.0 * arctan_val; // Pi = 4 * arctan(1)
            progress = (float) i / (float) params->terms;
            
            params->update_f(params->task_id, pi_val, progress);
        }

        // [Non-expropiative case] Check percentage of terms calculated to
        // voluntarily yield CPU
        if (params->yield_percent != -1.0 && (i + 1) % yield_step == 0) {
            printf("Task %d yielding CPU...\n", params->task_id);
            printf("Current terms = %d\n", i + 1);
            params->yield_f();
        }
    }

    pi_val = 4.0 * arctan_val;
    
    // Update UI with final Pi value before exiting
    printf("---------------------------------------------------\n");
    printf("Exiting pi_task %d with final value:\n", params->task_id);
    print_pi_val(pi_val, params->task_id);
    params->update_f(params->task_id, pi_val, 1.0);
    printf("---------------------------------------------------\n");

    return pi_val;
}
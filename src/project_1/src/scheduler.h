#include "types.h"

#define PREEMPTIVE 1
#define NONPREEMPTIVE 0

typedef struct scheduler_op {
    int mode; // 1: preemptive, 0: non-preemptive
    int quantum;
} scheduler_op;

// This will return until the last task completes
int start_scheduler(scheduler_op op);

int create_thread(void (*f)(void* args), void* args, int tickets);

void execute_thread(void (*f)(void));

// yield_cpu calls the scheduler. Invoked by tasks ONLY in non-preemptive mode
void yield_cpu();
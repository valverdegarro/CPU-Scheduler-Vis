#include "types.h"

#define WORK_UNIT_VALUE 50 // Terms of the Pi series per unit


void set_scheduler_op(int quantum, expropiative_mode ex_mode);
void create_pi_threads(int n_threads, thread_config configs[]);
void run_scheduler();
void stop_scheduler();
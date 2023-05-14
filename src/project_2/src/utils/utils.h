#ifndef UTILS_H
#define UTILS_H

#define EPSILON 0.0000001

double sample_exp_d(float mu);
int random_num();
char *get_current_timestamp();

void print_stats(int pid, int num_msgs, int acc_time_waiting, int acc_time_blocked);

#endif // UTILS_H
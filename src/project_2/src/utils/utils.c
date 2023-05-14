#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"

double sample_exp_d(float mu) {
  // Transform mean to lambda param
  double lambda = 1 / mu;

  // Generate a random variate using inverse sampling described in
  // https://en.wikipedia.org/wiki/Exponential_distribution#Random_variate_generation

  srand(time(NULL));
  double u = (double)rand() / RAND_MAX;
  u = u + EPSILON; // Avoid absolute 0

  return -log(u) / lambda;
}

int random_num() {
  srand(time(NULL)); // seed the random number generator with the current time
  int random_num = rand() % 100; // generate a random number between 0 and 99

  return random_num;
}

char *get_current_timestamp() {
    time_t current_time = time(NULL);
    struct tm *tm = localtime(&current_time);
    char *timestamp = malloc(sizeof(char) * 20);
    strftime(timestamp, 20, "%Y-%m-%d %H:%M:%S", tm);
    return timestamp;
}

void print_stats(int pid, int num_msgs, int acc_time_waiting, int acc_time_blocked) {
    printf("\n\
+-------------------------------------------------------------------------+\n\
|                              FINAL STATS                                |\n\
+--------+------------+--------------------+------------------------------+\n\
| PID:   | # messages | Time sleeping (ms) | Time waiting semaphores (us) |\n\
+------------------------------------------------------------------------+\n\
  %d        %d              %d                       %d\n\n", pid, num_msgs, acc_time_waiting/1000, acc_time_blocked);
}
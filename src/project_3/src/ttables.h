#ifndef TTABLES_H
#define TTABLES_H

#include "common.h"

#define LINE_SIZE 100
#define BLOCK_SIZE 1024

#define COLOR_COUNT 6
#define BAR_TRANSP 40
#define DEADLINE_TRANSP 80


int write_gheader(char *buff, int mcm);


// Writes a single time table (gantt chart environment) to buffer from the
// given ts_array
int write_ttable(char *buff, timeslot_t *ts, int mcm, int n_tasks);


#endif // TTABLES_H
#ifndef TTABLES_H
#define TTABLES_H

#include "../common.h"
#include <stdio.h>


/* Writes a single time table (gantt chart environment) to buffer from the given ts_array. 
 *
 * Parameters:
 *  - *buff:        the buffer to write to
 * 
 *  - *ts:          array of timeslot_t structs
 * 
 *  - size:         size of ts array
 * 
 *  - n_tasks:      number of tasks inputted by user
 * 
 *  - *misses:      array of bool, indicates which tasks misses their deadline, if any
 * 
 *  - miss_idx:     index of the timeslot where a deadline was missed (-1 if there are no misses)
 */
int write_ttable(char *buff, timeslot_t *ts, int size, int n_tasks, bool *misses, int miss_idx);


/* Writes the slide(s) (beamer frames) to display the time tables for the given executions array. Writing 
 * is done directly to the file indicated by *fptr_out. */
int write_ttable_slides(FILE *fptr_out, ttable_params *executions, bool single, int lcm, int n_tasks);


#endif // TTABLES_H
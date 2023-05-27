#ifndef TTABLES_H
#define TTABLES_H

#include "../common.h"
#include <stdio.h>


/* Writes the slide(s) (beamer frames) to display the time tables for the given executions array. Writing 
 * is done directly to the file indicated by *fptr_out. */
int write_ttable_slides(FILE *fptr_out, ttable_params *executions, bool single, int lcm, int n_tasks);


#endif // TTABLES_H
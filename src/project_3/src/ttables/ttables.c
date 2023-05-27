#include "ttables.h"
#include "ganttstr.h"
#include "../common.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#define LINE_SIZE 512
#define BLOCK_SIZE 2048

#define COLOR_COUNT 6
#define BAR_TRANSP 40
#define DEADLINE_TRANSP 80

#define MISS_LABEL_SIZE 5 // Bytes per task


const char* colors[] = {"red", "green", "blue", "magenta", "orange", "cyan"};

const char* alg_names[] = {"RM", "EDF", "LLF"};


/* Private functions */

int append_gheader(FILE *fptr_out, int mcm){
    char block[BLOCK_SIZE];
    int last_idx = mcm - 1;


    // Open the ganttchart environment
    snprintf(block, BLOCK_SIZE, "%s{%d}\n\n", GANTT_HEADER, last_idx);
    fputs(block, fptr_out);

    // Write the title list to show time unit labels
    snprintf(block, BLOCK_SIZE, "\\gantttitlelist{0,...,%d}{1} \\\\ \n\n", last_idx);
    fputs(block, fptr_out);


    return OK;
}


int append_deadlines(char **blocks, bool *deadlines, int ts_idx, int n_tasks) {

    char line[LINE_SIZE];

    for (int i = 0; i < n_tasks; i++) {
        if (deadlines[i]) {
            snprintf(line, LINE_SIZE, "\\ganttvline{}{%d}\n", ts_idx);
            strcat(blocks[i], line);
        }
    }

    return OK;
}


int create_ts_blocks(char **blocks, timeslot_t *ts, int size, int n_tasks) {
    char line[LINE_SIZE];

    // Variables to keep track of current task and its range
    int current_task = -1;
    int task_id;
    int start;
    int end;

    // Do an extra loop to print out the last bar
    for (int i = 0; i < size + 1; i ++) {

        // Avoid accessing the array in the last loop
        if (i == size) {
            task_id = -1;
        } else {
            task_id = ts[i].task_id;
            append_deadlines(blocks, ts[i].deadlines, i, n_tasks);
        }

        
        if (task_id != current_task) {

            // Whenever the task changes, write the bar for the previous task
            // (we only do this for non-empty segments, i.e. -1)
            if (current_task != -1) {
                snprintf(
                    line, LINE_SIZE, 
                    "\\ganttbar{}{%d}{%d}\n", start, end);

                strcat(blocks[current_task], line);
            }

            current_task = task_id;
            start = i;
            end = i;

        } else {
            // If we're still on the same task simply update the end point
            end = i; 
        }
    }

    return OK;
}


int append_blocks(FILE *fptr_out, char **blocks, int n_blocks, int size) {
    char line[LINE_SIZE];

    for (int i = 0; i < n_blocks; i++) {

        // Add some block header stuff such as the label and color
        snprintf(line, LINE_SIZE, "%% Task %d\n%s{T%d}{0}{0}\n\n", i, ROW_LABEL, i);
        fputs(line, fptr_out);

        int color = i % COLOR_COUNT;

        snprintf(
            line, LINE_SIZE, 
            "\\ganttset{bar/.append style={fill=%s!%d},vline/.append style = {%s!%d}}\n\n", 
            colors[color], BAR_TRANSP,
            colors[color], DEADLINE_TRANSP
        );

        fputs(line, fptr_out);


        // Concat the block itself
        fputs(blocks[i], fptr_out);


        // Add one final deadline at the end
        snprintf(line, LINE_SIZE, "\\ganttvline{}{%d}\n", size);
        fputs(line, fptr_out);


        // Only add newline between blocks
        if (i != n_blocks - 1) {
            fputs(ROW_BREAK, fptr_out);
        }
    }

    return OK;
}


int append_misses(FILE *fptr_out, bool *misses, int n_tasks, int miss_idx) {
    char block[BLOCK_SIZE];
    char label[n_tasks * MISS_LABEL_SIZE];

    int pos = 0;

    for (int i = 0; i < n_tasks; i++) {
        if (misses[i]) {
            pos += sprintf(&label[pos], "T%d ", i);
        }
    }

    snprintf(block, BLOCK_SIZE, "\n\\ganttvrule{%slate!}{%d}\n", label, miss_idx);
    fputs(block, fptr_out);

    return OK;
}


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
int write_ttable(FILE *fptr_out, timeslot_t *ts, int size, int n_tasks, bool *misses, int miss_idx) {

    int status = 0;


    // Initialize string buffer needed for writing each block of commands (1 block per task)
    char **blocks;

    blocks = malloc(n_tasks * sizeof(char*)); 

    for (int i = 0; i < n_tasks; i++) {
        blocks[i] = malloc(BLOCK_SIZE * sizeof(char));
        memset(blocks[i], 0, BLOCK_SIZE * sizeof(char));
    }


    // Open ganttchart environment
    status = append_gheader(fptr_out, size);
    if (status != 0){
        return -1;
    }


    // Append bars and deadline commands to each block
    create_ts_blocks(blocks, ts, size, n_tasks);
    

    // Concatenate blocks into a single string
    append_blocks(fptr_out, blocks, n_tasks, size);
    

    // Write missed deadlines vrule if neccessary
    if (miss_idx != -1) {
        append_misses(fptr_out, misses, n_tasks, miss_idx);
    }


    // Close ganttchart environment
    fputs(GANTT_TAIL, fptr_out);


    // Free blocks buffer
    for (int i = 0; i < n_tasks; i++) {
        free(blocks[i]);
    }
    free(blocks);


    return OK;
}


/* Public functions */

int write_ttable_slides(FILE *fptr_out, ttable_params *executions, bool single, int lcm, int n_tasks) {

    fputs(BEGIN_FRAME, fptr_out);

    for (int i = 0; i < N_ALGORITHMS; i++) {
        if (executions[i].ts != NULL) {

            // Divide slides into different frames according to user input
            if (!single && i != 0) {
                fputs("\n\\end{frame}\n\n", fptr_out);
                fputs(BEGIN_FRAME, fptr_out);
            }


            // Write the ganttchart with the time table
            fputs("\\textbf{\\small ", fptr_out);
            fputs(alg_names[i], fptr_out);
            fputs(":}\n\n", fptr_out);

            write_ttable(
                fptr_out, 
                executions[i].ts, 
                lcm, 
                n_tasks, 
                executions[i].misses, 
                executions[i].miss_idx);
        }
    }

    fputs("\n\\end{frame}\n\n", fptr_out);

    return OK;
}

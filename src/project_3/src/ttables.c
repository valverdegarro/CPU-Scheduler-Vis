#include "ttables.h"
#include "ganttstr.h"
#include "common.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Private functions */

// TODO: find out why this init function doesn't work
/*int init_blocks(char **blocks, int n_tasks) {

    blocks = malloc(n_tasks * sizeof(char*)); 

    for (int i = 0; i < n_tasks; i++) {
        blocks[i] = malloc(BLOCK_SIZE * sizeof(char));
        memset(blocks[i], 0, BLOCK_SIZE * sizeof(char));
    }

    return 0;
}*/


const char* colors[] = {"red", "green", "blue", "magenta", "orange", "cyan"};


int write_gheader(char *buff, int mcm){
    char block[BLOCK_SIZE];
    int last_idx = mcm - 1;

    // TODO: move this to the base.tex preamble so we don't redefine the element each time
    // Write the deadline element definition
    //snprintf(block, BLOCK_SIZE, "\n%s\n", GANTT_VLINE);
    //strcat(buff, block);

    // Open the ganttchart environment
    snprintf(block, BLOCK_SIZE, "%s{%d}\n\n", GANTT_HEADER, last_idx);
    strcat(buff, block);

    // Write the title list to show time unit labels
    snprintf(block, BLOCK_SIZE, "\\gantttitlelist{0,...,%d}{1} \\\\ \n\n", last_idx);
    strcat(buff, block);


    return 0;
}


int write_deadlines(char **blocks, bool *deadlines, int ts_idx, int n_tasks) {
    
    //      [Deadline procedure]
    //      Iterate deadlines mini-array (j, 0..n_tasks)
    //
    //          If deadlines[j].has_deadline
    //              Write vline{}{i} to blocks[j]
    //    
    //
    //          If deadline[j].missed
    //              Add (task_id, i) pair to list of missed deadlines (which struct should we use for this?)

    char line[LINE_SIZE];

    for (int i = 0; i < n_tasks; i++) {
        if (deadlines[i]) {
            snprintf(line, LINE_SIZE, "\\ganttvline{}{%d}\n", ts_idx);
            strcat(blocks[i], line);
        }

        // TODO: add code for handling deadline misses
    }

    return 0;
}


int append_blocks(char *buff, char **blocks, int n_blocks) {
    char line[LINE_SIZE];

    for (int i = 0; i < n_blocks; i++) {

        // Add some block header stuff such as the label and color
        snprintf(line, LINE_SIZE, "%% Task %d\n%s{T%d}{0}{0}\n\n", i, ROW_LABEL, i);
        strcat(buff, line);

        int color = i % COLOR_COUNT;

        snprintf(
            line, LINE_SIZE, 
            "\\ganttset{bar/.append style={fill=%s!%d},vline/.append style = {%s!%d}}\n\n", 
            colors[color], BAR_TRANSP,
            colors[color], DEADLINE_TRANSP
        );
        strcat(buff, line);


        // Concat the block itself
        strcat(buff, blocks[i]);


        // Only add newline between blocks
        if (i != n_blocks - 1) {
            strcat(buff, ROW_BREAK);
        }
    }

    return 0;
}


/* Public functions */

int write_ttable(char *buff, timeslot_t *ts, int size, int n_tasks){

    // Pseudocode:
    // Iterate over ts array (i, 0..mcm+1):
    //
    //      [Deadline procedure]
    //      Iterate deadlines mini-array (j, 0..n_tasks)
    //
    //          If deadlines[j].has_deadline
    //              Write vline{}{i} to blocks[j]
    //    
    //
    //          If deadline[j].missed
    //              Add (task_id, i) pair to list of missed deadlines (which struct should we use for this?)
    //
    //
    //      [Task procedures]
    //      task_id = ts[i].id OR -1 if i==mcm+1
    //
    //      If task_id != current task:
    //
    //          If current_task != -1:
    //              write gantbar{start}{end} to blocks[current_task]
    //
    //          current_task = task_id
    //          start = i
    //          end = i
    //
    //      Else:
    //          end = i


    int status = 0;

    // Variables to keep track of current task and its range
    int current_task = -1;
    int task_id;
    int start;
    int end;


    // Initialize string buffer needed for writing each block of commands
    char line[LINE_SIZE];
    char **blocks;

    blocks = malloc(n_tasks * sizeof(char*)); 

    for (int i = 0; i < n_tasks; i++) {
        blocks[i] = malloc(BLOCK_SIZE * sizeof(char));
        memset(blocks[i], 0, BLOCK_SIZE * sizeof(char));
    }


    // Open ganttchart environment
    status = write_gheader(buff, size);
    if (status != 0){
        return -1;
    }


    // Do an extra loop to print out the last bar
    for (int i = 0; i < size + 1; i ++) {

        // Avoid accessing the array in the last loop
        if (i == size) {
            task_id = -1;
        } else {
            task_id = ts[i].task_id;
            write_deadlines(blocks, ts[i].deadlines, i, n_tasks);
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


    // Concatenate blocks into a single string
    append_blocks(buff, blocks, n_tasks);
    

    // Close ganttchart environment
    strcat(buff, GANTT_TAIL);

    return 0;
}

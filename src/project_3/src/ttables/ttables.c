#include "ttables.h"
#include "ganttstr.h"
#include "../common.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>


#define LINE_SIZE 512
#define BLOCK_SIZE 2048

#define COLOR_COUNT 6
#define BAR_TRANSP 40
#define DEADLINE_TRANSP 80

#define MISS_LABEL_SIZE 5 // Bytes per task

#define TS_PER_SLIDE 30

#define CHART_HEIGHT 1.5 // in CM, per chart

#define CHART_SCALING_FACTOR 2


const char* colors[] = {"red", "green", "blue", "magenta", "orange", "cyan"};

const char* alg_names[] = {"RM", "EDF", "LLF"};

static float y_unit_height = 0.0;


/* Private functions */

/*
 * Sets the gantt chart height ('y unit chart' option) using a different scaling for
 * single slide and multi slide options.
 */
int set_y_unit_height(int n_tasks, bool single) {
    
    if (single) {
        
        y_unit_height = CHART_HEIGHT / n_tasks;
    
    } else{
        y_unit_height = (CHART_SCALING_FACTOR * CHART_HEIGHT) / n_tasks;
    }

    return OK;
}

int append_gheader(FILE *fptr_out, int start, int end){

    // This fixes some stupid fprintf formatting bug when using the GTK UI
    setlocale(LC_NUMERIC, "POSIX");


    // Open the ganttchart environment
    fprintf(fptr_out, GANTT_HEADER, y_unit_height);
    fprintf(fptr_out, "{%d}{%d}\n\n", start, end);

    // Write the title list to show time axis labels
    fprintf(fptr_out, "\\gantttitlelist{%d,...,%d}{1} \\\\ \n\n", start, end);


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


int create_ts_blocks(char **blocks, timeslot_t *ts, int ts_start, int ts_end, int n_tasks) {
    char line[LINE_SIZE];

    // Variables to keep track of current task and its range
    int current_task = -1;
    int task_id;
    int start;
    int end;

    // Do an extra loop to print out the last bar
    for (int i = ts_start; i <= ts_end + 1; i ++) {

        // Avoid accessing the array in the last loop
        if (i == ts_end + 1) {
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


int append_blocks(FILE *fptr_out, char **blocks, int n_blocks, int start, int end) {

    for (int i = 0; i < n_blocks; i++) {

        // Add some block header stuff such as the label and color
        fprintf(fptr_out, "%% Task %d\n%s{ \\footnotesize T%d}{0}{0}\n\n", i, ROW_LABEL, i);


        int color = i % COLOR_COUNT;

        fprintf(
            fptr_out, 
            "\\ganttset{bar/.append style={fill=%s!%d},vline/.append style = {%s!%d}}\n\n",
            colors[color], BAR_TRANSP,
            colors[color], DEADLINE_TRANSP);



        // Concat the block itself
        fputs(blocks[i], fptr_out);


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
int write_ttable(FILE *fptr_out, timeslot_t *ts, int start, int end, int n_tasks, bool *misses, int miss_idx) {

    int status = 0;


    // Initialize string buffer needed for writing each block of commands (1 block per task)
    char **blocks;

    blocks = malloc(n_tasks * sizeof(char*)); 

    for (int i = 0; i < n_tasks; i++) {
        blocks[i] = malloc(BLOCK_SIZE * sizeof(char));
        memset(blocks[i], 0, BLOCK_SIZE * sizeof(char));
    }


    // Open ganttchart environment
    status = append_gheader(fptr_out, start, end);
    if (status != 0){
        return -1;
    }


    // Append bars and deadline commands to each block
    create_ts_blocks(blocks, ts, start, end, n_tasks);
    

    // Concatenate blocks into a single string
    append_blocks(fptr_out, blocks, n_tasks, start, end);
    

    // Write vrule if missed deadline is inside range
    if (start <= miss_idx && miss_idx <= end + 1) {
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

int write_tt_frame(FILE *fptr_out, ttable_params *executions, int alg_idx, int start, int end, int n_tasks, int lcm) {

    int i_start = 0;
    int i_end = N_ALGORITHMS;

    // Open the frame env
    if (alg_idx != -1) {
        fprintf(fptr_out, "\n\\begin{frame}{Simulación de %s (%d-%d)}\n", alg_names[alg_idx], start, end);
        i_start = alg_idx;
        i_end = alg_idx + 1;
    } else{
        fprintf(fptr_out, "\n\\begin{frame}{Simulación de los algoritmos (%d - %d)}\n", start, end);
    }


    // Print the gantt charts for the selected algorithm(s)
    for (int i = i_start; i < i_end; i++){
        if (executions[i].ts != NULL) {

            fprintf(fptr_out, "\n\\textbf{ \\scriptsize %s:}\n\n", alg_names[i]);

            write_ttable(
                fptr_out, 
                executions[i].ts, 
                start,
                end, 
                n_tasks, 
                executions[i].misses, 
                executions[i].miss_idx);
        }
    }
    

    fputs("\n\\end{frame}\n", fptr_out);

    return OK;
}

void write_formula_rm(FILE *fptr_out, sche_data_t *data, int n_tasks) {

    fprintf(fptr_out, "\nFactor de utilización: \n");

    fprintf(fptr_out, "\\begin{equation}\n");
    fprintf(fptr_out, "\\mu = \\sum_{i=1}^{n}\\frac{c_{i}}{p_{i}} \n");
    fprintf(fptr_out, "\\end{equation}\n");

    fprintf(fptr_out, "\\begin{equation}\n");
    fprintf(fptr_out, "\\mu=\n");
    for (int i = 0; i < n_tasks; i++) {
        fprintf(fptr_out, "\\frac{%d}{%d}", data -> task_config[i].execution, data -> task_config[i].period);
        if (i + 1 != n_tasks) {
            fprintf(fptr_out, "+");
        } else {
            fprintf(fptr_out, "={%.3f}", data -> mu);
        }
    }
    fprintf(fptr_out, "\\end{equation}\n");

    fprintf(fptr_out, "\nCota de utilización: \n");

    fprintf(fptr_out, "\\begin{equation}\n");
    fprintf(fptr_out, "U(n) = n(2^{\\frac{1}{n}}-1) \n");
    fprintf(fptr_out, "\\end{equation}\n");

    fprintf(fptr_out, "\\begin{equation}\n");
    fprintf(fptr_out, "U({%d}) = {%d}(2^{\\frac{1}{{%d}}}-1) = {%.3f} \n", n_tasks, n_tasks, n_tasks, data -> u_n_tasks);
    fprintf(fptr_out, "\\end{equation}\n");

    fprintf(fptr_out, "\nCondición a cumplir para \\textbf{schedulability}\n");

    fprintf(fptr_out, "\\begin{equation}\n");
    fprintf(fptr_out, "\\mu \\leq U(n) \n");
    fprintf(fptr_out, "\\end{equation}\n");

    fprintf(fptr_out, "\\begin{equation}\n");
    fprintf(fptr_out, "{%.3f} \\leq {%.3f}\n", data -> mu, data -> u_n_tasks);
    fprintf(fptr_out, "\\end{equation}\n");
    
    fprintf(fptr_out, "\nEl resultado indica que: \n");

    fprintf(fptr_out, "\\begin{quote}\n");
    fprintf(fptr_out, "\\centering\n");
    fprintf(fptr_out, "{''%s''}\n", data -> message);
    fprintf(fptr_out, "\\end{quote}\n");
}

void write_formula_edf(FILE *fptr_out, sche_data_t *data, int n_tasks) {

    fprintf(fptr_out, "\nFactor de utilización: \n");

    fprintf(fptr_out, "\\begin{equation}\n");
    fprintf(fptr_out, "\\mu = \\sum_{i=1}^{n}\\frac{c_{i}}{p_{i}} \n");
    fprintf(fptr_out, "\\end{equation}\n");

    fprintf(fptr_out, "\\begin{equation}\n");
    fprintf(fptr_out, "\\mu=\n");
    for (int i = 0; i < n_tasks; i++) {
        fprintf(fptr_out, "\\frac{%d}{%d}", data -> task_config[i].execution, data -> task_config[i].period);
        if (i + 1 != n_tasks) {
            fprintf(fptr_out, "+");
        } else {
            fprintf(fptr_out, "={%.3f}", data -> mu);
        }
    }
    fprintf(fptr_out, "\\end{equation}\n");

    fprintf(fptr_out, "\nCondición a cumplir para \\textbf{schedulability}\n");

    fprintf(fptr_out, "\\begin{equation}\n");
    fprintf(fptr_out, "\\mu \\leq 1.0 \n");
    fprintf(fptr_out, "\\end{equation}\n");

    fprintf(fptr_out, "\\begin{equation}\n");
    fprintf(fptr_out, "{%.3f} \\leq {%.1f}\n", data -> mu, data -> u_n_tasks);
    fprintf(fptr_out, "\\end{equation}\n");
    
    fprintf(fptr_out, "\nEl resultado indica que: \n");

    fprintf(fptr_out, "\\begin{quote}\n");
    fprintf(fptr_out, "\\centering\n");
    fprintf(fptr_out, "{''%s''}\n", data -> message);
    fprintf(fptr_out, "\\end{quote}\n");
}

int write_ttest(FILE *fptr_out, ttest_params execution, int alg_idx, int n_tasks) {

    if (alg_idx == 0) {
        write_formula_rm(fptr_out, execution.data, n_tasks);
    }

    if (alg_idx == 1) {
        write_formula_edf(fptr_out, execution.data, n_tasks);
    }

    free(execution.data);

    return OK;
}

int write_ttest_frame(FILE *fptr_out, ttest_params execution, int alg_idx, int n_tasks) {

    fprintf(fptr_out, "\n\\begin{frame}{Test de schedulability de %s}\n", alg_names[alg_idx]);
    
    write_ttest(fptr_out, execution, alg_idx, n_tasks);

    fputs("\n\\end{frame}\n", fptr_out);

    return OK;
}


/* Public functions */

int write_ttable_slides(FILE *fptr_out, ttable_params *executions, bool single, int lcm, int n_tasks) {

    // Variables for keeping track of the range of timeslots we're currently writing
    int start;
    int end;


    // Variables for single or multi-slide per algorithm functionality
    int i_start = 0;
    int i_end = N_ALGORITHMS;
    int alg_idx;


    // Set the chart vertical scaling
    set_y_unit_height(n_tasks, single);


    // Make the frame titles smaller
    fputs("{ \\setbeamerfont{frametitle}{size=\\scriptsize}\n\n", fptr_out);


    if (single) {
        i_end = i_start + 1;
        fputs("\n\\section{Simulaciones}\n\n", fptr_out);
    }


    // NOTE: if single is true, the following for statement will only execute one loop
    // this handles the logic of having all algorithms side-by-side in the same slide or
    // having them separated
    for (int i = i_start; i < i_end; i++){

        if (executions[i].ts == NULL && !single) {
            continue;
        }

        alg_idx = i;

        if (single) {
            alg_idx = -1;
            
        } else {
            fprintf(fptr_out, "\n\\section{Simulación de %s}\n\n", alg_names[alg_idx]);
        }

        // Reset the range in each iteration
        start = 0;
        end = 0;

        while (end < lcm) {

            // Increment the end pointer, if it exceeds the lcm then set it to lcm
            end += TS_PER_SLIDE;

            if(end > lcm) {
                end = lcm;
            }

            // Do some writing to the file
            write_tt_frame(fptr_out, executions, alg_idx, start, end - 1, n_tasks, lcm);


            // Update the start pointer
            start = end;
        }

    }

    // Close the brackets so frame titles go back to normal
    fputs("\n\n}\n\n", fptr_out);

    return OK;
}

int write_ttest_slides(FILE *fptr_out, ttest_params *executions, bool single, int n_tasks) {


    // Variables for single or multi-slide per algorithm functionality
    int i_start = 0;
    int i_end = N_ALGORITHMS;
    int alg_idx;


    // Set the chart vertical scaling
    set_y_unit_height(n_tasks, single);


    // Make the frame titles smaller
    fputs("{ \\setbeamerfont{frametitle}{size=\\scriptsize}\n\n", fptr_out);

    // NOTE: if single is true, the following for statement will only execute one loop
    // this handles the logic of having all algorithms side-by-side in the same slide or
    // having them separated
    for (int i = i_start; i < i_end; i++){

        if (executions[i].enabled == false) {
            continue;
        }

        alg_idx = i;

        // Do some writing to the file
        write_ttest_frame(fptr_out, executions[i], alg_idx, n_tasks);

    }

    // Close the brackets so frame titles go back to normal
    fputs("\n\n}\n\n", fptr_out);

    return OK;
}


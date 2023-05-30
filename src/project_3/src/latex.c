#include "common.h"
#include "simulation.h"
#include "ttables/ttables.h"
#include <stdio.h>
#include <stdlib.h>

char table_colors[MAX_TASKS][7] = {
    //"ECF6CE",
    //"A4A4A4",
    //"F781D8",
    //"8181F7",
    //"80FF00",
    //"F4FA58",
    //"A9D0F5",
    //"0080FF",
    //"F5A9F2",
    //"04B45F",
    "FF0040",
    "088A85",
    "FFCC67",
    "FFC702",
    "6200C9",
    "303498",
    "FD6864",
    "F5A9F2",
    "FE642E",
    "DF7401"
    };

int read_static_file(char *filename, FILE *output) {
    char c;
    FILE *file = fopen(filename,"r");
    if (file == NULL) {
        printf("Cannot open file %s \n", filename);
       return FAIL;
    }

    c = fgetc(file);
    while (c != EOF) {
        fputc(c, output);
        c = fgetc(file);
    }

    fclose(file);

    return OK;
}


int gen_algorithm_static_info(gui_config *config, FILE *fptr_out) {
    int status;
    char *file_base_rm = "./src/latex/alg_static_rm.tex";
    char *file_base_edf = "./src/latex/alg_static_edf.tex";
    char *file_base_llf = "./src/latex/alg_static_llf.tex";

    if (config->rm_enabled) {
        status = read_static_file(file_base_rm, fptr_out);
        if (status != OK) {
            return status;
        }
    }

    if (config->edf_enabled) {
        status = read_static_file(file_base_edf, fptr_out);
        if (status != OK) {
            return status;
        }
    }

    if (config->llf_enabled) {
        status = read_static_file(file_base_llf, fptr_out);
        if (status != OK) {
            return status;
        }
    }

    return OK;
} 

void table_task_create_ppt(gui_config *config, FILE* file)
{
  // Task description table
  fputs("\\begin{frame}\n", file);
  fputs("\\frametitle{Tareas}", file);
  fputs("\\begin{table}[]\n", file);
  fputs("\\begin{tabular}{|c|c|c|}\n", file);
  fputs("\\hline\n", file);
  fputs("\\textbf{ID} & \\textbf{Tiempo de ejecución} & \\textbf{Periodo}", file);
  fputs("\\\\ \\hline\n", file);
  for(int i=0; i<config->num_tasks; i++)
  {
    fprintf(file, "\\cellcolor[HTML]{%s}%d &", table_colors[i], i);
    fprintf(file, "\\cellcolor[HTML]{%s}%d &", table_colors[i], config->task_config[i].execution);
    fprintf(file, "\\cellcolor[HTML]{%s}%d ", table_colors[i], config->task_config[i].period);
    fputs("\\\\ \\hline\n", file);
  }
  fputs("\\end{tabular}\n", file);
  fputs("\\end{table}\n", file);
  fputs("\\end{frame}\n", file);
}

int gen_execution_tables(gui_config *config, FILE *fptr_out) {
    // print tasks information
    table_task_create_ppt(config, fptr_out);


    ttable_params executions[N_ALGORITHMS];
    int lcm;

    for (int i = 0; i < N_ALGORITHMS; i++) {
        executions[i].ts = NULL;
        executions[i].miss_idx = -1;
    }

    if (config->rm_enabled) {
        sim_data_t * val = simulate_rm(config);
        executions[0].ts = val->ts;
        executions[0].miss_idx = val->miss_idx;
        executions[0].misses = val->misses;
        lcm = val->ts_size;
    }

    if (config->edf_enabled) {
        sim_data_t * val = simulate_edf(config);
        executions[1].ts = val->ts;
        executions[1].miss_idx = val->miss_idx;
        executions[1].misses = val->misses;
        lcm = val->ts_size;
    }

    if (config->llf_enabled) {
        sim_data_t * val = simulate_llf(config);
        executions[2].ts = val->ts;
        executions[2].miss_idx = val->miss_idx;
        executions[2].misses = val->misses;
        lcm = val->ts_size;
    }

    write_ttable_slides(fptr_out, executions, config->single_slide, lcm, config->num_tasks);

    return OK;
}

int generate_pdf(gui_config *config, FILE *fptr_out) {
    int status;

    // Generate the static algorithm section
    status = gen_algorithm_static_info(config, fptr_out);
    if (status != OK) {
        return status;
    }

    // Generate schedulability tests
    fputs("\\section{Tests de schedulability}\n", fptr_out);
    // TODO

    // Generate execution tables
    fputs("\\section{Ejecución}\n", fptr_out);
    gen_execution_tables(config, fptr_out);

    return OK;
}

int latex_execute(gui_config *config) {
    int status;
    char *file_out = "./src/latex/out.tex";
    char *file_base = "./src/latex/base.tex";
    FILE *fptr_out;

    // Open out file
    fptr_out = fopen(file_out,"w");
    if (fptr_out == NULL) {
        printf("Cannot open file %s \n", file_out);
       return FAIL;
    }

    status = read_static_file(file_base, fptr_out);
    if (status != OK) {
        return status;
    }

    status = generate_pdf(config, fptr_out);
    if (status != OK) {
        return status;
    }

    // close the document
    fputs("\n", fptr_out);
    fputs("\\end{document}", fptr_out);

    // Close file pointer
    fclose(fptr_out);

    // Compile the latex file
    system("pdflatex ./src/latex/out.tex");
    system("pdflatex ./src/latex/out.tex");

    // Show the pdf using evince
    system("evince out.pdf &");

    return OK;
}



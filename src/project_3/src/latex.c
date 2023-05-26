#include "common.h"
#include "simulation.h"
#include "ttables/ttables.h"
#include <stdio.h>
#include <stdlib.h>

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

int gen_execution_tables(gui_config *config, FILE *fptr_out) {
    ttable_params executions[N_ALGORITHMS];
    int lcm = 20; //TODO: set this with the results from simulate_*

    for (int i = 0; i < N_ALGORITHMS; i++) {
        executions[i].ts = NULL;
        executions[i].miss_idx = -1;
    }

    if (config->rm_enabled) {
        
        executions[0].ts = simulate_rm(config);
        //rm_sim.miss_idx= TODO: assign these once we have access to deadline miss info
        //rm_sim.misses = NULL;
    }

    if (config->edf_enabled) {
        executions[1].ts = simulate_rm(config); // TODO: change this to simulate_edf
        //rm_sim.miss_idx= TODO: assign these once we have access to deadline miss info
        //rm_sim.misses = NULL;
    }

    if (config->llf_enabled) {
        executions[2].ts = simulate_rm(config); // TODO: change this to simulate_llf
        //rm_sim.miss_idx= TODO: assign these once we have access to deadline miss info
        //rm_sim.misses = NULL;
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
    // TODO

    // Generate execution tables
    //timeslot_t *rm_results = simulate_rm(config);
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



#include "common.h"
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



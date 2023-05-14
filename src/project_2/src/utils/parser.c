#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../sync/types.h"

int read_parameters(int argc, char *argv[], char **buffer_name, float *mean) {
    if (argc != 3) {
        fprintf(stderr, "Error: The amount of parameters is not correct, it's expected 2 parameters\n");
        exit(FAIL);
    }

    *buffer_name = malloc(strlen(argv[1]) + 1);
    if (*buffer_name == NULL) {
        fprintf(stderr, "Error: Could not allocate memory\n");
        exit(FAIL);
    }

    strcpy(*buffer_name, argv[1]);

    *mean = atof(argv[2]);

    if(*mean == 0.0) {
        fprintf(stderr, "Error: The parameter needs to be a float or an integer and greater than 0\n");
        exit(FAIL);
    }

    return OK;
}


int read_finalizer_params(int argc, char *argv[], char **buffer_name) {
    if (argc != 2) {
        fprintf(stderr, "Error: The amount of parameters is not correct, it's expected 1 parameters\n");
        exit(FAIL);
    }

    *buffer_name = malloc(strlen(argv[1]) + 1);
    if (*buffer_name == NULL) {
        fprintf(stderr, "Error: Could not allocate memory\n");
        exit(FAIL);
    }
    strcpy(*buffer_name, argv[1]);

    return OK;
}
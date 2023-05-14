#ifndef PARAMETER_PARSER_H
#define PARAMETER_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read_creator_parameters(int argc, char *argv[], char **buffer_name,
                            int *messages) {
  const int expected_number_of_params = 3;
  const int fail_code = 1;
  const int exit_code = 0;
  if (argc != expected_number_of_params) {
    fprintf(stderr, "The amount of parameters is not correct, it's expected [%d] "
           "parameters\n",
           expected_number_of_params - 1);
    exit(fail_code);
  }

  *buffer_name = malloc(strlen(argv[1]) + 1);
  if (*buffer_name == NULL) {
    fprintf(stderr, "Could not allocate memory\n");
    exit(fail_code);
  }

  strcpy(*buffer_name, argv[1]);

  *messages = atoi(argv[2]);

  if(*messages == 0.0) {
      fprintf(stderr, "Error: The parameter needs to be an integer and greater than 0\n");
      exit(fail_code);
    }

  return exit_code;
}

#endif // PARAMETER_PARSER_H
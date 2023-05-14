#ifndef PARSER_H
#define PARSER_H

int read_parameters(int argc, char *argv[], char **buffer_name, float *mean);

int read_finalizer_params(int argc, char *argv[], char **buffer_name);

#endif // PARSER_H
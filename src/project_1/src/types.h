#ifndef TYPES_H
#define TYPES_H

#define OK 0
#define FAIL 1
#define MAX_THREADS 40

typedef struct thread_config {
    int workload;
    int number_of_tickets;
} thread_config;

typedef enum expropiative_mode {
    ON,
    OFF
} expropiative_mode;

#endif /* TYPES_H */

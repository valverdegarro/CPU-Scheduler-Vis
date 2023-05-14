#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define OK 0
#define FAIL -1

// Size of the array where counters are saved, ideally matches the maximum number of syscalls
#define COUNTER_SIZE 512

// Width in characters of each row in the table printed by print_summary
#define SUMMARY_TABLE_WIDTH 100

// Reads the received parameters and sets the correct value for step_mode_enabled
// Sets in child_argc and child_argv the command and arguments of the child process
// Returns 0 if success, -1 otherwise
int read_parameters(int argc, char **argv, int *child_argc, char **child_argv);

// start the tracing the child process specified in argv.
// Returns 0 if success, -1 otherwise
int start_tracing(int child_argc, char **child_argv, int *counter, pid_t *out_pid);

// Prints in the stdout a table with the syscalls that were used by the child (and the number of times)
// Returns 0 if success, -1 otherwise
int print_summary(int *counter);

// Reads the parameters, return value, and number of the syscall invoked by the child process, and prints them in the stdout.
// If step_mode_enabled == 1, then it should ask the user to press enter before continuing
// If step_mode_enabled == -1, then print_verbose does nothing and exits with 0.
// Returns 0 if success, -1 otherwise
int print_verbose(pid_t child, int *counter, int *exit);

// Receives the syscall number (syscall_num) and returns the syscall name (syscall_name).
// Check the linux syscall table here: https://filippo.io/linux-syscall-table/
// Returns 0 if success, -1 otherwise
char *get_syscall_name(int syscall_num);

// Called by main function and begin systrace routine.
int systrace(int argc, char **argv);
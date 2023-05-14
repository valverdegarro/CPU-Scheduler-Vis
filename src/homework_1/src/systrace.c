#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "systrace.h"
#include "syscall_table.h"

// Mode in which the program was invoked,
// -V enables the step mode (step_mode_enabled=1)
// -v disables the step mode (step_mode_enabled=0)
// default value is disabled
int step_mode_enabled = -1;

// Struct that holds the values of the registers, useful for 
// retreiving info from a syscall
struct user_regs_struct regs;


int systrace(int argc, char **argv) {
    int counter[COUNTER_SIZE] = {0};
    int child_argc = 0;
    char **child_argv = argv;
    int status;
    pid_t pid;

    status = read_parameters(argc, argv, &child_argc, child_argv);
    if (status != OK) {
        return status;
    }

    status = start_tracing(child_argc, child_argv, counter, &pid);
    if (status != OK) {
        return status;
    }

    if (pid != 0) {
        status = print_summary(counter);
    }

    return status;
}

int read_parameters(int argc, char **argv, int *child_argc, char **child_argv) {
    if(argc < 2) {
        printf("Insufficient arguments error. You need to provide at least the name of the process to trace.\n");
        return FAIL;
    } else  {
        for ( int i = 1; i < argc; ++i ) {

            if (strcmp(argv[i], "-v") == 0) {
                step_mode_enabled = 0;
            }
            else if (strcmp(argv[i], "-V") == 0) {
                step_mode_enabled = 1;
            } else {
                child_argv[*child_argc] = argv[i];
                ++(*child_argc);
            }
        }
    }

    if(*child_argc < 1){
        printf("Process name to trace was not found. Please provide the name of the process to trace");
        return FAIL;
    }

    return OK;
}

// returns 1 if the child process exits, 0 if the child is stopped in a syscall
int set_tracepoint(pid_t pid) {
    int status;
    while (1) {
        ptrace(PTRACE_SYSCALL, pid, 0, 0);
        waitpid(pid, &status, 0);
        if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80) {
            return 0;
        }
        if (WIFEXITED(status)) {
            return 1;
        }
    }
}

int start_tracing(int child_argc, char **child_argv, int *counter, pid_t *out_pid) {
    pid_t pid;
    int status;
    int exit;
    char *new_args [child_argc+1];

    pid = fork();
    *out_pid = pid;
    if(pid == 0) {
        // child process

        // copy args
        memcpy(new_args, child_argv, child_argc * sizeof(char*));
        new_args[child_argc] = NULL;

        ptrace(PTRACE_TRACEME);
        kill(getpid(), SIGSTOP);
        status = execvp(child_argv[0], new_args);
        if (status == -1) {
            printf("returning here\n");
            return FAIL;
        }
        return OK;
    } else {
        // trace process
        waitpid(pid, &status, 0);
        ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
        while(1) {
            exit = set_tracepoint(pid);
            if (exit == 1) {
                break;
            }
            status = print_verbose(pid, counter, &exit);
            if (status != OK) {
                return status;
            }
            if (exit == 1) {
                break;
            }
        }
        return OK;
    }
}

int print_summary(int *counter) {
    // Allocate memory for the table string
    char *table = (char*) malloc(COUNTER_SIZE* SUMMARY_TABLE_WIDTH * sizeof(char));

    // Prints table header
    printf("---------------------------------\n");
    printf("Count | Syscall\n");
    printf("---------------------------------\n");

    // Writes table body
    for(int i = 0; i < COUNTER_SIZE; i++) {
        if(counter[i] > 0) {
            char row[SUMMARY_TABLE_WIDTH];

            char* syscall_name = get_syscall_name(i);

            sprintf(row, "%d\t%s\n", counter[i], syscall_name);
            strcat(table, row);
        }
    }

    printf("%s", table);
    free(table);

    return OK;
}

int print_verbose(pid_t pid, int *counter, int *exit) {
    int status;

    // Obtain the register values for the syscall
    ptrace(PTRACE_GETREGS, pid, 0, &regs);

    // Extract syscall number from orig_rax and get its name
    long syscall_num = regs.orig_rax;
    char* syscall_name = get_syscall_name(syscall_num);


    // Save the value of registers rdi, rsi, rdx, r10, r8, r9
    // These hold the syscall arguments (in order from 1 - 6)
    /*unsigned long long int *arguments = {
        regs.rdi,
        regs.rsi,
        regs.rdx,
        regs.r10,
        regs.r8,
        regs.r9,
    };*/
    // NOTE: printing the raw register values without decoding them (non-trivial) adds 
    // little to no information, better to just omit them

    // Increase counter
    counter[syscall_num] += 1;

    // Advance to exit function
    *exit = set_tracepoint(pid);
    if (*exit == 1) {
        return OK;
    }

    // Get syscall return value, once again via the registers
    ptrace(PTRACE_GETREGS, pid, 0, &regs);
    long retval = (long)regs.rax;

    if(step_mode_enabled != -1)
    {
        // Print the syscall
        printf("%s = %ld\n", syscall_name, retval);
    }

    // Wait for the user to press a key to continue when -V option is used
    if (step_mode_enabled == 1) {
        char ch;
        printf("Press enter to continue...\n");
        while (getchar() != '\n') {
            continue;
        }
    }

    return OK;
}

char *get_syscall_name(int syscall_num) {
    return syscall_table[syscall_num].name;
}

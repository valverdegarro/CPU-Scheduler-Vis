#include "scheduler.h"

#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#define STATUS_DONE 0
#define STATUS_READY 1
#define STATUS_RUNNING 2

#define JB_SP 6
#define JB_PC 7
#define STACK_SIZE 16384

typedef unsigned long address_t;

// Thread control block
typedef struct thread_t {
    int id; // The index in threads array
    int tickets; // Number of tickets assigned to this thread, not really useful since the lottery algorithm uses the range
    int min_ticket; // Lowest value of the range of tickets assigned to this thread
    int max_ticket; // Highest value of the range of tickets assigned to this thread
    address_t pc;
    address_t sp;
    int status; // It could be: done, ready or running
    sigjmp_buf jmp_state;
    void* args;
    void (*f_ptr)(void* args);
} thread_t;

// Global variables that only the scheduler should modify
static int total_tickets = 0;
static int num_created_threads = 0;
static int num_active_threads = 0; // Number of threads pending to finish its work (threads with status running or ready)
static int scheduler_quantum_us;
static thread_t all_threads[MAX_THREADS];
thread_t* current_thread = NULL;
sigjmp_buf first_state;
static int scheduler_mode;
static int running = 0;
static int re_enter = 0;

// Declarations of functions
void schedule();

// Black box code
////////////////////////////////////////////////////////////////////////
#ifdef __x86_64__
// code for 64 bit Intel arch

// typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

// A translation required when using an address of a variable
// Use this as a black box in your code.
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
                 "rol    $0x11,%0\n"
                 : "=g"(ret)
                 : "0"(addr));
    return ret;
}

#else
// code for 32 bit Intel arch

// typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

// A translation required when using an address of a variable
// Use this as a black box in your code.
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
                 "rol    $0x9,%0\n"
                 : "=g"(ret)
                 : "0"(addr));
    return ret;
}

#endif
////////////////////////////////////////////////////////////////////////

/*
   PRIVATE METHODS
   ***************
*/

void get_next_thread(int* next_thread_to_run)
{
    int rand_number;
    int chosen_thread;
    if (num_active_threads == 0) {
        printf("No active threads to run, expect a failure!\n");
        return;
    }

    do {
        rand_number = (rand() % total_tickets);
        for (int i = 0; i < num_created_threads; i++) {
            if ((rand_number >= all_threads[i].min_ticket) && (rand_number < all_threads[i].max_ticket)) {
                // A thread was chosen
                chosen_thread = i;
            }
        }
    } while (all_threads[chosen_thread].status == STATUS_DONE);

    *next_thread_to_run = chosen_thread;
}

void clear_signal(void)
{
    if (scheduler_mode == NONPREEMPTIVE) {
        // No need to execute clear signal
        return;
    }

    sigset_t x;
    sigemptyset(&x);
    sigaddset(&x, SIGVTALRM);
    sigprocmask(SIG_UNBLOCK, &x, NULL);
}

static struct itimerval tv;

// The timer_isr is actually the thread scheduler
void timer_isr(int signal)
{
    int ret;
    tv.it_value.tv_sec = 0;
    tv.it_value.tv_usec = 0;
    ret = setitimer(ITIMER_VIRTUAL, &tv, NULL);
    if (ret != 0) {
        printf("Error starting timer\n");
    }

    if (running == 0) {
        return;
    }
    schedule();
}



void start_timer()
{
    //signal(SIGVTALRM, timer_isr);

    int ret;
    if (scheduler_mode == NONPREEMPTIVE) {
        // No need to execute the timer in non-preemptive mode
        return;
    }

    // This is the period between now and the first timer interrupt. If zero, the alarm is disabled.
    tv.it_value.tv_sec = scheduler_quantum_us / 1000000;
    tv.it_value.tv_usec = scheduler_quantum_us % 1000000;

    // This is the period between successive timer interrupts. If zero, the alarm will only be sent once.
    tv.it_interval.tv_sec = 0; // seconds
    tv.it_interval.tv_usec = 0;

    // Start timer
    ret = setitimer(ITIMER_VIRTUAL, &tv, NULL);
    if (ret != 0) {
        printf("Error starting timer\n");
    }
}

// schedule executes the scheduler routine
void schedule()
{
    int next_thread_to_run;

    if (num_active_threads <= 0) {
        running = 0;
        printf("No more active threads to run...\n");
        num_created_threads = 0;

        // Jump to the first state
        siglongjmp(first_state, 1);
    }

    // When the timer starts for the first time
    if (current_thread == NULL) {
        get_next_thread(&next_thread_to_run);

        // Start with the first thread
        current_thread = &all_threads[next_thread_to_run];
        current_thread->status = STATUS_RUNNING;

        clear_signal();

        // Jump to the selected thread
        start_timer();
        siglongjmp(current_thread->jmp_state, 1);

    } else { // For any other time

        if (re_enter == 0) {
            clear_signal();
            // Save the state of the current thread
            if (sigsetjmp(current_thread->jmp_state, 0) == 1) {
                clear_signal();
                // Resume executing the thread
                return;
            }

            // Current thread will move to status_ready
            // If the status is DONE then do not replace it
            if (current_thread->status == STATUS_RUNNING) {
                current_thread->status = STATUS_READY;
            }

            // Jump to the first state
            siglongjmp(first_state, 1);
        } else {
            clear_signal();
            re_enter = 0;

            // Select next thread
            get_next_thread(&next_thread_to_run);
            current_thread = &all_threads[next_thread_to_run];
            current_thread->status = STATUS_RUNNING;

            // Jump to the last state saved for the thread
            start_timer();
            siglongjmp(current_thread->jmp_state, 1);
        }        
    }
}

void initialize_timer(int quantum)
{
    // Install the ISR
    signal(SIGVTALRM, timer_isr);

    // Quantum is in ms, save it as us
    scheduler_quantum_us = quantum * 1000;

    start_timer();
}

void uninitialize_timer() {
    signal(SIGVTALRM, SIG_DFL);
}

void scheduler_preemptive(scheduler_op op)
{
    time_t t;

    initialize_timer(op.quantum);

    // Set a seed for the random number generator
    srand((unsigned)time(&t));

    // wait until the timer starts
    int i = 0;
    while (1) {
        if (i == 100000000) {
            printf("Inside for\n");
            i = 0;
        }
        i++;
    };
}

void scheduler_nonpreemptive(scheduler_op op)
{
    // Set a seed for the random number generator
    srand(time(NULL));

    // Start scheduling
    schedule();

    // This should return when the last task alive is executed
}

void finish_thread(void)
{
    current_thread->status = STATUS_DONE;
    num_active_threads--;

    // Call the scheduler
    if (scheduler_mode == NONPREEMPTIVE) {
        schedule();
    } else {
        raise(SIGVTALRM);
        // Wait some time until the signal is handled
        while(1) {
            printf("stuck!\n");
        };
    }
}

void f_wrapper(void (*f)(void))
{
    void* args = current_thread->args;

    // execute function
    current_thread->f_ptr(args);

    finish_thread();
}

void clean_scheduler()
{
    total_tickets = 0;
    num_created_threads = 0;
    num_active_threads = 0;
    current_thread = NULL;
    clear_signal();
    uninitialize_timer();
}

/*
   PUBLIC METHODS
   **************
*/

int start_scheduler(scheduler_op op)
{
    int finished = 1;
    // Check that tasks are not zero
    if (num_created_threads == 0) {
        printf("No tasks to schedule\n");
        return finished;
    }

    // First time the scheduler is called
    if (num_active_threads > 0) {
        running = 1;
    } else {
        printf("DO NOT CALL ME AGAIN! \n");
        finished = 1;
        return finished;
    }

    // Save the initial state
    if (sigsetjmp(first_state, 0) == 1) {
        if (running == 0) {
            // This means that the scheduler finished
            printf("Scheduler is finishing... \n");
            clean_scheduler();
            re_enter = 0;
            finished = 1;
            return finished;
        } else {
            // This case means that the scheduler must be called again
            re_enter = 1;
            finished = 0;
            uninitialize_timer();
            return finished;
        }
        
    }

    scheduler_mode = op.mode;
    if (op.mode == PREEMPTIVE) {
        scheduler_preemptive(op);
    } else if (op.mode == NONPREEMPTIVE) {
        scheduler_nonpreemptive(op);
    } else {
        printf("UNSUPPORTED SCHEDULE MODE\n");
        finished = 1;
        return finished;
    }

    return 1;
}

int create_thread(void (*f)(void* args), void* args, int tickets)
{
    int current_thread_id;

    if (f == NULL) {
        return FAIL;
    }

    current_thread_id = num_created_threads;

    all_threads[current_thread_id].id = current_thread_id;
    all_threads[current_thread_id].tickets = tickets;
    all_threads[current_thread_id].min_ticket = total_tickets;
    all_threads[current_thread_id].max_ticket = total_tickets + tickets;
    all_threads[current_thread_id].pc = (address_t)f_wrapper;
    all_threads[current_thread_id].sp = (address_t)malloc(STACK_SIZE);
    all_threads[current_thread_id].sp = all_threads[current_thread_id].sp + STACK_SIZE - sizeof(address_t);
    all_threads[current_thread_id].status = STATUS_READY;
    all_threads[current_thread_id].args = args;
    all_threads[current_thread_id].f_ptr = f;

    num_created_threads++;
    num_active_threads++;
    total_tickets += tickets;

    // Set this point as the start point. When the scheduler selects this thread for the first time, this is going to be
    // the place to start
    sigsetjmp(all_threads[current_thread_id].jmp_state, 0);

    (all_threads[current_thread_id].jmp_state->__jmpbuf)[JB_SP] = translate_address(all_threads[current_thread_id].sp);
    (all_threads[current_thread_id].jmp_state->__jmpbuf)[JB_PC] = translate_address(all_threads[current_thread_id].pc);

    return OK;
}

void yield_cpu()
{
    if (scheduler_mode == PREEMPTIVE) {
        printf("This function should not be called when running in PREEMPTIVE mode\n");
    } else {
        schedule();
    }
}

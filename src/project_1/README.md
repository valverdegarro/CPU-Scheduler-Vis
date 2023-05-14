
# Thread Lottery Scheduler

**Authors**: Gutiérrez Quirós Gerardo, Morera Madriz Brian, Ortega Acuña Esteban, Daniel Piedra, David Valverde Garro

This program demonstrates the behavior of a basic lottery scheduler that works on threads. Each thread approximates the value of Pi using Gregory's series up to the number of term defined by the user. The current value of Pi and the progress (%) made by each thread can be viewed underneath the inputs. We support the creation of up to 40 threads.

## How to compile
In the same directory as the included Makefile, run:

    make

## How to run the program

    ./project.out

## GUI Inputs

- **Modo de operación**: <u>Expropiative</u> mode uses the quantum value to run a timer which periodically preempts the CPU from the currently running thread. <u>Non-expropiative</u> mode indicates that the threads must volutarily yield the CPU every certain percentage of terms, this value is given by the user.

- **Número de threads**: how many threads the scheduler will create and manage (Max. 40).

- **Quantum**: in expropiative mode this defines for how many milliseconds a thread will be allowed to run before being preempted. In non-expropiative mode the value in this field is taken as a percentage between 1 and 100 to indicate how often a thread must yield.

- **Iniciar**: runs the lottery scheduler with the currently inputted values. Input fields will be disabled during this time.

- **Reiniciar**: re-enables the input fields and does some internal cleanup to prepare for another scheduler run. The results from the previous run won't disappear until the scheduler is run again.

For each thread:

- **Tiquetes**: The number of lottery tickets assigned to this thread. A thread with more tickets will be selected to run more often (Max. 100).

- **Carga**: The number of work units the thread will calculate (Max. 9999999). Internally this value is multiplied by 50 to determine the number of terms of Gregory's series to calculate (e.g. 100 work units = 5000 terms).



## Limitations and other comments

- We consider that the entirety of the requested functionality has been implemented as specified in the project description. Our program is able to simulate the expected behavior of a lottery scheduler (e.g. threads with little workload and plenty of tickets will finish faster than others and viceversa). 

- The GUI is capable of showing the progress of each thread and the behavior of the scheduler as long as sufficiently high workloads are given.

- Both operation modes work as intended and appropiately reflect the quantum or percentage values given by the user. 

- The individual threads are also able to approximate an acceptable amount of digits of Pi given a sufficiently high workload (e.g. millions of terms), this is due to the fact that Gregory's series is known to converge slowly.

- We avoided the use of unnecessary mechanisms such as **forks**, **pthreads**, **GDK threads**, **pipes**, etc. The scheduler works entirely by managing thread control blocks, timer interrupts, and using **sigsetjmp** and **siglongjmp** instructions to save and jump between its own environment and that of each thread that it manages.

- We also ran into an issue where the long-running scheduler operation would freeze the main GUI thread. This was solved by modifying the scheduler to be **non-blocking**, this means that the scheduler operation is divided into iterative steps that can be advanced by repeatedly calling the `start_scheduler` function in a loop. This allows us to process any UI update operations between scheduler calls.


## Summary of the modules:

- **types.h**: defines some data structures and constants shared between modules.

- **main.c**: main function which invokes the neccesary steps to initialize and display the GUI.

- **ui.c**: defines callbacks and initialization functions for the main GUI window and its controls (i.e. mode, number of threads, and quantum).

- **ui_grid.c**: defines the callbacks and functions needed to manage the grid of threads. This grid changes dynamically based on the inputted number of threads.

- **console.c**: auxiliary module for printing UI changes to the standard output for debugging purposes.

- **scheduler.c**: defines all the functionality needed to create threads and manage them with lottery scheduling. This module also defines the thread control block structure (`thread_t`) which saves all the relevant info about a thread, including its current environment.

- **s_control.c**: manages the communication between UI and scheduler modules (e.g. scheduler and thread configuration, UI updates, etc.).

- **pi_approximator.c**: defines the iterative function that approximates Pi using Gregory's series.
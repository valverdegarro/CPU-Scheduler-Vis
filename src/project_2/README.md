
# Shared memory: Producer - Consumer

**Authors**: Gutiérrez Quirós Gerardo, Morera Madriz Brian, Ortega Acuña Esteban, Daniel Piedra, David Valverde Garro

This project seeks to demonstrate the behavior of a scenario where multiple programs try to access a shared resource to perform different types of actions with it. Each program wants to take control of the shared resource, but it can only be accessed by one program at a time. Therefore, the use of semaphores is applied to control whether the shared resource is available or not for use. This shared resource is defined by a program that will be responsible for saving relevant information about the status of this shared resource, as well as the number of programs of each type trying to use it. Additionally, there will be a program that will be responsible for ending all programs that are trying to use the shared resource so that when it runs, there should be no one else trying to take control, except for the initial program that defined the resource.


## How to compile
In the same directory as the included Makefile, run:

    make

## How to run the program

- **Creator**
    ./creator.out [*name of the buffer*] [*number of messages*]

- **Producer**
    ./producer.out [*name of the buffer*] [*mean time to write a message inside the buffer*]

- **Consumer**
    ./producer.out [*name of the buffer*] [*mean time to read a message inside the buffer*]

- **Finalizer**
    ./finalizer.out [*name of the buffer*]


## Inputs

- **number of messages**: This value indicates the number of messages that can be saved inside the *buffer* and also works as the max amount of message can be contain in the buffer.

- **mean time**: This value indicates the average time in seconds, which follows an exponential distribution, that will represent the amount of time to wait to perform an action on the *buffer*, whether it is to write a message to the *buffer* or to read a message from it.

- **name of the buffer**: This value indicates the name of the buffer, which has a different meaning for each program. In the case of a *producer* or *consumer*, it represents the name of the shared resource from which they will respectively write and read messages, while for the *creator*, it represents the name which the *buffer* will be defined.

## Limitations and other comments

- We consider that the entirety of the requested functionality has been implemented as specified in the project description. Our program is able to simulate the expected behavior of a shared memory: Producer - Consumer. 

- The GUI is capable of showing how many producers and consumers are currently trying to use the buffer. It also shows the requested log, which displays the date and time of a buffer usage action. Finally, it graphically represents the current content of the buffer, the messages inside it, which message is the first and which is the last. Regarding the graphical representation of the buffer, it is considered a visual improvement to show in a more friendly and graphical way, using GTK components, the representation of the buffer's content.

- In regards to the inputs, a proper validation of the number of parameters is performed, as well as the validation of the data types of the entered parameters to ensure that the program is as robust as possible and to avoid any type of error.

- The display of information, requested for each program, was applied as indicated in the statement. Showing the information in a standard way, so that it is easy to interpret and there are not many changes between the programs that make it difficult to read and understand.

## Summary of the modules:

- **sync**
    - **cbuffer.c**: defines functions to get metada information about the *buffer* and init/construct functions for the *buffer* (shared memory resource).
    - **events.c**: defines functions management everything related with *events* (an event is any action related with the *buffer*, can be to write or read, even wait).
    - **sutils.c**: defines some utility functions to manage stuff realted with time format and ENUMs values.
    - **sync.c**: defines all the functionality needed to define the buffer and manage who can get control of the buffer to make an operation. This module defines the *semaphores* to manage when a *PROCUDER* can write a message on the *buffer* or when a *CONSUMER* can read a message from the *buffer*. Also, all the relevant info about the *buffer*, the *semaphores*, and how you can get access of the *buffer* depends on the kind of process that request access, is manage here.
    - **types.h**: defines some data structures and constants shared between modules.

- **utils**
    - **parser.c**: defines functions to read and parser parameters from the console.
    - **utils.c**: defines general functions to manage stuff like random choose or get the current timestamp.

- **consumer**
    - **main.c**: main module to define and manage all the funcionality related with the *consumer*.

- **creator**
    - **main.c**: main module to define and manage all the funcionality related with the *consumer*.
    - **ui.c**:  main function which invokes the neccesary steps to initialize and display the GUI.
    - **paramter_parser.h**: secondary module to read and parse the paramters from the console, related with the parameters to initialize the creator.

- **finalizer**
    - **main.c**: main module to define and manage all the funcionality related with the *finalizer*.

- **producer**
    - **main.c**: main module to define and manage all the funcionality related with the *producer*.

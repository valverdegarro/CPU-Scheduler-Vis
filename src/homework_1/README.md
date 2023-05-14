
# Systrace: System call trace

## Gutiérrez Quirós Gerardo, Morera Madriz Brian, Ortega Acuña Esteban, Daniel Piedra, David Valverde Garro

Receive the name of a **program** to run and track the system calls used during it's execution. Displays a table containing  a summary of the system calls used by the **program** that is passed by argument and the count per each system call used.
### How to compile

    make

### How to run the program

    ./systrace.out {-v | -V} command [args]

### Arguments
- **-v**: Displays one line per system call used by the program. Contains the name of the system call and it's corresponding return value. 
- **-V**: Displays the same information as '-v' but for each system call expects user to press enter to continue the printing.
- **-prog**: Name of the program to trace it's systemcalls.

Example to trace the _ls_ command

    ./systrace -v ls .

Output:

    execve = -2
    execve = -2
    execve = -2
    execve = 0
    brk = 94761415249920
    arch_prctl = -22
    mmap = 140497519087616
    access = -2
    openat = 3
    newfstatat = 0
    mmap = 140497519017984
    close = 0
    openat = 3
    read = 832
    newfstatat = 0
    mmap = 140497518837760
    mprotect = 0
    mmap = 140497518862336
    mmap = 140497518968832
    mmap = 140497519001600
    mmap = 140497519009792
    close = 0
    openat = 3
    read = 832
    pread64 = 784
    pread64 = 48
    pread64 = 68
    newfstatat = 0
    pread64 = 784
    mmap = 140497515446272
    mmap = 140497515610112
    mmap = 140497517268992
    mmap = 140497517629440
    mmap = 140497517654016
    close = 0
    openat = 3
    read = 832
    newfstatat = 0
    mmap = 140497518219264
    mmap = 140497518227456
    mmap = 140497518665728
    mmap = 140497518829568
    close = 0
    mmap = 140497518206976
    arch_prctl = 0
    set_tid_address = 33559
    set_robust_list = 0
    rseq = 0
    mprotect = 0
    mprotect = 0
    mprotect = 0
    mprotect = 0
    mprotect = 0
    prlimit64 = 0
    munmap = 0
    statfs = -2
    statfs = -2
    getrandom = 8
    brk = 94761415249920
    brk = 94761415385088
    openat = 3
    newfstatat = 0
    read = 407
    read = 0
    close = 0
    access = -2
    openat = 3
    newfstatat = 0
    mmap = 140497500766208
    close = 0
    ioctl = 0
    ioctl = 0
    statx = 0
    openat = 3
    newfstatat = 0
    getdents64 = 192
    getdents64 = 0
    close = 0
    newfstatat = 0
    Makefile  README.md  src  systrace.out  test
    write = 44
    close = 0
    close = 0
    ---------------------------------
    Count | Syscall
    ---------------------------------
    5       read
    1       write
    9       close
    18      mmap
    6       mprotect
    1       munmap
    3       brk
    2       ioctl
    4       pread64
    2       access
    4       execve
    2       statfs
    2       arch_prctl
    2       getdents64
    1       set_tid_address
    1       exit_group
    7       openat
    8       newfstatat
    1       set_robust_list
    1       prlimit64
    1       getrandom
    1       statx
    1       rseq

### Not working features and special comments

The requested functionality worked as expected. The program track the system calls used by the process passed as argument and prints the summary table with the system call name and its usage counter.
The **'-v'** and **'-V'** works as expected but we were only able to get the name and the return value of the system call.
For the translation of the syscall id to it's corresponding name, we handle two options during implementation stage but encountered some problems and decided to opt for a third option that is described below.

- A: The first option was to use the a table called **syscalls** in the library **<sys/syscall.h>**, in theory inside tihs library there's a structure that is the table that have map the *PID* with the *syscall name* and other information. The advantage of this structure is that this one has different definitions that change based on the architecture on the device.
- B : The second option consisted in get the *syscall name* from **ptrace**, using the enum option *PTRACE_GET_SYSCALL_INFO* that have information about the system call that is given in the paramaters. Inside the information that is returned by the function, inside it's supposed to have the *syscall name*, but you have to search inside that data as buffer, you need to right place to obtain the information but the problem is that the position isn't easy to identify. If you don't set the right value you will get a **NULL** value or a **Segmentation fault** error.
- **(Used)** To use an static table with the translation of the system call id and name for using a Linux table system call based on the architecture

  - Architecture → x86_64
  - Linux OS → Linux Ubuntu 20.04.5 LTS
  
- We were only able to get the values in the registers used to pass syscall arguments, however these are often pointers to unknown data types. This means that printing the full value of the arguments requires a lot of additional condition checking for each syscall such as number of parameters, types, etc. Since the raw register values add little information and a lot of clutter on screen, we decided not to print them.

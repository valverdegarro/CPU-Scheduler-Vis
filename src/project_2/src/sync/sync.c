#include "sync.h"
#include "cbuffer.h"
#include "events.h"
#include "sutils.h"
#include "types.h"

#include <fcntl.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* Private structs */

typedef struct shared_v {
  int buff_size;

  // Producer and consumer counts

  int n_prod;
  int n_cons;

  bool is_halted;
  bool is_new_event;

  cb_metadata cb_metadata;

} shared_v;

/* Shared semaphore names */

#define SEM_PROC_CTR_NAME "SEM_PROC_CTR"

#define SEM_HALT_MUT_NAME "SEM_HALT_MUT"
#define SEM_ZERO_PROD_NAME "SEM_ZERO_PROD"
#define SEM_ZERO_CONS_NAME "SEM_ZERO_CONS"
#define SEM_ZERO_MSG_NAME "SEM_ZERO_MSG"

#define SEM_CBUF_NAME "SEM_CBUF"
#define SEM_BFF_FREE_NAME "SEM_BFF_FREE"
#define SEM_BFF_USED_NAME "SEM_BFF_USED"

/* Shared semaphores */
static sem_t *proc_counter_mut;

static sem_t *halt_mut;
static sem_t *zero_producers;
static sem_t *zero_consumers;
static sem_t *zero_messages;

static sem_t *cbuffer_mut;
static sem_t *buff_used;
static sem_t *buff_free;

/* Shared variables */

static char *shared_mem = NULL; // this is the one that must be freed

static shared_v *sh_vars = NULL;

static event_t *shared_event = NULL;

static buff_msg *buffer = NULL;

static c_buffer *cbuffer = NULL; // Non mmaped struct

/* Private variables */
/* These are local to each process and don't require semaphores.*/

// Buffer size
int g_buff_sz = -1;

// Time spent waiting on semaphores (microseconds)
int sem_wait_usec = 0;

/* Private functions */

int get_mem_size(int buff_size) {
  return sizeof(shared_v) + sizeof(event_t) + get_buff_bytes(buff_size);
}

void reset_shr_mem() {
  sh_vars->buff_size = g_buff_sz;

  sh_vars->n_prod = 0;
  sh_vars->n_cons = 0;

  sh_vars->is_halted = false;
  sh_vars->is_new_event = false;

  cbuffer = init_cbuffer(&sh_vars->cb_metadata, buffer, g_buff_sz);
}

int read_buff_size(char *name) {
  int size;
  int fd = open(name, O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

  pread(fd, &size, sizeof(int), 0);

  close(fd);

  return size;
}

int init_mmap(char *name, int buff_size) {
  size_t sh_mem_size = get_mem_size(buff_size);

  // Create the file that underlies the shared memory
  int fd = open(name, O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH);

  ftruncate(fd, sh_mem_size);

  // Map file to memory using mmap, this returns a pointer to the starting
  // location
  // of the allocated memory
  shared_mem = (char *)mmap(NULL, sh_mem_size, PROT_READ | PROT_WRITE,
                            MAP_SHARED, fd, 0);

  if (shared_mem == MAP_FAILED) {
    printf("Mapping Failed\n");
    return FAIL;
  }

  // Set pointers to shared data
  char *temp_ptr = shared_mem;
  sh_vars = (shared_v *)temp_ptr;
  temp_ptr += sizeof(shared_v);
  shared_event = (event_t *)temp_ptr;
  temp_ptr += sizeof(event_t);
  buffer = (buff_msg *)temp_ptr;

  return OK;
}

int unlink_semaphores() {
  sem_unlink(SEM_PROC_CTR_NAME);

	sem_unlink(SEM_HALT_MUT_NAME);
	sem_unlink(SEM_ZERO_PROD_NAME);
	sem_unlink(SEM_ZERO_CONS_NAME);
	sem_unlink(SEM_ZERO_MSG_NAME);

  sem_unlink(SEM_CBUF_NAME);
  sem_unlink(SEM_BFF_USED_NAME);
  sem_unlink(SEM_BFF_FREE_NAME);

  return OK;
}

int init_semaphores() {
  // Unlink the semaphores first to avoid reusing old ones
  unlink_semaphores();

  // Now create them again
  proc_counter_mut = sem_open(SEM_PROC_CTR_NAME, O_CREAT, 0644, 1);

	halt_mut = sem_open(SEM_HALT_MUT_NAME, O_CREAT, 0644, 1);
	zero_producers = sem_open(SEM_ZERO_PROD_NAME, O_CREAT, 0644, 0);
	zero_consumers = sem_open(SEM_ZERO_CONS_NAME, O_CREAT, 0644, 0);
	zero_messages = sem_open(SEM_ZERO_MSG_NAME, O_CREAT, 0644, 0);

  cbuffer_mut = sem_open(SEM_CBUF_NAME, O_CREAT, 0644, 1);
  buff_used = sem_open(SEM_BFF_USED_NAME, O_CREAT, 0644, 0);
  buff_free = sem_open(SEM_BFF_FREE_NAME, O_CREAT, 0644, g_buff_sz);

  return OK;
}

int attach_semaphores() {
	proc_counter_mut = sem_open(SEM_PROC_CTR_NAME, 0);
	halt_mut = sem_open(SEM_HALT_MUT_NAME, 0);
	zero_producers = sem_open(SEM_ZERO_PROD_NAME, 0);
	zero_consumers = sem_open(SEM_ZERO_CONS_NAME, 0);
	zero_messages = sem_open(SEM_ZERO_MSG_NAME, 0);

  // Buffer semaphores
  cbuffer_mut = sem_open(SEM_CBUF_NAME, 0);
  buff_used = sem_open(SEM_BFF_USED_NAME, 0);
  buff_free = sem_open(SEM_BFF_FREE_NAME, 0);

  return OK;
}

// Returns the time spent waiting for semaphores (microseconds)
int get_sem_wait_time() {
  // Time accumulated in sync semaphores + event semaphores
  return sem_wait_usec + get_esem_wait_time();
}


int wait_zero_sem(sem_t *sem) {
	int status;

	status = timed_sem_wait(sem, &sem_wait_usec);
	if (status != 0) {
		printf("[wait_zero_sem] failed waiting on zero_* semaphore\n");
		return FAIL;
	}

  return OK;
}

int write_halt(bool value) {
  int status;
  status = timed_sem_wait(halt_mut, &sem_wait_usec);
  if (status != 0) {
    printf("[write_halt] failed waiting on halt_mut\n");
    return FAIL;
  }

  sh_vars->is_halted = value;

  status = sem_post(halt_mut);
  if (status != 0) {
    printf("[write_halt] failed posting on halt_mut\n");
    return FAIL;
  }
  return OK;
}

int read_proc_counter(proc_t proc_type, int *val) {
  int status;
  int *counter;

  // Get the counter based on the type
  if (proc_type == CONSUMER) {
    counter = &sh_vars->n_cons;
  } else if (proc_type == PRODUCER) {
    counter = &sh_vars->n_prod;
  } else {
    // Maybe the finalizer will have a registration variable in the future
    return OK;
  }

  status = timed_sem_wait(proc_counter_mut, &sem_wait_usec);
  if (status != 0) {
    printf("[register_process] failed waiting on proc_counter_mut\n");
    return FAIL;
  }

  // Increase counter
  *val = *counter;

  status = sem_post(proc_counter_mut);
  if (status != 0) {
    printf("[register_process] failed posting on proc_counter_mut\n");
    return FAIL;
  }

  return OK;
}

int write_proc_counter(proc_t proc_type, bool increase) {
  int status;
  int *counter;

  // Increase the counter based on the type
  if (proc_type == CONSUMER) {
    counter = &sh_vars->n_cons;
  } else if (proc_type == PRODUCER) {
    counter = &sh_vars->n_prod;
  } else {
    // Maybe the finalizer will have a registration variable in the future
    return OK;
  }

  status = timed_sem_wait(proc_counter_mut, &sem_wait_usec);
  if (status != 0) {
    printf("[register_process] failed waiting on proc_counter_mut\n");
    return FAIL;
  }

  // Increase/decrease counter
  if (increase == true) {
    *counter = *counter + 1;
  } else {
    *counter = *counter - 1;
  }

  status = sem_post(proc_counter_mut);
  if (status != 0) {
    printf("[register_process] failed posting on proc_counter_mut\n");
    return FAIL;
  }

  return OK;
}

int register_process(proc_t proc_type) {
  return write_proc_counter(proc_type, true);
}

int unregister_process(proc_t proc_type) {
  return write_proc_counter(proc_type, false);
}

// IMPORTANT: Only call this if you have the mutex on cbuffer_mut.
int build_rw_event(proc_t proc_type, int modified_idx, event_t *event) {
  event->proc_type = proc_type;
  event->pid = getpid();
  event->last = false; // Only the finalizer sets this to true

  read_producer_count(&event->prod_count);
  read_consumer_count(&event->cons_count);

  get_halted(&event->is_halted);

  event->modified_idx = modified_idx;
  event->buff_entry = get_tail(cbuffer);
  event->buff_exit = get_head(cbuffer);
  event->buff_count = get_count(cbuffer);

  return OK;
}

/* Public functions */

int read_producer_count(int *val) { return read_proc_counter(PRODUCER, val); }

int read_consumer_count(int *val) { return read_proc_counter(CONSUMER, val); }

int get_halted(bool *value) {
  int status;
  status = timed_sem_wait(halt_mut, &sem_wait_usec);
  if (status != 0) {
    printf("[is_halted] failed waiting on halt_mut\n");
    return FAIL;
  }

  *value = sh_vars->is_halted;

  status = sem_post(halt_mut);
  if (status != 0) {
    printf("[is_halted] failed posting on halt_mut\n");
    return FAIL;
  }
  return OK;
}

// Only call this if you're the Creator
int init_shr_mem(char *name, int buff_size) {
  int status;
  g_buff_sz = buff_size;

  status = init_mmap(name, buff_size);
  if (status != OK) {
    return status;
  }

  reset_shr_mem();

  // init semaphores
  status = init_events(shared_event, buffer);
  if (status != OK) {
    return status;
  }

  status = init_semaphores();
  if (status != OK) {
    return status;
  }

  // Set halt variable to false
  status = write_halt(false);
  if (status != OK) {
    return status;
  }

  return OK;
}

int free_shr_mem() { return OK; }

// Call this if you're a Producer, Consumer, or Finalizer
int connect_shr_mem(char *name, proc_t proc_type) {
  int status;
  bool halted = false;

	if (access(name, F_OK) == 0) {
    	// file exists
		printf("connecting to existing MMAP file\n");
	} else {
		printf("MMAP file \"%s\" does not exist\n", name);
		return FAIL;
	}

	status = attach_semaphores();
	if (status != OK) {
		return status;
	}

  g_buff_sz = read_buff_size(name);

  status = init_mmap(name, g_buff_sz);
  if (status != OK) {
    return status;
  }

  cbuffer = construct_cbuffer(&sh_vars->cb_metadata, buffer);

  status = attach_events(shared_event, buffer);
  if (status != OK) {
    return status;
  }

  // If halt is set, do not accept more processes of any type
  status = get_halted(&halted);
  if (status != OK) {
    return status;
  }
  if (halted) {
    printf("cannot connect to memory because system is halted");
    return FAIL;
  }

  register_process(proc_type);

  return OK;
}

int disconnect_shr_mem(proc_t proc_type) {
  int status = OK;
  bool halted;
  int prod_ctr;
  int cons_ctr;

  status = unregister_process(proc_type);

  status = get_halted(&halted);
  if (status != OK) {
    return status;
  }

	// update the zero prod counter to notify the finalize function that there are not more producers running
	if (proc_type == PRODUCER && halted) {
		status = read_proc_counter(PRODUCER, &prod_ctr);
		if (status != OK) {
			return status;
		}
		if (prod_ctr == 0) {
			status = sem_post(zero_producers);
			if (status != 0) {
				printf("[disconnect_shr] failed posting on zero_producers\n");
				return FAIL;
			}
		}
	} else if (proc_type == CONSUMER && halted) {
		status = read_proc_counter(CONSUMER, &cons_ctr);
		if (status != OK) {
			return status;
		}
		if (cons_ctr == 0) {
			status = sem_post(zero_messages);
			if (status != 0) {
				printf("[disconnect_shr] failed posting on zero_messages\n");
				return FAIL;
			}

			status = sem_post(zero_consumers);
			if (status != 0) {
				printf("[disconnect_shr] failed posting on zero_consumers\n");
				return FAIL;
			}
		}
	}

  return status;
}

int write_buff_msg(proc_t proc_type, buff_msg *msg, int *modified_idx){
	int status;
	bool halted;

  // Entry protocol
  status = timed_sem_wait(buff_free, &sem_wait_usec);

  if (status != 0) {
    printf("[write_buff_msg] failed waiting on buff_free \n");
    return FAIL;
  }

	// Producers can't write any more messages once finalizer toggles is_halted
	get_halted(&halted);
	if (halted && proc_type == PRODUCER) {
		return FAIL;
	}

  if (status != 0) {
    printf("[write_buff_msg] failed waiting on cbuffer_mut \n");
    return FAIL;
  }

  // Critical region
  *modified_idx = get_tail(cbuffer);
  put_msg(cbuffer, msg);

  event_t event;
  build_rw_event(proc_type, *modified_idx, &event);

  status = send_event(&event);

  // Exit protocol
  status = sem_post(cbuffer_mut);
  if (status != 0) {
    printf("[write_buff_msg] failed waiting cbuffer_mut \n");
    return FAIL;
  }

  status = sem_post(buff_used);
  if (status != 0) {
    printf("[write_buff_msg] failed waiting buff_used \n");
    return FAIL;
  }

  return OK;
}

int read_buff_msg(buff_msg *msg, int *modified_idx) {
  int status;

  // Entry protocol
  status = timed_sem_wait(buff_used, &sem_wait_usec);

  if (status != 0) {
    printf("[write_buff_msg] failed waiting on buff_used \n");
    return FAIL;
  }

  status = timed_sem_wait(cbuffer_mut, &sem_wait_usec);

  if (status != 0) {
    printf("[write_buff_msg] failed waiting on cbuffer_mut \n");
    return FAIL;
  }

  // Critical region
  *modified_idx = get_head(cbuffer);
  get_msg(cbuffer, msg);

  event_t event;
  build_rw_event(CONSUMER, *modified_idx, &event);

  status = send_event(&event);

  bool halted;
  status = get_halted(&halted);
  if (status != OK) {
    return status;
  }

  if (is_buff_empty(cbuffer) && halted) {
    sem_post(zero_messages);
  }

  // Exit protocol
  status = sem_post(cbuffer_mut);
  if (status != 0) {
    printf("[write_buff_msg] failed waiting cbuffer_mut \n");
    return FAIL;
  }

  status = sem_post(buff_free);
  if (status != 0) {
    printf("[write_buff_msg] failed waiting buff_free \n");
    return FAIL;
  }

  return OK;
}

int get_buff_entry(int *value) {

  timed_sem_wait(cbuffer_mut, &sem_wait_usec);
  *value = get_tail(cbuffer);
  sem_post(cbuffer_mut);

  return OK;
}

int get_buff_exit(int *value) {

  timed_sem_wait(cbuffer_mut, &sem_wait_usec);
  *value = get_head(cbuffer);
  sem_post(cbuffer_mut);

  return OK;
}

int finalize() {
	/*
	 * There are six steps to finalize.
	 * 1- Set is_halted to true
	 * 
	 * If there are any producers:
	 *     2- wait until producers are killed
	 * 
	 * If there are any consumers:
	 *     3- wait until remaining messages are consumed
	 *     4- send finalize messages until there are no more producers alive
	 *     5- wait until remaining consumers are killed
	 * 
	 * 6- notify creator of last event
	*/

	int status, prod_ctr, cons_ctr, index;
	buff_msg finalize_msg;
	event_t last_event;


	// Step 1: set halt flag to true
	printf("[Finalize] Raising halt flag to indicate system stop \n\n");

	status = write_halt(true); // tell all producers to stop
	if (status != OK) {
		return status;
	}

	// Only execute producer-related steps if there are any producers to kill,
	// otherwise the finalizer will get stuck
	status = read_proc_counter(PRODUCER, &prod_ctr);
	if (status != OK) {
		return status;
	}

	if (prod_ctr > 0) {
		for (int i = 0; i < prod_ctr; i++) {
			sem_post(buff_free); // Post semaphore to ensure producers can finalize when buffer is full
		}


		// Step 2: wait until producers are killed
		printf("[Finalize] Waiting until all producers have stopped...\n");

		status = wait_zero_sem(zero_producers); // wait until all producers are stopped
		if (status != OK) {
			return status;
		}

		printf("[Finalize] All producers have stopped.\n\n");

	} else {
		printf("[Finalize] Found no producers to kill. Skipping ahead...\n\n");
	}


	// Only execute consumer-related steps if there are any consumers to kill,
	// otherwise the finalizer will get stuck
	status = read_proc_counter(CONSUMER, &cons_ctr);
	if (status != OK) {
		return status;
	}

	if (cons_ctr > 0) {
		// Step 2: wait until remaining messages are consumed
		/*printf("[Finalize] Waiting until all messages in the buffer are consumed...\n\n");
		status = wait_zero_sem(zero_messages);
		if (status != OK) {
			return status;
		}*/

		// Step 3: send finalize messages
		finalize_msg.op_code = HALT;
		gettimeofday(&finalize_msg.ts, NULL);
		finalize_msg.proc_type = FINALIZER;
		finalize_msg.key = 0;

		printf("[Finalize] Sending at least %d HALT messages to kill remaining consumers...\n\n", cons_ctr);

		for(int i = 0; i < cons_ctr; i++) {
			//printf("\tWriting HALT message %d ...\n", i);
			write_buff_msg(FINALIZER, &finalize_msg, &index);
			//printf("\tDone! Message was written to buffer position %d\n\n", index);
			print_msg(&finalize_msg, index, true);
		}


		// Step 4: Wait until all consumers are killed
		printf("[Finalize] Waiting until all consumers have stopped...\n");
		status = wait_zero_sem(zero_consumers); // wait until all consumers are stopped
		if (status != OK) {
			return status;
		}
		printf("[Finalize] All consumers have stopped.\n\n");

	} else {
		printf("[Finalize] Found no consumers to kill. Skipping ahead...\n\n");
	}


	printf("[Finalize] Notifying creator of system finalization...\n\n");

	// Step 5: Send last event to notify the creator that were are done
  build_rw_event(FINALIZER, 0, &last_event);
	last_event.last = true;

	status = send_event(&last_event);
	if (status != OK) {
		return status;
	}

	printf("[Finalize] Successfully killed all producers and consumers.\n\n");

	return OK;
}

void print_msg(buff_msg *msg, int index, bool is_sender) {
	int status, consumers, producers;
  char *action = "Wrote to";

  if (!is_sender) {
    action = "Read from";
  }

	status = read_producer_count(&producers);
	if (status != 0) {
		printf("Error reading the producer counter\n");
	}

	status = read_consumer_count(&consumers);
	if (status != 0) {
		printf("Error reading the consumer counter\n");
	}

  printf("%s buffer at index [%d].\t#Producers alive: %d\t#Consumers alive: %d\n\n", action, index, producers, consumers);
}

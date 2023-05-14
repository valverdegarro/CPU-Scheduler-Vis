#include <semaphore.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include "types.h"

/* Performs a sem_wait on sem and adds the elapsed time (microseconds) into
 * time_sum. */
int timed_sem_wait(sem_t *sem, int *time_sum) {
  struct timeval st, et;
  int status;
  int elapsed;

  gettimeofday(&st, NULL);
  status = sem_wait(sem);
  gettimeofday(&et, NULL);

  // Calculate elapsed microseconds
  elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);

  *time_sum += elapsed;

  return status;
}


char *proc_type_to_string(proc_t ptype) {
	switch (ptype) {
		case CREATOR:
			return "CREATOR";
		case PRODUCER:
			return "PRODUCER";
		case CONSUMER:
			return "CONSUMER";
		case FINALIZER:
			return "FINALIZER";
		default:
			return "NOT VALID PROC_T";
	}
}


char *op_code_to_str(msg_code code){
	switch(code) {
		case NORMAL:
			return "NORMAL";
		case HALT:
			return "HALT";
		default:
			return "NOT A VALID MSG_CODE";
	}
}


void print_timeval(struct timeval tv, char *str, int size) {
  time_t nowtime;
  struct tm *nowtm;

  nowtime = tv.tv_sec;
  nowtm = localtime(&nowtime);
  strftime(str, size, "%Y-%m-%d %H:%M:%S", nowtm);
}
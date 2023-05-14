#include "../sync/sync.h"
#include <stdio.h>

#include "../utils/parser.h"

void print_finalizer_stats(int pid, int acc_time_blocked, int prod_count, int cons_count) {
printf("\
+--------------------------------------------------------------------------------------+\n\
|                                      FINAL STATS                                     |\n\
+--------+-------------------------------+----------------------+----------------------+\n\
| PID:   | Time waiting semaphores (us)  | Producers terminated | Consumers terminated |\n\
+--------------------------------------------------------------------------------------+\n\
  %d               %d                        %d                     %d\n\n", pid, acc_time_blocked, prod_count, cons_count);
}

int main(int argc, char *argv[]) {
	int status;
	int prod_count;
	int cons_count;
	int my_pid = getpid();

	char *buffer_name;

	proc_t proc_type = FINALIZER;
	
	status = read_finalizer_params(argc, argv, &buffer_name);

  	printf("Starting finalizer\n");

	status = connect_shr_mem(buffer_name, proc_type);
	if (status != OK) {
		printf("FAILED %d\n", status);
	}

	// Read producer and consumer counts before calling finalize()
	status = read_producer_count(&prod_count);
	if (status != OK) {
		printf("FAILED %d\n", status);
	}

	status = read_consumer_count(&cons_count);
	if (status != OK) {
		printf("FAILED %d\n", status);
	}

	printf("Initiating finalize procedure...\n");

	status = finalize();
	if (status != OK) {
		printf("error finishing system\n");
	}

	print_finalizer_stats(my_pid, get_sem_wait_time(), prod_count, cons_count);

	printf("Exiting finalizer...\n\n");

}

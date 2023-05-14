#include "../sync/sync.h"
#include <stdio.h>
#include <stdlib.h>
#include "../sync/sync.h"
#include "../utils/utils.c"
#include "../utils/parser.c"

int main(int argc, char *argv[]) {
	bool read_msg = true;
	int status;
	char *buffer_name;
	float mean;
	int num_msgs = 0;
	int acc_time_waiting = 0;
	int my_pid = getpid();

	// Read the parameters
	status = read_parameters(argc, argv, &buffer_name, &mean);
	if (status != OK) {
		printf("Error reading command line parameters\n\n");
		exit(-1);
	}

	// Connect to memory
	status = connect_shr_mem(buffer_name, CONSUMER);
	if (status != OK) {
		exit(-1);
	}
	
	buff_msg *msg = (buff_msg *)malloc(sizeof(buff_msg));
	int index;

	printf("PID: %d\n\n", my_pid);

	do {
		double time_s = sample_exp_d(mean);
		int time_us = (int)(time_s * 1000000);
		msg-> key = random_num();

		printf("\nNext read in: %.2f s\n", time_s);
		usleep(time_us);
		acc_time_waiting += time_us;

		// Read the message from the buffer
		printf("Writing...\n");
		read_buff_msg(msg, &index);
		num_msgs++;

		print_msg(msg, index, false);

		if((my_pid % 100) == msg->key) {
			printf("Received a message with a key that matches pid %% 100. Terminating...\n");
			read_msg = false;
		}

		if (msg->op_code == HALT) {
			printf("Received a message from the Finalizer. Terminating...");
			read_msg = false;
		}


	} while(read_msg);

	print_stats(my_pid, num_msgs, acc_time_waiting, get_sem_wait_time());

	// Disconnect from the memory
	status = disconnect_shr_mem(CONSUMER);
	if (status != OK) {
		printf("FAILED disconnecting from memory %d\n", status);
	}
	
}

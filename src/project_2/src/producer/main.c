#include "../sync/sync.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/time.h>

#include "../sync/sync.h"
#include "../utils/parser.h"
#include "../utils/utils.h"

int main(int argc, char *argv[]) {

	int status;
	struct timeval tv;
	pid_t pid; 
	float mean;
	char *buffer_name;
	int num_msgs = 0;
	int acc_time_waiting = 0;


	pid = getpid();

	status = read_parameters(argc, argv, &buffer_name, &mean);

	if (status != OK) {
		printf("Error reading command line parameters\n\n");
		exit(-1);
	}

	status = connect_shr_mem(buffer_name, PRODUCER);
	if (status != OK) {
		exit(-1);
	}

	if (status != OK) {
		printf("FAILED %d\n", status);
		exit(-1);
	} else {
		buff_msg *msg = (buff_msg*) malloc(sizeof(buff_msg));
		msg -> pid = pid;
		msg -> proc_type = PRODUCER;
		//msg -> ts = tv;
		msg -> op_code = NORMAL;

		int index;
		bool halted;

		printf("PID: %d\n\n", pid);

		do {
			double time_s = sample_exp_d(mean);
			int time_us = (int)(time_s * 1000000);
			msg-> key = random_num();

			printf("\nNext write in: %.2f s\n", time_s);
			usleep(time_us);

			// Set msg timestamp after sleep
			gettimeofday(&tv, NULL);
			msg -> ts = tv;

			acc_time_waiting += time_us;
			
			printf("Writing...\n");
			status = write_buff_msg(PRODUCER, msg, &index);

			if (status == OK) {
				//printf("FAILED writing the message %d\n", status);
				num_msgs++;

				// Print the message
				print_msg(msg, index, true);
			}

			status = get_halted(&halted);
			if (status != OK) {
				printf("FAILED %d\n", status);
				break;
			}

		} while(!halted);

		print_stats(pid, num_msgs, acc_time_waiting, get_sem_wait_time());

		status = disconnect_shr_mem(PRODUCER);
		if (status != OK) {
			printf("FAILED %d\n", status);
		}
	}
}

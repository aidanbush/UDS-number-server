/* Author: Aidan Bush
 * Assign: 3
 * Course: CMPT 360
 * Date: Nov. 17, 18
 * File: num_client.c
 * Description: Client program.
 */

/* standard libraries */
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>

typedef enum {
	OP_NONE = 0,
	OP_ERR,
	OP_STOR,
	OP_RECV,
} operation;

void send_num(car *server, uint64_t num)
{
}

void recv_num(car *server)
{
}

void print_usage(char *p_name)
{
	printf("Usage: %s [options] [num] server\n"
		"Client for sending and recieving numbers from the numbers server.\n\n"
		"Options:\n"
		"    -h display usage message\n"
		"    -s send number\n"
		"    -r receive number\n", basename(p_name));
}

int main(int argc, char **argv)
{
	int c;
	operation op = OP_NONE;

	while ((c = getopt(argc, argv, "h")) != -1) {
		switch (c) {
		case 's':
			if (op != OP_NONE)
				op = OP_ERR;
			else
				op = OP_STOR;
			break;
		case 'r':
			if (op != OP_NONE)
				op = OP_ERR;
			else
				op = OP_RECV;
			break;
		case 'h':
			print_usage(argv[0]);
			return 0;
		default:
			print_usage(argv[0]);
			return 1;
		}
	}

	switch (op) {
		case OP_STOR:
			// send_num
			break;
		case OP_RECV:
			// recv num
			break;
		case OP_NONE:
			printf("Must select one command\n");
			print_usage(argv[0]);
			return 1;
		case OP_ERR:
			printf("Only one command allowed\n");
		default:
			print_usage(argv[0]);
			return 1;
	}

	return 0;
}

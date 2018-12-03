/* Author: Aidan Bush
 * Assign: 3
 * Course: CMPT 360
 * Date: Nov. 17, 18
 * File: num_client.c
 * Description: Client program.
 */

/* standard libraries */
#include <sys/socket.h>
#include <sys/un.h>
#include <libgen.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
	OP_NONE = 0,
	OP_ERR,
	OP_STOR,
	OP_RECV,
} operation;

void send_num(char *server, uint64_t num)
{
	int sfd, rd;
	struct sockaddr_un sock = {0};
	char buf[100];

	if (strlen(server) >= sizeof(sock.sun_path)) {
		return;
	}

	sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sfd == -1) {
		perror("socket");
		return;
	}
	sock.sun_family = AF_UNIX;

	strcpy(sock.sun_path, server);

	if (connect(sfd, (struct sockaddr*)&sock, sizeof(sock)) == -1) {
		perror("connect");
		return;
	}

	printf("server: %s, num: %ld\n", server, num);
	// write msg
	// read msg

	close(sfd);
}

/*
void recv_num(char *server)
{
}
*/

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
	uint64_t num;
	char *server;
	operation op = OP_NONE;

	while ((c = getopt(argc, argv, "s:rh")) != -1) {
		switch (c) {
		case 's':
			num = atoll(optarg);

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

	if (op == OP_STOR || op == OP_RECV) {
		if (optind >= 1) {
			server = argv[optind];
		} else {
			print_usage(argv[0]);
			return 1;
		}
	}

	switch (op) {
		case OP_STOR:
			send_num(server, num);
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

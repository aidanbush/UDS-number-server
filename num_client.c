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

#include "msg.h"

typedef enum {
	OP_NONE = 0,
	OP_ERR,
	OP_STOR,
	OP_RECV,
} operation;

int create_sock(int *sfd, struct sockaddr_un *sock, char *server_name)
{
	if (strlen(server_name) >= sizeof(sock->sun_path)) {
		fprintf(stderr, "server name to long\n");
		return 0;
	}

	*sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (*sfd == -1) {
		perror("socket");
		return 0;
	}
	sock->sun_family = AF_UNIX;

	strcpy(sock->sun_path, server_name);

	if (connect(*sfd, (struct sockaddr*)sock, sizeof(*sock)) == -1) {
		perror("connect");
		return 0;
	}

	return 1;
}

void send_num(char *server, uint64_t num)
{
	int sfd;
	struct sockaddr_un sock = {0};
	packet_s *pkt = NULL;

	if (!create_sock(&sfd, &sock, server))
		return;

	printf("server: %s, num: %ld\n", server, num);
	if (!send_stor_pkt(sfd, num)) {
		fprintf(stderr, "ERROR in sending number %ld\n", num);
	}
	printf("wrote msg\n");

	printf("read msg\n");
	pkt = read_pkt(sfd);
	if (pkt != NULL) {
		printf("got response\n");
		free_pkt(pkt);
	}

	close(sfd);
}

void recv_num(char *server)
{
	//uint64_t num;
	int sfd;
	struct sockaddr_un sock = {0};
	packet_s *pkt = NULL;

	if (!create_sock(&sfd, &sock, server))
		return;

	printf("server: %s\n", server);
	// write msg
	if (!send_rtrv_pkt(sfd)) {
		fprintf(stderr, "ERROR in sending RTRV request\n");
	}
	printf("wrote msg\n");
	// read msg
	printf("read msg\n");
	pkt = read_pkt(sfd);
	if (pkt != NULL) {
		printf("got response\n");
		free_pkt(pkt);
	}

	close(sfd);
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
			break;
		case OP_RECV:
			recv_num(server);
			// set return val
			break;
		case OP_NONE:
			printf("Must select one command\n");
			print_usage(argv[0]);
			return 1;
		case OP_ERR:
			printf("Only one command allowed\n");
			__attribute__ ((fallthrough));
		default:
			print_usage(argv[0]);
			return 1;
	}

	return 0;
}

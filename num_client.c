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

int verbose;

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

	if (connect(*sfd, (struct sockaddr *)sock, sizeof(*sock)) == -1) {
		perror("connect");
		return 0;
	}

	return 1;
}

int send_num(char *server, int64_t num)
{
	int sfd;
	int ret_val = 0;
	struct sockaddr_un sock = {0};
	packet_s *pkt = NULL;

	if (verbose)
		printf("storing %ld\n", num);

	if (!create_sock(&sfd, &sock, server))
		return ret_val;

	if (!send_stor_pkt(sfd, num)) {
		fprintf(stderr, "ERROR in sending number %ld\n", num);
		ret_val = 0;
		goto send_num_return;
	}

	pkt = read_pkt(sfd);
	if (pkt == NULL) {
		fprintf(stderr, "ERROR in reading packet\n");
		ret_val = 0;
		goto send_num_return;
	}

	if (pkt->type == PKT_OP_OK) {
		ret_val = 1;
	} else {
		if (verbose)
			fprintf(stderr, "number not set\n");
		ret_val = 0;
	}

	free_pkt(pkt);

send_num_return:
	close(sfd);
	return ret_val;
}

int recv_num(char *server, int64_t *num)
{
	int sfd;
	int ret_val = 0;
	struct sockaddr_un sock = {0};
	packet_s *pkt = NULL;

	if (!create_sock(&sfd, &sock, server))
		return ret_val;

	if (!send_rtrv_pkt(sfd)) {
		fprintf(stderr, "ERROR in sending RTRV request\n");
		ret_val = 0;
		goto recv_num_return;
	}

	pkt = read_pkt(sfd);
	if (pkt == NULL) {
		fprintf(stderr, "ERROR in reading packet\n");
		ret_val = 0;
		goto recv_num_return;
	}

	if (pkt->type == PKT_OP_RNUM) {
		*num = pkt->num;
		if (verbose)
			printf("retrieved %ld\n", *num);
		ret_val = 1;
	} else {
		fprintf(stderr, "recieved ERR\n");
		ret_val = 0;
	}

	free_pkt(pkt);

recv_num_return:
	close(sfd);
	return ret_val;
}

void print_usage(char *p_name)
{
	printf("Usage: %s [options] server\n"
		"Client for sending and recieving numbers from the numbers server.\n\n"
		"Options:\n"
		"    -h display usage message\n"
		"    -s send number, must be followed by a 64 bit number\n"
		"    -r receive number\n"
		"    -v enable verbose output\n", basename(p_name));
}

int main(int argc, char **argv)
{
	int c;
	int ret_val = 0;
	int64_t num;
	char *server;
	operation op = OP_NONE;

	while ((c = getopt(argc, argv, "s:rhv")) != -1) {
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
		case 'v':
			verbose = 1;
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
		if (optind < argc) {
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
		if (recv_num(server, &num))
			ret_val = num;
		break;
	case OP_NONE:
		printf("You must select one command\n");
		print_usage(argv[0]);
		return 1;
	case OP_ERR:
		printf("Only one command allowed\n");
		// want fallthrough
		__attribute__ ((fallthrough));
	default:
		print_usage(argv[0]);
		return 1;
	}

	return ret_val;
}

/* Author: Aidan Bush
 * Assign: 3
 * Course: CMPT 360
 * Date: Nov. 17, 18
 * File: num_client.c
 * Description: Client program.
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

#include "msg.h"
#include "buf.h"

// socket accept backlog
#define BACKLOG	5

// buffer
buf_s *buf;

// signal flag variables
volatile sig_atomic_t exit_server;
volatile sig_atomic_t print_buffer;

// sigint handler, sets server to exit
void sigint_handler(int par)
{
	exit_server = 1;
}

// sighup handler, sets server to print
void sighup_handler(int par)
{
	print_buffer = 1;
}

// creats the sigint handler
int create_sigint_handler(void)
{
	int err;
	struct sigaction sa = {
		.sa_handler = &sigint_handler,
	};

	sigemptyset(&sa.sa_mask);

	err = sigaction(SIGINT, &sa, NULL);
	if (err == -1)
		perror("sigaction");

	return err != -1;
}

// creats the sighup handler
int create_sighup_handler(void)
{
	int err;
	struct sigaction sa = {
		.sa_handler = sighup_handler,
	};

	sigemptyset(&sa.sa_mask);

	err = sigaction(SIGHUP, &sa, NULL);
	if (err == -1)
		perror("sigaction");

	return err != -1;
}

// create the signal handlers
int create_signal_handlers(void)
{
	if (!create_sigint_handler())
		return 0;

	return create_sighup_handler();
}

// stores the given request to the buffer and sending the appropriate response
int stor_num(int sfd, packet_s *pkt)
{
	if (add_buf(buf, pkt->num)) {
		if (!send_ok_pkt(sfd))
			fprintf(stderr, "error in send OK\n");
	} else {
		if (!send_err_pkt(sfd))
			fprintf(stderr, "error in send ERR\n");
	}
	return 1;
}

// retrieves the number from the buffer and sending the appropriate response
int rtrv_num(int sfd, packet_s *pkt)
{
	if (retrieve_buf(buf, &(pkt->num))) {
		if (!send_num_pkt(sfd, pkt->num))
			fprintf(stderr, "error in send NUM packet\n");
	} else {
		if (!send_err_pkt(sfd))
			fprintf(stderr, "error in send ERR\n");
	}
	return 1;
}

// request handling function
void *handle_req(void *fd)
{
	int cfd = *((int *)(fd));
	packet_s *pkt;

	// read req
	pkt = read_pkt(cfd);
	if (pkt == NULL) {
		fprintf(stderr, "read_pkt error\n");
		goto handle_req_return;
	}

	switch (pkt->type) {
	case PKT_OP_STOR:
		stor_num(cfd, pkt);
		break;
	case PKT_OP_RTRV:
		rtrv_num(cfd, pkt);
		break;
	default:
		fprintf(stderr, "Recieved non-valid request, ignoring\n");
		break;
	}

	free_pkt(pkt);

handle_req_return:
	free(fd);
	close(cfd);
	return NULL;
}

// creates a new handling request thread, and detaches it
int create_req_thread(int cfd)
{
	pthread_t p;
	int *cfd_ptr = malloc(sizeof(int));

	if (cfd_ptr == NULL) {
		perror("malloc");
		return 0;
	}

	*cfd_ptr = cfd;

	if (pthread_create(&p, NULL, handle_req, (void *)cfd_ptr) != 0) {
		perror("pthread_create");
		return 0;
	}
	pthread_detach(p);

	return 1;
}

// checks if the buffer needs to be printed
void check_print_buffer(void)
{
	if (print_buffer) {
		print_buffer = 0;
		print_buf(buf);
	}
}

// main server function, with main server loop
int server(char *sock_name)
{
	int sfd, cfd;
	struct sockaddr_un sock = {0};
	socklen_t slen;

	slen = sizeof(sock);

	if (strlen(sock_name) >= sizeof(sock.sun_path))
		return -1;

	sfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (sfd == -1) {
		perror("socket");
		return -1;
	}
	sock.sun_family = AF_UNIX;

	unlink(sock_name);

	strcpy(sock.sun_path, sock_name);

	if (bind(sfd, (struct sockaddr *)&sock, sizeof(sock)) < 0) {
		perror("bind");
		close(sfd);
		return -1;
	}

	if (listen(sfd, BACKLOG) < 0) {
		perror("listen");
		close(sfd);
		return -1;
	}

	// loop until signaled to exit
	while (!exit_server) {
		cfd = accept(sfd, (struct sockaddr *)&sock, &slen);
		if (cfd == -1) {
			if (errno == EINTR) {
				check_print_buffer();
			} else {
				exit_server = 1;
				perror("accept");
			}
			continue;
		}

		create_req_thread(cfd);
		check_print_buffer();
	}

	close(sfd);
	return 0;
}

// main function
int main(void)
{
	// setup signal handlers
	if (!create_signal_handlers())
		return 1;

	// init buffer
	buf = init_buf();

	server("socket");

	free_buf(buf);
	buf = NULL;

	return 0;
}

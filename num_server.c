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

#include "msg.h"
#include "buf.h"

#define BACKLOG	5

buf_s *buf;

int stor_num(int sfd, packet_s *pkt)
{
	printf("STOR %ld\n", pkt->num);
	if (add_buf(buf, pkt->num)) {
		printf("OK\n");
		if (!send_ok_pkt(sfd)) {
			fprintf(stderr, "error in send OK\n");
		}
	} else {
		printf("ERR\n");
		if (!send_err_pkt(sfd)) {
			fprintf(stderr, "error in send ERR\n");
		}
	}
	return 1;
}

int rtrv_num(int sfd, packet_s *pkt)
{
	printf("RTRV\n");
	if (retrieve_buf(buf, &(pkt->num))) {
		printf("num: %ld\n", pkt->num);
		if (!send_num_pkt(sfd, pkt->num)) {
			fprintf(stderr, "error in send ERR\n");
		}
	} else {
		printf("ERR\n");
		if (!send_err_pkt(sfd)) {
			fprintf(stderr, "error in send ERR\n");
		}
	}
	return 1;
}

void *handle_req(void *fd)
{
	int cfd = *((int *)(fd));
	packet_s *pkt;

	printf("handle req\n");
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
			//TODO handle error
			break;
	}

	free_pkt(pkt);

handle_req_return:
	free(fd);
	close(cfd);
	return NULL;
}

int create_req_thread(int cfd) {
	int *cfd_ptr = malloc(sizeof(int));
	if (cfd_ptr == NULL) {
		perror("malloc");
		return 0;
	}

	*cfd_ptr = cfd;

	pthread_t p;
	if (pthread_create(&p, NULL, handle_req, (void *)cfd_ptr) != 0) {
		perror("pthread_create");
		return 0;
	}
	printf("pre join\n");
	pthread_join(p, NULL);
	printf("post join\n");

	return 1;
}

int server(char *sock_name)
{
	int sfd, cfd;
	struct sockaddr_un sock = {0};
	socklen_t slen;

	slen = sizeof(sock);

	if (strlen(sock_name) >= sizeof(sock.sun_path)) {
		return -1;
	}

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

	//listen
	if (listen(sfd, BACKLOG) < 0) {
		perror("listen");
		close(sfd);
		return -1;
	}

	// loop on accept
	while ((cfd = accept(sfd, (struct sockaddr *)&sock, &slen)) != -1) {
		printf("accept\n");
		create_req_thread(cfd);
	}

	perror("accept");
	close(sfd);
	return 1;
}

int main()
{
	// init memory
	buf = init_buf();

	server("test");

	free_buf(buf);
	buf = NULL;

	return 1;
}

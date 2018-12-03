#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>

#include "msg.h"

#define BACKLOG	5

int stor_num(packet_s *pkt)
{
	printf("STOR %ld\n", pkt->num);
	return 1;
}

int rtrv_num(packet_s *pkt)
{
	printf("STRV\n");
	return 1;
}

void handle_req(int cfd)
{
	packet_s *pkt;

	printf("handle req\n");
	// read req
	pkt = read_pkt(cfd);
	if (pkt == NULL) {
		fprintf(stderr, "read_pkt error\n");
		return;
	}

	switch (pkt->type) {
		case PKT_OP_STOR:
			stor_num(pkt);
			break;
		case PKT_OP_RTRV:
			rtrv_num(pkt);
			break;
		default:
			//TODO handle error
			break;
	}

	free_pkt(pkt);

	close(cfd);
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
		handle_req(cfd);
	}

	perror("accept");
	close(sfd);
	return 1;
}

int main()
{
	// init memory
	server("test");
}

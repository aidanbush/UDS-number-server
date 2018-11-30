#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define BUF_LEN	12

int server(char *sock_name)
{
	int sfd, cfd;
	struct sockaddr_un sock = {0};
	socklen_t slen;

	slen = sizeof(sock);

	if (strlen(sock_name) >= sizeof(sock.sun_path)) {
		return -1;
	}

	sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sfd == -1) {
		perror("socket");
		return -1;
	}
	sock.sun_family = AF_UNIX;

	unlink(sock_name);

	sock.sun_family = AF_UNIX;
	strcpy(sock.sun_path, sock_name);

	if (bind(sfd, (struct sockaddr *)&sock, sizeof(sock)) < 0) {
		perror("bind");
		close(sfd);
		return -1;
	}

	close(sfd);
	printf("exit");
	return -1;

	ssize_t plen;

	struct sockaddr_un client_sock;
	uint8_t buf[BUF_LEN];
	socklen_t len;

	// loop on accept
	while ((plen = recvfrom(sfd, buf, BUF_LEN, 0, (struct sockaddr *) &client_sock, &len)) >= 0) {
		printf("recv");
		//save stuff into struct
		//determine op code
		//write to read
		//send packet back
	}

	perror("recvfrom");
	close(sfd);
	return 1;
}

int main()
{
	// init memory
	server("test");
}

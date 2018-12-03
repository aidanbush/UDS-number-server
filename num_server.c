#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#define BACKLOG	5

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
		close(cfd);
		//save stuff into struct
		//determine op code
		//write to read
		//send packet back
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

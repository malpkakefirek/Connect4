#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXLINE 1024
#define LISTENQ 2
#define PORT 13

int startServer(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t len;
    struct sockaddr_in servaddr, cliaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return 1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PORT);

    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "bind error: %s\n", strerror(errno));
        return 1;
    }

    if (listen(listenfd, LISTENQ) < 0) {
        fprintf(stderr, "listen error: %s\n", strerror(errno));
        return 1;
    }
    fprintf(stderr, "Waiting for clients...\n");

    len = sizeof(cliaddr);
    if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &len)) < 0) {
        fprintf(stderr, "accept error: %s\n", strerror(errno));
        return 1;
    }

    printf("Connection from %s\n", inet_ntoa(cliaddr.sin_addr));
    return connfd;
}

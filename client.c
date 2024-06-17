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
#define SA struct sockaddr
#define PORT 13

int startClient(int argc, char **argv) {
    int sockfd, n;
    struct sockaddr_in servaddr;
    char recvline[MAXLINE + 1];
    int err;

    if (argc != 2) {
        fprintf(stderr, "ERROR: usage: %s <IPaddress>\n", argv[0]);
        return 1;
    }
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket error: %s\n", strerror(errno));
        return 1;
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    if ((err = inet_pton(AF_INET, argv[1], &servaddr.sin_addr)) <= 0) {	// convert IP address
        if (err == 0)
            fprintf(stderr, "inet_pton error for %s\n", argv[1]);
        else
            fprintf(stderr, "inet_pton error for %s: %s\n", argv[1], strerror(errno));
        return 1;
    }
    if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "connect error: %s\n", strerror(errno));
        return 1;
    }

    return sockfd;
}
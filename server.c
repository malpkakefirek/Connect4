#include        <sys/types.h>   /* basic system data types */
#include        <sys/socket.h>  /* basic socket definitions */
#include        <sys/time.h>    /* timeval{} for select() */
#include        <time.h>        /* timespec{} for pselect() */
#include        <netinet/in.h>  /* sockaddr_in{} and other Internet definitions */
#include        <arpa/inet.h>   /* inet(3) functions */
#include        <errno.h>       // error number definitions
#include        <fcntl.h>       /* for nonblocking */
#include        <netdb.h>       // definitions for network database operations
#include        <signal.h>      // signal handling definitions
#include        <stdio.h>       // standard I/O definitions
#include        <stdlib.h>      // standard library definitions
#include        <string.h>      // string manipulation definitions
#include        <unistd.h>      // POSIX standard function definitions

#define MAXLINE 1024           // maximum buffer size
#define LISTENQ 2              // maximum number of client connections

int startServer(int argc, char ** argv) {
    int listenfd, connfd;       
    socklen_t len;              
    char buff[MAXLINE], str[INET_ADDRSTRLEN + 1]; 
    time_t ticks;               
    struct sockaddr_in servaddr, cliaddr;

    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket error : %s\n", strerror(errno));
        return 1;
    }

    bzero(&servaddr, sizeof(servaddr)); // zero out the server address structure
    servaddr.sin_family = AF_INET;      
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 
    servaddr.sin_port = htons(13);      /* daytime server */

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        fprintf(stderr, "bind error : %s\n", strerror(errno)); 
        return 1;
    }

    if (listen(listenfd, LISTENQ) < 0) {
        fprintf(stderr, "listen error : %s\n", strerror(errno));
        return 1;
    }
    fprintf(stderr, "Waiting for clients ... \n");
    for ( ; ; ) {
        len = sizeof(cliaddr);
        if ((connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &len)) < 0) {
            fprintf(stderr, "accept error : %s\n", strerror(errno));
            continue;
        }

        bzero(str, sizeof(str)); // zero out the string buffer
        inet_ntop(AF_INET, (struct sockaddr *) &cliaddr.sin_addr, str, sizeof(str)); // convert client address to string
        printf("Connection from %s\n", str); // print client address

        ticks = time(NULL);
        snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks)); // format time into buffer
        if (write(connfd, buff, strlen(buff)) < 0)
            fprintf(stderr, "write error : %s\n", strerror(errno));
        close(connfd);
    }
}

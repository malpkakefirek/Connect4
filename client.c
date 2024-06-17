#include        <sys/types.h>   /* basic system data types */
#include        <sys/socket.h>  /* basic socket definitions */
#include        <netinet/in.h>  /* sockaddr_in{} and other Internet definitions */
#include        <arpa/inet.h>   /* inet(3) functions */
#include        <errno.h>       // error number definitions
#include        <stdio.h>       // standard I/O definitions
#include        <stdlib.h>      // standard library definitions
#include        <string.h>      // string manipulation definitions
#include        <unistd.h>      // POSIX standard function definitions

#define MAXLINE 1024           // maximum buffer size
#define SA      struct sockaddr // alias for struct sockaddr

int startClient(int argc, char **argv) {
	int					sockfd, n;                       // socket file descriptor and read count
	struct sockaddr_in	servaddr;                      // server address structure
	char				recvline[MAXLINE + 1];         // buffer for received line
	int err;

	if (argc != 2){
		fprintf(stderr, "ERROR: usage: a.out <IPaddress>  \n");
		return 1;
	}
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		fprintf(stderr,"socket error : %s\n", strerror(errno));
		return 1;
	}

	bzero(&servaddr, sizeof(servaddr));              // zero out the server address structure
	servaddr.sin_family = AF_INET;                   
	servaddr.sin_port   = htons(13);                 
	if ( (err=inet_pton(AF_INET, argv[1], &servaddr.sin_addr)) <= 0){
		if(err == 0 )                              
			fprintf(stderr,"inet_pton error for %s \n", argv[1] ); 
		else
			fprintf(stderr,"inet_pton error for %s : %s \n", argv[1], strerror(errno)); 
		return 1;
	}
	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){ // connect to the server
		fprintf(stderr,"connect error : %s \n", strerror(errno)); 
		return 1;
	}

	while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0;	/* null terminate */
		if (fputs(recvline, stdout) == EOF){        // print received data to stdout
			fprintf(stderr,"fputs error : %s\n", strerror(errno));
			return 1;
		}
	}
	if (n < 0)                                     // check if read failed
		fprintf(stderr,"read error : %s\n", strerror(errno));

	fprintf(stderr,"OK\n");
	fflush(stdout);

	exit(0);
}

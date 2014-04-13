#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <strings.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXLINE 1024

void str_client(char str[MAXLINE], int socket_fd)
{
	char	sndLine[MAXLINE]; //string message from client to server
	char	rcvLine[MAXLINE]; //string message from server to client

    memset((void *)sndLine, 0, MAXLINE);
    memset((void *)rcvLine, 0, MAXLINE);

   
	    write(socket_fd, (void *)str, strlen(str)); //writes to the server the message, and message length

	    if (read(socket_fd, rcvLine, MAXLINE) == 0) //receives the message from the server
	    {
		   printf("ERROR: server terminated \n");
		   return;
	    }

	    fputs(rcvLine, stdout);
	

}

int main(int argc, char *argv[])
{

	int	socket_fd;
	struct  sockaddr_in servaddr;

	if (argc != 2) {
		printf("Hey type something \n");
        return -1;
	}

	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0) {
        fprintf(stderr, "Error creating socket, errno = %d (%s) \n", 
                errno, strerror(errno));
        return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");//argv[1]
	servaddr.sin_port = htons(3333);//atoi(argv[2])

	if (connect(socket_fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        fprintf(stderr, "Error unable to connect to socket, errno = %d (%s) \n", errno,
                strerror(errno));
        return -1;
	}

	str_client(argv[1], socket_fd);

	return 0;

}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <pthread.h>
#include <netdb.h>

#define MAXLINE	1024

const int backlog = 4;

void getFunc(int, char *);

void headFunc(int, char*);

void putFunc(int, char *);

void delFunc(int, char*);

void httpOk(int);

void httpBadRequest(int);

void httpNotThere(int);

void *clientHandler(void *arg)
{
	
	int fd = *(int*)(arg);
	
	unsigned int i;
	
	char str[100];
	
	char method[100];
	
	char file[100];
	
	char test[10];
	
	read(fd, str, strlen(str));
	
	
	if(strcasecmp(method, "GET"))
	{
		getFunc(fd, file);
	}
	
	if(strcasecmp(method, "HEAD"))
	{
		headFunc(fd, file);
	}
	
	if(strcasecmp(method, "PUT"))
	{
		putFunc(fd, file);
	}
	
	if(strcasecmp(method, "DELETE"))
	{
		delFunc(fd, file);
	}
	
	
}

void getFunc(int fd, char *str)
{
	
}

void headFunc(int fd, char *str)
{
	
}

void putFunc(int fd, char *str)
{
	
}

void delFunc(int fd, char *str)
{
	
}

void okRequest(int fd)
{
	
}

void badRequest(int fd)
{
	
}

void notThere(int fd)
{
		
}

int main(int argc, char *argv[])
{

	int	listenfd, connfd;
    pthread_t tid;
	int     clilen;
	struct 	sockaddr_in cliaddr, servaddr;

/*
	if (argc != 3) {
		printf("Usage: caseServer <address> <port> \n");
        return -1;
	}
*/
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
		fprintf(stderr, "Error unable to create socket, errno = %d (%s) \n",
                errno, strerror(errno));
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family 	   = AF_INET;
	servaddr.sin_addr.s_addr   = inet_addr("127.0.0.1"); //argv[1]
	servaddr.sin_port          = htons(3333); //atoi(argv[2])

	if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        fprintf(stderr, "Error binding to socket, errno = %d (%s) \n",
                errno, strerror(errno));
        return -1;

	}

	if (listen(listenfd, backlog) == -1) {
        fprintf(stderr, "Error listening for connection request, errno = %d (%s) \n",
                errno, strerror(errno));
        return -1;
	}

	
	while (1) {
		clilen = sizeof(cliaddr);
		if ((connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen)) < 0 ) {
			if (errno == EINTR)
				continue;
			else {
                fprintf(stderr, "Error connection request refused, errno = %d (%s) \n",
                        errno, strerror(errno));
			}
		}

        if (pthread_create(&tid, NULL, clientHandler, (void *)&connfd) != 0) {
           fprintf(stderr, "Error unable to create thread, errno = %d (%s) \n",
                   errno, strerror(errno));
        }

	}
}

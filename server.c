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
#include <unistd.h>

#define MAXLINE	1024

const int backlog = 4;

void parseRequest(int, char *); // parsing (validates!)

void getFunc(int, char *); 

void headFunc(int, char *);

void putFunc(int, char *);

void delFunc(int, char *);

void statusMesgOK(int);

void statusMesgBadRequest(int);

void statusMesgNotFound(int);

void *clientHandler(void *arg)
{
	int n;
	
	int fd = *(int*)(arg);
	
	char str[MAXLINE];
	
	while (1) 
	{
        if ((n = read(fd, str, MAXLINE)) == 0) 
        {
            close (fd);
            return (void*) 0;
        }
        
        
        
        
        parseRequest(fd, str);
		
		close(fd);
		return (void*) 0;
    }
	
}

void parseRequest(int fd, char *str)
{
	char get[MAXLINE] = "GET\r\n";
	char head[MAXLINE] = "HEAD\r\n";
	char put[MAXLINE] = "PUT\r\n";
	char delete[MAXLINE] = "DELETE\r\n";
	
	
	char r[MAXLINE];
	
	strncat(r, str, strlen(str));
	
	if(strcmp(r, get) == 0)
	{
	  //getFunc(...);
		write(fd, get, MAXLINE);
	}
	else if(strcmp(r, head) == 0)
	{	
	  //headFunc(...);
		write(fd, head, MAXLINE);
	}
	else if(strcmp(r, put) == 0)
	{	
	  //putFunc(...);
		write(fd, put, MAXLINE);
	}
	else if(strcmp(r, delete) == 0)
	{	
	  //delFunc(...);
		write(fd, delete, MAXLINE);
	}
	else
	  statusMesgBadRequest(fd);
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

void statusMesgOK(int fd)
{
	
}

void statusMesgBadRequest(int fd)
{
  char * mesg = "Error 404: Bad Request!!\r\n";
  write(fd, mesg, strlen(mesg)); 
}

void statusMesgNotFound(int fd)
{
		
}

int main(int argc, char *argv[])
{

	int	listenfd, connfd;
    pthread_t tid;
	unsigned int     clilen;
	struct 	sockaddr_in cliaddr, servaddr;

	if (argc != 2) {
	  printf("Usage: caseServer <port> \n");
	  return -1;
	}


	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd == -1)
	{
		fprintf(stderr, "Error unable to create socket, errno = %d (%s) \n",
                errno, strerror(errno));
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family 	   = AF_INET;
	servaddr.sin_addr.s_addr   = inet_addr("127.0.0.1");
	servaddr.sin_port          = htons(atoi(argv[1]));

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

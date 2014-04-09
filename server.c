#include <stdio.h>
#include <stdlib.h>
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
#include <time.h>
#include <sys/stat.h>



// Server  Constants
#define MAXLINE	1024
const int backlog = 10;
const char * serverName = "The Amazing 24-hour Server";
const char * httpVersion = "HTTP/1.0";


// Status Messages
const  char * OK = "200 OK"; // Request has succeeded (GET: here's your file.  HEAD: Here's info, you don't have it cached.  DELETE: Success)
const  char * Created = "201 Created"; // New resource was created (PUT)
const  char * NotModified = "304 Not Modified"; // Same date on cached and server resource  (HEAD: You have it cached.  GET: You have up-to-date copy)
const  char * BadRequest = "400 Bad Request"; // Malformed request; throw it out
const  char * Forbidden = "403 Forbidden"; // Resource requested is not marked world-readable (PUT&DELETE: no write permission.  GET&HEAD: no read permission)
const  char * NotFound = "404 Not Found"; // Resource DNE (GET&HEAD&DELETE)



// URI struct and functions
struct URI
{
  int ID;
  char * relativePath;
  int contentLength;
  time_t expirationDate;
  time_t lastModifyDate;
};
URI * LoadResources(URI * files, int * getNoOfFiles);
int SaveResources();


// Hnadle requsets of clients
void ReadRequest(int, char *);
void TokenizeRequest(char *request, char *command, char *resource, char *httpVersion);

// Build Response
char * BuildResponse();
char * BuildResponse(char * httpVersion, )
{
  // Status-Line      =  HTTP-Verlsion Status-Code Reason-Phrase\r\n
  // (General-Header  =  Date   =  "Date" ":" HTTP-date
  //                   | Pragma =  "Pragma" ":" 1#pragma-directive  = pragma-directive  = no-cache" | extension-pragma
  // | Request-Header
  // | Entity-Header
  // CRLF
  // Entity-Body
}

char * BuildStatusLine();
char * BuildGeneralHeader(char * date);
char * BuildResponseHeader();
char * BuildEntityHeader(char * allowedOps, int contentLen, char * expires, char * lastModified)

// HTTP functions, as defined in http://www.w3.org/Protocols/HTTP/1.0/spec.html#Server
void GET(int, char *);
void HEAD(int, char *);
void PUT(int, char *);
void DELETE(int, char *);


// Function Definitions

void *clientHandler(void *arg)
{
  int n;
  int fd = *(int*)(arg);
  char request[MAXLINE];
	
  while (1) 
    {
      if ((n = read(fd, request, MAXLINE)) == 0) 
	{
	  close (fd);
	  return (void*) 0;
	}    

      ReadRequest(fd, request); 
      close(fd);
      return (void*) 0;
    }	
}


void ReadRequest(int fd, char * request)
{
  char *command, *resource, *httpVersion;
  TokenizeRequest(request, command, resource, httpVersion);
  
  
  if(strcmp(request, "GET") == 0)
    {
      GET(); // attempt function; returns what happened
      BulidResponse(); // given feedback, build a response
    }
  else if(strcmp(request, "HEAD") == 0)
    {	
      HEAD(); // attempt function; returns what happened
      BulidResponse(); // given feedback, build a response
    }
  else if(strcmp(request, "PUT") == 0)
    {	
      PUT(); // attempt function; returns what happened
      BulidResponse(); // given feedback, build a response
    }
  else if(strcmp(request, "DELETE") == 0)
    {	
      DELETE(); // attempt function; returns what happened
      BulidResponse(); // given feedback, build a response
    }
  else
    write(fd, BadRequest, strlen(BadRequest));
  close(fd);
}

void TokenizeRequest(char *request, char *command, char *resource, char *httpVersion)
{
  
}




void GET(int fd, char *resourceRequested)
{
  
}

void HEAD(int fd, char *resourceRequested)
{
  
}

void PUT(int fd, char *newResource)
{
  
}

void DELETE(int fd, char *toBeDeleted)
{
  
}



// Server Main

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

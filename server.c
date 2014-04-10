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
const char * serverName = "TheAmazing24-hourServer";


// Status Messages
const  char * OK = "200 OK"; // Request has succeeded (GET: here's your file.  HEAD: Here's info, you don't have it cached.  DELETE: Success)
const  char * Created = "201 Created"; // New resource was created (PUT)
const  char * NotModified = "304 Not Modified"; // Same date on cached and server resource  (HEAD: You have it cached.  GET: You have up-to-date copy)
const  char * BadRequest = "400 Bad Request"; // Malformed request; throw it out
const  char * Forbidden = "403 Forbidden"; // Resource requested is not marked world-readable (PUT&DELETE: no write permission.  GET&HEAD: no read permission)
const  char * NotFound = "404 Not Found"; // Resource DNE (GET&HEAD&DELETE)


/*
// URI struct and functions
struct URI
{
  int ID;
  char * relativePath;
  int contentLength;
  time_t expirationDate;
  time_t lastModifyDate;
  };
int LoadResources(void * file, int * getNoOfFiles);
int SaveResources(void * file, int noOfFiles);
*/


// Handle requests of clients
void ProcessRequest(int fd, char * request);
void TokenizeRequest(const char *request, char *method, char *uri, char *version, char *statBuffer, char *body);

// Build Response
char * BuildResponse(char *method, char *uri, char *version, char *statBuffer, char *body);

char * BuildSimpleResponse(char *method, char *uri, char *version); // return "method space httpVersion"
char * BuildStatusLine(char * version, char * statusCodeAndReason); // return "httpVersion statusCode statusReason" 
char * BuildGeneralHeader(char * location); // return "header PragmaPlaceholder";           (Pragma not implemented)
char * BuildResponseHeader(char *location, char *wwwAuthenticate); // return "absoluteURI serverName wwwAuthenticate"
char * BuildEntityHeader(char * allowedOps, int contentLen, char * expires, char * lastModified); // return "allow contentEncoding contentcontentLength expires last modified extension"

// HTTP functions, as defined in http://www.w3.org/Protocols/HTTP/1.0/spec.html#Server
void GET(int fd, char *uri, char *version);
void HEAD(int fd, char *uri, char *version);
void PUT(int fd, char *uri, char *version);
void DELETE(int fd, char *uri, char *version);

// threaded client handler
void * clientHandler(void *arg);


// Function Definitions

void * clientHandler(void *arg)
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
      
      ProcessRequest(fd, request); 
      close(fd);
      return (void*) 0;
    }
}


void ProcessRequest(int fd, char * request)
{
  char *method, *uri, *version, *statBuffer, *body;
  TokenizeRequest(request, method, uri, version, statBuffer, body);
  
  if(strcmp(method, "GET") == 0)
    {
      GET(fd, uri, version); // attempt function; returns what happened
      //BuildResponse(); // given feedback, build a response
    }
  else if(strcmp(method, "HEAD") == 0)
    {	
      HEAD(fd, uri, version); // attempt function; returns what happened
      //BuildResponse(); // given feedback, build a response
    }
  else if(strcmp(method, "PUT") == 0)
    {	
      PUT(fd, uri, version); // attempt function; returns what happened
      //BuildResponse(); // given feedback, build a response
    }
  else if(strcmp(method, "DELETE") == 0)
    {	
      DELETE(fd, uri, version); // attempt function; returns what happened
      //BuildResponse(); // given feedback, build a response
    }
  else
    write(fd, BadRequest, strlen(BadRequest));
  close(fd);
}

void TokenizeRequest(const char *request, char *method, char *uri, char *version, char *statBuffer, char *body)
{
  
}


char * BuildResponse(char *method, char *uri, char *version, char *statBuffer, char *body)
{
  char response[MAXLINE];
  
  strcat(response, BuildSimpleResponse(method, uri, version));
  strcat(response, BuildStatusLine());
  strcat(response, uri);
  strcat(response, version);
  strcat(response, statBuffer);
  strcat(response, method);
  return response;
}

char * tokenizeStatBuffer(const char *statBuffer, char *allowedOps, char *expires, char *lastModified)
{
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!
}
char * BuildSimpleResponse(char *method, char *uri, char *version) // return "method space httpVersion"
{
  char * simpleResponse;
  // Execute GET
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  return simpleResponse;
}
char * BuildStatusLine(char * version, char * statusCodeAndReason) // return "httpVersion statusCode statusReason" 
{
  return strcat(version, statusCodeAndReason);
}
char * BuildGeneralHeader(char * location) // return "header PragmaPlaceholder";           (Pragma not implemented)
{
  return location;
}
char * BuildResponseHeader(char *location, char * wwwAuthenticate) // return "absoluteURI serverName wwwAuthenticate"
{
  char * responseHeader;
  strcat(responseHeader, location);
  strcat(responseHeader, serverName);
  strcat(responseHeader, wwwAuthenticate);
}
char * BuildEntityHeader(char * allowedOps, int contentLen, char * expires, char * lastModified) // return "allow contentEncoding contentcontentLength expires last modified extension"
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


	//loadURI();
	
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

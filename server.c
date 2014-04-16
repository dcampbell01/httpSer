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
#include <string.h>
#include <sys/stat.h>
//#include <netinet/ip.h>




// Server  Constants
#define MAXLINE	40000
#define SERVER_NAME "TheNotSoAmazing10-dayServer"
#define HTTP_VERSION "HTTP/1.0"
#define DATE_FORMAT "%d %b %Y %H:%M:%S GMT" // RFC1123 format
typedef int bool;
#define true 1
#define false 0
#define LN "\r\n"
// Status Messages
const char * OK = "200 OK"; // Request has succeeded (GET: here's your file.  HEAD: Here's info, you don't have it cached.  DELETE: Success)
const char * Created = "201 Created"; // New resource was created (PUT)
const char * NotModified = "304 Not Modified"; // Same date on cached and server resource  (HEAD: You have it cached.  GET: You have up-to-date copy)
const char * BadRequest = "400 Bad Request"; // Malformed request; throw it out
const char * Forbidden  = "403 Forbidden"; // Resource requested is not marked world-readable (PUT&DELETE: no write permission.  GET&HEAD: no read permission)
const char * NotFound = "404 Not Found"; // Resource DNE (GET&HEAD&DELETE)
// Miscellaneous Constants
const int backlog = 10;


// threaded client handler
void * clientHandler(void *arg);
// Handle requests of clients
void ProcessRequest(int fd, char * request, int requestLen);
// HTTP method helper functions
char * getMimeType(char *name);
void sendHeaders(int fd, const char *status, char *extra, char *fileExtension, int length, time_t lastModDate);
// HTTP functions, as defined in http://www.w3.org/Protocols/HTTP/1.0/spec.html#Server
void GET(int fd, char *resource, int resourceLen);
void HEAD(int fd, char *resource, int resourceLen);
void PUT(int fd, char *resource, int resourceLen, char *body, int bodyLen);
void DELETE(int fd, char *resource, int resourceLen);




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

      ProcessRequest(fd, request, strlen(request)); 
      close(fd);
      return (void*) 0;
    }
}

void ProcessRequest(int fd, char * request, int requestLen)
{ 
  char * method;
  char * resource;
  char * version;
  char * body;
  // Does NOT handle receiving ZERO requirements nicely
  method = strtok(request, " ");
  resource = strtok(NULL, " ");
  version = strtok(NULL, " "); // currently does nothing with version


  // special case: if resource is just forward slash then resource is index.html
  if( (method == NULL) || (resource == NULL) || (version == NULL) ) // require 3 arguments
    {
      sendHeaders(fd, BadRequest, NULL, getMimeType(resource),0,0);
      return;
    }
  
  if( strcmp(resource, "/")==0 )
    resource = "index.html";
  
  printf("request: %s %s %s", method, resource, version); 
  int resourceLen = strlen(resource);
  if(strcmp(method, "GET") == 0)
    {
      GET(fd, resource, 10);
    }
  else if(strcmp(method, "HEAD") == 0)
    {	
      HEAD(fd, resource, resourceLen);
    }
  else if(strcmp(method, "PUT") == 0)
    {	
      char * receivedBody;
      receivedBody = strtok(NULL, "\0");
      PUT(fd, resource, resourceLen, receivedBody, strlen(receivedBody));
    }
  else if(strcmp(method, "DELETE") == 0)
    {	
      DELETE(fd, resource, resourceLen);
    }
  else
    sendHeaders(fd, BadRequest, NULL, getMimeType(resource),0,0);


  close(fd);
  return; // exit function
}

char * getMimeType(char *name)
{
  char *ext = strrchr(name, '.');
  if (!ext) 
    return NULL;
  

  if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0)
    return "text/html";
  else
    return NULL;
}

void sendHeaders(int fd, const char *status, char *extra, char *fileExtension, int length, time_t lastModDate)
{
  time_t now;
  time(&now);
  char timeBuf[128];
  char sb[MAXLINE];

  strcat(sb, HTTP_VERSION); strcat(sb, " "); strcat(sb, status); strcat(sb, LN);
  strcat(sb, "Server: "); strcat(sb, SERVER_NAME); strcat(sb, " "); strcat(sb, status); strcat(sb, LN);
  strftime(timeBuf, sizeof(timeBuf), DATE_FORMAT, gmtime(&now));
  strcat(sb, "Date: "); strcat(sb, timeBuf); strcat(sb, LN);
  if(extra)
    {
      strcat(sb, extra); strcat(sb, LN);
    }
  if(fileExtension)
    {
      strcat(sb, "Content-Type: "); strcat(sb, fileExtension); strcat(sb, LN);
    }
  if(length)
    {
      char lenStr[MAXLINE];
      snprintf(lenStr, sizeof(lenStr), "%d", length);
      strcat(sb, "Content-Length: "); strcat(sb, lenStr); strcat(sb, LN);
    }
  if(lastModDate != -1)
    {
      strftime(timeBuf, sizeof(timeBuf), DATE_FORMAT, gmtime(&lastModDate));
      strcat(sb, "Last-Modified: "); strcat(sb, timeBuf); strcat(sb, LN);
    }
  strcat(sb, LN); strcat(sb, LN);
  write(fd, sb, strlen(sb));
}



void GET(int fd, char *resource,  int resourceLen)
{
  // Open requested resource and send back the contents
  struct stat statBuf;
  char body[MAXLINE];

  FILE *file;
  file = fopen(resource, "r");
  if( ! file)
    {
      sendHeaders(fd, NotFound, NULL, getMimeType(resource),0,0);
    }
  else
    {
      if(stat(resource, &statBuf) == -1)
	{
	  perror("stat");
	  return;
	}
      int bodyLen = S_ISREG(statBuf.st_mode) ? statBuf.st_size : -1;
      sendHeaders(fd, OK, NULL, getMimeType(resource), bodyLen, statBuf.st_mtime);
      int next;
      FILE* fp = fdopen(fd, "w");
      char temp[MAXLINE];
      while(fgets(temp, MAXLINE, file) != NULL)
	strcat(body, temp);
      write(fd, body, strlen(body));
    }
}

void HEAD(int fd, char *resource,  int resourceLen)
{

  // Open requested resource and send back the contents
  struct stat statBuf;
  char body[MAXLINE];

  FILE *file;
  file = fopen(resource, "r");
  if( ! file)
    sendHeaders(fd, NotFound, NULL, getMimeType(resource),0,0);
  else
    {
      if(stat(resource, &statBuf) == -1)
	{
	  perror("stat");
	  return;
	}
      int bodyLen = S_ISREG(statBuf.st_mode) ? statBuf.st_size : -1;
      sendHeaders(fd, OK, NULL, getMimeType(resource), bodyLen, statBuf.st_mtime); 
      
    }
}

void PUT(int fd, char *resource,  int resourceLen, char *body, int bodyLen)
{
  FILE * file;
  int num;
  if((file = fopen(resource, "w")) != NULL)
    {
      if( (num = fputs( body, file )) != EOF )
	{
	  fclose(file);
	  sendHeaders(fd, Created, NULL, getMimeType(resource),0,0);
	}
      else
	sendHeaders(fd, Forbidden, NULL, getMimeType(resource),0,0);
    }
  else
    sendHeaders(fd, Forbidden, NULL, getMimeType(resource),0,0);
}

void DELETE(int fd, char *resource,  int resourceLen)
{
  FILE *file = fopen(resource, "a+");
  if (!file)
    {
      sendHeaders(fd, Forbidden, NULL, getMimeType(resource),0,0);
    }
  else
    {
      fclose(file);
      remove(resource);
      sendHeaders(fd, OK, NULL, getMimeType(resource),0,0);
    }
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
	servaddr.sin_addr.s_addr   = INADDR_ANY; // Assign server to all available IP Addresses
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


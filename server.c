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
//#include <netinet/ip.h>
#include <string.h>

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



// threaded client handler
void * clientHandler(void *arg);

// Handle requests of clients
void ProcessRequest(int fd, char * request, int requestLen);
void TokenizeRequest(const char *request, char *method, char *uri, char *version, struct stat statBuffer, char *body);
char **tokenize(const char *input, const char *sep); // from the internet!!

// HTTP functions, as defined in http://www.w3.org/Protocols/HTTP/1.0/spec.html#Server
void GET(int fd, char *uri, char *version);
void HEAD(int fd, char *uri, char *version);
void PUT(int fd, char *uri, char *version);
void DELETE(int fd, char *uri, char *version);

// Build Response
char * BuildResponse(char *method, char *uri, char *version, char *statBuffer, char *body); // not 100% sure that statBuffer should be the only stat-related variable
// HTTP\0.9 response
char * BuildSimpleResponse(char *method, char *uri, char *version); // return "method space httpVersion"
// HTTP\1.0 response
char * BuildStatusLine(char * version, char * statusCodeAndReason); // return "httpVersion statusCode statusReason" 
char * BuildGeneralHeader(char * location); // return "header PragmaPlaceholder";           (Pragma not implemented)
char * BuildResponseHeader(char *location, char *wwwAuthenticate); // return "absoluteURI serverName wwwAuthenticate"
char * BuildEntityHeader(char * allowedOps, int contentLen, char * expires, char * lastModified); // return "allow contentEncoding contentcontentLength expires last modified extension"




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



void ProcessRequest(int fd, char * request, int requestLen)
{
  char *method, *uri, *version, *body;
  struct stat statBuffer;
  TokenizeRequest(request, method, uri, version, statBuffer, body);
  
  if(strcmp(method, "GET") == 0)
    {
      GET(fd, uri, version);
    }
  else if(strcmp(method, "HEAD") == 0)
    {	
      HEAD(fd, uri, version);
    }
  else if(strcmp(method, "PUT") == 0)
    {	
      PUT(fd, uri, version);
    }
  else if(strcmp(method, "DELETE") == 0)
    {	
      DELETE(fd, uri, version);
    }
  else
    write(fd, BadRequest, strlen(BadRequest));
  close(fd);
}

// Currently, ONLY handles 0.9 requests
void TokenizeRequest(const char *request, char *method, char *uri, char *version, struct stat statBuffer, char *body)
{
  // request is the entire request string sent from the client and then
  //   it is broken down and stored in the other strings
  // *only method, uri, and version is returned in HTTP\0.9 requests
  char ** subStr = (char**)tokenize(request, " "); // Does NOT separate by newlines; for 1.0 requests more work is needed
  method = subStr[0];
  uri = subStr[1];
  version = subStr[2];
}
// http://stackoverflow.com/questions/1692206/the-intricacy-of-a-string-tokenization-function-in-c
// I did a straight copy and paste because that code was too much to bother with until we get the needed functionality.
char **tokenize(const char *input, const char *sep)
{
  /* strtok ruins its input string, so we'll work on a copy 
   */
  char* dup;

  /* This is the array filled with tokens and returned
   */
  char** toks = 0;

  /* Current token
   */
  char* cur_tok;

  /* Size of the 'toks' array. Starts low and is doubled when
  ** exhausted.
  */
  size_t size = 2;

  /* 'ntok' points to the next free element of the 'toks' array
   */
  size_t ntok = 0;
  size_t i;

  if (!(dup = strdup(input)))
    return NULL;

  if (!(toks = malloc(size * sizeof(*toks))))
    goto cleanup_exit;

  cur_tok = (char*)strtok(dup, sep);

  /* While we have more tokens to process...
   */
  while (cur_tok)
    {
      /* We should still have 2 empty elements in the array, 
      ** one for this token and one for the sentinel.
      */
      if (ntok > size - 2)
        {
	  char** newtoks;
	  size *= 2;

	  newtoks = realloc(toks, size * sizeof(*toks));

	  if (!newtoks)
	    goto cleanup_exit;

	  toks = newtoks;
        }

      /* Now the array is definitely large enough, so we just
      ** copy the new token into it.
      */
      toks[ntok] = strdup(cur_tok);

      if (!toks[ntok])
	goto cleanup_exit;

      ntok++;
      cur_tok = (char*)strtok(0, sep);
    }    

  free(dup);
  toks[ntok] = 0;
  return toks;

 cleanup_exit:
  free(dup);
  for (i = 0; i < ntok; ++i)
    free(toks[i]);
  free(toks);
  return NULL;
}


char * BuildResponse(char *method, char *uri, char *version, char *statBuffer, char *body)
{
  char response[MAXLINE];
  // HTTP\0.9 response
  strcat(response, BuildSimpleResponse(method, uri, version));

  //  HTTP\1.0 response (requires a little more logic in this function)
  //  strcat(response, BuildStatusLine()); strcat(response, "\r\n");
  //  strcat(response, BuildGeneralHeader()); strcat(response, "\r\n");
  //  strcat(response, BuildResponseHeader()); strcat(response, "\r\n");
  //strcat(response, BuildEntitityHeader());


  strcat(response, "\r\n\r\n");
  strcat(response, body); 
  return response;
}
char * BuildSimpleResponse(char *methodURIVersion) // return "method space httpVersion"
{
  // Execute GET
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  return simpleResponse;
}


void GET(int fd, char *resource, int resourceLen)
{
  char copy[MAXLINE];
  FILE * file;
  char e[1024];
  file = fopen(resourceRequested, "r");
  if(!file)
    {
      //throw that 404
    }
  else
    {
      while(fgets(copy, 1024, file) != NULL)
	{
	  /* get a line, up to 80 chars from fr. done if NULL */
	  sscanf (copy, "%s", e);
	  /* convert the string to a long int */
	  write(fd, copy, strlen(copy));
	  //write over to socket....
	}
      fclose(file); /* close the file prior to exiting the routine */
    }
}
void HEAD(int fd, char *uri, char *version)
{
  
}
void PUT(int fd, char *uri, char *version)
{
  
}
void DELETE(int fd, char *uri, char *version)
{
  
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
	servaddr.sin_addr.s_addr   = inet_addr(INADDR_ANY); // Assign server to all available IP Addresses
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

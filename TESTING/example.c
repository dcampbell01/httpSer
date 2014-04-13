#include <stdio.h

#include <stdlib.h>

#include <string.h>

#include <time.h>

#include <sys/stat.h>

#include <unistd.h>

#include <sys/types.h>

#include <dirent.h>

#include <netinet/in.h>

#include <sys/socket.h>


#define SERVER "webserver/1.0"

#define PROTOCOL "HTTP/1.0"

#define RFC1123FMT "%a, %d %b %Y %H:%M:%S GMT"



//*********************************************************

char *get_mime_type(char *name)

{

  char *ext = strrchr(name, '.');

  if (!ext) return NULL;

  if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) return "text/html";

  return NULL;

}

//*******************************************************************

void send_headers(FILE *f, int status, char *title, char *extra, char *mime, int length, time_t date)

{

  time_t now;

  char timebuf[128];


  fprintf(f, "%s %d %s\r\n", PROTOCOL, status, title);

  fprintf(f, "Server: %s\r\n", SERVER);

  now = time(NULL);

  strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&now));

  fprintf(f, "Date: %s\r\n", timebuf);

  if (extra) fprintf(f, "%s\r\n", extra);

  if (mime) fprintf(f, "Content-Type: %s\r\n", mime);

  if (length >= 0) fprintf(f, "Content-Length: %d\r\n", length);

  if (date != -1)

    {

      strftime(timebuf, sizeof(timebuf), RFC1123FMT, gmtime(&date));

      fprintf(f, "Last-Modified: %s\r\n", timebuf);

    }

  fprintf(f, "Connection: close\r\n");

  fprintf(f, "\r\n");

}

//************************************************************************

void send_error(FILE *f, int status, char *title, char *extra, char *text)

{

  send_headers(f, status, title, extra, "text/html", -1, -1);

  fprintf(f, "<HTML><HEAD><TITLE>%d %s</TITLE></HEAD>\r\n", status, title);

  fprintf(f, "<BODY><H4>%d %s</H4>\r\n", status, title);

  fprintf(f, "%s\r\n", text);

  fprintf(f, "</BODY></HTML>\r\n");

}

//*************************************************************************

//GET

void getfile(FILE *f, char *path, struct stat *statbuf)

{

  char data[4096];

  int n;


  FILE *file = fopen(path, "r");

  if (!file)

    send_error(f, 403, "Forbidden", NULL, "Access denied.");

  else

    {

      int length = S_ISREG(statbuf->st_mode) ? statbuf->st_size : -1;

      send_headers(f, 200, "OK", NULL, get_mime_type(path), length, statbuf->st_mtime);


      while ((n = fread(data, 1, sizeof(data), file)) > 0) fwrite(data, 1, n, f);

      fclose(file);

    }

}

//**************************************************************************

//HEAD

void filehead(FILE *f, char *path, struct stat *statbuf)

{

  char data[4096];

  int n;


  FILE *file = fopen(path, "r");

  if (!file)

    send_error(f, 403, "Forbidden", NULL, "Access denied.");

  else

    {

      int length = S_ISREG(statbuf->st_mode) ? statbuf->st_size : -1;

      send_headers(f, 200, "OK", NULL, get_mime_type(path), length, statbuf->st_mtime);


      fclose(file);

    }

}

//********************************************************************************

//DELETE method

void removefile(FILE *f, char *path)

{

  char data[4096];

  int n;

  FILE *file = fopen(path, "a+");

  if (!file)

    send_error(f, 403, "Forbidden", NULL, "Access denied.");

  else

    remove(path);

}

//**********************************************************************************

//PUT method

void putfile(FILE *f, char *path)

{

  char data[4096];

  int n;

  FILE *file = fopen(path, "a+");

  FILE *pfile = fopen("copy.html", "w");


  if (!file)

    send_error(f, 403, "Forbidden", NULL, "Access denied.");

  else

    {

      while ((n = fread(data, 1, sizeof(data), file)) > 0) fputs(data, pfile);

      fclose(file);

      fclose(pfile);

    }

}


//********************************************************************************

int process(FILE *f)

{

  char buf[4096];

  char *method;

  char *path;

  char *protocol;

  struct stat statbuf;

  char pathbuf[4096];

  int len;


  if (!fgets(buf, sizeof(buf), f)) return -1;

  printf("Request: %s", buf);
  //splits the buf string

  method = strtok(buf, " ");

  path = strtok(NULL, " ");

  protocol = strtok(NULL, "\r");


  printf("%s", method);

  printf("%s", path);

  printf("%s", protocol);


  if (!method || !path || !protocol)
 
    return -1;


  if(strcmp(method, "GET") == 0)

    getfile(f, path, &statbuf);

  else if(strcmp(method, "HEAD") == 0)

    filehead(f, path, &statbuf);

  else if(strcmp(method, "PUT") == 0)
    putfile(f, path);

  else if(strcmp(method, "DELETE") == 0)

    removefile(f, path);


  return 0;

}


//======================================================================================================

void error(const char *msg)

{

  perror(msg);//call short error message for standard error
  
exit(1);//abort program

}

//=======================================================================================================


int main(int argc, char *argv[])

{

  int sock, portnum;

  struct sockaddr_in serv_addr, cli_addr;


  if (argc < 2)//if the port number is less than 2, there is no port

    {

      fprintf(stderr,"Error there was  no port provided\n");

      exit(1);

    }


  sock = socket(AF_INET, SOCK_STREAM, 0);

  if(sock<0)

    error("Error opening socket.");

  bzero((char *)&serv_addr, sizeof(serv_addr));

  portnum = atoi(argv[1]);

  serv_addr.sin_family = AF_INET;

  serv_addr.sin_addr.s_addr = INADDR_ANY;

  serv_addr.sin_port = htons(portnum);

  if(bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))<0)

    error("Error with binding");

  listen(sock, 5);

  printf("HTTP server listening on port %d\n", portnum);


  while (1)

    {

      int s;

      FILE *f;


      s = accept(sock, NULL, NULL);

      if (s < 0) break;


      f = fdopen(s, "a+");

      process(f);

      fclose(f);

    }


  close(sock);

  return 0;
}

//=========================================================================================

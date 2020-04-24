/*
 * proxy.c - A program to act as a proxy between a client and 
 * a server for HTTP/1.0 Web requests using the GET method.
 * Author - Joshua DeMoss
 * 12/11/19
 *
 */
#include <stdio.h>
#include "csapp.h"

//Declaring functions ahead of time
void doit(int fd);
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg);
void *thread(void *vargp);


/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000

/* static variables to contain some headers that will always be sent in requests */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:56.0) Gecko/20100101 Firefox/56.0r\r\n";
static const char *connection_hdr = "Connection: close\r\n";
static const char *proxy_connection_hdr = "Proxy-Connection: close\r\n";



/* Listen for incoming connections and then service them using threading */
int main(int argc, char **argv) 
{
    /* Check command line args */
    if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(1);
    }

    int listenfd;
    listenfd = Open_listenfd(argv[1]); //open and return a listening socket on port
    
    //Variables for used for establishing a connection between proxy and client
    socklen_t clientlen;
    struct sockaddr_storage clientaddr;
    int *connfdp;
    char client_hostname[MAXLINE], client_port[MAXLINE];
    
    pthread_t tid; //for threading at the end of the while loop below
    
    while (1) { //continually listen for incomping requests and accept them
		clientlen = sizeof(clientaddr);
        connfdp = Malloc(sizeof(int));
		*connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen); //returns files desc for socket
        Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
        printf("Connected to (%s, %s)\n", client_hostname, client_port);

        //create a thread for the actual work of the proxy to be done in
		Pthread_create(&tid, NULL, thread, connfdp);                                        
  	}
  	return 0;
}


/*Function that is called when a thread is made
* It esentually just calls do it and makes sure to detach itself
* from the main thread
*/
void *thread(void *vargp)
{
    int connfd = *((int *)vargp);
    Pthread_detach(pthread_self());
    Free(vargp);
    doit(connfd);
    Close(connfd);
    return NULL;
}


/*
 * doit - handle one HTTP request form client, forward it to server,
  get response form server, and forward response to client
 */
void doit(int fd) 
{
    char buf[MAXLINE]; //used as a universal buffer throughout function

    rio_t rio; //associated with connection b/t client and proxy
    Rio_readinitb(&rio, fd); //establish that connection ^
    if (!Rio_readlineb(&rio, buf, MAXLINE))
        return;

    char url[MAXLINE], method[MAXLINE];  
    sscanf(buf, "%s http://%s", method, url);       //ignore version number bc we send HTTP/1.0 anyway

    if (strcasecmp(method, "GET")) {                     //if not a GET request
        clienterror(fd, method, "501", "Not Implemented",
                    "Proxy does not implement this method");
        return;
    }  

//Parsing the URL: ****************************************
    char *requested_page;       
    char *requested_port;
	if((requested_port = strchr(url, ':')) != NULL) { //If there is specified port number
		*requested_port = '\0';                       //change ':' to terminator so tht url points to host
        requested_port++;                             //increment port so that it points at everything after ':'
        if((requested_page = strchr(requested_port, '/')) != NULL) { //if there is a file path do similar for separation
            *requested_page = '\0';
            requested_page++;
        } else {
            requested_page = '\0';
        }
	} else {                                           //hard code port to 80 if there isn't an explicit request.
		requested_port = "80";
        if((requested_page = strchr(url, '/')) != NULL) {   //again check for if there is a file path
            *requested_page = '\0';
            requested_page++;
        } else {
            requested_page = '\0';
        }
	}

//Creating and Sending Headers to Server ****************************

    //store first line (GET /tiny.c HTTP/1.0) in headline
    //and store host in host (localhost)
    char headline[MAXLINE], host[MAXLINE];
    sprintf(headline, "GET /%s HTTP/1.0\r\n", requested_page);
    sprintf(host, "Host: %s\r\n", url);

    //open connection with server and send innitial headers
    int clientfd;
    clientfd = Open_clientfd(url, requested_port);
    Rio_writen(clientfd, headline, strlen(headline));
    Rio_writen(clientfd, host, strlen(host));
    Rio_writen(clientfd, (void *)user_agent_hdr, strlen(user_agent_hdr));
    Rio_writen(clientfd, (void *)connection_hdr, strlen(connection_hdr));
    Rio_writen(clientfd, (void *)proxy_connection_hdr, strlen(proxy_connection_hdr));

    //Send the rest of the client request headers to server if not already sent:
    char tempHdr[MAXLINE];
    int flag = 1;
    Rio_readlineb(&rio, buf, MAXLINE); //rio for client
    while(strcmp(buf, "\r\n")) { //searching through all headers
        flag = 1;
        sscanf(buf, "%s", tempHdr);
        if ((strcmp(tempHdr, "Host:") == 0) || 
            (strcmp(tempHdr, "User-Agent:") == 0 ) || 
            (strcmp(tempHdr, "Connection:") == 0) || 
            (strcmp(tempHdr, "Proxy-Connection:") == 0)){
            flag = 0;

        }
        if (flag == 1) {
            Rio_writen(clientfd, buf, strlen(buf));
        }
        Rio_readlineb(&rio, buf, MAXLINE); //rio for client
    }
    Rio_writen(clientfd, buf, strlen(buf));

//Recieving and forwarding the response from the server to the client *************
    
    //Modifying and forwarding proper header to client from proxy
    rio_t rio2;
    Rio_readinitb(&rio2, clientfd); //Get ready for read from server
    Rio_readlineb(&rio2, buf, MAXLINE);
    char description[MAXLINE];
    char status[MAXLINE];
    char version[MAXLINE]; //only to help sscanf skip the actual version since it is hard coded to HTTP/1.0
    sscanf(buf, "%s %s %[^\t\n]", version, status, description);

    char responseHeader[MAXLINE];
    sprintf(responseHeader, "HTTP/1.0 %s %s", status, description);
    Rio_writen(fd, (void *)responseHeader, strlen(responseHeader));

    //Forward everything else from the server to the proxy unmodified
    size_t n;
    while((n = Rio_readlineb(&rio2, buf, MAXLINE)) != 0) {
        Rio_writen(fd, buf, n);
    }

    Close(clientfd);
}

/*
 * clienterror - returns an error message to the client
 */
void clienterror(int fd, char *cause, char *errnum, 
		 char *shortmsg, char *longmsg) 
{
    char buf[MAXLINE], body[MAXBUF];

    /* Build the HTTP response body */
    sprintf(body, "<html><title>Proxy Error</title>");
    sprintf(body, "%s<body bgcolor=""ffffff"">\r\n", body);
    sprintf(body, "%s%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p>%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em>The Proxy Web server</em>\r\n", body);

    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    Rio_writen(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    Rio_writen(fd, buf, strlen(buf));
    Rio_writen(fd, body, strlen(body));
}
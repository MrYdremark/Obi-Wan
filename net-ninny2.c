/* NETNINNY */

// INCLUDED LIBRARIES
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

// INCLUDED HEADERS
#include "header_parser.h"
#include "content_filter.h"
#include "communications.h"

// DEFINES
#define BACKLOG 10

void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in*)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

// MAIN FUNCTION
int main(int argc, char* argv[]) {
  
  char * myport = "1337";;
  
  if (argv[1])
    myport = argv[1];
    
  
  fd_set master, read_fds;             // File descriptor sets for select()
  int fdmax;                           // Maximum fd number
  
  int listener;                        // Listening socket
  int newfd;                           // Socket to accept new connections
  int sndfd;                           // Socket to open connection for sending

  struct sockaddr_storage remote_addr; // Client address
  socklen_t remote_addr_len;           // Length of remote address

  int yes = 1;                         // Used for setsockopt()
  int i, j, rv;

  char remoteIP[INET6_ADDRSTRLEN];

  struct addrinfo hints, *ai, *p;

  char * sndaddr;
  
  FD_ZERO(&master);
  FD_ZERO(&read_fds);
  
  signal(SIGPIPE, SIG_IGN);

  
  
  // START FUNCTION ??
  memset(&hints, 0, sizeof hints);
  hints.ai_family   = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags    = AI_PASSIVE;
  if ((rv = getaddrinfo(NULL, myport, &hints, &ai)) !=0) {
    fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
    exit(1);
  }
  // END FUNCTION ??
  
  // Set up the listener and bind it to the port
  for (p = ai; p != NULL; p = p->ai_next) {
    // Create our listener
    listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (listener < 0) {
      continue;
    }
    // Override the adress is already in use message
    setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    // Bind to port
    if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
      close(listener);
      continue;
    }
    break;
  }

  // Check if we got bound
  if (p == NULL) {
    fprintf(stderr, "selectserver: failed to bind\n");
    exit(2);
  }
  
  freeaddrinfo(ai);

  // Start to listen
  if (listen(listener, BACKLOG) == -1) {
    perror("listen");
    exit(3);
  }

  // Add listener to master set
  FD_SET(listener, &master);

  fdmax = listener;

  // Main loop (This is teh shiet!)
  for(;;) {
    read_fds = master; // copy master to read_fds so we won't ruin master
    if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
      perror("select");
      exit(4);
    }

    // loop through existing connections and look for data to read
    for (i = 0; i <= fdmax; i++) {
      if (FD_ISSET(i, &read_fds)) { // We have a connection!!
	if (i == listener) {
	  // New connection
	  remote_addr_len = sizeof remote_addr;
	  newfd = accept(listener, (struct sockaddr *)&remote_addr, &remote_addr_len);
	  
	  if (newfd == -1) {
	    perror("accept");
	  }
	  else {
	    FD_SET(newfd, &master); // Add new connection to master set
	    if (newfd > fdmax) {
	      fdmax = newfd;
	    }
	    printf("* New connection from %s on "
		   "socket %d\n",
		   inet_ntop(remote_addr.ss_family, 
			     get_in_addr((struct sockaddr*)&remote_addr),
			     remoteIP, INET6_ADDRSTRLEN), newfd);
	  }
	}
	else {
	  // We got data from a client
	  struct ParsedHeader t;
	  // Buffer the GET header from client
	  char * buffer = malloc(1);
	  int buffer_size = header_buffer(i, &buffer);
	  if (buffer_size == -1)
	    break;
	    
	  // Parse header to get hostname
	  t = header_parser(buffer);
	  sndfd = connect_to(t.host);
	  fprintf(stderr, "Hostname: %s\n", t.host);

	  
	  // Filter keywords from GET request
	  if (validate(buffer) == -1) {
	    char * redirect = "HTTP/1.1 302 Found\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error1.html\r\nConnection: close\r\n\r\n";
	    send_to(i, redirect, strlen(redirect));
	    close(sndfd);
	    close(i);
	    FD_CLR(i, &master);
	    break;
	  }	    
	  else { 
	    if(send_to(sndfd, buffer, buffer_size) == -1)
	      break;
	  }
	  //fprintf(stderr, "%s", t.header);

	  
	  // Free up allocated memory
	  free(t.t_header);
	  free(buffer);
	  buffer_size = 0;
	  
	  // BUFFER HEADER

	  buffer = malloc(0);

	  buffer_size = header_buffer(sndfd, &buffer);
	  if (buffer_size == -1) {
	    break; 
	  }
	  //fprintf(stderr, "%s", buffer);
	  
	  
	  // Filter the header from keywords
	  if (validate(buffer) != -1) {
	  
	   
	    // Check if content is text
	    if (is_text(buffer) != -1) {
	  
	      // Content is text.. Lets buffer it.
	      if ((buffer_size = content_buffer(sndfd, &buffer, buffer_size)) == -1)
		break;

	      
	      // Check for keywords in content
	      if (validate(buffer) != -1) {
	  
		
		// Content is ok. Lets send it
		if(send_to(i, buffer, buffer_size) == -1)
		  break;
	      }
	      else {
	  
		// Content is not ok. Redirectiong
		char * redirect = "HTTP/1.1 302 Found\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error2.html\r\n\r\n";
	    if(send_to(i, redirect, strlen(redirect)) == -1)
	      break;
	      }
	    }
	    else {

	      // Header is ok and the content is not plain text. Send the data.
	      if(send_to(i, buffer, buffer_size) == -1)
		break;
	      char temp[1501];
	      int temp_size;
	      for (;;) {
		if ((temp_size = recv(sndfd, temp, sizeof temp, 0)) == -1)
		  break;
		if(send_to(i, temp, temp_size) == -1)
		  break;
		if (temp_size == 0)
		  break;
	      }
	    }
	  }
	  else {
	    
	  // The url is not ok. Redirecting..
	    char * redirect = "HTTP/1.1 302 Found\r\nLocation: http://www.ida.liu.se/~TDTS04/labs/2011/ass2/error1.html\r\n\r\n";
	    if(send_to(i, redirect, strlen(redirect)) == -1)
	      break;
	  }
	  // Free up memory and close file descriptors..
	  free(buffer);
	  close(i);
	  close(sndfd);
	  FD_CLR(i, &master);
	  break;
	} // Closes data transfer
      } // Closes current connection
    } // Closes fd loop
    printf("* Done..\n");
  } // Closes main loop
  return 0;
}


/* 
   NET-NINNY 0.1
   
   A simple proxy using TCP
   Used to filter keywords
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

int main () {
  // Declarations of variables
  int sockfd, newsockfd, portno, status;
  socklen_t client_len;
  char buffer[256];

  struct addrinfo *result, hints;
  struct sockaddr_in client_addr;

  // Hints for the getaddrinfo() function
  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_flags = AI_PASSIVE;
  hints.ai_socktype = SOCK_STREAM;
  
  // Creates a addrinfo struct using hints and returns to result
  if((status = getaddrinfo(NULL, "1337", &hints, &result)) != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
  
  // Typecast result into sockaddr_in and prints portnumber
  unsigned short int port = ntohs(((struct sockaddr_in *)(result->ai_addr))->sin_port);
  printf("Port: %hu\n", port);
  
  // Creating a socket
  sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
  
  // Typecasts result into sockaddr_in and binds the portnumber.
  struct sockaddr_in *addr = ((struct sockaddr_in *)(result->ai_addr));
  if((status = bind(sockfd, ((struct sockaddr *)addr), result->ai_addrlen)) == -1) {
    fprintf(stderr, "bind error: %s\n", gai_strerror(status));
    exit(1);
  }
  
  // Starts to listen on portnumber
  listen(sockfd, 5);

  client_len = sizeof(client_addr);
  newsockfd = accept(sockfd, (struct sockaddr * ) &client_addr, &client_len);
  
  int bytes_read = 0;
  while(1) {
    if ((bytes_read = recv(newsockfd, buffer, 255, 0)) <= 0) {
      break;
    }
    
    buffer[bytes_read] = '\0';
    printf("Message: %s\n", buffer);
    
    send(newsockfd, "I got your message\r\n", 20, 0);
  }
  
  close(newsockfd);
  close(sockfd);
  return 0;
}

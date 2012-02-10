#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>

#include "communications.h"

int send_to(int fd, char * buffer, int buffer_size) {
  int bytes_sent = 0;
  while(bytes_sent < buffer_size) {
    if((bytes_sent += send(fd, buffer+bytes_sent,
                           buffer_size-bytes_sent, MSG_NOSIGNAL)) == -1) {
      close(fd);
      return -1;
    }
  }
  return 0;
}

int header_buffer(int fd, char ** buffer) {
  int buffer_size = 0;
  int temp_size = 0;
  char temp[500];
  memset(temp, 0, sizeof temp);
  for(;;) {
    if((temp_size = recv(fd, temp, sizeof temp, 0)) == -1) {
      buffer[buffer_size] = '\0';
      close(fd);
      return -1;
    }
    *buffer = realloc(*buffer, buffer_size+temp_size+1);
    temp[temp_size] = '\0';
    memcpy(*buffer+buffer_size, temp, temp_size);
    buffer_size += temp_size;
    if(strstr(*buffer, "\r\n\r\n") != NULL) {
      return buffer_size;
    }
  }
}

int content_buffer(int fd, char ** buffer, int buffer_size) {
  int temp_size = 0;
  char temp[1501];
  for(;;) {
    if((temp_size = recv(fd, temp, sizeof temp, 0)) == -1) {
      buffer[buffer_size] = '\0';
      close(fd);
      return -1;
    }
    *buffer = realloc(*buffer, buffer_size+temp_size+1);
    temp[temp_size] = '\0';
    memcpy(*buffer+buffer_size, temp, temp_size);
    buffer_size += temp_size;
    if(temp_size == 0) {
      return buffer_size;
    }
  }
}

int connect_to(char * addr) {
  struct addrinfo hints, *result;
  int fd;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  
  getaddrinfo(addr, "80", &hints, &result);
  fd = socket(result->ai_family,
              result->ai_socktype,
              result->ai_protocol);
  connect(fd, result->ai_addr, result->ai_addrlen);

  return fd;
}

#include <string.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "communications.h"

int send_to(int fd, char * buffer, int buffer_size) {
  int bytes_sent = 0;
  while(bytes_sent < buffer_size) {
    if((bytes_sent += send(i, buffer+bytes_sent,
                           buffer_size-bytes_sent, 0)) == -1) {
      close(fd);
      return -1;
    }
  }
  return 0;
}

int header_buffer(int fd, char * buffer) {
  int buffer_size = 0;
  int temp_size = 0;
  char temp[1501];
  for(;;) {
    if((temp_size = recv(fd, temp, sizeof temp, 0)) == -1) {
      close(fd);
      return -1;
    }
    buffer = realloc(buffer, buffer_size+temp_size+1);
    memcpy(buffer+buffer_size, temp, temp_size);
    buffer_size += temp_size;
    buffer[buffer_size] = '\0';
    if(strstr(buffer, "\r\n\r\n") != NULL) {
      return buffer_size;
    }
  }
}

int content_buffer(char* buffer, int buffer_size) {
  int temp_size = 0;
  char temp[1501];
  for(;;) {
    if((temp_size = recv(fd, temp, sizeof temp, 0)) == -1) {
      close(fd);
      return -1;
    }
    buffer = realloc(buffer, buffer_size+temp_size+1);
    memcpy(buffer+buffer_size, temp, temp_size);
    buffer_size += temp_size;
    buffer[buffer_size] = '\0';
    if(temp_size == 0) {
      return buffer_size;
    }
  }
}

int connect_to(char * addr) {
  struct addrinfo hints, &result;
  
  getaddrinfo(addr, "80", &hints, &result);
  fd = socket(result->ai_family,
              result->ai_socktype,
              result->ai_protocol);
  connect(fd, result->ai_addr, result->ai_addrlen);

  return fd;
}

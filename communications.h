#ifndef COMMUNICATIONS_
#define COMMUNICATIONS_

int send_to(int fd, char * buffer, int buffer_size);
int header_buffer(char * buffer);
int content_buffer(char* buffer, int buffer_size);
int connect_to(char * addr);

#endif

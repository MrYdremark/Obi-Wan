#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "header_parser.h"

void to_lower(char * in, char * out, int size) {
  for(int i = 0; i < size; i++) {
    out[i] = tolower(in[i]);
  }
}

struct ParsedHeader header_parser(char * input) {
  struct ParsedHeader h;
  char end = '\r';
  char * header = malloc(strlen(input));
  char *result_begin, *begin = "host:";
  size_t result_size = 0;
  memcpy(header, input, strlen(input));
  to_lower(input, header, strlen(input));
  result_begin = (strstr(header, begin) + 6);
  char * result_end = strchr(result_begin, end);
  result_end[0] = '\0';
  //char result[strlen(result_begin)] = result_begin;
  free(header);
  
  h.header = input;
  h.host = result_begin;
  
  return h;
}

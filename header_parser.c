#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <regex.h>

#include "header_parser.h"

void to_lower(char * in, char * out, int size) {
  for(int i = 0; i < size; i++) {
    out[i] = tolower(in[i]);
  }
}

struct ParsedHeader header_parser(char * input) {
  struct ParsedHeader h;
  int status;
  regex_t regex;
  char * result_begin = NULL;

  regcomp(&regex, "host:", REG_EXTENDED|REG_ICASE|REG_NOSUB);
  if((status = regexec(&regex, input, (size_t) 0, NULL, 0)) == 0) {
    char end = '\r';
    char * header = malloc(strlen(input));
    char *begin = "host:";
    size_t result_size = 0;
    memcpy(header, input, strlen(input));
    to_lower(input, header, strlen(input));
    result_begin = (strstr(header, begin) + 6);
    char * result_end = strchr(result_begin, end);
    result_end[0] = '\0';
    //char result[strlen(result_begin)] = result_begin;
    free(header);
  }
  
  regfree(&regex);
  
  h.header = input;
  h.host = result_begin;
  
  return h;
}

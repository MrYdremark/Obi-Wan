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
    char * begin = "host:";
    char * header = malloc(strlen(input)+1);

    size_t result_size = 0;
    memcpy(header, input, strlen(input));
    to_lower(input, header, strlen(input));
    result_begin = (strstr(header, begin) + 6);
    
    // Delete the Proxy-Connection part
    char *pBegin; 
    char *pEnd;
    pBegin = strstr(input, "Proxy-Connection:");
    if (pBegin)
    {
      pEnd = strchr(pBegin, end) + 2;
      strcpy(pBegin, pEnd);
    }
    
    // Delete the Accept-Encoding part
    pBegin = NULL;
    pBegin = strstr(input, "Accept-Encoding:");
    if (pBegin)
    {
      pEnd = strchr(pBegin, end) + 2;
      strcpy(pBegin, pEnd);
    }
    char * result_end = strchr(result_begin, end);
    result_end[0] = '\0';
    
    // Send this to free later.
    h.t_header = header;
  }
  regfree(&regex);
  
  h.header = input;
  h.host = result_begin;
  
  return h;
}

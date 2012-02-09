/**********************************************************************
 *                              header_parser                         * 
 * Getting header in form of string and checking for host name and    *
 * puts it in a struct. It is also stripping the header of it's       *
 * keep-alive.                                                        *
 *                                                                    *
 *                                 is_text                            *
 * Also getting header in form of a string. Using a regex to check    *
 * if it contains: Content-Type: text                                 *
 *                                                                    *
 *                                 to_lower                           *
 * This part is self-explanatory.                                     *
 **********************************************************************/

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
    pBegin = strstr(input, "Accept-Encoding:") + 17;
    char * encoding = "*;q=0";
    if (pBegin)
    {
      pEnd = strchr(pBegin, end);
      strcpy(pBegin, encoding);
      strcpy(pBegin+5, pEnd);
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

int is_text(char * input) {
  int status;
  regex_t regex;
  regcomp(&regex, "content-type: text", REG_EXTENDED|REG_ICASE|REG_NOSUB);
  status = regexec(&regex, input, (size_t) 0, NULL, 0);
  regfree(&regex);
  return status;
}

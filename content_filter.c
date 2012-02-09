/******************************************************
 *                validate                            *
 * Takes one argument in form of a string and uses a  *
 * regex to check for inapropriate words in string    *
 *                                                    *
 * NOTE: returns 1 (one) if the string is OK          *
 * (no inapropriate words found) and 0 (zero) if it's *
 * NOT OK (inapropriate words found)                  *
 ******************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <regex.h>

#include "content_filter.h"

int validate(char * input) {
  int status;
  regex_t regex;
  regcomp(&regex, "spongebob|britney spears|norrköping|paris hilton",
          REG_EXTENDED|REG_ICASE|REG_NOSUB);
  status = (regexec(&regex, input, (size_t) 0, NULL, 0));
  regfree(&regex);
  return status;
}

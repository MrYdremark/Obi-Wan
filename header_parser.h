#ifndef HEADER_PARSER_
#define HEADER_PARSER_

struct ParsedHeader {
  char * host;
  char * header;
  char * t_header;
  int header_size;
};

void to_lower(char * in, char * out, int size);
struct ParsedHeader header_parser(char * input);
int is_text(char * input);

#endif

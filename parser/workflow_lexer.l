%{
#include <stdlib.h>
#define YYSTYPE char *
#include "workflow_yacc.h"
#include "workflow_parser.h"
%}
%%
desc                                              return DESC;
csed                                              return CSED;
\-\>                                              return RIGHT_ARROW;
[0-9]+                  yylval = strdup(yytext);  return NUMBER;
=                                                 return '=';
\"([^\"]|\\\"|\\n)+\"   yylval = strdup(yytext);  return ARGUMENT_IN_QUOTES;
[^ \t\n\#]+             yylval = strdup(yytext);  return ARGUMENT;
\#.*                    /* Игнорируем комментарии. */
[ \n\t]+                /* Игнорируем пробельные символы. */
%%

#define STREAM_BUFFER_SIZE 1024

char* streamBuffer[STREAM_BUFFER_SIZE + 2];

void notifyNewStream(void) {
  yywrap();
  BEGIN(INITIAL);
}

int yywrap() {
  size_t size = nextBuffer(streamBuffer, STREAM_BUFFER_SIZE);
  if (!size)
  return 1;
  // Ограничители для FLEX
  streamBuffer[size] = '\0';
  streamBuffer[size + 1] = '\0';
  YY_BUFFER_STATE bs = yy_scan_buffer(streamBuffer, size + 2);
  yy_switch_to_buffer(bs);
  return 0;
}

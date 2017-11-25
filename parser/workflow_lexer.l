%{
#include <stdlib.h>
#define YYSTYPE char *
#include "workflow_yacc.h"
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
[ \n\t]+                  /* Игнорируем пробельные символы. */
%%


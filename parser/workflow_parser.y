%{
#define YYSTYPE char *

#include <stdio.h>
#include <stdlib.h>
#include "yacc_parser.h"
#include "workflow_parser.h"

int yylex(void);
void yyerror(const char *s);
%}

%token NUMBER
%token DESC
%token CSED
%token ARGUMENT
%token ARGUMENT_IN_QUOTES
%token RIGHT_ARROW

%%

workflow:
        description instructions { pushEOF(); }
;

instructions: 
            NUMBER { pushNumber(atoi($1)); }
            | instructions RIGHT_ARROW NUMBER { pushNumber(atoi($3)); }
;

description: 
           DESC blocks CSED { pushDescEnd(); }
;

blocks: /* empty */
      | blocks block
;

block:
     NUMBER '=' command { pushNumber(atoi($1)); }
;

command:
       ARGUMENT args_list { pushCommand($1); }
;

args_list: /* empty */
         | args_list ARGUMENT_IN_QUOTES { pushArg($2); }
         | args_list ARGUMENT { pushArg($2); }
;

%%

void yyerror(const char *s) {
  pushError(s);
}



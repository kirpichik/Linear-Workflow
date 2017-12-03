
%skeleton "lalr1.cc"
%require "3.0"
%defines
%define parser_class_name { BisonWorkflowParser }

%define api.token.constructor
%define api.value.type variant
%define parse.assert
%define api.namespace { wkfw }

%code requires {
  #include <iostream>
  #include <string>
  
  #include "parser_types.h"
  #include "workflow_parser.h"
  
  namespace wkfw {

    class FlexWorkflowLexer;
    class WorkflowParser;

  }

}

%code top {

  #include "lexer.h"
  
  static wkfw::BisonWorkflowParser::symbol_type yylex(wkfw::FlexWorkflowLexer& lexer) {
    return lexer.nextLexem();
  }

}

%lex-param { wkfw::FlexWorkflowLexer& lexer }

%parse-param { wkfw::FlexWorkflowLexer& lexer }
%parse-param { wkfw::WorkflowParser& driver }

%define parse.trace
%define parse.error verbose

%define api.token.prefix {TOKEN_}

%token <size_t> NUMBER
%token DESC
%token CSED
%token <std::string> IDENT
%token <std::string> STRING
%token EQUAL
%token ARROW
%token END

%type< wkfw::WorkflowCommand > block;
%type< std::vector<std::string> > args_list;

%start workflow

%%

workflow:
        description instructions { YYACCEPT; }
;

instructions:
            NUMBER { driver.receiveInstruction($1); }
            | instructions ARROW NUMBER { driver.receiveInstruction($3); }
;

description:
            DESC blocks CSED
;

blocks: /* empty */
      | blocks block
;

block:
      NUMBER EQUAL args_list {
        size_t num = $1;
        std::string name = $3[0];
        std::vector<std::string> args($3.begin() + 1, $3.end());
        wkfw::WorkflowCommand cmd(num, name, args);
        driver.receiveCommand(cmd);
      }
;

args_list: IDENT { $$ = std::vector<std::string>(); $$.push_back($1); }
          | args_list STRING {
            std::vector<std::string>& args = $1;
            args.push_back($2);
            $$ = args;
          }
          | args_list IDENT {
            std::vector<std::string>& args = $1;
            args.push_back($2);
            $$ = args;
          }
;

%%

void wkfw::BisonWorkflowParser::error(const std::string& message) {
  driver.receiveError(message);
}

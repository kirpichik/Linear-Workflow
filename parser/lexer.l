
%{

#include <iostream>
#include <cstdlib>

#include "lexer.h"

#define yyterminate() wkfw::BisonWorkflowParser::make_END();

#define YY_USER_ACTION

%}

%option noyywrap
%option c++
%option yyclass="FlexWorkflowLexer"
%option prefix="wkfw_"

%%

desc {
  return wkfw::BisonWorkflowParser::make_DESC();
}

csed {
  return wkfw::BisonWorkflowParser::make_CSED();
}

\-\> {
  return wkfw::BisonWorkflowParser::make_ARROW();
}

[0-9]+ {
  /* Число. */
  return wkfw::BisonWorkflowParser::make_NUMBER((size_t) atoi(yytext));
}

\= {
  return wkfw::BisonWorkflowParser::make_EQUAL();
}

\"([^\"]|\\\"|\\n)+\" {
  /* Аргументы команды в кавычках. */
  std::string res(yytext);
  std::string retur = res.substr(1, res.length() - 2);
  return wkfw::BisonWorkflowParser::make_STRING(retur);
}

[^ \t\n\#]+ {
  /* Аргументы команды без кавычек или название команды. */
  return wkfw::BisonWorkflowParser::make_IDENT(yytext);
}

\#.* {
  /* Игнорируем комментарии. */
}

[ \n\t]+ {
  /* Игнорируем пробельные символы. */
}

<<EOF>> {
  return yyterminate();
}

%%

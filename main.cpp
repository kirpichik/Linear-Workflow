//
//  main.cpp
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include <iostream>

#include "yacc_parser.h"

//#include "worker.h"

int main(int argc, const char* argv[]) {
  return yyparse();
}

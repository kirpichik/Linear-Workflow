//
//  workflow_parser.cpp
//  Workflow
//
//  Created by Кирилл on 25.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include "workflow_parser.h"

#include <iostream>
#include <string>

void pushArg(const char* arg) {
  std::cout << arg << ", ";
}

void pushCommand(const char* cmd) {
  std::cout << "COMMAND: " << cmd;
}

void pushNumber(int num) {
  std::cout << " = " << num << std::endl;
}

void pushDescEnd() {
  std::cout << "\n!CSED!" << std::endl;
}

void pushError(const char* msg) {
  std::cout << msg;
}


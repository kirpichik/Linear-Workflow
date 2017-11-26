//
//  main.cpp
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include <iostream>
#include <fstream>

#include "workflow.h"

int main(int argc, const char* argv[]) {
  std::ifstream file("input.wkfw");
  try {
    wkfw::Workflow workflow(file);
    workflow.executeLazy();
  } catch(wkfw::InvalidConfigurationException& e) {
    std::cout << e.what() << std::endl;
  }
  return 0;
}

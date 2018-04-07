//
//  main.cpp
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "workflow.h"

int main(int argc, const char* argv[]) {
  std::vector<std::string> args(argv + 1, argv + argc);
  std::string inputFilename;
  std::string outputFilename;
  std::string workflowInput;

  // Разбор аргументов командной строки
  for (auto i = args.begin(); i < args.end(); i++) {
    if ((*i)[0] != '-') {  // Не название опции
      if (workflowInput != "") {
        std::cerr << "To many workflow input files." << std::endl;
        return 1;
      }
      workflowInput = *i;
    } else if ((i + 1) == args.end() ||
               (*(i + 1))[0] == '-') {  // Опции с аргументами
      std::cerr << "Option " << *i << " is not set." << std::endl;
      return 1;
    } else if ((*i) == "-i") {
      if (inputFilename != "") {
        std::cerr << "To many input files." << std::endl;
        return 1;
      }
      inputFilename = *++i;
    } else if ((*i) == "-o") {
      if (outputFilename != "") {
        std::cerr << "To many output files." << std::endl;
        return 1;
      }
      outputFilename = *++i;
    } else {
      std::cerr << "Unknown option: " << *i << std::endl;
      return 1;
    }
  }

  // Проверка наличия параметров
  if (workflowInput == "") {
    std::cerr << "No workflow input file." << std::endl;
    return 1;
  }

  std::ifstream file(workflowInput);
  if (!file.is_open()) {
    std::cerr << "Cannot open file: \"" << workflowInput << "\"" << std::endl;
    return 1;
  }

  try {
    wkfw::Workflow workflow(file, inputFilename, outputFilename);
    workflow.execute();
  } catch (const wkfw::InvalidWorkflowException& e) {
    std::cerr << "InvalidWorkflowException: " << e.what() << std::endl;
  } catch (const wkfw::WorkerExecuteException& e) {
    std::cerr << "WorkerExecuteException: " << e.what() << std::endl;
  }

  file.close();
  return 0;
}

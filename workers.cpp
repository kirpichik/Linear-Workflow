//
//  workers.cpp
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include <algorithm>
#include <fstream>
#include <vector>

#include "workers.h"

namespace workers {

const WorkerResult ReadFile::execute(const WorkerResult& previous) {
  std::ifstream input;
  std::vector<std::string> list;

  input.exceptions(std::ifstream::failbit | std::ifstream::badbit);

  try {
    input.open(filename);
    std::string line;
    while (std::getline(input, line)) list.push_back(line);
    input.close();
  } catch (std::ifstream::failure& e) {
    throw WorkerExecuteException("Cannot read lines from file \"" + filename +
                                 "\"");
  }

  return WorkerResult(list);
}

const WorkerResult WriteFile::execute(const WorkerResult& previous) {
  std::ofstream output;

  output.exceptions(std::ofstream::failbit | std::ofstream::badbit);

  try {
    output.open(filename);
    for (auto line : previous.getValue()) output << line << std::endl;
    output.close();
  } catch (std::ofstream::failure& e) {
    throw WorkerExecuteException("Cannot write lines to file \"" + filename +
                                 "\"");
  }

  return WorkerResult();
}

const WorkerResult Grep::execute(const WorkerResult& previous) {
  std::vector<std::string> list;

  for (auto line : previous.getValue())
    if (line.find(pattern) != -1) list.push_back(line);

  return WorkerResult(list);
}

const WorkerResult Sort::execute(const WorkerResult& previous) {
  std::vector<std::string> list = previous.getValue();

  std::sort(list.begin(), list.end());

  return WorkerResult(list);
}

/**
 * Заменяет подстроки в с троке.
 *
 * @param str Исходная строка
 * @param pattern Паттерн для замены
 * @param substitution Замена для паттерна
 *
 * @return Строка с примененными заменами.
 */
static std::string replace(const std::string& str, const std::string& pattern,
                           const std::string& substitution) {
  std::string result(str);
  size_t index = 0;
  while (true) {
    index = str.find(pattern, index);
    if (index == std::string::npos) break;
    result.replace(index, pattern.size(), substitution);
    index += substitution.size();
  }
  return result;
}

const WorkerResult Replace::execute(const WorkerResult& previous) {
  std::vector<std::string> list;
  
  for (auto line : previous.getValue())
    list.push_back(replace(line, pattern, substitution));
  
  return WorkerResult(list);
}

const WorkerResult Dump::execute(const WorkerResult& previous) {
  WriteFile::execute(previous);
  
  return WorkerResult(previous.getValue());
}

}  // namespace workers

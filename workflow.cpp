//
//  workflow.cpp
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include "workflow.h"

/*
 
 Автомат для разбора блока описания:
 
 1. desc
 2. \n
 3. [0-9]+
 4. =
 5. ((readfile)|(writefile)|(grep)|(sort)|(replace)|(dump))
 6. .*
 7. \#.*
 8. csed
 
 1. -> 2, 7, 8
 2. -> 1, 2, 3, 7, 8
 3. -> 4
 4. -> 5
 5. -> 6, 2, 7
 6. -> 2, 7
 7. -> 2
 8. -> end
 
 Автомат для разбора блока инструкций:
 
 1. [0-9]+
 2. (->)
 3. \n
 4. EOF
 
 1. -> 2, 3, 4
 2. -> 1, 3
 3. -> 1, 2, 3, 4
 4. -> end
 
*/

namespace wkfw {
  
//static const std::string
static const std::string DESCRIPTION_BLOCK_BEGIN = "desc";
static const std::string DESCRIPTION_BLOCK_END = "csed";

/**
 * Удаляет из строки комментарии и лишние пробелы.
 * Подготавливает строку для разбора.
 * Комментарием считается место от символа решетки '#' до конца строки.
 */
static std::string prepareLine(const std::string& line) {
  std::string result(line);
  return result.substr(0, result.find("#"));
}
  
DescriptionParser::DescriptionParser(std::istream& stream) throw(
    InvalidDescriptionException) {
  stream.exceptions(std::istream::failbit | std::istream::badbit);
  
  size_t lineNumber = 0;
  
  try {
    std::string line;
    while (!stream.eof() && std::getline(stream, line)) {
      lineNumber++;
      line = prepareLine(line);
      if (line.empty() || line == "desc")
      parseDescriptionLine(line, lineNumber);
    }
  } catch (std::istream::failure& e) {
    if (!stream.eof())
      throw InvalidDescriptionException(lineNumber, 0);
  }
}
  
void DescriptionParser::parseDescriptionLine(const std::string line, const size_t lineNumber) throw(InvalidDescriptionException) {
  
}

const Worker* DescriptionParser::getWorkerById(const size_t ident) const {
  auto found = description.find(ident);
  return found == description.end() ? nullptr : found->second;
}

DescriptionParser::~DescriptionParser() {
  for (auto const& worker : description) delete worker.second;
}

const Worker* LazyInstructionParser::nextInstruction() throw(
    InvalidInstructionException) {
  // TODO - разбор и валидация инструкций
  return nullptr;
}

ValidateInstructionParser::ValidateInstructionParser(
    const DescriptionParser& desc,
    std::istream& stream) throw(InvalidInstructionException)
    : InstructionParser(desc) {
  // TODO - разбор и валидация инструкций
}

const Worker* ValidateInstructionParser::nextInstruction() throw(
    InvalidInstuctionsSequenceException) {
  if (position >= instructions.size()) return nullptr;
  return description.getWorkerById(instructions[position++]);
}

Workflow::Workflow(std::istream& stream) throw(InvalidConfigurationException)
    : description(DescriptionParser(stream)), stream(stream) {}

void Workflow::executeLazy() throw(
    InstructionParser::InvalidInstructionException) {
  LazyInstructionParser parser(description, stream);
  execute(parser);
}

void Workflow::executeNormal() throw(
    InstructionParser::InvalidInstructionException) {
  ValidateInstructionParser parser(description, stream);
  execute(parser);
}

void Workflow::execute(InstructionParser& parser) throw(
    InstructionParser::InvalidInstructionException) {
  WorkerResult lastResult;
  Worker const* worker;

  while ((worker = parser.nextInstruction()))
    lastResult = worker->execute(lastResult);
}

}  // namespace wkfw

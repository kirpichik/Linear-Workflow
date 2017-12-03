//
//  workflow_parser.cpp
//  Workflow
//
//  Created by Кирилл on 25.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include <sstream>

#include "workflow_parser.h"
#include "workers.h"

namespace wkfw {

WorkflowParser::WorkflowParser(std::istream& stream) throw(InvalidWorkflowException) {
  FlexWorkflowLexer lexer;
  lexer.switch_streams(&stream);
  
  BisonWorkflowParser parser(lexer, *this);
  
  if (parser.parse())
    throw InvalidWorkflowException("Parse error: " + errorMsg);
}

const Worker* WorkflowParser::getWorkerById(const size_t ident) const {
  auto found = description.find(ident);
  return found == description.end() ? nullptr : found->second;
}

WorkflowParser::WorkflowParser() {
  for (auto const& worker : description)
    delete worker.second;
}
  
const Worker* WorkflowParser::nextInstruction() {
  return position < instructions.size() ? getWorkerById(instructions[position++]) : nullptr;
}
  
void WorkflowParser::resetSteps() {
  position = 0;
}
  
  void WorkflowParser::receiveCommand(const wkfw::WorkflowCommand& cmd) throw(wkfw::InvalidWorkflowException) {
  if (description.find(cmd.instructionNumber) != description.end())
    throw InvalidWorkflowException("Repeating instruction numbers in description block.");
  
  const Worker* worker = workers::constructWorker(cmd.instructionNumber, cmd.name, cmd.args);
  if (worker == nullptr) {
    std::ostringstream str;
    str << "Cannot find instruction " << cmd.instructionNumber << " = ";
    str << "\"" << cmd.name << "\" with " << cmd.args.size() << " args.";
    throw InvalidWorkflowException(str.str());
  }
  
  description[cmd.instructionNumber] = worker;
}
  
void WorkflowParser::receiveInstruction(const size_t num) {
  instructions.push_back(num);
}
  
void WorkflowParser::receiveError(const std::string& msg) throw(wkfw::InvalidWorkflowException) {
  errorMsg = msg;
}

}

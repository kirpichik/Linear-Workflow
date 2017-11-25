//
//  workflow.cpp
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include "workflow.h"

namespace wkfw {

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

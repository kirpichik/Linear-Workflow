//
//  workflow.cpp
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include "workflow.h"

namespace wkfw {

Workflow::Workflow(std::istream& stream, InstructionParserType type) throw(InvalidConfigurationException) : type(type), description(DescriptionParser(stream)), stream(stream) {
  if (type == LAZY)
    instructionParser = new LazyInstructionParser(description, stream);
  else
    instructionParser = new ValidateInstructionParser(description, stream);
}

void Workflow::execute() throw(InstructionParser::InvalidInstructionException) {
  WorkerResult lastResult;
  Worker const* worker;

  while ((worker = instructionParser->nextInstruction()))
    lastResult = worker->execute(lastResult);
}
  
Workflow::~Workflow() {
  if (instructionParser)
    delete instructionParser;
}

}  // namespace wkfw

//
//  workflow.cpp
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include "workflow.h"
#include "workers.h"

namespace wkfw {

Workflow::Workflow(std::istream& stream, const InstructionParserType type, const std::string& ifname, const std::string& ofname) throw(InvalidConfigurationException) : description(DescriptionParser(stream)), stream(stream), ifname(ifname), ofname(ofname) {
  if (type == LAZY)
    instructionParser = new LazyInstructionParser(description, stream);
  else
    instructionParser = new ValidateInstructionParser(description, stream);
}

void Workflow::execute() throw(InstructionParser::InvalidInstructionException, wkfw::WorkerExecuteException) {
  WorkerResult lastResult;
  Worker const* worker = instructionParser->nextInstruction();
  
  if (worker == nullptr)
    return;
  
  // Проверяем наличие чтения из файла
  if (worker->getAcceptType() != WorkerResult::NONE) {
    if (ifname == "")
      throw InstructionParser::InvalidInstuctionsSequenceException("No input file set.");
    workers::ReadFile reader(0, ifname);
    lastResult = reader.execute(lastResult);
  }

  do
    lastResult = worker->execute(lastResult);
  while ((worker = instructionParser->nextInstruction()));
  
  // Проверяем наличие записи в файл
  if (lastResult.getType() != WorkerResult::NONE) {
    if (ofname == "")
      throw InstructionParser::InvalidInstuctionsSequenceException("No output file set.");
    workers::WriteFile writer(0, ofname);
    writer.execute(lastResult);
  }
}
  
Workflow::~Workflow() {
  if (instructionParser)
    delete instructionParser;
}

}  // namespace wkfw

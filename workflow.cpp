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

Workflow::Workflow(std::istream& stream, const std::string& ifname, const std::string& ofname) throw(InvalidWorkflowException) : parser(WorkflowParser(stream)), ifname(ifname), ofname(ofname) {}

void Workflow::execute() throw(WorkerExecuteException) {
  WorkerResult lastResult;
  Worker const* worker = parser.nextInstruction();
  
  if (worker == nullptr)
    return;
  
  // Проверяем наличие чтения из файла
  if (worker->getAcceptType() != WorkerResult::NONE) {
    if (ifname == "")
      throw WorkerExecuteException("No input file set.");
    workers::ReadFile reader(0, ifname);
    lastResult = reader.execute(lastResult);
  }

  // Выполняем инструкции
  do
    lastResult = worker->execute(lastResult);
  while ((worker = parser.nextInstruction()));
  
  // Проверяем наличие записи в файл
  if (lastResult.getType() != WorkerResult::NONE) {
    if (ofname == "")
      throw WorkerExecuteException("No output file set.");
    workers::WriteFile writer(0, ofname);
    writer.execute(lastResult);
  }
}

}  // namespace wkfw

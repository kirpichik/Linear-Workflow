//
//  workflow.h
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#ifndef WORKFLOW_H_
#define WORKFLOW_H_

#include <exception>
#include <istream>
#include <map>
#include <string>
#include <vector>

#include "worker.h"
#include "workflow_parser.h"

namespace wkfw {

/**
 * Основной класс для работы Workflow.
 */
class Workflow {
 public:
  Workflow(std::istream& stream,
           const std::string& ifname,
           const std::string& ofname) throw(InvalidWorkflowException);

  /**
   * Запустить выполнение инструкций.
   * */
  void execute() throw(WorkerExecuteException);

 private:
  const std::string ifname;
  const std::string ofname;
  WorkflowParser parser;
};

}  // namespace wkfw

#endif /* WORKFLOW_H_ */

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
  /**
   * Типы парсеров инструкций.
   */
  enum InstructionParserType {
    LAZY,  // Читает инструкции по одной и валидирует их во время исполнения.
    NORMAL // Читает инструкции и валидирует их, после чего выполняет.
  };
  
  Workflow(std::istream& stream, const InstructionParserType type, const std::string& ifname, const std::string& ofname) throw(InvalidConfigurationException);

  /**
   * Запустить выполнение инструкций.
   * */
  void execute() throw(InstructionParser::InvalidInstructionException, wkfw::WorkerExecuteException);
  
  ~Workflow();

 private:
  const std::string ifname;
  const std::string ofname;
  std::istream& stream;
  const DescriptionParser description;
  InstructionParser* instructionParser;
};

}  // namespace wkfw

#endif /* WORKFLOW_H_ */

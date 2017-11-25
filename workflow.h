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
  Workflow(std::istream& stream) throw(InvalidConfigurationException);

  /**
   * Запустить выполнение используя ленивый парсер инструкций.
   */
  void executeLazy() throw(InstructionParser::InvalidInstructionException);

  /**
   * Запустить выполнение с предварительным парсингом и валидацией инструкций.
   */
  void executeNormal() throw(InstructionParser::InvalidInstructionException);

 private:
  std::istream& stream;
  const DescriptionParser description;

  /**
   * Запустить выполнение с данным парсером.
   *
   * @param parser Парсер для получения инструкций.
   * */
  void execute(InstructionParser& parser) throw(
      InstructionParser::InvalidInstructionException);
};

}  // namespace wkfw

#endif /* WORKFLOW_H_ */

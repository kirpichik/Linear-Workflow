//
//  workflow_parser.h
//  Workflow
//
//  Created by Кирилл on 25.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#ifndef WORKFLOW_PARSER_H_
#define WORKFLOW_PARSER_H_

#include <exception>
#include <string>
#include <map>
#include <vector>

#include "lexer.h"
#include "parser.hpp"
#include "parser_types.h"

#include "worker.h"

namespace wkfw {

/**
 * Бросается при любых ошибках разбора Workflow.
 */
class InvalidWorkflowException : public std::exception {
public:
  InvalidWorkflowException(const std::string& desc) : reason(desc) {}
  
  const char* what() const throw() override { return reason.c_str(); }
  
private:
  const std::string reason;
};

/**
 * Разбирает, проверяет на валидность и хранит описание Workflow.
 */
class WorkflowParser {
public:
  
  WorkflowParser(std::istream& stream) throw(InvalidWorkflowException);
  
  /**
   * Получает обработчик по его уникальному номеру.
   *
   * @param ident Уникальный номер обработчика.
   *
   * @return Обработчик по его номеру или nullptr,
   * если обработчика с данным номером не существует.
   */
  const Worker* getWorkerById(const size_t ident) const;
  
  /**
   * @return Следующая инструкция для исполнения Workflow.
   */
  const Worker* nextInstruction();
  
  /**
   * Сбрасывает счетчик шагов в начало.
   */
  void resetSteps();
  
  WorkflowParser();
  
  friend class FlexWorkflowLexer;
  friend class BisonWorkflowParser;
  
private:
  std::map<size_t, const Worker*> description;
  std::vector<size_t> instructions;
  size_t position = 0;
  std::string errorMsg;
  
  /**
   * Проверяет валидность команды и запоминает ее.
   *
   * @param cmd Команда
   */
  void receiveCommand(const WorkflowCommand& cmd) throw(InvalidWorkflowException);
  
  /**
   * Запоминает инструкцию/
   */
  void receiveInstruction(const size_t num);
  
  /**
   * Бросает исключение из парсера.
   */
  void receiveError(const std::string& msg) throw(InvalidWorkflowException);
};

}

#endif /* WORKFLOW_PARSER_H_ */

//
//  workflow_parser.h
//  Workflow
//
//  Created by Кирилл on 25.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#ifndef WORKFLOW_PARSER_H_
#define WORKFLOW_PARSER_H_

#ifdef __cplusplus
#define EXTERNC extern "C"
#else
#define EXTERNC
#endif

EXTERNC void pushArg(const char* arg);

EXTERNC void pushCommand(const char* cmd);

EXTERNC void pushNumber(int num);

EXTERNC void pushDescEnd(void);

EXTERNC void pushError(const char* msg);

EXTERNC void pushEOF(void);

/**
 * Записывает новые данные из потока в буфер.
 *
 * @param buff Буфер.
 * @param size Размер буфера.
 *
 * @return Количество данных записаных в буфер.
 */
EXTERNC size_t nextBuffer(char* buff, size_t size);

#ifdef __cplusplus

#include <exception>
#include <string>
#include <map>
#include <vector>
#include <list>

#include "worker.h"

namespace wkfw {

/**
 * Бросается при любых ошибках разбора конфигурации.
 */
class InvalidConfigurationException : public std::exception {
public:
  InvalidConfigurationException(const std::string& desc) : reason(desc) {}
  
  const char* what() const throw() override { return reason.c_str(); }
  
private:
  const std::string reason;
};

/**
 * Разбирает, проверяет на валидность и хранит блок описания Workflow.
 */
class DescriptionParser {
public:
  /**
   * Бросается при ошибке в блоке описания Workflow.
   */
  class InvalidDescriptionException : public InvalidConfigurationException {
   public:
    InvalidDescriptionException() : InvalidConfigurationException("Invalid symbol in description block") {}
   protected:
    InvalidDescriptionException(const std::string& desc) : InvalidConfigurationException(desc) {}
  };
  
  class InvalidDescriptionInstructionException : public InvalidDescriptionException {
   public:
    InvalidDescriptionInstructionException() : InvalidDescriptionException("Invalid instruction in description block") {}
  };
  
  DescriptionParser(std::istream& stream) throw(InvalidDescriptionException);
  
  /**
   * Получает обработчик по его уникальному номеру.
   *
   * @param ident Уникальный номер обработчика.
   *
   * @return Обработчик по его номеру или nullptr,
   * если обработчика с данным номером не существует.
   */
  const Worker* getWorkerById(const size_t ident) const;
  
  ~DescriptionParser();
  
private:
  std::map<size_t, Worker*> description;
};

/**
 * Интерфейс для получения дальнейших иструкций обработчику Workflow.
 */
class InstructionParser {
public:
  /**
   * Бросает при ошибке в блоке инструкций Workflow.
   */
  class InvalidInstructionException : public InvalidConfigurationException {
   public:
    InvalidInstructionException() : InvalidConfigurationException("Invalid symbol in instruction block") {}
   protected:
    InvalidInstructionException(const std::string& desc) : InvalidConfigurationException(desc) {}
  };
  
  /**
   * Ошибочная последовательность инструкций.
   */
  class InvalidInstuctionsSequenceException : public InvalidInstructionException {
  public:
    InvalidInstuctionsSequenceException() : InvalidInstructionException("Invalid instructions sequence in instruction block") {}
  };
  
  InstructionParser(const DescriptionParser& desc) throw(InvalidInstructionException)
  : description(desc) {}
  
  /**
   * @return Следующая инструкция для исполнения Workflow.
   */
  virtual const Worker* nextInstruction() throw(
  InvalidInstructionException) = 0;
  
  /**
   * Сбрасывает счетчик шагов в начало.
   */
  virtual void resetSteps() = 0;
  
  virtual ~InstructionParser() {}
  
protected:
  const DescriptionParser& description;
};

/**
 * Ленивый парсер инструкций.
 * Считывает следующую инструкцию по запросу и не проводит ее валидацию.
 */
class LazyInstructionParser : public InstructionParser {
public:
  LazyInstructionParser(const DescriptionParser& desc, std::istream& stream) throw(InvalidInstructionException);
  
  const Worker* nextInstruction() throw(InvalidInstructionException) override;
  
  void resetSteps() override;
  
private:
  WorkerResult::ResultType previousType = WorkerResult::NONE;
  std::istream& stream;
  std::vector<size_t> instructions;
  size_t position = 0;
};

/**
 * Считывает все инструкции при инициализации и проводит их валидацию.
 */
class ValidateInstructionParser : public InstructionParser {
public:
  ValidateInstructionParser(
                            const DescriptionParser& desc,
                            std::istream& stream) throw(InvalidInstructionException);
  
  const Worker* nextInstruction() throw(InvalidInstuctionsSequenceException) override;
  
  void resetSteps() override;
  
private:
  std::vector<size_t> instructions;
  size_t position = 0;
};

}

#endif /* __cplusplus */

#undef EXTERNC

#endif /* WORKFLOW_PARSER_H_ */

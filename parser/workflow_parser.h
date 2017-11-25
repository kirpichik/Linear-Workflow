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

#ifdef __cplusplus

#include <exception>
#include <string>
#include <map>
#include <vector>

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
 * Общий интерфейс для парсера, принимающего
 * данные от YACC.
 */
class YACCParser {
 public:
  virtual void pushCommand(const size_t ident, const std::string& name, const std::vector<std::string>& args) = 0;
  virtual void pushInstruction(const size_t number) = 0;
  virtual void pushError(const std::string& msg) {
    throwErrorState();
  }
 protected:
  /**
   * Поднять флаг ошибочного состояния.
   */
  void throwErrorState() {
    errorState = true;
  }
  
  /**
   * @return true, если поднят флаг ошибочного состояния.
   */
  bool isErrorState() {
    return errorState;
  }
 private:
  // Флаг выставляется в истину, когда во время разбора
  // последнего буфера происходит исключение.
  bool errorState = false;
};

/**
 * Разбирает, проверяет на валидность и хранит блок описания Workflow.
 */
class DescriptionParser : protected YACCParser {
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
  
  void pushCommand(const size_t ident, const std::string& name, const std::vector<std::string>& args) override;
  void pushInstruction(const size_t number) override;
};

/**
 * Интерфейс для получения дальнейших иструкций обработчику Workflow.
 */
class InstructionParser : protected YACCParser {
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
  
  InstructionParser(const DescriptionParser& desc) throw(InvalidInstructionException)
  : description(desc) {}
  
  /**
   * @return Следующая инструкция для исполнения Workflow.
   */
  virtual const Worker* nextInstruction() throw(
  InvalidInstructionException) = 0;
  
protected:
  const DescriptionParser& description;
};

/**
 * Ленивый парсер инструкций.
 * Считывает следующую инструкцию по запросу и не проводит ее валидацию.
 */
class LazyInstructionParser : public InstructionParser {
public:
  LazyInstructionParser(const DescriptionParser& desc,
                        std::istream& stream) throw(InvalidInstructionException)
  : InstructionParser(desc), stream(stream) {}
  
  const Worker* nextInstruction() throw(
  InvalidInstructionException) override;
  
private:
  WorkerResult::ResultType previousType = WorkerResult::ResultType::NONE;
  std::istream& stream;
  // Буфер инструкций
  std::vector<size_t> instructBuff;
  
  void pushCommand(const size_t ident, const std::string& name, const std::vector<std::string>& args) override;
  void pushInstruction(const size_t number) override;
};

/**
 * Считывает все инструкции при инициализации и проводит их валидацию.
 */
class ValidateInstructionParser : public InstructionParser {
public:
  class InvalidInstuctionsSequenceException
  : public InvalidInstructionException {
    InvalidInstuctionsSequenceException() : InvalidInstructionException("Invalid instructions sequence in instruction block") {}
  };
  
  ValidateInstructionParser(
                            const DescriptionParser& desc,
                            std::istream& stream) throw(InvalidInstructionException);
  
  const Worker* nextInstruction() throw(
  InvalidInstuctionsSequenceException) override;
  
private:
  std::vector<size_t> instructions;
  size_t position;
  
  void pushCommand(const size_t ident, const std::string& name, const std::vector<std::string>& args) override;
  void pushInstruction(const size_t number) override;
};

}

#endif /* __cplusplus */

#undef EXTERNC

#endif /* WORKFLOW_PARSER_H_ */

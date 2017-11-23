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

namespace wkfw {

/**
 * Бросается при любых ошибках разбора конфигурации.
 */
class InvalidConfigurationException : public std::exception {
 public:
  InvalidConfigurationException(const size_t line, const size_t coloumn,
                                const std::string desc)
      : reason(desc + " at line " + std::to_string(line) + " and position " +
               std::to_string(coloumn)) {}

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
    InvalidDescriptionException(const size_t line, const size_t coloumn)
        : InvalidConfigurationException(
              line, coloumn, "Invalid symbol in description block") {}
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
  
  /**
   * Разбирает строку из блока описания
   */
  void parseDescriptionLine(const std::string line, const size_t lineNumber) throw(InvalidDescriptionException);
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
    InvalidInstructionException(const size_t line, const size_t coloumn)
        : InvalidConfigurationException(
              line, coloumn, "Invalid symbol in instruction block") {}
    InvalidInstructionException(const size_t line, const size_t coloumn,
                                const std::string desc)
        : InvalidConfigurationException(line, coloumn, desc) {}
  };

  InstructionParser(const DescriptionParser& desc) throw(
      InvalidInstructionException)
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
  std::istream& stream;
};

/**
 * Считывает все инструкции при инициализации и проводит их валидацию.
 */
class ValidateInstructionParser : public InstructionParser {
 public:
  class InvalidInstuctionsSequenceException
      : public InvalidInstructionException {
    InvalidInstuctionsSequenceException(const size_t line, const size_t coloumn)
        : InvalidInstructionException(
              line, coloumn,
              "Invalid instructions sequence in instruction block") {}
  };

  ValidateInstructionParser(
      const DescriptionParser& desc,
      std::istream& stream) throw(InvalidInstructionException);

  const Worker* nextInstruction() throw(
      InvalidInstuctionsSequenceException) override;

 private:
  const std::vector<size_t> instructions;
  size_t position;
};

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

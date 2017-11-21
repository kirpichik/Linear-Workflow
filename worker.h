//
//  worker.h
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#ifndef WORKER_H_
#define WORKER_H_

#include <exception>
#include <string>
#include <vector>

/**
 * Исключение, бросаемое при попытке получить результат при его отсутствии.
 */
class NoResultException : public std::exception {
 public:
  const char* what() const throw() override { return "No result given!"; }
};

/**
 * Результат выполнения Worker-а.
 */
class WorkerResult {
 public:
  enum ResultType { NONE, TEXT };
  /**
   * Результат выполнения отсутствует.
   */
  WorkerResult() : type(NONE) {}

  /**
   * Результат выполнения - текст.
   *
   * @param value Результат выполнения.
   */
  WorkerResult(const std::vector<std::string>& value)
      : type(TEXT), value(value) {}
  
  WorkerResult(const WorkerResult& result) : type(result.type), value(result.value) {}
  
  WorkerResult& operator=(const WorkerResult& from) {
    type = from.type;
    value = from.value;
    return (*this);
  }
  
  bool operator==(const WorkerResult& other) const {
    return type == other.type && value == other.value;
  }
  
  bool operator!=(const WorkerResult& other) const {
    return type != other.type || value != other.value;
  }

  /**
   * @return Тип результата выполнения.
   */
  const ResultType getType() const { return type; }

  /**
   * @return Результат выполнения.
   */
  const std::vector<std::string> getValue() const throw(NoResultException) {
    if (type == NONE) throw NoResultException();
    return value;
  }

 private:
  ResultType type;
  std::vector<std::string> value;
};

/**
 * Блок схемы Workflow
 */
class Worker {
 public:
  /**
   * Выполняет обработчик блока.
   *
   * @param previous Результат выполнения предыдущего блока.
   * @return Результат выполнения данного блока.
   */
  virtual const WorkerResult execute(const WorkerResult& previous) = 0;
};

#endif /* WORKER_H_ */

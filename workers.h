//
//  workers.h
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#ifndef WORKERS_H_
#define WORKERS_H_

#include <string>

#include "worker.h"

/**
 * Набор реализаций обработчиков блоков.
 */
namespace workers {

/**
 * Считывание текстового файла в память, целиком.
 *
 * None -> Text
 */
class ReadFile : public Worker {
 public:
  ReadFile(const std::string& filename) : filename(filename) {}

  const WorkerResult execute(const WorkerResult& previous) const
      throw(WorkerExecuteException) override;

 private:
  const std::string filename;
};

/**
 * Запись текста в файл.
 *
 * Text -> None
 */
class WriteFile : public Worker {
 public:
  WriteFile(const std::string& filename) : filename(filename) {}

  virtual const WorkerResult execute(const WorkerResult& previous) const
      throw(WorkerExecuteException) override;

 private:
  const std::string filename;
};

/**
 * Выбор из входного текста строк, разделенных символами переноса строки,
 * содержащих заданное слово.
 *
 * Text -> Text
 */
class Grep : public Worker {
 public:
  Grep(const std::string& pattern) : pattern(pattern) {}

  const WorkerResult execute(const WorkerResult& previous) const
      throw(WorkerExecuteException) override;

 private:
  const std::string pattern;
};

/**
 * Лексикогорафическая сортировка входного набора строк.
 *
 * Text -> Text
 */
class Sort : public Worker {
 public:
  const WorkerResult execute(const WorkerResult& previous) const
      throw(WorkerExecuteException) override;
};

/**
 * Замена слова <паттерн> на слово <замена>.
 *
 * Text -> Text
 */
class Replace : public Worker {
 public:
  Replace(const std::string& pattern, const std::string& substitution)
      : pattern(pattern), substitution(substitution) {}

  const WorkerResult execute(const WorkerResult& previous) const
      throw(WorkerExecuteException) override;

 private:
  const std::string pattern;
  const std::string substitution;
};

/**
 * Сохранение пришедшего текста в указанном файле и передача дальше.
 *
 * Text -> Text
 */
class Dump : public WriteFile {
 public:
  Dump(const std::string& filename) : WriteFile(filename) {}

  const WorkerResult execute(const WorkerResult& previous) const
      throw(WorkerExecuteException) override;

 private:
  const std::string filename;
};

}  // namespace workers

#endif /* WORKERS_H_ */

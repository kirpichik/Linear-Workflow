//
//  workflow_parser.cpp
//  Workflow
//
//  Created by Кирилл on 25.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include <mutex>
#include <sstream>
#include <queue>
#include <thread>

#include "workflow_parser.h"
#include "yacc_parser.h"
#include "workers.h"

/**
 * Сохраненный терминальный символ.
 */
class TerminalSub {
public:
  enum Type {
    DESC_END,    // Конец блока описания
    END_OF_FILE, // Конец файла
    COMMAND,     // Команда из блока описания
    INSTRUCTION, // Инструкция из блока инструкций
    ERROR,       // Ошибка разбора
    BEGIN        // Начало разбора
  };
  
  // DESC_END, END_OF_FILE, ERROR
  TerminalSub(const Type type) : type(type), instructionNumber(0) {}
  
  // COMMAND
  TerminalSub(const size_t num, const std::string& name, const std::vector<std::string>& args) : type(COMMAND), instructionNumber(num), name(name), args(args) {}
  
  // INSTRUCTION
  TerminalSub(const size_t num) : type(INSTRUCTION), instructionNumber(num) {}
  
  // BEGIN
  TerminalSub() : type(BEGIN), instructionNumber(0) {}
  
  const Type getType() {
    return type;
  }
  
  const size_t getInstNumber() {
    return instructionNumber;
  }
  
  std::string& getName() {
    return name;
  }
  
  std::vector<std::string>& getArgs() {
    return args;
  }
  
  TerminalSub& operator=(const TerminalSub& terminal) {
    type = terminal.type;
    instructionNumber = terminal.instructionNumber;
    name = terminal.name;
    args = terminal.args;
    return (*this);
  }
  
private:
  Type type;
  size_t instructionNumber;      // Номер инструкции (для INSTRUCTION и COMMAND)
  std::string name;              // Имя команды
  std::vector<std::string> args; // Аргументы команды
  
};

/**
 * Текущее состояние YACC парсера.
 * Сохраненные разобранные данные о команде до помещения в очередь.
 * Так как о разборе команды уведомляется постепенно в обратном порядке,
 * то эти данные нужно хранить до заполнения всех полей.
 *
 * Сначала получаем и запоминаем аргументы, потом имя команды,
 * а потом номер. На номере описание команды считается завершенным.
 *
 * Если поле isDescBlock выставлено в false, то при получении числа
 * другие поля не изменяются.
 */
struct YACCState {
  std::string commandName;
  std::vector<std::string> args;
  bool isDescBlock = true;
};

// Текущее состояние чтения команды. Обнуляется при считывании числа.
static YACCState currentCommandState;

// Текущий поток для перенаправления ввода
static std::istream* currentStream = nullptr;

// Очередь сохраненных терминальных символов из парсера
static std::queue<TerminalSub> terminalsQueue;

// Блокировщик парсинга
static std::mutex parseLocker;

// Блокировщик очереди
static std::mutex queueLocker;

// Блокировщик потока ввода
static std::mutex streamLocker;

// Поток парсера
static std::thread* parserThread = nullptr;

static void parserThreadExecutor() {
  notifyNewStream();
  yyparse();
  queueLocker.unlock();
  parseLocker.unlock();
}

/**
 * Устанавливает новый поток для перенаправления в лексер.
 *
 * @param stream Новый поток
 */
static void setParserStream(std::istream& stream) {
  currentStream = &stream;
  
  // Блокируем, чтобы парсер прочитал один буфер и остановился.
  parseLocker.lock();
  
  // Блокируем очередь до конца работы парсера
  queueLocker.lock();
  
  // Запускаем отдельный поток для парсера.
  // Уведомляем парсер о новом потоке, он читает первый буфер и останавливается
  if (parserThread != nullptr) {
    // TODO - Остановится ли он?
    delete parserThread;
  }
  parserThread = new std::thread(parserThreadExecutor);
}

static const TerminalSub nextTerminal() {
  while(true) {
    // Проверяем наличие элементов в очереди
    queueLocker.lock();
    if (!terminalsQueue.empty()) {
      TerminalSub terminal = terminalsQueue.front();
      terminalsQueue.pop();
      queueLocker.unlock();
      return terminal;
    }
    queueLocker.unlock();
    
    // Разрешаем парсеру сделать еще одну итерацию.
    parseLocker.unlock();
    parseLocker.lock();
  }
}

size_t nextBuffer(char* buff, size_t size) {
  std::size_t n = 0;
  
  // Разблокируем очередь в конце работы парсера
  queueLocker.unlock();
  
  while(size > 0 && currentStream->good()) {
    currentStream->read(&buff[n], size);
    size_t i = currentStream->gcount();
    n += i;
    size -= i;
  }
  
  // Ждем, пока нам разрешат читать дальше
  parseLocker.lock();
  parseLocker.unlock();
  
  // Блокируем очередь до конца работы парсера
  queueLocker.lock();
  
  return n;
}

void pushArg(const char* arg) {
  currentCommandState.args.push_back(std::string(arg));
}

void pushCommand(const char* cmd) {
  currentCommandState.commandName = std::string(cmd);
}

void pushNumber(int num) {
  if (currentCommandState.isDescBlock) {
    terminalsQueue.push(TerminalSub(num, currentCommandState.commandName, currentCommandState.args));
      currentCommandState.args.clear();
    }
  else
    terminalsQueue.push(TerminalSub(num));
}

void pushDescEnd() {
  terminalsQueue.push(TerminalSub(TerminalSub::DESC_END));
  currentCommandState.isDescBlock = false;
}

void pushError(const char* msg) {
  terminalsQueue.push(TerminalSub(TerminalSub::ERROR));
}

void pushEOF() {
  terminalsQueue.push(TerminalSub(TerminalSub::END_OF_FILE));
}

namespace wkfw {

DescriptionParser::DescriptionParser(std::istream& stream) throw(InvalidDescriptionException) {
  setParserStream(stream);
  
  TerminalSub previousSub;
  while(previousSub.getType() != TerminalSub::DESC_END) {
    previousSub = nextTerminal();
    
    switch (previousSub.getType()) {
      case TerminalSub::COMMAND: {
        if (description.find(previousSub.getInstNumber()) != description.end())
          throw InvalidDescriptionException("Repeating instructions number.");
        description[previousSub.getInstNumber()] = workers::constructWorkerByName(previousSub.getInstNumber(), previousSub.getName(), previousSub.getArgs());
        break;
      }
      case TerminalSub::DESC_END:
        break;
      default:
        throw InvalidDescriptionException("Invalid symbols in description block.");
    }
  }
}

const Worker* DescriptionParser::getWorkerById(const size_t ident) const {
  auto found = description.find(ident);
  return found == description.end() ? nullptr : found->second;
}

DescriptionParser::~DescriptionParser() {
  for (auto const& worker : description) delete worker.second;
}
  
LazyInstructionParser::LazyInstructionParser(const DescriptionParser& desc, std::istream& stream) throw(InvalidInstructionException) : InstructionParser(desc), stream(stream) {}

const Worker* LazyInstructionParser::nextInstruction() throw(InvalidInstructionException) {
  if (position < instructions.size())
    return description.getWorkerById(instructions[position++]);
  
  TerminalSub terminal = nextTerminal();
  switch (terminal.getType()) {
    case TerminalSub::INSTRUCTION: {
      const Worker* worker = description.getWorkerById(terminal.getInstNumber());
      if (!worker)
        throw InvalidInstructionException("Unknown instruction number: " + std::to_string(terminal.getInstNumber()));
      if (position && worker->getAcceptType() != previousType)
        throw InvalidInstuctionsSequenceException();
      previousType = worker->getReturnType();
      position++;
      return worker;
    }
    case TerminalSub::END_OF_FILE:
      return nullptr;
    default:
      throw InvalidInstructionException("Unknown symbol in instruction block.");
  }
}
  
void LazyInstructionParser::resetSteps() {
  position = 0;
}

ValidateInstructionParser::ValidateInstructionParser(const DescriptionParser& desc, std::istream& stream) throw(InvalidInstructionException) : InstructionParser(desc) {
  TerminalSub terminal;
  WorkerResult::ResultType previousType = WorkerResult::NONE;
  
  while(terminal.getType() != TerminalSub::END_OF_FILE) {
    terminal = nextTerminal();
    
    switch (terminal.getType()) {
      case TerminalSub::INSTRUCTION: {
        const Worker* worker = description.getWorkerById(terminal.getInstNumber());
        if (!worker)
          throw InvalidInstructionException("Unknown instruction number: " + std::to_string(terminal.getInstNumber()));
        if (instructions.size() != 0 && worker->getAcceptType() != previousType)
          throw InvalidInstuctionsSequenceException();
        previousType = worker->getReturnType();
        instructions.push_back(terminal.getInstNumber());
      };
      case TerminalSub::END_OF_FILE:
        break;
      default:
        throw InvalidInstructionException("Unknown symbol in instruction block.");
    }
  }
}
  
void ValidateInstructionParser::resetSteps() {
  position = 0;
}

const Worker* ValidateInstructionParser::nextInstruction() throw(InvalidInstuctionsSequenceException) {
  if (position >= instructions.size()) return nullptr;
  return description.getWorkerById(instructions[position++]);
}

}

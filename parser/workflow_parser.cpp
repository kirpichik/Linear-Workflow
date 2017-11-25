//
//  workflow_parser.cpp
//  Workflow
//
//  Created by Кирилл on 25.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include <mutex>
#include <fstream>

#include "workflow_parser.h"
#include "yacc_parser.h"
#include "workers.h"

struct CurrentParser {
  // Объект для блокировки
  std::mutex locker;
  // Парсер, который подает на вход текст и ожидает уведомления
  wkfw::YACCParser* currentParser = nullptr;
  
  /* Так как парсер разбирает в обратном порядке, мы получаем сначала
   список аргументов команды, потом ее имя, а потом номер.
   Поэтому мы храним аргументы и имя до тех пор, пока не получим
   число. После получения числа, уведомляем об этом основной парсер.
   */
  // Список запомненных на данный момент аргументов команды
  std::vector<std::string> currArgs;
  // Имя команды
  std::string commandName;
  // Если true, то сейчас блок описания, иначе - инструкций
  bool isDescBlock = true;
};

// Размер буфера перенаправления потока C++ в лексер.
static const size_t REDIRECT_BUFF_SIZE = 1024;

static CurrentParser currentParser;

void pushArg(const char* arg) {
  if (!currentParser.currentParser)
    return;
  currentParser.currArgs.push_back(std::string(arg));
}

void pushCommand(const char* cmd) {
  if (!currentParser.currentParser)
    return;
  currentParser.commandName = std::string(cmd);
}

void pushNumber(int num) {
  if (!currentParser.currentParser)
    return;
  if (currentParser.isDescBlock)
    currentParser.currentParser->pushCommand(num, currentParser.commandName, currentParser.currArgs);
  else
    currentParser.currentParser->pushInstruction(num);
}

void pushDescEnd() {
  if (!currentParser.currentParser)
    return;
  currentParser.isDescBlock = false;
}

void pushError(const char* msg) {
  if (!currentParser.currentParser)
    return;
  currentParser.currentParser->pushError(std::string(msg));
}

/**
 * Блокирует попытки подключения других парсеров,
 * пока блокировка данного парсера не будет снята.
 *
 * @param parser Парсер, который хочет получить контроль над вводом.
 */
void lockParserOn(wkfw::YACCParser* parser) {
  currentParser.locker.lock();
  currentParser.currentParser = parser;
}

/**
 * Снимает блокировку с подключенного парсера.
 * При вызове из потока, который не вызывал блокировку,
 * поведение неопределено.
 */
void unlockParser() {
  currentParser.currentParser = nullptr;
  currentParser.currArgs.clear();
  currentParser.isDescBlock = true;
  currentParser.locker.unlock();
}

/**
 * Считывает из потока в буфер.
 *
 * @param in Входной поток.
 * @param buff Буфер для записи.
 * @param len Размер буфера.
 *
 * @return Считаный размер.
 */
size_t redirectBytes(std::istream& in, char* buff, size_t len) {
  std::size_t n = 0;
  
  while(len > 0 && in.good()) {
    in.read( &buff[n], len );
    size_t i = in.gcount();
    n += i;
    len -= i;
  }
  
  return n;
}

namespace wkfw {

DescriptionParser::DescriptionParser(std::istream& stream) throw(InvalidDescriptionException) {
  char* buff = new char[REDIRECT_BUFF_SIZE + 2];
  while (true) {
    size_t redirected = redirectBytes(stream, buff, REDIRECT_BUFF_SIZE);
  
    if (!redirected)
      break;
      
    /* Выставление ограничения буфера для лексера */
    buff[redirected] = '\0';
    buff[redirected + 1] = '\0';
  
    scanBuffer(buff, sizeof(buff));
    if (isErrorState())
      throw InvalidDescriptionException();
  }
}

const Worker* DescriptionParser::getWorkerById(const size_t ident) const {
  auto found = description.find(ident);
  return found == description.end() ? nullptr : found->second;
}

DescriptionParser::~DescriptionParser() {
  for (auto const& worker : description) delete worker.second;
}

const Worker* LazyInstructionParser::nextInstruction() throw(InvalidInstructionException) {
  // TODO - разбор и валидация инструкций
  return nullptr;
}

ValidateInstructionParser::ValidateInstructionParser(const DescriptionParser& desc, std::istream& stream) throw(InvalidInstructionException)
: InstructionParser(desc) {
  // TODO - разбор и валидация инструкций
}

const Worker* ValidateInstructionParser::nextInstruction() throw(InvalidInstuctionsSequenceException) {
  if (position >= instructions.size()) return nullptr;
  return description.getWorkerById(instructions[position++]);
}
  
void DescriptionParser::pushCommand(const size_t ident, const std::string& name, const std::vector<std::string>& args) {
  Worker* worker = workers::constructWorkerByName(ident, name, args);
  if (!worker) {
    throwErrorState();
    return;
  }
  description[ident] = worker;
}

/**
 * Ничего делать не нужно, т.к. это блок описания.
 */
void DescriptionParser::pushInstruction(const size_t number) {}

/**
 * Ничего делать не нужно, т.к. это блок инструкций.
 */
void ValidateInstructionParser::pushCommand(const size_t ident, const std::string& name, const std::vector<std::string>& args) {}

void ValidateInstructionParser::pushInstruction(const size_t number) {
  instructions.push_back(number);
}

/**
 * Ничего делать не нужно, т.к. это блок инструкций.
 */
void LazyInstructionParser::pushCommand(const size_t ident, const std::string& name, const std::vector<std::string>& args) {}

void LazyInstructionParser::pushInstruction(const size_t number) {
  if (isErrorState())
    return;
  
  const Worker* worker = description.getWorkerById(number);
  if (!worker || worker->getAcceptType() != previousType) {
    throwErrorState();
    return;
  }
  previousType = worker->getReturnType();
  instructBuff.push_back(number);
}

}

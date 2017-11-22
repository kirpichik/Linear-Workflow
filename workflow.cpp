//
//  workflow.cpp
//  Workflow
//
//  Created by Кирилл on 20.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include "workflow.h"

namespace wkfw {

DescriptionParser::DescriptionParser(std::istream& stream) throw(
    InvalidDescriptionException) {
  // TODO - разбор блока описания
}

const wkfw::Worker* DescriptionParser::getWorkerById(const size_t ident) const {
  auto found = description.find(ident);
  return found == description.end() ? nullptr : found->second;
}

DescriptionParser::~DescriptionParser() {
  for (auto const& worker : description) delete worker.second;
}

const wkfw::Worker* LazyInstructionParser::nextInstruction() throw(
    InvalidInstructionException) {
  // TODO - разбор и валидация инструкций
  return nullptr;
}

ValidateInstructionParser::ValidateInstructionParser(
    const DescriptionParser& desc,
    std::istream& stream) throw(InvalidInstructionException)
    : InstructionParser(desc) {
  // TODO - разбор и валидация инструкций
}

const wkfw::Worker* ValidateInstructionParser::nextInstruction() throw(
    InvalidInstuctionsSequenceException) {
  if (position >= instructions.size()) return nullptr;
  return description.getWorkerById(instructions[position++]);
}

Workflow::Workflow(std::istream& stream) throw(InvalidConfigurationException)
    : description(DescriptionParser(stream)), stream(stream) {}

void Workflow::executeLazy() throw(
    InstructionParser::InvalidInstructionException) {
  LazyInstructionParser parser(description, stream);
  execute(parser);
}

void Workflow::executeNormal() throw(
    InstructionParser::InvalidInstructionException) {
  ValidateInstructionParser parser(description, stream);
  execute(parser);
}

void Workflow::execute(InstructionParser& parser) throw(
    InstructionParser::InvalidInstructionException) {
  wkfw::WorkerResult lastResult;
  wkfw::Worker const* worker;

  while ((worker = parser.nextInstruction()))
    lastResult = worker->execute(lastResult);
}

}  // namespace wkfw

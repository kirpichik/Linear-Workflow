//
//  parser_types.h
//  Workflow
//
//  Created by Кирилл on 03.12.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#ifndef PARSER_TYPES_H
#define PARSER_TYPES_H

#include <string>
#include <vector>

namespace wkfw {

/**
 * Команда из блока команд для парсера
 */
struct WorkflowCommand {
  size_t instructionNumber;
  std::string name;
  std::vector<std::string> args;

  WorkflowCommand(const size_t num,
                  const std::string& name,
                  const std::vector<std::string>& args)
      : instructionNumber(num), name(name), args(args) {}

  WorkflowCommand() : instructionNumber(0), name(), args() {}

  ~WorkflowCommand() {}
};

}  // namespace wkfw

#endif /* PARSER_TYPES_H */

//
//  test_parser.cpp
//  WorkflowTests
//
//  Created by Кирилл on 01.12.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include "gtest/gtest.h"

#include <vector>
#include <sstream>

#include "workflow_parser.h"

static const std::vector<std::string> rightSamples = {
  
  // =====================================
  
  "desc\
  1 = readfile i.txt\
  2 = sort\
  3 = writefile o.txt\
  csed\
  1 -> 2 -> 3",
  
  // ======================================
  
  "desc\
  \n#Test\n\
  \n\
  \n\
  1 = replace abracadabra cadabraabra # Test\n\
  \
  2 = grep braab #Test\n\
  3 = sort#Test\n\
  \n\
  \n\
  0 = readfile in.txt#####\n\
  5 = writefile out.txt#TEST #Test #test\n\
  \n\
  #Test\n\
  \n\
  csed\
  \n\
  #test\n\
  0 -> 1 -> 2 -> 3 -> 5#TEST\n\
  \n\
  \n\
  ",
  
  // ======================================
  
  "desc\
  1 = sort\
  csed\
  1",
  
  // ======================================
  
  "desc\
  3 = readfile i.txt\
  2 = replace a b\
  4 = replace a b\
  5 = replace a b\
  6 = replace a b\
  1 = writefile o.txt\
  csed\
  3 -> 2 -> 4 -> 5 -> 6 -> 1",
  
  // ======================================
  
  "desc\
  0 = dump dump.txt\
  1 = readfile in.txt\
  2 = writefile out.txt\
  csed\
  1 -> 0 -> 2"
  
  // ======================================
};

static const std::vector<std::string> wrongSamples = {
  
  // ======================================
  // Ситаксические ошибки
  // ======================================
  
  "d\
  1 = readfile i.txt\
  2 = sort\
  3 = writefile o.txt\
  ced\
  1 -> 2  3",
  
  // ======================================
  // Повторяются номера инструкций в блоке описания
  // ======================================
  
  "desc\
  1 = readfile i.txt\
  1 = sort\
  3 = writefile o.txt\
  csed\
  1 -> 2 -> 3",
  
  // ======================================
  // Отсутствует инструкция 0 в блоке описания
  // ======================================
  
  "desc\
  1 = readfile i.txt\
  2 = sort\
  3 = writefile o.txt\
  csed\
  1 -> 0 -> 3",
  
  // ======================================
  // Неправильное название команды
  // ======================================
  
  "desc\
  1 = read i.txt\
  2 = sort\
  3 = writefile o.txt\
  csed\
  1 -> 2 -> 3",
  
  // ======================================
  // Отсутствует блок инструкций
  // ======================================
  
  "desc\
  1 = readfile i.txt\
  2 = sort\
  3 = writefile o.txt\
  csed"
  
  // ======================================
};

/**
 * Выполняет разбор строки парсером и проверяет наличие всех номеров инструкций
 * в блоке описания и всех номеров и их порядок в блоке исполнения.
 *
 * @param input Строка для разбора
 * @param descInst Набор инструкций, которые должны быть распаршены
 * @param inst Набор и порядок инструкций, которые должны быть распаршены
 *
 * @return true, если все совпало
 */
static bool checkParser(const std::string& input, const std::vector<size_t>& descInst, const std::vector<size_t>& inst) throw(wkfw::InvalidWorkflowException) {
  std::istringstream stream(input);
  wkfw::WorkflowParser parser(stream);
  
  for (auto inst : descInst)
    if (parser.getWorkerById(inst) == nullptr)
      return false;
  
  for (auto inst : inst) {
  const wkfw::Worker* worker = parser.nextInstruction();
    if (worker == nullptr || worker->getId() != inst)
      return false;
  }
  
  return true;
}

TEST(Parser, Right) {
  ASSERT_TRUE(checkParser(rightSamples[0], { 1, 2, 3 }, { 1, 2, 3 }));
  ASSERT_TRUE(checkParser(rightSamples[1], { 0, 1, 2, 3, 5 }, { 0, 1, 2, 3, 5 }));
  ASSERT_TRUE(checkParser(rightSamples[2], { 1 }, { 1 }));
  ASSERT_TRUE(checkParser(rightSamples[3], { 1, 2, 3, 4, 5, 6 }, { 3, 2, 4, 5, 6, 1 }));
  ASSERT_TRUE(checkParser(rightSamples[4], { 0, 1, 2 }, { 1, 0, 2 }));
}

TEST(Parser, Wrong) {
  ASSERT_THROW(checkParser(wrongSamples[0], { 0 }, { 0 }), wkfw::InvalidWorkflowException);
  ASSERT_THROW(checkParser(wrongSamples[1], { 0 }, { 0 }), wkfw::InvalidWorkflowException);
  ASSERT_THROW(checkParser(wrongSamples[2], { 0 }, { 0 }), wkfw::InvalidWorkflowException);
  ASSERT_THROW(checkParser(wrongSamples[3], { 0 }, { 0 }), wkfw::InvalidWorkflowException);
  ASSERT_THROW(checkParser(wrongSamples[4], { 0 }, { 0 }), wkfw::InvalidWorkflowException);
}

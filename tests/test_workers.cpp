//
//  test_workers.cpp
//  WorkflowTests
//
//  Created by Кирилл on 21.11.17.
//  Copyright © 2017 Кирилл. All rights reserved.
//

#include <gtest/gtest.h>

#include <cstdio>
#include <set>
#include <fstream>

#include "workers.h"

const std::string TEMP_TEST_FILE = "._temp_test_";

/**
 * Общий класс для тестирования работы с файлами.
 * Предоставляет методы для создания/удаления/проверки наличия файла.
 * При создании запоминает файл и по заверщении теста автоматически удаляет его.
 * Так же всегда запоминает файл TEMP_TEST_FILE
 * */
class IOWorkerTest : public ::testing::Test {
public:
  IOWorkerTest() : trackingFiles({ TEMP_TEST_FILE }) {}
  
  /**
   * Создает файл, заполняет его содержимым и запоминает его для удаления в конце теста.
   * Если файл уже имеется, перезаписывает его без ошибок.
   * */
  void createFile(const std::string& filename, const std::string& content) {
    std::ofstream output;
    
    output.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    
    try {
      output.open(filename);
      output << content;
      output.close();
    } catch (std::ofstream::failure& ignored) {}
  }
  
  /**
   * @return true, если файл существует и его содержимое соответствует искомому.
   * */
  bool validateFile(const std::string& filename, const std::vector<std::string> prototype) {
    std::ifstream file(filename);
    std::vector<std::string> list;
    
    if (!file.is_open())
      return false;
    
    std::string line;
    while (std::getline(file, line)) list.push_back(line);
    file.close();
    
    return prototype == list;
  }
  
  
  /**
   * Удаляет файл и забывает о его существовании.
   * */
  void removeFile(const std::string& filename) {
    if (trackingFiles.find(filename) != trackingFiles.end())
      trackingFiles.erase(filename);
    remove(filename.c_str());
  }
  
 protected:
  void TearDown() override {
    for (auto filename : trackingFiles)
      remove(filename.c_str());
  }
  
 private:
  // Отслеживаемые файлы
  std::set<std::string> trackingFiles;
};

TEST_F(IOWorkerTest, ReadFileRight) {
  workers::ReadFile read(TEMP_TEST_FILE);
  
  createFile(TEMP_TEST_FILE, "abc\ndef\nghi");
  
  ASSERT_EQ(read.execute(WorkerResult()),
            WorkerResult({ "abc", "def", "ghi" }));
  
  createFile(TEMP_TEST_FILE, "abc\ndef\nghi\n");
  
  ASSERT_EQ(read.execute(WorkerResult()),
            WorkerResult({ "abc", "def", "ghi" }));
}

TEST_F(IOWorkerTest, WriteFileRight) {
  workers::WriteFile write(TEMP_TEST_FILE);
  
  ASSERT_EQ(write.execute(WorkerResult({ "abc", "def", "ghi" })), WorkerResult());
  
  ASSERT_TRUE(validateFile(TEMP_TEST_FILE, { "abc", "def", "ghi" }));
}

TEST(Workers, GrepRight) {
  workers::Grep grep("abc");
  
  ASSERT_EQ(grep.execute(WorkerResult({ "abc def ghi", "jkl abc mno", "pqr stu abc" })),
            WorkerResult({ "abc def ghi", "jkl abc mno", "pqr stu abc" }));
  
  ASSERT_EQ(grep.execute(WorkerResult({ "abc def ghi", "jkl mno", "pqr stu yz" })),
            WorkerResult({ "abc def ghi" }));
  
  ASSERT_EQ(grep.execute(WorkerResult({ "vwx def ghi", "jkl mno", "pqr stu yz" })),
            WorkerResult(std::vector<std::string>()));
}

TEST(Workers, SortRight) {
  workers::Sort sort;
  
  ASSERT_EQ(sort.execute(WorkerResult(std::vector<std::string>())),
            WorkerResult(std::vector<std::string>()));
  
  ASSERT_EQ(sort.execute(WorkerResult({ "ghi", "abc", "def" })),
            WorkerResult({ "abc", "def", "ghi" }));
  
  ASSERT_EQ(sort.execute(WorkerResult({ "abc", "def", "ghi" })),
            WorkerResult({ "abc", "def", "ghi" }));
}

TEST(Workers, ReplaceRight) {
  workers::Replace replace("abc", "def");
  
  ASSERT_EQ(replace.execute(WorkerResult({ "" })),
            WorkerResult({ "" }));
  
  ASSERT_EQ(replace.execute(WorkerResult({ "", "" })),
            WorkerResult({ "", "" }));
  
  ASSERT_EQ(replace.execute(WorkerResult(std::vector<std::string>())),
            WorkerResult(std::vector<std::string>()));
  
  ASSERT_EQ(replace.execute(WorkerResult({ "abc" })),
            WorkerResult({ "def" }));
  
  ASSERT_EQ(replace.execute(WorkerResult({ "abc", "abc" })),
            WorkerResult({ "def", "def" }));
  
  ASSERT_EQ(replace.execute(WorkerResult({ "ghi abc jkl" })),
            WorkerResult({ "ghi def jkl" }));
  
  ASSERT_EQ(replace.execute(WorkerResult({ "def" })),
            WorkerResult({ "def" }));
  
  ASSERT_EQ(replace.execute(WorkerResult({ "def abc def" })),
            WorkerResult({ "def def def" }));
}

TEST_F(IOWorkerTest, DumpRight) {
  workers::Dump dump(TEMP_TEST_FILE);
  
  ASSERT_EQ(dump.execute(WorkerResult({ "test text" })),
            WorkerResult({ "test text" }));
}

TEST_F(IOWorkerTest, ReadFileWrong) {
  workers::ReadFile read(TEMP_TEST_FILE);
  
  ASSERT_THROW(read.execute(WorkerResult()), WorkerExecuteException);
}

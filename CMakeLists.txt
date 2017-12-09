cmake_minimum_required(VERSION 3.0)

project(Workflow)

find_package(FLEX REQUIRED)
if(NOT FLEX_FOUND)
  message(SEND_ERROR "FLEX not found.")
  return()
else()
  include_directories(${FLEX_INCLUDE_DIR})
endif()

find_package(BISON REQUIRED)
if(NOT BISON_FOUND)
  message(SEND_ERROR "Bison not found.")
  return()
else()
  BISON_TARGET(WorkflowParser parser/parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp)
  FLEX_TARGET(WorkflowLexer parser/lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp)
  ADD_FLEX_BISON_DEPENDENCY(WorkflowLexer WorkflowParser)
endif()

add_definitions(-std=c++11)

# To fix undefined macro in bison generated file.
add_definitions(-DYY_NULLPTR=0)

include_directories(.)
include_directories(./parser)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(COMMON_SOURCES
  ${FLEX_WorkflowLexer_OUTPUTS}
  ${BISON_WorkflowParser_OUTPUTS}
  workers.cpp
  workflow.cpp
  parser/workflow_parser.cpp
)
set(TARGET_SOURCES
  main.cpp
)
file(GLOB TEST_SOURCES
  tests/*.cpp
  gtest/*.cc
)

# Main

add_executable(Workflow ${COMMON_SOURCES} ${TARGET_SOURCES})

target_link_libraries(Workflow ${FLEX_LIBRARIES})

# Tests

add_executable(WorkflowTests ${COMMON_SOURCES} ${TEST_SOURCES})

target_link_libraries(WorkflowTests ${FLEX_LIBRARIES} pthread)

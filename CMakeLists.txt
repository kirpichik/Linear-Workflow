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
BISON_TARGET(WorkflowParser parser/parser.y parser/parser.cpp)
FLEX_TARGET(WorkflowLexer parser/lexer.l parser/lexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(WorkflowLexer WorkflowParser)
endif()

add_definitions(-std=c++11)
add_definitions(-DYY_NULLPTR=0)

include_directories(.)
include_directories(./parser)

file(GLOB COMMON_SOURCES workers.cpp workflow.cpp parser/lexer.cpp parser/parser.cpp parser/workflow_parser.cpp)
set(TARGET_SOURCES main.cpp)
file(GLOB TEST_SOURCES tests/*.cpp gtest/*.cc)

add_executable(Workflow ${COMMON_SOURCES} ${TARGET_SOURCES})

target_link_libraries(Workflow ${FLEX_LIBRARIES} pthread)

add_executable(WorkflowTests ${COMMON_SOURCES} ${TEST_SOURCES})

target_link_libraries(WorkflowTests ${FLEX_LIBRARIES} pthread)

#!/bin/sh

echo "Generating Bison files..."

bison -o parser.cpp parser.y

if [ ! $? -eq 0 ]
then
  exit
fi


echo "Generating Flex files..."

flex -o lexer.cpp lexer.l

if [ ! $? -eq 0 ]
then
  exit
fi

echo "Done."

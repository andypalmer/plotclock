#!/usr/local/bin/zsh
mkdir -p build/bin
c++ -o build/bin/tests build/objs/**/*.o /usr/local/Cellar/cpputest/3.8/lib/libCppUTest.a /usr/local/Cellar/cpputest/3.8/lib/libCppUTestExt.a  -g

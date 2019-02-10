#!/usr/local/bin/zsh
mkdir -p build/objs/{src,tests}
for f in src/**/*.cpp; do
  c++ -g -Wall -Weverything -Werror -Wswitch-default -Wswitch-enum -Wno-unused-parameter -Wno-c99-extensions -Wno-keyword-macro -Wno-reserved-id-macro -Wno-c++11-long-long -Weverything -Wno-disabled-macro-expansion -Wno-padded -Wno-global-constructors -Wno-exit-time-destructors -Wno-weak-vtables -Wno-old-style-cast \
    -Iinclude/geometry \
    -c -MMD -MP \
    -o build/objs/${f:r}.o ${f}
done
for f in tests/**/*.cpp; do
  c++ -g -Wall -Weverything -Werror -Wswitch-default -Wswitch-enum -Wno-unused-parameter -Wno-c99-extensions -Wno-keyword-macro -Wno-reserved-id-macro -Wno-c++11-long-long -Wno-disabled-macro-expansion -Wno-padded -Wno-global-constructors -Wno-exit-time-destructors -Wno-weak-vtables -Wno-old-style-cast \
    -DCPPUTEST_COMPILATION \
    -include /usr/local/Cellar/cpputest/3.8/include/CppUTest/MemoryLeakDetectorNewMacros.h \
    -include /usr/local/Cellar/cpputest/3.8/include/CppUTest/MemoryLeakDetectorMallocMacros.h \
    -I/usr/local/Cellar/cpputest/3.8/include/ \
    -Iinclude/geometry \
    -c -MMD -MP \
    -o build/objs/${f:r}.o ${f}
done

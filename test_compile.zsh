#!/usr/local/bin/zsh
mkdir -p build/objs/{src,tests}
for f in src/**/*.cpp; do
  c++ -g -Wall -Wextra -Weverything -Werror \
    -Iinclude/geometry \
    -c -MMD -MP \
    -o build/objs/${f:r}.o ${f}
done

CPPUTEST_MEMORY_LEAK_WARNING_DISABLE=(-Wno-c++11-long-long -Wno-keyword-macro -Wno-reserved-id-macro)
CPPUTEST_FRAMEWORK_WARNING_DISABLE=(-Wno-old-style-cast -Wno-disabled-macro-expansion -Wno-weak-vtables -Wno-global-constructors -Wno-exit-time-destructors -Wno-padded)
for f in tests/**/*.cpp; do
  c++ -g -Wall -Wextra -Weverything -Werror \
    $CPPUTEST_MEMORY_LEAK_WARNING_DISABLE \
    $CPPUTEST_FRAMEWORK_WARNING_DISABLE \
    -DCPPUTEST_COMPILATION \
    -include /usr/local/Cellar/cpputest/3.8/include/CppUTest/MemoryLeakDetectorNewMacros.h \
    -include /usr/local/Cellar/cpputest/3.8/include/CppUTest/MemoryLeakDetectorMallocMacros.h \
    -I/usr/local/Cellar/cpputest/3.8/include/ \
    -Iinclude/geometry \
    -c -MMD -MP \
    -o build/objs/${f:r}.o ${f}
done

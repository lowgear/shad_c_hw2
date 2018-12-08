#CC=C:/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/gcc.exe
#CXX=C:/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/g++.exe
#LD=C:/mingw-w64/x86_64-8.1.0-posix-seh-rt_v6-rev0/mingw64/bin/ld.exe

INCLUDES=-I$(GTEST) -I$(GTEST)/include
WARN_OPTS=-Wall -Werror -pedantic
LDFLAGS=$(FLAGS) -lm -lpthread

CFLAGS=$(WARN_OPTS) $(INCLUDES) \
        -std=c11

CXXFLAGS=$(WARN_OPTS) $(INCLUDES) \
        -std=c++17

LINK_EXECUTABLE=$(LD) -o $@ $^ $(LDFLAGS)

COMPILE_CXX_SRC=$(CXX) $(CXXFLAGS) -c -o $@ $^

COMPILE_C_SRC=$(CC) $(CFLAGS) -c -o $@ $^

all: asm vm check;

asm: asm.o
	$(LINK_EXECUTABLE)

asm.o: asm.c
	$(COMPILE_C_SRC)

vm: vm.o
	$(LINK_EXECUTABLE)

vm.o: vm.c
	$(COMPILE_C_SRC)

check: check.o $(GTEST)/src/gtest-all.o $(GTEST)/src/gtest_main.o
	$(LINK_EXECUTABLE)

check.o: check.cpp
	$(COMPILE_CXX_SRC)

$(GTEST)/src/gtest-all.o: $(GTEST)/src/gtest-all.cc
	$(COMPILE_CXX_SRC)

$(GTEST)/src/gtest_main.o: $(GTEST)/src/gtest_main.cc
	$(COMPILE_CXX_SRC)
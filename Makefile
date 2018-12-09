LD=/usr/bin/ld
CXX=/usr/bin/g++
CC=/usr/bin/gcc

SRCD=.
RM_ALL=rm -f

INCLUDES=-I$(GTEST) -I$(GTEST)/include -I$(SRCD)
WARN_OPTS=-Wall -Werror -pedantic
LDFLAGS=$(FLAGS) -lm -lpthread

CFLAGS=$(WARN_OPTS) $(INCLUDES) -std=c11

CXXFLAGS=$(WARN_OPTS) $(INCLUDES) -std=c++17

LINK_EXECUTABLE=$(LD) $(LDFLAGS) -o $@ $^

COMPILE_CXX_SRC=$(CXX) $(CXXFLAGS) -c -o $@ $^

COMPILE_C_SRC=$(CC) $(CFLAGS) -c -o $@ $^

all: asm vm check;

asm: asm.o strtools.o
	$(LINK_EXECUTABLE)

asm.o: $(SRCD)/asm.c
	$(COMPILE_C_SRC)

vm: vm.o strtools.o
	$(LINK_EXECUTABLE)

vm.o: $(SRCD)/vm.c
	$(COMPILE_C_SRC)

GTEST_SRC=$(GTEST)/src/*.cc
GTEST_OBJ=$(GTEST_SRC:.cc=.o)

$(GTEST_OBJ): $(GTEST_SRC)
#	$(COMPILE_CXX_SRC)

check: check.o $(GTEST_OBJ)
	$(LINK_EXECUTABLE)

#check: check.o $(GTEST)/src/gtest-all.o $(GTEST)/src/gtest_main.o
#	$(LINK_EXECUTABLE)

check.o: $(SRCD)/check.cpp
	$(COMPILE_CXX_SRC)

$(GTEST)/src/gtest-all.o: $(GTEST)/src/gtest-all.cc
#	$(COMPILE_CXX_SRC)

$(GTEST)/src/gtest_main.o: $(GTEST)/src/gtest_main.cc
#	$(COMPILE_CXX_SRC)

strtools.o: $(SRCD)/strtools.c
	$(COMPILE_C_SRC)

TO_DELETE=asm vm check

clean:
	$(RM_ALL) *.o $(TO_DELETE) $(TO_DELETE:=.exe) $(TO_DELETE:=.bin)

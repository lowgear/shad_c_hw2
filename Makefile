SRCD="."
RM_ALL=rm -rf

INCLUDES=-I$(GTEST) -I$(GTEST)/include -I$(SRCD)
WARN_OPTS=-Wall -Werror -pedantic
LDFLAGS=$(FLAGS) -lm -lpthread

CFLAGS=$(WARN_OPTS) $(INCLUDES) -std=c11

CXXFLAGS=$(WARN_OPTS) $(INCLUDES) -std=c++17

LINK_EXECUTABLE=$(LD) -o $@ $^ $(LDFLAGS)

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

check: check.o gtest-all.o gtest_main.o
	$(LINK_EXECUTABLE)

check.o: $(SRCD)/check.cpp
	$(COMPILE_CXX_SRC)

gtest-all.o: $(GTEST)/src/gtest-all.cc
	$(COMPILE_CXX_SRC)

gtest_main.o: $(GTEST)/src/gtest_main.cc
	$(COMPILE_CXX_SRC)

strtools.o: $(SRCD)/strtools.c
	$(COMPILE_C_SRC)

clean:
	$(RM_ALL) *.o asm vm check
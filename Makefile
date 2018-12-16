LD=/usr/bin/g++
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

asm: asm.o strtools.o labels.o readtoken.o token_tools.o
	$(LINK_EXECUTABLE)

token_tools.o: $(SRCD)/asm/token_tools.c
	$(COMPILE_C_SRC)

readtoken.o: $(SRCD)/asm/readtoken.c
	$(COMPILE_C_SRC)

labels.o: $(SRCD)/asm/labels.c
	$(COMPILE_C_SRC)

asm.o: $(SRCD)/asm.c
	$(COMPILE_C_SRC)

vm: vm.o strtools.o handlers.o handlersarray.o vmstruct.o
	$(LINK_EXECUTABLE)

handlers.o: $(SRCD)/vm/handlers.c
	$(COMPILE_C_SRC)

handlersarray.o: $(SRCD)/vm/handlersarray.c
	$(COMPILE_C_SRC)

vmstruct.o: $(SRCD)/vm/vmstruct.c
	$(COMPILE_C_SRC)

vm.o: $(SRCD)/vm.c
	$(COMPILE_C_SRC)

check: check.o gtest-all.o gtest_main.o strtools.o labels.o readtoken.o token_tools.o
	$(LINK_EXECUTABLE)

check.o: $(SRCD)/check.cpp
	$(COMPILE_CXX_SRC)

gtest-all.o: $(GTEST)/src/gtest-all.cc
	$(COMPILE_CXX_SRC)

gtest_main.o: $(GTEST)/src/gtest_main.cc
	$(COMPILE_CXX_SRC)

strtools.o: $(SRCD)/utils/strtools.c
	$(COMPILE_C_SRC)

TO_DELETE=asm vm check

clean:
	$(RM_ALL) *.o $(TO_DELETE) $(TO_DELETE:=.exe) $(TO_DELETE:=.bin)

cmake_minimum_required(VERSION 3.12)
project(c_asm_vm C CXX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(GTEST C:/Users/aidar.islamov/Downloads/googletest-release-1.8.1/googletest)

include_directories(${GTEST} ${GTEST}/include)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_executable(fake_vm vm.c)

add_executable(fake_asm asm.c strtools.c strtools.h argtools.h mingw_stdio_fix.h asm/tokenizer.h)

add_executable(fake_tests check.cpp)

target_link_libraries(fake_tests ${GTEST}/libgtest.a ${GTEST}/libgtest_main.a)

file(RELATIVE_PATH BIN_DIR ${c_asm_vm_SOURCE_DIR} ${PROJECT_BINARY_DIR})

#########################################
# Custom Makefile
add_custom_target(asm
        COMMAND make -C ${c_asm_vm_SOURCE_DIR}
        CC=${CMAKE_C_COMPILER}
        CXX=${CMAKE_CXX_COMPILER}
        LD=${CMAKE_CXX_COMPILER}
        GTEST=C:/Users/aidar.islamov/Downloads/googletest-release-1.8.1/googletest
        OUTDIR=${BIN_DIR})

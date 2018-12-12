cmake_minimum_required(VERSION 3.12)
project(c_asm_vm C CXX)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(GTEST C:/Users/aidar.islamov/Downloads/googletest-release-1.8.1/googletest)

include_directories(${GTEST} ${GTEST}/include .)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_executable(fake_vm vm.c)

add_executable(fake_asm asm.c strtools.c)

add_executable(fake_tests check.cpp ${GTEST}/src/gtest_main.cc ${GTEST}/src/gtest-all.cc)

#file(RELATIVE_PATH BIN_DIR ${c_asm_vm_SOURCE_DIR} ${PROJECT_BINARY_DIR})

#########################################
# Custom Makefile
set(BUILD_DIR ${c_asm_vm_SOURCE_DIR}/build)

set(MAKE_EXE C:/GnuWin32/bin/make.exe -f ${c_asm_vm_SOURCE_DIR}/Makefile)
set(GTEST C:/Users/aidar.islamov/Downloads/googletest-release-1.8.1/googletest)
set(RM_ALL "del /S /Q")

add_custom_target(All
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMAND ${MAKE_EXE} all
        CC=${CMAKE_C_COMPILER}
        CXX=${CMAKE_CXX_COMPILER}
        LD=${CMAKE_CXX_COMPILER}
        GTEST=${GTEST}
        SRCD=${c_asm_vm_SOURCE_DIR}
        RM_ALL=${RM_ALL})

add_custom_target(check
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMAND ${MAKE_EXE} check
        CC=${CMAKE_C_COMPILER}
        CXX=${CMAKE_CXX_COMPILER}
        LD=${CMAKE_CXX_COMPILER}
        GTEST=${GTEST}
        SRCD=${c_asm_vm_SOURCE_DIR}
        RM_ALL=${RM_ALL})

add_custom_target(asm
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMAND ${MAKE_EXE} asm
        CC=${CMAKE_C_COMPILER}
        CXX=${CMAKE_CXX_COMPILER}
        LD=${CMAKE_CXX_COMPILER}
        GTEST=${GTEST}
        SRCD=${c_asm_vm_SOURCE_DIR}
        RM_ALL=${RM_ALL})

add_custom_target(vm
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMAND ${MAKE_EXE} vm
        CC=${CMAKE_C_COMPILER}
        CXX=${CMAKE_CXX_COMPILER}
        LD=${CMAKE_CXX_COMPILER}
        GTEST=${GTEST}
        SRCD=${c_asm_vm_SOURCE_DIR}
        RM_ALL=${RM_ALL})

add_custom_target(Clean
        WORKING_DIRECTORY ${BUILD_DIR}
        COMMAND ${MAKE_EXE} clean
        CC=${CMAKE_C_COMPILER}
        CXX=${CMAKE_CXX_COMPILER}
        LD=${CMAKE_CXX_COMPILER}
        GTEST=${GTEST}
        SRCD=${c_asm_vm_SOURCE_DIR}
        RM_ALL=${RM_ALL})

#include <cstdio>
#include <malloc.h>
#include <cstring>

#include "gtest/gtest.h"

extern "C" {
namespace utils {

#include "utils/strtools.h"
#include "utils/vector.h"

}
namespace Asm {
#include "asm/labels.h"
#include "asm/readtoken.h"
#include "asm/token_tools.h"
}
namespace vm {
#include "vm/vmstruct.h"
#include "vm/handlers.h"
#include "vm/inst_return_codes.h"
}
}

TEST(strToUpper, Sanity) {
    char str[] = "mIxEd";
    char expected[] = "MIXED";

    utils::strToUpper(str);

    EXPECT_TRUE(strcmp(str, expected) == 0);
}

TEST(strToLower, Sanity) {
    char str[] = "mIxEd";
    char expected[] = "mixed";

    utils::strToLower(str);

    EXPECT_TRUE(strcmp(str, expected) == 0);
}

TEST(strIsSpace, Sanity) {
    EXPECT_TRUE(utils::strIsSpace(""));
    EXPECT_TRUE(utils::strIsSpace("  "));
    EXPECT_TRUE(utils::strIsSpace("\t"));
    EXPECT_FALSE(utils::strIsSpace(" weiofjw "));
}

DEFINE_VECTOR(int)

TEST(Vector, Sanity) {
    V_int_Ptr vec;
    INIT_VEC(vec, 1, fault)

    for (int i = 0; i < 100; ++i) {
        PUSH_BACK_P(&vec, i, fault)
    }

    for (size_t i = 0; i < CNT(vec); ++i) {
        EXPECT_EQ((int)i, ID(vec, i));
    }

    EXPECT_LE(CNT(vec), SIZE(vec));

    FREE_V(vec);

    return;

    fault:
    EXPECT_TRUE(false);
}

TEST(LabDescOps, Sanity) {
    Asm::V_LabelDescPtr_Ptr vec;
    INIT_VEC(vec, 1, fault)

    do {
        EXPECT_TRUE(Asm::AddLabDesc(&vec, "one"));
        EXPECT_TRUE(Asm::AddLabDesc(&vec, "two"));
        EXPECT_TRUE(Asm::AddLabDesc(&vec, "three"));

        size_t threeId = Asm::FindLabDesc(vec, "three");
        EXPECT_LT(threeId, CNT(vec));
        EXPECT_TRUE(strcmp("three", ID(vec, threeId)->labelName) == 0);

        size_t fourId = Asm::FindLabDesc(vec, "four");
        EXPECT_EQ(fourId, CNT(vec));
    } while (0);

    for (size_t i = 0; i < CNT(vec); ++i) {
        Asm::FreeDesc(ID(vec, i));
    }

    FREE_V(vec)

    return;

    fault:
    EXPECT_TRUE(false);
}

TEST(ReadToken, Sanity) {
    char fileName[] = "testfile";
    FILE *fp = fopen(fileName, "w+");
    fprintf(fp, "  TOKEN1 \ttoken2\n token3  verylongtoken  ");

    ASSERT_TRUE(fseek(fp, 0UL, SEEK_SET) == 0);

    char buffer[8];

    EXPECT_TRUE(Asm::ReadToken(fp, buffer, 8));
    EXPECT_TRUE(strcmp(buffer, "TOKEN1") == 0);

    EXPECT_TRUE(Asm::ReadToken(fp, buffer, 8));
    EXPECT_TRUE(strcmp(buffer, "TOKEN2") == 0);

    EXPECT_TRUE(Asm::ReadToken(fp, buffer, 8));
    EXPECT_TRUE(strcmp(buffer, "TOKEN3") == 0);

    EXPECT_FALSE(Asm::ReadToken(fp, buffer, 8));

    EXPECT_TRUE(Asm::ReadToken(fp, buffer, 8));

    fclose(fp);

    remove(fileName);
}

TEST(IsRx, Sanity) {
    EXPECT_FALSE(Asm::IsRx(""));
    EXPECT_FALSE(Asm::IsRx("r0x"));
    EXPECT_FALSE(Asm::IsRx(" R0X"));
    EXPECT_FALSE(Asm::IsRx("R8X"));

    EXPECT_TRUE(Asm::IsRx("R0X"));
    EXPECT_TRUE(Asm::IsRx("R1X"));
    EXPECT_TRUE(Asm::IsRx("R2X"));
    EXPECT_TRUE(Asm::IsRx("R3X"));
    EXPECT_TRUE(Asm::IsRx("R4X"));
    EXPECT_TRUE(Asm::IsRx("R5X"));
    EXPECT_TRUE(Asm::IsRx("R6X"));
    EXPECT_TRUE(Asm::IsRx("R7X"));
    EXPECT_TRUE(Asm::IsRx("SP"));
}

TEST(IsRs, Sanity) {
    EXPECT_FALSE(Asm::IsRs(""));
    EXPECT_FALSE(Asm::IsRs("r0l"));
    EXPECT_FALSE(Asm::IsRs(" R0H"));
    EXPECT_FALSE(Asm::IsRs("R8H"));

    EXPECT_TRUE(Asm::IsRs("R0L"));
    EXPECT_TRUE(Asm::IsRs("R1L"));
    EXPECT_TRUE(Asm::IsRs("R2L"));
    EXPECT_TRUE(Asm::IsRs("R3L"));
    EXPECT_TRUE(Asm::IsRs("R4L"));
    EXPECT_TRUE(Asm::IsRs("R5L"));
    EXPECT_TRUE(Asm::IsRs("R6L"));
    EXPECT_TRUE(Asm::IsRs("R7L"));
    EXPECT_TRUE(Asm::IsRs("R0H"));
    EXPECT_TRUE(Asm::IsRs("R1H"));
    EXPECT_TRUE(Asm::IsRs("R2H"));
    EXPECT_TRUE(Asm::IsRs("R3H"));
    EXPECT_TRUE(Asm::IsRs("R4H"));
    EXPECT_TRUE(Asm::IsRs("R5H"));
    EXPECT_TRUE(Asm::IsRs("R6H"));
    EXPECT_TRUE(Asm::IsRs("R7H"));
}

TEST(RxToId, Sanity) {
    EXPECT_EQ(Asm::RxToId("R0X"), 0);
    EXPECT_EQ(Asm::RxToId("R1X"), 1);
    EXPECT_EQ(Asm::RxToId("R2X"), 2);
    EXPECT_EQ(Asm::RxToId("R3X"), 3);
    EXPECT_EQ(Asm::RxToId("R4X"), 4);
    EXPECT_EQ(Asm::RxToId("R5X"), 5);
    EXPECT_EQ(Asm::RxToId("R6X"), 6);
    EXPECT_EQ(Asm::RxToId("R7X"), 7);
    EXPECT_EQ(Asm::RxToId("SP"), 7);
}

TEST(RsToId, Sanity) {
    EXPECT_EQ(Asm::RsToId("R0L"), (0 << 1) | 0);
    EXPECT_EQ(Asm::RsToId("R1L"), (1 << 1) | 0);
    EXPECT_EQ(Asm::RsToId("R2L"), (2 << 1) | 0);
    EXPECT_EQ(Asm::RsToId("R3L"), (3 << 1) | 0);
    EXPECT_EQ(Asm::RsToId("R4L"), (4 << 1) | 0);
    EXPECT_EQ(Asm::RsToId("R5L"), (5 << 1) | 0);
    EXPECT_EQ(Asm::RsToId("R6L"), (6 << 1) | 0);
    EXPECT_EQ(Asm::RsToId("R7L"), (7 << 1) | 0);

    EXPECT_EQ(Asm::RsToId("R0H"), (0 << 1) | 1);
    EXPECT_EQ(Asm::RsToId("R1H"), (1 << 1) | 1);
    EXPECT_EQ(Asm::RsToId("R2H"), (2 << 1) | 1);
    EXPECT_EQ(Asm::RsToId("R3H"), (3 << 1) | 1);
    EXPECT_EQ(Asm::RsToId("R4H"), (4 << 1) | 1);
    EXPECT_EQ(Asm::RsToId("R5H"), (5 << 1) | 1);
    EXPECT_EQ(Asm::RsToId("R6H"), (6 << 1) | 1);
    EXPECT_EQ(Asm::RsToId("R7H"), (7 << 1) | 1);
}

TEST(IS_LABEL_, Sanity) {
    EXPECT_TRUE(IS_LABEL("label:"));
    EXPECT_TRUE(IS_LABEL("l:"));
    EXPECT_TRUE(IS_LABEL("aBaCaBa:"));

    EXPECT_FALSE(IS_LABEL("MOV"));
    EXPECT_FALSE(IS_LABEL("R0X"));
    EXPECT_FALSE(IS_LABEL("#123"));
}

TEST(ParseImm8, Sanity) {
    uint8_t val;

    EXPECT_FALSE(Asm::ParseImm8("123", nullptr));
    EXPECT_FALSE(Asm::ParseImm8("#256", nullptr));
    EXPECT_FALSE(Asm::ParseImm8("#1234", nullptr));
    EXPECT_FALSE(Asm::ParseImm8("#-1", nullptr));

    EXPECT_TRUE(Asm::ParseImm8("#0", &val));
    EXPECT_EQ(val, 0);
    EXPECT_TRUE(Asm::ParseImm8("#1", &val));
    EXPECT_EQ(val, 1);
    EXPECT_TRUE(Asm::ParseImm8("#255", &val));
    EXPECT_EQ(val, 255);
}

TEST(ParseImm4, Sanity) {
    uint8_t val;

    EXPECT_FALSE(Asm::ParseImm4("12", nullptr));
    EXPECT_FALSE(Asm::ParseImm4("#16", nullptr));
    EXPECT_FALSE(Asm::ParseImm4("#123", nullptr));
    EXPECT_FALSE(Asm::ParseImm4("#-1", nullptr));

    EXPECT_TRUE(Asm::ParseImm4("#0", &val));
    EXPECT_EQ(val, 0);
    EXPECT_TRUE(Asm::ParseImm4("#1", &val));
    EXPECT_EQ(val, 1);
    EXPECT_TRUE(Asm::ParseImm4("#15", &val));
    EXPECT_EQ(val, 15);
}

TEST(TrimBraces, Sanity) {
    char str[] = "(string)";

    Asm::TrimBrackets(str);

    EXPECT_TRUE(strcmp(str, "string") == 0);
}

TEST(ISMEMOP_, Sanity) {
    EXPECT_TRUE(ISMEMOP("()"));
    EXPECT_TRUE(ISMEMOP("(123)"));
    EXPECT_TRUE(ISMEMOP("(R3X)"));

    EXPECT_FALSE(ISMEMOP("R3X"));
    EXPECT_FALSE(ISMEMOP("(R3X"));
    EXPECT_FALSE(ISMEMOP("R3X)"));
}

vm::VM _vm;

TEST(initVM, Sanity) {
    ASSERT_EQ(vm::REG_NUM, 8);
    ASSERT_EQ(vm::SP_ID, 7);

    for (size_t i = 0; i < vm::REG_NUM; ++i) {
        _vm.R[i] = static_cast<uint16_t>(i + 1);
    }

    ASSERT_NE(_vm.R[0], 0);
    ASSERT_NE(_vm.R[1], 0);
    ASSERT_NE(_vm.R[2], 0);
    ASSERT_NE(_vm.R[3], 0);
    ASSERT_NE(_vm.R[4], 0);
    ASSERT_NE(_vm.R[5], 0);
    ASSERT_NE(_vm.R[6], 0);
    ASSERT_NE(_vm.R[7], 0);

    vm::initVM(&_vm, 42);

    EXPECT_EQ(_vm.R[0], 0);
    EXPECT_EQ(_vm.R[1], 0);
    EXPECT_EQ(_vm.R[2], 0);
    EXPECT_EQ(_vm.R[3], 0);
    EXPECT_EQ(_vm.R[4], 0);
    EXPECT_EQ(_vm.R[5], 0);
    EXPECT_EQ(_vm.R[6], 0);
    EXPECT_EQ(_vm.R[7], 42);
}

TEST(SetAtAddr, Sanity) {
    vm::set16AtAddr(&_vm, 0, 0x0000);
    EXPECT_EQ(_vm.mem[0], 0x00);
    EXPECT_EQ(_vm.mem[1], 0x00);

    vm::set16AtAddr(&_vm, 0, 0x1234);
    EXPECT_EQ(_vm.mem[0], 0x34);
    EXPECT_EQ(_vm.mem[1], 0x12);

    vm::set16AtAddr(&_vm, 4, 0xABCD);
    EXPECT_EQ(_vm.mem[4], 0xCD);
    EXPECT_EQ(_vm.mem[5], 0xAB);
}

TEST(GET_, Sanoty) {
    vm::set16AtAddr(&_vm, 0, 0x1234U);
    vm::set16AtAddr(&_vm, 2, 0x5678U);
    vm::set16AtAddr(&_vm, 4, 0x9ABCU);

    EXPECT_EQ(GET(&_vm, 0), 0x1234U);
    EXPECT_EQ(GET(&_vm, 2), 0x5678U);
    EXPECT_EQ(GET(&_vm, 4), 0x9ABCU);
}

TEST(GET_ARG_, Sanity) {
    EXPECT_EQ(GET_ARG(0b110, 2, 0), 0b10);
    EXPECT_EQ(GET_ARG(0b110, 2, 1), 0b11);
    EXPECT_EQ(GET_ARG(0b1010, 3, 1), 0b101);
    EXPECT_EQ(GET_ARG(0b01111010000, 4, 7), 0b0111);
}

TEST(movRR, Sanity) {
    _vm.R[1] = 123;
    _vm.R[5] = 231;

    vm::movRR(&_vm, 0b00000'00000'001'101);

    EXPECT_EQ(_vm.R[1], 231);
}

TEST(movRRp, Sanity) {
    _vm.R[1] = 123;
    _vm.R[5] = 4;
    vm::set16AtAddr(&_vm, 4, 0xABCD);

    EXPECT_EQ(vm::movRRp(&_vm, 0b00001'00000'001'101), vm::CONTINUE);

    EXPECT_EQ(_vm.R[1], 0xABCD);
}

TEST(movRRp, UnalignedAccess) {
    _vm.R[5] = 3;

    EXPECT_EQ(vm::movRRp(&_vm, 0b00001'00000'001'101), vm::UNALIGNED_MEM_ACCESS);
}

TEST(movRpR, Sanity) {
    _vm.R[1] = 0xABCD;
    _vm.R[5] = 4;

    EXPECT_EQ(vm::movRpR(&_vm, 0b00000'00000'101'001), vm::CONTINUE);

    EXPECT_EQ(GET(&_vm, 4), 0xABCD);
}

TEST(movRpR, UnalignedAccess) {
    _vm.R[5] = 3;

    EXPECT_EQ(vm::movRpR(&_vm, 0b00001'00000'101'001), vm::UNALIGNED_MEM_ACCESS);
}

TEST(movRsI, Sanity) {
    _vm.R[5] = 0;

    EXPECT_EQ(vm::movRsI(&_vm, 0b0000'1010'00000000 | 0xAB), vm::CONTINUE);
    EXPECT_EQ(vm::movRsI(&_vm, 0b0000'1011'00000000 | 0xCD), vm::CONTINUE);

    EXPECT_EQ(_vm.R[5], 0xCDAB);
}

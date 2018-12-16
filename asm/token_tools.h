#pragma once

bool IsRx(const char *rx);

int RxToId(const char *rx);

bool IsRs(const char *rx);

int RsToId(const char *rx);

#define IS_LABEL(str) ((str)[strlen(str) - 1] == ':')

bool ParseImm8(const char* str, uint8_t* imm8);

bool ParseImm4(const char* str, uint8_t* imm4);

void TrimBrackets(char* str);

#define ISMEMOP(a) (((a)[0] == '(' && (a)[strlen((a)) - 1] == ')') ? 1 : 0)

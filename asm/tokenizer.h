#pragma once

struct Tokenizer;

char* GetToken(FILE* file);

void InitTokenizer(struct Tokenizer *tokenizer);
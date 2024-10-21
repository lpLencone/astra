#ifndef ASTRA_LEXER_H_
#define ASTRA_LEXER_H_

#include <stddef.h>

#include "slice.h"
#include "da.h"

typedef enum {
    TokenKind_Exit,
    TokenKind_Identifier,
    TokenKind_IntLit,
    TokenKind_Semicolon,
} TokenKind;

typedef struct {
    TokenKind kind;
    Slice slice;
} Token;

typedef DArray(Token) DArrayToken;

DArrayToken lexer_analyse(Slice src);

char const *token_kind_cstr(TokenKind kind);

#endif // ASTRA_LEXER_H_

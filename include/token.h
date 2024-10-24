#ifndef TOKEN_H_
#define TOKEN_H_

#include <stdio.h>
#include <stdlib.h>

#include "da.h"
#include "slice.h"
#include "lib.h"

typedef enum {
    // Keywords (terminals)
    TokenKind_Exit = 1,
    TokenKind_I64,
    // Terminals
    TokenKind_Semicolon,
    TokenKind_LeftParen,
    TokenKind_RightParen,
    TokenKind_Assign,
    TokenKind_AddSign,
    TokenKind_SubSign,
    TokenKind_MulSign,
    TokenKind_DivSign,
    // Non-terminals
    TokenKind_Identifier,
    TokenKind_IntLit,
} TokenKind;

typedef struct {
    TokenKind kind;
    union {
        Slice lexeme;
        int64_t intlit;
    };
} Token;

typedef DArray(Token) DArrayToken;

char const *token_kind_cstr(TokenKind kind);

#endif // TOKEN_H_

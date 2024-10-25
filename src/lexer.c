#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

static char const *const separators = " \n\t;()";
static char const *const whitespace = " \n\t";

DArrayToken lexer_analyse(Slice src)
{
    DArrayToken tokens = { 0 };

    Slice token_lexeme;
    while (!slice_isnull(token_lexeme = slice_token(&src, separators))) {
        token_lexeme = slice_triml(token_lexeme, whitespace);

        Token tok = { .lexeme = token_lexeme };
        if (isalpha(*token_lexeme.data)) {
            if (slice_isalnum(token_lexeme)) {
                tok.kind = TokenKind_Identifier;
                if (slice_eq_cstr(tok.lexeme, "exit")) {
                    tok.kind = TokenKind_Exit;
                } else if (slice_eq_cstr(tok.lexeme, "u64")) {
                    tok.kind = TokenKind_U64;
                }
            } else {
                panic("Invalid token \"%.*s\"", SLICEFMT(token_lexeme));
            }

        } else if (slice_isdigit(token_lexeme)) {
            tok.kind = TokenKind_IntLit;
            tok.intlit = slice_uint64(tok.lexeme, NULL);
        } else if (slice_eq_cstr(token_lexeme, ";")) {
            tok.kind = TokenKind_Semicolon;
        } else if (slice_eq_cstr(token_lexeme, "(")) {
            tok.kind = TokenKind_LeftParen;
        } else if (slice_eq_cstr(token_lexeme, ")")) {
            tok.kind = TokenKind_RightParen;
        } else if (slice_eq_cstr(token_lexeme, "=")) {
            tok.kind = TokenKind_Assign;
        } else if (slice_eq_cstr(token_lexeme, "+")) {
            tok.kind = TokenKind_AddOp;
        } else if (slice_eq_cstr(token_lexeme, "-")) {
            tok.kind = TokenKind_SubOp;
        } else if (slice_eq_cstr(token_lexeme, "*")) {
            tok.kind = TokenKind_MulOp;
        } else if (slice_eq_cstr(token_lexeme, "/")) {
            tok.kind = TokenKind_DivOp;

        } else {
            panic("Invalid token string: \"%.*s\"\n", SLICEFMT(token_lexeme));
        }

        da_push(&tokens, &tok);

        src = slice_triml(src, whitespace);
    }
    return tokens;
}

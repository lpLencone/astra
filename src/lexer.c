#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

static char const *const separators = " \n\t;";
static char const *const whitespace = " \n\t";

DArrayToken lexer_analyse(Slice src)
{
    DArrayToken tokens = { 0 };

    Slice token_slice;
    src = slice_triml(src, whitespace);
    while (!slice_isnull((token_slice = slice_token(&src, separators)))) {
        src = slice_triml(src, whitespace);

        Token tok = { 0 };
        if (isalpha(*token_slice.data)) {
            if (slice_isalnum(token_slice)) {
                tok.kind = TokenKind_Identifier;
                tok.slice = token_slice;
                if (slice_eq_cstr(tok.slice, "exit")) {
                    tok.kind = TokenKind_Exit;
                } else {
                    panic("Cannot handle non-\"exit\" identifiers");
                }
            } else {
                panic("Invalid token.");
            }

        } else if (slice_isdigit(token_slice)) {
            tok.kind = TokenKind_IntLit;
            tok.slice = token_slice;

        } else if (slice_eq_cstr(token_slice, ";")) {
            tok.kind = TokenKind_Semicolon;
            tok.slice = token_slice;

        } else {
            panic("What is this? %s\n", src.data);
        }

        da_push(&tokens, &tok);
    }
    return tokens;
}

char const *token_kind_cstr(TokenKind kind)
{
    switch (kind) {
        case TokenKind_Exit:
            return "TokenKind_Exit";
        case TokenKind_Identifier:
            return "TokenKind_Identifier";
        case TokenKind_IntLit:
            return "TokenKind_IntLit";
        case TokenKind_Semicolon:
            return "TokenKind_Semicolon";
        default:
            panic("Unreachable");
    }
}

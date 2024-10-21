#include "parser.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

static bool parse_expr(Token tok, NodeExpr *expr)
{
    if (tok.kind != TokenKind_IntLit) {
        eprintf("Token kind is not IntLit\n");
        return false;
    }
    expr->token = tok;
    return true;
}

bool parse(DArrayToken tokens, NodeExit *root)
{
    // TODO: "Expected token blah-blah-blah..."
#define toknext (assert(i < tokens.length), tokens.data[i++])

    size_t i = 0;
    while (i < tokens.length) {
        Token tok = toknext;
        if (tok.kind == TokenKind_Exit) {
            NodeExpr expr;
            if (!parse_expr(toknext, &expr)) {
                return false;
            }
            root->expr = expr;
            if (toknext.kind != TokenKind_Semicolon) {
                eprintf("Token kind is not semicolon\n");
                return false;
            }
        } else {
            panic("What do now that kind is %s?", token_kind_cstr(tok.kind));
        }
    }
    return true;

#undef toknext
}

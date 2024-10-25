#include "token.h"
#include "lib.h"

inline char const *token_kind_cstr(TokenKind kind)
{
    switch (kind) {
        case TokenKind_Exit:
            return "TokenKind_Exit";
        case TokenKind_U64:
            return "TokenKind_I64";
        case TokenKind_Semicolon:
            return "TokenKind_Semicolon";
        case TokenKind_LeftParen:
            return "TokenKind_LeftParen";
        case TokenKind_RightParen:
            return "TokenKind_RightParen";
        case TokenKind_Assign:
            return "TokenKind_Assign";
        case TokenKind_AddOp:
            return "TokenKind_AddOp";
        case TokenKind_SubOp:
            return "TokenKind_SubOp";
        case TokenKind_MulOp:
            return "TokenKind_MulOp";
        case TokenKind_DivOp:
            return "TokenKind_DivOp";
        case TokenKind_Identifier:
            return "TokenKind_Identifier";
        case TokenKind_IntLit:
            return "TokenKind_IntLit";
    }
    panic("Unknown token kind: %d", kind);
}


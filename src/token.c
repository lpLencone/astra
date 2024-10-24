#include "token.h"

inline char const *token_kind_cstr(TokenKind kind)
{
    switch (kind) {
        case TokenKind_Exit:
            return "TokenKind_Exit";
        case TokenKind_I64:
            return "TokenKind_I64";
        case TokenKind_Semicolon:
            return "TokenKind_Semicolon";
        case TokenKind_LeftParen:
            return "TokenKind_LeftParen";
        case TokenKind_RightParen:
            return "TokenKind_RightParen";
        case TokenKind_Assign:
            return "TokenKind_Assign";
        case TokenKind_AddSign:
            return "TokenKind_AddSign";
        case TokenKind_SubSign:
            return "TokenKind_SubSign";
        case TokenKind_MulSign:
            return "TokenKind_MulSign";
        case TokenKind_DivSign:
            return "TokenKind_DivSign";
        case TokenKind_Identifier:
            return "TokenKind_Identifier";
        case TokenKind_IntLit:
            return "TokenKind_IntLit";
    }
    panic("Unreachable");
}


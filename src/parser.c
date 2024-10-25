#include "parser.h"

#include <assert.h>

#include "lib.h"
#include "token.h"

typedef struct {
    DArrayToken tokens;
    size_t index;
    Arena *arena;
} Parser;

#define parser_panic(p, fmt, ...)              \
    do {                                       \
        arena_free(p->arena);                  \
        panic(fmt __VA_OPT__(, ) __VA_ARGS__); \
    } while (0)

static Token parse_next(Parser *p)
{
    expect(p->index < p->tokens.length, "Token exhaustion.");
    return p->tokens.data[p->index++];
}

static void parse_rewind(Parser *p)
{
    expect(p->index > 0, "Can't rewind from initial position.");
    p->index--;
}

static NodeExpr *parse_expr(Parser *p);

static NodeFactor *parse_factor(Parser *p)
{
    Token tok = parse_next(p);
    NodeFactor *node_factor = arena_alloc(p->arena, sizeof *node_factor);
    switch (tok.kind) {
        case TokenKind_IntLit:
            node_factor->kind = FactorKind_Int;
            node_factor->token = tok;
            break;
        case TokenKind_Identifier:
            node_factor->kind = FactorKind_Id;
            node_factor->token = tok;
            break;
        case TokenKind_LeftParen:
            node_factor->kind = FactorKind_Expr;
            node_factor->expr = parse_expr(p);
            expect(parse_next(p).kind == TokenKind_RightParen, "Parenthesis unclosed");
            break;
        default:
            parser_panic(
                    p, "Unexpected token for factor: \"%s\"", token_kind_cstr(tok.kind));
    }
    return node_factor;
}

static NodeTerm *parse_term_g(Parser *p, NodeTerm *child)
{
    Token tok = parse_next(p);
    TermKind kind;
    switch (tok.kind) {
        case TokenKind_MulOp:
            kind = TermKind_Mul;
            break;
        case TokenKind_DivOp:
            kind = TermKind_Div;
            break;
        default:
            parse_rewind(p);
            return child;
    }
    NodeTerm *node_term = arena_alloc(p->arena, sizeof *node_term);
    node_term->kind = kind;
    node_term->term = child;
    node_term->factor = parse_factor(p);
    return parse_term_g(p, node_term);
}

static NodeTerm *parse_term(Parser *p)
{
    NodeTerm *node_term = arena_alloc(p->arena, sizeof *node_term);
    node_term->factor = parse_factor(p);
    node_term->kind = TermKind_Factor;
    return parse_term_g(p, node_term);
}

static NodeExpr *parse_expr_g(Parser *p, NodeExpr *child)
{
    Token tok = parse_next(p);
    ExprKind kind;
    switch (tok.kind) {
        case TokenKind_AddOp:
            kind = ExprKind_Add;
            break;
        case TokenKind_SubOp:
            kind = ExprKind_Sub;
            break;
        default:
            parse_rewind(p);
            return child;
    }
    NodeExpr *node_expr = arena_alloc(p->arena, sizeof *node_expr);
    node_expr->kind = kind;
    node_expr->expr = child;
    node_expr->term = parse_term(p);
    return parse_expr_g(p, node_expr);
}

static NodeExpr *parse_expr(Parser *p)
{
    NodeExpr *node_expr = arena_alloc(p->arena, sizeof *node_expr);
    node_expr->kind = ExprKind_Term;
    node_expr->term = parse_term(p);
    return parse_expr_g(p, node_expr);
}

static Token parse_id(Parser *p)
{
    Token tok = parse_next(p);
    expect(tok.kind == TokenKind_Identifier, "Token (%s) should be identifier",
           token_kind_cstr(tok.kind));
    return tok;
}

static void parse_semi(Parser *p)
{
    TokenKind kind = parse_next(p).kind;
    expect(kind == TokenKind_Semicolon, "Token (%s) should be semicolon <;>",
           token_kind_cstr(kind));
}

static void parse_eq(Parser *p)
{
    TokenKind kind = parse_next(p).kind;
    expect(kind == TokenKind_Assign, "Token (%s) should be Assign <=>",
           token_kind_cstr(kind));
}

static NodeStmt parse_stmt(Parser *p)
{
    Token tok = parse_next(p);
    NodeStmt stmt;
    if (tok.kind == TokenKind_U64) {
        stmt.kind = StmtKind_Decl;
        stmt.id = parse_id(p);
        parse_eq(p);
        stmt.expr = parse_expr(p);

    } else if (tok.kind == TokenKind_Exit) {
        stmt.kind = StmtKind_Exit;
        stmt.expr = parse_expr(p);

    } else {
        panic("Invalid token (%s, %.*s)", token_kind_cstr(tok.kind),
              SLICEFMT(tok.lexeme));
    }

    // Maybe instead of a token for each terminal, like `;`, There should be a
    // TokenKind_Terminal against which a `match` function would be called comparing its
    // slice to an argument `terminal` match(p, ";") :: slice_eq_cstr(tok.slice, ";") ?
    // tok = parse_next(p) : panic()
    parse_semi(p);
    return stmt;
}

NodeProg parse(DArrayToken tokens, Arena *arena)
{
    Parser p = {
        .tokens = tokens,
        .arena = arena,
    };
    NodeProg prog = { 0 };
    while (p.index < p.tokens.length) {
        NodeStmt stmt = parse_stmt(&p);
        // Use arena to allocate statements
        da_push(&prog.stmts, &stmt);
    }
    return prog;
}

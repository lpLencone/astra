#include "parser.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"
#include "lib.h"
#include "token.h"

typedef struct {
    DArrayToken tokens;
    size_t index;
    Arena arena;
} Parser;

#define parser_panic(p, fmt, ...)              \
    do {                                       \
        parser_free(p);                        \
        panic(fmt __VA_OPT__(, ) __VA_ARGS__); \
    } while (0)

static void parser_free(Parser *p)
{
    arena_free(&p->arena);
}

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
    NodeFactor *node_factor = arena_alloc(&p->arena, sizeof *node_factor);
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

void parse_term_g(Parser *p, NodeTerm *node_term)
{
    Token tok = parse_next(p);
    switch (tok.kind) {
        case TokenKind_MulSign:
            node_term->kind = TermKind_Mul;
            break;
        case TokenKind_DivSign:
            node_term->kind = TermKind_Div;
            break;
        default:
            node_term->kind = TermKind_Factor;
            parse_rewind(p);
            return;
    }
    node_term->term = arena_alloc(&p->arena, sizeof *node_term);
    node_term->term->factor = parse_factor(p);
    parse_term_g(p, node_term->term);
}

static NodeTerm *parse_term(Parser *p)
{
    NodeTerm *node_term = arena_alloc(&p->arena, sizeof *node_term);
    node_term->factor = parse_factor(p);
    parse_term_g(p, node_term);
    return node_term;
}

static void parse_expr_g(Parser *p, NodeExpr *node_expr)
{
    Token tok = parse_next(p);
    switch (tok.kind) {
        case TokenKind_AddSign:
            node_expr->kind = ExprKind_Add;
            break;
        case TokenKind_SubSign:
            node_expr->kind = ExprKind_Sub;
            break;
        default:
            node_expr->kind = ExprKind_Term;
            parse_rewind(p);
            return;
    }
    node_expr->expr = arena_alloc(&p->arena, sizeof *node_expr->expr);
    node_expr->expr->term = parse_term(p);
    parse_expr_g(p, node_expr->expr);
}

static NodeExpr *parse_expr(Parser *p)
{
    NodeExpr *node_expr = arena_alloc(&p->arena, sizeof *node_expr);
    node_expr->term = parse_term(p);
    parse_expr_g(p, node_expr);
    return node_expr;
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

static NodeStmt parse_stmt(Parser *p, SymbolMap *sm)
{
    Token tok = parse_next(p);
    NodeStmt stmt;
    if (tok.kind == TokenKind_I64) {
        stmt.kind = StmtKind_Decl;
        stmt.id = parse_id(p);
        parse_eq(p);
        stmt.expr = parse_expr(p);
        SymbolEntry se = {
            // TODO: Cannot parse expression everytime
            .intlit = node_expr_eval(stmt.expr, sm),
        };
        symbolmap_put(sm, stmt.id.lexeme, se);

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

NodeProg parse(DArrayToken tokens, SymbolMap *sm)
{
    Parser p = {
        .tokens = tokens,
    };
    NodeProg prog = { 0 };
    while (p.index < p.tokens.length) {
        NodeStmt stmt = parse_stmt(&p, sm);
        da_push(&prog.stmts, &stmt);
    }
    return prog;
}

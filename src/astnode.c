#include "astnode.h"
#include "lib.h"

char const *node_kind_cstr(NodeKind kind)
{
    switch (kind) {
        case NodeKind_Expr:
            return "NodeKind_Expr";
        case NodeKind_Term:
            return "NodeKind_Term";
        case NodeKind_Factor:
            return "NodeKind_Factor";
        case NodeKind_Stmt:
            return "NodeKind_Stmt";
        case NodeKind_Prog:
            return "NodeKind_Prog";
    }
    panic("node_kind_cstr: Unreachable");
}

bool node_expr_is_id(NodeExpr *node_expr)
{
    return node_expr->kind == ExprKind_Term && node_expr->term->kind == TermKind_Factor &&
           node_expr->term->factor->kind == FactorKind_Id;
}

Slice node_expr_get_id_lexeme(NodeExpr *node_expr)
{
    expect(node_expr_is_id(node_expr),
           "Cannot get identifier lexeme from non-identifier expression");
    return node_expr->term->factor->token.lexeme;
}

uint64_t node_factor_eval(NodeFactor *node_factor, SymbolMap *sm)
{
    expect(node_factor != NULL, "Node factor cannot be NULL");
    switch (node_factor->kind) {
        case FactorKind_Expr:
            return node_expr_eval(node_factor->expr, sm);
        case FactorKind_Int:
            return node_factor->token.intlit;
        case FactorKind_Id:
            return symbolmap_get(sm, node_factor->token.lexeme)->intlit;
    }
    panic("Unknown FactorKind %d", node_factor->kind);
}

uint64_t node_term_eval(NodeTerm *node_term, SymbolMap *sm)
{
    expect(node_term != NULL, "Node term cannot be NULL");
    switch (node_term->kind) {
        case TermKind_Factor:
            return node_factor_eval(node_term->factor, sm);
        case TermKind_Mul:
            return node_factor_eval(node_term->factor, sm) * // [factor] [ {*|/} g-term ]
                   node_term_eval(node_term->term, sm);
        case TermKind_Div:
            return node_factor_eval(node_term->factor, sm) /
                   node_term_eval(node_term->term, sm);
    }
    panic("Unknown TermKind %d", node_term->kind);
}

uint64_t node_expr_eval(NodeExpr *node_expr, SymbolMap *sm)
{
    expect(node_expr != NULL, "Node expression cannot be NULL");
    switch (node_expr->kind) {
        case ExprKind_Term:
            return node_term_eval(node_expr->term, sm);
        case ExprKind_Add:
            return node_term_eval(node_expr->term, sm) + // [term] [G-expr]
                   node_expr_eval(node_expr->expr, sm);
        case ExprKind_Sub:
            return node_term_eval(node_expr->term, sm) - // [term] [G-expr]
                   node_expr_eval(node_expr->expr, sm);
    }
    panic("Unknown ExprKind %d", node_expr->kind);
}

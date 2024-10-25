#include "astnode.h"
#include "lib.h"

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
            return node_term_eval(node_term->term, sm) * // [factor] [ {*|/} g-term ]
                   node_factor_eval(node_term->factor, sm);
        case TermKind_Div:
            return node_term_eval(node_term->term, sm) /
                   node_factor_eval(node_term->factor, sm);
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
            return node_expr_eval(node_expr->expr, sm) + // [term] [G-expr]
                   node_term_eval(node_expr->term, sm);
        case ExprKind_Sub:
            return node_expr_eval(node_expr->expr, sm) - // [term] [G-expr]
                   node_term_eval(node_expr->term, sm);
    }
    panic("Unknown ExprKind %d", node_expr->kind);
}

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

static void print_stmt(NodeStmt *stmt, size_t depth);

void print_ast(NodeProg *prog)
{
    printf("Prog {\n");
    for (size_t i = 0; i < prog->stmts.length; i++) {
        print_stmt(&prog->stmts.data[i], 2);
    }
    printf("}\n");
}

/*
 * Ast Print
 */

static void print_expr(NodeExpr *expr);

static void print_factor(NodeFactor *factor)
{
    switch (factor->kind) {
        case FactorKind_Int:
            printf("%zu", factor->token.intlit);
            break;
        case FactorKind_Id:
            printf("%.*s", SLICEFMT(factor->token.lexeme));
            break;
        default:
            printf("(");
            print_expr(factor->expr);
            printf(")");
    }
}

static void print_term(NodeTerm *term)
{
    switch (term->kind) {
        case TermKind_Factor:
            print_factor(term->factor);
            break;
        default:
            print_term(term->term);
            printf(" %c ", term->kind == TermKind_Mul ? '*' : '/');
            print_factor(term->factor);
            break;
    }
}

static void print_expr(NodeExpr *expr)
{
    switch (expr->kind) {
        case ExprKind_Term:
            print_term(expr->term);
            break;
        default:
            print_expr(expr->expr);
            printf(" %c ", expr->kind == ExprKind_Add ? '+' : '-');
            print_term(expr->term);
            break;
    }
}

static void print_stmt(NodeStmt *stmt, size_t depth)
{
    for (size_t i = 0; i < depth; i++) {
        printf(" ");
    }
    switch (stmt->kind) {
        case StmtKind_Decl:
            printf("u64 %.*s = ", SLICEFMT(stmt->id.lexeme));
            print_expr(stmt->expr);
            printf(";\n");
            break;
        case StmtKind_Exit:
            printf("exit ");
            print_expr(stmt->expr);
            printf(";\n");
            break;
    }
}


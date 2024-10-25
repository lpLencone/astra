#ifndef ASTRA_ASTNODE_H_
#define ASTRA_ASTNODE_H_

#include "token.h"
#include "symbolmap.h"

typedef enum {
    NodeKind_Expr = 1,
    NodeKind_Term,
    NodeKind_Factor,
    NodeKind_Stmt,
    NodeKind_Prog,
} NodeKind;

typedef struct NodeExpr NodeExpr;
typedef struct NodeTerm NodeTerm;
typedef struct NodeFactor NodeFactor;

struct NodeFactor {
    enum {
        FactorKind_Int = 1,
        FactorKind_Id,
        FactorKind_Expr,
    } kind;
    union {
        Token token;
        NodeExpr *expr;
    };
};

struct NodeTerm {
    enum {
        TermKind_Mul = 1,
        TermKind_Div,
        TermKind_Factor,
    } kind;
    NodeTerm *term;
    NodeFactor *factor;
};

struct NodeExpr {
    enum {
        ExprKind_Add = 1,
        ExprKind_Sub,
        ExprKind_Term,
    } kind;
    NodeTerm *term;
    NodeExpr *expr;
};

typedef struct {
    enum {
        StmtKind_Exit = 1,
        StmtKind_Decl,
    } kind;
    Token id;
    NodeExpr *expr;
} NodeStmt;

typedef struct {
    DArray(NodeStmt) stmts;
} NodeProg;

char const *node_kind_cstr(NodeKind kind);
bool node_expr_is_id(NodeExpr *node_expr);
Slice node_expr_get_id_lexeme(NodeExpr *node_expr);

uint64_t node_factor_eval(NodeFactor *node_factor, SymbolMap *sm);
uint64_t node_term_eval(NodeTerm *node_term, SymbolMap *sm);
uint64_t node_expr_eval(NodeExpr *node_expr, SymbolMap *sm);

#endif // ASTRA_ASTNODE_H_

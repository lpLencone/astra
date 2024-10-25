#include "generator.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"
#include "symbolmap.h"

typedef struct {
    SymbolMap sm;
    uint32_t stack_offset;
} Generator;

static void generate_stmt(Generator *g, NodeStmt stmt, Sprout *assembly)
{
    switch (stmt.kind) {
        case StmtKind_Decl:
            Slice id = stmt.id.lexeme;

            if (node_expr_is_id(stmt.expr)) {
                Slice rhs_lexeme = node_expr_get_id_lexeme(stmt.expr);
                SymbolEntry *symb = symbolmap_get(&g->sm, rhs_lexeme);
                expect(symb != NULL, "Symbol used before being declared");
                if (!symbolmap_symbol_exists(&g->sm, id)) {
                    g->stack_offset += sizeof symb->intlit;
                }
                SymbolEntry declsymb = {
                    .intlit = symb->intlit,
                    .stack_offset = g->stack_offset,
                };
                symbolmap_put(&g->sm, id, declsymb);

                sprout_push_fmt(
                        assembly, "\tmov\trax, qword [rbp - %u]\n", g->stack_offset,
                        symb->stack_offset);
                sprout_push_fmt(
                        assembly, "\tmov\tqword [rbp - %u], rax\n", g->stack_offset,
                        symb->stack_offset);

            } else {
                uint64_t intlit = node_expr_eval(stmt.expr, &g->sm);
                if (!symbolmap_symbol_exists(&g->sm, id)) {
                    g->stack_offset += sizeof intlit;
                }
                SymbolEntry declsymb = {
                    .intlit = intlit,
                    .stack_offset = g->stack_offset,
                };
                symbolmap_put(&g->sm, id, declsymb);
                sprout_push_fmt(
                        assembly, "\tmov\tqword [rbp - %u], %u\n", g->stack_offset,
                        intlit);
            }

            break;

        case StmtKind_Exit:
            sprout_insert_cstr(assembly, 0, "%define\tSYS_EXIT\t0x3C\n");

#define ERROR_CODE_EXPR_BUFSZ 32
            size_t error_code;
            char error_code_buf[ERROR_CODE_EXPR_BUFSZ] = { 0 };
            if (node_expr_is_id(stmt.expr)) {
                SymbolEntry *res =
                        symbolmap_get(&g->sm, node_expr_get_id_lexeme(stmt.expr));
                expect(res != NULL, "Identifier not found in symbol map");
                error_code = res->intlit;
                expect(error_code <= UINT8_MAX, "Error code should be at largest %u",
                       UINT8_MAX);
                snprintf(
                        error_code_buf, ERROR_CODE_EXPR_BUFSZ, "[rbp - %u]",
                        res->stack_offset);
            } else {
                error_code = node_expr_eval(stmt.expr, &g->sm);
                expect(error_code <= UINT8_MAX, "Error code should be at largest %u",
                       UINT8_MAX);
                snprintf(error_code_buf, ERROR_CODE_EXPR_BUFSZ, "%zu", error_code);
            }
#undef ERROR_CODE_EXPR_BUFSZ

            // Exit
            sprout_push_cstr(assembly, "\t; Exit\n");
            sprout_push_fmt(assembly, "\tmov\trdi, %s\n", error_code_buf);
            sprout_push_cstr(assembly, "\tmov\trax, SYS_EXIT\n");
            sprout_push_cstr(assembly, "\tpop\trbp\n");
            sprout_push_cstr(assembly, "\tsyscall\n");
            break;

        default:
            panic("Invalid statement kind: %d", stmt.kind);
    }
}

Sprout generate(NodeProg *prog)
{
    Sprout assembly = { 0 };

    sprout_push_cstr(&assembly, "global\t_start\n");
    sprout_push_cstr(&assembly, "_start:\n");
    sprout_push_cstr(&assembly, "\tpush\trbp\n");
    sprout_push_cstr(&assembly, "\tmov\trbp, rsp\n");

    Generator g = { 0 };
    symbolmap_new_map(&g.sm);

    for (size_t i = 0; i < prog->stmts.length; i++) {
        NodeStmt stmt = prog->stmts.data[i];
        generate_stmt(&g, stmt, &assembly);
    }

    return assembly;
}

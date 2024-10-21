/* Syntax Analyser */

#ifndef ASTRA_PARSER_H_
#define ASTRA_PARSER_H_

#include "lexer.h" // Including definitions for Token

typedef struct {
    Token token;
} NodeExpr;

typedef struct {
    NodeExpr expr;
} NodeExit;

bool parse(DArrayToken tokens, NodeExit *root);

#endif // ASTRA_PARSER_H_

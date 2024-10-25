/* Syntax Analyser */

#ifndef ASTRA_PARSER_H_
#define ASTRA_PARSER_H_

#include "astnode.h"
#include "token.h"
#include "arena.h"

NodeProg parse(DArrayToken tokens, Arena *arena);

#endif // ASTRA_PARSER_H_

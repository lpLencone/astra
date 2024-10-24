/* Syntax Analyser */

#ifndef ASTRA_PARSER_H_
#define ASTRA_PARSER_H_

#include "astnode.h"
#include "token.h"
#include "symbolmap.h"

NodeProg parse(DArrayToken tokens, SymbolMap *sm);

#endif // ASTRA_PARSER_H_

/* Syntax Analyser */

#ifndef ASTRA_PARSER_H_
#define ASTRA_PARSER_H_

#include "astnode.h"
#include "token.h"

NodeProg parse(DArrayToken tokens);

#endif // ASTRA_PARSER_H_

#ifndef ASTRA_LEXER_H_
#define ASTRA_LEXER_H_

#include <stddef.h>

#include "token.h"

DArrayToken lexer_analyse(Slice src);

#endif // ASTRA_LEXER_H_

#ifndef SYMBOLMAP_H_
#define SYMBOLMAP_H_

#include "da.h"
#include "hashmap.h"

typedef DArray(HashMap) SymbolMap;

typedef struct {
    uint64_t intlit;
    uint32_t stack_offset;
} SymbolEntry;

void symbolmap_free(SymbolMap *sm);
void symbolmap_new_map(SymbolMap *sm);
// Put token to last map in symbolmap array
void symbolmap_put(SymbolMap *sm, Slice id, SymbolEntry se);
// Remove last map from symbolmap array
void symbolmap_pop(SymbolMap *sm);
SymbolEntry *symbolmap_get(SymbolMap *sm, Slice id);

bool symbolmap_symbol_exists(SymbolMap *sm, Slice id);

#endif // SYMBOLMAP_H_

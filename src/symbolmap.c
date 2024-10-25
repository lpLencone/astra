#include "symbolmap.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

void symbolmap_free(SymbolMap *sm)
{
    for (size_t i = 0; i < sm->length; i++) {
        hm_free(sm->data + i);
    }
    da_free(sm);
}

void symbolmap_new_map(SymbolMap *sm)
{
    HashMap map = hm_new(100, dirty_hash);
    da_push(sm, &map);
}

void symbolmap_put(SymbolMap *sm, Slice id, SymbolEntry se)
{
    expect(sm->length > 0, "Need to create a new map before putting symbols");
    hm_update_or_insert(sm->data + sm->length - 1, id, &se, sizeof se);
}

void symbolmap_pop(SymbolMap *sm)
{
    expect(sm->length > 0, "No map to pop");
    da_pop(sm);
}

SymbolEntry *symbolmap_get(SymbolMap *sm, Slice id)
{
    for (int i = sm->length - 1; i >= 0; i--) {
        SymbolEntry *value = hm_get(sm->data + i, id);
        if (value != NULL) {
            return value;
        }
    }
    return NULL;
}

bool symbolmap_symbol_exists(SymbolMap *sm, Slice id)
{
    return symbolmap_get(sm, id) != NULL;
}


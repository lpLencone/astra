#ifndef ASTRA_HASHMAP_H_
#define ASTRA_HASHMAP_H_

#include <stdint.h>
#include <stdbool.h>

#include "slice.h"

typedef size_t (*HashFn)(uint8_t const *key, size_t keylen);

typedef enum {
    BucketStatus_Empty = 0,
    BucketStatus_Occupied,
    BucketStatus_Removed,
} BucketStatus;

typedef struct {
    Slice key;
    void *value;
    BucketStatus status;
} Bucket;

typedef struct {
    size_t capacity;
    size_t count;
    Bucket *buckets;
    HashFn hash;
} HashMap;

void hm_free(HashMap *hm);
HashMap hm_new(size_t capacity, HashFn hash);

// Keys are merely referenced inside the hashmap; caller should mind lifetimes
void hm_insert(HashMap *hm, Slice key, void *value, size_t valsz);
void hm_update_or_insert(HashMap *hm, Slice key, void *value, size_t valsz);
void *hm_get(HashMap *hm, Slice key);

bool hm_key_exists(HashMap *hm, Slice key);

size_t dirty_hash(uint8_t const *key, size_t keylen);

#endif // ASTRA_HASHMAP_H_

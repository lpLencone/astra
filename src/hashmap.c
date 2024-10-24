#include "hashmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib.h"

static Bucket *hm_get_bucket(HashMap *hm, Slice key);

static void hm_bucket_define(Bucket *b, Slice key, void *value, size_t valsz)
{
    if (b->status == BucketStatus_Occupied) {
        free(b->value);
    }
    b->key = key;
    b->value = malloc(valsz);
    memcpy(b->value, value, valsz);
    b->status = BucketStatus_Occupied;
}

void hm_free(HashMap *hm)
{
    for (size_t i = 0; i < hm->capacity; i++) {
        Bucket b = hm->buckets[i];
        if (b.status == BucketStatus_Occupied) {
            free(b.value);
        }
    }
    if (hm->capacity != 0) {
        free(hm->buckets);
    }
    *hm = (HashMap) { 0 };
}

HashMap hm_new(size_t capacity, HashFn hash)
{
    HashMap hm = { .capacity = capacity, .hash = hash };
    hm.buckets = calloc(capacity, sizeof *hm.buckets);
    return hm;
}

void hm_insert(HashMap *hm, Slice key, void *value, size_t valsz)
{
    expect(hm->count < hm->capacity, "HashMap is full");
    Bucket *b = hm_get_bucket(hm, key);
    if (b->status != BucketStatus_Occupied) {
        hm_bucket_define(b, key, value, valsz);
        hm->count++;
    }
}

void hm_update_or_insert(HashMap *hm, Slice key, void *value, size_t valsz)
{
    Bucket *b = hm_get_bucket(hm, key);
    if (b->status != BucketStatus_Occupied) {
        expect(hm->count < hm->capacity, "HashMap is full");
        hm->count++;
    }
    hm_bucket_define(b, key, value, valsz);
}

void *hm_get(HashMap *hm, Slice key)
{
    Bucket *b = hm_get_bucket(hm, key);
    if (b->status == BucketStatus_Occupied) {
        expect(b->value != NULL, "Occupied bucket cannot point to NULL");
        return b->value;
    }
    return NULL;
}

bool hm_key_exists(HashMap *hm, Slice key)
{
    return hm_get(hm, key) != NULL;
}

size_t dirty_hash(uint8_t const *key, size_t keylen)
{
    size_t hash = 0;
    for (size_t i = 0; i < keylen; i++) {
        hash += key[i] * 113;
    }
    return hash;
}

static Bucket *hm_get_bucket(HashMap *hm, Slice key)
{
    size_t hash = hm->hash((uint8_t *) key.data, key.length) % hm->capacity;
    size_t initial_hash = hash;
    // Find bucket with key `key` if it exists, else a free bucket (empty or removed)
    while (hm->buckets[hash].status == BucketStatus_Occupied &&
           !slice_eq(hm->buckets[hash].key, key)) {
        hash = hash + 1 % hm->capacity;
        expect(hash != initial_hash, "Could not find `key` in hashmap");
    }
    return hm->buckets + hash;
}

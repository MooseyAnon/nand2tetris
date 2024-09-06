/* Hash table implementation. */

#include <stdint.h>
#include <string.h>

#include "hash.h"

typedef struct {
    char *key;  // key will be NULL if slot is empty
    void *value;
} Item;


struct HT {
    size_t length;
    size_t _total_size;

    Item *items;
};


// these are somewhat magic numbers for the FNV-1a hashing algorithm
// more info: https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

#define HT_INIT_SIZE 16
#define OOM "-------- OUT OF MEMORY ---------"

// declare static functions
char * _set(Item *, size_t, char *, void *, size_t *);


static void myprint(char *func_name, char *ptr_name, char *message)
{
    printf("(%s)-(%s): %s", func_name, ptr_name, message);
}


int issame(const char w1[], const char w2[]) {

    while (*w1 != '\0' && *w2 != '\0' && *w1 == *w2) {
        w1++;
        w2++;
    }

    if (*w1 != '\0' || *w2 != '\0')
        return 0;

    return 1;
}


static uint64_t hash(const char *key)
{
    uint64_t h = FNV_OFFSET;

    for (const char *p = key; *p; p++) {
        h ^= (uint64_t)(unsigned char)(*p);
        h *= FNV_PRIME;
    }

    return h;
}


void _destroy_item(Item *item)
{
    if (!item->value || !item->key)
        return;

    free((void *)item->key);
    free((void *)item->value);

    // set key to null
    item->key = NULL;
}


void destroy(HT *hash)
{
    if (!hash || !hash->items)
        return;

    Item *tmp = hash->items;
    for (size_t i = 0; i < hash->length; i++, hash->items++)
        _destroy_item(hash->items);

    free(tmp);
    free(hash);
}


static void _resize(HT *ht, size_t new_size)
{
    Item *tmp = malloc(sizeof(*tmp) * new_size);

    if (!tmp) {
        myprint("_resize", "tmp", OOM);
        exit(1);
    }

    // loop through the array and re-hash all entries in new mem block
    // we do ht->_total_size / 2 because we want to go through the previous
    // items array, which is half the size and we dont want to go out of
    // bounds. We do the division here because the new size has already been
    // set externally.
    for (size_t i = 0; i < (ht->_total_size); i++) {
        Item curr = ht->items[i];
        if (curr.key == NULL)
            continue;

        if (!_set(tmp, new_size, curr.key, curr.value, NULL))
            printf("Issue rehashing key - %s\n", curr.key);
    }

    free(ht->items);

    ht->items = tmp;
    ht->_total_size = new_size;
}


static void resize(HT *ht)
{
    if (ht->length / (double)ht->_total_size >= 0.75) {
        // double table size
        _resize(ht, ht->_total_size * 2);
    }
}


HT * create(void)
{
    // alloc hash table
    HT *hash_table = malloc(sizeof(*hash_table));
    if (!hash_table) {
        myprint("create", "hash_table", "-------- OUT OF MEMORY ---------");
        goto error;
    }

    hash_table->length = 0;
    hash_table->_total_size = HT_INIT_SIZE;

    // create initial block of memory
    Item *container = calloc(sizeof(*container), hash_table->_total_size);
    if (!container)
        goto container_calloc_error;

    hash_table->items = container;

    return hash_table;

container_calloc_error:
    // free hash_table to have no dangling pointer
    free(hash_table);
    myprint("create", "container", "-------- OUT OF MEMORY ---------");


error:
    return NULL;
}


void * get(HT *ht, const char *key)
{
    uint64_t _hash = hash(key);
    // AND hash with capacity-1 to ensure it's within entries array.
    // this is essential same as hash % _total_size
    size_t idx = (size_t)(_hash & (uint64_t)(ht->_total_size - 1));

    void *result = NULL;
    while (ht->items[idx].key != NULL) {
        if (issame(ht->items[idx].key, key)) {
            // we have found the key
            result = ht->items[idx].value;
            break;
        }

        // probe till we find space
        idx++;
        if (idx >= ht->_total_size)
            // wrap around
            idx = 0;
    }

    return result;
}


char * _set(Item *items, size_t total_size, char *key, void *value, size_t *len)
{
    uint64_t _hash = hash(key);
    // AND hash with capacity-1 to ensure it's within entries array.
    // this is essential same as hash % _total_size
    size_t idx = (size_t)(_hash & (uint64_t)(total_size - 1));

    char *result = NULL;
    // add value to table, probe if needed
    while (items[idx].key != NULL) {
        // if key already exists, update value
        if (issame(items[idx].key, key)) {
            items[idx].value = value;
            result = items[idx].key;
            break;
        }

        // probe
        idx++;
        if (idx >= total_size)
            // wrap around
            idx = 0;
    }

    // if the key doesn't exist, set it here
    if (result == NULL) {
        // copy key
        result = strdup(key);
        if (!result)
            return NULL;

        if (len != NULL)
            // increment length
            (*len)++;

        items[idx].key = (char *)result;
        items[idx].value = value;
    }

    return result;
}


const char * set(HT *ht, char *key, void *value)
{
    if (value == NULL)
        return NULL;

    // resize, if needed
    resize(ht);

    // const char *result = _set(ht->items, ht->_total_size, key, value, &ht->length);

    // if (result)
    //     ht->length++;

    return _set(ht->items, ht->_total_size, key, value, &ht->length);
}


size_t length(HT *ht)
{
    return ht->length;
}


HTI iterator(HT *ht)
{
    HTI iter;

    iter._table = ht;
    iter.idx = 0;

    return iter;
}


bool next(HTI *iter)
{
    HT *table = iter->_table;

    bool result = false;
    while (iter->idx < table->_total_size) {
        size_t idx = iter->idx;
        iter->idx++;

        Item curr = table->items[idx];
        if (curr.key) {
            // found a non-empty slot in table
            iter->key = curr.key;
            iter->value = curr.value;

            result = true;
            break;
        }
    }

    return result;
}

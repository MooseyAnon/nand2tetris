/* Hash Table */

#ifndef HASH
#define HASH

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


typedef struct HT HT;

typedef struct {
    char *key;     // entry key, used for lockup
    void *value;   // value associated with key

    HT * _table;   // reference to the hash table
    size_t idx;    // current index into ht._entries
} HTI;


HT * create(void);
void * get(HT *, const char *);
const char * set(HT *, char *, void *);
size_t length(HT *);

HTI iterator(HT *);
// Move iterator to next item in hash table, update iterator's key
// and value to current item, and return true. If there are no more
// items, return false. Don't call ht_set during iteration.
bool next(HTI *);

void destroy(HT *);

#endif

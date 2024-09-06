#include <assert.h>

#include "hash.h"


int ____issame(const char w1[], const char w2[]) {

    while (*w1 != '\0' && *w2 != '\0' && *w1 == *w2) {
        w1++;
        w2++;
    }

    if (*w1 != '\0' || *w2 != '\0')
        return 0;

    return 1;
}


HT * prefill_hash()
{
    HT *map = create();

    assert(set(map, "null", "000") != NULL);
    assert(set(map, "M", "001") != NULL);
    assert(set(map, "D", "010") != NULL);
    assert(set(map, "MD", "011") != NULL);
    assert(set(map, "A", "100") != NULL);
    assert(set(map, "AM", "101") != NULL);
    assert(set(map, "AD", "110") != NULL);
    assert(set(map, "AMD", "111") != NULL);

    assert(length(map) == 8);
    return map;   
}

void test_create()
{
    HT *table = create();
    assert(length(table) == 0);

    destroy(table);
}


void test_set()
{
    HT *table = create();

    char *key = "test-key";
    int value = 100;

    const char *ret = set(table, key, &value);

    assert(ret != NULL);
    assert(length(table) == 1);
    assert(____issame(key, ret));
    printf("%s\n", ret);

}


void test_get()
{
    HT *table = create();

    char *key = "test-key";
    int value = 100;

    const char *ret = set(table, key, &value);

    assert(ret != NULL);
    assert(length(table) == 1);

    int result = *(int *)get(table, "test-key");

    assert(result == value);
}


void test_nonexisting_key()
{
    HT *table = create();
    assert(get(table, "test-key") == NULL);
}


void test_overwrite()
{
    HT *table = create();

    char *key = "test-key";
    int value = 100;

    assert(set(table, key, &value) != NULL);

    int result = *(int *)get(table, "test-key");
    assert(result == 100);

    // overwrite the keys value
    value = 300;

    assert(set(table, key, &value) != NULL);
    assert(length(table) == 1);

    // ensure upadated result is correct
    assert(*(int *)get(table, "test-key") == 300);

}


void test_iter()
{
    HT *map = prefill_hash();
    HTI it = iterator(map);

    // next(&it);
    // printf("KEY: %s, VALUE: %s\n", it.key, (char *)it.value);

    // next(&it);
    // printf("KEY: %s, VALUE: %s\n", it.key, (char *)it.value);


    while (next(&it) != false)
        printf("KEY: %s, VALUE: %s\n", it.key, (char *)it.value);
}


void tests()
{
    test_create();
    test_set();
    test_get();
    test_nonexisting_key();
    test_overwrite();
    test_iter();
}


int main()
{
    tests();
    printf("----------- HASH TESTS PASS --------------\n");
    return 0;
}

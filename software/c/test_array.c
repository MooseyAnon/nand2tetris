/* Array tests. */
#include <assert.h>

#include "array.h"

int issame(const char w1[], const char w2[]) {

    while (*w1 != '\0' && *w2 != '\0' && *w1 == *w2) {
        w1++;
        w2++;
    }

    if (*w1 != '\0' || *w2 != '\0')
        return 0;

    return 1;
}


/* test creating an integer array */
void newitem_int(Item *it, void *data)
{
    int *n = calloc(sizeof(*n), 1);
    *n = *(int*)data;
    it->data = n;

    it->c = *(int*)data + 'a';
}


/* test creating an array of simple strings */
void newitem_char(Item *it, void *data)
{
    it->data = data;
}


void test_newarr()
{
    Array *n = newarr(newitem_int);
    assert(n->length == 0);
    assert(n->_total_size == 16);

    for (size_t i = 0; i < n->_total_size; i++) {
        Item item = n->items[i];
        assert(item.data == NULL);
        // printf("nothing at idx '%d'\n", (int)i);
    }

    freearray(n);
}


void test_push__basic()
{
    Array *n = newarr(newitem_int);

    assert(n->length == 0);
    assert(n->_total_size == 16);

    int test = 100;
    push(n, &test);

    assert(n->length == 1);
    assert(&n->items[0] != NULL);
    assert(n->items[0].data != NULL);

    for (size_t i = 1; i < n->_total_size; i++) {
        Item item = n->items[i];
        assert(item.data == NULL);
    }

    freearray(n);
}


void test_push__basic2()
{
    Array *n = newarr(newitem_int);
    assert(n->length == 0);
    assert(n->_total_size == 16);

    for (int i = 0; i < 8; i++)
        push(n, &i);

    assert(n->length == 8);

    for (size_t i = 0; i < n->length; i++) {
        Item item = n->items[i];
        assert(item.data != NULL);
        assert(*(int *)item.data == ((int)i));
    }

    freearray(n);
}


void test_push__resize()
{
    Array *n = newarr(newitem_int);
    assert(n->length == 0);
    assert(n->_total_size == 16);

    for (int i = 100; i < 115; i++)
        push(n, &i);

    assert(n->length == 15);
    assert(n->_total_size == 32);
    assert(&n->items[0] != NULL);
    assert(n->items[0].data != NULL);

    for (size_t i = 0; i < n->length; i++) {
        Item item = n->items[i];

        assert(item.data != NULL);
        assert(*(int *)item.data == ((int)i + 100));
    }

    freearray(n);
}


void test_push()
{
    Array *n = newarr(newitem_int);

    for (int i = 1; i < 1000; i++)
        push(n, &i);

    // start tests, use a big number to ensure resize works
    assert(n->length == 999);
    assert(n->_total_size == 2048);

    Array *tmp = n;
    for (size_t i = 0; i < n->length; i++, tmp->items++) {
        Item item = *tmp->items;
        assert(*(int *)item.data == (int)(i + 1));
    }

    // ensure pointer is nulled after cleanup
    freearray(n);
    assert(n->items == NULL);
}


void test_pop__basic()
{
    Array *n = newarr(newitem_int);

    for (int i = 0; i < 9; i++)
        push(n, &i);

    for (size_t i = 0; i < 9; i++) {
        Item res = pop(n);
        if (res.data != NULL)
            assert(*(int*)res.data == (int)(8 - i));
    }

    assert(n->length == 0);
    assert(n->_total_size == 16);

    // make sure we return NULL on empty pops
    for (size_t i = 0; i < 10; i++)
        assert(!pop(n).data);

    // ensure pointer is nulled after cleanup
    freearray(n);
    assert(n->items == NULL);
}


void test_pop__overwrite()
{

    Array *n = newarr(newitem_int);

    for (int i = 1; i < 21; i++)
        push(n, &i);

    assert(n->length == 20);
    assert(n->_total_size == 32);

    Array *tmp = n;
    for (size_t i = 0; i < n->length; i++, tmp->items++) {
        Item item = *tmp->items;
        assert(*(int *)item.data == (int)(i + 1));
    }

    // pop all items
    for (size_t i = 0; i < 21; i++)
        pop(n);

    // write 10 new numbers from a different range
    for (int i = 100; i < 110; i++)
        push(n, &i);

    Array *tmp2 = n;
    for (size_t i = 0; i < n->length; i++, tmp2->items++) {
        Item item = *tmp2->items;
        assert(*(int *)item.data == (int)(i + 100));
    }

    // check we have correct length and size
    assert(n->length == 10);
    assert(n->_total_size == 32);

    // ensure pointer is nulled after cleanup
    freearray(n);
    assert(n->items == NULL);
}


void test_str_array()
{
    char *test_strs[] = {"This", "is", "a", "test", "array"};

    Array *n = newarr(newitem_char);

    for (int i = 0; i < 5; i++)
        push(n, test_strs[i]);

    assert(n->length == 5);

    for (int i = 4; i >= 0; i--)
        // printf("%s\n", (char *)pop(n).data);
        assert(issame((char *)pop(n).data, test_strs[i]));
}


void test_iterator__int()
{
    Array *n = newarr(newitem_int);

    for (int i = 1; i < 1000; i++)
        push(n, &i);

    // start tests, use a big number to ensure resize works
    assert(n->length == 999);
    assert(n->_total_size == 2048);

    int i = 1;  // counter to assert against in tests
    AIT tmp = iter(n);
    while (array_next(&tmp))
        assert(*(int *)tmp.data == i++);

    // ensure pointer is nulled after cleanup
    freearray(n);
    assert(n->items == NULL);
}


void test_iterator__char()
{
    char *test_strs[] = {"This", "is", "a", "test", "array"};
    Array *n = newarr(newitem_char);

    for (int i = 0; i < 5; i++)
        push(n, test_strs[i]);

    int i = 0;  // counter to assert against in tests
    AIT tmp = iter(n);
    while (array_next(&tmp))
        assert(issame((char *)tmp.data, test_strs[i++]));

    // ensure pointer is nulled after cleanup
    freearray(n);
    assert(n->items == NULL);


}


void test_pop() {
    test_pop__basic();
    test_pop__overwrite();
}


void test_iterator()
{
    test_iterator__int();
    test_iterator__char();
}


void tests() {
    test_newarr();
    test_push__basic();
    test_push__basic2();
    test_push__resize();
    test_push();
    test_pop();
    test_str_array();
    test_iterator();
}


int main()
{
    tests();

    printf("----- ARRAY TESTS PASS ------\n");
    return 0;
}

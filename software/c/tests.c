/* String tests. */
#include <assert.h>
#include <stdio.h>

#include "mystring.h"


int issame(const char w1[], const char w2[])
{
    while (*w1 != '\0' && *w2 != '\0' && *w1 == *w2) {
        w1++;
        w2++;
    }

    if (*w1 != '\0' || *w2 != '\0')
        return 0;

    return 1;
}


void test_issame()
{
    assert(issame("hello", "hello"));
    assert(issame("hello world!!", "hello world!!"));
    assert(!issame("", "hello"));
    assert(!issame("hello", ""));
    assert(!issame("hello ", "hello"));
    assert(!issame("hello", "bello"));
    assert(!issame("hello", "hella"));
    assert(!issame("hello", "hell"));
    assert(!issame("hell", "hello"));
    assert(!issame("aello", "hello"));
}


void test_newstr_length()
{
    assert(newstr("hello").length == 5);
    assert(!(newstr("hello").length == 6));
    assert(!(newstr("hello").length == 4));
    assert(newstr("world").length == 5);
    assert(newstr("hello world!!").length == 13);
    assert(newstr("          hello world").length == 21);
}


void test_newstr_actual_string()
{
    assert(issame(newstr("hello").s, "hello"));
    assert(issame(newstr("This is a much longer word").s, "This is a much longer word"));
    assert(!issame(newstr("hello ").s, "hello"));
    assert(!issame(newstr("hello").s, "bello"));
    assert(!issame(newstr("hhello").s, "hello"));
    assert(!issame(newstr(" hello").s, "hello"));

    // empty string
    assert(issame(newstr("").s, ""));    
}


void test_newstr()
{
    test_newstr_length();
    test_newstr_actual_string();
}


void test_concat__basic()
{
    String w1, w2;

    w1 = newstr("this is ");
    w2 = newstr("a concatenated string");
    assert(issame(concat(w1, w2).s, "this is a concatenated string"));

    w1 = newstr("");
    w2 = newstr("a concatenated string");
    assert(issame(concat(w1, w2).s, "a concatenated string"));

    w1 = newstr("this is ");
    w2 = newstr("a concatenated string");
    assert(!issame(concat(w1, w2).s, "this is a concatenated"));
}


void test_concatRaw__basic()
{
    String w1 = newstr("hello world");
    size_t len = w1.length;

    w1 = concatRaw(w1, "!!");

    assert(w1.length == (len + 2));
    assert(issame(w1.s, "hello world!!"));
}


void test_concatRaw()
{
    char *t[] = {"a", "b", "c"};

    String w1 = newstr("");

    for (int i = 0; i < 3; i++)
        w1 = concatRaw(w1, t[i]);

    assert(w1.length == 3);
    assert(issame(w1.s, "abc"));
}


void test_concatChar__basic()
{
    char *t[] = {"a", "b", "c"};

    String w1 = newstr("");

    for (int i = 0; i < 3; i++)
        w1 = concatChar(w1, t[i]);

    assert(w1.length == 3);
    assert(issame(w1.s, "abc"));
}


void test_concatChar()
{
    char *t[] = {"w", "o", "r", "l", "d"};

    String w1 = newstr("hello ");

    for (int i = 0; i < 5; i++)
        w1 = concatChar(w1, t[i]);

    assert(w1.length == 11);
    assert(issame(w1.s, "hello world"));
}


void test_concatChar__stress()
{
    String w1 = newstr("");

    for (int i = 0; i < 1000; i++)
        w1 = concatChar(w1, "a");

    assert(w1.length == 1000);
}


void test_concat()
{
    test_concat__basic();
    test_concatRaw__basic();
    test_concatRaw();
    test_concatChar__basic();
    test_concatChar();
    test_concatChar__stress();
}


void test_startswith()
{
    String w1 = newstr("// this is a comment");
    assert(startswith(w1, "//"));
    assert(!startswith(w1, " /"));

    String w2 = newstr("hello world!");
    assert(startswith(w2, "h"));
    assert(startswith(w2, "hell"));
    assert(startswith(w2, "hello"));
    assert(!startswith(w2, "bello"));
    assert(!startswith(w2, "helo"));

    String w3 = newstr("");
    assert(startswith(w3, ""));
    assert(!startswith(w3, " "));

    freestr(w1);
    freestr(w2);
    freestr(w3);
}


void test_charat()
{
    String w1 = newstr("// this is a comment");
    assert(charat(w1, (size_t)0) == '/');
    assert(charat(w1, (size_t)9) == 's');
    assert(charat(w1, (size_t)10) == ' ');
    assert(charat(w1, w1.length - 1) == 't');
    // this successfully exists
    // assert(charat(w1, w1.length) == 't');
}


void tests()
{
    test_issame();
    test_newstr();
    test_concat();
    test_startswith();
    test_charat();
}


int main()
{
    tests();
    printf("----- STRING TESTS PASS ------\n");
    return 0;
}

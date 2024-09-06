/* An attempt at as C string library */

#include <stdio.h>

#include "mystring.h"


#define OOM "-------- OUT OF MEMORY ---------"


/* exit with an error message */
static void exit_with_message(char *func_name, char *ptr_name, char *message)
{
    printf("(%s)-(%s): %s\n", func_name, ptr_name, message);
    exit(1);
}


void freestr(String w)
{
    if (!w.s)
        return;

    free(w.s);

    if (w.s != NULL)
        w.s = NULL;
}


String concat(const String w1, const String w2)
{
    String newString;

    char *buff = malloc(sizeof(*buff) * (w1.length + w2.length + 1));

    if (!buff)
        // 'catch' oom error
        goto oom_error;

    size_t i = 0;
    size_t j = 0;

    while (i < w1.length) {
        buff[i] = w1.s[i];
        i++;
    }

    while (j < w2.length) {
        buff[i++] = w2.s[j++];
    }

    // set final character
    buff[i] = '\0';

    newString.s = buff;
    newString.length = w1.length + w2.length;

    // free strings in w1, w2
    freestr(w1);
    freestr(w2);

    return newString;

oom_error:
    // free strings before exiting
    freestr(w1);
    freestr(w2);
    exit_with_message("concat", "buff", OOM);
    // return empty string for the compiler, does not matter as we will exit
    // anyways
    return (String){0, NULL};
}


String concatRaw(const String w1, char w2[])
{
    // turn w2 to a proper string, then concatenate
    String n = newstr(w2);
    return concat(w1, n);
}


String concatChar(const String w1, char w2[])
{
    String n;

    char *buff = calloc(sizeof(*buff), (w1.length + 2));
    if (!buff)
        // 'catch' oom error
        goto oom_error;

    for (size_t i = 0; i < w1.length; i++)
        buff[i] = w1.s[i];

    // string is 0 indexed so buff[w1.length] is one past the last buff element
    buff[w1.length] = w2[0];
    buff[w1.length + 1] = '\0';

    n.s = buff;
    n.length = w1.length + 1;

    freestr(w1);

    return n;

oom_error:
    // free w1 before exiting
    freestr(w1);
    exit_with_message("concatChar", "buff", OOM);
    // return empty string for the compiler, does not matter as we will exit anyways
    return (String){0, NULL};
}


int startswith(const String haystack, char n[])
{
    // we create the needle string in here so we can free it after the
    // comparison
    String needle = newstr(n);

    int result = 0;

    if (needle.length <= haystack.length) {
        // we need to create this tmp variable as we'll be moving through the
        // string using pointers and in order to successfully free the string the
        // pointer variable needs to point to the beginning, otherwise it
        // causes a memory free error.
        char *tmp = needle.s;
        for (size_t i = 0; i < needle.length; i++, tmp++) {
            if (*tmp != haystack.s[i])
                break;
        }

        // if we got to the end of the needle, result == 1, else 0
        result = (*tmp == '\0');
    }

    // free the string contents of the needle
    freestr(needle);

    return result;
}


char charat(const String str, size_t index)
{
    // off by one error means last item is (s->length - 1)
    if (index >= str.length)
        exit_with_message("charat", "None", "Out of bounds error");

    char val = str.s[index];
    return val;
}


String newstr(char word[])
{
    String newString;

    size_t length = 0;
    while (word[length] != '\0')
        length++;

    // using *buff in the sizeof function 'locks' together the declaration and
    // type information i.e. if buff type changes, the malloc does not need to
    // change as well. This gives less room for errors.
    // more info: https://stackoverflow.com/a/605858
    char *buff = malloc(sizeof(*buff) * length + 1);

    if (!buff)
        exit_with_message("newstr", "buff", OOM);

    size_t i = 0;
    // copy word to buffer
    while ((buff[i] = *word++) != '\0')
        i++;

    buff[i] = '\0';
    newString.s = buff;
    newString.length = length;

    return newString;
}

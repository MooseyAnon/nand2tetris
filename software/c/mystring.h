/* String library headers */

#ifndef MYSTRING
#define MYSTRING

#include <stdlib.h>


/* simple struct for managing strings */
typedef struct {
    size_t length;
    char *s;
} String;


/* create new string */
String newstr(char *);

/* get char at the given index - if out of bounds, log error and exit */
char charat(const String, size_t idx);

/* concatenate two strings */
String concat(const String, const String);

/* concatenate a string with a character array (i.e. c string) */
String concatRaw(const String, char *);

/*
concatenate a single character with pre-existing string. This is useful
when building a string via loop during string processing task. Single character
strings are not guarenteed to have a null terminating character which makes
creating a new string via concatRaw impossible as newstr ties to get the length
of a null terminated character array.

Another case is when reading a file. A UTF encoded file can have garbage
characters trailing the character we want e.g. from `getc`. Using concatChar
ensures that regardless of garbage, we only get the first string of any
character array (typically this is the character we care about).

This function is 'loose' as it accepts a character array but only takes the
first character. This is to mitigate the above issues of garbage trailing the
character we care about.
*/
String concatChar(const String, char *);

/*
prefix matching - returns 1 if there is a prefix match, else return 0

prefix can one or more characters i.e. 'hello' is a prefix of 'hello world'.
*/
int startswith(const String, char *prefix);

void freestr(String);

#endif

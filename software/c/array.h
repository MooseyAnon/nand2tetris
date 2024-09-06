#ifndef ARRAY
#define ARRAY

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


// abstraction around elements of the array
typedef struct {
    char c;
    void *data;  // designed to hold any type of data
} Item;

/*
A custom callback used during append's to correctly create a new
array item. This is because users are better placed knowing the exact
types of the incoming data into the array, and it is better left to the
user to define the initialization of the items.

NOTE: is memory is alloc'ed, it is up to the user to free.

@param Item *item: A pointer to an item struct that will hold the new data
    i.e. next free slot in the array.
@pram void *data: Incoming data, correctly casting the void ptr, allocating
    any required memory etc ate all left to the user.
*/
typedef void (*NewItemHandler)(Item *, void *);


typedef struct {
    // holds length of the array i.e. array.length, but also acts as
    // the indexing mechanism for the next free location in the array.
    // array->items[array.length] == next free location (off-by-one).
    size_t length;
    size_t _total_size;       // total allocated memory for the array
    NewItemHandler callback;  // custom function for creating a new item
    Item *items;              // pointer to first item in the array
} Array;


/* holds the result of the iterator */
typedef struct {
    size_t curr_idx;
    Array *arr;
    void *data;
} AIT;                        // array iterator


Array * newarr(NewItemHandler callback);

void push(Array *, void *);   // add new value to the end of of the array
Item pop(Array *);            // Remove last value from the array

AIT iter(Array *);

bool array_next(AIT *);

void freearray(Array *);

#endif

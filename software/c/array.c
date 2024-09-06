/* My attempt at a dynamic array. */

#include "array.h"


#define ARRAY_INIT_SIZE 16
#define OOM "-------- OUT OF MEMORY ---------\n"


/* Logging memory allocation errors */
static void myprint(char *func_name, char *ptr_name, char *message)
{
    printf("(%s)-(%s): %s", func_name, ptr_name, message);
}


static int isempty(Array *arr)
{
    return arr->length <= 0;
}


void debug(char * name, Item * arr, size_t len)
{
    Item * tmp = arr;
    for (size_t i = 0; i < len; i++, tmp++)
        printf(
            "DEBUG (%s): value at idx '%lu' -> '%d'\n",
            name, i, *(int*)tmp->data);
}


void copy(Item * src, Item * dest, size_t len)
{
    for (size_t i = 0; i < len; i++) {
        // printf("copying over %d\n", *(int *)src[i].data);
        dest[i] = src[i];
    }
}


void freearray(Array *arr)
{
    if (!arr->items)
        return;

    free(arr);

    if (arr->items != NULL)
        arr->items = NULL;
}


void _resize(Array *arr)
{
    Item *tmp = calloc(sizeof(*tmp), arr->_total_size);
    if (!tmp)
        goto error;

    // copy existing items to resized memory block
    copy(arr->items, tmp, arr->length);

    free(arr->items);
    arr->items = tmp;

    return;

error:
    myprint("_resize", "tmp", OOM);
    freearray(arr);
    exit(1);
}


static void resize(Array *arr)
{
    // check if we actually need to resize
    if ((arr->length / (double)arr->_total_size) < 0.75)
        return;

    if ((arr->_total_size * 2) < arr->_total_size) {
        // overflow
        myprint("resize", "_total_size", "overflow issue");
        return;
    }

    // double array size: this is probs not very efficient!
    arr->_total_size *= 2;
    _resize(arr);

}


Array * newarr(NewItemHandler callback)
{
    Array *arr = malloc(sizeof(*arr));
    if (!arr) {
        myprint("create", "arr", OOM);
        goto error;
    }

    arr->length = 0;
    arr->_total_size = ARRAY_INIT_SIZE;
    arr->callback = callback;

    Item *container = calloc(sizeof(*container), arr->_total_size);
    if (!container)
        goto container_calloc_error;

    arr->items = container;

    return arr;

container_calloc_error:
    // free array to avoid dangling pointers
    free(arr);
    myprint("create", "container", OOM);

error:
    return NULL;
}


void push(Array *arr, void *value)
{
    if (value == NULL)
        return;

    // check if we need to resize
    resize(arr);

    size_t idx = arr->length;
    arr->callback(&arr->items[idx], value);
    arr->length++;
}


Item pop(Array *arr)
{
    // fake item to return if array is empty
    Item result = { '#', NULL };

    if (!isempty(arr)) {
        result = arr->items[arr->length - 1];
        arr->length--;
    }

    return result;
}


AIT iter(Array *arr)
{
    AIT n;

    n.arr = arr;
    n.curr_idx = 0;
    n.data = NULL;

    return n;
}


bool array_next(AIT *iter)
{
    Array *arr = iter->arr;

    // prevent going out of boundary
    if (iter->curr_idx >= arr->length)
        return false;

    iter->data = arr->items[iter->curr_idx].data;
    iter->curr_idx++;  // increment index for next iteration

    return true;
}

#include "list.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include "vector.h"

const size_t LIST_RESIZE_FACTOR = 2;


// Definition of the list struct.
typedef struct list
{
    size_t size;
    size_t capacity;
    void **arr;
    free_func_t freer;
} list_t;


// Allocates memory for a new list with space for the given number of elements.
list_t *list_init(size_t initial_size, free_func_t freer)
{
    list_t *list = malloc(sizeof(list_t));
    assert(list != NULL);

    list -> size = 0;
    list -> capacity = initial_size;
    // Note that free_func_t is defined in list.h.
    list -> freer = freer;

    // The array is just storing pointers to vector_t objects, so allocate
    // memory for storing an integer.
    list -> arr = malloc(initial_size * sizeof(void*));
    assert(list -> arr != NULL);

    return list;
}


// Releases the memory allocated for a list.
void list_free(list_t *list)
{
    // Free every vector in the backing array.
    for (size_t i = 0; i < list->size; i++)
    {
        assert(list -> freer != NULL);
        assert(list -> arr[i] != NULL);
        list -> freer(list -> arr[i]);
    }

    free(list -> arr);
    free(list);
}


// Gets the element at a given index in a list.
void *list_get(list_t *list, size_t index)
{
    assert(index < list -> size);

    void *elem = list -> arr[index];

    return elem;
}


// Replaces an element at the specified index.
void list_replace(list_t *list, size_t index, void *elem)
{
    assert(index < list -> size);

    void *old = list -> arr[index];
    list -> arr[index] = elem;

    free(old);
}


// Appends an element to the end of a list, resizing if needed.
void list_add(list_t *list, void *elem)
{
    assert(elem != NULL);
    assert(list != NULL);

    // Resize the list if capacity is reached.
    if (list -> size == list -> capacity)
    {
        list -> arr = realloc(list -> arr, 2 * list -> capacity * sizeof(void*));
        list -> capacity *= 2;
    }

    list -> arr[list -> size] = elem;
    list -> size ++;
}



// Removes the element at the specifiec index of a list and returns it.
void *list_remove(list_t *list, size_t index)
{
    assert(list -> size != 0 && index < list -> size);
    void *elem = list_get(list, index);
    list -> size --;

    if (index != list->size)
    {
        for (size_t i = index; i < list -> size; i ++)
        {
            list->arr[i] = list->arr[i + 1];
        }
    }

    list->arr[list->size] = NULL;
    return elem;
}



// Adds the contents of one list to another list, and DOES NOT FREE THE
// POINTERS IN THE SECOND LIST.
void list_extend(list_t* list, list_t* another_list)
{
    for (size_t i = 0; i < list_size(another_list); i ++)
    {
        void* elem = list_get(another_list, i);
        list_add(list, elem);
    }
    
    free(list_arr(another_list));
    free(another_list);
}

// Checks if two lists of vectors are equal
bool list_equal(list_t* list1, list_t* list2)
{
    // First checks if sizes are equal
    if (list1 -> size != list2 -> size) {return false;}
    // Then checks if lists are perfect copies of each other
    for (size_t i = 0; i < list1 -> size; i++)
    {
        if (vec_equal(*(vector_t*) list1 -> arr[i], *(vector_t*) list2 -> arr[i]))
        {
            return false;
        }
    }
    return true;
}


// Accessor functions -----------------------------------------------

// Gets the pointer to the backing array.
void **list_arr(list_t *list){
    return list -> arr;
}


// Gets the size of a list (the number of occupied elements).
size_t list_size(list_t *list){
    return list -> size;
}

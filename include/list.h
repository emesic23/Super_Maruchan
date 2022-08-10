#ifndef __LIST_H__
#define __LIST_H__

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>


// A growable array of pointers.
typedef struct list list_t;


// A function that can be called on list elements to release their resources.
typedef void (*free_func_t)(void *);


// Allocates memory for a new list with space for the given number of elements.
list_t *list_init(size_t initial_size, free_func_t freer);


// Releases the memory allocated for a list.
void list_free(list_t *list);


// Replaces on element at specified index.
void list_replace(list_t *list, size_t index, void *elem);


// Gets the size of a list (the number of occupied elements). Note that this is NOT the list's capacity.
size_t list_size(list_t *list);


// Gets the element at a given index in a list. Asserts that the index is valid, given the list's current size.
void *list_get(list_t *list, size_t index);


// Removes the element at a given index in a list and returns it, moving all subsequent elements towards the start of the list.
void *list_remove(list_t *list, size_t index);


// Appends an element to the end of a list.
// If the list is filled to capacity, resizes the list to fit more elements and asserts that the resize succeeded.
void list_add(list_t *list, void *elem);


// Adds the contents of one list to another list, and DOES NOT FREE THE
// POINTERS IN THE SECOND LIST.
void list_extend(list_t* list, list_t* another_list);

// Checks if two lists are equal
bool list_equal(list_t* list1, list_t* list2);


// Accessor functions -----------------------------------------------

// Gets the pointer to the backing array.
void **list_arr(list_t *list);


#endif // #ifndef __LIST_H__

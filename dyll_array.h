// C Data Structures
// (C) Comprosoft 2018 - All Rights Reserved
//
//	dyll_array.c - Header file for Dynamic Linked-List Array (DyLL_Arr)
//
//	  Allows you to add and delete items at will, and the array will automatically resize
#ifndef DYNAMIC_LINKED_LIST_ARRAY_HEADER
#define DYNAMIC_LINKED_LIST_ARRAY_HEADER

#include <stdbool.h>
#include <stddef.h>		//For size_t
#include <stdlib.h>		//For malloc and free

typedef void* pDyLL_Arr_t;

pDyLL_Arr_t new_dyll_array();
void free_dyll_array(pDyLL_Arr_t dyll);

//Add or remove elements from the array (makes a copy, or deletes the copy)
bool dyll_add_element(pDyLL_Arr_t dyll, void* element, size_t el_size);
bool dyll_delete_element(pDyLL_Arr_t dyll, size_t index);


//Add at a specific index in the array (before or after a given index)
bool dyll_add_element_before(pDyLL_Arr_t dyll, size_t index, void* element, size_t el_size);
bool dyll_add_element_after(pDyLL_Arr_t dyll, size_t index, void* element, size_t el_size);



//If len is NULL, then does not return the length of the element
//	Whatever you do, do NOT free the returned pointer (it will mess up the internal array)
const void* dyll_get_element(pDyLL_Arr_t dyll, size_t index, size_t* len);

//Make a copy of the element at the index, then return the newly allocated buffer (which needs to be freed)
void* dyll_copy_element(pDyLL_Arr_t dyll, size_t index, size_t* len);

//Return the buffer at index, then delete the element from the array
void* dyll_flush_element(pDyLL_Arr_t dyll, size_t index, size_t* len);



// Normalizes all of the data, then returns a single buffer containing everything in the array
//  sequentially, one right after the other
//
// The new buffer becomes detached from the array object, and must be freed after use
void* dyll_flush_array(pDyLL_Arr_t dyll, size_t* total_len);


//Get the total number of items in the array (or -1 on error)
size_t dyll_get_count(pDyLL_Arr_t dyll);

//Get the total size (in bytes) of all items in the array (or -1 on error)
size_t dyll_get_size(pDyLL_Arr_t dyll);

#endif

// C Data Structures
// (C) Comprosoft 2018 - All Rights Reserved
//
//	dynamic_array.c - Header for the Dynamic Array data structure
//
#ifndef DYNAMIC_ARRAY_HEADER
#define DYNAMIC_ARRAY_HEADER

#include <stddef.h>	/* For size_t */
#include <stdbool.h>

typedef void *pDynamic_Arr_t;
typedef void (*Free_Func_t)(void*);


pDynamic_Arr_t new_dynamic_array(size_t el_size);
void free_dynamic_array(pDynamic_Arr_t, Free_Func_t func);


bool add_array_element(pDynamic_Arr_t arr, const void* new);
bool delete_array_element(pDynamic_Arr_t arr, size_t index, bool maintainOrder);

//new_arr should be a pointer to an array of element (such as a char*)
//  If new_arr is a pointer to an array of pointers, use add_p (such as char**)
bool add_array_elements(pDynamic_Arr_t arr, const void* new_arr, size_t count);
bool add_array_elements_p(pDynamic_Arr_t arr, const void** new_ptrs, size_t count);

//Where to insert the next value
bool set_array_index(pDynamic_Arr_t arr, size_t index);

void* get_array_element(pDynamic_Arr_t arr, size_t index);

//Resize the pointer
void* flush_dynamic_array(pDynamic_Arr_t arr);

size_t get_array_count(pDynamic_Arr_t arr);

#endif // DYNAMIC_ARRAY_HEADER Included

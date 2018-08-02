// C Data Structures
// (C) Comprosoft 2018 - All Rights Reserved
//
//	dynamic_array.c - Implementation for the Dynamic Array data structure
//
#include "dynamic_array.h"
#include <stdlib.h>
#include <string.h>

//arr should be a pDynamic_Obj_t object
#define ResAddr(arr,index) ((void*) (((char*) (arr)->ptr) + ((index) * (arr)->el_size)))


// Private Dynamic Array object
typedef struct {
    void* ptr;      // Internal string buffer
    size_t el_size; // How big is each element
    size_t index;   // Where to insert the next character
    size_t len;     // Absolute string length (with null-terminator)
    size_t max;     // Biggest index used (DO NOT MESS WITH THIS!!!)
} Dynamic_Obj_t, *pDynamic_Obj_t;


pDynamic_Arr_t new_dynamic_array(size_t el_size) {
    if (el_size == 0) {return NULL;}
    pDynamic_Obj_t arr = malloc(sizeof(Dynamic_Obj_t));

    if (!arr) {return NULL;}

    arr->el_size = el_size;
    arr->index = 0;
    arr->ptr = NULL;

    return (pDynamic_Arr_t) arr;
}


void free_dynamic_array(pDynamic_Arr_t a, Free_Func_t func) {

	pDynamic_Obj_t arr = (pDynamic_Obj_t) a;
    if (!arr) {return;}

    if (func != NULL) {
        size_t i;
        for (i = 0; i < get_array_count(arr); ++i) {
            func(*(void**) get_array_element(arr,i));
        }
    }

    if (arr->ptr != NULL) {free(arr->ptr);}
    free(arr);
}





bool add_array_element(pDynamic_Arr_t a,const void* new) {

	pDynamic_Obj_t arr = (pDynamic_Obj_t) a;
    if (!(arr && new)) {return false;}

    if (arr->ptr == NULL) {
        arr->index = 0;
        arr->max = 0;
        arr->len = 16;  //Initial length
        arr->ptr = malloc(arr->len * arr->el_size);
    }

    if (!arr->ptr) {return false;}

    if (arr->index >= arr->len) {
		void* new_ptr = realloc(arr->ptr, (arr->len+16) * arr->el_size);
		if (!new_ptr) {return false;}

        arr->len+=16;
        arr->ptr = new_ptr;
    }


    memcpy(ResAddr(arr,arr->index),new,arr->el_size);

    arr->index+=1;
    if (arr->index > arr->max) {arr->max = arr->index;}
	return true;
}


bool add_array_elements(pDynamic_Arr_t a, const void* new_arr, size_t count) {

	pDynamic_Obj_t arr = (pDynamic_Obj_t) a;
	if (!(a && new_arr)) {return false;}

    size_t i;
    for (i = 0; i < count; ++i) {
		void* next = (void*) (((char*) new_arr) + (i * arr->el_size));
		if (!add_array_element(arr,next)) {return false;}
    }

	return true;
}

bool add_array_elements_p(pDynamic_Arr_t a, const void** new_ptrs, size_t count) {

    if (!(a && new_ptrs)) {return false;}

    size_t i;
    for (i = 0; i < count; ++i) {
        if (!add_array_element(a,new_ptrs[i])) {return false;}
    }

	return true;
}



bool delete_array_element(pDynamic_Arr_t a, size_t index, bool maintainOrder) {

	pDynamic_Obj_t arr = (pDynamic_Obj_t) a;
	if (!arr) {return false;}
	if (index >= arr->len) {return false;}
	
	arr->max-=1;
	if (maintainOrder) {
		//Move all other elements back
		memmove(ResAddr(arr,index), ResAddr(arr,index+1), index - arr->max);

	} else {
		//Move the last element into the space (does not overlap)
		memcpy(ResAddr(arr,arr->max), ResAddr(arr,index), arr->el_size);
	}

	return true;
}


bool set_array_index(pDynamic_Arr_t a, size_t index) {

	pDynamic_Obj_t arr = (pDynamic_Obj_t) a;
    if (!arr) {return false;}
    if (index >= arr->len) {return false;}
    arr->index = index;

	return true;
}

void* get_array_element(pDynamic_Arr_t a, size_t index) {

	pDynamic_Obj_t arr = (pDynamic_Obj_t) a;
    if (!arr) {return NULL;}
    if (index >= arr->len) {return NULL;}

    return ResAddr(arr,index); 
}

//Return NULL on error or an empty array
void* flush_dynamic_array(pDynamic_Arr_t a) {

	pDynamic_Obj_t arr = (pDynamic_Obj_t) a;
    if (!arr) {return NULL;}

    //Resize array to match the max size
    void* ret = NULL;
    if (arr->max == 0) {if (arr->ptr) {free(arr->ptr);} ret = NULL;}
    else {
		ret = realloc(arr->ptr,arr->max * arr->el_size);
		if (!ret) {return NULL; /* Realloc Failure (not good!) */}
	}

    arr->index = 0;
    arr->max = 0;
    arr->len = 0;
    arr->ptr = NULL;

    return ret;
}

//How many items are in this array???
size_t get_array_count(pDynamic_Arr_t a) {

	pDynamic_Obj_t arr = (pDynamic_Obj_t) a;
    if (!arr) {return 0;}
    if (arr->ptr == NULL) {return 0;}
    return (arr->max);
}

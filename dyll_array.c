// C Data Structures
// (C) Comprosoft 2018 - All Rights Reserved
//
//	dyll_array.c - Implementation for Dynamic Linked-List Array (DyLL_Arr)
//
#include "dyll_array.h"
#include <string.h>

#define INIT_ITEMS	10		//List starts with 10 items every time
#define ITEMS_INC	10		//List adds 10 items for every realloc

#define LL_NULL ((size_t) -1)



//Single item in the doubly-linked list
typedef struct {
	void* data;			//Malloc'd buffer that actually stores the data
	size_t len;			//How long is this segment of data
	size_t next;
	size_t pre;
} DyLL_LL_t, *pDyLL_LL_t;

//The private DyLL object
typedef struct {
	size_t start_item;		// Where does the linked list array start?
	size_t end_item;		// Where does the linked list array end?
	size_t next_item;		// Where is the next free item in the ll array?

	size_t items_inuse;		// Total number of items currently in use
	size_t bytes;			// Number of bytes of data being used (in total)

	size_t items_alloc;		// Total number of items allocated in ll
	pDyLL_LL_t ll;			// Linked list of all entries in this array
} DyLL_Arr_Obj_t, *pDyLL_Arr_Obj_t;


//Private function prototypes:
static size_t dyll_get_index(pDyLL_Arr_Obj_t dyll,size_t index);
static void dyll_free_entry(pDyLL_Arr_Obj_t dyll, size_t entry);
static bool dyll_add_chunk(pDyLL_Arr_Obj_t dyll);
static size_t dyll_next_entry(pDyLL_Arr_Obj_t dyll);
static bool dyll_copy_data(pDyLL_Arr_Obj_t dyll, size_t index, void* data, size_t el_size) ;
static void dyll_free_data(pDyLL_Arr_Obj_t dyll, size_t index);



//--------------------- Private Functions --------------------------------


//Add another chunk of memory to the internal linked-list array
static bool dyll_add_chunk(pDyLL_Arr_Obj_t dyll) {
	void* new = realloc((void*) dyll->ll, dyll->items_alloc + ITEMS_INC);
	if (!new) {return false; /* Realloc should not fail*/ }
	dyll->ll = new;

	//Also update the indexes
	size_t i;
	for (i = dyll->items_alloc; i < dyll->items_alloc + ITEMS_INC; ++i) {
		dyll->ll[i].next = i+1;
	}
	dyll->ll[dyll->items_alloc].pre = LL_NULL;
	dyll->items_alloc += ITEMS_INC;
	return true;
}


//Pop an entry off the free list
static size_t dyll_next_entry(pDyLL_Arr_Obj_t dyll) {

	//When to resize the buffer?
	size_t next = dyll->next_item;
	if (next >= dyll->items_alloc) {
		if (!dyll_add_chunk(dyll)) {return LL_NULL;}
	}

	//Pop this item off the "free" list
	dyll->next_item = dyll->ll[next].next;
	dyll->items_inuse+=1;
	return next;
}

//Add an entry back on to the free list
static void dyll_free_entry(pDyLL_Arr_Obj_t dyll, size_t entry) {
	dyll->ll[entry].next = dyll->next_item;
	dyll->next_item = entry;
	dyll->items_inuse-=1;
}


//Returns LL_NULL on failure
static size_t dyll_get_index(pDyLL_Arr_Obj_t dyll, size_t index) {
	
	//Traverse along the array until the index is found
	size_t idx = dyll->start_item;
	while((idx != LL_NULL) && (index > 0)) {
		idx = dyll->ll[idx].next;
		--index;
	}

	return idx;
}


static bool dyll_copy_data(pDyLL_Arr_Obj_t dyll, size_t index, void* data, size_t el_size) {

	dyll->ll[index].data = malloc(el_size);
	if (!dyll->ll[index].data) {return false; /* Malloc should not fail */ }

	memcpy(dyll->ll[index].data,data,el_size);
	dyll->ll[index].len = el_size;
	dyll->bytes+=el_size; 

	return true;
}

static void dyll_free_data(pDyLL_Arr_Obj_t dyll, size_t idx) {
	if (dyll->ll[idx].data) {free(dyll->ll[idx].data);}
	dyll->bytes-=dyll->ll[idx].len;
}








//--------------------- Public Functions --------------------------------

pDyLL_Arr_t new_dyll_array() {

	pDyLL_Arr_Obj_t dyll = (pDyLL_Arr_Obj_t) calloc(1,sizeof(DyLL_Arr_Obj_t));
	if (!dyll) {return NULL;}

	dyll->ll = (pDyLL_LL_t) malloc(INIT_ITEMS*sizeof(DyLL_LL_t));
	if (!dyll->ll) {free(dyll); return NULL;}

	//Initialize the list to a default free list (of 10 items)
	size_t i;
	dyll->items_alloc = INIT_ITEMS;
	dyll->start_item = LL_NULL;
	dyll->end_item = LL_NULL;
	for (i = 0; i < dyll->items_alloc; ++i) {
		dyll->ll[i].next = i+1;
	}

	return (pDyLL_Arr_t) dyll;
}



void free_dyll_array(pDyLL_Arr_t d) {

	pDyLL_Arr_Obj_t dyll = (pDyLL_Arr_Obj_t) d;
	if (!dyll) {return;}

	if (dyll->ll) {

		//Release all of the internal linked-list buffers:
		size_t i = dyll->start_item;
		while(i != LL_NULL) {
			pDyLL_LL_t ll = (dyll->ll + i);
			if (ll->data) {free(ll->data); ll->data = NULL;}
			i = ll->next;
		}

		free(dyll->ll);
	}

	free(dyll);
}









bool dyll_add_element(pDyLL_Arr_t d, void* element, size_t el_size) {
	
	pDyLL_Arr_Obj_t dyll = (pDyLL_Arr_Obj_t) d;
	if (!dyll) {return false;}

	size_t next = dyll_next_entry(dyll);
	if (next == LL_NULL) {return false;}

	//Copy the data into this item
	if (!dyll_copy_data(dyll,next,element,el_size)) {
		dyll_free_entry(dyll,next);
		return false;
	}

	//Now add to the "inuse" list
	size_t last = dyll->end_item;
	if (last == LL_NULL) {
		//Special case for an empty list
		dyll->start_item = next;
		dyll->ll[next].pre = LL_NULL;
	} else {
		dyll->ll[last].next = next;
		dyll->ll[next].pre = last;
	}
	dyll->ll[next].next = LL_NULL;
	dyll->end_item = next;

	return true;
}


bool dyll_add_element_before(pDyLL_Arr_t d, size_t index, void* element, size_t el_size) {

	pDyLL_Arr_Obj_t dyll = (pDyLL_Arr_Obj_t) d;
	if (!dyll) {return false;}

	size_t idx = dyll_get_index(dyll,index);
	if (idx == LL_NULL) {return false;}

	size_t next = dyll_next_entry(dyll);
	if (next == LL_NULL) {return false;}

	//Copy the data into this item
	if (!dyll_copy_data(dyll,next,element,el_size)) {
		dyll_free_entry(dyll,next);
		return false;
	}

	//Update the linked list	
	if (dyll->ll[idx].pre != LL_NULL) {
		dyll->ll[dyll->ll[idx].pre].next = next;
	} else {
		dyll->start_item = next;
	}

	dyll->ll[next].pre = dyll->ll[idx].pre;
	dyll->ll[next].next = idx;
	dyll->ll[idx].pre = next;

	return true;
}


bool dyll_add_element_after(pDyLL_Arr_t d, size_t index, void* element, size_t el_size) {

	pDyLL_Arr_Obj_t dyll = (pDyLL_Arr_Obj_t) d;
	if (!dyll) {return false;}

	size_t idx = dyll_get_index(dyll,index);
	if (idx == LL_NULL) {return false;}

	size_t next = dyll_next_entry(dyll);
	if (next == LL_NULL) {return false;}

	//Copy the data into this item
	if (!dyll_copy_data(dyll,next,element,el_size)) {
		dyll_free_entry(dyll,next);
		return false;
	}
	
	//Update the linked list
	dyll->ll[next].next = dyll->ll[idx].next;
	dyll->ll[next].pre = idx;
	dyll->ll[idx].next = next;

	if (dyll->ll[next].next != LL_NULL) {
		dyll->ll[dyll->ll[next].next].pre = next;
	} else {
		dyll->end_item = next;
	}

	return true;
}




bool dyll_delete_element(pDyLL_Arr_t d, size_t index) {

	pDyLL_Arr_Obj_t dyll = (pDyLL_Arr_Obj_t) d;
	if (!dyll) {return false;}

	size_t idx = dyll_get_index(dyll,index);
	if (idx == LL_NULL) {return false;}

	//Release any stored data
	dyll_free_data(dyll,idx);

	//Patch up the linked list
	if (dyll->ll[idx].next != LL_NULL) {
		dyll->ll[dyll->ll[idx].next].pre = dyll->ll[idx].pre;
	} else {
		dyll->end_item = dyll->ll[idx].pre;
	}

	if (dyll->ll[idx].pre != LL_NULL) {
		dyll->ll[dyll->ll[idx].pre].next = dyll->ll[idx].next;
	} else {
		dyll->start_item = dyll->ll[idx].next;
	}

	dyll_free_entry(dyll,idx);

	return true;
}



const void* dyll_get_element(pDyLL_Arr_t d, size_t index, size_t* len) {

	pDyLL_Arr_Obj_t dyll = (pDyLL_Arr_Obj_t) d;
	if (!dyll) {return NULL;}

	size_t idx = dyll_get_index(dyll,index);
	if (idx == LL_NULL) {return NULL;}

	void* data = dyll->ll[idx].data;
	if (len != NULL) {*len = dyll->ll[idx].len;}
	return data;
}


void* dyll_copy_element(pDyLL_Arr_t dyll, size_t index, size_t* len) {

	size_t temp_len;
	const void* data = dyll_get_element(dyll,index,&temp_len);
	if (!data) {return NULL;}

	void* new_buf = malloc(temp_len);
	if (!new_buf) {return NULL;}

	//Copy the data
	memcpy(new_buf,data,temp_len);
	if (len != NULL) {*len = temp_len;}
	return new_buf;
}


void* dyll_flush_element(pDyLL_Arr_t d, size_t index, size_t* len) {

	pDyLL_Arr_Obj_t dyll = (pDyLL_Arr_Obj_t) d;
	if (!dyll) {return NULL;}

	size_t idx = dyll_get_index(dyll,index);
	if (idx == LL_NULL) {return NULL;}

	void* temp_buf = dyll->ll[idx].data;
	if (len != NULL) {*len = dyll->ll[idx].len;}
	
	//Delete the data (should NOT fail)
	dyll->ll[idx].data = NULL;	//Don't free the old data
	dyll_delete_element(d,index);	

	return temp_buf;
}






void* dyll_flush_array(pDyLL_Arr_t d, size_t* total_len) {

	pDyLL_Arr_Obj_t dyll = (pDyLL_Arr_Obj_t) d;
	if (!dyll) {return NULL;}

	//Build the new buffer
	size_t total = 0;
	void* new_buf = malloc(dyll->bytes);
	if (!new_buf) {return NULL;}

	//Copy everything out of here
	void* temp_buf = new_buf;
	while(1) {
		size_t temp_len;
		void* buf = dyll_flush_element(d,0, &temp_len);
		if (!buf) {break;}
		
		memcpy(temp_buf,buf,temp_len);
		free(buf);
		total+=temp_len;
		temp_buf = (void*) (((char*) temp_buf)+temp_len);	//Cast to byte array
	}

	if (total_len != NULL) {*total_len = total;}
	return new_buf;
}






size_t dyll_get_count(pDyLL_Arr_t dyll) {
	if (!dyll) {return -1;}
	return ((pDyLL_Arr_Obj_t) dyll)->items_inuse;
}


size_t dyll_get_size(pDyLL_Arr_t dyll) {
	if (!dyll) {return -1;}
	return ((pDyLL_Arr_Obj_t) dyll)->bytes;
}

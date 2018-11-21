// General-purpose XML utility
#include "xml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


//Dynamic array buffer structure type
typedef struct {
	size_t inuse;			// Number of items that I am currently using
	size_t alloc;			// Number of total items available
	void** arr;				// Array of pointers

	size_t* updateLen;		// Public integer length to update
	void*** updateArr;		// Public array pointer to update
} BUFFER_t, *pBUFFER_t;



//Private XML node ([P]rivate [Node], or PNODE)
typedef struct XML_PNODE_t {
	char* name;							// Name of the tag	
	char* value;						// Test inside the tag
	struct XML_PNODE_t* parent;			// Who owns me?
	
	size_t num_attrib;					// Number of attributes I own
	pXML_ATTRIB_t* attrib;				// Array of attributes

	size_t num_children;				// Number of children I own
	struct XML_PNODE_t** children;		// Array of children

//------------Private Variables--------------

	BUFFER_t attrib_buffer;				// For creating a list of attributes
	BUFFER_t child_buffer;				// For creating a list of children
} XML_PNODE_t, *pXML_PNODE_t;






//************************Other Functions***************************

static inline char* dupstr(const char* input) {
	if (!input) {return NULL;}

	char* buf = malloc(strlen(input) + 1);
	if (!buf) {return NULL;}
	strcpy(buf,input);
	return buf;
}


static inline void set_string(char** ptr, char* string, bool copy) {
	if (*ptr) {free(*ptr);}
	if (copy) {*ptr = dupstr(string);}
	else {*ptr = string;}
}




//************************Buffer Functions***************************

//Update the updateXXX variables inside buf
static inline void buffer_update(pBUFFER_t buf) {
	if (buf->updateLen) {*buf->updateLen = buf->inuse;}
	if (buf->updateArr) {*buf->updateArr = buf->arr;}	
}


static inline void copy_buffer_size(pBUFFER_t from, pBUFFER_t to) {
	if (to->arr) {free(to->arr);}
	
	to->inuse = from->inuse;
	to->alloc = from->alloc;
	to->arr = (void**) calloc(to->alloc,sizeof(void*));

	//memcpy(to->arr,from->arr,sizeof(void*) * to->inuse);
	buffer_update(to);
}




#define INITIAL_SIZE  16
#define INCREASE_SIZE 16

static inline void insert_buffer(pBUFFER_t buf, void* ptr) {
	if (!(buf->arr)) {		//Initial Allocation
		buf->alloc = INITIAL_SIZE;
		buf->arr = (void **) calloc(buf->alloc,sizeof(void*));
	}

	buf->arr[buf->inuse++] = ptr;

	if (buf->inuse >= buf->alloc) {	//Resize by X every time
		buf->alloc+=INCREASE_SIZE;
		buf->arr = (void**) realloc(buf->arr,(buf->alloc) * sizeof(void*));
	}

	buffer_update(buf);
}



static inline void free_buffer(pBUFFER_t buf) {
	if (buf->arr) {free(buf->arr); buf->arr = NULL;}
	buf->inuse = buf->alloc = 0;
	buffer_update(buf);
}


//************************XML Attributes***************************

pXML_ATTRIB_t new_xml_attrib() {
	pXML_ATTRIB_t attr = (pXML_ATTRIB_t) calloc(1,sizeof(XML_ATTRIB_t));

	//Default name and value strings
	attr->name = dupstr("NAME");
	attr->value = dupstr("VALUE");
	return attr;
}

pXML_ATTRIB_t duplicate_xml_attrib(pXML_ATTRIB_t attr) {
	pXML_ATTRIB_t new = new_xml_attrib();

	if (attr->name) {xml_attrib_set_name(new,attr->name,true);}
	if (attr->value) {xml_attrib_set_value(new,attr->value,true);}
	return new;
}

void free_xml_attrib(pXML_ATTRIB_t attr) {
	if (attr->name)  {free(attr->name);}
	if (attr->value) {free(attr->value);}
	free(attr);
}

void xml_attrib_set_name(pXML_ATTRIB_t attr, char* name, bool copy) {
	set_string(&attr->name,name,copy);
}

void xml_attrib_set_value(pXML_ATTRIB_t attr, char* value, bool copy) {
	set_string(&attr->value,value,copy);
}






//************************XML Nodes***************************

pXML_NODE_t new_xml_node() {
	pXML_PNODE_t node = calloc(1,sizeof(XML_PNODE_t));

	//Default Values
	node->name = dupstr("NAME");
	node->value = dupstr("VALUE");

	//Update Buffer Pointers
	node->attrib_buffer.updateLen = &node->num_attrib;
	node->attrib_buffer.updateArr = (void***) &node->attrib;
	node->child_buffer.updateLen = &node->num_children;
	node->child_buffer.updateArr = (void***) &node->children;

	return (pXML_NODE_t) node;
}


pXML_NODE_t duplicate_xml_node(pXML_NODE_t n) {

	pXML_PNODE_t node = (pXML_PNODE_t) n;
	pXML_PNODE_t new = (pXML_PNODE_t) new_xml_node();	

	if (node->name) {xml_set_name((pXML_NODE_t) new,node->name,true);}
	if (node->value) {xml_set_value((pXML_NODE_t) new,node->value,true);}

	//Note: Copy buffer updates num_attrib, attrib, num_children, and children
	copy_buffer_size(&node->attrib_buffer,&new->attrib_buffer);
	copy_buffer_size(&node->child_buffer,&new->child_buffer);

	//Copy attrbutes
	size_t i;
	for (i = 0; i < node->num_attrib; ++i) {
		new->attrib[i] = duplicate_xml_attrib(node->attrib[i]);
	}

	//Copy children
	for (i = 0; i < node->num_children; ++i) {
		new->children[i] = (pXML_PNODE_t) duplicate_xml_node((pXML_NODE_t) node->children[i]);
	}

	return (pXML_NODE_t) new;
}




//Does this recursively
void free_xml_node(pXML_NODE_t n) {
	size_t i;
	pXML_PNODE_t node = (pXML_PNODE_t) n;

	//Free attributes
	for (i = 0; i < node->num_attrib; ++i) {
		free_xml_attrib(node->attrib[i]);
	}

	//Free children
	for (i = 0; i < node->num_children; ++i) {
		free_xml_node((pXML_NODE_t) node->children[i]);
	}

	free_buffer(&node->attrib_buffer);
	free_buffer(&node->child_buffer);
	
	if (node->name) {free(node->name);}
	if (node->value) {free(node->value);}

	free(node);
}



void xml_set_name(pXML_NODE_t node, char* name, bool copy) {
	set_string(&((pXML_PNODE_t)node)->name,name,copy);
}

void xml_set_value(pXML_NODE_t node, char* value, bool copy) {
	set_string(&((pXML_PNODE_t)node)->value,value,copy);
}


void xml_add_attrib(pXML_NODE_t n, pXML_ATTRIB_t attr, bool copy) {
	pXML_PNODE_t node = (pXML_PNODE_t) n;
	if (copy) {
		insert_buffer(&node->attrib_buffer,duplicate_xml_attrib(attr));
	} else {
		insert_buffer(&node->attrib_buffer,attr);
	}
}

void xml_add_child_node(pXML_NODE_t n, pXML_NODE_t child, bool copy) {
	pXML_PNODE_t node = (pXML_PNODE_t) n;
	((pXML_PNODE_t)child)->parent = node;
	if (copy) {	
		insert_buffer(&node->child_buffer,duplicate_xml_node(child));
	} else {
		insert_buffer(&node->child_buffer,child);
	}	
}




//************************Print and Debug***************************



// Similar to printf, but concatenates the string
//	Automatically resizes buf to accomidate the new string
//
// Upon failure, returns false and frees the buffer
static bool sprintf_cat(char** pBuf, const char* format, ...) {
	
	va_list vl;
	char* buf = *pBuf;

	//Get the number of characters to append to the buffer
	va_start(vl,format);
	int chars = vsnprintf(NULL,0,format,vl);
	va_end(vl);
	if (chars < 0) {return free(buf), false; /* Print Error */}

	//Figure out the new length needed for the String buffer
	size_t curLen = ((buf != NULL) ? strlen(buf) : 0);
	size_t newLen = curLen+chars+1;		//Don't forget null terminator

	//Resize the buffer to match the new size
	void* newBuf = realloc(buf, curLen+chars+1);
	if (!newBuf) {return free(buf), false; /* Realloc Error */}
	
	//Print the string
	va_start(vl,format);
	vsnprintf(newBuf+curLen,newLen-curLen,format,vl);
	va_end(vl);

	//Update the buffer
	*pBuf = newBuf;
	return true;
}



static void xml_print_recurse(pXML_NODE_t node, size_t level) {
	size_t i;

	for (i = 0; i < level; ++i) {printf("  ");}
	printf("<%s",node->name);
	
	for (i = 0; i < node->num_attrib; ++i) {
		pXML_ATTRIB_t attr = node->attrib[i];
		printf(" %s=\"%s\"",attr->name,attr->value);
	}

	printf(">%s",node->value);
	if (node->num_children > 0) {
		printf("\n");

		for (i = 0; i < node->num_children; ++i) {
			xml_print_recurse(node->children[i],level+1);
		}

		for (i = 0; i < level; ++i) {printf("  ");}
	}

	printf("</%s>\n",node->name);
}

void xml_print_node(pXML_NODE_t node) {
	xml_print_recurse(node,0);
}



//Just like sprintf_cat, but returns false on failure
//	This makes the code slightly more ledgible
#define sprintf_c(pBuf,format,...) if (!sprintf_cat((pBuf),(format),##__VA_ARGS__)) {return false;}

static bool xml_to_string_recurse(pXML_NODE_t node, size_t level, char** pBuf) {

	size_t i;

	for (i = 0; i < level; ++i) {sprintf_c(pBuf,"  ");}
	sprintf_c(pBuf,"<%s",node->name);
	
	for (i = 0; i < node->num_attrib; ++i) {
		pXML_ATTRIB_t attr = node->attrib[i];
		sprintf_c(pBuf," %s=\"%s\"",attr->name,attr->value);
	}

	sprintf_c(pBuf,">%s",node->value);
	if (node->num_children > 0) {
		sprintf_c(pBuf,"\n");

		for (i = 0; i < node->num_children; ++i) {
			if (!xml_to_string_recurse(node->children[i],level+1,pBuf)) {
				return false;
			}
		}

		for (i = 0; i < level; ++i) {sprintf_c(pBuf,"  ");}
	}

	sprintf_c(pBuf,"</%s>\n",node->name);
	return true;
}


char* xml_to_string(pXML_NODE_t node) {
	char* buf = NULL;
	xml_to_string_recurse(node,0,&buf);
	return buf;
}

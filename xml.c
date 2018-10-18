// C Data Structures
// (C) Comprosoft 2018 - All Rights Reserved
//
//	xml.c - Implementation of a general-purpose XML structure type
//
#include "xml.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//************************Private Functions***************************
#define INITIAL_SIZE  16
#define INCREASE_SIZE 16


static inline char* dupstr(const char* input) {
	if (!input) {return NULL;}

	char* buf = malloc(strlen(input) + 1);
	if (!buf) {return NULL;}
	strcpy(buf,input);
	return buf;
}


static inline void set_string(char** ptr, char* string, int copy) {
	if (*ptr) {free(*ptr);}
	if (copy) {*ptr = dupstr(string);}
	else {*ptr = string;}
}


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
}




//************************XML Attributes***************************

pXML_ATTRIB_t new_xml_attrib() {
	return (pXML_ATTRIB_t) calloc(1,sizeof(XML_ATTRIB_t));
}

pXML_ATTRIB_t duplicate_xml_attrib(pXML_ATTRIB_t attr) {
	pXML_ATTRIB_t new = new_xml_attrib();

	if (attr->name) {new->name = dupstr(attr->name);}
	if (attr->value) {new->value = dupstr(attr->value);}

	return attr;
}

void free_xml_attrib(pXML_ATTRIB_t attr) {
	if (attr->name)  {free(attr->name);}
	if (attr->value) {free(attr->value);}
	free(attr);
}

void xml_attrib_set_name(pXML_ATTRIB_t attr, char* name, int copy) {
	set_string(&attr->name,name,copy);
}

void xml_attrib_set_value(pXML_ATTRIB_t attr, char* value, int copy) {
	set_string(&attr->value,value,copy);
}






//************************XML Nodes***************************

pXML_NODE_t new_xml_node() {
	return (pXML_NODE_t) calloc(1,sizeof(XML_NODE_t));
}

pXML_NODE_t duplicate_xml_node(pXML_NODE_t node) {
	pXML_NODE_t new = new_xml_node();	

	//Blank copy of everything, then duplicate later
	*new = *node;

	if (node->name) {new->name = dupstr(node->name);}
	if (node->value) {new->value = dupstr(node->value);}

	if (node->attrib)   {new->attrib = calloc(node->allocated_attrib,sizeof(void**));}
	if (node->children) {new->children = calloc(node->allocated_children,sizeof(void**));}

	new->a = duplicate_string(&node->a);
	new->e = duplicate_string(&node->e);

	int i;
	for (i = 0; i < node->num_attrib; ++i) {
		new->attrib[i] = duplicate_xml_attrib(node->attrib[i]);
	}

	for (i = 0; i < node->num_children; ++i) {
		new->children[i] = duplicate_xml_node(node->children[i]);
	}

	return new;
}




//Does this recursively
void free_xml_node(pXML_NODE_t node) {
	int i;

	for (i = 0; i < node->num_attrib; ++i) {
		free_xml_attrib(node->attrib[i]);
	}

	for (i = 0; i < node->num_children; ++i) {
		free_xml_node(node->children[i]);
	}

	if (node->attrib)   {free(node->attrib);}
	if (node->children) {free(node->children);}

	if (node->name) {free(node->name);}
	if (node->value) {free(node->value);}

	free_string(&node->a);
	free_string(&node->e);
	free(node);
}



void xml_set_name(pXML_NODE_t node, char* name, int copy) {
	set_string(&node->name,name,copy);
}

void xml_set_value(pXML_NODE_t node, char* value, int copy) {
	set_string(&node->value,value,copy);
}


void xml_add_attribute(pXML_NODE_t node, pXML_ATTRIB_t attr, int copy) {
	if (copy) {
		insert_buffer((pBUFFER_t) &node->num_attrib,duplicate_xml_attrib(attr));
	} else {
		insert_buffer((pBUFFER_t) &node->num_attrib,attr);
	}
}

void xml_add_child_node(pXML_NODE_t node, pXML_NODE_t n, int copy) {
	n->parent = node;	/* Hope this line doesn't break something... */
	if (copy) {	
		insert_buffer((pBUFFER_t) &node->num_children,duplicate_xml_node(n));
	} else {
		insert_buffer((pBUFFER_t) &node->num_children,n);
	}	
}

static void xml_print_recurse(pXML_NODE_t node, int level) {
	int i;

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

void xml_print_node(pXML_NODE_t node) {xml_print_recurse(node,0);}






//************************String Concatenation***************************


//Remove Non-Ascii characters
static inline void filter(char* input) {
	char* temp = input;
	for(;(*input) != 0; ++input) {
		if ((*input > 31) && (*input < 127)) {
			*(temp++) = *input;
		}
	}
	*temp = 0;
}


void concat_string(pCONCAT_STR_t a, char* str, int doFilter) {
	if (!a->buf) {
		a->len = INITIAL_SIZE;
		a->buf = (char*) calloc(a->len+1,sizeof(char));
	}

	if (doFilter) {filter(str);}
	if ((strlen(a->buf) + strlen(str)) > a->len) {
		a->len+=INCREASE_SIZE;
		a->buf = realloc(a->buf,(size_t) (a->len+1) * sizeof(char));
		a->buf[a->len] = 0;
	}

	strncat(a->buf,str,a->len);
	a->buf[strlen(a->buf)] = 0;
}


char* flush_con_string(pCONCAT_STR_t a) {
	char* buf;
	if (!a->buf) {buf = (char*) calloc(1,sizeof(char));}
	else {buf = realloc(a->buf,(strlen(a->buf)+1)*sizeof(char));}

	a->buf = NULL;
	a->len = 0;
	return buf;
}


CONCAT_STR_t duplicate_string(pCONCAT_STR_t a) {
	CONCAT_STR_t ret = *a;	

	if (a->buf) {
		ret.buf = (char*) calloc(a->len+1,sizeof(char));
		memcpy(ret.buf,a->buf,a->len);
	}

	return ret;
}


void free_string(pCONCAT_STR_t a) {
	if (a->buf) {free(a->buf);}
	a->len = 0;
}

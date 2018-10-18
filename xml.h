// C Data Structures
// (C) Comprosoft 2018 - All Rights Reserved
//
//	xml.h - Implementation of a general-purpose XML structure type
//
#ifndef XML_HEADER
#define XML_HEADER


typedef struct {
	char * name;
	char * value;
} XML_ATTRIB_t, *pXML_ATTRIB_t;

typedef struct {
	char* buf;			// Points to malloc location in memory
	size_t len;			// How big is this string in memory??
} CONCAT_STR_t, *pCONCAT_STR_t;

//DO NOT MODIFY num_xxxx and allocated_xxxx
typedef struct XML_NODE_t {
	char* name;						// Name of the tag
	char* value;					// Text inside the tag

	int num_attrib;					// Number of attributes in use
	int allocated_attrib;			// Number of spaces in the attrib array (multiples of 10)
	pXML_ATTRIB_t * attrib;			// Attributes array

	int num_children;				// Number of children in use
	int allocated_children;			// Number of spaces in the childrne array (multiples of 10)
	struct XML_NODE_t ** children;	// Array of children nodes that I own
	
	struct XML_NODE_t* parent;		// Who owns me?? (Can be changed)

	CONCAT_STR_t a;		//String for the node
	CONCAT_STR_t e;		//String for the element
} XML_NODE_t, *pXML_NODE_t;


//This is TERRIBLE coding practice, but when XML_NODE_t is
//	passed to a function, num_children or num_attrib are
//	cast to this struct
typedef struct {
	int inuse;			//Number of items that I am currently using
	int alloc;			//Number of total items available
	void** arr;			//Array of pointers
} BUFFER_t, *pBUFFER_t;



//************XML Attributes************

pXML_ATTRIB_t new_xml_attrib();
pXML_ATTRIB_t duplicate_xml_attrib(pXML_ATTRIB_t);
void free_xml_attrib(pXML_ATTRIB_t);

void xml_attrib_set_name(pXML_ATTRIB_t node, char* name, int copy);
void xml_attrib_set_value(pXML_ATTRIB_t node, char* value, int copy);



//************XML Nodes***************

pXML_NODE_t new_xml_node();
pXML_NODE_t duplicate_xml_node(pXML_NODE_t);
void free_xml_node(pXML_NODE_t node);

void xml_set_name(pXML_NODE_t node, char* name, int copy);
void xml_set_value(pXML_NODE_t node, char* value, int copy);
void xml_add_attribute(pXML_NODE_t node, pXML_ATTRIB_t attr, int copy);
void xml_add_child_node(pXML_NODE_t node, pXML_NODE_t n, int copy);

void xml_print_node(pXML_NODE_t node);



//************String Concatenation********
void concat_string(pCONCAT_STR_t a, char* str, int doFilter); 
char* flush_con_string(pCONCAT_STR_t);
CONCAT_STR_t duplicate_string(pCONCAT_STR_t a);
void free_string(pCONCAT_STR_t a);


#endif

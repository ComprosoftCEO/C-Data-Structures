//General-purpose XML object
#ifndef XML_HEADER
#define XML_HEADER

#include <stdbool.h>		/* For bool data type */
#include <stddef.h>			/* For size_t data type */


//XML Attribute Object
typedef struct {
	char* name;
	char* value;
} XML_ATTRIB_t, *pXML_ATTRIB_t;


//XML Node Object
typedef struct XML_NODE_t {
	const char* const name;						// Name of the tag	
	const char* const value;					// Test inside the tag
	struct XML_NODE_t* const parent;			// Who owns me?
	
	const size_t num_attrib;					// Number of attributes I own
	pXML_ATTRIB_t const* const attrib;			// Array of attributes

	const size_t num_children;					// Number of children I own
	struct XML_NODE_t* const* const children;	// Array of children
} XML_NODE_t, *pXML_NODE_t;



//************XML Attributes************

pXML_ATTRIB_t new_xml_attrib();
pXML_ATTRIB_t duplicate_xml_attrib(pXML_ATTRIB_t);
void free_xml_attrib(pXML_ATTRIB_t);

void xml_attrib_set_name(pXML_ATTRIB_t node, char* name, bool copy);
void xml_attrib_set_value(pXML_ATTRIB_t node, char* value, bool copy);



//************XML Nodes***************

pXML_NODE_t new_xml_node();
pXML_NODE_t duplicate_xml_node(pXML_NODE_t);
void free_xml_node(pXML_NODE_t node);

void xml_set_name(pXML_NODE_t node, char* name, bool copy);
void xml_set_value(pXML_NODE_t node, char* value, bool copy);
void xml_add_attrib(pXML_NODE_t node, pXML_ATTRIB_t attr, bool copy);
void xml_add_child_node(pXML_NODE_t node, pXML_NODE_t child, bool copy);



//************Print and Debug**************

void xml_print_node(pXML_NODE_t node);
char* xml_to_string(pXML_NODE_t node);		// Be sure to free the string when done


#endif

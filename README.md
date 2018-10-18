# C Data Structures
A variety of useful C data structures to aid in future projects:
* __[Dynamic Array](#dynamic-array)__
* __[Dynamic Linked-List Array](#dynamic-linked-list-array)__ 
* __[XML Object](#xml-object)__

_More to come in the future..._

<br>

## Dynamic Array
* Header file: *dynamic_array.h*
* Code file: *dynamic_array.c*

This data structure essentially functions as an array, but is automatically resized as new items 
are added. Deleting items from the array requires either reordering or moving all items to fill
in the empty space. Each item in the array is the same size, set when the array is first created.


<br>

## Dynamic Linked-List Array
* Header file: *dyll_array.h*
* Code file: *dyll_array.c*

While similar to the Dynamic Array, the Dynamic Linked-List Array (or DyLL), stores items as a
doubly-linked-list as opposed to an array. The linked-list is automatically resized as new items
are added. Items can be deleted from anywhere in the array and order is maintained without moving
anything. Each item in the array can be a different size.


<br>

## XML Object
* Header file: *xml.h*
* Code file: *xml.c*

Allows you to create and manipulate an XML structure in memory using a series of function calls.

_Note: This object is still needs some work..._

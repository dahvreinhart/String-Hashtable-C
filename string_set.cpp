/* **************************************************************************************************************
 * Dahv Reinhart
 * V00735279
 * SENG265 - Assignment 1
 * 
 * The purpose of string_set.cpp is to handle the creation of a hashtable data structure.		
 * The hashtable is made up of a defined number of nodes, each the head of a linked list.
 * The data stored in each node is a string (array of characters). This string can be created, 
 * deleted and manipulated by an external user.
 * **************************************************************************************************************
 */
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <typeinfo>

#include "string_set.h"

using namespace std;

/* The constructor initialized the hashtable structure by setting all nodes to null
 * upon initial creation. It also initialized the iterator pointers to a null value
 * so they can then be used for the various operations to be done on the hashtable.
 */
string_set::string_set() {

	iterator_index = 0;
	iterator_node = NULL;
	for (int i = 0; i < HASH_TABLE_SIZE; i++) { //set all nodes to NULL as an initial assignment
		hash_table[i] = NULL;
	}
}

/* The add function attaches a new node containing a new string into the hashtable structure.
 * to do this, it takes the hash value from hash_function and calculates the correct placement
 * of a node within the hashtable by its cumulative unicode integer value. At the end of this function
 * the iterator is reset. error messages occur if the string passed in is not a legal string, if
 * the element attempting to be added already exists within the structure, or if the memory 
 * supposedto be allocated for the creation of a new node within the list is faulty.
 */
void string_set::add(const char *s) {   
	
	if (typeid(s) != typeid(const char*)) {  //printed error message if the parameter passed in
		cout << "Invalid Input" << endl; //is not a legal string.
		exit(-2);
	}

	if (contains(s) == 1)
		throw duplicate_exception{}; //exception if there is already an identical element
                                             //within the structure.
	int addLoc = hash_function(s);	
	node *addN = new node;   //creation of the new element to be added

	if (addN == NULL) {
		throw memory_exception{}; //throw exception if memory allocation failed
	}

	addN->s = new char[strlen(s) + 1]; //assignment of the parameter data to the new node
	strcpy(addN->s, s);

	if (addN->s == NULL) {
		throw memory_exception{};
	}

	addN->next = hash_table[addLoc]; //addition of the new node to the hashtable structure
	hash_table[addLoc] = addN;
	reset(); //reset the iterator so it begins at the beginning of the structure when called further
}

/* The remove function is responsible for deleting a node from the hashtable structure. 
 * To do this, it must hadle a number of possible cases dependant on the happenstance
 * placement of the node to be deleted in the hashtable structure. Exceptions/errors are
 * thrown in the cases where the string parameter to be matched to a node is not a legal string
 * or if no element matching the parameter string exists within the structure.
 */
void string_set::remove(const char *s) { 

	if (typeid(s) != typeid(const char*)) { //throw exception if parameter is not legal string
		cout << "Invalid Input" << endl;
		exit(-2);
	}
	
	if (contains(s) == 0)
		throw not_found_exception{}; //throw exception if no such element exists

	iterator_index = hash_function(s);
	iterator_node = hash_table[iterator_index]; //find the linked list to which the string belongs
	node *pre;
	node *curr;

	//in the case that the node to be deleted is the first element of an otherwise empty list
	if ((hash_table[iterator_index]->next == NULL) && (strcmp(s , hash_table[iterator_index]->s) == 0))
		hash_table[iterator_index] = NULL; 	

	//in the case where the node to be deleted is the first element of a populated list
	else if ((hash_table[iterator_index]->next != NULL) && (strcmp(s , hash_table[iterator_index]->s) == 0)) {
		curr = hash_table[iterator_index]->next;
		hash_table[iterator_index]->s = hash_table[iterator_index]->next->s;
		hash_table[iterator_index]->next = curr->next;
		delete curr; //frees the memory previously occupied by the deleted node
		
	}

	//in the case where the node to be deleted is not the first element but rather a node
	//within its respective linked list.
	else {
		while (iterator_node != NULL) {
			pre = iterator_node;
			curr = iterator_node->next;
		
			if (strcmp(s , curr->s) == 0) {
				pre->next = curr->next;
				delete curr;
				break; //frees the memory previously occupied by the deleted node
			}
		}	
	}
	reset(); //reset the iterator so it will begin at the beginning of the structure when called further
}

/* The contains function searches through the hashtable structure and determines weather a given string
 * exists within any of the nodes of the structure. It will return 1 if a matching string is found and 0
 * if no such element exists within the structure. An exception is thrown if the parameter being searched
 * for is not a legal string.
 *
 */
int string_set::contains(const char *s) { 

	if (typeid(s) != typeid(const char*)) {
		cout << "Invalid Input" << endl; //throw exception if parameter is not a legal string
		exit(-2);
	}

	iterator_index = hash_function(s);
	iterator_node = hash_table[iterator_index]; //find the location of the linked list where the element should be

	while (iterator_node != NULL) {
		if (strcmp(s, iterator_node->s) == 0) { //loop through the linked list
			return 1; //item is found
		}
		iterator_node = iterator_node->next;
	}
	return 0; //no such element exists
}

/* The reset function is a simple function for reseting the iterator. It does this by setting the values
 * of the iterator pointers to null in preparation for a new search of the structure starting from the 
 * beginning.
 */
void string_set::reset() {
	iterator_index = 0;
	iterator_node = NULL;
}

/* The next function uses the iterator to return the next element in the array.
 * It does this by looping through the structure in a stepwise fashion. The current  
 * location of the next function is preserved in the data values assigned to the iterator
 * pointers.  In this way, each call to next will return consecuitive element in the structure
 * starting from position hash_table[0].
 */
const char *string_set::next() { 
	
	if (iterator_index > HASH_TABLE_SIZE -1) { //we have reached the end of the hashtable
		reset();
		return NULL;
	}

	while (iterator_index < HASH_TABLE_SIZE) { // loops through the hashtable structure

		if (iterator_node == NULL) { //if no linked list exists at that hashtable position, keep looking
			iterator_index++;
			iterator_node = hash_table[iterator_index];
			continue;
		}

		if ((iterator_node->next != NULL) || (iterator_node != NULL)) { //if a populated list is found
			
			node *temp = iterator_node;
			temp->s = iterator_node->s;

			if (iterator_node->next == NULL) { //if we have reached the end of a given list
				iterator_index++;
				iterator_node = hash_table[iterator_index];
				return temp->s;
			}

			iterator_node = iterator_node->next; //increment the iterator in preparation for a further call
			return temp->s;
		}
	}
		return NULL;
}

/* The destructor deletes all allocated, dynamic memory.  This includes deletion of each node in the lists,
 * each node in the hashtable, and gradually, the hashtable itself.
 */
string_set::~string_set() {
	for (int i = 0; i < HASH_TABLE_SIZE; i++) { //loop through the hashtable structure
		if (hash_table[i] != NULL) { //if a position is populated with a list
			node *temp = hash_table[i];
			iterator_node = hash_table[i]->next;
			temp = NULL; //delete the first element
			delete temp;

			while (iterator_node != NULL) { //loop through the remainder of the list deleting nodes
				temp = iterator_node;
				iterator_node = iterator_node->next;
				temp = NULL;
				delete temp;	
			}
		}
		else {
			hash_table[i] = NULL; //if a hashtable position is unused, simply delete it outright
			delete hash_table[i];
		}
	}
}
 
/* The hash function method is reponsible for finding the correct level of the hashtable to access
 * a given linked list.  This is important for a number of functions above. To add, remove or search
 * the structure, you must knew in which list contained within the hashtable you should begin looking
 * for what you want. This is what the hash function does.  It calculates the cumulative unicode integer
 * value of the parameter string passed in. It ensure all activity remains within the parameters of
 * the hash table by then modding this cumulative value by the size of the hashtable itself. An exception
 * is thrown if the parameter passed in is not a legal string.
 */
int string_set::hash_function(const char *s) {

	if (typeid(s) != typeid(const char*)) {
		cout << "Invalid Input" << endl; //if the parameter passed in not a legal string
		exit(-2);
	}

	reset(); //reset the iterator to ensure consistant calculation
	int i = 0;
	while (s[i] != '\0') {
		iterator_index = (iterator_index + s[i]) % HASH_TABLE_SIZE;
		i++;
	}
	return iterator_index;
}

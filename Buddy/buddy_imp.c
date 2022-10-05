#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>

#define MAX_ORDER 30	  // maximum order ;
#define MAX_SIZE (1 << MAX_ORDER) // total and maximum size = 2^K_MAX
#define MIN_ORDER 6  //for blocks of order smaller or equal to 4 addresses where not stored correctly


//double linked list
typedef struct dllist_t dllist_t;
struct dllist_t
{
	unsigned int free :1; 			// width one field indicating if a block is being used or not
	int order;	   					// (2^order)
	dllist_t *prev;	   				// previous block in list of free nodes
	dllist_t *next;	   				// next block in list of free nodes
};


#define METADATA sizeof(struct dllist_t)


static void *head = NULL;
static dllist_t *free_nodes[MAX_ORDER + 1];

void init_memory();
int get_order(int size);
dllist_t *get_block(int order);

// Functions to manage the list of free nodes for every size
void add_node(dllist_t *node);
void remove_node(dllist_t *node);

//	Functions to manage the operations on blocks
void merge(dllist_t *block);
dllist_t *join_blocks(dllist_t *left, dllist_t *right);
dllist_t *half(dllist_t *block, int new_order);

// Memory managment functions
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

/*
void print_free_nodes() {
	//printf("FREE:");
	int hh = 0;
    int j = 0;
	for(hh = MAX_ORDER; hh > MIN_ORDER; hh--) {
		dllist_t *aux = free_nodes[hh];
		while (aux) {
			if (aux->free == 1)
                j = j + 1;
	//			printf("%i%c%p\n", hh, '.', aux);

			aux = aux->next;
		}
	}
	//printf("\n\n\n");
}

void print_not_free_nodes() {
	//printf("NOT FREE\n");
	int hh = 0;
    int j = 0;
	for(hh = 11; hh > 10; hh--) {
		dllist_t *aux = free_nodes[hh];
		while (aux) {
			if (aux->free == 0)
                j = j + 1;
				//printf("%i%c%p\n", hh, '.', aux);

			aux = aux->next;
		}
	}
	//printf("\n\n\n");
}
*/

void *malloc(size_t size) {

	if (size == 0)
		return NULL;

	if (size < 0)
		return NULL;

	if (size > MAX_SIZE)
		return NULL;

	if (!head)		   // this is the first block to be allocated
		init_memory(); // memory structure needs to be initilizated

	int order = get_order(size + METADATA); // minimum order in which data fits
	dllist_t *block = get_block(order);

	if (block) { // not null, so a block has been found or created
		block->free = 0;
		remove_node(block);
		return (block + 1);
	}

	return NULL;  //most likely to mean theres not more memory available
}


void *calloc(size_t nmemb, size_t size) {	

	if (nmemb == 0)
        	return NULL;

	if (size == 0)
		return NULL;

	int s = nmemb * size;

	void *space = malloc(s);
	if (space != NULL) {
		//memset(space + sizeof(int) + 1, 0, nmemb * size); // fill blocks
		memset(space, 0, nmemb * size); // fill blocks
		return space;
	}
	return NULL;
}


void *realloc(void *ptr, size_t size) {

	if (ptr == NULL) {
		return malloc(size);
	}

	if (size == 0) {
		free(ptr);
		return NULL;
	}

	void *new_location = malloc(size);

	if (new_location) {
		memmove(new_location, ptr, size);
		free(ptr);						 				// frees old data
		return new_location;
	}

	return NULL;
}

void free(void *ptr) {

	if (ptr) {
		dllist_t *block = (((dllist_t *)ptr) - 1);
		if (block->free == 0) {
			block->free = 1;
			merge(block);
		}
	}
}

void init_memory() { // creates first block

	int i = 0;
	head = sbrk(MAX_SIZE);
	
	for (i = 0; i < MAX_ORDER + 1; i++) {
	    free_nodes[i] = NULL;
	}

	
	dllist_t *first = head;
	first->order = MAX_ORDER;
	first->free = 1;
	first->next = NULL;
	first->prev = NULL;

	free_nodes[MAX_ORDER] = first;

}

int get_order(int size)
{
	/*
	custom log2 function because the function log2 from package math.h
	wouldnt accept a size_t argument
	*/

	int x = 0;
	int lg2 = 0;
	int aux_size = size;

	while (aux_size >>= 1)
		x++; // x is the integer value of log 2 of size

	int y = 2 << (x - 1);     	// available space in block
	if (y >= size)		  	// the size is an exact power of 2
		lg2 = x;		// fits in the block
	else
		lg2 = x + 1;

	if (lg2 <= MIN_ORDER)
		return MIN_ORDER + 1;
	return lg2;
	//return x + 1; // it is not exact power, so it is allocated in the closest bigest block
}

dllist_t *get_block(int order)
{
	if (order > MAX_ORDER) {
		return NULL;
	}

	dllist_t *aux = free_nodes[order];  //error generated here

	if (aux == NULL) {
		dllist_t *bigger_block = NULL;
		bigger_block = get_block(order + 1); //if it gets to this line, it means that there doesnt exist a (free) block of this size, so it needs to find a bigger one to be divided
		if (bigger_block != NULL)
		{ // it is necessary to split in halves these blocks
			aux = half(bigger_block, order);
		}
	}

	return aux;
}

dllist_t *half(dllist_t *block, int new_order)
{

	while (block->order > new_order)
	{
		remove_node(block);				 // remove from free list
		int changed_order = block->order - 1;
		block->order = changed_order; // this means half the original size
		int size = 1 << block->order; // 2^order

		dllist_t *next_block = ((void *)block) + size;
		next_block->order = changed_order;
		next_block->free = 1;
        	next_block->next = NULL;
        	next_block->prev = NULL;
	
		add_node(next_block);
		add_node(block);
	}
	return block;
}


void merge(dllist_t *block) {
	// the two posibilities to merge block with are:
	dllist_t *left_bud = block->prev;
	dllist_t *right_bud = block->next;
	int x = 0;
	if (left_bud && left_bud->free == 1 && left_bud->order == block->order) { // it is going to merge block with its left buddy
		remove_node(left_bud);
		remove_node(block);
		join_blocks(left_bud, block);
		add_node(left_bud);
		merge(left_bud);

		
	}
	else if (right_bud && right_bud->free == 1 && right_bud->order == block->order) {
		remove_node(block);
		remove_node(right_bud);
		join_blocks(block, right_bud);
		add_node(block);
		merge(block);	//checks if the now bigger block can be merged with other bigger blocks
	}
}


dllist_t *join_blocks(dllist_t *left, dllist_t *right) {

	dllist_t *merged_block = left;

	merged_block->free = 1;
	merged_block->order = right->order + 1;
	merged_block->next = right->next;
	merged_block->prev = left->prev;

	if (right->next)
		right->next->prev = merged_block;

	if (left->prev)
		left->prev->next = merged_block;

	return merged_block;
}


void add_node(dllist_t *node)
{
    if (node != NULL) {
	int o = node->order;

	if (!free_nodes[o]) { // there isnt any free node of this size yet
		free_nodes[o] = node;
		node->next = NULL;
		node->prev = NULL;
	}

	else { // we add the node to the existing nodes of this size

		node->prev = NULL;
		node->next = free_nodes[o];
		free_nodes[o]->prev = node;
		free_nodes[o] = node;
	}
    }
}

void remove_node(dllist_t *node)
{
    if (node != NULL) {

	int o = node->order;

	
	// if they exist, the successor of node becomes the successor of the predecessor of node
	if (node->prev)
		node->prev->next = node->next;

	// if they exist, the predecesor of node becomes the predecessor of the successor of node
	if (node->next) {
		if (free_nodes[o] == node)
			free_nodes[o] = node->next;

		node->next->prev = node->prev;
	}

	else {
		if (free_nodes[o] == node)
			free_nodes[o] = NULL;
	}

		
	node->prev = NULL;
	node->next = NULL;

	}
}

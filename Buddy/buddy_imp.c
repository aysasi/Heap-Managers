 #include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>


#define MAX_ORDER 22                     	// maximum order 
#define MAX_SIZE (1 << MAX_ORDER)     		// total and maximum size = 2^K_MAX


typedef struct dllist_t dllist_t;
struct dllist_t {
	unsigned	free: 1; 					// width one field indicating if a block is being used or not
	size_t		order; 						// (2^order)
	dllist_t*		prev;					// previous block in list of free nodes
	dllist_t*		next;					// next block in list of free nodes
};

void* head = NULL;
dllist_t* free_nodes[MAX_ORDER + 1];


void init_memory();
int get_order();
dllist_t* get_block(int order);

// Functions to manage the list of free nodes for every size
void add_node(dllist_t* node); 
void remove_node(dllist_t* node);

//	Functions to manage the operations on blocks
void merge(dllist_t* block);
dllist_t* join_blocks(dllist_t* left, dllist_t* right);
dllist_t* half(dllist_t* block, size_t new_order);


void *malloc(size_t size) {

	if (size <= 0 || size > MAX_SIZE) 
		return NULL;
	

    if (!head)                              // this is the first block to be allocated
        init_memory();                      // memory structure needs to be initilizated

    int order = get_order(size);            // minimum order in which data fits
    dllist_t* block = get_block(order);


	if (block) { 							//not null, so a block has been found or created
		block->free = 0;
		return (block + 1);
	}

    return NULL; 

}


void *calloc(size_t count, size_t size) {

	void* space = malloc(count * size);
	if (space)
	    memset(space, 0, count * size); 	// fill blocks
	return space;
}


void *realloc(void *ptr, size_t size) {

    void* new_location = malloc(size);

    if (new_location) {     /				// if it is possible to allocate the data
        memcpy(new_location, ptr, size);    // copies data from old region to new
	    free(ptr);                          // frees old data
    }

	return new_location;
}


void free(void *ptr) {

	if (ptr) {
		dllist_t* block = (((dllist_t*)ptr) -1);
		if (block->free){
			block->free = 1;
			merge(block);
		}
	}
}


void init_memory() {						// creates first block
		
	head = sbrk(MAX_SIZE);
	dllist_t* first = head;
	first->order = MAX_ORDER;
	first->free = 1;
	first->next = NULL;
	first->prev = NULL;

	free_nodes[MAX_ORDER] = first;	
}


int get_order(size_t size) {
    /* 
    custom log2 function because the function log2 from package math.h
    wouldnt accept a size_t argument
    */

    int x = 0;
    size_t aux_size = size;
    
    while(aux_size >>= 1)  
        x++;                    			// x is the integer value of log 2 of size
          
    int y = 2 << (x-1);         			// available space in block
    if (y >= size)               			// the size is an exact power of 2
        return x;               			// fits in the block

    return x + 1;               			// it is not exact power, so it is allocated in the closest bigest block
}


dllist_t* get_block(int order) {

	dllist_t* aux = free_nodes[order];
	//dllist_t* aux2 = free_nodes[order];
	while (aux) {
		if (aux->free)
			return aux;

		aux = aux->next;
	}

	dllist_t* bigger_block = get_block(order + 1);
    if (bigger_block) {       				//it is necessary to split in halves these blocks
        aux = half(bigger_block, order);
    }
	return aux;
}


dllist_t* half(dllist_t* block, size_t new_order) {

	while (block->order > new_order) {
		remove_node(block);						// remove from free list
		block->order = block->order - 1;		// this means half the original size
		size_t size = 1 << block->order; 		// 2^order

		dllist_t* next_block = ((void*) block) + size;
		next_block->order = block->order;
		next_block->free = 1;

		add_node(next_block);
		add_node(block);
	}
	return block;
}


void merge(dllist_t* block) {
	// the two posibilities to merge block with are:
	dllist_t* left_bud = block->prev;
	dllist_t* right_bud = block->next;


	if (left_bud && left_bud->free && left_bud->order == block->order) {	//it is going to merge block with its left buddy
		merge(join_blocks(left_bud, block));

		remove_node(left_bud);
		remove_node(block);
		left_bud->order++;
		add_node(left_bud);
	} 
	else if (right_bud && right_bud->free && right_bud->order == block->order) {
		merge(join_blocks(block, right_bud));

		remove_node(block);
		remove_node(right_bud);
		block->order++;
		add_node(block);
	} 
}


dllist_t* join_blocks(dllist_t* left, dllist_t* right) {

  dllist_t* merged_block = left;

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


void add_node(dllist_t* node) {

	size_t o = node->order;

	//if (k == 4)
	//	printf("%d\n", "Hola");
	
	if (!free_nodes[o]) {				// there isnt any free node of this size yet
		free_nodes[o] = node;
		node->next = NULL;
		node->prev = NULL;
	}
    else {								// we add the node to the existing nodes of this size
        node->prev=NULL;
        node->next = free_nodes[o];
	    free_nodes[o]->prev = node;
	    free_nodes[o] = node;
    }
}


void remove_node(dllist_t* node) {

	size_t o = node->order;

	if (free_nodes[o] == node)          // node is going to be the first element in the list of free nodes
		free_nodes[o] = node->next;

    // if they exist, the successor of node becomes the successor of the predecessor of node
	if (node->prev)             
		node->prev->next = node->next;

    // if they exist, the predecesor of node becomes the predecessor of the successor of node
	if (node->next)
		node->next->prev = node->prev;

	node->prev = NULL;
	node->next = NULL;
}

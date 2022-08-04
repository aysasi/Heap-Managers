#include <unistd.h>
#include <stdio.h>
#include <string.h>


#define LIST_SIZE sizeof(llist_t)


typedef struct llist_t llist_t;
struct llist_t {
	unsigned 	free:1;	                    // width one field indicating if a block is being used or not
	size_t 		size;                       // size of each block
	llist_t* 	next;                       // contiguous block to each block
};

llist_t* head = NULL;                       // first element of the list


llist_t* create_block(llist_t* last, size_t size);
llist_t* last_block(llist_t**, size_t);
void free(void* ptr);


void *malloc(size_t size) {

	llist_t* last = head;                // to iterate over the blocks

	llist_t* block = last_block(&last, size);
    llist_t* block_new = create_block(last, size);

    if (!head){                         // the list is empty
        head = block_new;
        return (head + 1);
    }

    if (block) {                        
        block->free = 0;                // this block is being used
        return (block + 1);
    }

    else{
        if (!block_new)
			return NULL;
        return (block_new + 1);
    }

}


void *calloc(size_t count, size_t size) {

	void* space = malloc(count * size);
	if (space)
	    memset(space, 0, count * size);     // fill blocks
	return space;
}


void *realloc(void *ptr, size_t size) {

    void* new_location = malloc(size);

    if (new_location) {                     // if it is possible to allocate the data
        memcpy(new_location, ptr, size);    // copies data from old region to new
	    free(ptr);                          // frees old data
    }

	return new_location;
}


void free(void* ptr) {
    llist_t* block = ((llist_t*) ptr) - 1;

    llist_t* aux = head;                    // begins from the first block
	while (aux && aux->next != block && aux != block)  {
		aux = aux->next;
	}

    if (aux->next == block){
        aux->next = block->next;
    }
    

    block->free = 1;
}


llist_t* create_block(llist_t* last, size_t size) {

	llist_t* new_block = sbrk(size + LIST_SIZE);     // increases the data space by size bytes

	new_block->size = size; 
	new_block->next = NULL;             // because now it is the last block of the list
	new_block->free = 0;

    if (last)
        last->next = new_block;

	return new_block;
}


llist_t* last_block(llist_t** last, size_t size) {

	llist_t* aux = head;                    // begins from the first block
	while (aux && !(aux->free && aux->size >= size))  {
		*last = aux;
		aux = aux->next;
	}
	return aux;
}

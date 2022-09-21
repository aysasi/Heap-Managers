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

static llist_t* head = NULL;                       // first element of the list


llist_t* create_block(llist_t* last, size_t size);
llist_t* last_block(llist_t**, size_t);

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);


void *malloc(size_t size) {

    if (size ==0)
        return NULL;

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


void *calloc(size_t nmemb, size_t size) {

    if (nmemb == 0 || size == 0)
        return NULL;

    void* space = malloc(nmemb * size);
	if (space) {
	    memset(space, 0, nmemb * size);     // fill blocks
	    
	    }
	return space;
	
    //else
    //    return NULL;  //i think this is not necessary because space is already NULL?
	
}


void *realloc(void *ptr, size_t size) {

    if(!ptr)
        return malloc(size);


    if (size == 0) {
        free(ptr);
        return NULL;
    }

    if(((llist_t*)ptr - 1)->size >= size)
        return ptr;


    void* new_location = malloc(size);

    if (new_location) {                     // if it is possible to allocate the data
        memmove(new_location, ptr, size);    // copies data from old region to new
	    free(ptr);                          // frees old data
	    return new_location;
    }
	return NULL;
	
}


void free(void *ptr) {
    if(ptr)
        ((llist_t*)ptr - 1)->free = 1;
    // if ptr is null, no operation is performed
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


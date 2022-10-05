# Heap-Managers
Operating Systems final project. It includes two versions of malloc/calloc/realloc and free. One is a linked-list implementation and the other is an implementation of the buddy system.

## Intructions for the assignment
1. Your task is to implement two versions of malloc/calloc/realloc and free: One should be a linked-list implementation and the other an implementation of the buddy system.
2. You need to use sbrk to allocate memory from the kernel on Linux for the list version and increment the heap size in suitable steps. For the buddy system, you are allowed to allocate a sufficiently large block at once.
3. At a free, you must merge adjacent unallocated blocks in the list version and buddies in the buddy version.
4. Your implementations should work with gawk. Use any version (versions 3.x are recommended, being known to work properly; versions >= 4 may cause problems).

## Set up
I have tested my implementations with gawk 3.1.8, as it is the last version before a v.4, which may cause problems. Both implementations are found on `/Buddy/buddy_imp.c` and `/Linked/linked_list.c`, respectively.

For setting up the project, check the following instructions:
1. First, it is necesaty to extract the compressed `gawk-3.1.8` file.
2. Run the command `cd ./gawk-3.1.8` and then `./configure`. Once it has finished,a first `make check` is required.
3. After it has finished, go to file `Makefile` and in line 93 and  line 380 type `malloc_imp.$(OBJEXT)` and `\ malloc_imp.c` respectively.
4. Go back to the initial directory and type `make linked` or `make buddy` to test them.

Whenever you run any of the `make` commands from above, a file called `malloc_imp.c` is created in `/gawk-3.1.8` containing the code of the desired application. 

## Useful links
* [Implementing Malloc](http://moss.cs.iit.edu/cs351/slides/slides-malloc.pdf)
* [Question on implementing Malloc](https://stackoverflow.com/questions/5422061/malloc-implementation)
* [Malloc Implementation](https://stackoverflow.com/questions/5422061/malloc-implementation)
* [Malloc tutorial](https://danluu.com/malloc-tutorial/)
* [Buddy System](https://www.geeksforgeeks.org/buddy-system-memory-allocation-technique/)

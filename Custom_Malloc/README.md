## Custom Malloc Program

#### Description

This is a malloc implementation for dynamic memory allocation. The program uses the functions sbrk() and brk() to adjust the heap in order to provide memory allocations for user requests. The program also implements a free function with which allocated memory can be deallocated, thereby freeing heap space. The heap is adjusted accordingly.
The next fit algorithm is used for this implementation which always looks for the next available free space in the allocations based on the last search. 

#### Setup Instructions

1. create a header file titled mymalloc.h which defines the functions that are needed. 
```
void *my_nextfit_malloc(int size);
void my_free(void *ptr);
```
2. with these definitions we can then include mymalloc.h within the mymalloc.c file provided as well as any driver file or other implementation it may be used for. To do this simply add the preprocessor directive mymalloc.h so the program knows where to look:
```
#include "mymalloc.h"
```
3. Add any macros for malloc names if you wish to alter the names of the commands. Add implementation that you wish to use the dynamic memory allocation for and use my_free and my_nextfit_malloc as needed.

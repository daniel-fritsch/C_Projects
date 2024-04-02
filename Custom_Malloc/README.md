## Custom Malloc Program

This is a malloc implementation for dynamic memory allocation. The program uses the functions sbrk() and brk() to adjust the heap in order to provide memory allocations for user requests. The program also implements a free function with which allocated memory can be deallocated, thereby freeing heap space. The heap is adjusted accordingly.
The next fit algorithm is used for this implementation which always looks for the next available free space in the allocations based on the last search. 

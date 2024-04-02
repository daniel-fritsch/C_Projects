#include <stdio.h>
#include <unistd.h> // For sbrk()
#include <errno.h>


// node structure
struct Node {
    int size;  // size of the memory user requested stored after the position of the node
    unsigned int is_free : 1; // bit that signals if the assigned space is allocated or not(1 or 0)
    struct Node *next; // next node in sequence pointer
    struct Node *previous; // previous node in sequence pointer
};

// global variables
struct Node *heap_start = NULL;
struct Node *heap_end = NULL;


// this creates the start of the heap by creating the first node
void *initialize_heap(int size) {
    // Allocate memory for a new node and the space of first request
    void *pointer_check = sbrk(sizeof(struct Node)+size);
    if (errno == ENOMEM) { 
        printf("sbrk returned -1"); // check sbrk error
        return NULL;
    }

    // create the first node of heap
    heap_start = (struct Node *)pointer_check;
    
    // Initialize the properties of the new node for the first memory request
    heap_start->size = size;        // Initial size
    heap_start->is_free = 1;     // 1 indicates that the space is allocated
    heap_start->next = NULL;     // There are no other nodes yet
    heap_start->previous = NULL;     

    heap_end = heap_start; // set the end of the heap to the node as well

    return heap_start;
}

void *extend_heap(int size) {
    // extend the heap and assign previous brk to pointer_check
    void *pointer_check = sbrk(sizeof(struct Node)+size);

    if (errno == ENOMEM) {
        printf("sbrk returned -1"); // check sbrk error
        return NULL;
    }

    // case pointer check to a Node pointer
    struct Node *new_node = (struct Node *)pointer_check;
    
    // node properties
    new_node->size = size;    
    new_node->is_free = 1;     
    new_node->next = NULL;  

    // linking the new node into the existing list
    new_node->previous = heap_end;
    heap_end->next = new_node;
    heap_end = new_node;  

    return new_node;
}

// global variable to keep track of current node
struct Node *next_fit_node = NULL;
// next fit handles searching for a deallocated space. Returns a pointer to a free section if found
// does not alter/change the heap in any way.
void *next_fit(int size) {
    if (next_fit_node == NULL) {
        next_fit_node = heap_start;
    }
    if (next_fit_node == heap_start && next_fit_node->next == NULL) {
        return NULL;
    }
    struct Node *current = next_fit_node->next;
    

    while (current != next_fit_node) {
        if (current->is_free == 0 && current->size >= size){
            next_fit_node = current;
            return(current);
        }

        current = current->next;
        if (current == NULL) {
            current = heap_start;
        }
    }
    return NULL;
}

// function handles allocations to unallocates spaces found within the heap.
void *memory_request(void *ptr, int size) {
    struct Node *node_ptr = (struct Node *)ptr;
    // get full space of unallocated block and change its size to that requested, mark allocated
    int full_chunk_size = node_ptr->size;
    node_ptr->size = size;
    node_ptr->is_free = 1;

    // assuming there is enough space in the deallocated segment found to ...
    // create allocated node and another node for leftover deallocated space.
    if (full_chunk_size > size) {
        int leftover_unallocated_space = full_chunk_size - size;
        int node_size = sizeof(struct Node);
        // make a new extra deallocated node so we don't lose space to fragmentation if possible
        if (node_size <= leftover_unallocated_space) {
            int new_deallocated_node_size = leftover_unallocated_space - node_size;
            
            // insert the new node
            struct Node *new_node = (struct Node *) (ptr + size + sizeof(struct Node));
            
            // Initialize the properties of the new node
            new_node->size = new_deallocated_node_size;
            new_node->is_free = 0;
            new_node->next = node_ptr->next;
            new_node->previous = node_ptr;

            // Update pointers to properly link into chain
            if (node_ptr->next != NULL) {
                node_ptr->next->previous = new_node;
            } else  {
              heap_end = new_node;
            }
            node_ptr->next = new_node;
        }
    } 
}

// main function for malloc (calls all the other memory allocation cases above)
void *my_nextfit_malloc(int size) {
    if (size <= 0){
        return NULL; // make sure the user input size is greater than zero to prevent negative offsets
    }

    void *result_ptr; // return value var

    if (heap_start == NULL){
        result_ptr = initialize_heap(size);
        return result_ptr + sizeof(struct Node); // make sure to return the memory position after node so as not to overwrite it
    }

    void *free_space = next_fit(size);
    if (free_space == NULL){
        result_ptr = (void *)extend_heap(size);
        return result_ptr + sizeof(struct Node); // make sure to return the memory position after node so as not to overwrite it
    }

    result_ptr = memory_request(free_space, size);
    return result_ptr + sizeof(struct Node); // make sure to return the memory position after node so as not to overwrite it
}

// memory deallocation and coalescing here
void my_free(void *ptr) {
    // Cast ptr to a pointer to struct Node
    struct Node *node_ptr = (struct Node *)(ptr-sizeof(struct Node));

    if (node_ptr->next == NULL){
        if (node_ptr->previous == NULL){
            brk(heap_start);
            heap_start = NULL;
            heap_end = NULL;
        } else {
            if (node_ptr->previous->is_free == 0){
                struct Node *previous_node = node_ptr->previous;
                if (previous_node->previous == NULL){
                    brk(heap_start);
                    heap_start = NULL;
                    heap_end = NULL;
                } else {
                    previous_node->previous->next = NULL;
                    heap_end = previous_node->previous;
                    brk(previous_node);
                }
            } else {
                node_ptr->previous->next = NULL;
                heap_end = node_ptr->previous;
                brk(node_ptr);
            }
        }
        return;
    }

    // mark space as deallocated
    node_ptr->is_free=0;

    // if an adjacent node is unallocated, combine them to make one large unallocated node
    if (node_ptr->next->is_free == 0){
        int node_size = sizeof(struct Node) + node_ptr->next->size;

        node_ptr->size += node_size;
        node_ptr->next = node_ptr->next->next;
        node_ptr->next->previous = node_ptr;
    }
    
    if (heap_start != node_ptr) {
        if (node_ptr->previous->is_free == 0){
            int node_size = sizeof(struct Node) + node_ptr->size;

            node_ptr->previous->size += node_size;
            node_ptr->previous->next = node_ptr->next;
            node_ptr->next->previous = node_ptr->previous;
        }
    }
}


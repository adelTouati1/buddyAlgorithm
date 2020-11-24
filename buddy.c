#include <stdio.h>
#include <math.h>
#include <sys/mman.h>
#include <stdbool.h>
#include "buddy.h"

/* Adel Touati 
   Jaime Guevara */

/* supports memory upto 2^(MAX_KVAL-1) (or 64 GB) in size */
#define  MAX_KVAL  37

// These will help us to keep track of which list of blocks is not being used
// as well as which blocks are currently free, and which blocks are being reserved
#define  UNUSED		0
#define  FREE  		1
#define  RESERVED	2

// Declare helper function prototypes
int log_two(size_t size);
int next_best_two(size_t size);
void mem_error(void);
static struct block_header get_buddy(struct block_header ptr, int best_fit_pow);
static void *divide_mem_block(int curr_pow, int best_fit_pow);
 void *create_mem_block(int curr_pow, int best_fit_pow);
 static void remove_buddy(struct block_header buddy, int best_fit_pow);
static void merge(struct block_header client, struct block_header buddy);
static void rearrange(struct block_header client, int i);
static void	merge_with_buddy(struct block_header ptr, int best_fit_pow);
/* the header for an available block */
struct block_header {
	short tag; // This should be 0 if the block is reserved, 1 if the block is available
	short kval; // This is the size of our block (i.e. k)

	// We can assume that these next two structs are for determining who's the next/prev nodes
	struct block_header *next; 
	struct block_header *prev;
};


/* A static structure stores the table of pointers to the lists in the buddy system.  */
struct pool {
	void *start; // pointer to the start of the memory pool
	int lgsize;  // log2 of size
	size_t size; // size of the pool, same as 2 ^ lgsize
	/* the table of pointers to the buddy system lists */
	struct block_header avail[MAX_KVAL];
} pool;

struct pool mem_pool;
// Pointer for list of buddy lists
struct block_header *availPtr;

/*
 * Public Methods 
 */

/**
 * Buddy Initializer - should be called either by itself or by other functions.
 * @param size - the size for the chunk of memory we want to use
 * @return TRUE - if the setup went according to what was needed
 * 		   ENOMEM - if there was an error in this part
 */
int buddy_init(size_t size) 
{
	// Make sure that our size isn't zero and that we don't go over our max
	// Since we're doing bit-shifting we need to use Long (L)
	if (size == 0 || size < (1L<<19)) {
		mem_pool.start = mmap(NULL, (1L<<29), PROT_READ|PROT_WRITE, MAP_SHARED,-1 ,0); // Set our start point for our pool
		mem_pool.size = 1L<<29; // Default so that size (2^lgsize) is 512MB
	} else if (size > (1L<<(MAX_KVAL-1))) {
		// If we've exceeded our memory size, then there's an error and we should report it and set our errno to ENOMEM
		mem_error(); 
		return ENOMEM;
	} else {
		size = next_best_two(size);
		mem_pool.start = mmap(NULL, pow(2,size), PROT_READ|PROT_WRITE, MAP_SHARED, -1 ,0); // Set our start point for our pool
		mem_pool.size = size;
	}
	// Get the lgsize of our desired size
	mem_pool.lgsize = log_two(mem_pool.size);

	//Set the initial block header
	mem_pool.avail[size].tag = 1; // Set to available
	mem_pool.avail[size].kval = mem_pool.lgsize;
	mem_pool.avail[size].next = NULL;
	mem_pool.avail[size].prev = NULL;

	availPtr = mem_pool.avail;
	
	return TRUE;
}





void *buddy_malloc(size_t size)
{
	int best_fit_pow, current_pow;
	size += sizeof(availPtr);

	if(size > (size_t)mem_pool.lgsize){
		best_fit_pow = next_best_two(size);
	}else{
		best_fit_pow = mem_pool.lgsize;
		current_pow = best_fit_pow;
		while (current_pow <= mem_pool.lgsize){
			if(mem_pool.avail[current_pow].tag == 0){
				//create a new block 
					void * new_block;
			new_block = create_mem_block(current_pow, best_fit_pow);

			if(new_block == NULL)
				mem_error();
			else
				return new_block;
			}
			current_pow++;
		}
	}


	return NULL;
}
// a helper method for creating a memory blocks.
 void *create_mem_block(int curr_pow, int best_fit_pow){
	if(curr_pow == best_fit_pow){
		struct block_header block = mem_pool.avail[curr_pow];

		if(block.next == NULL)
			block.tag = UNUSED;
		else{
			availPtr = block.next;
			block.prev = UNUSED;
		}
		block.tag = RESERVED;
		block.kval = curr_pow;

		void * ptr =  availPtr;
		size_t temp_size = (size_t) ptr +sizeof(mem_pool.avail);
		return (void *)(temp_size);
	}
	else
		return divide_mem_block(curr_pow, best_fit_pow);
}
/**
 * This function clears out the necessary amount of memory
 * in the specified memory block, for the given size
 * 
 * @param nmemb - the nth memory block we want
 * @param size - the size of memory we want to clear
 */
void *buddy_calloc(size_t nmemb, size_t size) 
{
	size_t best_two;
	int i;

	best_two = next_best_two(nmemb * size);
	void *req = buddy_malloc(best_two);

	for (i = 0; i < (int)best_two; i++)
	{
		req = '\0'; // Clear out the memory for this block
		//req++;
	}
	
	return req;
}

static void *divide_mem_block(int curr_pow, int best_fit_pow){
	if(curr_pow == best_fit_pow)
		return create_mem_block(curr_pow, best_fit_pow);

	// start dividing
	struct block_header big_block = mem_pool.avail[curr_pow];
	if(mem_pool.avail[curr_pow].next == NULL)
		mem_pool.avail[curr_pow].tag = FREE;
	else{
		mem_pool.avail[curr_pow].next = big_block.next;
		mem_pool.avail[curr_pow].prev = NULL;
	}

	// next block
	curr_pow--;
	/*struct block_header new_block  = mem_pool.avail[1 << (curr_pow+1)];
	new_block.tag = UNUSED;
	new_block.kval = curr_pow;
	//big_block
	new_block.prev = availPtr;
	new_block.next = NULL;*/

	mem_pool.avail[curr_pow] = big_block;
	big_block.kval--;
	//new_block
	big_block.next = availPtr;
	big_block.prev = NULL;

	// tail-recursively start dividing big blocks
	return divide_mem_block(curr_pow, best_fit_pow);
}

// removes buddy from a tree
static void remove_buddy(struct block_header buddy, int best_fit_pow){
	struct block_header prev_block, next_block;
	prev_block.next = buddy.prev;
	next_block.prev = buddy.next;

	if(prev_block.next == NULL){
		if(next_block.prev == NULL)
			mem_pool.avail[best_fit_pow].tag = UNUSED;
		else{
			mem_pool.avail[best_fit_pow] = next_block;
			next_block.prev = NULL;
		}
	}
	else{
		if(next_block.prev == NULL)
			prev_block.next = NULL;
		else{
			prev_block.next = next_block.prev;
			next_block.prev = prev_block.next;
		}
	}
}
// check if two memory block pointers are buddies
static bool are_buddies(struct block_header client, struct block_header anon){
	size_t client_add;
	client_add = (size_t)client.kval - mem_pool.lgsize;
	return (mem_pool.lgsize + ((client_add ^ 1) << client.kval)) == (size_t)anon.prev;
}
// merges two buddies
static void merge(struct block_header client, struct block_header buddy){
	struct block_header main_block, sec_block;
	int curr_pow;

	if(buddy.kval < client.kval){
		main_block = buddy;
		sec_block = client;
	}else {
		main_block = client;
		sec_block = buddy;
	}

	curr_pow = main_block.kval;

	main_block.kval++;
	sec_block.kval++;

	remove_buddy(buddy, curr_pow);
	merge_with_buddy(main_block, main_block.kval);
}

// finds and either rearranges two memory blocks or merges them if they are buddies
// and both are free
static void	merge_with_buddy(struct block_header ptr, int best_fit_pow){
	struct block_header buddy = get_buddy(ptr, best_fit_pow);
	if(buddy.tag == UNUSED) {
	 rearrange(ptr, best_fit_pow);
    } else{ 
	merge(ptr, buddy);}
}
// returns the buddy memory block pointer for given block pointer
static struct block_header get_buddy(struct block_header ptr, int best_fit_pow){
	struct block_header next = mem_pool.avail[best_fit_pow];
	while(next.tag != UNUSED){
		if(are_buddies(ptr, next))
			return next;
		next.next = next.next;
	}
	return next;
}
// rearranges two memory blocks
static void rearrange(struct block_header client, int i){
	struct block_header curr_block;
	curr_block = mem_pool.avail[i];
	if(curr_block.tag == UNUSED){
		curr_block = client;
		client.next = NULL;
		client.prev = NULL;
		return;
	}

	while(curr_block.tag != UNUSED){
		if(client.kval < curr_block.kval){
			client.prev = curr_block.prev;
			//curr_block
			client.next = availPtr;
			if(curr_block.prev != NULL){
				//client
			curr_block.prev = availPtr;
			}else{
				mem_pool.avail[i] = client;
			}
			//client
			curr_block.prev = availPtr;
			return;
		}
		//curr_block
		availPtr = curr_block.next;
	}
	//client
	//parent_block
	client.prev = availPtr;
	client.next = NULL;
}
void *buddy_realloc(void* ptr, size_t size) 
{
	return NULL;
}

void buddy_free(void *obj) 
{
struct block_header ptr = mem_pool.avail[(sizeof(obj) - sizeof(struct block_header))];

	ptr.tag = UNUSED;
	ptr.prev = NULL;
	ptr.next = NULL;
	int curr_pow = ptr.kval;
	if(mem_pool.avail[curr_pow].tag == UNUSED)
	  mem_pool.avail[curr_pow] = ptr; 
	  else{merge_with_buddy(ptr, curr_pow);}
}
/* Required method for unit testing */
void print_buddy_lists(void)
{
	
}

/* Required method for unit testing */
int get_min_k(void) {
	// This would just give us the size of our block, plust the smallest amount of block space we can have and get its k
	size_t min_k = next_best_two(sizeof(struct block_header) + (1L<<19));
	
	return min_k;
}

/* Required method for unit testing */
int get_max_k(void) {
	return mem_pool.lgsize;
}

/**
 * This is a helper to return the lgsize of our 
 * desired memory size.
 * 
 * @param size - the size we requested
 */ 
int log_two(size_t size) {
	return (int)ceil(log(size)/log(2)); // This should always give us the ceiling of log base 2 of our size as an int
}

/**
 * This is a helper for determining the next biggest 
 * integer that is a power of two based on the size
 * we have requested
 * 
 * @param size - the size we requested
 * @return the next biggest power of two int
 */
int next_best_two(size_t size) {
	if (size < 2) 
	{
		return 0;
	}

	size_t next = 1;

	while (next < size)
	{
		next <<= 1; // Shift our bit over by one each time until we get to the next biggest size that's a power of 2
	}
	
	return log_two(next);
}

/**
 * Helper function for printing out an error if we need
 * to let the user know that we've encountered ENOMEM
 */
void mem_error() {
	fprintf(stderr, "The program has encountered a memory error, it will now quit");
	errno = ENOMEM;
	exit(EXIT_FAILURE);
}


/*
 *
 *  Definition for malloc et al preload wedge
 *    These are wrappers for the buddy versions
 *    of the cooresponding methods implemented 
 *    above.  This will be completed in part 2.
 */
#ifdef BUDDY_PRELOAD
void * malloc(size_t size) 
{
	return NULL;
}

void *calloc(size_t nmemb, size_t size) 
{
	return NULL;
}

void *realloc(void *ptr, size_t size)
{
	return NULL;
} 

void free(void *ptr) 
{
	return NULL;
}

#endif /* BUDDY_PRELOAD */

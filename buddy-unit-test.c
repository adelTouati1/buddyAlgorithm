/* Author: Luke Hindman
 * Date: Fri May  3 15:03:55 MDT 2019
 * Description:  Extension of the unit tests provided for the 
 *     Buddy memory manager project. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "buddy.h"


#define MAX_REQUEST 4096
#define MAX_ITEMS 100

#define SILENT 0
#define TERSE  1
#define VERBOSE  2
#define INTERACTIVE 3

#define MODE_MALLOC 0
#define MODE_CALLOC 1
#define MODE_REALLOC 2

#define TEST_SIZE 2048L * 1024L * 1024L

int verbosity = TERSE;


static void haltTest(void)
{
	fprintf(stderr,"Memory in inconsistent state, halting tests\n");
	exit(1);
}

static void printAndClear(void)
{
	print_buddy_lists();
	if (verbosity == INTERACTIVE) {
		char ch = getchar();
		if (system("clear")) {
			fprintf(stderr,"system: unable to clear screen\n");
		}
		if (ch == 'q')
			exit(0);
	}
}


void simpleFreeTest(int mode)
{
	char *x;
	
	// buddy_malloc one byte to make buddy system
	// split all the way down
	switch(mode) {

		case MODE_CALLOC:
		printf("Running simpleFreeTest with buddy_calloc()\n");
		x = (char *) buddy_calloc(1,sizeof(char));
		break;
		case MODE_REALLOC:
		printf("Running simpleFreeTest with buddy_realloc()\n");
		x = (char *) buddy_realloc(NULL,sizeof(char));
		break;
		default:
		printf("Running simpleFreeTest with buddy_malloc()\n");
		x = (char *) buddy_malloc(sizeof(char));
		break;
	}
	
	if (x == NULL) {
		fprintf(stderr,"ERROR! Buddy system failed to allocate 1 byte.\n");
		haltTest();
	}
	if (verbosity > 0) {
		printf("   Buddy system succeeding in allocating 1 byte. \n");
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after malloc'ing 1 byte.\n");
		printAndClear();
	}

	// buddy_free should make all the blocks merge back into one block
	buddy_free(x);
	if (verbosity > 0) {
		printf("   Buddy system succeeding in free'ing 1 byte. \n");
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after free'ing the block .\n");
		printAndClear();
	}
}

void multiByteFreeTest(int mode)
{
	char *x;
	char *y;
	char *z;
	
	// buddy_malloc multiple bytes to make buddy system
	// split multiple blocks all the way down and roll them back up again

	switch(mode) {

		case MODE_CALLOC:
		printf("Running multiByteFreeTest with buddy_calloc()\n");
		x = (char *) buddy_calloc(1,sizeof(char));
		y = (char *) buddy_calloc(1,sizeof(char));
		z = (char *) buddy_calloc(1,sizeof(char));
		break;
		case MODE_REALLOC:
		printf("Running multiByteFreeTest with buddy_realloc()\n");
		x = (char *) buddy_realloc(NULL,sizeof(char));
		y = (char *) buddy_realloc(NULL,sizeof(char));
		z = (char *) buddy_realloc(NULL,sizeof(char));
		break;
		default:
		printf("Running multiByteFreeTest with buddy_malloc()\n");
		x = (char *) buddy_malloc(sizeof(char));
		y = (char *) buddy_malloc(sizeof(char));
		z = (char *) buddy_malloc(sizeof(char));
		break;
	}

	if (x == NULL || y == NULL || z == NULL) {
		fprintf(stderr,"ERROR! Buddy system failed to allocate multiple bytes.\n");
		haltTest();
	}
	if (verbosity > 0) {
		printf("   Buddy system succeeding in allocating multiple bytes. \n");
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after malloc'ing multiple bytes.\n");
		printAndClear();
	}

	// buddy_free should make all the blocks merge back into one block
	buddy_free(x);
	buddy_free(y);
	buddy_free(z);

	if (verbosity > 0) {
		printf("   Buddy system succeeding in free'ing multiple bytes. \n");
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after free'ing the blocks .\n");
		printAndClear();
	}
}

void multiByteStringTest(int mode)
{
	int i;
	char * quote = "When you come to the end of your rope, tie a knot and hang on.";
	int num_chars = strlen(quote);
	char * char_bytes[num_chars];

	// split all the way down
	switch(mode) {
		case MODE_CALLOC:
		printf("Running multiByteStringTest with buddy_calloc()\n");
		break;
		case MODE_REALLOC:
		printf("Running multiByteStringTest with buddy_realloc()\n");
		break;
		default:
		printf("Running multiByteStringTest with buddy_malloc()\n");
		break;
	}
	
	if (verbosity > 1) printAndClear();

	if (verbosity > 0) printf("   allocating memory blocks\n");
	/* Allocate multiple bytes */
	if (verbosity > 0) printf("\t");
	for (i = 0; i < num_chars; i++) {
		if (verbosity > 0) printf(".");
		switch(mode) {
			case MODE_CALLOC:
			char_bytes[i] = (char *) buddy_calloc(1,sizeof(char));
			break;
			case MODE_REALLOC:
			char_bytes[i] = (char *) buddy_realloc(NULL,sizeof(char));
			break;
			default:
			char_bytes[i] = (char *) buddy_malloc(sizeof(char));
			break;
		}
	}
	if (verbosity > 0) printf("\n");
	if (verbosity > 1) printAndClear();

	if (verbosity > 0) printf("   storing characters into memory blocks\n");
	/* Store characters from quote into char array */
	if (verbosity > 0)printf("\t");
	for (i = 0; i < num_chars; i++) {
		if (verbosity > 0)printf(".");
		*char_bytes[i] = quote[i];
	}
	if (verbosity > 0)printf("\n");
	if (verbosity > 1) printAndClear();
	if (verbosity > 0) printf("   reading characters from memory blocks\n");
	if (verbosity > 0) printf("\tOriginal: %s\n", quote);
	/* Display the stored values */
	if (verbosity > 0) printf("\tBuddy Bytes: ");
	for (i=0; i < num_chars; i++) {
		if (verbosity > 0) printf("%c",*char_bytes[i]);
	}
	if (verbosity > 0) printf("\n");

	/* Free multiple bytes */
	if (verbosity > 0) printf("   releasing memory blocks\n");
	if (verbosity > 0) printf("\t");
	for (i = 0; i < num_chars; i++) {
		if (verbosity > 0) printf(".");
		buddy_free(char_bytes[i]);
	}
	if (verbosity > 0) printf("\n");
	if (verbosity > 1) printAndClear();

}

void largestBlockTest(int mode, size_t test_size)
{
	char *x;
	
	/* Allocate 70% of specified test size, this will evaluate to 
	 *   a kvalue that equals the max pool size assuming that size
	 *   is the same as the pool size. */
	test_size = (size_t) (test_size * 0.70);

	// buddy_malloc one byte to make buddy system
	// split all the way down
	switch(mode) {

		case MODE_CALLOC:
		printf("Running largestBlockTest with buddy_calloc()\n");
		x = (char *) buddy_calloc(test_size,sizeof(char));
		break;
		case MODE_REALLOC:
		printf("Running largestBlockTest with buddy_realloc()\n");
		x = (char *) buddy_realloc(NULL,test_size * sizeof(char));
		break;
		default:
		printf("Running largestBlockTest with buddy_malloc()\n");
		x = (char *) buddy_malloc(test_size * sizeof(char));
		break;
	}
	
	if (x == NULL) {
		fprintf(stderr,"ERROR! Buddy system failed to allocate %lu bytes.\n", test_size);
		haltTest();
	}
	if (verbosity > 0) {
		printf("   Buddy system succeeding in allocating %lu bytes. \n", test_size);
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after malloc'ing %lu bytes.\n", test_size);
		printAndClear();
	}

	// buddy_free should make all the blocks merge back into one block
	buddy_free(x);
	if (verbosity > 0) {
		printf("   Buddy system succeeding in free'ing %lu bytes. \n", test_size);
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after free'ing the block .\n");
		printAndClear();
	}
}

void maxAllocationTest(int mode)
{
	size_t i;
	int k;
	int max_k = get_max_k();
	int min_k = (max_k - get_min_k() < 16)?get_min_k():max_k-16;
	size_t max_tests = 1L<<(max_k - min_k);
	void *ptr[max_tests + 1];
	size_t num_tests;
	size_t test_size;
	
	switch(mode) {
		case MODE_CALLOC:
		printf("Running maxAllocationTest with buddy_calloc()\n");
		break;
		case MODE_REALLOC:
		printf("Running maxAllocationTest with buddy_realloc()\n");
		break;
		default:
		printf("Running maxAllocationTest with buddy_malloc()\n");
		break;
	}


	//buddy_malloc larger and larger blocks until memory is exhausted
	for (k=min_k; k<=max_k; k++) {
		num_tests = 1L<<(max_k - k);
		if (k == get_min_k()) {
			test_size = 1;
		} else {
			test_size = (size_t)((1L<<k) * 0.6);
		}
		for (i=0;i<=num_tests;i++) {
			switch(mode) {
				case MODE_CALLOC:
				ptr[i] = (char *) buddy_calloc(test_size,sizeof(char));
				break;
				case MODE_REALLOC:
				ptr[i] = (char *) buddy_realloc(NULL,test_size * sizeof(char));
				break;
				default:
				ptr[i] = (char *) buddy_malloc(test_size * sizeof(char));
				break;
			}

			if (ptr[i] == NULL) {
				if (i < num_tests) {
					perror("buddy_malloc()");
					printf("ERROR: maxAllocationTest() unable to allocate a block %lu of size %lu bytes\n", i , 1L<<k);
					if (verbosity > 1) printAndClear();
					haltTest();
				} else {
					if (verbosity > 0) 
						printf("   Success! buddy_malloc was unable to allocate %lu blocks of size %lu bytes\n", i+1 , 1L<<k);
					if (verbosity > 1) printAndClear();
				}
				break;
			} else {
				if (i == num_tests) {
					printf("ERROR: maxAllocationTest() allocated more blocks than max available: %lu, %lu\n",i,num_tests);
					haltTest();
				}
				
				if (verbosity > 1) printAndClear();
			}
		}
		if (verbosity > 0)  {
			printf("   buddy_malloc'd %lu blocks of size %lu\n", num_tests, 1L<<k);
			printf("   releasing memory blocks\n");
		}
		for (i=0; i < num_tests; i++) {
			if (ptr[i])
				buddy_free(ptr[i]);
		}
		if (verbosity > 1) {
				printAndClear();
		}
			
	}


}

void reallocAsFreeTest(void)
{
	char *x;
	
	printf("Running reallocAsFreeTest\n");
	x = (char *) buddy_malloc(sizeof(char));
	
	if (x == NULL) {
		fprintf(stderr,"ERROR! Buddy system failed to allocate 1 byte.\n");
		haltTest();
	}
	if (verbosity > 0) {
		printf("   Buddy system succeeding in allocating 1 byte. \n");
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after malloc'ing 1 byte.\n");
		printAndClear();
	}

	// buddy_free should make all the blocks merge back into one block
	buddy_realloc(x,0);
	if (verbosity > 0) {
		printf("   Buddy system succeeding in free'ing 1 byte. \n");
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after free'ing the block .\n");
		printAndClear();
	}
}

void reallocGrowTest(void)
{
	char *x;

	char * quote = "I don't know how many of you have ever met Dijkstra, but you probably know that arrogance in computer science is measured in nano-Dijkstras. --Alan kay";
	size_t test_size = (strlen(quote) + 1);
	printf("Running reallocGrowTest\n");
	x = (char *) buddy_malloc( test_size * sizeof(char));
	if (x == NULL) {
		fprintf(stderr,"ERROR! Buddy system failed to allocate %lu byte.\n", test_size);
		haltTest();
	}
	if (verbosity > 0) {
		printf("   Buddy system succeeding in allocating %lu byte. \n", test_size);
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after malloc'ing %lu byte.\n", test_size);
		printAndClear();
	}

	/* Copy the quote into the allocated memory and print it */
	strncpy(x,quote,test_size);
	if (verbosity > 0) printf("   Famous Quote: %s\n",x);

	/* Use realloc to quadruple the size of memory allocated.  This should
	 * increase the kvalue by at least 1 */
	test_size *= 4;
	x = buddy_realloc(x,test_size);
	if (x == NULL) {
		fprintf(stderr,"ERROR! Buddy system failed to allocate %lu bytes.\n", test_size);
		haltTest();
	}
	if (verbosity > 0) printf("   Realloc Quote: %s\n",x);

	if (verbosity > 0) {
		printf("   Buddy system succeeding in realloc'ing %lu bytes. \n", test_size);
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after realloc'ing the block .\n");
		printAndClear();
	}

	/* Free the block memory */
	buddy_free(x);
	if (verbosity > 0) {
		printf("   Buddy system succeeding in free'ing %lu bytes. \n", test_size);
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after free'ing the block .\n");
		printAndClear();
	}

}

void reallocShrinkTest(void)
{
	char *x;

	char * quote = "The real problem is that programmers have spent far too much time worrying about efficiency in the wrong places and at the wrong times; premature optimization is the root of all evil (or at least most of it) in programming. --Knuth";
	size_t test_size = (strlen(quote) + 1);
	printf("Running reallocShrinkTest\n");
	x = (char *) buddy_malloc( test_size * sizeof(char));
	if (x == NULL) {
		fprintf(stderr,"ERROR! Buddy system failed to allocate %lu byte.\n", test_size);
		haltTest();
	}
	if (verbosity > 0) {
		printf("   Buddy system succeeding in allocating %lu byte. \n", test_size);
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after malloc'ing %lu byte.\n", test_size);
		printAndClear();
	}

	/* Copy the quote into the allocated memory and print it */
	strncpy(x,quote,test_size);
	if (verbosity > 0)  printf("   Famous Quote: %s\n",x);

	/* Use realloc to x to 25% the original size of memory allocated.  This should
	 * decrease the kvalue by at least 1 */
	test_size /= 4;
	x = buddy_realloc(x,test_size);
	if (x == NULL) {
		fprintf(stderr,"ERROR! Buddy system failed to allocate %lu bytes.\n", test_size);
		haltTest();
	}
	/* We reduced the size of our string, so we lost the '\0' terminator */
	x[test_size-1] = '\0';
	if (verbosity > 0) printf("   Realloc Quote: %s\n",x);

	if (verbosity > 0) {
		printf("   Buddy system succeeding in realloc'ing %lu bytes. \n", test_size);
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after realloc'ing the block .\n");
		printAndClear();
	}

	/* Free the block memory */
	buddy_free(x);
	if (verbosity > 0) {
		printf("   Buddy system succeeding in free'ing %lu bytes. \n", test_size);
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after free'ing the block .\n");
		printAndClear();
	}

}

void doubleFreeTest(void)
{
	char *x;
	
	printf("Running doubleFreeTest with buddy_malloc()\n");
	x = (char *) buddy_malloc(sizeof(char));

	if (x == NULL) {
		fprintf(stderr,"ERROR! Buddy system failed to allocate 1 byte.\n");
		haltTest();
	}
	if (verbosity > 0) {
		printf("   Buddy system succeeding in allocating 1 byte. \n");
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after malloc'ing 1 byte.\n");
		printAndClear();
	}

	// buddy_free should make all the blocks merge back into one block
	buddy_free(x);
	if (verbosity > 0) {
		printf("   Buddy system succeeding in free'ing 1 byte. \n");
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after free'ing the block .\n");
		printAndClear();
	}

	buddy_free(x);
	if (verbosity > 0) {
		printf("   Buddy system handled a double free without crashing. \n");
	}

}

void nullFreeTest(void)
{
	printf("Running nullFreeTest with buddy_malloc()\n");

	buddy_free(NULL);
	if (verbosity > 0) {
		printf("   Buddy system handled a NULL free without crashing. \n");
	}

}


void reallocInplaceTest(void)
{
	char *x, *new_x;

	char * quote = "People think that computer science is the art of geniuses but the actual reality is the opposite, just many people doing things that build on each other, like a wall of mini stones. --Knuth";
	size_t test_size = (strlen(quote) + 1);
	printf("Running reallocInplaceTest\n");
	x = (char *) buddy_malloc( test_size * sizeof(char));
	if (x == NULL) {
		fprintf(stderr,"ERROR! Buddy system failed to allocate %lu byte.\n", test_size);
		haltTest();
	}
	if (verbosity > 0) {
		printf("   Buddy system succeeding in allocating %lu byte. \n", test_size);
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after malloc'ing %lu byte.\n", test_size);
		printAndClear();
	}

	/* Copy the quote into the allocated memory and print it */
	strncpy(x,quote,test_size);
	if (verbosity > 0) printf("   Famous Quote: %s\n",x);

	/* Use realloc to increase the size of memory allocated.  This should
	 * result in no change to kvalue and as a result, no change in the allocated
	 * block of memory (IE: The pointer x should not change) */
	test_size = (int) (test_size * 1.10);
	new_x = buddy_realloc(x,test_size);
	if (new_x == NULL) {
		fprintf(stderr,"ERROR! Buddy system failed to allocate %lu bytes.\n", test_size);
		haltTest();
	}
	if (verbosity > 0) printf("   Realloc Quote: %s\n",new_x);

	if (new_x != x) {
		printf("   Hmm... The grow operation resulted in a new block being allocated... Better check into this more\n");
	} else {
		if (verbosity > 0) printf("   Realloc made no changes to the block allocation (as expected)\n");
	}

	if (verbosity > 0) {
		printf("   Buddy system succeeding in realloc'ing %lu bytes. \n", test_size);
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after realloc'ing the block .\n");
		printAndClear();
	}

	/* Free the block memory */
	buddy_free(new_x);
	if (verbosity > 0) {
		printf("   Buddy system succeeding in free'ing %lu bytes. \n", test_size);
	}
	if (verbosity > 1) {
		printf("   Buddy system lists after free'ing the block .\n");
		printAndClear();
	}

}





int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage: %s {silent|terse|verbose|interactive}\n", argv[0]);
		exit(1);
	}
	if (argv[1][0] == 's') {
		verbosity = SILENT;
	} else if (argv[1][0] == 't') {
		verbosity = TERSE;
	} else if (argv[1][0] == 'v') {
		verbosity = VERBOSE;
	} else if (argv[1][0] == 'i') {
		verbosity = INTERACTIVE;
		setvbuf(stdin, NULL, _IONBF, 0);
	}
	 
	if (system("clear")) {
		fprintf(stderr,"system: unable to clear screen\n");
	}

	buddy_init(TEST_SIZE);	
	if (verbosity > 0) {
		printf("Buddy system initialized.\n");
	}
	if (verbosity > 1) {
		printf("Buddy system lists after initialization.\n");
		printAndClear();
	}

	/* test buddy_malloc() */	
	simpleFreeTest(MODE_MALLOC);
	multiByteFreeTest(MODE_MALLOC);
	multiByteStringTest(MODE_MALLOC);
	largestBlockTest(MODE_MALLOC, TEST_SIZE);
	maxAllocationTest(MODE_MALLOC);

	/* test buddy_calloc() */
	simpleFreeTest(MODE_CALLOC);
	multiByteFreeTest(MODE_CALLOC);
	multiByteStringTest(MODE_CALLOC);
	largestBlockTest(MODE_CALLOC, TEST_SIZE);
	maxAllocationTest(MODE_CALLOC);

	/* test buddy_realloc() */
	simpleFreeTest(MODE_REALLOC);
	multiByteFreeTest(MODE_REALLOC);
	multiByteStringTest(MODE_REALLOC);
	largestBlockTest(MODE_REALLOC, TEST_SIZE);
	maxAllocationTest(MODE_REALLOC);
	reallocAsFreeTest();
	reallocGrowTest();
	reallocShrinkTest();
	reallocInplaceTest();
	
	/* Free tests */
	doubleFreeTest();
	nullFreeTest();
	

	exit(0);	
}


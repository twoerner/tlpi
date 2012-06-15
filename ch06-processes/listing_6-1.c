/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program shows various types of C variables along with comments indicating in
 * which segment each variable is located. These comments assume a nonoptimizing
 * compiler and an ABI in which all arguments are passed on the stack.
 */

#include <stdio.h>
#include <stdlib.h>

char globBuf[65536];		// uninitialized data segment
int primes[] = {2, 3, 5, 7};	// initialized data segment

static int
square (int x)			// allocated in frame for square()
{
	int result;		// allocated in frame for square()

	result = x * x;
	return result;		// return value passed via register
}

static void
do_calc (int val)		// allocated in frame for do_calc()
{
	printf ("the square of %d is %d\n", val, square (val));

	if (val < 1000) {
		int t;		// allcoated in frame for do_calc()

		t = val * val * val;
		printf ("the cube of %d is %d\n", val, t);
	}
}

int
main (void)			// allocated in frame for main()
{
	static int key = 9973;	// initialized data segment
	static char mbuf[10240000]; // uninitialized data segment
	char *ptr;		// allocated in frame for main()

	ptr = malloc (1024);	// points to memory in heap segment

	do_calc (key);

	return 0;
}

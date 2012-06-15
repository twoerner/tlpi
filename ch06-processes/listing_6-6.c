/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the problems that can occur when using setjmp()/longjmp()
 * with an optimizing compiler. When we compile the program normally, we see the expected output:
 *
 *	$ cc -o listing_6-6 listing_6-6.c
 *	$ ./listing_6-6
 *	inside do_jump(): nvar=777 rvar=888 vvar=999
 *	after longjmp():  nvar=777 rvar=888 vvar=999
 *
 * However, when we compile with optimizations, we get the following unexpected result:
 *
 *	$ cc -O -o listing_6-6 listing_6-6.c
 *	$ ./listing_6-6
 *	inside do_jump(): nvar=777 rvar=888 vvar=999
 *	after longjmp():  nvar=111 rvar=222 vvar=999
 */

#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static jmp_buf env_G;

static void
do_jump (int nvar, int rvar, int vvar)
{
	printf ("inside do_jump(): nvar=%d rvar=%d vvar=%d\n", nvar, rvar, vvar);
	longjmp (env_G, 1);
}

int
main (void)
{
	int nvar;
	register int rvar;
	volatile int vvar;

	nvar = 111;
	rvar = 222;
	vvar = 333;

	if (setjmp (env_G) == 0) {
		nvar = 777;
		rvar = 888;
		vvar = 999;
		do_jump (nvar, rvar, vvar);
	}
	else
		printf ("after longjmp():  nvar=%d rvar=%d vvar=%d\n", nvar, rvar, vvar);

	return 0;
}

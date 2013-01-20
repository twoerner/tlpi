/*
 * Copyright (C) 2013  Trevor Woerner <twoerner@gmail.com>
 */

/*
 * In Section 41.12 it is pointed out that if a symbol is defined in
 * both a relocatable object (executable) and a shared object, then the
 * symbol in the relocatable object is preferred EVEN IF it gets called
 * from within the shared object(!).
 *
 * However, Section 42.1.6 points out that if the shared object is loaded
 * dynamically at run time, then the symbol from the shared object would be
 * called instead of the one from the relocatable object UNLESS the
 * -export-dynamic (or -rdynamic) option is passed to gcc.
 *
 * This doodle asertains the truth behind the first statement.
 */

#include <stdio.h>

void xyz (void);

void
myfunc (void)
{
	printf ("main myfunc\n");
}

int
main (void)
{
	printf ("calling library\n");
	xyz ();
	printf ("\ncalling myfunc() directly\n");
	myfunc ();

	return 0;
}

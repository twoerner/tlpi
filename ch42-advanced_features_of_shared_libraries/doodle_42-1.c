/*
 * Copyright (C) 2012  Trevor Woerner <twoerner@gmail.com>
 */

#include <stdio.h>

void myfunc (int);

int
main (void)
{
	printf ("calling myfunc()\n");
	myfunc (42);
	printf ("done\n");

	return 0;
}

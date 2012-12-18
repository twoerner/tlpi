/*
 * Copyright (C) 2012  Trevor Woerner <twoerner@gmail.com>
 */

#include <stdio.h>

__asm__(".symver myfunc,myfunc@VER_1");
void myfunc (void);

int
main (void)
{
	printf ("calling myfunc()\n");
	myfunc ();
	printf ("done\n");

	return 0;
}

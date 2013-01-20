/*
 * Copyright (C) 2013  Trevor Woerner <twoerner@gmail.com>
 */

#include <stdio.h>

void
myfunc (void)
{
	printf ("library myfunc\n");
}

void
xyz (void)
{
	myfunc ();
}

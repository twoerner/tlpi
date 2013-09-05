/*
 * Copyright (C) 2013  Trevor Woerner <twoerner@gmail.com>
 */

#include <stdio.h>

int my_func (int, int);

int
main (void)
{
	printf ("calling my_func (4, 9) yields: %d\n", my_func (4, 9));
	return 0;
}

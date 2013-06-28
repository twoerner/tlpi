/*
 * Copyright (C) 2013  Trevor Woerner <twoerner@gmail.com>
 */

/*
 * When performing static linking, is an entire module loaded from the
 * archive, or just the functions that are used?
 */

#include <stdio.h>

void func1_1 (void);

int
main (void)
{
	func1_1 ();
	return 0;
}

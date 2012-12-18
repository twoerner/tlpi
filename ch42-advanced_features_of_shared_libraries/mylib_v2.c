/*
 * Copyright (C) 2012  Trevor Woerner <twoerner@gmail.com>
 */

#include <stdio.h>

__asm__(".symver myfunc_v1, myfunc@VER_1");
void
myfunc_v1 (void)
{
	printf ("this is the V1 function\n");
}

__asm__(".symver myfunc_v2, myfunc@@VER_2");
void
myfunc_v2 (int a)
{
	printf ("this is the V2 function (%d)\n", a);
}

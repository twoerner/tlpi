/*
 * Copyright (C) 2012  Trevor Woerner
 */

#include <stdio.h>

void x2 (void);

void
x1 (void)
{
	printf ("%s starting\n", __func__);
	x2 ();
	printf ("%s ending\n", __func__);
}

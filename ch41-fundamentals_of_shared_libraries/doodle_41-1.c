/*
 * Copyright (C) 2012  Trevor Woerner
 */

#include <stdio.h>

void x1 (void);
void x2 (void);

int
main (void)
{
	printf ("%s starting\n", __func__);
	x1 ();
	printf ("%s ending\n", __func__);
	return 0;
}

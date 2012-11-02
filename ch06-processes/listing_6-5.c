/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates the use of setjmp() and longjmp().
 */

#include <stdio.h>
#include <setjmp.h>

static jmp_buf env_G;

static void
f2 (void)
{
	longjmp (env_G, 2);
}

static void
f1 (int argc)
{
	if (argc == 1)
		longjmp (env_G, 1);
	f2 ();
}

int
main (int argc, char *argv[])
{
	switch (setjmp (env_G)) {
		case 0:
			printf ("calling f1() after initial setjmp()\n");
			f1 (argc);
			break;

		case 1:
			printf ("jumped back from f1()\n");
			break;

		case 2:
			printf ("jumped back from f2()\n");
			break;
	}

	return 0;
}

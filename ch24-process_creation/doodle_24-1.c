/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

#include <stdio.h>
#include <unistd.h>

// set to 0 or 1
#define FORGET_EXIT 0

int
main (void)
{
	pid_t pid;

	printf ("parent\n");
	pid = vfork();

	if (pid == 0) {
		printf ("child\n");
#if (FORGET_EXIT == 0)
		_exit (0);
#endif
	}
	return 0;
}

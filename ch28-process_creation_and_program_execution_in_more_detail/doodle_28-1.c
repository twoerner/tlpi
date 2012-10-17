/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Does a non-thread have a thread ID or thread group ID?
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>

pid_t gettid (void);

int
main (void)
{
	printf ("pid:  %ld\n", (long)getpid ());
	printf ("tid:  %ld\n", (long)syscall (SYS_gettid));

	return 0;
}

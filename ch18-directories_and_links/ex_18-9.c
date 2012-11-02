/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * In section 18.10 we showed two different techniques (using fchdir() and
 * chdir() respectively) to return to the previous current working directory
 * after changing the current working directory to another location. Suppose
 * we are performing such an operation repeatedly. Which method do you expect
 * to be the more efficient? Why? Write a program to confirm your answer.
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>

static void using_fchdir (void);
static void using_chdir (void);

int
main (void)
{
	unsigned i;
	clock_t start, end;

	start = clock ();
	for (i=0; i<1000000; ++i)
		using_fchdir ();
	end = clock ();
	printf ("using fchdir: %10ld\n", (long)end - start);

	start = clock ();
	for (i=0; i<1000000; ++i)
		using_chdir ();
	end = clock ();
	printf ("using  chdir: %10ld\n", (long)end - start);

	return 0;
}

static void
using_fchdir (void)
{
	int fd;

	fd = open ("/", O_RDONLY);
	chdir ("/");
	fchdir (fd);
	close (fd);
}

static void
using_chdir (void)
{
	char buf[PATH_MAX];

	getcwd (buf, PATH_MAX);
	chdir ("/");
	chdir (buf);
}

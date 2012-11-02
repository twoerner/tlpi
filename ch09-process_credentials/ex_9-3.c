/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Implement initgroups() using getgroups() and library functions for retrieving
 * information from the passwd and group files (Section 8.4). Remember that a
 * process must be priviledged in order to be able to call setgroups().
 */

#define _BSD_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int
main (void)
{
	int i;
	int grpSz;
	gid_t *grpList_p;

	grpSz = getgroups (0, NULL);
	if (grpSz == -1) {
		perror ("getgroups()");
		return 1;
	}
	grpList_p = malloc (grpSz * sizeof (gid_t));
	if (grpList_p == NULL) {
		perror ("malloc()");
		return 1;
	}

	grpSz = getgroups (grpSz, grpList_p);
	for (i=0; i<grpSz; ++i)
		printf ("%ld ", (long)grpList_p[i]);
	printf ("\n");

	free (grpList_p);
	return 0;
}

/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program uses setpriority() to change the nice value of the process(es)
 * specified by its cmdline args, and then calls getpriority to verify the
 * change.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>

int
main (int argc, char *argv[])
{
	int which, prio;
	id_t who;

	if (argc != 4 || strchr ("pgu", argv[1][0]) == NULL) {
		printf ("usage: %s {p|g|u} <who> <priority>\n", argv[0]);
		printf ("  where:\n");
		printf ("    p    <who> is a pid\n");
		printf ("    g    <who> is a pgid\n");
		printf ("    u    <who> is a user id\n");
		return 1;
	}

	which = (argv[1][0] == 'p')? PRIO_PROCESS :
		(argv[1][0] == 'g')? PRIO_PGRP :
		PRIO_USER;
	who = (id_t)atol (argv[2]);
	prio = atoi (argv[3]);

	if (setpriority (which, who, prio) == -1) {
		perror ("setpriority()");
		return 1;
	}

	// verify
	errno = 0;
	prio = getpriority (which, who);
	if (prio == -1 && errno != 0) {
		perror ("getpriority()");
		return 1;
	}

	printf ("nice value: %d\n", prio);
	return 0;
}

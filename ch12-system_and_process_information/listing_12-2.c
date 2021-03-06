/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program displays the information returned by uname().
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <sys/utsname.h>

int
main (void)
{
	struct utsname uts;

	if (uname (&uts) == -1) {
		perror ("uname()");
		return 1;
	}

	printf ("node name:    %s\n", uts.nodename);
	printf ("system name:  %s\n", uts.sysname);
	printf ("release:      %s\n", uts.release);
	printf ("version:      %s\n", uts.version);
	printf ("machine:      %s\n", uts.machine);
#ifdef _GNU_SOURCE
	printf ("domain name:  %s\n", uts.domainname);
#endif

	return 0;
}

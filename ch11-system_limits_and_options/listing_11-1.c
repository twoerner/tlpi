/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates the use of sysconf() to display
 * various system limits.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

static void
sysconf_print (const char *msg_p, int name)
{
	long lim;

	errno = 0;
	lim = sysconf (name);
	if (lim != -1)
		printf ("%s %ld\n", msg_p, lim);
	else {
		if (errno == 0)
			printf ("%s (indeterminate)\n", msg_p);
		else {
			perror ("sysconf()");
			exit (1);
		}
	}
}

int
main (void)
{
	sysconf_print ("_SC_ARG_MAX:             ", _SC_ARG_MAX);
	sysconf_print ("_SC_LOGIN_NAME_MAX:      ", _SC_LOGIN_NAME_MAX);
	sysconf_print ("_SC_OPEN_MAX:            ", _SC_OPEN_MAX);
	sysconf_print ("_SC_NGROUPS_MAX:         ", _SC_NGROUPS_MAX);
	sysconf_print ("_SC_PAGESIZE:            ", _SC_PAGESIZE);
	sysconf_print ("_SC_RTSIG_MAX:           ", _SC_RTSIG_MAX);

	return 0;
}

/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates the user of asctime(), as well as of the time-conversion
 * functions described so far. It retrieves the current calendar time, and then uses
 * various time-conversion functions and displays their results.
 */

#include <stdio.h>
#include <locale.h>
#include <time.h>
#include <sys/time.h>

#define SECONDS_IN_TROPICAL_YEAR (365.24219 * 24 * 60 * 60)

int
main (void)
{
	time_t t;
	struct tm *gm_p, *loc_p;
	struct tm gm, loc;
	struct timeval tv;

	t = time (NULL);
	printf ("seconds since epoch (1 Jan 1970): %ld", (long)t);
	printf (" (about %6.3f years)\n", t / SECONDS_IN_TROPICAL_YEAR);

	if (gettimeofday (&tv, NULL) == -1) {
		perror ("gettimeofday()");
		return 1;
	}
	printf (" gettimeofday() returned          %ld secs, %ld microsecs\n", (long)tv.tv_sec, (long)tv.tv_usec);

	gm_p = gmtime (&t);
	if (gm_p == NULL) {
		perror ("gmtime()");
		return 1;
	}
	gm = *gm_p;
	printf ("broken-down by gmtime():\n");
	printf ("  year=%d mon=%d mday=%d hour=%d min=%d sec=%d ",
			gm.tm_year, gm.tm_mon, gm.tm_mday, gm.tm_hour, gm.tm_min, gm.tm_sec);
	printf ("wday=%d yday=%d isdst=%d\n", gm.tm_wday, gm.tm_yday, gm.tm_isdst);

	loc_p = localtime (&t);
	if (loc_p == NULL) {
		perror ("localtime()");
		return 1;
	}
	loc = *loc_p;
	printf ("broken-down by localtime():\n");
	printf ("  year=%d mon=%d mday=%d hour=%d min=%d sec=%d ",
			loc.tm_year, loc.tm_mon, loc.tm_mday, loc.tm_hour, loc.tm_min, loc.tm_sec);
	printf ("wday=%d yday=%d isdst=%d\n\n", loc.tm_wday, loc.tm_yday, loc.tm_isdst);

	printf ("ascimte() formats gmtime() as: %s", asctime (&gm));
	printf ("ctime()   formats time()   as: %s", ctime (&t));
	printf ("mktime() of gmtime() value:    %ld secs\n", (long)mktime (&gm));
	printf ("mktime() of localtime value:   %ld secs\n", (long)mktime (&loc));

	return 0;
}

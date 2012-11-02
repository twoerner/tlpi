/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program shows the effect of setting the TZ environment variable.
 *
 *	$ ./listing_10-4
 *	$ TZ=":Pacific/Auckland" ./listing_10-4
 *	$ LANG=de_DE ./listing_10-4
 *	$ LANG=de_DE LC_TIME=it_IT ./listing_10-4
 *	$ LC_ALL=fr_FR ./listing_10-4
 */

#include <stdio.h>
#include <time.h>
#include <locale.h>

#define BUF_SIZE 200

int
main (void)
{
	time_t t;
	struct tm *loc_p;
	char buf[BUF_SIZE];

	if (setlocale (LC_ALL, "") == NULL) {
		perror ("locale()");
		return 1;
	}

	t = time (NULL);
	printf ("ctime() of time() value is:   %s", ctime (&t));

	loc_p = localtime (&t);
	if (loc_p == NULL) {
		perror ("localtime()");
		return 1;
	}
	printf ("asctime() of localtime() is:  %s", asctime (loc_p));

	if (strftime (buf, BUF_SIZE, "%A, %d %B %Y, %T %Z", loc_p) == 0) {
		perror ("strftime()");
		return 1;
	}
	printf ("strftime() of localtime() is: %s\n", buf);

	return 0;
}

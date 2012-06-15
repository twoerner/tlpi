/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the use of strptime() and strftime(). This program takes a
 * cmdline arg containing a date and time, converts it to a broken-down
 * time using strptime(), and then displays the result of performing the reverse conversion
 * using strftime(). It takes up to 3 args, of which the first two are required.
 * The first arg is the string containing a date and time.
 * The second arg is the format specification to be used by strptime() to parse the first arg.
 * The optional third arg is the format string to be used by strftime() for the reverse conversion.
 *
 *	$ ./listing_10-3 "9:39:46pm 1 Feb 2011" "%I:%M:%S%p %d %b %Y"
 *	$ ./listing_10-3 "9:39:46pm 1 Feb 2011" "%I:%M:%S%p %d %b %Y" "%F %T"
 */

#define _XOPEN_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>

#define SBUF_SIZE 1000

int
main (int argc, char *argv[])
{
	struct tm tm;
	char sbuf[SBUF_SIZE];
	char *ofmt_p;

	if (argc < 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <time string> <time format string of first arg> <time format string for output>\n", argv[0]);
		return 1;
	}

	if (setlocale (LC_ALL, "") == NULL) {
		perror ("setlocale()");
		return 1;
	}

	memset (&tm, 0, sizeof (tm));
	if (strptime (argv[1], argv[2], &tm) == NULL) {
		perror ("strptime()");
		return 1;
	}

	// tells mktime() to determine if DST is in effect
	tm.tm_isdst = -1;
	printf ("calendar time:     %ld\n", (long)mktime (&tm));

	ofmt_p = argc > 3? argv[3] : "%T %A, %d %B %Y %Z";
	if (strftime (sbuf, SBUF_SIZE, ofmt_p, &tm) == 0) {
		perror ("strftime()");
		return 1;
	}
	printf ("strftime() yields: %s\n", sbuf);

	return 0;
}

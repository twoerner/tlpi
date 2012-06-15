/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * A demonstration program to display the current time as a string
 * formatted with the provided time format string.
 */

#include <stdio.h>
#include <time.h>

#define BUF_SIZE 1000

char *curr_time (const char *format_p);

int
main (int argc, char *argv[])
{
	if (argc != 2) {
		printf ("usage: %s <time format string>\n", argv[0]);
		return 1;
	}

	printf ("curr_time of '%s' is: '%s'\n", argv[1], curr_time (argv[1]));
	return 0;
}

char *
curr_time (const char *format_p)
{
	static char buf[BUF_SIZE];
	time_t t;
	size_t s;
	struct tm *tm_p;

	t = time (NULL);
	tm_p = localtime (&t);
	if (tm_p == NULL)
		return NULL;

	s = strftime (buf, BUF_SIZE, format_p?:"%c", tm_p);
	if (s == 0)
		return NULL;
	return buf;
}

/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Demonstrates the use oft he timerfd API.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/timerfd.h>

static void
itimerspec_from_str (char *str_p, struct itimerspec *ts_p)
{
	char *cptr, *sptr;

	cptr = strchr (str_p, ':');
	if (cptr != NULL)
		*cptr = '\0';

	sptr = strchr (str_p, '/');
	if (sptr != NULL)
		*sptr = '\0';

	ts_p->it_value.tv_sec = atoi (str_p);
	ts_p->it_value.tv_nsec = (sptr != NULL)? atoi (sptr + 1) : 0;

	if (cptr == NULL) {
		ts_p->it_interval.tv_sec = 0;
		ts_p->it_interval.tv_nsec = 0;
	}
	else {
		sptr = strchr (cptr + 1, '/');
		if (sptr != NULL)
			*sptr = '\0';
		ts_p->it_interval.tv_sec = atoi (cptr + 1);
		ts_p->it_interval.tv_nsec = (sptr != NULL)? atoi (sptr + 1) : 0;
	}
}

int
main (int argc, char *argv[])
{
	struct itimerspec ts;
	struct timespec start, now;
	int maxExp, fd, secs, nanosecs;
	uint64_t numExp, totalExp;
	ssize_t sz;

	if (argc < 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <secs>[/<nano secs>][:<interval secs>[/<interval nanosecs>]] [<max-exp>]\n", argv[0]);
		return 1;
	}

	itimerspec_from_str (argv[1], &ts);
	maxExp = (argc > 2)? atoi (argv[2]) : 1;
	if (maxExp < 1) {
		fprintf (stderr, "please use a positive value for <max-exp>\n");
		return 1;
	}

	fd = timerfd_create (CLOCK_REALTIME, 0);
	if (fd == -1) {
		perror ("timerfd_create()");
		return 1;
	}

	if (timerfd_settime (fd, 0, &ts, NULL) == -1) {
		perror ("timerfd_settime()");
		return 1;
	}

	if (clock_gettime (CLOCK_MONOTONIC, &start) == -1) {
		perror ("clock_gettime()");
		return 1;
	}

	for (totalExp=0; totalExp<(uint64_t)maxExp; ) {
		sz = read (fd, &numExp, sizeof (uint64_t));
		if (sz != sizeof (uint64_t)) {
			perror ("read()");
			return 1;
		}

		totalExp += numExp;

		if (clock_gettime (CLOCK_MONOTONIC, &now) == -1) {
			perror ("clock_gettime()");
			return 1;
		}

		secs = now.tv_sec - start.tv_sec;
		nanosecs = now.tv_nsec - start.tv_nsec;
		if (nanosecs < 0) {
			--secs;
			nanosecs += 1000000000;
		}

		printf ("%d.%03d:\n", secs, (nanosecs + 500000) / 1000000);
		printf ("\texpirations read: %llu  total: %llu\n", (unsigned long long)numExp, (unsigned long long)totalExp);
	}

	return 0;
}

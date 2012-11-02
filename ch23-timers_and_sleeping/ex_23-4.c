/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Modify the program in Listing 23-5 to use sigwaitinfo() instead of a
 * signal handler.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#define UNUSED __attribute__((unused))
#define BUF_SIZE 1024
#define TIMER_SIG SIGRTMAX

static char *
current_time (const char *format_p)
{
	static char buf[BUF_SIZE];
	time_t tm;
	size_t sz;
	struct tm *tm_p;

	tm = time (NULL);
	tm_p = localtime (&tm);
	if (tm_p == NULL)
		return NULL;

	sz = strftime (buf, BUF_SIZE, (format_p != NULL)? format_p : "%c", tm_p);
	return (sz == 0)? NULL : buf;

}

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
	struct sigevent sevent;
	timer_t *tidlist_p;
	int i;
	sigset_t sigSet;

	if (argc < 2) {
		printf ("usage: %s <secs>[/<nsecs>][:<interval secs>[/<interval nsecs>]]...\n", argv[0]);
		return 0;
	}

	tidlist_p = calloc (argc - 1, sizeof (timer_t));
	if (tidlist_p == NULL) {
		perror ("malloc()");
		return 1;
	}

	sevent.sigev_notify = SIGEV_SIGNAL;
	sevent.sigev_signo = TIMER_SIG;

	sigemptyset (&sigSet);
	sigaddset (&sigSet, TIMER_SIG);
	sigprocmask (SIG_BLOCK, &sigSet, NULL);

	for (i=0; i<argc-1; ++i) {
		itimerspec_from_str (argv[i+1], &ts);
		sevent.sigev_value.sival_ptr = &tidlist_p[i];

		if (timer_create (CLOCK_REALTIME, &sevent, &tidlist_p[i]) == -1) {
			perror ("timer_create()");
			return 1;
		}
		printf ("timerID: %ld (%s)\n", (long)tidlist_p[i], argv[i+1]);

		if (timer_settime (tidlist_p[i], 0, &ts, NULL) == -1) {
			perror ("timer_settime()");
			return 1;
		}
	}

	while (1) {
		int sig;
		siginfo_t sigInfo;
		timer_t *tid_p;

		sig = sigwaitinfo (&sigSet, &sigInfo);
		if (sig == -1) {
			perror ("sigwaitinfo()");
			return 1;
		}

		tid_p = sigInfo.si_value.sival_ptr;
		printf ("[%s] got signal %d\n", current_time ("%T"), sig);
		printf ("\t*sival_ptr         = %ld\n", (long)*tid_p);
		printf ("\ttimer_getoverrun() = %d\n", timer_getoverrun (*tid_p));
	}

	return 0;
}

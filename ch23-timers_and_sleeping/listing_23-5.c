/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Demonstrate the use of signals as a notification mechanism
 * for a POSIX timer.
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

// use of printf() is UNSAFE
// use of current_time() is UNSAFE: not reentrant (static variable)
static void
signal_handler (int sig, siginfo_t *sigInfo_p, UNUSED void *uc)
{
	timer_t *tid_p;

	tid_p = sigInfo_p->si_value.sival_ptr;
	printf ("[%s] got signal %d\n", current_time ("%T"), sig);
	printf ("\t*sival_ptr         = %ld\n", (long)*tid_p);
	printf ("\ttimer_getoverrun() = %d\n", timer_getoverrun (*tid_p));
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
	struct sigaction sa;
	struct sigevent sevent;
	timer_t *tidlist_p;
	int i;

	if (argc < 2) {
		printf ("usage: %s <secs>[/<nsecs>][:<interval secs>[/<interval nsecs>]]...\n", argv[0]);
		return 0;
	}

	tidlist_p = calloc (argc - 1, sizeof (timer_t));
	if (tidlist_p == NULL) {
		perror ("malloc()");
		return 1;
	}

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = signal_handler;
	sigaction (TIMER_SIG, &sa, NULL);

	sevent.sigev_notify = SIGEV_SIGNAL;
	sevent.sigev_signo = TIMER_SIG;

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

	while (1)
		pause ();

	return 0;
}

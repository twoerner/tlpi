/*
 * Copyright (C) 2013  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Demonstrate how to use sleep(3) and clock_settimer(2) together in one
 * program. Note that "SA_RESTART" doesn't work on sleep(3).
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
signal_handler (UNUSED int sig, siginfo_t *sigInfo_p, UNUSED void *uc)
{
	int val;

	val = sigInfo_p->si_value.sival_int;
	printf ("[%s] %d\n", current_time ("%T"), val);
}

int
main (void)
{
	struct itimerspec ts;
	struct sigaction sa;
	struct sigevent sevent;
	timer_t timerid;
	int ret;

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = SA_RESTART | SA_SIGINFO;
	sa.sa_sigaction = signal_handler;
	sigaction (TIMER_SIG, &sa, NULL);

	memset (&sevent, 0, sizeof (sevent));
	sevent.sigev_notify = SIGEV_SIGNAL;
	sevent.sigev_signo = TIMER_SIG;

	// 2
	memset (&ts, 0, sizeof (ts));
	ts.it_value.tv_sec = 2;
	ts.it_interval.tv_sec = 2;
	sevent.sigev_value.sival_int = 2;
	if (timer_create (CLOCK_MONOTONIC, &sevent, &timerid) == -1) {
		perror ("timer_create(2)");
		return 1;
	}
	if (timer_settime (timerid, 0, &ts, NULL) == -1) {
		perror ("timer_settime()");
		return 1;
	}

	// 3
	memset (&ts, 0, sizeof (ts));
	ts.it_value.tv_sec = 3;
	ts.it_interval.tv_sec = 3;
	sevent.sigev_value.sival_int = 3;
	if (timer_create (CLOCK_MONOTONIC, &sevent, &timerid) == -1) {
		perror ("timer_create(2)");
		return 1;
	}
	if (timer_settime (timerid, 0, &ts, NULL) == -1) {
		perror ("timer_settime()");
		return 1;
	}

	while (1) {
		printf ("[%s] before sleep 5\n", current_time ("%T"));
		ret = 5;
		do {
			ret = sleep (ret);
			if (ret == 0)
				printf ("[%s] after  sleep 5\n", current_time ("%T"));
			else
				printf ("-> sleep ret:%d\n", ret);
		} while (ret != 0);
	}

	return 0;
}

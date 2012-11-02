/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Although alarm() is implemented as a system call within the Linux kernel,
 * this is redundant. Implement alarm() using setitimer().
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

// set to 0 or 1
#define USE_MY_ALARM 1

#if (USE_MY_ALARM == 1)
static unsigned
my_alarm (unsigned int seconds)
{
	struct itimerval newVal, oldVal;

	memset (&newVal, 0, sizeof (newVal));
	if (seconds != 0)
		newVal.it_value.tv_sec = seconds;

	setitimer (ITIMER_REAL, &newVal, &oldVal);

	if (oldVal.it_value.tv_usec >= 500000)
		return oldVal.it_value.tv_sec + 1;
	return oldVal.it_value.tv_sec;
}
#endif

static void
signal_handler (int sig)
{
	printf ("received signal %d (%s)\n", sig, strsignal (sig)); // UNSAFE
}

int
main (void)
{
	struct sigaction sa;
	unsigned ret;
	time_t now;

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = signal_handler;
	sigaction (SIGALRM, &sa, NULL);

#if (USE_MY_ALARM == 1)
	ret = my_alarm (10);
#else
	ret = alarm (10);
#endif
	now = time (NULL);
	printf ("%s\tret1: %u\n", ctime (&now), ret);
	sleep (2);
#if (USE_MY_ALARM == 1)
	ret = my_alarm (5);
#else
	ret = alarm (5);
#endif
	now = time (NULL);
	printf ("%s\tret2: %u\n", ctime (&now), ret);

	pause ();

	now = time (NULL);
	printf ("%s", ctime (&now));

	return 0;
}

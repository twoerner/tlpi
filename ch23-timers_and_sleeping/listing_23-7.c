/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Demonstrate notification via a thread for a POSIX timer.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define BUF_SIZE 1024

static pthread_mutex_t mtx_G = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond_G = PTHREAD_COND_INITIALIZER;
static int expireCnt_G = 0;

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
thread_function (union sigval sv)
{
	timer_t *tid_p;
	int i;

	tid_p = sv.sival_ptr;

	printf ("[%s] thread notify\n", current_time ("%T"));
	printf ("\ttimer id:           %ld\n", (long)*tid_p);
	printf ("\ttimer_getoverrun(): %d\n", timer_getoverrun (*tid_p));

	i = pthread_mutex_lock (&mtx_G);
	if (i != 0) {
		perror ("pthread_mutex_lock()");
		exit (1);
	}

	expireCnt_G += 1 + timer_getoverrun (*tid_p);

	i = pthread_mutex_unlock (&mtx_G);
	if (i != 0) {
		perror ("pthread_mutex_unlock()");
		exit (1);
	}

	i = pthread_cond_signal (&cond_G);
	if (i != 0) {
		perror ("pthread_cond_signal()");
		exit (1);
	}
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

	if (argc < 2) {
		printf ("usage: %s <secs>[/<nsecs>][:<interval secs>[/<interval nsecs>]]...\n", argv[0]);
		return 0;
	}

	tidlist_p = calloc (argc - 1, sizeof (timer_t));
	if (tidlist_p == NULL) {
		perror ("malloc()");
		return 1;
	}

	sevent.sigev_notify = SIGEV_THREAD;
	sevent.sigev_notify_function = thread_function;
	sevent.sigev_notify_attributes = NULL;

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

	i = pthread_mutex_lock (&mtx_G);
	if (i != 0) {
		perror ("pthread_mutex_lock()");
		return 1;
	}

	while (1) {
		i = pthread_cond_wait (&cond_G, &mtx_G);
		if (i != 0) {
			perror ("pthread_cond_wait()");
			return 1;
		}
		printf ("main(): expireCnt = %d\n", expireCnt_G);
	}

	return 0;
}

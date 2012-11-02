/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Is it possible to pass NULL for new_value in setitimer()?
 */

#include <stdio.h>
#include <sys/time.h>

int
main (void)
{
	int ret;
	struct itimerval old;

	ret = setitimer (ITIMER_REAL, NULL, &old);
	if (ret == -1) {
		perror ("setitimer()");
		return 1;
	}
	printf ("old.it_interval.tv_sec:  %ld\n", old.it_interval.tv_sec);
	printf ("old.it_interval.tv_usec: %ld\n", old.it_interval.tv_usec);
	printf ("old.it_value.tv_sec:     %ld\n", old.it_value.tv_sec);
	printf ("old.it_value.tv_usec:    %ld\n", old.it_value.tv_usec);

	return 0;
}

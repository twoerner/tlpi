/*
 * Copyright (C) 2012  Trevor Woerner
 */

#include <stdio.h>
#include <sys/resource.h>

static void print_rlimit (const char *str_p, int resource);

int
main (void)
{
	print_rlimit ("RLIMIT_RTPRIO", RLIMIT_RTPRIO);
	print_rlimit ("RLIMIT_RTTIME", RLIMIT_RTTIME);

	return 0;
}

static void
print_rlimit (const char *str_p, int resource)
{
	int ret;
	struct rlimit rlim;

	printf ("%s: ", str_p);

	// get limit
	ret = getrlimit (resource, &rlim);
	if (ret == -1) {
		perror ("getrlimit()");
		return;
	}

	// soft
	printf ("soft=");
	if (rlim.rlim_cur == RLIM_INFINITY)
		printf ("(infinite)");
	else
		printf ("%llu", (unsigned long long)rlim.rlim_cur);
	printf (" ");

	// hard
	printf ("hard=");
	if (rlim.rlim_max == RLIM_INFINITY)
		printf ("(infinite)");
	else
		printf ("%llu", (unsigned long long)rlim.rlim_max);

	printf ("\n");
}

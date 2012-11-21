/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

#include <stdio.h>
#include <sys/resource.h>

void
print_rlimit (int resource)
{
	int ret;
	struct rlimit rlim;
	static struct {
		int resource;
		const char *str_p;
	} LimitTable[] = {
		{RLIMIT_AS, "RLIMIT_AS"},
		{RLIMIT_CORE, "RLIMIT_CORE"},
		{RLIMIT_CPU, "RLIMIT_CPU"},
		{RLIMIT_DATA, "RLIMIT_DATA"},
		{RLIMIT_FSIZE, "RLIMIT_FSIZE"},
		{RLIMIT_LOCKS, "RLIMIT_LOCKS"},
		{RLIMIT_MEMLOCK, "RLIMIT_MEMLOCK"},
		{RLIMIT_MSGQUEUE, "RLIMIT_MSGQUEUE"},
		{RLIMIT_NICE, "RLIMIT_NICE"},
		{RLIMIT_NOFILE, "RLIMIT_NOFILE"},
		{RLIMIT_NPROC, "RLIMIT_NPROC"},
		{RLIMIT_RSS, "RLIMIT_RSS"},
		{RLIMIT_RTPRIO, "RLIMIT_RTPRIO"},
		{RLIMIT_RTTIME, "RLIMIT_RTTIME"},
		{RLIMIT_SIGPENDING, "RLIMIT_SIGPENDING"},
		{RLIMIT_STACK, "RLIMIT_STACK"},
	};
	size_t tblSz = sizeof (LimitTable) / sizeof (LimitTable[0]);
	size_t i;

	for (i=0; i<tblSz; ++i) {
		if (LimitTable[i].resource == resource) {
			printf ("%s: ", LimitTable[i].str_p);
			break;
		}
	}
	if (i >= tblSz)
		printf ("resource %d (unknown): ", resource);

	// get limit
	ret = getrlimit (resource, &rlim);
	if (ret == -1) {
		perror ("getrlimit()");
		return;
	}

	// soft
	printf ("soft=");
	if (rlim.rlim_cur == RLIM_INFINITY)
		printf ("infinite");
	else
		printf ("%lld", (long long)rlim.rlim_cur);
	printf (" ");

	printf ("hard=");
	if (rlim.rlim_max == RLIM_INFINITY)
		printf ("infinite");
	else
		printf ("%lld", (long long)rlim.rlim_max);
	printf ("\n");
}

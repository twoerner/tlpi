/*
 * Copyright (C) 2012  Trevor Woerner
 */

#include <stdio.h>
#include <sys/resource.h>
#include "print_rlimit.h"

int
main (void)
{
	print_rlimit (RLIMIT_RTPRIO);
	print_rlimit (RLIMIT_RTTIME);

	return 0;
}

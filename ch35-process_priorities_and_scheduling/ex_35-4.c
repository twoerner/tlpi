/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * If two processes use a pipe to exchange a large amount of data on a
 * multiprocessor system, the communication should be faster if the processes
 * run on the same CPU than if they run on different CPUs. The reason is that
 * when the two processes run on the same CPU, the pipe data will be more
 * quickly accessed because it can remain in the CPU's cache. By contrast, when
 * the processes run on separate CPUs, the benefits of the CPU cache are lost.
 * Write a program that uses sched_setaffinity() to demonstrate this effect, by
 * forcing the processes either onto the same CPU or onto different CPUs.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define __USE_GNU
#include <sched.h>

int
main (int argc, char *argv[])
{
	int ret;
	int pipeFd[2];
	int xferCnt;
	cpu_set_t allCPUs, CPUmask;
	int cpuCnt;

	// check usage
	if (argc != 3) {
		printf ("usage: %s <different CPUs?> <count>\n", argv[0]);
		printf ("  where:\n");
		printf ("    <different CPUs?>   y|n\n");
		printf ("        y  - specify both processes should use different CPUs\n");
		printf ("        !y - both processes should use the same CPU\n");
		printf ("    <count>       number of bytes to xfer\n");
		return 1;
	}

	// get CPU count
	ret = sched_getaffinity (0, sizeof (allCPUs), &allCPUs);
	if (ret == -1) {
		perror ("sched_getaffinity (allCPUs)");
		return 1;
	}
	cpuCnt = CPU_COUNT (&allCPUs);
	printf ("CPU count on this system: %d\n", cpuCnt);
	if (argv[1][0] == 'y') {
		if (cpuCnt == 1) {
			printf ("there only is 1 CPU on this system, multi-CPU test can't be performed\n");
			return 1;
		}
		printf ("using different CPUs\n");
	}
	else
		printf ("using the same CPU\n");

	// by default, run everything on 0th CPU
	CPU_ZERO (&CPUmask);
	CPU_SET (0, &CPUmask);
	ret = sched_setaffinity (0, sizeof (CPUmask), &CPUmask);
	if (ret == -1) {
		perror ("parent sched_setaffinity()");
		return 1;
	}

	// get byte count from cmdline
	xferCnt = atoi (argv[2]);
	if (xferCnt <= 0) {
		printf ("<count> (%d) must be greater than 0\n", xferCnt);
		return 1;
	}

	// prepare pipe
	ret = pipe (pipeFd);
	if (ret == -1) {
		perror ("pipe()");
		return 1;
	}

	switch (fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: { // child
			ssize_t rdCnt;
			char buf[256];

			// close unused write end of pipe
			close (pipeFd[1]);

			// CPU affinity
			if (argv[1][0] == 'y') {
				CPU_SET (cpuCnt-1, &CPUmask);
				ret = sched_setaffinity (0, sizeof (CPUmask), &CPUmask);
				if (ret == -1) {
					perror ("child sched_setaffinity()");
					_exit (1);
				}
			}

			// read from pipe until EOF
			while (1) {
				rdCnt = read (pipeFd[0], buf, sizeof (buf));
				if (rdCnt <= 0) {
					close (pipeFd[0]);
					_exit (0);
				}
			}
			break; }

		default: { // parent
			int i;

			// close unused read end of pipe
			close (pipeFd[0]);

			for (i=0; i<xferCnt; ++i)
				write (pipeFd[1], "0", 1);

			break; }
	}

	return 0;
}

/*
 * CONCLUSION:
 *
 * Maybe I coded something up incorrectly, but the test runs indicate that
 * running both processes on different CPUs is _significantly_ faster (roughly
 * 2x) than running them on the same CPU.
 *
 * Using the same cpu:
 *
 *	$ time ./ex_35-4 n 10000000
 *	CPU count on this system: 8
 *	using the same CPU
 *
 *	real    0m9.461s
 *	user    0m0.621s
 *	sys     0m4.118s
 *
 *	$ time ./ex_35-4 n 10000000
 *	CPU count on this system: 8
 *	using the same CPU
 *
 *	real    0m9.352s
 *	user    0m0.488s
 *	sys     0m4.193s
 *
 *	$ time ./ex_35-4 n 10000000
 *	CPU count on this system: 8
 *	using the same CPU
 *
 *	real    0m10.843s
 *	user    0m0.567s
 *	sys     0m4.859s
 *
 *	$ time ./ex_35-4 n 10000000
 *	CPU count on this system: 8
 *	using the same CPU
 *
 *	real    0m10.323s
 *	user    0m0.591s
 *	sys     0m4.571s
 *
 * Using different CPUs:
 *
 *	$ time ./ex_35-4 y 10000000
 *	CPU count on this system: 8
 *	using different CPUs
 *
 *	real    0m4.304s
 *	user    0m0.263s
 *	sys     0m4.006s
 *
 *	$ time ./ex_35-4 y 10000000
 *	CPU count on this system: 8
 *	using different CPUs
 *
 *	real    0m4.592s
 *	user    0m0.395s
 *	sys     0m4.162s
 *
 *	$ time ./ex_35-4 y 10000000
 *	CPU count on this system: 8
 *	using different CPUs
 *
 *	real    0m4.247s
 *	user    0m0.295s
 *	sys     0m3.920s
 *
 *	$ time ./ex_35-4 y 10000000
 *	CPU count on this system: 8
 *	using different CPUs
 *	
 *	real    0m3.940s
 *	user    0m0.283s
 *	sys     0m3.634s
 *
 */

/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * Write a program that places itself under the SCHED_FIFO scheduling policy
 * and then creates a child process. Both processes should execute a function
 * that causes the process to consume a maximum of 3 seconds of CPU time.
 * (This can be done by using a loop in which the times() system call is
 * repeatedly called to determine the amount of CPU time so far consumed.)
 * After each quarter of a second of consumed CPU time, the function should
 * print a message that displays the process ID and the amount of CPU time so
 * far consumed. After each second of consumed CPU time, the function should
 * call sched_yield() to yield the CPU to the other process. (Alternatively,
 * the processes could raise each other's scheduling priority using
 * sched_setparam().) The program's output should demonstrate that the two
 * processes alternately execute for 1 second of CPU time. (Note carefully the
 * advice given in Section 35.3.2 about preventing a runaway realtime process
 * from hogging the CPU.)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define __USE_GNU
#include <sched.h>
#include <unistd.h>
#include <sys/times.h>

#define ALRM 60

static void fn (void);

int
main (void)
{
	int ret;
	struct sched_param prm;
	cpu_set_t cpuset;

	CPU_ZERO (&cpuset);
	CPU_SET (0, &cpuset);
	ret = sched_setaffinity (0, sizeof (cpuset), &cpuset);
	if (ret == -1) {
		perror ("sched_setaffinity()");
		return 1;
	}

	prm.sched_priority = 1;
	ret = sched_setscheduler (0, SCHED_FIFO, &prm);
	if (ret == -1) {
		perror ("sched_setscheduler()");
		return 1;
	}

	switch (fork ()) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: // child
			alarm (ALRM);
			fn ();
			_exit (0);

		default: // parent
			alarm (ALRM);
			fn ();
			break;
	}

	return 0;
}

static void
fn (void)
{
	struct tms timeData, startData;
	long ticksPerSec;
	long clockDiff;
	long printed, block;

	printf ("{%05d} starting\n", getpid ());

	times (&startData);

	ticksPerSec = sysconf (_SC_CLK_TCK);
	if (ticksPerSec == -1) {
		perror ("sysconf(_SC_CLK_TCK)");
		exit (1);
	}

	printed = 0;
	while (1) {
		times (&timeData);
		clockDiff = (long)(timeData.tms_utime - startData.tms_utime);

		// exit after 3 seconds of CPU time
		if (clockDiff >= (long)(3 * ticksPerSec)) {
			printf ("{%05d} done\n", getpid ());
			return;
		}

		// print something every 1/4 second
		block = clockDiff / (long)(ticksPerSec / 4);
		if (clockDiff % (long)(ticksPerSec / 4) == 0) {
			if (block > printed) {
				printf ("{%05d} CPU time consumed: %ld\n", getpid (), (long)clockDiff);
				printed = block;
			}
		}

		// yield processor every second
		if ((clockDiff % ticksPerSec) == 0)
			sched_yield ();
	}
}


/*
 * NOTE: this program must be run as the super-user.
 */

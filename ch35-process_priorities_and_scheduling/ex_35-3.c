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

/*
 * NOTE: this program only "works" as expected when running on a kernel
 *       compiled with -rt extensions.
 *
 * When run on a "normal" kernel I get:
 *	$ ./ex_35-3 
 *	{21839} starting
 *	{21840} starting
 *	{21839} CPU time consumed: 25
 *	{21840} CPU time consumed: 25
 *	{21839} CPU time consumed: 50
 *	{21840} CPU time consumed: 50
 *	{21839} CPU time consumed: 75
 *	{21840} CPU time consumed: 75
 *	{21839} CPU time consumed: 100
 *	{21840} CPU time consumed: 100
 *	{21839} CPU time consumed: 125
 *	{21840} CPU time consumed: 125
 *	{21839} CPU time consumed: 150
 *	{21840} CPU time consumed: 150
 *	{21839} CPU time consumed: 175
 *	{21840} CPU time consumed: 175
 *	{21839} CPU time consumed: 200
 *	{21840} CPU time consumed: 200
 *	{21839} CPU time consumed: 225
 *	{21840} CPU time consumed: 225
 *	{21839} CPU time consumed: 250
 *	{21840} CPU time consumed: 250
 *	{21839} CPU time consumed: 275
 *	{21840} CPU time consumed: 275
 *	{21839} done
 *	$ {21840} done
 *
 * When run on an -rt kernel I get:
 *	$ ./ex_35-3 
 *	{04714} starting
 *	{04713} starting
 *	{04713} CPU time consumed: 25
 *	{04713} CPU time consumed: 50
 *	{04713} CPU time consumed: 75
 *	{04713} CPU time consumed: 100
 *	{04714} CPU time consumed: 25
 *	{04714} CPU time consumed: 50
 *	{04714} CPU time consumed: 75
 *	{04714} CPU time consumed: 100
 *	{04713} CPU time consumed: 125
 *	{04713} CPU time consumed: 150
 *	{04713} CPU time consumed: 175
 *	{04713} CPU time consumed: 200
 *	{04714} CPU time consumed: 125
 *	{04714} CPU time consumed: 150
 *	{04714} CPU time consumed: 175
 *	{04714} CPU time consumed: 200
 *	{04714} CPU time consumed: 225
 *	{04714} CPU time consumed: 250
 *	{04714} CPU time consumed: 275
 *	{04714} done
 *	{04713} CPU time consumed: 225
 *	{04713} CPU time consumed: 250
 *	{04713} CPU time consumed: 275
 *	{04713} done
 */

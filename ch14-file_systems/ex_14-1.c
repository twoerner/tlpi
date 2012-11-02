/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Write a program that measures the time required to create and then remove a
 * large number of 1-byte files from a single directory. The program should create
 * files with names of the form xNNNNNN, where NNNNNN is replaced by a random six-digit
 * number. The files should be created in the random order in which their names are
 * generated, and then deleted in increasing numerical order (i.e., an order that is
 * different from that in which they were created). The number of files (NF) and the
 * directory in which they are to be created should be specifiable on the command
 * line. Measure the times required for different values of NF (e.g., in the range from
 * 1000 to 20,000) and for different file systems (e.g., ext2, ext3, and XFS). What
 * patterns do you observe on each file system as NF increases? How do the various
 * file systems compare? Do the results change if the files are created in increasing
 * numerical order (x000001, x000002, x000003, and so on) and then deleted in the same
 * order? If so, what do you think the reason(s) might be? Again, do the results vary
 * across filesystem types?
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <getopt.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/times.h>

static void usage (const char *pgm_p);
static int myintsort (const void *a_ptr, const void *b_ptr);

int
main (int argc, char *argv[])
{
	int c;
	struct option longOpts[] = {
		{"help", no_argument, NULL, 'h'},
		{"verbose", no_argument, NULL, 'V'},
		{"sequential", no_argument, NULL, 's'},
		{"directory", required_argument, NULL, 'd'},
		{NULL, 0, NULL, 0},
	};
	unsigned numFiles, i;
	int tmp;
	char fname[PATH_MAX];
	bool sequential;
	struct tms tmsTimes;
	clock_t clockTime;
	long clockTicks;
	const char *dir_p = ".";
	unsigned *fnums_p;
	bool verbose;

	// process cmdline args
	sequential = false;
	verbose = false;
	while ((c = getopt_long (argc, argv, "hsd:V", longOpts, NULL)) != -1) {
		switch (c) {
			case 'h':
				usage (argv[0]);
				return 0;

			case 's':
				sequential = true;
				break;

			case 'd':
				dir_p = optarg;
				break;

			case 'V':
				verbose = true;
				break;

			default:
				usage (argv[0]);
				return 1;
		}
	}
	if (argc != (optind + 1)) {
		printf ("incorrect number of cmdline args\n");
		usage (argv[0]);
		return 1;
	}
	tmp = atoi (argv[optind]);
	if (tmp <= 0) {
		printf ("poor choice of <num>: %d\n", tmp);
		return 1;
	}
	numFiles = (unsigned)tmp;

	// seed random number generator
	srand ((unsigned)time (NULL));

	// allocate file name list
	fnums_p = (unsigned*)malloc (sizeof (unsigned) * numFiles);
	if (fnums_p == NULL) {
		perror ("malloc()");
		return 1;
	}

	// figure out _SC_CLK_TCK
	clockTicks = sysconf (_SC_CLK_TCK);
	if (clockTicks == -1) {
		perror ("sysconf (_SC_CLK_TCK)");
		return 1;
	}

	// file-creation loop
	i = 0;
	while (1) {
		if (i == (2 * numFiles)) {
			fprintf (stderr, "can't seem to create enough files\n");
			return 1;
		}

		if (sequential) {
			snprintf (fname, PATH_MAX, "%s/x%06u", dir_p, i);
			fnums_p[i] = i;
		}
		else {
			tmp = rand () % 999999;
			snprintf (fname, PATH_MAX, "%s/x%06i", dir_p, tmp);
			fnums_p[i] = (unsigned)tmp;
		}

		tmp = open (fname, O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
		if (tmp == -1) {
			if (verbose)
				fprintf (stderr, "can't create: '%s'\n", fname);
			continue;
		}
		if (verbose)
			printf ("creating: '%s'\n", fname);

		write (tmp, "a", 1);
		close (tmp);
		if (++i >= numFiles)
			break;
	}

	// file-deletion loop
	qsort (fnums_p, numFiles, sizeof (unsigned), myintsort);
	for (i=0; i<numFiles; ++i) {
		snprintf (fname, PATH_MAX, "%s/x%06u", dir_p, fnums_p[i]);
		unlink (fname);
		if (verbose)
			printf ("deleting '%s'\n", fname);
	}

	// free
	free (fnums_p);

	// print out duration
	clockTime = clock ();
	if (clockTime == -1) {
		perror ("clock()");
		return 1;
	}
	if (times (&tmsTimes) == -1) {
		perror ("times()");
		return 1;
	}
	printf ("total:%.2f  user:%.2f  system:%.2f\n",
			(double)clockTime / CLOCKS_PER_SEC,
			(double)tmsTimes.tms_utime / clockTicks,
			(double)tmsTimes.tms_stime / clockTicks);

	return 0;
}

static void
usage (const char *pgm_p)
{
	printf ("usage: %s [options] <num>\n", pgm_p);
	printf ("  where:\n");
	printf ("    <num>                  the number of files to create\n");
	printf ("  options:\n");
	printf ("    --help|-h              print usage information and exit successfully\n");
	printf ("    --verbose|-V           be verbose\n");
	printf ("    --sequential|-s        create the file names sequentially\n");
	printf ("    --directory|-d <dir>   the directory in which to create the files (default: $PWD)\n");
}

static int
myintsort (const void *a_ptr, const void *b_ptr)
{
	int a, b;

	a = *(int*)a_ptr;
	b = *(int*)b_ptr;

	if (a > b)
		return 1;
	if (a < b)
		return -1;
	return 0;
}

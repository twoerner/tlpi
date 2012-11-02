/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program illustrates the effect of free() on the program break.
 * This program allocates multiple blocks of memory and then frees some
 * or all of them, depending on its (optional) cmdline args.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_ALLOCS 1000000

int
main (int argc, char *argv[])
{
	char *ptr[MAX_ALLOCS];
	int freeStep, freeMin, freeMax, blockSize, numAllocs, j;

	printf ("\n");

	if (argc < 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <num-allocs> <block-size> [step [min [max]]]\n", argv[0]);
		return 1;
	}

	numAllocs = atoi (argv[1]);
	if (numAllocs > MAX_ALLOCS) {
		printf ("too many requested allocs\n");
		return 1;
	}

	blockSize = atoi (argv[2]);

	freeStep = (argc > 3)? atoi (argv[3]) : 1;
	freeMin = (argc > 4)? atoi (argv[4]) : 1;
	freeMax = (argc > 5)? atoi (argv[5]) : numAllocs;

	if (freeMax > numAllocs)
		freeMax = numAllocs;

	printf ("Initial program break:		0x%08x\n", (unsigned)sbrk (0));
	printf ("Allocating %d*%d bytes\n", numAllocs, blockSize);

	for (j=0; j<numAllocs; ++j) {
		ptr[j] = malloc (blockSize);
		if (ptr[j] == NULL) {
			perror ("malloc()");
			return 1;
		}
	}

	printf ("Program break is now:		0x%08x\n", (unsigned)sbrk (0));

	printf ("Freeing blocks from %d to %d in steps of %d\n", freeMin, freeMax, freeStep);
	for (j=freeMin-1; j<freeMax; j+=freeStep)
		free (ptr[j]);

	printf ("Program break after free():	0x%08x\n", (unsigned)sbrk (0));
	return 0;
}

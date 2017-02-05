/*
 * Copyright (C) 2012  Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * This program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>

static void usage(const char *progName_p, const char *msg_p);

int
main(int argc, char *argv[])
{
	int j;

	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
		usage(argv[0], NULL);
		exit(EXIT_SUCCESS);
	}

	for (j=1; j<argc; ++j)
		if (msgctl(atoi(argv[j]), IPC_RMID, NULL) == -1) {
			perror("msgctl()");
			exit(EXIT_FAILURE);
		}

	exit(EXIT_SUCCESS);
}

static void
usage(const char *progName_p, const char *msg_p)
{
	if (msg_p != NULL)
		fprintf(stderr, "%s", msg_p);
	fprintf(stderr, "usage: %s <msqid...>\n", progName_p);
}

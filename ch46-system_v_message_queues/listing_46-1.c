/*
 * Copyright (C) 2012  Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * This program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

#include <stdio.h>
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
	int numKeyFlags;
	int flags, msqid, opt;
	unsigned perms;
	long lkey;
	key_t key;

	numKeyFlags = 0;
	flags = 0;
	while ((opt = getopt(argc, argv, "cf:k:px")) != -1) {
		switch (opt) {
			case 'c':
				flags |= IPC_CREAT;
				break;

			case 'f':
				key = ftok(optarg, 1);
				if (key == -1) {
					perror("ftok()");
					exit(EXIT_FAILURE);
				}
				++numKeyFlags;
				break;

			case 'k':
				if (sscanf(optarg, "%li", &lkey) != 1) {
					fprintf(stderr, "can't convert '%s' to long\n", optarg);
					exit(EXIT_FAILURE);
				}
				key = lkey;
				++numKeyFlags;
				break;

			case 'p':
				key = IPC_PRIVATE;
				++numKeyFlags;
				break;

			case 'x':
				flags |= IPC_EXCL;
				break;

			default:
				usage(argv[0], "bad option\n");
				exit(EXIT_FAILURE);
		}
	}

	if (numKeyFlags != 1) {
		usage(argv[0], "exactly one of -f, -k, or -p must be supplied\n");
		exit(EXIT_FAILURE);
	}

	perms = (optind == argc) ? (S_IRUSR | S_IWUSR) : atoi(argv[optind]);
	msqid = msgget(key, flags | perms);
	if (msqid == -1) {
		perror("msgget()");
		exit(EXIT_FAILURE);
	}

	printf ("%d\n", msqid);
	exit(EXIT_SUCCESS);
}

static void
usage(const char *progName_p, const char *msg_p)
{
	if (msg_p != NULL)
		fprintf(stderr, "%s", msg_p);
	fprintf(stderr, "usage: %s [OPTIONS] {-f <pathname>|-k <key>|-p} [OCTAL PERMS]\n", progName_p);
	fprintf(stderr, "  options:\n");
	fprintf(stderr, "    -c               use IPC_CREAT flag\n");
	fprintf(stderr, "    -x               use IPC_EXCL flag\n");
	fprintf(stderr, "  required (one of):\n");
	fprintf(stderr, "    -f <pathname>    generate key using ftok() on <path>\n");
	fprintf(stderr, "    -k <key>         use <key> as key\n");
	fprintf(stderr, "    -p               use IPC_PRIVATE key\n");
}

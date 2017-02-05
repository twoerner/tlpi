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

#define MAX_MTEXT_LEN 1024

typedef struct {
	long mtype;
	char mtext[MAX_MTEXT_LEN];
} mbuf_t;

static void usage(const char *progName_p, const char *msg_p);

int
main(int argc, char *argv[])
{
	int msqid, flags, type;
	ssize_t msgLen;
	size_t maxBytes;
	mbuf_t msg;
	int opt;

	flags = 0;
	type = 0;
	while ((opt = getopt(argc, argv, "ent:x")) != -1) {
		switch (opt) {
			case 'e':
				flags |= MSG_NOERROR;
				break;

			case 'n':
				flags |= IPC_NOWAIT;
				break;

			case 't':
				type = atoi(optarg);
				break;

#ifdef MSG_EXCEPT
			case 'x':
				flags |= MSG_EXCEPT;
				break;
#endif
			default:
				usage(argv[0], NULL);
				exit(EXIT_FAILURE);
		}
	}

	if (argc < optind + 1 || argc > optind + 2) {
		usage(argv[0], "wrong number of cmdline args\n");
		exit(EXIT_FAILURE);
	}

	msqid = atoi(argv[optind]);
	maxBytes = (argc > optind + 1)? atoi(argv[optind + 1]) : MAX_MTEXT_LEN;

	msgLen = msgrcv(msqid, &msg, maxBytes, type, flags);
	if (msgLen == -1) {
		perror("msgrcv()");
		exit(EXIT_FAILURE);
	}
	printf("received: type=%ld; length=%ld", msg.mtype, (long)msgLen);
	if (msgLen > 0)
		printf("; body=%s", msg.mtext);
	printf("\n");

	exit(EXIT_SUCCESS);
}

static void
usage(const char *progName_p, const char *msg_p)
{
	if (msg_p != NULL)
		fprintf(stderr, "%s", msg_p);
	fprintf(stderr, "usage: %s [OPTIONS] <msqid> [<max bytes>]\n", progName_p);
	fprintf(stderr, "  options:\n");
	fprintf(stderr, "    -e               use MSG_NOERROR flags\n");
	fprintf(stderr, "    -t <type>        select message of <type>\n");
	fprintf(stderr, "    -n               use IPC_NOWAIT flags\n");
#ifdef MSG_EXCEPT
	fprintf(stderr, "    -x               use MSG_EXCEPT flags\n");
#endif
}

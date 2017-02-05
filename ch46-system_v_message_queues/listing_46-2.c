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
	int msqid, flags, msgLen;
	mbuf_t msg;
	int opt;

	flags = 0;
	while ((opt = getopt(argc, argv, "n")) != -1) {
		if (opt == 'n')
			flags |= IPC_NOWAIT;
		else {
			usage(argv[0], NULL);
			exit(EXIT_FAILURE);
		}
	}

	if (argc < optind + 2 || argc > optind + 3) {
		usage(argv[0], "wrong number of cmdline args\n");
		exit(EXIT_FAILURE);
	}

	msqid = atoi(argv[optind]);
	msg.mtype = atoi(argv[optind + 1]);

	msgLen = 0;
	if (argc > optind + 2) {
		msgLen = strlen(argv[optind + 2]) + 1;
		if (msgLen > MAX_MTEXT_LEN) {
			fprintf(stderr, "message text too long (max:%d)\n", MAX_MTEXT_LEN);
			exit(EXIT_FAILURE);
		}
		memcpy(msg.mtext, argv[optind + 2], msgLen);
	}

	// send message
	if (msgsnd(msqid, &msg, msgLen, flags) == -1) {
		perror("msgsnd()");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);

}

static void
usage(const char *progName_p, const char *msg_p)
{
	if (msg_p != NULL)
		fprintf(stderr, "%s", msg_p);
	fprintf(stderr, "usage: %s [OPTIONS] <msqid> <msg-type> [<msg text>]\n", progName_p);
	fprintf(stderr, "  options:\n");
	fprintf(stderr, "    -n               use IPC_NOWAIT flags\n");
}

/*
 * Copyright (C) 2012  Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * This program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/stat.h>

int
main(void)
{
	int maxind, ind, msqid;
	struct msqid_ds ds;
	struct msginfo msginfo;

	maxind = msgctl(0, MSG_INFO, (struct msqid_ds *)&msginfo);
	if (maxind == -1) {
		perror("msgctl()");
		exit(EXIT_FAILURE);
	}

	printf("maxind: %d\n\n", maxind);
	printf("%6s %4s %8s %15s\n", "index", "id", "key", "messages");

	for (ind=0; ind<=maxind; ++ind) {
		msqid = msgctl(ind, MSG_STAT, &ds);
		if (msqid == -1) {
			if (errno != EINVAL && errno != EACCES) {
				perror("msgctl(2)");
				exit(EXIT_FAILURE);
			}
			continue;
		}

		printf("%4d %8d 0x%08lx %7ld\n", ind, msqid, (unsigned long)ds.msg_perm.__key, (long)ds.msg_qnum);
	}

	exit(EXIT_SUCCESS);
}

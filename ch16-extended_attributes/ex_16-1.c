/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Write a program that can be used to create or modify a _user_ EA for a file
 * (i.e. a simple version of setfattr(1)). The filename and the EA name/value
 * should be supplied as cmdline args.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <alloca.h>
#include <sys/types.h>
#include <sys/xattr.h>

static void usage (const char *cmd_p);

int
main (int argc, char *argv[])
{
	int ret;
	char *buf_p = NULL;

	if (argc != 4) {
		usage (argv[0]);
		return 1;
	}

	// check if the user supplied the "user." or not
	if (strncmp (argv[2], "user.", 5) != 0) {
		buf_p = (char*)alloca (strlen (argv[2]) + 6);
		if (buf_p == NULL) {
			perror ("alloca()");
			return 1;
		}
		ret = snprintf (buf_p, strlen (argv[2]) + 6, "user.%s", argv[2]);
		if (ret < 0) {
			perror ("snprintf()");
			return 1;
		}
		if ((size_t)ret >= (strlen (argv[2]) + 6)) {
			printf ("bad snprintf() length? (%zu)\n", strlen (argv[2]) + 6);
			return 1;
		}
	}
	else
		buf_p = argv[2];

	ret = setxattr (argv[1], buf_p, argv[3], strlen (argv[3]), 0);
	if (ret == -1) {
		perror ("setxattr()");
		return 1;
	}

	return 0;
}

static void
usage (const char *cmd_p)
{
	printf ("create or modify a user extended attribute on a file\n");
	printf ("usage: %s <path> <EA name> <EA value>\n", cmd_p);
	printf ("  where:\n");
	printf ("    <path>     the file object whose extended attributes to create/modify\n");
	printf ("    <EA name>  the name of the extended attribute\n");
	printf ("    <EA value> the value of the extended attribute\n");
}

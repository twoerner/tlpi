/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Write a simple version of the chattr(1) command, which modifies file i-node flags.
 * See the chattr(1) man page for details of the "chattr" cmdline interface. You don't
 * need to implement the -R, -V, and -v options.
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fs.h>

typedef enum {
	ActionNone_e,
	ADD_e, REMOVE_e, SET_e,
	ActionEnd_e,
} Action_e;

static void usage (const char *cmd_p);
static void process_mode (const char *modeStr_p, Action_e *actionRet_p, int *attrRet_p);

int
main (int argc, char *argv[])
{
	Action_e action;
	int attr, i, fd, ret, oldAttr;

	if (argc <= 2) {
		usage (argv[0]);
		return 1;
	}

	process_mode (argv[1], &action, &attr);
	if ((action <= ActionNone_e) || (action >= ActionEnd_e)) {
		printf ("mode must start with an action, which must be one of [-+=]\n");
		usage (argv[0]);
		return 1;
	}

	for (i=2; i<argc; ++i) {
		fd = open (argv[i], 0);
		if (fd == -1) {
			perror ("open()");
			return 1;
		}

		if ((action == ADD_e) || (action == REMOVE_e)) {
			ret = ioctl (fd, FS_IOC_GETFLAGS, &oldAttr);
			if (ret == -1) {
				perror ("ioctl(GET)");
				return 1;
			}
			if (action == ADD_e)
				attr |= oldAttr;
			if (action == REMOVE_e)
				attr = oldAttr & ~attr;
		}
		ret = ioctl (fd, FS_IOC_SETFLAGS, &attr);
		if (ret == -1)
			perror ("ioctl(SET)");
	}

	return 0;
}

static void
process_mode (const char *modeStr_p, Action_e *actionRet_p, int *attrRet_p)
{
	size_t len, i;
	Action_e action;
	int attr;

	if (modeStr_p == NULL)
		return;
	action = ActionNone_e;
	attr = 0;

	len = strlen (modeStr_p);
	if (len > 16)
		return;

	for (i=0; i<len; ++i) {
		switch (modeStr_p[i]) {
			case '+':
				action = ADD_e;
				break;

			case '-':
				action = REMOVE_e;
				break;

			case '=':
				action = SET_e;
				break;

			case 'a':
				attr |= FS_APPEND_FL;
				break;

			case 'c':
				attr |= FS_COMPR_FL;
				break;

			case 'D':
				attr |= FS_DIRSYNC_FL;
				break;

			case 'i':
				attr |= FS_IMMUTABLE_FL;
				break;

			case 'j':
				attr |= FS_JOURNAL_DATA_FL;
				break;

			case 'A':
				attr |= FS_NOATIME_FL;
				break;

			case 'd':
				attr |= FS_NODUMP_FL;
				break;

			case 't':
				attr |= FS_NOTAIL_FL;
				break;

			case 's':
				attr |= FS_SECRM_FL;
				break;

			case 'S':
				attr |= FS_SYNC_FL;
				break;

			case 'T':
				attr |= FS_TOPDIR_FL;
				break;

			case 'u':
				attr |= FS_UNRM_FL;
				break;

			case 'e':
				attr |= FS_EXTENT_FL;
				break;

			default:
				printf ("unknown flag: '%c' (0x%08x)\n", modeStr_p[i], modeStr_p[i]);
				break;
		}
	}

	if (actionRet_p != NULL)
		*actionRet_p = action;
	if (attrRet_p != NULL)
		*attrRet_p = attr;
	
}

static void
usage (const char *cmd_p)
{
	printf ("usage: %s <mode> <files...>\n", cmd_p);
	printf ("  change file attributes on a Linux file system\n");
	printf ("  <mode> -> +-=[acdeijstuADST]\n");
	printf ("    + -> adds mode\n");
	printf ("    - -> removes mode\n");
	printf ("    = -> sets mode\n");
	printf ("    a -> append only\n");
	printf ("    c -> compressed\n");
	printf ("    d -> no dump\n");
	printf ("    e -> extent format\n");
	printf ("    i -> immutable\n");
	printf ("    j -> data journalling\n");
	printf ("    s -> secure deletion\n");
	printf ("    t -> no tail-merging\n");
	printf ("    u -> undeletable\n");
	printf ("    A -> no atime updates\n");
	printf ("    D -> synchronous directory updates\n");
	printf ("    S -> synchronous updates\n");
	printf ("    T -> top of directory hierarchy\n");
}

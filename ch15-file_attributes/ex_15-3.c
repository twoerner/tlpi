/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * On a system running Linux 2.6, modify the program in listing_15-1 so
 * that the file timestamps are displayed with nanosecond accuracy.
 */

#define _BSD_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "ch15_util.h"

static void
display_stat_info (const struct stat *statInfo_p)
{
	char buf[256];

	printf ("file type:                ");
	switch (statInfo_p->st_mode & S_IFMT) {
		case S_IFREG:
			printf ("regular file");
			break;

		case S_IFDIR:
			printf ("directory");
			break;

		case S_IFCHR:
			printf ("character device");
			break;

		case S_IFBLK:
			printf ("block device");
			break;

		case S_IFLNK:
			printf ("symbolic (soft) link");
			break;

		case S_IFIFO:
			printf ("FIFO or pipe");
			break;

		case S_IFSOCK:
			printf ("socket");
			break;

		default:
			printf ("(unknown file type)");
			break;
	}
	printf ("\n");

	printf ("device containing i-node: major=%ld minor=%ld\n",
			(long)major (statInfo_p->st_dev), (long)minor (statInfo_p->st_dev));
	printf ("i-node:                   %ld\n", (long)statInfo_p->st_ino);
	printf ("mode:                     %lo (%s)\n", (unsigned long)statInfo_p->st_mode, file_perm_str (statInfo_p->st_mode, 1));
	if (statInfo_p->st_mode & (S_ISUID | S_ISGID | S_ISVTX))
		printf ("      special bits set:   %s%s%s\n",
				(statInfo_p->st_mode & S_ISUID)? "set-UID " : "",
				(statInfo_p->st_mode & S_ISGID)? "set-GID " : "",
				(statInfo_p->st_mode & S_ISVTX)? "sticky " : "");
	printf ("hard links:               %ld\n", (long)statInfo_p->st_nlink);
	printf ("ownership:                UID=%ld  GID=%ld\n", (long)statInfo_p->st_uid, (long)statInfo_p->st_gid);
	if (S_ISCHR (statInfo_p->st_mode) || S_ISBLK (statInfo_p->st_mode))
		printf ("device (st_rdev):         major=%ld  minor=%ld\n", (long)major (statInfo_p->st_rdev), (long)minor (statInfo_p->st_rdev));
	printf ("file size:                %lld\n", (long long)statInfo_p->st_size);
	printf ("optimal I/O block size:   %lld\n", (long long)statInfo_p->st_blksize);
	printf ("512B blocks allocated:    %lld\n", (long long)statInfo_p->st_blocks);

	snprintf (buf, sizeof (buf), "%s", ctime (&statInfo_p->st_atime));
	buf[strlen (buf) - 1] = 0;
	printf ("last file access:         %s nsec:%ld\n", buf, (long)statInfo_p->st_atim.tv_nsec);
	snprintf (buf, sizeof (buf), "%s", ctime (&statInfo_p->st_mtime));
	buf[strlen (buf) - 1] = 0;
	printf ("last file modification:   %s nsec:%ld\n", buf, (long)statInfo_p->st_mtim.tv_nsec);
	snprintf (buf, sizeof (buf), "%s", ctime (&statInfo_p->st_ctime));
	buf[strlen (buf) - 1] = 0;
	printf ("last status change:       %s nsec:%ld\n", buf, (long)statInfo_p->st_ctim.tv_nsec);
}

static void
usage (const char *cmd_p)
{
	printf ("usage: %s [option] <file>\n", cmd_p);
	printf ("  where:\n");
	printf ("    <file>           is the file whose metadata the program should print\n");
	printf ("  option:\n");
	printf ("    --link|-l        use lstat() instead of stat()\n");
}

int
main (int argc, char *argv[])
{
	struct option longOpts[] = {
		{"help", no_argument, NULL, 'h'},
		{"link", no_argument, NULL, 'l'},
		{NULL, 0, NULL, 0},
	};
	struct stat statInfo;
	bool statLink = false;
	int c;

	while ((c = getopt_long (argc, argv, "hl", longOpts, NULL)) != -1) {
		switch (c) {
			case 'h':
				usage (argv[0]);
				return 0;

			case 'l':
				statLink = true;
				break;
		}
	}
	if (argc != (optind + 1)) {
		usage (argv[0]);
		return 1;
	}

	if (statLink) {
		if (lstat (argv[optind], &statInfo) == -1) {
			perror ("lstat()");
			return 1;
		}
	}
	else {
		if (stat (argv[optind], &statInfo) == -1) {
			perror ("stat()");
			return 1;
		}
	}

	display_stat_info (&statInfo);
	return 0;
}

/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the use of nftw().
 */

#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <unistd.h>
#include <ftw.h>

static void usage (const char *cmd_p);
static int dir_tree (const char *pathname_p, const struct stat *sbuf_p, int type, struct FTW *ftw_p);

static void
usage (const char *cmd_p)
{
	printf ("usage: %s [OPTIONS] <starting directory>\n", cmd_p);
	printf ("  options:\n");
	printf ("    -d    use FTW_DEPTH flag\n");
	printf ("    -m    use FTW_MOUNT flag\n");
	printf ("    -p    use FTW_PHYS flag\n");
}

static int
dir_tree (const char *pathname_p, const struct stat *sbuf_p, int type, struct FTW *ftw_p)
{
	switch (sbuf_p->st_mode & S_IFMT) {
		case S_IFREG:  printf ("-"); break;
		case S_IFDIR:  printf ("d"); break;
		case S_IFCHR:  printf ("c"); break;
		case S_IFBLK:  printf ("b"); break;
		case S_IFLNK:  printf ("l"); break;
		case S_IFIFO:  printf ("p"); break;
		case S_IFSOCK: printf ("s"); break;
		default:       printf ("?"); break;
	}

	printf (" %s  ",
			(type == FTW_D)  ? "D  " : (type == FTW_DNR) ? "DNR" :
			(type == FTW_DP) ? "DP " : (type == FTW_F)   ? "F  " :
			(type == FTW_SL) ? "SL " : (type == FTW_SLN) ? "SLN" :
			(type == FTW_NS) ? "NS " : "   ");

	if (type != FTW_NS)
		printf ("%7ld ", (long)sbuf_p->st_ino);
	else
		printf ("       ");

	printf (" %*s", 4 * ftw_p->level, "");
	printf ("%s\n", &pathname_p[ftw_p->base]);

	return 0;
}

int
main (int argc, char *argv[])
{
	int flags, opt;

	flags = 0;
	while ((opt = getopt (argc, argv, "dmp")) != -1) {
		switch (opt) {
			case 'd': flags |= FTW_DEPTH; break;
			case 'm': flags |= FTW_MOUNT; break;
			case 'p': flags |= FTW_PHYS;  break;
			default:
				usage (argv[0]);
		}
	}

	if (argc > optind + 1) {
		printf ("required cmdline arg missing\n");
		usage (argv[0]);
		return 3;
	}

	if (nftw ((argc > optind)? argv[optind] : ".", dir_tree, 10, flags) == -1) {
		perror ("nftw()");
		return 1;
	}

	return 0;
}

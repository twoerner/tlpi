/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Write a program that uses nftw() to traverse a directory tree and finishes
 * by printing out counts and percentages of the various types (regular,
 * directory, symbolic link, and so on) of the files in the tree.
 */

#define UNUSED __attribute__((unused))
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <unistd.h>
#include <ftw.h>

// globals
static unsigned nReg = 0;
static unsigned nDir = 0;
static unsigned nChr = 0;
static unsigned nBlk = 0;
static unsigned nLnk = 0;
static unsigned nFifo = 0;
static unsigned nSock = 0;

static int dir_tree (UNUSED const char *pathname_p, const struct stat *sbuf_p, UNUSED int type, UNUSED struct FTW *ftw_p);

static int
dir_tree (UNUSED const char *pathname_p, const struct stat *sbuf_p, UNUSED int type, UNUSED struct FTW *ftw_p)
{
	switch (sbuf_p->st_mode & S_IFMT) {
		case S_IFREG:  ++nReg;  break;
		case S_IFDIR:  ++nDir;  break;
		case S_IFCHR:  ++nChr;  break;
		case S_IFBLK:  ++nBlk;  break;
		case S_IFLNK:  ++nLnk;  break;
		case S_IFIFO:  ++nFifo; break;
		case S_IFSOCK: ++nSock; break;
		default:
		       break;
	}

	return 0;
}

int
main (int argc, char *argv[])
{
	if (nftw ((argc == 2)? argv[1] : ".", dir_tree, 10, 0) == -1) {
		perror ("nftw()");
		return 1;
	}

	printf ("starting at: %s\n", (argc == 2)? argv[1] : ".");
	printf ("   regular files: %u\n", nReg);
	printf ("     directories: %u\n", nDir);
	printf ("    char devices: %u\n", nChr);
	printf ("   block devices: %u\n", nBlk);
	printf ("       sym links: %u\n", nLnk);
	printf ("           fifos: %u\n", nFifo);
	printf ("         sockets: %u\n", nSock);

	return 0;
}

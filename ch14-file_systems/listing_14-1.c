/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program provides a cmdline-level interface to the mount(2)
 * system call. In effect, it is a crude version of the mount(8) command. The following
 * shell session log demonstrates the use of this program. We begin by creating a
 * directory to be used as a mount point and mounting a file system:
 *
 *	$ su
 *	Password:
 *	# mkdir testfs
 *	# ./listing_14-1 -t ext2 -o bsdgroups /dev/sda12 testfs
 *	# cat /proc/mounts | grep sda12
 *	/dev/sda12 $PWD/testfs ext3 rw 0 0
 *	# grep sda12 /etc/mtab
 *
 * The preceeding grep command produces no output because our
 * program doesn't update /etc/mtab.
 *
 *	# ./listing_14-1 -f Rr /dev/sda12 testfs
 *	# cat /proc/mounts | grep sda12
 *	/dev/sda12 $PWD/testfs ext3 ro 0 0
 *
 * Finally we move the mount point to a new location within the directory hierarchy:
 *
 *	# mkdir demo
 *	# ./listing_14-1 -f m testfs demo
 *	# cat /proc/mounts
 *	/dev/sda12 demo ext3 ro 0 0
 */

#include <stdio.h>
#include <sys/mount.h>

static void
usage (const char *cmd_p, const char *msg_p)
{
}

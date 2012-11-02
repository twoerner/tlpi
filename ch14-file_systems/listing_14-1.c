/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
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
#include <string.h>
#include <unistd.h>
#include <sys/mount.h>

static void usage (const char *cmd_p, const char *msg_p);

int
main (int argc, char *argv[])
{
	unsigned long flags;
	char *data_p, *fsType_p;
	size_t i;
	int opt;

	flags = 0;
	data_p = NULL;
	fsType_p = NULL;

	while ((opt = getopt (argc, argv, "o:t:f:")) != -1) {
		switch (opt) {
			case 'o':
				data_p = optarg;
				break;

			case 't':
				fsType_p = optarg;
				break;

			case 'f':
				for (i=0; i<strlen (optarg); ++i) {
					switch (optarg[i]) {
						case 'b': flags |= MS_BIND; break;
						case 'd': flags |= MS_DIRSYNC; break;
						case 'l': flags |= MS_MANDLOCK; break;
						case 'm': flags |= MS_MOVE; break;
						case 'A': flags |= MS_NOATIME; break;
						case 'V': flags |= MS_NODEV; break;
						case 'D': flags |= MS_NODIRATIME; break;
						case 'E': flags |= MS_NOEXEC; break;
						case 'S': flags |= MS_NOSUID; break;
						case 'r': flags |= MS_RDONLY; break;
						case 'c': flags |= MS_REC; break;
						case 'R': flags |= MS_REMOUNT; break;
						case 's': flags |= MS_SYNCHRONOUS; break;
						default:
							  usage (argv[0], NULL);
							  return 1;
					}
				}
				break;

			default:
				usage (argv[0], NULL);
				return 1;
		}
	}
	if (argc != (optind + 2)) {
		usage (argv[0], "wrong number of cmdline args");
		return 1;
	}

	if (mount (argv[optind], argv[optind+1], fsType_p, flags, data_p) == -1) {
		perror ("mount()");
		return 1;
	}

	return 0;
}

static void
usage (const char *cmd_p, const char *msg_p)
{
	if (msg_p != NULL)
		fprintf (stderr, "%s\n", msg_p);

	fprintf (stderr, "usage: %s [options] <source> <target>\n", cmd_p);
	fprintf (stderr, "  where:\n");
	fprintf (stderr, "    <source>           file system to attach\n");
	fprintf (stderr, "    <target>           directory to which to attach <target>\n");
	fprintf (stderr, "  options:\n");
	fprintf (stderr, "    -t <fstype>        e.g. 'ext2', 'reiserfs'\n");
	fprintf (stderr, "    -o <data>          filesystem-dependent options\n");
	fprintf (stderr, "    -f <flags>         mount flags, can be any of:\n");
	fprintf (stderr, "                 b -> MS_BIND         create a bind mount\n");
	fprintf (stderr, "                 d -> MS_DIRSYNC      sync directory updates\n");
	fprintf (stderr, "                 l -> MS_MANDLOCK     permit mandatory locking\n");
	fprintf (stderr, "                 m -> MS_MOVE         atomically move subtree\n");
	fprintf (stderr, "                 A -> MS_NOATIME      don't update last access time\n");
	fprintf (stderr, "                 V -> MS_NODEV        don't permit device access\n");
	fprintf (stderr, "                 D -> MS_NODIRATIME   don't update atime on directories\n");
	fprintf (stderr, "                 E -> MS_NOEXEC       don't allow executables\n");
	fprintf (stderr, "                 S -> MS_NOSUID       don't allow set-user/group-ID programs\n");
	fprintf (stderr, "                 r -> MS_RDONLY       mount read-only\n");
	fprintf (stderr, "                 c -> MS_REC          recursive mount\n");
	fprintf (stderr, "                 R -> MS_REMOUNT      remount\n");
	fprintf (stderr, "                 s -> MS_SYNCHRONOUS  make writes synchronous\n");
}

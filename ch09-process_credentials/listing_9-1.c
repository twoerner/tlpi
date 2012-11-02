/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program uses the getresuid(), getresgid(), setfsuid() setfsgid(), getgroups()
 * system calls to retrieve all of the process's user and group IDs.
 */

#define _GNU_SOURCE
#define _USE_GNU

#include <stdio.h>
#include <unistd.h>
#include <sys/fsuid.h>
#include <limits.h>
#include <pwd.h>
#include <grp.h>
#include <ctype.h>

#define SG_SIZE (NGROUPS_MAX + 1)

char *
user_name_from_id (uid_t uid)
{
	struct passwd *pwd_p;

	pwd_p = getpwuid (uid);
	return (pwd_p == NULL)? NULL : pwd_p->pw_name;
}

char *
group_name_from_id (gid_t gid)
{
	struct group *grp_p;

	grp_p = getgrgid (gid);
	return (grp_p == NULL)? NULL : grp_p->gr_name;
}

int
main (void)
{
	uid_t ruid, euid, suid, fsuid;
	gid_t rgid, egid, sgid, fsgid;
	gid_t suppGroups[SG_SIZE];
	int numGroups, j;
	char *ptr;

	if (getresuid (&ruid, &euid, &suid) == -1) {
		perror ("getresuid()");
		return 1;
	}
	if (getresgid (&rgid, &egid, &sgid) == -1) {
		perror ("getresgid()");
		return 1;
	}

	/*
	   attempts to change the file-system IDs are always ignored
	   for unprivileged processes, but even so, the following
	   calls return the current file-system IDs
	*/
	fsuid = setfsuid (0);
	fsgid = setfsgid (0);

	printf ("UID: ");
	ptr = user_name_from_id (ruid);
	printf ("real=%s (%ld); ", ptr?:"?", (long)ruid);
	ptr = user_name_from_id (euid);
	printf ("eff=%s (%ld); ", ptr?:"?", (long)euid);
	ptr = user_name_from_id (suid);
	printf ("saved=%s (%ld); ", ptr?:"?", (long)suid);
	ptr = user_name_from_id (fsuid);
	printf ("fs=%s (%ld);", ptr?:"?", (long)fsuid);
	printf ("\n");

	printf ("GID: ");
	ptr = group_name_from_id (rgid);
	printf ("real=%s (%ld); ", ptr?:"?", (long)rgid);
	ptr = group_name_from_id (egid);
	printf ("eff=%s (%ld); ", ptr?:"?", (long)egid);
	ptr = group_name_from_id (sgid);
	printf ("saved=%s (%ld); ", ptr?:"?", (long)sgid);
	ptr = group_name_from_id (fsgid);
	printf ("fs=%s (%ld);\n", ptr?:"?", (long)fsgid);

	numGroups = getgroups (SG_SIZE, suppGroups);
	if (numGroups == -1) {
		perror ("getgroups()");
		return 1;
	}
	printf ("supplementary groups (%d): ", numGroups);
	for (j=0; j<numGroups; ++j) {
		ptr = group_name_from_id (suppGroups[j]);
		printf ("%s (%ld) ", ptr?:"?", (long)suppGroups[j]);
	}
	printf ("\n");

	return 0;
}

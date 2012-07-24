/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Write a program that displays the permissions from the ACL entry that
 * corresponds to a particular user or group. The program should take two
 * cmdline args:
 *     1) a 'u' or 'g' indicating whether the second cmdline arg is a user
 *        or a group
 *     2) identifies a particular user or group
 *
 * The functions defined in listing 8-1 (on page 159) can be used to allow
 * the second cmdline arg to be specified either numerically or as a name.
 * If the ACL entry that corresponds to the given user or group falls into
 * the group class, the program should additionally display the permissions
 * that would apply after the ACL entry has been modified by the ACL mask
 * entry.
 */

#include <stdio.h>
#include <dirent.h>
#include <acl/libacl.h>
#include <sys/types.h>
#include <sys/acl.h>

#include "libch08.h"

typedef enum {
	USER_e, GROUP_e
} UorG_t;

static void usage (const char *pgm_p);

int
main (int argc, char *argv[])
{
	DIR *cwd_p;
	struct dirent *thisDir_p;
	acl_t acl;
	acl_entry_t entry;
	acl_tag_t tag;
	int entryId;
	uid_t *uid_p, cmdlineUid;
	gid_t *gid_p, cmdlineGid;
	UorG_t entryType;

	// cmdline args
	if (argc != 3) {
		usage (argv[0]);
		return 1;
	}
	switch (argv[1][0]) {
		case 'u':
			entryType = USER_e;
			if (!user_id_from_name (argv[2], &cmdlineUid)) {
				fprintf (stderr, "user_id_from_name() failure\n");
				return 1;
			}
			break;

		case 'g':
			entryType = GROUP_e;
			if (!group_id_from_name (argv[2], &cmdlineGid)) {
				fprintf (stderr, "group_id_from_name() failure\n");
				return 1;
			}
			break;

		default:
			usage (argv[0]);
			return 1;
	}

	cwd_p = opendir (".");
	if (cwd_p == NULL) {
		perror ("opendir(.)");
		return 1;
	}

	while ((thisDir_p = readdir (cwd_p)) != NULL) {
		if (thisDir_p->d_type != DT_REG)
			continue;
		acl = acl_get_file (thisDir_p->d_name, ACL_TYPE_ACCESS);
		if (acl == NULL)
			continue;

		for (entryId=ACL_FIRST_ENTRY; ; entryId=ACL_NEXT_ENTRY) {
			if (acl_get_entry (acl, entryId, &entry) == -1)
				break;
			if (acl_get_tag_type (entry, &tag) == -1)
				break;

			if (entryType == USER_e) {
				if (tag == ACL_USER) {
					uid_p = acl_get_qualifier (entry);
					if (uid_p == NULL)
						break;

					if (*uid_p == cmdlineUid)
						printf ("acl user matches: %s\n", thisDir_p->d_name);
					
					acl_free (uid_p);
				}
			}
			else {
				if (tag == ACL_GROUP) {
					gid_p = acl_get_qualifier (entry);
					if (gid_p == NULL)
						break;

					if (*gid_p == cmdlineGid)
						printf ("acl grp  matches: %s\n", thisDir_p->d_name);

					acl_free (gid_p);
				}
			}
		}

		acl_free (acl);
	}

	closedir (cwd_p);

	return 0;
}

static void
usage (const char *pgm_p)
{
	printf ("display the permissions for the ACL entry that correponds\n");
	printf (" to a particular user or group\n");
	printf ("usage: %s <u|g> <user or group>\n", pgm_p);
}

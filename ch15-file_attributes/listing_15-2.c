/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the use of chown().
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>

static bool
user_id_from_name (const char *name_p, uid_t *uidRtn_p)
{
	struct passwd *pwd_p;
	uid_t u;
	char *end_p;

	if (name_p == NULL || *name_p == 0)
		return false;

	// check for a number
	u = strtol (name_p, &end_p, 10);
	if (*end_p == 0) {
		*uidRtn_p = u;
		return true;
	}

	pwd_p = getpwnam (name_p);
	if (pwd_p == NULL)
		return false;

	*uidRtn_p = pwd_p->pw_uid;
	return true;
}

static bool
group_id_from_name (const char *name_p, gid_t *gidRtn_p)
{
	struct group *grp_p;
	gid_t g;
	char *end_p;

	if (name_p == NULL || *name_p == 0)
		return false;

	// check for a number
	g = strtol (name_p, &end_p, 10);
	if (*end_p == 0) {
		*gidRtn_p = g;
		return true;
	}

	grp_p = getgrnam (name_p);
	if (grp_p == NULL)
		return false;

	*gidRtn_p = grp_p->gr_gid;
	return true;
}

int
main (int argc, char *argv[])
{
	uid_t uid;
	gid_t gid;
	int j;

	if (argc < 3 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <owner> <group> [<file>[...]]\n", argv[0]);
		printf ("  note:\n");
		printf ("    <owner> or <group> can be '-' meaning 'leave unchanged'\n");
		return 1;
	}

	if (strcmp (argv[1], "-") == 0)
		uid = -1;
	else {
		if (!user_id_from_name (argv[1], &uid)) {
			printf ("uid error\n");
			return 1;
		}
	}

	if (strcmp (argv[2], "-") == 0)
		gid = -1;
	else {
		if (!group_id_from_name (argv[2], &gid)) {
			printf ("gid error\n");
			return 1;
		}
	}

	for (j=3; j<argc; ++j) {
		if (chown (argv[j], uid, gid) == -1) {
			perror ("chown()");
			return 1;
		}
	}

	return 0;
}

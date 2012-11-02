/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Lesser General Public License (Version 3) 
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>

char *
user_name_from_id (uid_t uid)
{
	struct passwd *pwd_p;

	pwd_p = getpwuid (uid);
	return (pwd_p == NULL)? NULL : pwd_p->pw_name;
}

bool
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

char *
group_name_from_id (gid_t gid)
{
	struct group *grp_p;

	grp_p = getgrgid (gid);
	return (grp_p == NULL)? NULL : grp_p->gr_name;
}

bool
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

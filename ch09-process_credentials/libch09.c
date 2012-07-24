/*
 * Copyright (C) 2012  Trevor Woerner
 */

#include <stdio.h>
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

char *
group_name_from_id (gid_t gid)
{
	struct group *grp_p;

	grp_p = getgrgid (gid);
	return (grp_p == NULL)? NULL : grp_p->gr_name;
}

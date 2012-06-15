/*
 * Copyright (C) 2012  Trevor Woerner
 */

// implement getpwnam() using setpwent(), getpwent(), and endpwent()

#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <sys/types.h>

struct passwd *my_getpwnam (const char *name);

int
main (int argc, char *argv[])
{
	struct passwd *pwinfo_p;

	if (argc != 2) {
		printf ("usage\n");
		return 1;
	}

	pwinfo_p = my_getpwnam (argv[1]);
	if (pwinfo_p == NULL) {
		printf ("NULL\n");
		return 1;
	}

	printf ("uid: %u\n", pwinfo_p->pw_uid);
	return 0;
}

struct passwd *
my_getpwnam (const char *name)
{
	struct passwd *pwinfo_p;

	while ((pwinfo_p = getpwent ()) != NULL)
		if (strncmp (pwinfo_p->pw_name, name, strlen (name)) == 0) {
			endpwent ();
			return pwinfo_p;
		}

	return NULL;
}

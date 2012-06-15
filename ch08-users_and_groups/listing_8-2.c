/*
 * Copyright (C) 2012  Trevor Woerner
 */

#define _BSD_SOURCE
#define _XOPEN_SOURCE

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <pwd.h>
#include <shadow.h>

int
main (void)
{
	char *username_p, *password_p, *encrypted_p, *ptr;
	struct passwd *pwd_p;
	struct spwd *spwd_p;
	bool authOK;
	size_t len;
	long lnmax;

	lnmax = sysconf (_SC_LOGIN_NAME_MAX);
	if (lnmax == -1)
		lnmax = 256;

	username_p = malloc (lnmax);
	if (username_p == NULL) {
		perror ("malloc()");
		return 1;
	}

	printf ("username: ");
	fflush (stdout);
	if (fgets (username_p, lnmax, stdin) == NULL) {
		perror ("fgets()");
		return 1;
	}

	len = strlen (username_p);
	if (username_p[len-1] == '\n')
		username_p[len-1] = 0;

	pwd_p = getpwnam (username_p);
	if (pwd_p == NULL) {
		printf ("couldn't get password record\n");
		return 1;
	}
	spwd_p = getspnam (username_p);
	if (spwd_p == NULL && errno == EACCES) {
		printf ("no permission to read shadow password file\n");
		return 1;
	}

	if (spwd_p != NULL)
		pwd_p->pw_passwd = spwd_p->sp_pwdp;

	password_p = getpass ("password: ");

	encrypted_p = crypt (password_p, pwd_p->pw_passwd);
	for (ptr=password_p; *ptr!=0;)
		*ptr++ = 0;
	if (encrypted_p == NULL) {
		perror ("crypt()");
		return 1;
	}

	authOK = strcmp (encrypted_p, pwd_p->pw_passwd) == 0;
	if (!authOK) {
		printf ("incorrect password\n");
		return 1;
	}

	printf ("successfully authenticated UID=%ld\n", (long)pwd_p->pw_uid);
	return 0;
}

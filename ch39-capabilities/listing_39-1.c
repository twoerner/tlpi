/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program uses the libcap API to raise CAP_DAC_READ_SEARCH in its
 * effective capability in order to read the shadow password file, and then
 * drops the capability immediately after this access. In order for an
 * unprivileged user to run this program, the file capability set must be
 * modified as follows:
 *
 *	$ sudo setcap "cap_dac_read_search=p" ./listing_39-1
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <shadow.h>
#include <errno.h>
#include <unistd.h>
#include <crypt.h>
#include <sys/types.h>

#include "ch39lib.h"

int
main (void)
{
	char *username_p, *password_p, *encrypted_p, *ptr;
	struct passwd *pwd_p;
	struct spwd *spwd_p;
	bool authOk;
	size_t len;
	long lnmax;

	lnmax = sysconf (_SC_LOGIN_NAME_MAX);
	if (lnmax == -1)
		lnmax = 256;

	username_p = (char*)malloc (lnmax);
	if (username_p == NULL) {
		perror ("malloc()");
		return 1;
	}

	printf ("username: ");
	fflush (stdout);
	if (fgets (username_p, lnmax, stdin) == NULL)
		return 1;
	len = strlen (username_p);
	if (username_p[len - 1] == '\n')
		username_p[len - 1] = 0;

	pwd_p = getpwnam (username_p);
	if (pwd_p == NULL) {
		perror ("getpwnam()");
		return 1;
	}

	if (raise_cap (CAP_DAC_READ_SEARCH) == -1) {
		perror ("raise_cap()");
		return 1;
	}

	spwd_p = getspnam (username_p);
	if (spwd_p == NULL && errno == EACCES) {
		perror ("getspnam()");
		return 1;
	}

	if (drop_all_caps () == -1) {
		perror ("drop_all_caps()");
		return 1;
	}

	if (spwd_p != NULL)
		pwd_p->pw_passwd = spwd_p->sp_pwdp;

	password_p = getpass ("password: ");
	encrypted_p = crypt (password_p, pwd_p->pw_passwd);
	for (ptr=password_p; *ptr; ++ptr)
		*ptr = 0;
	if (encrypted_p == NULL) {
		printf ("crypt() error\n");
		return 1;
	}

	authOk = strcmp (encrypted_p, pwd_p->pw_passwd) == 0;
	if (!authOk) {
		printf ("incorrect password\n");
		return 1;
	}

	printf ("successfully authenticated UID:%ld\n", (long)pwd_p->pw_uid);

	return 0;
}

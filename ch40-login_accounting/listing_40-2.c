/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/* This program uses some of the functions described in section 40.3  */
/* to dump the contents of a utmpx-format file.                       */

#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <utmpx.h>
#include <paths.h>

int
main (int argc, char *argv[])
{
	struct utmpx *ut_p;

	if (argc > 1 && strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s [<utmp-path>]\n", argv[0]);
		return 1;
	}

	if (argc > 1)
		if (utmpxname (argv[1]) == -1) {
			perror ("utmpxname()");
			return 1;
		}

	setutxent ();

	printf ("user     type      PID   line   id  host      date/time\n");
	while ((ut_p = getutxent ()) != NULL) {
		printf ("%-8s ", ut_p->ut_user);
		printf ("%-9.9s ",
				(ut_p->ut_type == EMPTY)? "EMPTY" :
				(ut_p->ut_type == RUN_LVL)? "RUN_LVL" :
				(ut_p->ut_type == BOOT_TIME)? "BOOT_TIME" :
				(ut_p->ut_type == NEW_TIME)? "NEW_TIME" :
				(ut_p->ut_type == OLD_TIME)? "OLD_TIME" :
				(ut_p->ut_type == INIT_PROCESS)? "INIT_PROCESS" :
				(ut_p->ut_type == LOGIN_PROCESS)? "LOGIN_PROCESS" :
				(ut_p->ut_type == USER_PROCESS)? "USER_PROCESS" :
				(ut_p->ut_type == DEAD_PROCESS)? "DEAD_PROCESS" : "????");
		printf ("%5ld %-6.6s %-3.5s %-9.9s ", (long)ut_p->ut_pid, ut_p->ut_line, ut_p->ut_id, ut_p->ut_host);
		printf ("%s", ctime ((time_t*)&(ut_p->ut_tv.tv_sec)));
	}

	endutxent ();
	return 0;
}

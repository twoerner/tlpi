/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program displays data from a process accounting file.
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <sys/acct.h>
#include <sys/types.h>
#include <sys/stat.h>

#define BUF_SZ 128

static long long compt2ll (comp_t ct);
static char* user_name_from_id (uid_t uid);

int
main (int argc, char *argv[])
{
	int acctFd;
	struct acct_v3 ac;
	ssize_t numRead;
	char *ptr;
	char timeBuf[BUF_SZ];
	struct tm *loc_p;
	time_t t;

	if (argc != 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <file>\n", argv[0]);
		return 1;
	}

	acctFd = open (argv[1], O_RDONLY);
	if (acctFd == -1) {
		perror ("open()");
		return 1;
	}

	printf ("command\t\tflags\tterm.\tuser\t\tstart time\t\tCPU\telapsed\n");
	printf ("\t\t\tstatus\t\t\t\t\t\ttime\ttime\n");

	while ((numRead = read (acctFd, &ac, sizeof (struct acct))) > 0) {
		if (numRead != sizeof (struct acct)) {
			printf ("partial read\n");
			return 1;
		}

		// command
		printf ("%-8.8s\t", ac.ac_comm);

		// flags
		printf ("%c", (ac.ac_flag & AFORK)? 'F' : '-');
		printf ("%c", (ac.ac_flag & ASU)? 'S' : '-');
		printf ("%c", (ac.ac_flag & AXSIG)? 'X' : '-');
		printf ("%c", (ac.ac_flag & ACORE)? 'C' : '-');

		// exit/terminal status
		printf ("\t%#6lx\t", (unsigned long)ac.ac_exitcode);

		// user
		ptr = user_name_from_id (ac.ac_uid);
		printf ("%-8.8s\t", (ptr == NULL)? "???" : ptr);

		// start time
		t = ac.ac_btime;
		loc_p = localtime (&t);
		if (loc_p == NULL)
			printf ("???");
		else {
			strftime (timeBuf, BUF_SZ, "%Y-%m-%d %T", loc_p);
			printf ("%s\t", timeBuf);
		}

		// cpu + elapsed time
		printf ("%5.2f\t%7.2f\t", (double)(compt2ll (ac.ac_utime) + compt2ll (ac.ac_stime)) / sysconf (_SC_CLK_TCK), (double)ac.ac_etime / sysconf (_SC_CLK_TCK));

		// end
		printf ("\n");
	}

	if (numRead == -1)
		return 1;

	return 0;
}

static long long
compt2ll (comp_t ct)
{
	const unsigned EXP_SZ = 3;
	const unsigned MANTISSA_SZ = 13;
	const unsigned MANTISSA__mask = (1<< MANTISSA_SZ) - 1;
	long long mantissa, exp;

	mantissa = ct & MANTISSA__mask;
	exp = (ct >> MANTISSA_SZ) & ((1 << EXP_SZ) - 1);
	return mantissa << (exp * 3);
}

static char *
user_name_from_id (uid_t uid)
{
	struct passwd *pwd_p;

	pwd_p = getpwuid (uid);
	return (pwd_p == NULL)? NULL : pwd_p->pw_name;
}

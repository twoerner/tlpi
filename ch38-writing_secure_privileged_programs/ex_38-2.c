/*
 * Copyright (C) 2012  Trevor Woerner
 */

/*
 * Write a set-user-ID-root program similar to the sudo(8) program. This
 * program should take cmdline options and args as follows:
 *
 *	$ ./ex_38-2 [-u <user>] <pgm> [<args>...]
 *
 * This program executes <pgm>, with its optional arguments, as though it was
 * run by <user>. If the '-u' option is omitted, then <user> should default to
 * 'root'.
 */

#include <stdio.h>
#include <stdlib.h>
#define __USE_GNU
#include <unistd.h>
#include <pwd.h>
#include <sys/wait.h>
#include <sys/types.h>

static void usage (const char *pgm_p);

int
main (int argc, char *argv[])
{
	const char *user_p = "root";
	char *pgm_p = NULL;
	int opt, i;
	pid_t chldPid;
	int status;

	// optional cmdline args
	while ((opt = getopt (argc, argv, "u:")) != -1) {
		switch (opt) {
			case 'u':
				user_p = optarg;
				break;

			default:
				usage (argv[0]);
				return 1;
		}
	}

	// required cmdline args
	if (optind < argc) {
		pgm_p = argv[optind];
	}
	else {
		printf ("required cmdline arg missing:\n\n");
		usage (argv[0]);
		return 1;
	}
	printf ("optind %d  argc:%d\n", optind, argc);
	printf ("pgm:%s  user:%s  pgm-opts:", pgm_p, user_p);
	for (i=optind+1; i<argc; ++i)
		printf ("%s ", argv[i]);
	printf ("\n");

	chldPid = fork ();
	switch (chldPid) {
		case -1:
			perror ("fork()");
			return 1;

		case 0: { // child
			int ret;
			struct passwd *pwentry_p;

			setbuf (stdout, NULL);

			pwentry_p = getpwnam (user_p);
			if (pwentry_p == NULL) {
				perror ("getpwnam()");
				_exit (1);
			}
			ret = setresuid (pwentry_p->pw_uid, pwentry_p->pw_uid, pwentry_p->pw_uid);
			if (ret == -1) {
				perror ("setresuid()");
				_exit (1);
			}

			printf ("running '%s' as user '%s'\n", pgm_p, user_p);
			execvp (pgm_p, &argv[optind]);
			perror ("execvp()");
			_exit (127);
		}

		default: // parent
			waitpid (chldPid, &status, 0);
			break;
	}

	if (WIFEXITED (status))
		return WEXITSTATUS (status);

	return 125;
}

static void
usage (const char *pgm_p)
{
	printf ("usage: %s [-u <user>] <pgm> [<args>...]\n", pgm_p);
	printf ("  where:\n");
	printf ("    <user>      user as which to run <pgm>\n");
	printf ("    <pgm>       the program to run\n");
	printf ("    <args>      arguments to <pgm>\n");
}

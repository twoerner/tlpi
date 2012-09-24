/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the nonreentrant nature of the crypt() function.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <crypt.h>

static char *str2_pG;
static int handled_G = 0;

static void
handler (int __attribute__((unused)) sig)
{
	crypt (str2_pG, "xx");
	++handled_G;
}

int
main (int argc, char *argv[])
{
	char *cr1_p;
	int callNum, mismatch;
	struct sigaction sa;

	if (argc != 3) {
		printf ("usage: %s <str1> <str2>\n", argv[0]);
		return 1;
	}

	str2_pG = argv[2];
	cr1_p = strdup (crypt (argv[1], "xx"));
	if (cr1_p == NULL) {
		perror ("crypt()");
		return 1;
	}

	memset (&sa, 0, sizeof (sa));
	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (sigaction (SIGINT, &sa, NULL) == -1) {
		perror ("sigaction()");
		return 1;
	}

	for (callNum=1,mismatch=0; ; ++callNum) {
		if (strcmp (crypt (argv[1], "xx"), cr1_p) != 0) {
			++mismatch;
			printf ("mismatch on call %d (mismatched=%d handled=%d)\n", callNum, mismatch, handled_G);
		}
	}

	return 0;
}

/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the use of system(). The program executes a loop
 * that reads a command string, executes it using system(), and then analyzes
 * and displays the value returned by system().
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 256

static void
print_wait_status (const char *msg_p, int status)
{
	if (msg_p != NULL)
		printf ("%s", msg_p);

	if (WIFEXITED (status))
		printf ("child exited, status:%d\n", WEXITSTATUS (status));
	else if (WIFSIGNALED (status)) {
		printf ("child killed by signal %s", strsignal (WTERMSIG (status)));
		if (WCOREDUMP (status))
			printf (" (core dumped)");
		printf ("\n");
	}
	else if (WIFSTOPPED (status))
		printf ("child stopped by signal %s\n", strsignal (WSTOPSIG (status)));
	else if (WIFCONTINUED (status))
		printf ("child continued\n");
	else
		printf ("what happened to this child? (status:0x%08x)\n", (unsigned)status);
}

int
main (void)
{
	char str[MAX_CMD_LEN];
	int status;

	for (;;) {
		printf ("command: ");
		fflush (stdout);

		if (fgets (str, MAX_CMD_LEN, stdin) == NULL)
			break;

		status = system (str);
		printf ("system() returned: status{0x%04x (status>>8:%d status&0xff:%d)}\n",
				(unsigned)status, status>>8, status&0xff);

		if (status == -1) {
			perror ("system()");
			return 1;
		}
		if (WIFEXITED (status) && WEXITSTATUS (status) == 127)
			printf ("could not invoke shell (probably)\n");
		else
			print_wait_status (NULL, status);
	}

	return 0;
}

/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Helper routines for displaying signal sets
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>

void
print_signal_set (FILE *outFile_p, const char *prefix_p, const sigset_t *sigset_p)
{
	int sig, cnt;

	/** -- preconds -- (begin) -- **/
	if (outFile_p == NULL)
		outFile_p = stdout;
	if (prefix_p == NULL) {
		fprintf (stderr, "prefix_p NULL\n");
		return;
	}
	if (sigset_p == NULL) {
		fprintf (stderr, "sigset_p NULL\n");
		return;
	}
	/** -- preconds -- (end)   -- **/

	cnt = 0;
	for (sig=1; sig<NSIG; ++sig) {
		if (sigismember (sigset_p, sig)) {
			++cnt;
			fprintf (outFile_p, "%s%d (%s)\n", prefix_p, sig, strsignal (sig));
		}
	}

	if (cnt == 0)
		fprintf (outFile_p, "%s<empty signal set>\n", prefix_p);
}

int
print_signal_mask (FILE *outFile_p, const char *msg_p)
{
	sigset_t currentMask;

	/** -- preconds -- (begin) -- **/
	if (outFile_p == NULL)
		outFile_p = stdout;
	/** -- preconds -- (end)   -- **/

	if (msg_p != NULL)
		fprintf (outFile_p, "%s", msg_p);
	if (sigprocmask (SIG_BLOCK, NULL, &currentMask) == -1)
		return -1;
	print_signal_set (outFile_p, "\t\t", &currentMask);
	return 0;
}

int
print_pending_signals (FILE *outFile_p, const char *msg_p)
{
	sigset_t pendingSignals;

	/** -- preconds -- (begin) -- **/
	if (outFile_p == NULL)
		outFile_p = stdout;
	/** -- preconds -- (end)   -- **/

	if (msg_p != NULL)
		fprintf (outFile_p, "%s", msg_p);
	if (sigpending (&pendingSignals) == -1)
		return -1;
	print_signal_set (outFile_p, "\t\t", &pendingSignals);
	return 0;
}

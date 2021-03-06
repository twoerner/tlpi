/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * This program demonstrates the difference in signal mask handling for the
 * two types of nonlocal gotos (setjmp()/longjmp() and sigsetjmp()/siglongjmp()).
 * This instance uses setjmp()/longjmp().
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>
#include "listing_20-4.h"

static volatile sig_atomic_t canJump_G = 0;
static jmp_buf env_G;

static void
handler (int sig)
{
	/* NOTE this function uses non-async-signal-safe functions:
	 * printf(), strsignal(), etc
	 */

	printf ("received signal %d (%s), signal mask:\n", sig, strsignal (sig));
	print_signal_mask (stdout, NULL);

	if (canJump_G == 0) {
		printf ("'env_G' buffer not yet set, doing a simple return\n");
		return;
	}
	longjmp (env_G, 1);
}

int
main (void)
{
	struct sigaction sa;

	print_signal_mask (stdout, "signal mask at startup:\n");

	sigemptyset (&sa.sa_mask);
	sa.sa_flags = 0;
	sa.sa_handler = handler;
	if (sigaction (SIGINT, &sa, NULL) == -1) {
		perror ("sigaction()");
		return 1;
	}

	printf ("calling setjmp()\n");
	if (setjmp (env_G) == 0)
		canJump_G = 1;
	else
		print_signal_mask (stdout, "signal mask after jump from handler:\n");

	while (1)
		pause ();

	return 0;
}

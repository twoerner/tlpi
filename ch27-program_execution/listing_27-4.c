/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Using execle() to specify program args as a list.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>

int
main (int argc, char *argv[])
{
	char *envVec[] = {
		"GREET=salut",
		"BYE=adieu",
		NULL
	};
	char *fname_p;

	if (argc != 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <pathname>\n", argv[0]);
		return 1;
	}

	fname_p = strrchr (argv[1], '/');
	if (fname_p)
		++fname_p;
	else
		fname_p = argv[1];

	execle (argv[1], fname_p, "hello world", NULL, envVec);
	perror ("execle()");
	return 1;
}

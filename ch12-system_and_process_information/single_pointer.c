/*
 * Copyright (C) 2012  Trevor Woerner
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void get_name (char **name_pp);

int
main (void)
{
	char *name_p;

	get_name (&name_p);
	printf ("name: %s\n", name_p);
	free (name_p);

	return 0;
}

static void
get_name (char **name_pp)
{
	*name_pp = malloc (strlen ("trevor") + 1);
	if (*name_pp == NULL) {
		perror ("malloc()");
		exit (1);
	}

	strcpy (*name_pp, "trevor");
}

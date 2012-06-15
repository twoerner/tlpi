/*
 * Copyright (C) 2012  Trevor Woerner
 */

#include <stdio.h>
#include <stdlib.h>

int my_add (int a, int b);

int
main (int argc, char *argv[])
{
	int a, b;

	if (argc != 3) {
		printf ("usage\n");
		return 1;
	}

	a = atoi (argv[1]);
	b = atoi (argv[2]);

	printf ("my_add: %d + %d = %d\n", a, b, my_add (a, b));
	return 0;
}

int
my_add (int a, int b)
{
	int answer;

	answer = a + b;
	return answer;
}

/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * In section 4.3.2 we noted that it is not possible to open a file for writing
 * if it is currenting being executed (open() returns -1 with errno = ETXTBSY).
 * Nevertheless, it is possible to do the following from the shell:
 *
 *	$ make ex_18-1
 *	$ ./ex_18-1&
 *	$ vi ex_18-1.c
 *	$ make ex_18-1
 *
 * The last command overwrites the exising executable of the same name. How is
 * this possible? (For a clue, use 'ls -li' to look at the i-node number of the
 * executable file after each compilation).
 */

#include <stdio.h>
#include <unistd.h>

int
main (void)
{
	printf ("hello, world\n");
	while (1)
		pause ();
	return 0;
}

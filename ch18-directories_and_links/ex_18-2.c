/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Why does the call to chmod() in the following code fail?
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

int
main (void)
{
	int fd, ret;

	ret = mkdir ("test", S_IRUSR | S_IWUSR | S_IXUSR);
	if (ret == -1) {
		perror ("mkdir()");
		return 1;
	}

	ret = chdir ("test");
	if (ret == -1) {
		perror ("chdir()");
		return 1;
	}

	fd = open ("myfile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
	if (fd == -1) {
		perror ("open()");
		return 1;
	}

	//ret = symlink ("test/myfile", "../mylink");
	ret = symlink ("myfile", "../mylink");
	if (ret == -1) {
		perror ("symlink()");
		return 1;
	}

	ret = chmod ("../mylink", S_IRUSR);
	if (ret == -1) {
		perror ("chmod()");
		return 1;
	}

	return 0;
}

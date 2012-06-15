/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Implement dup() and dup2() using fcntl() and, where necessary, close(). (You may
 * ignore the fact that dup2() and fcntl() return different 'errno' values for some error
 * cases.) For dup2(), remember to handle the special case where 'oldfd' equals 'newfd'. In
 * this case, you should check whether 'oldfd' is valid, which can be done by, for example,
 * checking if fcntl (oldfd, F_GETFL) succeeds. If 'oldfd' is not valid, then the function
 * should return -1 with 'errno' set to 'EBADF'.
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int mydup (int oldfd);
int mydup2 (int oldfd, int newfd);

int
main (void)
{
	int ret;

	ret = mydup (0);
	if (ret == -1) {
		printf ("mydup error\n");
		return 1;
	}
	printf ("mydup(0) = %d\n", ret);

	ret = mydup2 (0, 2);
	if (ret == -1) {
		printf ("mydup2 error\n");
		return 1;
	}
	printf ("mydup2 (0, 2) = %d\n", ret);

	return 0;
}

int
mydup (int oldfd)
{
	int ret;

	errno = 0;
	ret = fcntl (oldfd, F_DUPFD, 0);
	if (ret == -1)
		perror ("fcntl (F_DUPFD)");
	return ret;
}

int
mydup2 (int oldfd, int newfd)
{
	int ret;

	errno = 0;
	if (oldfd == newfd) {
		ret = fcntl (oldfd, F_GETFD);
		if (ret == -1)
			return ret;
		return newfd;
	}

	ret = close (newfd);
	if (ret == -1) {
		perror ("close()");
		return -1;
	}

	ret = fcntl (oldfd, F_DUPFD, newfd);
	if (ret == -1) {
		perror ("fcntl (F_DUPFD)");
		return -1;
	}

	return newfd;
}

/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Implement readv() and writev() using read(), write(), and suitable functions from the
 * 'malloc' package.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>

ssize_t my_readv (int fd, const struct iovec *iov, int iovcnt);
ssize_t my_writev (int fd, const struct iovec *iov, int iovcnt);

int
main (int argc, char *argv[])
{
}

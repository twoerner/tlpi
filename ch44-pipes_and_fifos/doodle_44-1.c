/*
 * Copyright (C) 2016  Trevor Woerner <twoerner@gmail.com>
 *
 * This doodle is similar to exercise 44-3, but my guess is the children don't
 * have to explicitly close their ends of the pipes, that should be handled by
 * their _exit()s.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>

static char* print_date(void);


int
main (int argc, char *argv[])
{
	int pfd[2];
	int i, tmp;

	if (argc < 2 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "usage: %s <sleeptime>...\n", argv[0]);
		return 1;
	}

	setbuf(stdout, NULL);
	printf("%s:  parent started\n", print_date());

	if (pipe(pfd) == -1) {
		perror("pipe()");
		return 1;
	}

	for (i=1; i<argc; ++i) {
		switch (fork()) {
			case -1:
				perror("fork()");
				return -1;
				break;

			case 0: // child
				tmp = atoi(argv[i]);
				sleep((unsigned)tmp);
				printf("%s:  child %d (PID=%ld) closing\n", print_date(), i, (long)getpid());
				_exit(EXIT_SUCCESS);

			default: // parent
				break;
		}
	}

	close(pfd[1]);
	if (read(pfd[0], &tmp, 1) != 0) {
		perror("read()");
		return 1;
	}
	printf("%s:  parent, all children synced\n", print_date());

	return 0;
}

static char*
print_date(void)
{
	static char buf[32];
	time_t now;

	now = time(NULL);
	memset(buf, 0, sizeof(buf));
	ctime_r(&now, buf);
	buf[strlen(buf) - 1] = 0;
	return buf;
}

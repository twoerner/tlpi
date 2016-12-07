/*
 * Copyright (C) 2016  Trevor Woerner <twoerner@gmail.com>
 * (see LICENSE file included in this repository)
 */

/*
 * Write a program that measures the bandwidth provided by pipes. As
 * command-line arguments, the program should accept:
 * - the number of data blocks to be sent
 * - the size of each data block
 * After creating a pipe, the program splits into two processes:
 * - a child that writes the data blocks to the pipe as fast as possible
 * - a parent that reads the data blocks
 * After all the data has been read, the parent should print the elapsed time
 * required and the bandwidth (bytes transfered per second).
 * Measure the bandwidth for diffent data block sizes.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <locale.h>

#define PIPE_RD 0
#define PIPE_WR 1

static size_t blockCnt_G = 0;
static size_t blockSiz_G = 0;

static bool process_cmdline_args(int argc, char *argv[]);
static void usage(const char *cmd_p);
static struct timespec diff_time(struct timespec start, struct timespec end);

int
main(int argc, char *argv[])
{
	int ret;
	pid_t pid;
	int pipeFd[2];
	uint8_t *buf_p;

	setlocale(LC_NUMERIC, "");

	// process cmdline arguments
	if (!process_cmdline_args(argc, argv))
		exit(EXIT_FAILURE);

	// verify parameters
	if (blockCnt_G == 0) {
		printf("please specify a block count\n\n");
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
	if (blockSiz_G == 0) {
		printf("please specify a block size\n\n");
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	// create pipe
	ret = pipe(pipeFd);
	if (ret != 0) {
		perror("pipe2");
		exit(EXIT_FAILURE);
	}

	// create buffer
	buf_p = (uint8_t*)malloc(blockSiz_G);
	if (buf_p == NULL) {
		perror("malloc()");
		exit(EXIT_FAILURE);
	}

	// fork child
	pid = fork();
	switch (pid) {
		// error
		case -1:
			perror("fork()");
			exit(EXIT_FAILURE);

		// child writes to pipe (closes read)
		case 0: {
			size_t i;

			close(pipeFd[PIPE_RD]);
			for (i=0; i<blockCnt_G; ++i)
				write(pipeFd[PIPE_WR], buf_p, blockSiz_G);
			break;
		}

		// parent reads from pipe (closes write)
		default: {
			struct timespec timeBefore, timeAfter, timeDiff;
			double sec, bw;

			close(pipeFd[PIPE_WR]);

			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timeBefore);
			while (read(pipeFd[PIPE_RD], buf_p, blockSiz_G));
			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &timeAfter);
			close(pipeFd[PIPE_RD]);

			timeDiff = diff_time(timeBefore, timeAfter);
			sec = (double)timeDiff.tv_sec + ((double)timeDiff.tv_nsec * (double)1e-9);
			bw = ((double)blockCnt_G * (double)blockSiz_G) / sec;
			printf("elapsed time: %f sec\n", sec);
			printf("       bytes: %zd\n", blockCnt_G * blockSiz_G);
			printf("   bandwidth: %'.0f bytes/second\n", bw);
			break;
		}
	}

	free(buf_p);
	return 0;
}

static struct timespec
diff_time(struct timespec start, struct timespec end)
{
	struct timespec temp;
	if ((end.tv_nsec-start.tv_nsec)<0) {
		temp.tv_sec = end.tv_sec-start.tv_sec-1;
		temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
	} else {
		temp.tv_sec = end.tv_sec-start.tv_sec;
		temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	}
	return temp;
}

static void
usage(const char *cmd_p)
{
	printf("usage: %s {-s|--block-size} S {-c|--block-cnt} C [options]\n", cmd_p);
	printf("  where\n");
	printf("    -s|--block-size S            specify the block size \"S\"to use\n");
	printf("    -c|--block-cnt  C            specify the block count \"C\" to use\n");
	printf("  options\n");
	printf("    -h|--help                    display this help\n");
}

static bool
process_cmdline_args(int argc, char *argv[])
{
	struct option longOpts[] = {
		{"help", no_argument, NULL, 'h'},
		{"block-cnt", required_argument, NULL, 'c'},
		{"block-size", required_argument, NULL, 's'},
		{NULL, 0, NULL, 0},
	};
	int c;
	size_t tmp;

	while (1) {
		c = getopt_long(argc, argv, "hc:s:", longOpts, NULL);
		if (c == -1)
			break;
		switch (c) {
			case 'h':
				printf("help\n");
				break;

			case 'c':
				if (sscanf(optarg, "%zd", &tmp) != 1) {
					perror("sscanf(c)");
					return false;
				}
				blockCnt_G = tmp;
				break;

			case 's':
				if (sscanf(optarg, "%zd", &tmp) != 1) {
					perror("sscanf(s)");
					return false;
				}
				blockSiz_G = tmp;
				break;

			default:
				printf("getopt_long() returned: %c (0x%0x)\n", c, c);
				return false;
		}
	}

	return true;
}

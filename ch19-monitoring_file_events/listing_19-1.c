/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the inotify API by performing the following steps:
 * - create an inotify descriptor using inotify_init()
 * - add a watch for each file on the cmdline using inotify_add_watch()
 * - display any events
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>
#include <sys/inotify.h>

#define BUF_LEN (10 * (sizeof (struct inotify_event) + NAME_MAX + 1))

static void display_inotify_event (struct inotify_event *ievt_p);

int
main (int argc, char *argv[])
{
	int inotifyFd, watch, i;
	char buf[BUF_LEN];
	ssize_t numRead;
	char *ptr;
	struct inotify_event *event_p;

	if (argc < 2 || strcmp (argv[1], "--help") == 0) {
		printf ("usage: %s <pathname...>\n", argv[0]);
		return 1;
	}

	inotifyFd = inotify_init ();
	if (inotifyFd == -1) {
		perror ("inotify_init()");
		return 1;
	}

	for (i=1; i<argc; ++i) {
		watch = inotify_add_watch (inotifyFd, argv[i], IN_ALL_EVENTS);
		if (watch == -1) {
			perror ("inotify_add_watch()");
			return 1;
		}

		printf ("watching %s using watch %d\n", argv[i], watch);
	}

	while (1) {
		numRead = read (inotifyFd, buf, BUF_LEN);
		if (numRead == 0) {
			printf ("read() from inotifyFd returned 0\n");
			return 1;
		}
		if (numRead == -1) {
			printf ("read() from inotifyFd returned -1\n");
			return 1;
		}
		printf ("read %ld bytes from inotifyFd\n", (long)numRead);

		for (ptr=buf; ptr<buf+numRead; ) {
			event_p = (struct inotify_event*)ptr;
			display_inotify_event (event_p);
			ptr += sizeof (struct inotify_event) + event_p->len;
		}
	}
}

static void
display_inotify_event (struct inotify_event *event_p)
{
	printf ("\twd: %2d; ", event_p->wd);
	if (event_p->cookie > 0)
		printf ("cookie: %4d; ", event_p->cookie);
	printf ("mask: ");
	if (event_p->mask & IN_ACCESS)
		printf ("IN_ACCESS");
	if (event_p->mask & IN_ATTRIB)
		printf ("IN_ATTRIB");
	if (event_p->mask & IN_CLOSE_NOWRITE)
		printf ("IN_CLOSE_NOWRITE");
	if (event_p->mask & IN_CLOSE_WRITE)
		printf ("IN_CLOSE_WRITE");
	if (event_p->mask & IN_CREATE)
		printf ("IN_CREATE");
	if (event_p->mask & IN_DELETE)
		printf ("IN_DELETE");
	if (event_p->mask & IN_DELETE_SELF)
		printf ("IN_DELETE_SELF");
	if (event_p->mask & IN_IGNORED)
		printf ("IN_IGNORED");
	if (event_p->mask & IN_ISDIR)
		printf ("IN_ISDIR");
	if (event_p->mask & IN_MODIFY)
		printf ("IN_MODIFY");
	if (event_p->mask & IN_MOVE_SELF)
		printf ("IN_MOVE_SELF");
	if (event_p->mask & IN_MOVED_FROM)
		printf ("IN_MOVED_FROM");
	if (event_p->mask & IN_MOVED_TO)
		printf ("IN_MOVED_TO");
	if (event_p->mask & IN_OPEN)
		printf ("IN_OPEN");
	if (event_p->mask & IN_Q_OVERFLOW)
		printf ("IN_Q_OVERFLOW");
	if (event_p->mask & IN_UNMOUNT)
		printf ("IN_UNMOUNT");
	printf ("\n");

	if (event_p->len > 0)
		printf ("\tname: %s\n", event_p->name);
}

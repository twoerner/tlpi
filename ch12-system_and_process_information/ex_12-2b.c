/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Write a program that draws a tree showing the hierarchical parent-child
 * relationships of all processes on a system, going all the way back to 'init'. For each
 * process, the program should display the process ID, and the command being
 * executed. The output of the program should be similar to that produced by pstree(1),
 * although it does not need to be as sophisticated. The parent of each process on the
 * system can be found by inspecting the 'PPid:' line of all the /proc/PID/status files
 * on the system. Be careful to handle the possibility that a process's parent (and thus
 * its /proc/PID directory) disappears during the scan of all /proc/PID directories.
 */

#define _BSD_SOURCE
#define LINE_SZ 128

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <alloca.h>

#include "ch12_util.h"

typedef struct {
	bool valid;
	bool printed;
	char name[LINE_SZ];
	unsigned pid;
	unsigned ppid;
	unsigned chldCnt;
	unsigned chldPrinted;
} ProcInfo_t;
static ProcInfo_t *procInfo_pG;
static unsigned procInfoCnt_G = 0;

static void print_proc_and_children (unsigned idx, unsigned level, unsigned *pList_p);
static unsigned get_child_count (unsigned pid);

int
main (void)
{
	unsigned i;
	char **procDirs_pp;
	char fname[PATH_MAX];
	char lineStr[LINE_SZ];
	char cmdStr[LINE_SZ];
	char ppidStr[LINE_SZ];
	unsigned info;
	FILE *statusFile_p;

	if (!get_proc_dirs (&procInfoCnt_G, &procDirs_pp)) {
		printf ("can't get /proc directory list\n");
		return 1;
	}

	procInfo_pG = (ProcInfo_t*)malloc (procInfoCnt_G * sizeof (ProcInfo_t));
	if (procInfo_pG == NULL) {
		perror ("malloc()");
		return 1;
	}
	memset (procInfo_pG, 0, procInfoCnt_G * sizeof (ProcInfo_t));

	// process list
	for (i=0; i<procInfoCnt_G; ++i) {
		// open /proc/PID/status file
		if (snprintf (fname, PATH_MAX, "/proc/%s/status", procDirs_pp[i]) >= PATH_MAX) {
			printf ("couldn't create /proc fname with %s\n", procDirs_pp[i]);
			continue;
		}
		statusFile_p = fopen (fname, "r");
		if (statusFile_p == NULL) {
			perror ("fopen()");
			continue;
		}

		info = 0;
		while (fgets (lineStr, LINE_SZ, statusFile_p)) {
			if (strncmp (lineStr, "Name:", 5) == 0) {
				++info;
				memcpy (cmdStr, lineStr, LINE_SZ);
			}
			if (strncmp (lineStr, "PPid:", 5) == 0) {
				++info;
				memcpy (ppidStr, lineStr, LINE_SZ);
			}
			if (info == 2)
				break;
		}
		fclose (statusFile_p);

		if (info == 2) {
			procInfo_pG[i].valid = true;
			strcpy (procInfo_pG[i].name, &cmdStr[6]);
			if (strchr (procInfo_pG[i].name, '\n'))
				*(strchr (procInfo_pG[i].name, '\n')) = 0;
			sscanf (procDirs_pp[i], "%u", &procInfo_pG[i].pid);
			sscanf (&ppidStr[6], "%u", &procInfo_pG[i].ppid);
		}
	}

	// figure out child counts
	for (i=0; i<procInfoCnt_G; ++i)
		procInfo_pG[i].chldCnt = get_child_count (procInfo_pG[i].pid);

	for (i=0; i<procInfoCnt_G; ++i)
		if (procInfo_pG[i].valid && !procInfo_pG[i].printed)
			print_proc_and_children (i, 0, NULL);

	free (procInfo_pG);
	free_proc_dirs (procInfoCnt_G, &procDirs_pp);

	return 0;
}

static void
print_proc_and_children (unsigned thisIdx, unsigned level, unsigned *pList_p)
{
	unsigned i, chldIdx;
	unsigned *list_p;

	if (!procInfo_pG[thisIdx].valid)
		return;
	if (procInfo_pG[thisIdx].printed)
		return;

	// print this item
	if (level >= 1) {
		for (i=0; i<level; ++i) {
			chldIdx = pList_p[i];
			if (procInfo_pG[chldIdx].chldPrinted == procInfo_pG[chldIdx].chldCnt) {
				printf ("  +");
				++procInfo_pG[chldIdx].chldPrinted;
			}
			else if (procInfo_pG[chldIdx].chldPrinted > procInfo_pG[chldIdx].chldCnt)
				printf ("   ");
			else
				printf ("  |");
		}
	}
	printf ("- %-5u %-5u %s\n", procInfo_pG[thisIdx].pid, procInfo_pG[thisIdx].ppid, procInfo_pG[thisIdx].name);
	procInfo_pG[thisIdx].printed = true;

	// print the children
	if (procInfo_pG[thisIdx].chldCnt > 0) {
		for (chldIdx=0; chldIdx<procInfoCnt_G; ++chldIdx)
			if (procInfo_pG[chldIdx].ppid == procInfo_pG[thisIdx].pid)
				if (procInfo_pG[chldIdx].valid && !procInfo_pG[chldIdx].printed) {
					++procInfo_pG[thisIdx].chldPrinted;

					list_p = alloca ((level + 1) * sizeof (unsigned));
					if (list_p != NULL) {
						for (i=0; i<level; ++i) {
//fprintf (stderr, "[%u] adding %u\n", i, list_p[i]);
							list_p[i] = pList_p[i];
						}
//fprintf (stderr, "adding %u\n", i);
						list_p[i] = thisIdx;
					}

					print_proc_and_children (chldIdx, level+1, list_p);
				}
	}
}

static unsigned
get_child_count (unsigned pid)
{
	unsigned i;
	unsigned cnt;

	cnt = 0;
	for (i=0; i<procInfoCnt_G; ++i)
		if (procInfo_pG[i].ppid == pid)
			++cnt;
	return cnt;
}

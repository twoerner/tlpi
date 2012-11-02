/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Lesser General Public License (Version 3)
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * Helper routines for displaying signal sets
 */

#ifndef __LISTING_20_4__H
#define __LISTING_20_4__H

void print_signal_set (FILE *outFile_p, const char *prefix_p, const sigset_t *sigset_p);
int print_signal_mask (FILE *outFile_p, const char *msg_p);
int print_pending_signals (FILE *outFile_p, const char *msg_p);

#endif

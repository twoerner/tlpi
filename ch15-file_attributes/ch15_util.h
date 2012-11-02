/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Lesser General Public License (Version 3) 
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

#ifndef CH15_UILT__H
#define CH15_UILT__H

#include <sys/types.h>

#define FP_SPECIAL 1

char *file_perm_str (mode_t perm, int flags);

#endif

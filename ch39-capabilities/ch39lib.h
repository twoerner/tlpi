/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

#ifndef __CH39_CAPS__H
#define __CH39_CAPS__H

#include <sys/capability.h>

int modify_cap (int capability, int setting);
int raise_cap (int capability);
int drop_all_caps (void);

#endif

/*
 * Copyright (C) 2012, Michael Kerrisk
 * Licensed under the GNU Affero General Public License, version 3
 *
 * this program contains tweaks by Trevor Woerner
 * (see LICENSE file included in this repository)
 */

#include <stdio.h>
#include "ch39lib.h"

int
modify_cap (int capability, int setting)
{
	cap_t caps;
	cap_value_t capList[1];

	caps = cap_get_proc ();
	if (caps == NULL)
		return -1;

	capList[0] = capability;
	if (cap_set_flag (caps, CAP_EFFECTIVE, 1, capList, setting) == -1) {
		cap_free (caps);
		return -1;
	}

	if (cap_set_proc (caps) == -1) {
		cap_free (caps);
		return -1;
	}

	if (cap_free (caps) == -1)
		return -1;

	return 0;
}

int
raise_cap (int capability)
{
	return modify_cap (capability, CAP_SET);
}

int
drop_all_caps (void)
{
	cap_t empty;
	int ret;

	empty = cap_init ();
	if (empty == NULL)
		return -1;

	ret = cap_set_proc (empty);
	
	if (cap_free (empty) == -1)
		return -1;

	return ret;
}

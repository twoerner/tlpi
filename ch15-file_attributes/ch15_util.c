/*
 * Copyright (C) 2012  Trevor Woerner
 */

#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ch15_util.h"

#define STRSZ 10

char *
file_perm_str (mode_t perm, int flags)
{
	static char str[STRSZ];

	snprintf (str, STRSZ, "%c%c%c%c%c%c%c%c%c",
			(perm & S_IRUSR)? 'r' : '-',
			(perm & S_IWUSR)? 'w' : '-',
			(perm & S_IXUSR)?
				(((perm & S_ISUID) && (flags & FP_SPECIAL))? 's' : 'x') :
				(((perm & S_ISUID) && (flags & FP_SPECIAL))? 'S' : '-'),
			(perm & S_IRGRP)? 'r' : '-',
			(perm & S_IWGRP)? 'w' : '-',
			(perm & S_IXGRP)?
				(((perm & S_ISGID) && (flags & FP_SPECIAL))? 's' : 'x') :
				(((perm & S_ISGID) && (flags & FP_SPECIAL))? 'S' : '-'),
			(perm & S_IROTH)? 'r' : '-',
			(perm & S_IWOTH)? 'w' : '-',
			(perm & S_IXOTH)?
				(((perm & S_ISVTX) && (flags & FP_SPECIAL))? 't' : 'x') :
				(((perm & S_ISVTX) && (flags & FP_SPECIAL))? 'T' : '-'));

	return str;
}

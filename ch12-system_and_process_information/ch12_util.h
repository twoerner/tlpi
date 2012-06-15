/*
 * Copyright (C) 2012  Trevor Woerner
 */

#ifndef CH12_EXERCISES_UTIL__H
#define CH12_EXERCISES_UTIL__H

#include <stdbool.h>

bool get_proc_dirs (unsigned *cnt_p, char ***procDirs_ppp);
void free_proc_dirs (unsigned cnt, char ***procDirs_ppp);

#endif

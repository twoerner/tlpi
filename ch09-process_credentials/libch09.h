/*
 * Copyright (C) 2012  Trevor Woerner
 */

#ifndef TLPI_LIBCH09__H
#define TLPI_LIBCH09__H

#include <pwd.h>
#include <grp.h>
#include <sys/types.h>

char *user_name_from_id (uid_t uid);
char *group_name_from_id (gid_t git);

#endif

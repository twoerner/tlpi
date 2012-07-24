/*
 * Copyright (C) 2012  Trevor Woerner
 */

#ifndef TLPI_LIBCH09__H
#define TLPI_LIBCH09__H

#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>

char *user_name_from_id (uid_t uid);
bool user_id_from_name (const char *name_p, uid_t *uid_p);
char *group_name_from_id (gid_t git);
bool group_id_from_name (const char *name_p, gid_t *gid_p);

#endif

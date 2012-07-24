/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * This program demonstrates the use of some of the ACL library functions. It
 * retrieves and displays the ACL on a file (i.e. it provides a subset of the
 * functionality of the "getfacl" command). If the '-d' cmdline option is
 * specified, then the program displays the default ACL (of a directory) instead
 * of the access ACL.
 *
 *	$ touch tfile
 *	$ setfacl -m 'u:uucp:r,u:nobody:rw,g:video:r' tfile
 *	$ ./listing_17-1 tfile
 *	user_obj			rw-
 *	user		uucp		r--
 *	user		nobody		rw-
 *	group_obj			r--
 *	group		video		r--
 *	mask				rw-
 *	other				r--
 */

#include <stdio.h>
#include <unistd.h>
#include <acl/libacl.h>
#include <sys/acl.h>

#include "libch09.h"

static void usage (const char *pgm_p);

int
main (int argc, char *argv[])
{
	acl_t acl;
	acl_type_t type;
	acl_entry_t entry;
	acl_tag_t tag;
	uid_t *uid_p;
	gid_t *gid_p;
	acl_permset_t permset;
	char *name_p;
	int entryId, permVal, opt;

	type = ACL_TYPE_ACCESS;
	while ((opt = getopt (argc, argv, "d")) != -1) {
		switch (opt) {
			case 'd':
				type = ACL_TYPE_DEFAULT;
				break;

			case '?':
				usage (argv[0]);
				return 1;
		}
	}

	if (optind + 1 != argc) {
		usage (argv[0]);
		return 1;
	}

	acl = acl_get_file (argv[optind], type);
	if (acl == NULL) {
		perror ("acl_get_file()");
		return 1;
	}

	for (entryId=ACL_FIRST_ENTRY; ; entryId=ACL_NEXT_ENTRY) {
		if (acl_get_entry (acl, entryId, &entry) != 1)
			break;
		if (acl_get_tag_type (entry, &tag) == -1) {
			perror ("acl_get_tag_type()");
			return 1;
		}

		printf ("%-12s",
				(tag == ACL_USER_OBJ)? "user_obj" :
				(tag == ACL_USER)? "user" :
				(tag == ACL_GROUP_OBJ)? "group_obj" :
				(tag == ACL_GROUP)? "group" :
				(tag == ACL_MASK)? "mask" :
				(tag == ACL_OTHER)? "other" :
				"???");

		// optional user/group ACLs
		if (tag == ACL_USER) {
			uid_p = acl_get_qualifier (entry);
			if (uid_p == NULL) {
				perror ("acl_get_qualifier()");
				return 1;
			}
			name_p = user_name_from_id (*uid_p);
			if (name_p == NULL)
				printf ("\t\t%d", *uid_p);
			else
				printf ("\t\t%s", name_p);

			if (acl_free (uid_p) == -1)
				perror ("acl_free (uid_p)");
		}
		else if (tag == ACL_GROUP) {
			gid_p = acl_get_qualifier (entry);
			if (gid_p == NULL) {
				perror ("acl_get_qualifier()");
				return 1;
			}

			name_p = group_name_from_id (*gid_p);
			if (name_p == NULL)
				printf ("\t\t%d", *gid_p);
			else
				printf ("\t\t%s", name_p);

			if (acl_free (gid_p) == -1)
				perror ("acl_free (gid_p)");
		}
		else
			printf ("\t\t");

		// permissions
		if (acl_get_permset (entry, &permset) == -1) {
			perror ("acl_get_permset()");
			return 1;
		}
		if ((permVal = acl_get_perm (permset, ACL_READ)) == -1) {
			perror ("acl_get_perm (ACL_READ)");
			return 1;
		}
		printf ("\t\t%c", (permVal == 1)? 'r' : '-');
		if ((permVal = acl_get_perm (permset, ACL_WRITE)) == -1) {
			perror ("acl_get_perm (ACL_WRITE)");
			return 1;
		}
		printf ("%c", (permVal == 1)? 'w' : '-');
		if ((permVal = acl_get_perm (permset, ACL_EXECUTE)) == -1) {
			perror ("acl_get_perm (ACL_EXECUTE)");
			return 1;
		}
		printf ("%c", (permVal == 1)? 'x' : '-');

		printf ("\n");
	}

	if (acl_free (acl) == -1) {
		perror ("acl_free (acl)");
		return 1;
	}

	return 0;
}

static void
usage (const char *pgm_p)
{
	printf ("usage: %s [-d] <file>\n", pgm_p);
	printf ("  option:\n");
	printf ("    -d        show default (instead of access) ACL\n");
}

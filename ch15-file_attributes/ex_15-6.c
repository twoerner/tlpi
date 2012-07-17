/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * The "chmod a+rX <file>" command enables read permission for all categories of user,
 * and likewise enables execute permission for all categories of user if <file> is a directory
 * or execute permission is enabled for any of the user categories for <file>, as
 * demonstrated in the following example:
 *
 *	$ ls -ld dir file prog
 *	dr-------- 2 mtk users       48 May  4 12:28  dir
 *	-r-------- 2 mtk users    19794 May  4 12:28  file
 *	-r-x------ 2 mtk users    19336 May  4 12:28  prog
 *	$ chmod a+rX dir file prog
 *	$ls -ld dir file prog
 *	dr-------- 2 mtk users       48 May  4 12:28  dir
 *	-r-------- 2 mtk users    19794 May  4 12:28  file
 *	-r-x------ 2 mtk users    19336 May  4 12:28  prog
 */


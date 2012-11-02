/*
 * Copyright (C) 2012  Trevor Woerner
 * (see LICENSE file included in this repository)
 */

/*
 * The 'tee' command reads its standard input until EOF, writing a copy of the input
 * to standard output and to the file named in its cmdline arg. Implement 'tee' using
 * I/O system calls. By default, 'tee' overwrites any existing file with the given name.
 * Implement the '-a' cmdline option (tee -a <file>), which causes 'tee' to append text
 * to the end of the file if it already exists.
 */


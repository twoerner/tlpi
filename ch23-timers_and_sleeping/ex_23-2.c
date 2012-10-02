/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Try running the program in Listing 23-3 in the background with a 60-second
 * sleep interval, while using the following command to send as many SIGINT
 * signals as possible to the background process:
 *
 *	$ while true; do kill -INT <pid>; done
 *
 * You should observe that the program sleeps rather longer than expected.
 * Replace the use of nanosleep() with the use of clock_gettime() (use a
 * CLOCK_REALTIME clock) and clock_nanosleep() with the TIMER_ABSTIME flag.
 * (This exercise requires Linux 2.6). Repeat the test with the modified
 * program and explain the difference.
 */


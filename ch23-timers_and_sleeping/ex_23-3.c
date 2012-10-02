/*
 * Copyright (C) 2012  Trevor Woerner
 *
 * Write a program to show that if the evp argument to timer_create() is
 * specified as NULL, then this is equivalent to specifying evp as a pointer
 * to a sigevent structure with sigev_notify set to SIGEV_SIGNAL, sigev_signo
 * set to SIGALRM, and si_value.sival_int set to the timer ID.
 */


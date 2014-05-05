/*
 * NAME:
 * error.c
 *
 * PURPOSE:
 * Write error message to screen or wherever needed...
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * NA
 *
 * OUTPUT:
 * NA
 *
 * NOTES:
 * To be phased out, use fmlogmsg and fmerrmsg of libfmutil instead.
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/12/2001
 *
 * MODIFIED:
 * NA
 *
 * CVS_ID:
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>

void error(char *where, char *what) {

    fprintf(stderr," ERROR(%s):\n", where);
    fprintf(stderr," %s\n", what);
}

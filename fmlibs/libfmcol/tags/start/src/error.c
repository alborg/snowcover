/*
 * NAME:
 * error.c
 *
 * PURPOSE:
 * Write error message to screen or wherever needed...
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/12/2001
 */

#include <stdio.h>
#include <stdlib.h>

void error(char *where, char *what) {

    fprintf(stderr," ERROR(%s):\n", where);
    fprintf(stderr," %s\n", what);
}


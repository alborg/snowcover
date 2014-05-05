/*
 * NAME:
 * NA
 *
 * PURPOSE:
 * NA
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
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, METNO/FOU, 31.08.2007 
 *
 * MODIFIED:
 * Steinar Eastwood, met.no, 05.09.2007  Added include time.h.
 * Øystein Godøy, METNO/FOU, 06.09.2007: Moved some include statements
 * from C-file to headerfile. Whether this should be the general practise
 * is still to be determined.
 * Øystein Godøy, METNO/FOU, 05.10.2007: Added fmfindfile.
 *
 * ID:
 * $Id$
 */

#ifndef FMTOUCH_H
#define FMTOUCH_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <utime.h>
#include <sys/stat.h>
#include "fmutil_types.h"
#include "fmstrings.h"
#include <time.h>


char *fmfindfile(char *path, char *datestr, char *timestr, char *areastr);
int fmfileexist(char *filename); 
int fmtouch(char *filename, time_t newtime); 

#endif /* FMTOUCH */

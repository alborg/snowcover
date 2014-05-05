/*
 * NAME:
 * std_stdat.h
 *
 * PURPOSE:
 * Header file for std_stdat.c software.
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
 * Øystein Godøy, DNMI/FOU, 25/01/2002
 *
 * MODIFIED:
 * NA
 *
 * CVS_ID:
 * $Id$
 */

#ifndef _STD_STDAT_H
#define _STD_STDAT_H

#include <fmutil.h>
#include <fmcolaccess.h>

/*
 * Time differences are given in seconds
 */
#define SDIFF 5400 /* Accepted time difference from synop time */
#define TSDIFF 1800 /* Generally acceppted time difference syno */
#define TDDIFF 2700 /* Generally acceppted time difference drau */
#define STDATSTRLEN 1024

int read_stdat(char *ipath, char *basename, fmcol_boolean synop, 
	time_t timeid, stdatstruct *pobs); 
int get_stdat_h(char *infile, stdatstruct *obsl);
int fill_stdat_h(char *infile, time_t vtime, stdatstruct *obsl); 
int init_stdat(stdatstruct *obsl); 
int free_stdat(stdatstruct *obsl);

#endif /*  _STD_STDAT_H */

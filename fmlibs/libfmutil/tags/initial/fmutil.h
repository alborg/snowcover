/*
 * NAME:
 * fmutil.h
 *
 * PURPOSE:
 * Top include file for libfmutil.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 */

#ifndef FMUTIL_H
#define FMUTIL_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#ifndef FMBOOL
#define FMBOOL
typedef enum {FMFALSE, FMTRUE} fmboolean, fmbool;
#endif

#ifndef FMPI
#define fmPI 3.141592654
#endif

#ifndef FMREARTH
#define FMREARTH 6371.
#endif

#ifndef FMRETURNCODES
typedef enum {
    FM_OK,
    FM_SYNTAX_ERR,
    FM_IO_ERR,
    FM_MEMALL_ERR,
    FM_MEMFREE_ERR
} fmreturncode;
#endif

#include <fmcoord.h> /* Definitions of coordinate systems */
#include <fmtime.h> /* Definitions related to time specs */
#include <fmsolar.h> /* Definitions related to Sun specs */
#include <fmangles.h> /* Definitions related to obs angles est. */
#include <fmstrings.h> /* Definitions related to string handling */
#include <fmstorage.h> /* Definitions related to memory allocation and nav. */
#include <fmimage.h> /* Definitions related to images */
#include <fmbyteswap.h> /* Definitions related to byte swapping */
#include <fmcolormaps.h> /* Definitions related to colormaps */

void fmlogmsg(char *where, char *what); 
void fmerrmsg(char *where, char *what); 
double fmrad2deg(double radians); 
double fmdeg2rad(double degrees); 

#endif /* FMUTIL_H */

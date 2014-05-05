/*
 * NAME:
 * safdli_stdat.h
 *
 * PURPOSE:
 * Header file for safdli_stdat.c
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
 * Øystein Godøy, met.no/FOU, 23.12.2004 
 *
 * MODIFIED:
 * NA
 *
 * CVS_ID:
 * $Id$
 */

#ifndef SAFDLI_STDAT_H
#define SAFDLI_STDAT_H

#include <dirent.h>

#include <fmutil.h>
#include <fmcol.h>
#include <fmcolaccess.h>
#ifdef FMCOL_HAVE_LIBOSIHDF5
#include <safhdf.h>
/*
#include <fmcoord.h>
*/

#define SAFDLI_MISVAL -999. 

int safdli_stdat(fmgeopos gpos, fmprojspec myproj,
	osihdf *safdli, safdli_data *dli);
int safdli_init(int boxsize, safdli_data *dli);
int safdli_free(safdli_data *dli);
char *safdli_prod(char *filename);
int safdli_name(fmtime refdate, char *basename, char *path, char *filename);
int safdli_check(fmio_img refdata, osihdf *safdli);

#endif /* FMCOL_HAVE_LIBOSIHDF5 */
#endif /* SAFDLI_STDAT_H */


/*
 * NAME:
 * safssi_stdat.h
 *
 * PURPOSE:
 * Header file for safssi_stdat.c
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
 * Øystein Godøy, DNMI/FOU, 27/09/2002
 *
 * MODIFIED:
 * Øystein Godøy, MI/FOU, 11.04.2003
 *
 * CVS_ID:
 * $Id$
 */

#ifndef _SAFSSI_STDAT_H
#define _SAFSSI_STDAT_H

#include <dirent.h>

#include <fmutil.h>
#include <fmcol.h>
#include <fmcolaccess.h>
#ifdef FMCOL_HAVE_LIBOSIHDF5
#include <safhdf.h>

#define SAFSSI_MISVAL -999. 

int safssi_stdat(fmgeopos gpos, fmprojspec myproj,
	osihdf *safssi, safssi_data *ssi);
int safssi_init(int boxsize, safssi_data *ssi);
int safssi_free(safssi_data *ssi);
char *safssi_prod(char *filename);
int safssi_name(fmtime refdate, char *basename, char *path, char *filename);
int safssi_check(fmio_img refdata, osihdf *safssi);

#endif /* FMCOL_HAVE_LIBOSIHDF5 */
#endif /* _SAFSSI_STDAT_H */


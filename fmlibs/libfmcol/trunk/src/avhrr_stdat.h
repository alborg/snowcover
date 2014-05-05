/*
 * NAME:
 * avhrr_stdat.h
 *
 * PURPOSE:
 * Header file for avhrr_stdat.c, see avhrr_stdat.c for details.
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
 * Øystein Godøy, DNMI/FOU, 06/11/2000
 *
 * MODIFIED:
 * Øystein Godøy, MI/FOU, 11.04.2003
 * Øystein Godøy, met.no/FOU, 22.12.2004
 * Changed storage structure, this needs to be reflected by fmcolaccess.c as
 * well...
 * Øystein Godøy, METNO/FOU, 25.04.2007: Adapted for libfmutil/libfmio.
 *
 * CVS_ID:
 * $Id$
 */ 

#include <stdio.h>
#include <stdlib.h>

#include <fmcol.h>
#include <fmcolaccess.h>
/*
#include <fmcoord.h>
#include <fmsubtrack.h>
*/

#ifndef _AVHRR_STDAT_H
#define _AVHRR_STDAT_H

#define OUTOFIMAGE -40100
#define MISVAL -99999
#define MAXCH 5

int avhrr_stdat(fmgeopos gpos, fmprojspec myproj, fmio_img img, as_data *a);
int avhrr_init(int boxsize, int nochan, as_data *a);
int avhrr_free(as_data *a);
    
#endif /* _AVHRR_STDAT_H */


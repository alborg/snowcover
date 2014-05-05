/*
 * NAME:
 * nwp_stdat.h
 *
 * PURPOSE:
 * Header file for nwp_stdat.c
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
 * Øystein Godøy, DNMI/FOU, 28/01/2002
 *
 * MODIFIED:
 * Øystein Godøy, MI/FOU, 16.04.2003
 * Øystein Godøy, met.no/FOU, 18.10.2004: NWP interface is changed.
 *
 * CVS_ID:
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <fmutil.h>
#include <fmcol.h>
#include <nwp_read.h>
#include <fmcolaccess.h>

/*
#include <fmcoord.h>
*/

#ifndef _NWP_STDAT_H
#define _NWP_STDAT_H

#define OUTOFIMAGE -40100
#define MISVAL -99999
#define UNDEF +1.E+35

int nwp_stdat(fmgeopos gpos, fmprojspec myproj, fmucsref refucs,
	nwpdata nwp, ns_data *n);
int nwp_init(int boxsize, float boxresx, float boxresy, int nopar, ns_data *n);
int nwp_free(ns_data *n);
    
#endif /* _NWP_STDAT_H */


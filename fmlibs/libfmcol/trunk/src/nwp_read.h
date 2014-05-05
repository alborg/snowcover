/*
 * NAME:
 * nwp_read.h
 *
 * PURPOSE:
 * To provide an interface between libusenwp and libfmcol.
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
 * Øystein Godøy, met.no/FOU, 18.10.2004 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 17.10.2007: Removed hardcoded filenames.
 * Øystein Godøy, METNO/FOU, 20.09.2008: Modified to handle HIRLAM12.
 *
 * CVS_ID:
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>

#include <fmutil.h>
#include <fmcol.h>
#include <fmcolaccess.h>

#ifndef NWP_READ
#define NWP_READ

#define NOFIELDS1 5
#define NOFIELDS2 1
#define NOFIELDS3 4
#define NOFIELDS NOFIELDS1+NOFIELDS2+NOFIELDS3

typedef struct {
    fmsec1970 validtime;
    int leadtime;
    fmucsref refucs;
    float *t950hpa; /* temp at 950 hPa */
    float *t800hpa; /* temp at 800 hPa */
    float *t700hpa; /* temp at 700 hPa */
    float *t500hpa; /* temp at 500 hPa */
    float *t0m; /* surface temp */
    float *t2m; /* 2m temp */
    float *ps; /* mean sea level pressure */
    float *topo; /* model topography */
    float *pw; /* precipitable water */
    float *rh; /* relative humidity at surface */
} nwpdata;

int nwpdata_init(nwpdata *nwp); 
int nwpdata_read(char *nwppath, 
    fmfilelist surflist, fmfilelist levlist, fmfilelist preslist,
    fmtime reqtime, fmucsref refucs, nwpdata *nwp);
int nwpdata_free(nwpdata *nwp);

#endif /* NWP_READ */

/*
 * NAME:
 * nwp_read.h
 *
 * PURPOSE:
 * To provide an interface between libusenwp and libfmcol.
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
 * Øystein Godøy, met.no/FOU, 18.10.2004 
 *
 * MODIFIED:
 * NA
 */

#include <stdio.h>
#include <stdlib.h>

#include <satimg.h>
#include <collocate1.h>

#ifndef NWP_READ
#define NWP_READ

#define FFNS 4
#define NOFIELDS1 9
#define NOFIELDS2 1
#define NOFIELDS NOFIELDS1+NOFIELDS2

typedef struct {
    fmsec1970 validtime;
    int leadtime;
    struct ucs refucs;
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
int nwpdata_read(char *filenames, fmtime reqtime, struct ucs refucs, 
	nwpdata *nwp);
int nwpdata_free(nwpdata *nwp);

#endif /* NWP_READ */

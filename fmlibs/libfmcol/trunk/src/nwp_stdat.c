/*
 * NAME:
 * nwp_stdat.c
 *
 * PURPOSE:
 * To return a square box of data surrounding a station position or the 
 * closest point defined by latitude and longitude. If the dimension of 
 * the requested data structure is 1x1 only a single point is returned. 
 *
 * NOTES:
 * It is required that libusenwp/libmifield is used to retrieve the NWP
 * data as this software interpolates to the same grid as the AVHRR data
 * is provided at. Thus the same setup can be used to retrieve NWP as
 * AVHRR data.
 *
 * Only quadratic regions with odd dimension are supported yet. 
 *
 * BUGS:
 * NA
 *
 * RETURN VALUES:
 * 0 - normal and correct ending
 * 1 - i/o problem
 * 2 - memory problem
 * 3 - other
 * 10 - no valid data
 *
 * DEPENDENCIES:
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 28/01/2002
 *
 * MODIFIED:
 * Øystein Godøy, MI/FOU, 16.04.2003
 * Øystein Godøy, met.no/FOU, 18.10.2004: NWP interface is changed.
 * Øystein Godøy, METNO/FOU, 17.10.2007: Removed some print statements.
 *
 * CVS_ID:
 * $Id$
 */

#include <nwp_stdat.h>
#include <time.h>

int nwp_stdat(fmgeopos gpos, fmprojspec myproj, fmucsref refucs,
	nwpdata nwp, ns_data *n) {

    int dx, dy, i, j, k, l, m;
    int fieldsize;
    float Bx, By;
    struct tm mytime;
    fmucspos tgxy;
    fmindex tgin;
    /*
    fmgeopos tgll;
    */

    /*
     * Set lead time for model forecast.
     */
    (*n).ltime = nwp.leadtime;

    /* 
     * Set valid time for forecast.
     */
    (*n).vtime = (int) nwp.validtime;

    /*
     * Estimate UCS for the requested geographical position as well as xy
     * coordinates within the image.
     */
    /* Remove when new code is tested ØG
    tgll.lat = (double) gpos.lat;
    tgll.lon = (double) gpos.lon;
    tgxy = ll2xy(tgll, myproj) ;
    tgin = xy2ind(rim, tgxy);
    */
    tgxy = fmgeo2ucs(gpos, myproj);
    tgin = fmucs2ind(refucs,tgxy);
    if (tgin.col < 0 || tgin.row < 0) return(3);
    (*n).nav.Bx = (float) tgxy.eastings;
    (*n).nav.By = (float) tgxy.northings;
    fieldsize = refucs.iw*refucs.ih;

    /*
     * Check boxsize
     */
    if ((*n).nav.iw%2 == 0 || (*n).nav.ih%2 == 0) {
	error("nwp_stdat","area required must be odd\n");
	return(2); 
    }

    dx = (int) floorf((float) (*n).nav.iw/2.);
    dy = (int) floorf((float) (*n).nav.ih/2.);

    /*
     * Collect data
     */
    k = 0;
    for (i=(tgin.row-dy); i<=(tgin.row+dy); i++) {
	for (j=(tgin.col-dx); j<=(tgin.col+dx); j++) {
	    l = fmivec(j,i,refucs.iw);
	    if (nwp.t950hpa[l] < UNDEF) {
		((*n).t950hpa)[k] = nwp.t950hpa[l];
	    } else {
		((*n).t950hpa)[k] = MISVAL;
	    }
	    if (nwp.t800hpa[l] < UNDEF) {
		((*n).t800hpa)[k] = nwp.t800hpa[l];
	    } else {
		((*n).t800hpa)[k] = MISVAL;
	    }
	    if (nwp.t700hpa[l] < UNDEF) {
		((*n).t700hpa)[k] = nwp.t700hpa[l];
	    } else {
		((*n).t700hpa)[k] = MISVAL;
	    }
	    if (nwp.t500hpa[l] < UNDEF) {
		((*n).t500hpa)[k] = nwp.t500hpa[l];
	    } else {
		((*n).t500hpa)[k] = MISVAL;
	    }
	    if (nwp.t0m[l] < UNDEF) {
		((*n).t0m)[k] = nwp.t0m[l];
	    } else {
		((*n).t0m)[k] = MISVAL;
	    }
	    if (nwp.t2m[l] < UNDEF) {
		((*n).t2m)[k] = nwp.t2m[l];
	    } else {
		((*n).t2m)[k] = MISVAL;
	    }
	    if (nwp.ps[l] < UNDEF) {
		((*n).ps)[k] = nwp.ps[l];
	    } else {
		((*n).ps)[k] = MISVAL;
	    }
	    if (nwp.pw[l] < UNDEF) {
		((*n).pw)[k] = nwp.pw[l];
	    } else {
		((*n).pw)[k] = MISVAL;
	    }
	    if (nwp.rh[l] < UNDEF) {
		((*n).rh)[k] = nwp.rh[l];
	    } else {
		((*n).rh)[k] = MISVAL;
	    }
	    if (nwp.topo[l] < UNDEF) {
		((*n).topo)[k] = nwp.topo[l];
	    } else {
		((*n).topo)[k] = MISVAL;
	    }

	    k++;
	}
    }

    return(0);
}

int nwp_init(int boxsize, float boxresx, float boxresy, int nopar, ns_data *n) {
    int i, j;

    (*n).nopar = nopar;
    /*
    (*n).nopix = boxsize*boxsize;
    (*n).nav.Ax = boxresx;
    (*n).nav.Ay = boxresy;
    (*n).nav.iw = boxsize;
    (*n).nav.ih = boxsize;
    */
    (*n).nopix = BOXSIZE2D;
    (*n).nav.Ax = boxresx;
    (*n).nav.Ay = boxresy;
    (*n).nav.iw = BOXSIZE;
    (*n).nav.ih = BOXSIZE;

    for (j=0; j<(*n).nopix; j++) {
	(*n).t950hpa[j] = MISVAL;
	(*n).t800hpa[j] = MISVAL;
	(*n).t700hpa[j] = MISVAL;
	(*n).t500hpa[j] = MISVAL;
	(*n).t0m[j] = MISVAL;
	(*n).t2m[j] = MISVAL;
	(*n).pw[j] = MISVAL;
	(*n).rh[j] = MISVAL;
	(*n).ps[j] = MISVAL;
	(*n).topo[j] = MISVAL;
    }
    return(0);
}

int nwp_free(ns_data *n) {
    int i;

    /*
    for (i=0; i<NOFIELDS; i++) {
	if ((*n).data[i] != NULL) {
	    free((*n).data[i]);
	    (*n).data[i] = NULL;
	} else {
	    break;
	}
    }
    
    if (i != (*n).nopar) {
	error("nwp_free","Inconsistent number of channels freed");
	(*n).nopar = 0;
	(*n).nopix = 0;
	return(3);
    }
    */

    (*n).nopar = 0;
    (*n).nopix = 0;

    return(0);
}

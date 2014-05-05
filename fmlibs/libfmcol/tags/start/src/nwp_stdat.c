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
 * Øystein Godøy, MI/FOU, 16.04.2003
 */

#include <nwp_stdat.h>
#include <time.h>

int nwp_stdat(struct latlon gpos, fmproj myproj, struct ucs refucs,
	int nwptime[], float *nwpfield, ns_data *n) {

    int dx, dy, i, j, k, l, m;
    int fieldsize;
    float Bx, By;
    struct tm mytime;
    fmposxy tgxy;
    fmindxy tgin;
    fmposll tgll;
    fmimage rim;

    /*
     * Set lead time for model forecast.
     */
    (*n).ltime = nwptime[4];

    /* 
     * Set valid time for forecast.
     */
    mytime.tm_sec = 0;
    mytime.tm_min = 0;
    mytime.tm_hour = nwptime[3];
    mytime.tm_mday = nwptime[2];
    mytime.tm_mon = nwptime[1]-1;
    mytime.tm_year = nwptime[0]-1900;
    (*n).vtime = (int) mktime(&mytime);

    /*
     * Estimate UCS for the requested geographical position as well as xy
     * coordinates within the image.
     */
    rim.Bx = (double) refucs.Bx;
    rim.By = (double) refucs.By;
    rim.Ax = (double) refucs.Ax;
    rim.Ay = (double) refucs.Ay;
    rim.iw = refucs.iw;
    rim.ih = refucs.ih;
    tgll.lat = (double) gpos.lat;
    tgll.lon = (double) gpos.lon;
    tgxy = ll2xy(tgll, myproj) ;
    tgin = xy2ind(rim, tgxy);
    if (tgin.x < 0 || tgin.y < 0) return(3);
    (*n).nav.Bx = (float) tgxy.x;
    (*n).nav.By = (float) tgxy.y;
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
    for (m=0; m<(*n).nopar; m++) {
	k = 0;
	for (i=(tgin.y-dy); i<=(tgin.y+dy); i++) {
	    for (j=(tgin.x-dx); j<=(tgin.x+dx); j++) {
		l = ivec(j,i,refucs.iw);
		if (nwpfield[(m*fieldsize)+l] < UNDEF) {
		    ((*n).data[m])[k] = nwpfield[(m*fieldsize)+l];
		} else {
		    ((*n).data[m])[k] = MISVAL;
		}
		k++;
	    }
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
    (*n).nopix = NWPSIZE2D;
    (*n).nav.Ax = boxresx;
    (*n).nav.Ay = boxresy;
    (*n).nav.iw = NWPSIZE;
    (*n).nav.ih = NWPSIZE;

    /*
    for (i=0; i<NOFIELDS; i++) {
	(*n).data[i] = NULL;
    }
    */
    for (i=0; i<nopar; i++) {
	/*
	(*n).data[i] = (float *) malloc((*n).nopix*sizeof(float));
	if (! (*n).data[i]) {
	    error("nwp_init","Could not allocate channel tile");
	    return(3);
	}
	*/
	for (j=0; j<(*n).nopix; j++) {
	    (*n).data[i][j] = MISVAL;
	}
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

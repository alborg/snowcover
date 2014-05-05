/*
 * NAME:
 * avhrr_stdat.c
 *
 * PURPOSE:
 * To return a square box surrounding a station position defined by latitude
 * and longitude. If the dimension of the requested data structure is 1x1 only
 * a single point is returned. 
 *
 * NOTES:
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
 * Øystein Godøy, DNMI/FOU, 07/11/2000
 * MODIFIED:
 * Øystein Godøy, DNMI/FOU, 28/01/2002
 * Changed variables, upos -> refucs, added use of imghead2??? functions
 * Øystein Godøy, MI/FOU, 11.04.2003
 * Added use of new libraries libfmcoord and libfmsubtrack
 */

#include <avhrr_stdat.h>
#include <time.h>

int avhrr_stdat(struct latlon gpos, fmproj myproj, struct imgh img, as_data *a) {

    int dx, dy, i, j, k, l, m, in, jn;
    int nodata = 1;
    struct dto timeid;
    struct clb c;
    struct sbtr st;
    struct tm mytime;
    fmangreq angreq;
    fmangles ang;
    fmposxy tgxy;
    fmposll tgll;
    fmindxy tgin;
    fmimage rim;

    /*
     * Convert from image header to useable data structures
     */
    rim.Bx = (double) img.Bx;
    rim.By = (double) img.By;
    rim.Ax = (double) img.Ax;
    rim.Ay = (double) img.Ay;
    rim.iw = img.iw;
    rim.ih = img.ih;
    sprintf((*a).source,"%s", img.sa);
    imghead2dto(img,&timeid);
    imghead2cal(img,&c);
    (*a).nav.Ax = img.Ax;
    (*a).nav.Ay = img.Ay;

    /*
     * Set UNIX time
     */
    mytime.tm_sec = 0;
    mytime.tm_min = img.mi;
    mytime.tm_hour = img.ho;
    mytime.tm_mday = img.dd;
    mytime.tm_mon = img.mm-1;
    mytime.tm_year = img.yy-1900;
    (*a).vtime = (int) mktime(&mytime);

    /*
     * Identify channel 3
     */
    if (img.ch[2] == 3) {
	(*a).ch3 = 'b';
    } else if (img.ch[2] == 6) {
	(*a).ch3 = 'a';
    } else {
	error("avhrr_stdat","Channel 3 id trouble...");
	exit(0);
    }

    /*
     * Get UCS position of the requested geographical position within the
     * image.
     */
    tgll.lat = (double) gpos.lat;
    tgll.lon = (double) gpos.lon;
    tgxy = ll2xy(tgll, myproj) ;
    tgin = xy2ind(rim, tgxy);
    if (tgin.x < 0 || tgin.y < 0) return(3);
    (*a).nav.Bx = (float) tgxy.x;
    (*a).nav.By = (float) tgxy.y;
	
    /*
     * Check bounding box size
     */
    if ((*a).nav.iw%2 == 0 || (*a).nav.ih%2 == 0) {
	error("avhrr_stdat","area required must be odd\n");
	return(2); 
    }

    /*
     * Satellite ground track is needed to retrieve viewing geometry.
     */
    angreq.ref = rim;
    angreq.ll = tgll;
    angreq.ind = tgin;
    angreq.date = timeid;
    angreq.subtrack.track = img.track;
    angreq.subtrack.numtrack = img.numtrack;
    angreq.myproj = MEOS;
    if (! fmangest(angreq, &ang)) {
	error("avhrr_stdat","angsat return");
	return(2);
    }
    (*a).ang.soz = (float) ang.soz;
    (*a).ang.saz = (float) ang.saz;
    (*a).ang.raz = (float) ang.raz;
    (*a).ang.aza = (float) ang.aza;
    (*a).ang.azu = (float) ang.azu;

    dx = (int) floorf((float) (*a).nav.iw/2.);
    dy = (int) floorf((float) (*a).nav.ih/2.);

    /*
     * Collect the actual data.
     */
    for (m=0; m<img.z; m++) {
	k = 0;
	for (i=(tgin.y-dy); i<=(tgin.y+dy); i++) {
	    for (j=(tgin.x-dx); j<=(tgin.x+dx); j++) {
		if (i < 0 || j < 0 || i >= img.ih || j >= img.iw) continue;
		l = ivec(j,i,rim.iw);
		if (img.image[m][l] != 0) {
		    (*a).data[m][k] = calib(img.image[m][l],img.ch[m],c);
		}
		k++;
	    }
	}
    }

    return(0);
}

int avhrr_init(int boxsize, int nochan, as_data *a) {
    int i, j;

    (*a).nochan = nochan;
    /*
    (*a).nopix = boxsize*boxsize;
    (*a).nav.iw = boxsize;
    (*a).nav.ih = boxsize;
    */
    (*a).nopix = BOXSIZE2D;
    (*a).nav.iw = BOXSIZE;
    (*a).nav.ih = BOXSIZE;

    for (i=0; i<nochan; i++) {
	for (j=0; j<(*a).nopix; j++) {
	    (*a).data[i][j] = MISVAL;
	}
    }
    return(0);
}

int avhrr_free(as_data *a) {
    int i;

    (*a).nochan = 0;
    (*a).nopix = 0;

    return(0);
}

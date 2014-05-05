/*
 * NAME:
 * avhrr_stdat.c
 *
 * PURPOSE:
 * To return a square box surrounding a station position defined by latitude
 * and longitude. If the dimension of the requested data structure is 1x1 only
 * a single point is returned. 
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * NA
 *
 * OUTPUT:
 * 0 - normal and correct ending
 * 1 - i/o problem
 * 2 - memory problem
 * 3 - other
 * 10 - no valid data
 *
 * NOTES:
 * Only quadratic regions with odd dimension are supported yet. 
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 07/11/2000
 *
 * MODIFIED:
 * Øystein Godøy, DNMI/FOU, 28/01/2002
 * Changed variables, upos -> refucs, added use of imghead2??? functions
 * Øystein Godøy, MI/FOU, 11.04.2003
 * Added use of new libraries libfmcoord and libfmsubtrack
 * Øystein Godøy, met.no/FOU, 21.12.2004
 * Added out of area test...
 * Øystein Godøy, met.no/FOU, 22.12.2004
 * Changed data structure that holds data to reflect actual channels...
 * Øystein Godøy, METNO/FOU, 26.04.2007: Adapted for libfmutil/libfmio.
 *
 * CVS_ID:
 * $Id$
 */

#include <avhrr_stdat.h>
#include <time.h>

int avhrr_stdat(fmgeopos gpos, fmprojspec myproj, fmio_img img, as_data *a) {

    char *where="avhrr_stdat";
    char what[FMSTRING512];
    int dx, dy, i, j, k, l, m, in, jn;
    int nodata = 0;
    fmtime timeid, mytime;
    fmsubtrack st;
    fmscale byte2float;
    fmangreq angreq;
    fmangles ang;
    fmucspos tgxy;
    fmgeopos tgll;
    fmindex tgin;
    fmucsref rim;
    fmio_img imgref;

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
    fm_img2fmtime(img,&timeid);
    fm_img2slopes(img,&byte2float);
    (*a).nav.Ax = img.Ax;
    (*a).nav.Ay = img.Ay;

    /*
     * Set UNIX time
     */
    (*a).vtime = (int) tofmsec1970(timeid);
    /* Remove when new code is tested... ØG
    mytime.tm_sec = 0;
    mytime.tm_min = img.mi;
    mytime.tm_hour = img.ho;
    mytime.tm_mday = img.dd;
    mytime.tm_mon = img.mm-1;
    mytime.tm_year = img.yy-1900;
    (*a).vtime = (int) mktime(&mytime);
    */

    /*
     * Identify channel 3
     */
    /* Not in use ØG
    if (img.ch[2] == 3) {
	(*a).ch3 = 'b';
    } else if (img.ch[2] == 6) {
	(*a).ch3 = 'a';
    } else {
	error("avhrr_stdat","Channel 3 id trouble...");
	exit(0);
    }
    */

    /*
     * Get UCS position of the requested geographical position within the
     * image.
     */
    tgll.lat = (double) gpos.lat;
    tgll.lon = (double) gpos.lon;
    tgxy = fmgeo2ucs(tgll, myproj) ;
    tgin = fmucs2ind(rim, tgxy);
    if (tgin.col < 0 || tgin.row < 0) {
	sprintf(what,"Indexing image failed on %.2fN %.2fE",tgll.lat,tgll.lon);
	fmerrmsg(where,what);
	return(FM_IO_ERR);
    }
    (*a).nav.Bx = (float) tgxy.eastings;
    (*a).nav.By = (float) tgxy.northings;
	
    /*
     * Check bounding box size
     */
    if ((*a).nav.iw%2 == 0 || (*a).nav.ih%2 == 0) {
	error("avhrr_stdat","area required must be odd\n");
	return(FM_SYNTAX_ERR); 
    }

    /*
     * Satellite ground track is needed to retrieve viewing geometry.
     */
    angreq.ref = rim;
    angreq.ll = tgll;
    angreq.ind = tgin;
    angreq.date = timeid;
    /* Remove new code is tested... ØG
    angreq.subtrack.track = img.track;
    */
    angreq.subtrack.npoints = img.numtrack;
    angreq.subtrack.gpos = malloc(angreq.subtrack.npoints*sizeof(fmgeopos));
    if (! angreq.subtrack.gpos) return(FM_MEMALL_ERR);
    for (i=0;i<angreq.subtrack.npoints;i++) {
	(angreq.subtrack.gpos[i]).lat = img.track[i].latitude;
	(angreq.subtrack.gpos[i]).lon = img.track[i].longitude;
    }
    angreq.myproj = MEOS;
    if (! fmangest(angreq, &ang)) {
	fmerrmsg(where,"angsat returned with error");
	return(FM_VAROUTOFSCOPE_ERR);
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
	for (i=(tgin.row-dy); i<=(tgin.row+dy); i++) {
	    for (j=(tgin.col-dx); j<=(tgin.col+dx); j++) {
		if (i < 0 || j < 0 || i >= img.ih || j >= img.iw) continue;
		l = fmivec(j,i,rim.iw);
		/*
		 * The out of area check is only performed using channel 4
		 * to avoid problems on satellites not using the split
		 * channel 3 configuration. This could be done more
		 * efficient, but that'll have to wait...
		 */
		if (img.image[m][l] == 0) {
		    if (img.image[3][l] == 0) nodata++;
		} else {
		    if (img.ch[m] == 1) {
			(*a).ch1[k] = fm_byte2float(img.image[m][l],byte2float,"Reflectance");
		    } else if (img.ch[m] == 2) {
			(*a).ch2[k] = fm_byte2float(img.image[m][l],byte2float,"Reflectance");
		    } else if (img.ch[m] == 6) {
			(*a).ch3a[k] = fm_byte2float(img.image[m][l],byte2float,"Reflectance");
		    } else if (img.ch[m] == 3) {
			(*a).ch3b[k] = fm_byte2float(img.image[m][l],byte2float,"Temperature");
		    } else if (img.ch[m] == 4) {
			(*a).ch4[k] = fm_byte2float(img.image[m][l],byte2float,"Temperature");
		    } else if (img.ch[m] == 5) {
			(*a).ch5[k] = fm_byte2float(img.image[m][l],byte2float,"Temperature");
		    } else {
			fmerrmsg(where,"Could not recognise channel id.");
			return(1);
		    }
		}
		k++;
	    }
	}
    }

    /*
     * If all pixels are unprocessed this is likely to be out of coverage,
     * return code to indicate this...
     */
    if (nodata == k) {
	fmlogmsg(where,"This request contains all unprocessed pixels.");
	printf(" k: %d nodata: %d\n", k, nodata);
	return(FM_OTHER_ERR);
    }

    return(FM_OK);
}

int avhrr_init(int boxsize, int nochan, as_data *a) {
    int i;

    (*a).nochan = nochan;

    (*a).nopix = BOXSIZE2D;
    (*a).nav.iw = BOXSIZE;
    (*a).nav.ih = BOXSIZE;

    for (i=0; i<(*a).nopix; i++) {
	(*a).ch1[i] = MISVAL;
	(*a).ch2[i] = MISVAL;
	(*a).ch3a[i] = MISVAL;
	(*a).ch3b[i] = MISVAL;
	(*a).ch4[i] = MISVAL;
	(*a).ch5[i] = MISVAL;
    }
    return(FM_OK);
}

int avhrr_free(as_data *a) {
    int i;

    (*a).nochan = 0;
    (*a).nopix = 0;

    return(FM_OK);
}

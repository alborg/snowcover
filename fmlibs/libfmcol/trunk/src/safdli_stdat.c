/*
 * NAME:
 * safdli_stdat.c
 *
 * PURPOSE:
 * To return a square box surrounding a station position defined by latitude
 * and longitude with SAFOSI DLI irradiance data. If the dimension of 
 * the requested data structure is 1x1 only a single point is returned. 
 *
 * This function operates on the native SAFOSI DLI data structure osihdf.
 *
 * NOTES:
 * Only quadratic regions with odd dimension are supported yet. 
 *
 * The cloud mask products are expected to be consistent with the AVHRR
 * tiles, else failure is expected...
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
 * Øystein Godøy, met.no/FOU, 23.12.2004 
 *
 * MODIFIED:
 * NA
 */

#include <safdli_stdat.h>
#include <time.h>

#ifdef FMCOL_HAVE_LIBOSIHDF5
int safdli_stdat(fmgeopos gpos, fmprojspec myproj,
	osihdf *safdli, safdli_data *dli) {

    char *where="safdli_stdat";
    int dx, dy, i, j, k, l, m;
    int nodata = 0;
    fmtime timeid;
    fmindex xyp, tgin;
    fmucspos tgxy;
    fmgeopos tgll;
    fmucsref rim;

    /*
     * Convert from image header to useable data structures, first
     * satellite id.
     */
    sprintf((*dli).source,"%s", safdli->h.source);

    /*
     * The UCS info is converted
     */
    rim.Ax = safdli->h.Ax;
    rim.Ay = safdli->h.Ay;
    rim.Bx = safdli->h.Bx;
    rim.By = safdli->h.By;
    rim.iw = safdli->h.iw;
    rim.ih = safdli->h.ih;

    (*dli).nav.Ax = rim.Ax;
    (*dli).nav.Ay = rim.Ay;

    /*
     * Set UNIX time for product
     */
    /* Remove when tested ØG
    timeid.mi = safdli->h.minute;
    timeid.ho = safdli->h.hour;
    timeid.dd = safdli->h.day;
    timeid.mm = safdli->h.month;
    timeid.yy = safdli->h.year;
    (*dli).vtime = dto2unixtime(timeid);;
    */
    timeid.fm_min = safdli->h.minute;
    timeid.fm_hour = safdli->h.hour;
    timeid.fm_mday = safdli->h.day;
    timeid.fm_mon = safdli->h.month;
    timeid.fm_year = safdli->h.year;
    (*dli).vtime = tofmsec1970(timeid);;

    /*
     * Get UCS position of the requested geographical position within the
     * image.
     */
    /* Remove when tested ØG
    tgll.lat = (double) gpos.lat;
    tgll.lon = (double) gpos.lon;
    tgxy = ll2xy(tgll, myproj);
    */
    tgxy = fmgeo2ucs(gpos, myproj);
    /* Remove when tested ØG
    tgin = xy2ind(rim, tgxy);
    */
    tgin = fmucs2ind(rim,tgxy);
    if (tgin.col < 0 || tgin.row < 0) return(3);
    (*dli).nav.Bx = (float) tgxy.eastings;
    (*dli).nav.By = (float) tgxy.northings;

    /*
     * Check bounding box size, these parameters are set by the init
     * function...
     */
    if ((*dli).nav.iw%2 == 0 || (*dli).nav.ih%2 == 0) {
	error("avhrr_stdat","area required must be odd\n");
	return(2); 
    }

    /*
     * Collect the actual data, l is used for pixel count within the
     * actual image, and k within the storage tile.
     */
    dx = (int) floorf((float) (*dli).nav.iw/2.);
    dy = (int) floorf((float) (*dli).nav.ih/2.);
    k = 0;
    for (i=(tgin.row-dy); i<=(tgin.row+dy); i++) {
	for (j=(tgin.col-dx); j<=(tgin.col+dx); j++) {
	    /*
	     * Check if within image coverage...
	     */
	    if (i < 0 || j < 0 || i >= rim.ih || j >= rim.iw) continue;
	    /*
	     * Navigation is performed in 2D while data is stored in 1D...
	     */
	    l = ivec(j,i,rim.iw);
	    /*
	     * Collect data...
	     */
	    (*dli).data[k] = ((float *) safdli->d[0].data)[l];
	    (*dli).qflg[k] = ((unsigned short *) safdli->d[1].data)[l];
	    
	    /*
	     * Check number of unprocessed pixels
	     */
	    if ((*dli).data[k] < -100.) nodata++;
	
	    k++;
	}
    }

    /*
     * If all pixels are unprocessed this is likely to be out of coverage,
     * return code to indicate this...
     */
    if (nodata == k) {
	logmsg(where,"This request contains all unprocessed pixels.");
	return(10);
    }

    return(0);
}

int safdli_init(int boxsize, safdli_data *dli) {

    char *errmsg="safdli_init";
    int i,j;

    (*dli).nopix = BOXSIZE2D;
    (*dli).nav.iw = BOXSIZE;
    (*dli).nav.ih = BOXSIZE;

    for (j=0; j<(*dli).nopix; j++) {
	(*dli).data[j] = SAFDLI_MISVAL;
    }
    
    return(0);
}

int safdli_free(safdli_data *dli) {

    char *errmsg="safdli_free";
    int i,j;

    (*dli).nopix = 0;
    (*dli).nav.iw = 0;
    (*dli).nav.ih = 0;

    return(0);
}

int safdli_name(fmtime refdate, char *basename, char *path, char *filename) {

    int nf;
    const char *dummy1=".";
    const char *dummy2="..";
    char *errmsg="safdli_name";
    char *mystr;
    struct dirent *direntp;
    DIR *dirp;

    mystr = (char *) malloc(NAMELEN);
    if (!mystr) {
	error(errmsg,"Could not allocate mystr");
	return(2);
    }

    /*
     * Må sjekke status her...
     */
    sprintf(mystr,"%4d%02d%02d%02d%02d",
	refdate.fm_year, refdate.fm_mon, refdate.fm_mday, 
	refdate.fm_hour, refdate.fm_min);

    /*
    printf(" #### %s %s %s\n",basename,path, mystr);
    */

    dirp = opendir(path);
    if (!dirp) {
	error(errmsg,"Opendir in did not work properly");
	return(2);
    } 

    nf = 0;
    while ((direntp = readdir(dirp)) != NULL) {
	if (strstr(direntp->d_name,".hdf") == NULL) {
	    continue;
	}
	if (strstr(direntp->d_name,mystr) == NULL) {
	    continue;
	}
	if (basename == NULL) {
	    if (strstr(direntp->d_name,dummy1) && 
		    strlen(direntp->d_name) == 1) {
		continue;
	    }
	    if (strstr(direntp->d_name,dummy2)) {
		continue;
	    }
	    sprintf(filename,"%s",direntp->d_name);
	    nf++;
	} else {
	    if (strstr(direntp->d_name,basename) == NULL) {
		continue;
	    }
	    sprintf(filename,"%s",direntp->d_name);
	    nf++;
	}
    }

    if (closedir(dirp) != 0) {
	error(errmsg,"Could not close directory");
	return(2);
    }

    free(mystr);

    if (nf == 0) {
	error(errmsg,
		"Did not find any files");
	return(3);
    }
    if (nf > 1) {
	error(errmsg,
		"Did find more than 1 file!! Why??");
	return(3);
    }

    return(0);
}

char *safdli_prod(char *filename) {
    static char *safdliprod[] = {"nr","ns","at","gr"};
    char *meosprod[] = {"nr","ns","at","gr"};
    char *prod=NULL, *errmsg="safdli_prod";
    int i, np=4;

    for (i=0; i<np; i++) {
	if (strstr(filename,meosprod[i])) {
	    prod = safdliprod[i];
	    return(prod);
	}
    }

    return(prod);
}

int safdli_check(fmio_img refdata, osihdf *safdli) {
    char *safdlisat[] = {"noaa12","noaa14","noaa15","noaa16","noaa17"};
    char *meossat[] = {"NOAA-12","NOAA-14","NOAA-15","NOAA-16","NOAA-17"};
    char *safdliprod[] = {"dnnr","dnns","dnat","dngr"};
    char *meosprod[] = {"nr","ns","at","gr"};
    char *errmsg="safdli_check";
    int i, nsat=5, narea=4;
    fmcol_boolean sat, area, time;
    struct tm *dlitm;
    time_t mytime;

    sat = area = time = FMCOL_FALSE;

    /*
     * Check satellite
     */
    for (i=0; i<nsat; i++) {
	if (strcmp(refdata.sa,meossat[i])==0 && 
		strcmp(safdli->h.source,safdlisat[i]) == 0) {
	    sat = FMTRUE;
	    break;
	}
    }

    /*
     * Check area
     */
    for (i=0; i<narea; i++) {
	if (strcmp(refdata.area,meosprod[i])==0 && 
		strcmp(safdli->h.area,safdliprod[i]) == 0) {
	    area = FMTRUE;
	    break;
	}
    }

    /*
     * Check time
     */
    /* This information is not set in the files yet...
    mytime = (time_t) safdli->sec_1970;
    printf(" %d %d\n", mytime, safdli->sec_1970);
    dlitm = gmtime(&mytime);
    printf("%d %d %d %d %d\n",
	    dlitm->tm_year,dlitm->tm_mon,dlitm->tm_mday,dlitm->tm_hour,
	    dlitm->tm_min);
    */
    if (!sat || !area) {
	error(errmsg,"Either sat or area error");
	fprintf(stderr," Satellite: %s <-> %s\n",
		refdata.sa,safdli->h.source);
	fprintf(stderr," Area: %s <-> %s\n",
		refdata.area,safdli->h.area);
	return(2);
    }

    return(0);
}

#endif /* FMCOL_HAVE_LIBOSIHDF5 */

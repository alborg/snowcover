/*
 * NAME:
 * safssi_stdat.c
 *
 * PURPOSE:
 * To return a square box surrounding a station position defined by latitude
 * and longitude with SAFOSI SSI irradiance data. If the dimension of 
 * the requested data structure is 1x1 only a single point is returned. 
 *
 * This function operates on the native SAFOSI SSI data structure osihdf.
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * NA
 *
 * OUTPUT:
 * RETURN VALUES:
 * 0 - normal and correct ending
 * 1 - i/o problem
 * 2 - memory problem
 * 3 - other
 * 10 - no valid data
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
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 27/09/2002
 *
 * MODIFIED:
 * Øystein Godøy, DNMI/FOU, 11.04.2003
 * Added use of new libraries libfmcoord and libfmsubtrack
 * Øystein Godøy, met.no/FOU, 21.12.2004
 * Added out of area test...
 *
 * CVS_ID:
 * $Id$
 */ 

#include <safssi_stdat.h>
#include <time.h>

#ifdef FMCOL_HAVE_LIBOSIHDF5

int safssi_stdat(fmgeopos gpos, fmprojspec myproj,
	osihdf *safssi, safssi_data *ssi) {

    char *where="safssi_stdat";
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
    sprintf((*ssi).source,"%s", safssi->h.source);

    /*
     * The UCS info is converted
     */
    rim.Ax = safssi->h.Ax;
    rim.Ay = safssi->h.Ay;
    rim.Bx = safssi->h.Bx;
    rim.By = safssi->h.By;
    rim.iw = safssi->h.iw;
    rim.ih = safssi->h.ih;

    /*
    printf(" lat: %.2f lon: %.2f\n",gpos.lat,gpos.lon);
    printf(" Bx: %.2f By: %.2f Ax: %.2f Ay: %.2f\n",
	    rim.Bx,rim.By,rim.Ax,rim.Ay);
    */

    (*ssi).nav.Ax = rim.Ax;
    (*ssi).nav.Ay = rim.Ay;

    /*
     * Set UNIX time for product
     */
    timeid.fm_min = safssi->h.minute;
    timeid.fm_hour = safssi->h.hour;
    timeid.fm_mday = safssi->h.day;
    timeid.fm_mon = safssi->h.month;
    timeid.fm_year = safssi->h.year;
    (*ssi).vtime = tofmsec1970(timeid);;

    /*
     * Get UCS position of the requested geographical position within the
     * image.
     */
    tgxy = fmgeo2ucs(gpos, myproj);
    tgin = fmucs2ind(rim,tgxy);
    if (tgin.col < 0 || tgin.row < 0) {
	fmerrmsg(where, "This is out of image...");
	fprintf(stdout," lat: %.2f lon: %.2f\n",tgll.lat,tgll.lon);
	fprintf(stdout," northings: %d eastings: %d\n",tgxy.northings,tgxy.eastings);
	fprintf(stdout," myproj: %d\n", myproj);
	return(FM_SYNTAX_ERR);
    }
    (*ssi).nav.Bx = (float) tgxy.eastings;
    (*ssi).nav.By = (float) tgxy.northings;

    /*
     * Check bounding box size, these parameters are set by the init
     * function...
     */
    if ((*ssi).nav.iw%2 == 0 || (*ssi).nav.ih%2 == 0) {
	fmerrmsg("avhrr_stdat","area required must be odd\n");
	return(FM_SYNTAX_ERR); 
    }

    /*
     * Collect the actual data, l is used for pixel count within the
     * actual image, and k within the storage tile.
     */
    dx = (int) floorf((float) (*ssi).nav.iw/2.);
    dy = (int) floorf((float) (*ssi).nav.ih/2.);
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
	    l = fmivec(j,i,rim.iw);
	    /*
	     * Collect data...
	     */
	    (*ssi).data[k] = ((float *) safssi->d[0].data)[l];
	    (*ssi).qflg[k] = ((unsigned short *) safssi->d[1].data)[l];
	    
	    /*
	     * Check number of unprocessed pixels
	     */
	    if ((*ssi).data[k] < -100.) nodata++;
	
	    k++;
	}
    }

    /*
     * If all pixels are unprocessed this is likely to be out of coverage,
     * return code to indicate this...
     */
    if (nodata == k) {
	fmlogmsg(where,"This request contains all unprocessed pixels.");
	return(FM_OTHER_ERR);
    }

    return(FM_OK);
}

int safssi_init(int boxsize, safssi_data *ssi) {

    char *where="safssi_init";
    int i,j;

    (*ssi).nopix = BOXSIZE2D;
    (*ssi).nav.iw = BOXSIZE;
    (*ssi).nav.ih = BOXSIZE;

    for (j=0; j<(*ssi).nopix; j++) {
	(*ssi).data[j] = SAFSSI_MISVAL;
    }
    
    return(0);
}

int safssi_free(safssi_data *ssi) {

    char *where="safssi_free";
    int i,j;

    (*ssi).nopix = 0;
    (*ssi).nav.iw = 0;
    (*ssi).nav.ih = 0;

    return(0);
}

int safssi_name(fmtime refdate, char *basename, char *path, char *filename) {

    int nf;
    const char *dummy1=".";
    const char *dummy2="..";
    char *where="safssi_name";
    char *mystr;
    struct dirent *direntp;
    DIR *dirp;

    mystr = (char *) malloc(NAMELEN);
    if (!mystr) {
	fmerrmsg(where,"Could not allocate mystr");
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
	fmerrmsg(where,"Opendir in did not work properly");
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
	fmerrmsg(where,"Could not close directory");
	return(2);
    }

    free(mystr);

    if (nf == 0) {
	fmerrmsg(where,
		"Did not find any files");
	return(3);
    }
    if (nf > 1) {
	fmerrmsg(where,
		"Did find more than 1 file!! Why??");
	return(3);
    }

    return(0);
}

char *safssi_prod(char *filename) {
    static char *safssiprod[] = {"nr","ns","at","gr"};
    char *meosprod[] = {"nr","ns","at","gr"};
    char *prod=NULL, *where="safssi_prod";
    int i, np=4;

    for (i=0; i<np; i++) {
	if (strstr(filename,meosprod[i])) {
	    prod = safssiprod[i];
	    return(prod);
	}
    }

    return(prod);
}

int safssi_check(fmio_img refdata, osihdf *safssi) {
    char *safssisat[] = {"noaa12","noaa14","noaa15","noaa16","noaa17"};
    char *meossat[] = {"NOAA-12","NOAA-14","NOAA-15","NOAA-16","NOAA-17"};
    char *safssiprod[] = {"dnnr","dnns","dnat","dngr"};
    char *meosprod[] = {"nr","ns","at","gr"};
    char *where="safssi_check";
    int i, nsat=5, narea=4;
    fmcol_boolean sat, area, time;
    struct tm *ssitm;
    time_t mytime;

    sat = area = time = FMCOL_FALSE;

    /*
     * Check satellite
     */
    for (i=0; i<nsat; i++) {
	if (strcmp(refdata.sa,meossat[i])==0 && 
		strcmp(safssi->h.source,safssisat[i]) == 0) {
	    sat = FMTRUE;
	    break;
	}
    }

    /*
     * Check area
     */
    for (i=0; i<narea; i++) {
	if (strcmp(refdata.area,meosprod[i])==0 && 
		strcmp(safssi->h.area,safssiprod[i]) == 0) {
	    area = FMTRUE;
	    break;
	}
    }

    /*
     * Check time
     */
    /* This information is not set in the files yet...
    mytime = (time_t) safssi->sec_1970;
    printf(" %d %d\n", mytime, safssi->sec_1970);
    ssitm = gmtime(&mytime);
    printf("%d %d %d %d %d\n",
	    ssitm->tm_year,ssitm->tm_mon,ssitm->tm_mday,ssitm->tm_hour,
	    ssitm->tm_min);
    */
    if (!sat || !area) {
	fmerrmsg(where,"Either sat or area error");
	fprintf(stderr," Satellite: %s <-> %s\n",
		refdata.sa,safssi->h.source);
	fprintf(stderr," Area: %s <-> %s\n",
		refdata.area,safssi->h.area);
	return(2);
    }

    return(0);
}

#endif /* FMCOL_HAVE_LIBOSIHDF5 */

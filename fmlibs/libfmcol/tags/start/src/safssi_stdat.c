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
 * Øystein Godøy, DNMI/FOU, 27/09/2002
 * Øystein Godøy, DNMI/FOU, 11.04.2003
 * Added use of new libraries libfmcoord and libfmsubtrack
 */

#include <safssi_stdat.h>
#include <time.h>

int safssi_stdat(struct latlon gpos, fmproj myproj,
	osihdf *safssi, safssi_data *ssi) {

    int dx, dy, i, j, k, l, m;
    int nodata = 1;
    struct dto timeid;
    struct xy xyp;
    fmposxy tgxy;
    fmindxy tgin;
    fmposll tgll;
    fmimage rim;

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

    (*ssi).nav.Ax = rim.Ax;
    (*ssi).nav.Ay = rim.Ay;

    /*
     * Set UNIX time for product
     */
    timeid.mi = safssi->h.minute;
    timeid.ho = safssi->h.hour;
    timeid.dd = safssi->h.day;
    timeid.mm = safssi->h.month;
    timeid.yy = safssi->h.year;
    (*ssi).vtime = dto2unixtime(timeid);;

    /*
     * Get UCS position of the requested geographical position within the
     * image.
     */
    tgll.lat = (double) gpos.lat;
    tgll.lon = (double) gpos.lon;
    tgxy = ll2xy(tgll, myproj) ;
    tgin = xy2ind(rim, tgxy);
    if (tgin.x < 0 || tgin.y < 0) return(3);
    (*ssi).nav.Bx = (float) tgxy.x;
    (*ssi).nav.By = (float) tgxy.y;

    /*
     * Check bounding box size, these parameters are set by the init
     * function...
     */
    if ((*ssi).nav.iw%2 == 0 || (*ssi).nav.ih%2 == 0) {
	error("avhrr_stdat","area required must be odd\n");
	return(2); 
    }

    /*
     * Collect the actual data, l is used for pixel count within the
     * actual image, and k within the storage tile.
     */
    dx = (int) floorf((float) (*ssi).nav.iw/2.);
    dy = (int) floorf((float) (*ssi).nav.ih/2.);
    k = 0;
    for (i=(tgin.y-dy); i<=(tgin.y+dy); i++) {
	for (j=(tgin.x-dx); j<=(tgin.x+dx); j++) {
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
	    (*ssi).data[k] = ((float *) safssi->d[0].data)[l];
	    (*ssi).qflg[k] = ((unsigned short *) safssi->d[1].data)[l];
		
	    k++;
	}
    }

    return(0);
}

int safssi_init(int boxsize, safssi_data *ssi) {

    char *errmsg="safssi_init";
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

    char *errmsg="safssi_free";
    int i,j;

    (*ssi).nopix = 0;
    (*ssi).nav.iw = 0;
    (*ssi).nav.ih = 0;

    return(0);
}

int safssi_name(struct dto refdate, char *basename, char *path, char *filename) {

    int nf;
    const char *dummy1=".";
    const char *dummy2="..";
    char *errmsg="safssi_name";
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
	refdate.yy, refdate.mm, refdate.dd, refdate.ho, refdate.mi);

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

char *safssi_prod(char *filename) {
    static char *safssiprod[] = {"nr","ns","at","gr"};
    char *meosprod[] = {"nr","ns","at","gr"};
    char *prod=NULL, *errmsg="safssi_prod";
    int i, np=4;

    for (i=0; i<np; i++) {
	if (strstr(filename,meosprod[i])) {
	    prod = safssiprod[i];
	    return(prod);
	}
    }

    return(prod);
}

int safssi_check(struct imgh refdata, osihdf *safssi) {
    char *safssisat[] = {"noaa12","noaa14","noaa15","noaa16","noaa17"};
    char *meossat[] = {"NOAA-12","NOAA-14","NOAA-15","NOAA-16","NOAA-17"};
    char *safssiprod[] = {"dnnr","dnns","dnat","dngr"};
    char *meosprod[] = {"nr","ns","at","gr"};
    char *errmsg="safssi_check";
    int i, nsat=5, narea=4;
    boolean sat, area, time;
    struct tm *ssitm;
    time_t mytime;

    sat = area = time = FALSE;

    /*
     * Check satellite
     */
    for (i=0; i<nsat; i++) {
	if (strcmp(refdata.sa,meossat[i])==0 && 
		strcmp(safssi->h.source,safssisat[i]) == 0) {
	    sat = TRUE;
	    break;
	}
    }

    /*
     * Check area
     */
    for (i=0; i<narea; i++) {
	if (strcmp(refdata.area,meosprod[i])==0 && 
		strcmp(safssi->h.area,safssiprod[i]) == 0) {
	    area = TRUE;
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
	error(errmsg,"Either sat or area error");
	fprintf(stderr," Satellite: %s <-> %s\n",
		refdata.sa,safssi->h.source);
	fprintf(stderr," Area: %s <-> %s\n",
		refdata.area,safssi->h.area);
	return(2);
    }

    return(0);
}

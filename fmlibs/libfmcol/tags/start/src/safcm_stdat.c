/*
 * NAME:
 * safcm_stdat.c
 *
 * PURPOSE:
 * To return a square box surrounding a station position defined by latitude
 * and longitude with SAFNWC PPS cloudmask/type data. If the dimension of 
 * the requested data structure is 1x1 only a single point is returned. 
 *
 * This function operates on the native SAFNWC PPS data structure CTy_t.
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
 * Øystein Godøy, DNMI/FOU, 03/09/2002
 * Øystein Godøy, DNMI/FOU, 20.12.2002
 * Changed safcm_name to support the new names of the PPS package.
 * Øystein Godøy, DNMI/FOU, 11.04.2003
 * Added use of new library libfmcoord.
 */

#include <safcm_stdat.h>
#include <time.h>

int safcm_stdat(struct latlon gpos, fmproj myproj, 
	CTy_t *ctype, safcm_data *cm) {

    int dx, dy, i, j, k, l, m, in, jn;
    int nodata = 1;
    struct clb c;
    fmposxy tgxy;
    fmindxy tgin;
    fmposll tgll;
    fmimage rim;

    /*
     * Convert from image header to useable data structures, first
     * satellite id.
     */
    sprintf((*cm).source,"%s", ctype->satellite_id);

    /*
     * The UCS info is converted, remember that SAFNWC use meter unit
     * while DNMI use kilometer...
     */
    rim.Ax = 
	M2KM(fabs(ctype->reg->area_extent[2]-ctype->reg->area_extent[0])/
	    (double) ctype->reg->xsize);
    rim.Ay = 
	M2KM(fabs(ctype->reg->area_extent[3]-ctype->reg->area_extent[1])/
	    (double) ctype->reg->ysize);
    rim.Bx = M2KM(ctype->reg->area_extent[0]);
    rim.By = M2KM(ctype->reg->area_extent[3]);
    rim.iw = ctype->reg->xsize;
    rim.ih = ctype->reg->ysize;

    (*cm).nav.Ax = (float) rim.Ax;
    (*cm).nav.Ay = (float) rim.Ay;

    /*
     * Set UNIX time for product
     */
    (*cm).vtime = ctype->sec_1970;

    /*
     * Get UCS position of the requested geographical position within the
     * image.
     */
    tgll.lat = (double) gpos.lat;
    tgll.lon = (double) gpos.lon;
    tgxy = ll2xy(tgll, myproj) ;
    tgin = xy2ind(rim, tgxy);
    if (tgin.x < 0 || tgin.y < 0) return(3);
    (*cm).nav.Bx = (float) tgxy.x;
    (*cm).nav.By = (float) tgxy.y;

    /*
     * Check bounding box size, these parameters are set by the init
     * function...
     */
    if ((*cm).nav.iw%2 == 0 || (*cm).nav.ih%2 == 0) {
	error("avhrr_stdat","area required must be odd\n");
	return(2); 
    }

    /*
     * Transfer decoding information for cloudmask/type to storage tile
     * structure...
     */
    for (i=0; i<SM_NUMBER_OF_CLOUDTYPE_VALUES; i++) {
	/*
	printf(" Lengde: %d\n", strlen(ctype->cloudtype_lut[i]));
	*/
	if (ctype->cloudtype_lut[i]) {
	    /*
	    strncpy((*cm).description[i],
		    ctype->cloudtype_lut[i],MAX_LENGTH_STRING);
	    */
	    strncpy((*cm).description[i],
		    ctype->cloudtype_lut[i],50);
	} else {
	    sprintf((*cm).description[i],"NA");
	}
    }

    /*
     * Collect the actual data, l is used for pixel count within the
     * actual image, and k within the storage tile.
     */
    dx = (int) floorf((float) (*cm).nav.iw/2.);
    dy = (int) floorf((float) (*cm).nav.ih/2.);
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
	    (*cm).data[k] = (unsigned char) ctype->cloudtype[l];
		
	    k++;
	}
    }

    return(0);
}

int safcm_init(int boxsize, safcm_data *cm) {

    char *errmsg="safcm_init";
    int i,j;

    (*cm).nopix = BOXSIZE2D;
    (*cm).nav.iw = BOXSIZE;
    (*cm).nav.ih = BOXSIZE;

    for (j=0; j<(*cm).nopix; j++) {
	(*cm).data[j] = SAFCM_MISVAL;
    }
    for (i=0;i<SM_NUMBER_OF_CLOUDTYPE_VALUES;i++) {
	for (j=0;j<MAX_LENGTH_STRING;j++) {
	    (*cm).description[i][j] = '\0';
	}
    }
    
    return(0);
}

int safcm_free(safcm_data *cm) {

    char *errmsg="safcm_free";
    int i,j;

    (*cm).nopix = 0;
    (*cm).nav.iw = 0;
    (*cm).nav.ih = 0;

    for (i=0;i<SM_NUMBER_OF_CLOUDTYPE_VALUES;i++) {
	for (j=0;j<MAX_LENGTH_STRING;j++) {
	    (*cm).description[i][j] = '\0';
	}
    }

    return(0);
}

int safcm_name(struct dto refdate, char *basename, char *path, char *filename) {

    int nf;
    const char *dummy1=".";
    const char *dummy2="..";
    char *errmsg="safcm_name";
    char *mystr;
    struct dirent *direntp;
    DIR *dirp;

    mystr = (char *) malloc(NAMELEN);
    if (!mystr) {
	error(errmsg,"Could not allocate mystr");
	return(2);
    }

    sprintf(mystr,"%4d%02d%02d_%02d%02d",
	refdate.yy, refdate.mm, refdate.dd, refdate.ho, refdate.mi);

    dirp = opendir(path);
    if (!dirp) {
	error(errmsg,"Opendir in did not work properly");
	return(2);
    } 

    nf = 0;
    while ((direntp = readdir(dirp)) != NULL) {
	if (strstr(direntp->d_name,"cloudtype.hdf") == NULL) {
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

char *safcm_prod(char *filename) {
    static char *safcmprod[] = {"dnnr","dnns","dnat","dngr"};
    char *meosprod[] = {"nr","ns","at","gr"};
    char *prod=NULL, *errmsg="safcm_prod";
    int i, np=4;

    for (i=0; i<np; i++) {
	if (strstr(filename,meosprod[i])) {
	    prod = safcmprod[i];
	    return(prod);
	}
    }

    return(prod);
}

int safcm_check(struct imgh refdata, CTy_t *ctype) {
    char *safcmsat[] = {"noaa12","noaa14","noaa15","noaa16","noaa17"};
    char *meossat[] = {"NOAA-12","NOAA-14","NOAA-15","NOAA-16","NOAA-17"};
    char *safcmprod[] = {"dnnr","dnns","dnat","dngr"};
    char *meosprod[] = {"nr","ns","at","gr"};
    char *errmsg="safcm_check";
    int i, nsat=5, narea=4;
    boolean sat, area, time;
    struct tm *cmtm;
    time_t mytime;

    sat = area = time = FALSE;

    /*
    printf(" %s %s %d %d\n", 
	    ctype->satellite_id, ctype->reg->id,
	    ctype->sec_1970, ctype->orbit_number);
    */

    /*
     * Check satellite
     */
    for (i=0; i<nsat; i++) {
	if (strcmp(refdata.sa,meossat[i])==0 && 
		strcmp(ctype->satellite_id,safcmsat[i]) == 0) {
	    sat = TRUE;
	    break;
	}
    }

    /*
     * Check area
     */
    for (i=0; i<narea; i++) {
	if (strcmp(refdata.area,meosprod[i])==0 && 
		strcmp(ctype->reg->id,safcmprod[i]) == 0) {
	    area = TRUE;
	    break;
	}
    }

    /*
     * Check time
     */
    /* This information is not set in the files yet...
    mytime = (time_t) ctype->sec_1970;
    printf(" %d %d\n", mytime, ctype->sec_1970);
    cmtm = gmtime(&mytime);
    printf("%d %d %d %d %d\n",
	    cmtm->tm_year,cmtm->tm_mon,cmtm->tm_mday,cmtm->tm_hour,
	    cmtm->tm_min);
    */
    if (!sat || !area) {
	error(errmsg,"Either sat or area error");
	fprintf(stderr," Satellite: %s <-> %s\n",
		refdata.sa,ctype->satellite_id);
	fprintf(stderr," Area: %s <-> %s\n",
		refdata.area,ctype->reg->id);
	return(2);
    }

    return(0);
}

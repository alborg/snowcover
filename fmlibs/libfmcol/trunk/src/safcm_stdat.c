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
 * REQUIREMENTS:
 *
 * INPUT:
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
 * The cloud mask products are expected to be consistent with the AVHRR
 * tiles, else failure is expected...
 *
 * BUGS:
 * Some instances of invlaid records have been found in the databases. The
 * problem seems to be related to the classnames and time specification,
 * but it is yet not understood whether this is caused by this software of
 * the HDF interface in hdfaccess.c.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 03/09/2002
 *
 * MODIFIED:
 * Øystein Godøy, DNMI/FOU, 20.12.2002
 * Changed safcm_name to support the new names of the PPS package.
 * Øystein Godøy, DNMI/FOU, 11.04.2003
 * Added use of new library libfmcoord.
 * Øystein Godøy, met.no/FOU, 21.12.2004
 * Added chack of valid data.
 * Øystein Godøy, METNO/FOU, 2013-04-18: Switched to new PPS libraries.
 * Øystein Godøy, METNO/FOU, 2013-06-27: Added new satellites NOAA-19 and
 * MetOp-02 to safcm_check.
 *
 * CVS_ID:
 * $Id$
 */

#include <safcm_stdat.h>
#include <time.h>

#ifdef FMCOL_HAVE_LIBSMHI_SAF

int safcm_stdat(fmgeopos gpos, fmprojspec myproj, 
	CTy_t *ctype, safcm_data *cm) {

    char *where = "safcm_stdat";
    char what[FMSTRING256];
    int dx, dy, i, j, k, l, m, in, jn;
    int nodata = 1;
    int notprocessed = 0;
    /*
    struct clb c;
    */
    fmucspos tgxy;
    fmindex tgin;
    fmgeopos tgll;
    fmucsref rim;

    if (FMCOL_NO_CLOUDTYPE_VALUES != SM_NUMBER_OF_CLOUDTYPE_VALUES) {
	sprintf(what,"The number of cloud types seem to have changed %d->%d",FMCOL_NO_CLOUDTYPE_VALUES,SM_NUMBER_OF_CLOUDTYPE_VALUES);
	fmerrmsg(where, what);
	return(FM_IO_ERR);
    }

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
    tgxy = fmgeo2ucs(tgll, myproj) ;
    tgin = fmucs2ind(rim, tgxy);
    if (tgin.col < 0 || tgin.row < 0) return(FM_SYNTAX_ERR);
    (*cm).nav.Bx = (float) tgxy.eastings;
    (*cm).nav.By = (float) tgxy.northings;

    /*
     * Check bounding box size, these parameters are set by the init
     * function...
     */
    if ((*cm).nav.iw%2 == 0 || (*cm).nav.ih%2 == 0) {
	fmerrmsg("avhrr_stdat","area required must be odd\n");
	return(FM_SYNTAX_ERR); 
    }

    /*
     * Transfer decoding information for cloudmask/type to storage tile
     * structure...
     */
    for (i=0; i<FMCOL_NO_CLOUDTYPE_VALUES; i++) {
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
	    (*cm).data[k] = (unsigned char) ctype->cloudtype[l];
	    /*
	     * Check number of unprocessed pixels
	     */
	    if (ctype->cloudtype[l] == 0) notprocessed++;
		
	    k++;
	}
    }
    
    /*
     * If all pixels are unprocessed this is likely to be out of coverage,
     * return code to indicate this...
     */
    if (notprocessed == k) {
	fmlogmsg(where,"This request contains all unprocessed pixels.");
	return(FM_OTHER_ERR);
    }

    return(FM_OK);
}

int safcm_init(int boxsize, safcm_data *cm) {

    char *where="safcm_init";
    char what[FMSTRING256];
    int i,j;

    if (FMCOL_NO_CLOUDTYPE_VALUES != SM_NUMBER_OF_CLOUDTYPE_VALUES) {
	sprintf(what,"The number of cloud types seem to have changed %d->%d",FMCOL_NO_CLOUDTYPE_VALUES,SM_NUMBER_OF_CLOUDTYPE_VALUES);
	fmerrmsg(where, what);
	return(FM_IO_ERR);
    }

    (*cm).nopix = BOXSIZE2D;
    (*cm).nav.iw = BOXSIZE;
    (*cm).nav.ih = BOXSIZE;

    for (j=0; j<(*cm).nopix; j++) {
	(*cm).data[j] = SAFCM_MISVAL;
    }
    for (i=0;i<FMCOL_NO_CLOUDTYPE_VALUES;i++) {
	for (j=0;j<FMSTRING256;j++) {
	    (*cm).description[i][j] = '\0';
	}
    }
    
    return(FM_OK);
}

int safcm_free(safcm_data *cm) {

    char *where="safcm_free";
    char what[FMSTRING256];
    int i,j;

    if (FMCOL_NO_CLOUDTYPE_VALUES != SM_NUMBER_OF_CLOUDTYPE_VALUES) {
	sprintf(what,"The number of cloud types seem to have changed %d->%d",FMCOL_NO_CLOUDTYPE_VALUES,SM_NUMBER_OF_CLOUDTYPE_VALUES);
	fmerrmsg(where, what);
	return(FM_IO_ERR);
    }

    (*cm).nopix = 0;
    (*cm).nav.iw = 0;
    (*cm).nav.ih = 0;

    for (i=0;i<FMCOL_NO_CLOUDTYPE_VALUES;i++) {
	for (j=0;j<MAX_LENGTH_STRING;j++) {
	    (*cm).description[i][j] = '\0';
	}
    }

    return(FM_OK);
}

int safcm_name(fmtime refdate, char *basename, char *path, char *filename) {

    int nf;
    const char *dummy1=".";
    const char *dummy2="..";
    char *where="safcm_name";
    char *mystr;
    struct dirent *direntp;
    DIR *dirp;

    mystr = (char *) malloc(NAMELEN);
    if (!mystr) {
	fmerrmsg(where,"Could not allocate mystr");
	return(2);
    }

    sprintf(mystr,"%4d%02d%02d_%02d%02d",
	refdate.fm_year, refdate.fm_mon, refdate.fm_mday, 
	refdate.fm_hour, refdate.fm_min);

    dirp = opendir(path);
    if (!dirp) {
	fmerrmsg(where,"Opendir in did not work properly");
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

char *safcm_prod(char *filename) {
    static char *safcmprod[] = {"dnnr","dnns","dnat","dngr"};
    char *meosprod[] = {"nr","ns","at","gr"};
    char *prod=NULL, *where="safcm_prod";
    int i, np=4;

    for (i=0; i<np; i++) {
	if (strstr(filename,meosprod[i])) {
	    prod = safcmprod[i];
	    return(prod);
	}
    }

    return(prod);
}

int safcm_check(fmio_img refdata, CTy_t *ctype) {
    char *safcmsat[] = {"noaa12","noaa14","noaa15","noaa16","noaa17","noaa18","noaa19","metop02"};
    char *meossat[] = {"NOAA-12","NOAA-14","NOAA-15","NOAA-16","NOAA-17","NOAA-18","NOAA-19","MetOp-02"};
    char *safcmprod[] = {"dnnr","dnns","dnat","dngr"};
    char *meosprod[] = {"nr","ns","at","gr"};
    char *where="safcm_check";
    int i, nsat=8, narea=4;
    fmboolean sat, area, time;
    struct tm *cmtm;
    time_t mytime;

    sat = area = time = FMFALSE;

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
	    sat = FMTRUE;
	    break;
	}
    }

    /*
     * Check area
     */
    for (i=0; i<narea; i++) {
	if (strcmp(refdata.area,meosprod[i])==0 && 
		strcmp(ctype->reg->id,safcmprod[i]) == 0) {
	    area = FMTRUE;
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
	fmerrmsg(where,"Either sat or area error");
	fprintf(stderr," Satellite: [%s] <-> [%s]\n",
		refdata.sa,ctype->satellite_id);
	fprintf(stderr," Area: [%s] <-> [%s]\n",
		refdata.area,ctype->reg->id);
	fprintf(stderr," sat: %d, area: %d\n",sat,area);
	return(2);
    }

    return(FM_OK);
}

#endif /* HAVE_LIBSMHI_SAF */

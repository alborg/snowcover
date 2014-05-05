/*
 * NAME:
 * nwp_read.c
 *
 * PURPOSE:
 * To provide an interface between libusenwp and libfmcol.
 *
 * REQUIREMENTS:
 * o libsatimg
 * o libusenwp
 *
 * INPUT:
 * o filenames to search (not used at present)
 * o requested time
 * o requested output grid (as specified by remote sensing software)
 *
 * OUTPUT:
 * data structure
 * return values:
 * 0-OK
 * 1-command syntax error
 * 2-I/O problem
 * 3-Memory trouble
 *
 * NOTES:
 * Filenames are hardcoded at present, input filenames are not used due to
 * constraints in the C/Fortran interface (all filenames have to be equal
 * lengths) and the initialisation has to be predefined. Dynamic
 * allocation should be implemented but it needs to be contiguous in
 * memory...
 *
 * Some filenames are hardcoded, but that is to make sure that the
 * interface is updated whenever the operational NWP models are modified.
 *
 * Run nwpdata_init before nwpdata_read!!
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 18.10.2004 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 16.10.2007: Modified file specification of
 * which files to read.
 *
 * CVS_ID:
 * $Id$
 */

#include <nwp_read.h>

int nwpdata_read(char *nwppath, 
    fmfilelist surflist, fmfilelist levlist, fmfilelist preslist,
    fmtime reqtime, fmucsref refucs, nwpdata *nwp) {

    char *where="nwpdata_read";
    char what[FMSTRING256];
    char filename[FMSTRING1024];
    int i, imgsize;
    int nfiles, iunit=10, interp=1, itime[5];
    int nparam1=NOFIELDS1, nparam2=NOFIELDS2, nparam3=NOFIELDS3;
    int ierror, iundef, len1, len2, len3;
    float satgrid[10], *nwpfield1, *nwpfield2, *nwpfield3;
    char **feltfilesurf, **feltfilepresl, **feltfilemodl;
    int iparam1[NOFIELDS1][4]={ /* Spec of what to get surface level*/
	{ 30, 2, 1000, 0},  /* Temp. at 0m */
	{ 31, 2, 1000, 0},  /* Temp. at 2m */
	{  8, 2, 1000, 0},  /* Surface pressure hPa */
	{101, 2, 1000, 9},  /* Model topography */
	{ 32, 2, 1000, 0}  /* Relative humidity */
    }; 
    int iparam2[NOFIELDS2][4]={ /* Spec of what to get from model levels*/
	{  0, 0,  0, 7}   /* Precipitable water */
    }; 
    int iparam3[NOFIELDS3][4]={ /* Spec of what to get from pressure levels*/
	{ 18, 1,  950, 6},  /* Temp. at 950 hPa */
	{ 18, 1,  800, 6},  /* Temp. at 800 hPa */
	{ 18, 1,  700, 6},  /* Temp. at 700 hPa */
	{ 18, 1,  500, 6},  /* Temp. at 500 hPa */
    };
    int icontrol[6]={ /* Accep. spec */
	    88,  /* Producer 88 -> MI */
	-32767,  /* Model grid -32767-> first found */
	    +6,  /* Min allowed forecast length in hours */
	   +18,  /* Max allowed forecast length in hours */
	    -2,  /* Min allowed offset in hours from image time */
	    +2   /* Max allowed offset in hours from image time */
    };
    fmtime nwptime;
    /*
    fmfilelist linklist1, linklist2, linklist3;
    */

    /*
     * Generate filename symlinks and arrays to pass to getfield
     */
    /* Removed when starting to use /starc...
    if (fmfilelist_alloc(&linklist1,surflist.nfiles)) {
	sprintf(what,"Could not allocate linklist1");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }
    sprintf(linklist1.path,"%s",nwppath);
    for (i=0;i<linklist1.nfiles;i++) {
	sprintf(filename,"%s/%s",surflist.path, surflist.filename[i]);
	fmfeltfile_gettime(filename, &nwptime);
	sprintf(linklist1.filename[i],"HIR20surf_%04d%02d%02d_%02d%02d.dat",
		nwptime.fm_year,nwptime.fm_mon,nwptime.fm_mday,
		nwptime.fm_hour,nwptime.fm_min
	       );
    }
    if (fmlinkfiles(surflist,linklist1)) {
	sprintf(what,"Could not create symbolic links for surface files");
	fmerrmsg(where,what);
	return(FM_IO_ERR);
    }

    if (fmfilelist_alloc(&linklist2,surflist.nfiles)) {
	sprintf(what,"Could not allocate linklist2");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }
    sprintf(linklist2.path,"%s",nwppath);
    for (i=0;i<linklist2.nfiles;i++) {
	sprintf(filename,"%s/%s",levlist.path, levlist.filename[i]);
	fmfeltfile_gettime(filename, &nwptime);
	sprintf(linklist2.filename[i],"HIR20modl_%04d%02d%02d_%02d%02d.dat",
		nwptime.fm_year,nwptime.fm_mon,nwptime.fm_mday,
		nwptime.fm_hour,nwptime.fm_min
	       );
    }
    if (fmlinkfiles(levlist,linklist2)) {
	sprintf(what,"Could not create symbolic links for surface files");
	fmerrmsg(where,what);
    }

    if (fmfilelist_alloc(&linklist3,preslist.nfiles)) {
	sprintf(what,"Could not allocate linklist3");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }
    sprintf(linklist3.path,"%s",nwppath);
    for (i=0;i<linklist3.nfiles;i++) {
	sprintf(filename,"%s/%s",preslist.path, preslist.filename[i]);
	fmfeltfile_gettime(filename, &nwptime);
	sprintf(linklist3.filename[i],"HIR20presl_%04d%02d%02d_%02d%02d.dat",
		nwptime.fm_year,nwptime.fm_mon,nwptime.fm_mday,
		nwptime.fm_hour,nwptime.fm_min
	       );
    }
    if (fmlinkfiles(preslist,linklist3)) {
	sprintf(what,"Could not create symbolic links for surface files");
	fmerrmsg(where,what);
	return(FM_IO_ERR);
    }
    */

    /*
     * Generate character array to be transmitted to getfield, keep it
     * contigous in memory
     */
    printf("The following files will be searched for NWP data:\n");
    len1 = strlen(surflist.path)+strlen(surflist.filename[0])+1+1;
    feltfilesurf = malloc(surflist.nfiles*sizeof(char *));
    if (! feltfilesurf) {
	sprintf(what,"Could not allocate feltfilesurf rows");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }
    feltfilesurf[0] = malloc(surflist.nfiles*len1*sizeof(char));
    if (! feltfilesurf[0]) {
	sprintf(what,"Could not allocate feltfilesurf string");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }
    for (i=0;i<surflist.nfiles;i++) {
	feltfilesurf[i] = feltfilesurf[0]+i*len1;
    }
    for (i=0;i<surflist.nfiles;i++) {
	sprintf(feltfilesurf[i],"%s/%s",surflist.path,surflist.filename[i]);
	printf("%s\n",feltfilesurf[i]);
    }

    len2 = strlen(levlist.path)+strlen(levlist.filename[0])+1+1;
    feltfilemodl = malloc(levlist.nfiles*sizeof(char *));
    if (! feltfilemodl) {
	sprintf(what,"Could not allocate feltfilemodl rows");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }
    feltfilemodl[0] = malloc(levlist.nfiles*len2*sizeof(char));
    if (! feltfilemodl[0]) {
	sprintf(what,"Could not allocate feltfilemodl string");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }
    for (i=0;i<levlist.nfiles;i++) {
	feltfilemodl[i] = feltfilemodl[0]+i*len2;
    }
    for (i=0;i<levlist.nfiles;i++) {
	sprintf(feltfilemodl[i],"%s/%s",levlist.path,levlist.filename[i]);
	printf("%s\n",feltfilemodl[i]);
    }

    len3 = strlen(preslist.path)+strlen(preslist.filename[0])+1+1;
    feltfilepresl = malloc(preslist.nfiles*sizeof(char *));
    if (! feltfilepresl) {
	sprintf(what,"Could not allocate feltfilepresl rows");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }
    feltfilepresl[0] = malloc(preslist.nfiles*len2*sizeof(char));
    if (! feltfilepresl[0]) {
	sprintf(what,"Could not allocate feltfilepresl string");
	fmerrmsg(where,what);
	return(FM_MEMALL_ERR);
    }
    for (i=0;i<preslist.nfiles;i++) {
	feltfilepresl[i] = feltfilepresl[0]+i*len2;
    }
    for (i=0;i<preslist.nfiles;i++) {
	sprintf(feltfilepresl[i],"%s/%s",preslist.path,preslist.filename[i]);
	printf("%s\n",feltfilepresl[i]);
    }

    /* Requested grid description, only one supported yet */
    satgrid[0] = 60.;
    satgrid[1] = 0.;
    satgrid[2] = 1000.;
    satgrid[3] = 1000.;
    satgrid[4] = 0.;
    satgrid[5] = 0.;
    satgrid[6] = refucs.Ax;
    satgrid[7] = refucs.Ay;
    satgrid[8] = refucs.Bx;
    satgrid[9] = refucs.By;
    /* Requested valid time */
    itime[0] = reqtime.fm_year;
    itime[1] = reqtime.fm_mon;
    itime[2] = reqtime.fm_mday;
    itime[3] = reqtime.fm_hour;
    itime[4] = reqtime.fm_min;

    /*
     * The fields are returned in the vector "field[FMIO_MAXIMGSIZE]" which is
     * organized like this: "field[nx*ny*nparam]", where nx and ny are
     * the numbers of gridpoints in x and y direction and nparam is the
     * number of fields/parameters collected. 
     */
    fmlogmsg(where,
    "Collecting operational HIRLAM-data and interpolating to satellite grid...");
    len1 =  strlen(feltfilesurf[0])+1;
    len2 =  strlen(feltfilemodl[0])+1;
    len3 =  strlen(feltfilepresl[0])+1;
    printf(" len1: %d len2: %d len3: %d\n", len1, len2, len3);
    imgsize = refucs.iw*refucs.ih;
    if (imgsize > FMIO_MAXIMGSIZE) {
	fmerrmsg(where,
	    "NWP fields are too large (%d) for the allocated space (%d)",
	    imgsize, FMIO_MAXIMGSIZE);
	return(FM_IO_ERR);
    }
    nwpfield1 = (float *) malloc(NOFIELDS1*imgsize*sizeof(float));
    if (!nwpfield1) {
	fmerrmsg(where,"Could not allocate nwpfield1..");
	return(FM_MEMALL_ERR);
    }
    nwpfield2 = (float *) malloc(NOFIELDS2*imgsize*sizeof(float));
    if (!nwpfield2) {
	fmerrmsg(where,"Could not allocate nwpfield2..");
	return(FM_MEMALL_ERR);
    }
    nwpfield3 = (float *) malloc(NOFIELDS3*imgsize*sizeof(float));
    if (!nwpfield3) {
	fmerrmsg(where,"Could not allocate nwpfield3..");
	return(FM_MEMALL_ERR);
    }

    /*
     * Collect data from the files containing surface and parameter fields
     */
    fmlogmsg(where,"Then surface fields are searched...");
    getfield_(&(surflist.nfiles), feltfilesurf[0], &iunit, &interp, 
	    satgrid, &refucs.iw, &refucs.ih, itime, 
	    &nparam1, iparam1, icontrol, nwpfield1, &iundef, &ierror, len1);
    if (ierror) {
	fmerrmsg(where,"Error reading surface and parameter fields");
	return(FM_IO_ERR);
    }

    /*
     * Collect data from the files containing model level fields
     */
    fmlogmsg(where,"Then model fields are searched...");
    getfield_(&(levlist.nfiles), feltfilemodl[0], &iunit, &interp, 
	    satgrid, &refucs.iw, &refucs.ih, itime, 
	    &nparam2, iparam2, icontrol, nwpfield2, &iundef, &ierror, len2);
    if (ierror) {
	fmerrmsg(where,"Error reading model level fields");
	return(FM_IO_ERR);
    }

    /*
     * Collect data from the files containing pressure level fields
     */
    fmlogmsg(where,"Then pressure fields are searched...");
    getfield_(&(preslist.nfiles), feltfilepresl[0], &iunit, &interp, 
	    satgrid, &refucs.iw, &refucs.ih, itime, 
	    &nparam3, iparam3, icontrol, nwpfield3, &iundef, &ierror, len3);
    if (ierror) {
	fmerrmsg(where,"Error reading pressure level fields");
	return(FM_IO_ERR);
    }

    /*
     * Print status information about the model field which was found.
     * if "iundef" is different from 0, undefined values were found
     * in the HIRLAM fields. The code for undefined is +1.e+35.
     */
    printf(" Date: %02d/%02d/%d", 
	    itime[2], itime[1], itime[0]);
    printf(" and time: %02d:00 UTC\n", itime[3]);
    printf(" Forecast length: %d\n", itime[4]);
    printf(" Status of 'getfield':");
    printf(" ierror=%d iundef=%d\n\n", ierror, iundef);

    /*
     * Remove symbolic links
     */
    /*
    if(fmremovefiles(surflist) != FM_OK) {
	fmerrmsg(where,"Could not remove symbolic links within surflist");
    }
    if(fmremovefiles(levlist) != FM_OK) {
	fmerrmsg(where,"Could not remove symbolic links within levlist");
    }
    if(fmremovefiles(preslist) != FM_OK) {
	fmerrmsg(where,"Could not remove symbolic links within preslist");
    }
    fmfilelist_free(&surflist);
    fmfilelist_free(&levlist);
    fmfilelist_free(&preslist);
    */

    /*
     * Transfer the time information to the nwpdata structure
     */
    nwp->leadtime = itime[4];
    nwptime.fm_year = itime[0];
    nwptime.fm_mon = itime[1];
    nwptime.fm_mday = itime[2];
    nwptime.fm_hour = itime[3];
    nwptime.fm_min = 0;
    nwptime.fm_sec = 0;
    nwp->validtime = tofmsec1970(nwptime);

    /*
     * Transfer the UCS information
     */
    nwp->refucs = refucs;

    /*
     * Allocate the data structure that will contain the NWP data
     */
    if (!nwp->t950hpa) {
	nwp->t950hpa = (float *) malloc(imgsize*sizeof(float));
	if (!nwp->t950hpa) return(FM_MEMALL_ERR);
    }
    if (!nwp->t800hpa) {
	nwp->t800hpa = (float *) malloc(imgsize*sizeof(float));
	if (!nwp->t800hpa) return(FM_MEMALL_ERR);
    }
    if (!nwp->t700hpa) {
	nwp->t700hpa = (float *) malloc(imgsize*sizeof(float));
	if (!nwp->t700hpa) return(FM_MEMALL_ERR);
    }
    if (!nwp->t500hpa) {
	nwp->t500hpa = (float *) malloc(imgsize*sizeof(float));
	if (!nwp->t500hpa) return(FM_MEMALL_ERR);
    }
    if (!nwp->t2m) {
	nwp->t2m = (float *) malloc(imgsize*sizeof(float));
	if (!nwp->t2m) return(FM_MEMALL_ERR);
    }
    if (!nwp->t0m) {
	nwp->t0m = (float *) malloc(imgsize*sizeof(float));
	if (!nwp->t0m) return(FM_MEMALL_ERR);
    }
    if (!nwp->ps) {
	nwp->ps = (float *) malloc(imgsize*sizeof(float));
	if (!nwp->ps) return(FM_MEMALL_ERR);
    }
    if (!nwp->pw) {
	nwp->pw = (float *) malloc(imgsize*sizeof(float));
	if (!nwp->pw) return(FM_MEMALL_ERR);
    }
    if (!nwp->rh) {
	nwp->rh = (float *) malloc(imgsize*sizeof(float));
	if (!nwp->rh) return(FM_MEMALL_ERR);
    }
    if (!nwp->topo) {
	nwp->topo = (float *) malloc(imgsize*sizeof(float));
	if (!nwp->topo) return(FM_MEMALL_ERR);
    }

    /*
     * Transfer the NWP data from the local temporary array to the nwpdata
     * structure
     */
    for (i=0;i<imgsize;i++) {
	/*
	 * Surface, parameter and pressure fields
	 */
	nwp->t0m[i] = nwpfield1[0*imgsize+i];
	nwp->t2m[i] = nwpfield1[1*imgsize+i];
	nwp->ps[i] = nwpfield1[2*imgsize+i];
	nwp->topo[i] = nwpfield1[3*imgsize+i];
	nwp->rh[i] = nwpfield1[4*imgsize+i];
	
	/*
	 * Model level fields
	 */
	nwp->pw[i] = nwpfield2[0*imgsize+i];

	/*
	 * Pressure level fields
	 */
	nwp->t950hpa[i] = nwpfield3[0*imgsize+i];
	nwp->t800hpa[i] = nwpfield3[1*imgsize+i];
	nwp->t700hpa[i] = nwpfield3[2*imgsize+i];
	nwp->t500hpa[i] = nwpfield3[3*imgsize+i];
    }

    /*
     * Free allocated memory
     */
    free(nwpfield1);
    free(nwpfield2);
    free(nwpfield3);

    return(FM_OK);
}

/*
 * NAME:
 * nwpdata_init
 *
 * PURPOSE:
 * To initialize the data structure and ease memory management.
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
 * Øystein Godøy, met.no/FOU, 21.10.2004 
 *
 * MODIFIED:
 * NA
 */

int nwpdata_init(nwpdata *nwp) {

    nwp->t950hpa = NULL;
    nwp->t800hpa = NULL;
    nwp->t700hpa = NULL;
    nwp->t500hpa = NULL;

    nwp->t0m = NULL;
    nwp->t2m = NULL;

    nwp->ps = NULL;
    nwp->pw = NULL;
    nwp->rh = NULL;

    nwp->topo = NULL;

    return(FM_OK);
}

/*
 * NAME:
 * nwpdata_free
 *
 * PURPOSE:
 * To free resources allocated within nwpdata_read.
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
 * Øystein Godøy, met.no/FOU, 21.10.2004 
 *
 * MODIFIED:
 * NA
 */

int nwpdata_free(nwpdata *nwp) {

    if (nwp->t950hpa) free(nwp->t950hpa);
    if (nwp->t800hpa) free(nwp->t800hpa);
    if (nwp->t700hpa) free(nwp->t700hpa);
    if (nwp->t500hpa) free(nwp->t500hpa);

    if (nwp->t0m) free(nwp->t0m);
    if (nwp->t2m) free(nwp->t2m);

    if (nwp->ps) free(nwp->ps);
    if (nwp->pw) free(nwp->pw);
    if (nwp->rh) free(nwp->rh);

    if (nwp->topo) free(nwp->topo);

    return(FM_OK);
}

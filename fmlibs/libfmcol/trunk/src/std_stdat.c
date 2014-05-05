/*
 * NAME:
 * std_stdat.c
 *
 * PURPOSE:
 * To check available ASCII files containing data from ground stations of
 * either synop or drau type. The ASCII files are generated in a
 * preprocessing step using ROBS. Using a config file specification of
 * either synop, drau or all files can be performed. Setting a boolean
 * variable in the same file will restrict datainput to synoptic times.
 * syno files are given for each hour, drau files every 3 hour, thus in
 * order to find the closest files +/- 0.5 hours are used for syno files
 * and +/- 1.5 hours for drau files (Ref.: Rebecca Rudsar).
 *
 * INCLUDED FUNCTIONS:
 *   read_stdat - general function to check and read station data
 *   get_stdat_h - retrieves the header of the ASCII files, used by
 * read_stdat
 *   fill_stdat_h - after the correct file is found by get_stdat_h this
 * functions reads data into the structure, used by read_stdat.
 *   init_stdat - initializes the data structure, used before read_stdat
 * or within.
 *   free_stdat - frees memory allocated, used after read_stdat or within
 * read_stdat.
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * NA
 *
 * OUTPUT:
 * RETURN VALUES:
 * 0 - correct and normal
 * 2 - I/O error
 * 3 - memory error
 *
 * NOTES:
 * Does not handle drau files properly yet, should discuss timing in such
 * files with Steinar to deceide how to handle them...
 * Implements only handling of synop files at present...
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/12/2001
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 22.08.2007: Changed reading of geographical
 * positions to double instead of float.
 * Øystein Godøy, METNO/FOU, 26.09.2008: Modifications to match changes in
 * fm_obs2ascii.
 *
 * CVS_ID:
 * $Id$
 */ 

#include <fmcol.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <std_stdat.h>

int read_stdat(char *ipath, char *basename, fmcol_boolean synop,
	time_t timeid, stdatstruct *pobs) {

    char *where="read_stdat";
    char what[FMSTRING512];
    time_t stime, deltat;
    short hour;
    fmcol_boolean failure=FMTRUE;
    const char *dummy1="UPDATED";
    char *infile;
    struct dirent *direntp;
    struct tm time_str;
    DIR *dirp;
    int i;

    /*
     * Check that data structure is initialized and ready to use.
     */
    if ((*pobs).obs != NULL) {
	fmerrmsg(where,"stdatstruct was not initialized");
	return(FM_MEMALL_ERR);
    }
    
    /*
     * First scan directory to get all files therein, then discard files
     * not useable.
     */
    dirp = opendir(ipath);
    if (!dirp) {
	fmerrmsg(where,"Opendir on %s did not work properly", ipath);
	return(FM_IO_ERR);
    } 

    infile = (char *) malloc(FILELEN*sizeof(char));
    if (!infile) {
	fmerrmsg(where,"Could not allocate memory to hold infile name");
	return(FM_MEMALL_ERR);
    }

    while ((direntp = readdir(dirp)) != NULL) {
	if (basename == NULL) {
	    if (direntp->d_name[0] == '.') {
		continue;
	    } else if (strstr(direntp->d_name,dummy1)) {
		continue;
	    }
	} else {
	    if (direntp->d_name[0] == '.') {
		continue;
	    } else if (strstr(direntp->d_name,basename) == NULL) {
		continue;
	    }
	}
	/*
	 * Get header of file
	 */
	sprintf(infile,"%s/%s", ipath, direntp->d_name);
	if (get_stdat_h(infile, pobs) != 0) {
	    continue;
	}
	/*
	 * Check whether this is the wanted file or not
	 */
	/*
	 * Changed as synop time is added to header...
	hour = (short) ceilf(
		((float)(*pobs).timeid1.ho+(float)(*pobs).timeid2.ho)/2.);
	*/
	/* remove when tested properly ØG
	hour = (short) ceilf((float)(*pobs).timeid0.fm_hour);
	time_str.tm_sec = 0;
	time_str.tm_min = 0;
	time_str.tm_hour = hour;
	time_str.tm_mday = (*pobs).timeid0.fm_mday;
	time_str.tm_mon = (*pobs).timeid0.fm_mon-1;
	time_str.tm_year = (*pobs).timeid0.fm_year-1900;
	stime = mktime(&time_str);
	*/
	stime = tofmsec1970((*pobs).timeid0);
	if (strstr(basename,"syno") != NULL) {
	    deltat = (time_t) TSDIFF;
	} else if (strstr(basename,"drau") != NULL) {
	    deltat = (time_t) TDDIFF;
	} 
	if (synop) {
	    deltat = (time_t) SDIFF;
	}

	if (timeid > (stime-deltat) && timeid < (stime+deltat)) {
	    failure = FMFALSE;
	    break;
	}
    }
    if (failure) {
	fmerrmsg(where,"The time difference to satellite data is too large");
	free(infile);
	return(FM_IO_ERR);
    }

    /*
     * Allocate memory for station data
     */
    fmlogmsg(where,"Reading observations from %s", infile);
    (*pobs).obs = (obsstruct *) malloc((*pobs).nostat*sizeof(obsstruct));
    if (!(*pobs).obs) {
	fmerrmsg(where,"Could not allocate obsstruct");
	free(infile);
	return(FM_IO_ERR);
    }

    /*
     * Read the requested file.
     */
    if (fill_stdat_h(infile, timeid, pobs) != 0) {
	fmerrmsg(where,"Could not access data in file %s", infile);
	if (free_stdat(pobs) != 0) {
	    fmerrmsg(where,"terminal memory leakage...");
	    return(FM_MEMFREE_ERR);
	}
	free(infile);
	return(FM_IO_ERR);
    }
    /*
     * Test printout...
     */
    /*
    for (i=0;i<(*pobs).nostat; i++) {
	printf(" %d stID: %s %02d/%02d/%d %02d:%02d %f %.2fN %.2fE\n",
		i, 
		((*pobs).obs)[i].stID,
		((*pobs).obs)[i].year,
		((*pobs).obs)[i].month,
		((*pobs).obs)[i].day,
		((*pobs).obs)[i].hour,
		((*pobs).obs)[i].min,
		((*pobs).obs)[i].TdTdTd,
		((*pobs).obs)[i].pos.lat,
		((*pobs).obs)[i].pos.lon
	);
    }
    exit(0);
    */

    return(FM_OK);
}

int get_stdat_h(char *infile, stdatstruct *obsl) {

    char *where="get_stdat_h";
    char *dummy;
    char *tagvar = "* stID stType lat lon year month day hour min TTT TdTdTd TwTwTw PPPP ff dd RRR E sss N Nh Cl Cm Ch VV ww W1 W2";
    FILE *fp;

    dummy = (char *) malloc(STDATSTRLEN);
    if (!dummy) {
	fmerrmsg(where,"Memory allocation dummy");
	return(FM_MEMALL_ERR);
    }

    fp = fopen(infile,"r");
    if (!fp) {
	fmerrmsg(where,"Could not open %s, continues to next file", infile);
	free(dummy);
	return(10);
    }

    /*
     * Get station type in this file
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	fmerrmsg(where,"Could not read header type in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }
    if (sscanf(dummy,"%*c%s",(*obsl).type) != 1) {
	fmerrmsg(where,"Could not read type in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }

    /*
     * Get first time in file
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	fmerrmsg(where,"Could not read header time1 in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }
    if (sscanf(dummy,"%*c%d%d%d%d%d%d",
		&((*obsl).timeid0).fm_year,
		&((*obsl).timeid0).fm_mon,
		&((*obsl).timeid0).fm_mday,
		&((*obsl).timeid0).fm_hour,
		&((*obsl).timeid0).fm_min,
		&(*obsl).unixtime0) != 6) {
	fmerrmsg(where,"Could not read start time1 in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }

    /*
     * Get second time in file
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	fmerrmsg(where,"Could not read header time2 in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }
    if (sscanf(dummy,"%*c%d%d%d%d%d%d",
		&((*obsl).timeid1).fm_year,
		&((*obsl).timeid1).fm_mon,
		&((*obsl).timeid1).fm_mday,
		&((*obsl).timeid1).fm_hour,
		&((*obsl).timeid1).fm_min,
		&(*obsl).unixtime1) != 6) {
	fmerrmsg(where,"Could not read start time2 in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }

    /*
     * Get third time in file
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	fmerrmsg(where,"Could not read header time2 in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }
    if (sscanf(dummy,"%*c%d%d%d%d%d%d",
		&((*obsl).timeid2).fm_year,
		&((*obsl).timeid2).fm_mon,
		&((*obsl).timeid2).fm_mday,
		&((*obsl).timeid2).fm_hour,
		&((*obsl).timeid2).fm_min,
		&(*obsl).unixtime2) != 6) {
	fmerrmsg(where,"Could not read start time2 in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }

    /*
     * Get number of observations
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	fmerrmsg(where,"Could not read header noobs in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }
    if (sscanf(dummy,"%*c%*s%d",&(*obsl).noobs) != 1) {
	fmerrmsg(where,"Could not read noobs in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }

    /*
     * Get number of stations
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	fmerrmsg(where,"Could not read header nostat in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }
    if (sscanf(dummy,"%*c%*s%d",&(*obsl).nostat) != 1) {
	fmerrmsg(where,"Could not read nostat in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }
    /*
     * Get variables list and check for consistency
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	fmerrmsg(where,"Could not read header variables list in %s", infile);
	free(dummy);
	return(FM_IO_ERR);
    }
    fmremovenewline(dummy);
    if (strcmp(dummy,tagvar) != 0) {
	fmerrmsg(where,"Wrong header variables list in %s\n\t%s\n\t%s", 
	    infile,dummy,tagvar);
	free(dummy);
	return(FM_IO_ERR);
    }

    fclose(fp);

    free(dummy);
    return(FM_OK);
}


int fill_stdat_h(char *infile, time_t vtime, stdatstruct *obsl) {

    char *where="fill_stdat_h";
    int nf, i;
    int year;
    short month, day, hour, minute;
    char *dummy, *stid, *stidtab;
    char *f1 = "%s%s%lf%lf%d%hd%hd%hd%hd%f%f%f%f%hd%d%f";
    char *f2 = "%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd";
    char *format;
    time_t stime, deltat;
    struct tm time_str;
    FILE *fp;

    dummy = (char *) malloc(STDATSTRLEN);
    if (!dummy) {
	fmerrmsg(where,"Memory allocation dummy");
	return(FM_MEMALL_ERR);
    }

    format = (char *) malloc(STDATSTRLEN);
    if (!format) {
	fmerrmsg(where,"Memory allocation format");
	return(FM_MEMALL_ERR);
    }
    sprintf(format,"%s%s",f1,f2);

    stid = (char *) malloc(10);
    if (!stid) {
	fmerrmsg(where,"Memory allocation stid");
	free(dummy);
	free(format);
	return(FM_MEMALL_ERR);
    }

    stidtab = (char *) malloc((10+2)*((*obsl).noobs));
    /*printf(" %d\n",(*obsl).noobs);*/
    if (!stidtab) {
	fmerrmsg(where,"Memory allocation stidtab");
	free(dummy);
	free(format);
	free(stid);
	return(FM_MEMALL_ERR);
    }

    fp = fopen(infile,"r");
    if (!fp) {
	fmerrmsg(where,"Could not open %s", infile);
	free(dummy);
	free(format);
	free(stid);
	free(stidtab);
	return(FM_IO_ERR);
    }

    nf = i = 0;
    /*
     * Skip header as this is read before and read data records
     * Only one observation is stored for each station. It is assumed that
     * observations for the same station is given in sequence. The last is
     * chosen if no time difference.
     */
    while (fgets(dummy,(int) STDATSTRLEN,fp) != NULL) {
	nf++;
	if (nf<=8 && dummy[0]=='*') continue; /* Skipping header */
	sscanf(dummy,"%s",stid);
	if (i>0 && (strstr(stidtab,stid) != NULL)) {
	    sscanf(dummy,"%*s%*s%*f%*f%d%hd%hd%hd%hd",
		    &year,&month,&day,&hour,&minute);
	    /* 
	     * Check if same station as previous record (assumed)
	     */
	    if (strcmp(((*obsl).obs)[i-1].stID,stid) != 0) continue;
	    printf(" Duplicate at line: %d\n", i-1);
	    /*
	     * Get unix time for the current duplicate data record
	     */
	    time_str.tm_sec = 0;
	    time_str.tm_min = minute;
	    time_str.tm_hour = hour;
	    time_str.tm_mday = day;
	    time_str.tm_mon = month-1;
	    time_str.tm_year = year-1900;
	    stime = mktime(&time_str);
	    deltat = abs((int) vtime-stime);
	    /*
	     * Get unix time for data already stored
	     */
	    time_str.tm_sec = 0;
	    time_str.tm_min = ((*obsl).obs)[i-1].min;
	    time_str.tm_hour = ((*obsl).obs)[i-1].hour;
	    time_str.tm_mday = ((*obsl).obs)[i-1].day;
	    time_str.tm_mon = ((*obsl).obs)[i-1].month-1;
	    time_str.tm_year = ((*obsl).obs)[i-1].year-1900;
	    stime = mktime(&time_str);
	    /*
	     * Skip record if record already stored is closer in time to
	     * requested time.
	     */
	    if (deltat > abs((int) vtime-stime)) {
		continue;
	    }
	    i--;
	}
	sprintf(stidtab,"%s ",stid);
	sscanf(dummy,format,
		((*obsl).obs)[i].stID,
		((*obsl).obs)[i].stType,
		&((*obsl).obs)[i].pos.lat,
		&((*obsl).obs)[i].pos.lon,
		&((*obsl).obs)[i].year,
		&((*obsl).obs)[i].month,
		&((*obsl).obs)[i].day,
		&((*obsl).obs)[i].hour,
		&((*obsl).obs)[i].min,
		&((*obsl).obs)[i].TTT,
		&((*obsl).obs)[i].TdTdTd,
		&((*obsl).obs)[i].TwTwTw,
		&((*obsl).obs)[i].PPPP,
		&((*obsl).obs)[i].ff,
		&((*obsl).obs)[i].dd,
		&((*obsl).obs)[i].RRR,
		&((*obsl).obs)[i].E,
		&((*obsl).obs)[i].sss,
		&((*obsl).obs)[i].N,
		&((*obsl).obs)[i].Nh,
		&((*obsl).obs)[i].Cl,
		&((*obsl).obs)[i].Cm,
		&((*obsl).obs)[i].Ch,
		&((*obsl).obs)[i].VV,
		&((*obsl).obs)[i].ww,
		&((*obsl).obs)[i].W1,
		&((*obsl).obs)[i].W2
	);
	i++;
    }
    /*printf(" i: %d - %d\n", i, (*obsl).noobs);*/

    fclose(fp);

    free(dummy);
    free(format);
    free(stidtab);
    free(stid);
    return(FM_OK);
}

int init_stdat(stdatstruct *obsl) {

    (*obsl).nostat = 0;
    (*obsl).noobs = 0;
    (*obsl).obs = NULL;
    (*obsl).timeid0.fm_year = 0;
    (*obsl).timeid0.fm_mon = 0;
    (*obsl).timeid0.fm_mday = 0;
    (*obsl).timeid0.fm_hour = 0;
    (*obsl).timeid0.fm_min = 0;
    (*obsl).timeid0.fm_sec = 0;
    (*obsl).timeid1.fm_year = 0;
    (*obsl).timeid1.fm_mon = 0;
    (*obsl).timeid1.fm_mday = 0;
    (*obsl).timeid1.fm_hour = 0;
    (*obsl).timeid1.fm_min = 0;
    (*obsl).timeid1.fm_sec = 0;
    (*obsl).timeid2.fm_year = 0;
    (*obsl).timeid2.fm_mon = 0;
    (*obsl).timeid2.fm_mday = 0;
    (*obsl).timeid2.fm_hour = 0;
    (*obsl).timeid2.fm_min = 0;
    (*obsl).timeid2.fm_sec = 0;

    return(FM_OK);
}

int free_stdat(stdatstruct *obsl) {
    char *where="free_stdat";

    if ((*obsl).obs == NULL) {
	fmerrmsg(where,"Your tried to free something not allocated");
	return(FM_MEMALL_ERR);
    }
    
    free((*obsl).obs);
    (*obsl).nostat = 0;
    (*obsl).noobs = 0;
    (*obsl).obs = NULL;

    return(FM_OK);
}

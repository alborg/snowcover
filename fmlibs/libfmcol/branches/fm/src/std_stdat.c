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
 * RETURN VALUES:
 * 0 - correct and normal
 * 2 - I/O error
 * 3 - memory error
 *
 * NOTE:
 * Does not handle drau files properly yet, should discuss timing in such
 * files with Steinar to deceide how to handle them...
 * Implements only handling of synop files at present...
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/12/2001
 */

#include <collocate1.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>
#include <std_stdat.h>

int read_stdat(char *ipath, char *basename, boolean synop,
	time_t timeid, stdatstruct *pobs) {

    time_t stime, deltat;
    short hour;
    boolean failure=TRUE;
    const char *dummy1="UPDATED";
    char *errmsg="read_stdat";
    char *infile;
    struct dirent *direntp;
    struct tm time_str;
    DIR *dirp;

    /*
     * Check that data structure is initialized and ready to use.
     */
    if ((*pobs).obs != NULL) {
	error("read_stdat","stdatstruct was not initialized");
	return(3);
    }
    
    /*
     * First scan directory to get all files therein, then discard files
     * not useable.
     */
    dirp = opendir(ipath);
    if (!dirp) {
	perror("Opendir in read_ascii_stdat did not work properly");
	return(2);
    } 

    infile = (char *) malloc(FILELEN*sizeof(char));
    if (!infile) {
	perror("Could not allocate memory for infile");
	return(3);
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
	hour = (short) ceilf((float)(*pobs).timeid0.ho);
	time_str.tm_sec = 0;
	time_str.tm_min = 0;
	time_str.tm_hour = hour;
	time_str.tm_mday = (*pobs).timeid0.dd;
	time_str.tm_mon = (*pobs).timeid0.mm-1;
	time_str.tm_year = (*pobs).timeid0.yy-1900;
	stime = mktime(&time_str);
	if (strstr(basename,"syno") != NULL) {
	    deltat = (time_t) TSDIFF;
	} else if (strstr(basename,"drau") != NULL) {
	    deltat = (time_t) TDDIFF;
	} 
	if (synop) {
	    deltat = (time_t) SDIFF;
	}

	if (timeid > (stime-deltat) && timeid < (stime+deltat)) {
	    failure = FALSE;
	    break;
	}
    }
    if (failure) {
	error(errmsg,
		"The time difference to satellite data is too large");
	free(infile);
	return(10);
    }

    /*
     * Allocate memory for station data
     */
    printf("\n The chosen file for station data:\n %s\n", infile);
    (*pobs).obs = (obsstruct *) malloc((*pobs).nostat*sizeof(obsstruct));
    if (!(*pobs).obs) {
	error(errmsg,"Could not allocate obsstruct");
	free(infile);
	return(10);
    }

    /*
     * Read the requested file.
     */
    if (fill_stdat_h(infile, timeid, pobs) != 0) {
	error("read_ascii_stdat","Could not access data in file");
	printf(" %s\n", infile);
	if (free_stdat(pobs) != 0) {
	    error("read_stdat","terminal memory leakage...");
	    exit(3);
	}
	free(infile);
	return(10);
    }
    /*
     * Test printout...
     */
    /*
    for (i=0;i<(*pobs).nostat; i++) {
	printf(" %d stID: %s %02d/%02d/%d %02d:%02d %f\n",
		i, 
		((*pobs).obs)[i].stID,
		((*pobs).obs)[i].year,
		((*pobs).obs)[i].month,
		((*pobs).obs)[i].day,
		((*pobs).obs)[i].hour,
		((*pobs).obs)[i].min,
		((*pobs).obs)[i].TdTdTd
	);
    }
    */

    return(0);
}

int get_stdat_h(char *infile, stdatstruct *obsl) {
    char *dummy;
    char *tagvar = "* stID stType lat lon year month day hour min TTT TdTdTd TwTwTw PPPP ff dd RRR E sss N Nh Cl Cm Ch VV ww W1 W2";
    FILE *fp;

    dummy = (char *) malloc(STDATSTRLEN);
    if (!dummy) {
	error("get_stdat_h","Memory allocation dummy");
	return(3);
    }

    fp = fopen(infile,"r");
    if (!fp) {
	printf(" Could not open %s, continues to next file\n", infile);
	free(dummy);
	return(10);
    }

    /*
     * Get station type in this file
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	error("get_stdat_h","Could not read header type");
	printf(" %s\n", infile);
	free(dummy);
	return(2);
    }
    if (sscanf(dummy,"%*c%s",(*obsl).type) != 1) {
	error("get_stdat_h","Could not read type");
	printf(" %s\n", infile);
	free(dummy);
	return(2);
    }

    /*
     * Get first time in file
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	error("get_stdat_h","Could not read header time1");
	printf(" %s\n", infile);
	printf(" %s\n", dummy);
	free(dummy);
	return(2);
    }
    if (sscanf(dummy,"%*c%hd%hd%hd%hd%hd%d",
		&((*obsl).timeid0).yy,
		&((*obsl).timeid0).mm,
		&((*obsl).timeid0).dd,
		&((*obsl).timeid0).ho,
		&((*obsl).timeid0).mi,
		&(*obsl).unixtime0) != 6) {
	error("get_stdat_h","Could not read start time1");
	printf(" %s\n", infile);
	printf(" %s\n", dummy);
	free(dummy);
	return(2);
    }

    /*
     * Get second time in file
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	error("get_stdat_h","Could not read header time2");
	printf(" %s\n", infile);
	printf(" %s\n", dummy);
	free(dummy);
	return(2);
    }
    if (sscanf(dummy,"%*c%hd%hd%hd%hd%hd%d",
		&((*obsl).timeid1).yy,
		&((*obsl).timeid1).mm,
		&((*obsl).timeid1).dd,
		&((*obsl).timeid1).ho,
		&((*obsl).timeid1).mi,
		&(*obsl).unixtime1) != 6) {
	error("get_stdat_h","Could not read start time2");
	printf(" %s\n", infile);
	printf(" %s\n", dummy);
	free(dummy);
	return(2);
    }

    /*
     * Get third time in file
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	error("get_stdat_h","Could not read header time2");
	printf(" %s\n", infile);
	printf(" %s\n", dummy);
	free(dummy);
	return(2);
    }
    if (sscanf(dummy,"%*c%hd%hd%hd%hd%hd%d",
		&((*obsl).timeid2).yy,
		&((*obsl).timeid2).mm,
		&((*obsl).timeid2).dd,
		&((*obsl).timeid2).ho,
		&((*obsl).timeid2).mi,
		&(*obsl).unixtime2) != 6) {
	error("get_stdat_h","Could not read start time2");
	printf(" %s\n", infile);
	printf(" %s\n", dummy);
	free(dummy);
	return(2);
    }

    /*
     * Get number of observations
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	error("get_stdat_h","Could not read header noobs");
	printf(" %s\n", infile);
	printf(" %s\n", dummy);
	free(dummy);
	return(2);
    }
    if (sscanf(dummy,"%*c%*s%d",&(*obsl).noobs) != 1) {
	error("get_stdat_h","Could not read noobs");
	printf(" %s\n", infile);
	printf(" %s\n", dummy);
	free(dummy);
	return(2);
    }

    /*
     * Get number of stations
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	error("get_stdat_h","Could not read header nostat");
	printf(" %s\n", infile);
	printf(" %s\n", dummy);
	free(dummy);
	return(2);
    }
    if (sscanf(dummy,"%*c%*s%d",&(*obsl).nostat) != 1) {
	error("get_stdat_h","Could not read nostat");
	printf(" %s\n", infile);
	printf(" %s\n", dummy);
	free(dummy);
	return(2);
    }
    /*
     * Get variables list and check for consistency
     */
    if (fgets(dummy, (int) STDATSTRLEN, fp) == NULL) {
	error("get_stdat_h","Could not read header variables list");
	printf(" %s\n", infile);
	printf(" %s\n", dummy);
	free(dummy);
	return(2);
    }
    stringchop(dummy);
    if (strcmp(dummy,tagvar) != 0) {
	error("get_stdat_h","Wrong header variables list");
	printf(" %s\n", infile);
	printf(" [%s]\n [%s]\n", dummy, tagvar);
	free(dummy);
	return(2);
    }

    fclose(fp);

    free(dummy);
    return(0);
}


int fill_stdat_h(char *infile, time_t vtime, stdatstruct *obsl) {
    int nf, i;
    int year;
    short month, day, hour, minute;
    char *dummy, *stid, *stidtab;
    char *f1 = "%s%s%f%f%d%hd%hd%hd%hd%f%f%f%f%hd%d%f";
    char *f2 = "%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd%hd";
    char *format;
    time_t stime, deltat;
    struct tm time_str;
    FILE *fp;

    dummy = (char *) malloc(STDATSTRLEN);
    if (!dummy) {
	error("fill_stdat_h","Memory allocation dummy");
	return(3);
    }

    format = (char *) malloc(STDATSTRLEN);
    if (!format) {
	error("fill_stdat_h","Memory allocation format");
	return(3);
    }
    sprintf(format,"%s%s",f1,f2);

    stid = (char *) malloc(10);
    if (!stid) {
	error("fill_stdat_h","Memory allocation stid");
	free(dummy);
	free(format);
	return(3);
    }

    stidtab = (char *) malloc((10+2)*((*obsl).noobs));
    /*printf(" %d\n",(*obsl).noobs);*/
    if (!stidtab) {
	error("fill_stdat_h","Memory allocation stidtab");
	free(dummy);
	free(format);
	free(stid);
	return(3);
    }

    fp = fopen(infile,"r");
    if (!fp) {
	error("fill_stdat_h","Could not open:");
	printf(" %s\n", infile);
	free(dummy);
	free(format);
	free(stid);
	free(stidtab);
	return(2);
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
    return(0);
}

int init_stdat(stdatstruct *obsl) {

    (*obsl).nostat = 0;
    (*obsl).noobs = 0;
    (*obsl).obs = NULL;
    return(0);
}

int free_stdat(stdatstruct *obsl) {

    if ((*obsl).obs == NULL) {
	error("free_stdat_h","Your tried to free something not allocated");
	return(3);
    }
    
    free((*obsl).obs);
    (*obsl).nostat = 0;
    (*obsl).noobs = 0;
    (*obsl).obs = NULL;

    return(0);
}

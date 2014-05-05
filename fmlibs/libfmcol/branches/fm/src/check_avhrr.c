/*
 * NAME:
 * check_avhrr.c
 *
 * PURPOSE:
 * To check the available AVHRR files and return information on these.
 *
 * RETURN VALUES:
 * 0 - correct and normal
 * 2 - I/O error
 * 3 - memory error
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/12/2001
 * 
 * MODIFIED:
 * Øystein Godøy, met.no/FOU, 30.09.2004 
 * Removed a possible memory leak by implementing free(infile).
 */

#include <collocate1.h>
#include <satimg.h>
#include <time.h>
#include <sys/types.h>
#include <dirent.h>

int check_avhrr(char *ipath, char *basename, fstruct *avhrr_f,
	time_t older, time_t newer) {

    int i, j, nf, nwc;
    const char *dummy1=".";
    const char *dummy2="..";
    char *where="check_avhrr";
    char *infile, *ptr, wc[NOWC][NAMELEN];
    char *basenamecopy;
    struct dirent *direntp;
    struct imgh header;
    struct tm time_str;
    DIR *dirp;

    /*
     * Initialize the imgh structure to avoid memory leakage in readheader
     * This is only required for the parts that might be allocated below
     */
    header.track = NULL;
    for (i=0;i<NCHAN;i++) {
	header.image[i] = NULL;
    }

    /*
     * Split basename if several substrings are specified
     */
    /*
    basenamecopy = (char *) malloc(strlen(basename)*sizeof(char));
    */
    basenamecopy = (char *) malloc(2048*sizeof(char));
    if (!basenamecopy) {
	errmsg(where,"Could not create basenamecopy");
	return(3);
    }
    sprintf(basenamecopy,"%s",basename);
    if (basenamecopy) {
	ptr = strtok(basenamecopy,",");
	strncpy(wc[0],ptr,NAMELEN);
	nwc = 1;
	while ((ptr = strtok(NULL,",")) != NULL) {
	    strncpy(wc[nwc],ptr,NAMELEN);
	    nwc++;
	    if (nwc > NOWC) {
		error(where,"Too many wildcards, change header file");
		return(2);
	    }
	}
    }
    free(basenamecopy);

    /*
     * First scan directory to get all files therein, then discard files
     * not useable.
     */
    dirp = opendir(ipath);
    if (!dirp) {
	perror("Opendir in check_avhrr did not work properly");
	return(2);
    } 

    nf = 0;
    while ((direntp = readdir(dirp)) != NULL) {
	if (basename == NULL) {
	    if (strstr(direntp->d_name,dummy1) && 
		    strlen(direntp->d_name) == 1) {
		continue;
	    }
	    if (strstr(direntp->d_name,dummy2)) {
		continue;
	    }
	    nf++;
	} else {
	    for (j=0;j<nwc;j++) {
		if (strstr(direntp->d_name,wc[j])) {
		    nf++;
		    break;
		}
	    }
	}
    }


    (*avhrr_f).n = nf;
    if (nf > FMCOLMAXFILES) {
	errmsg(where,"Increase FMCOLMAXFILES");
	return(3);
    }
    if (nf == 0) return(0);
    if (((*avhrr_f).f) == NULL) {
	(*avhrr_f).f = (fnsstruct *) malloc(nf*sizeof(fnsstruct));
	if (((*avhrr_f).f) == NULL) {
	    perror("Could not allocate memory for AVHRR filelist");
	    return(3);
	}
    }

    rewinddir(dirp);
    i = 0;
    while ((direntp = readdir(dirp)) != NULL) {
	if (basename == NULL) {
	    if (strstr(direntp->d_name,dummy1) && 
		    strlen(direntp->d_name) == 1) {
		continue;
	    }
	    if (strstr(direntp->d_name,dummy2)) {
		continue;
	    }
	    /*strcpy(((*avhrr_f).f[i]).name,direntp->d_name);*/
	    strcpy((*avhrr_f).f[i].name,direntp->d_name);
	    i++;
	} else {
	    for (j=0;j<nwc;j++) {
		if (strstr(direntp->d_name,wc[j])) {
		    /*strcpy(((*avhrr_f).f[i]).name,direntp->d_name);*/
		    strcpy((*avhrr_f).f[i].name,direntp->d_name);
		    i++;
		    break;
		}
	    }
	}
    }
    if (closedir(dirp) != 0) {
	error("check_avhrr","Could not close directory");
	return(2);
    }
    if (i != nf) {
	fprintf(stderr," First time loop: %d, second time loop: %d files\n", 
		nf, i);
	return(1);
    }

    /*
     * Open file and get aquisition time.
     */
    infile = (char *) malloc(FILELEN*sizeof(char));
    if (!infile) {
	perror("Could not allocate memory for infile");
	return(3);
    }

    for (i=0; i<nf; i++) {
	/*sprintf(infile,"%s/%s", ipath,((*avhrr_f).f[i]).name);*/
	sprintf(infile,"%s/%s", ipath,(*avhrr_f).f[i].name);

	if (readheader(infile, &header) == 0) {
	    time_str.tm_year = header.yy-1900;
	    time_str.tm_mon = header.mm-1;
	    time_str.tm_mday = header.dd;
	    time_str.tm_hour = header.ho;
	    time_str.tm_min = header.mi;
	    time_str.tm_sec = 00.;
	    time_str.tm_isdst = 0;

	    ((*avhrr_f).f[i]).unixtime = mktime(&time_str);

	    if (((*avhrr_f).f[i]).unixtime > older) {
		((*avhrr_f).f[i]).unixtime = -1;
	    }
	    if (((*avhrr_f).f[i]).unixtime < newer) {
		((*avhrr_f).f[i]).unixtime = -2;
	    }
	} else {
	    ((*avhrr_f).f[i]).unixtime = -1;
	}
    }
    cleardata(&header);

    free(infile);

    return(0);
}

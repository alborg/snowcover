/*
 * NAME:
 * check_safct.c
 *
 * PURPOSE:
 * To check the available SAFCT files and return information on these.
 *
 * NOTES:
 * NA
 *
 * RETURN VALUES:
 * 0 - correct and normal
 * 2 - I/O error
 * 3 - memory error
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/12/2001
 * Øystein Godøy, DNMI/FOU, 19/12/2002
 */

#include <collocate1.h>
#include <pps_cloudproducts_io.h>
#include <sys/types.h>
#include <dirent.h>

int check_safct(char *ipath, char *basename, fstruct *safct_f, 
	time_t older, time_t newer) {

    int i, j, nf, nwc;
    const char *dummy1=".";
    const char *dummy2="..";
    char *errmsg="check_safct";
    char *infile, *ptr, wc[NOWC][NAMELEN];
    struct dirent *direntp;
    CTy_t *ctype;
    struct tm time_str;
    DIR *dirp;

    /*
     * Split basename if several substrings are specified
     */
    if (basename) {
	ptr = strtok(basename,",");
	strncpy(wc[0],ptr,NAMELEN);
	nwc = 1;
	while ((ptr = strtok(NULL,",")) != NULL) {
	    strncpy(wc[nwc],ptr,NAMELEN);
	    nwc++;
	    if (nwc > NOWC) {
		error(errmsg,"Too many wildcards, change header file");
		return(2);
	    }
	}
    }

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

    (*safct_f).n = nf;
    if (nf == 0) return(0);
    if (((*safct_f).f) == NULL) {
	(*safct_f).f = (fnsstruct *) malloc(nf*sizeof(fnsstruct));
	if (((*safct_f).f) == NULL) {
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
	    strcpy(((*safct_f).f[i]).name,direntp->d_name);
	    i++;
	} else {
	    for (j=0;j<nwc;j++) {
		if (strstr(direntp->d_name,wc[j])) {
		    strcpy(((*safct_f).f[i]).name,direntp->d_name);
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
	sprintf(infile,"%s/%s", ipath,((*safct_f).f[i]).name);

	if ((ctype = readCloudType(infile, FALSE, FALSE, FALSE)) != NULL) {

	    ((*safct_f).f[i]).unixtime = ctype->sec_1970;

	    if (((*safct_f).f[i]).unixtime > older) {
		((*safct_f).f[i]).unixtime = -1;
	    }
	    if (((*safct_f).f[i]).unixtime < newer) {
		((*safct_f).f[i]).unixtime = -2;
	    }
	    freeCloudType(ctype);
	} else {
	    ((*safct_f).f[i]).unixtime = -1;
	}
    }

    return(0);
}

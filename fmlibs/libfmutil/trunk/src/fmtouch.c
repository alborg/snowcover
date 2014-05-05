/*
 * NAME:
 * NA
 *
 * PURPOSE:
 * To touch a file, create it if not existing.
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * NA
 *
 * OUTPUT:
 * NA
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, METNO/FOU, 31.08.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 06.09.2007: Moved include statements to
 * fmtouch.h Whether this sould be the practise or not is yet to be
 * determined.
 * Øystein Godøy, METNO/FOU, 05.10.2007: Added fmfindfile.
 *
 * ID:
 * $Id$
 */

#include <fmtouch.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

int fmtouch(char *filename, time_t newtime) {

    FILE *fp=NULL;
    struct utimbuf chtm;

    chtm.actime = newtime;
    chtm.modtime = newtime;

    if (fmfileexist(filename) == FM_IO_ERR) {
	if ((fp = fopen(filename,"w")) == NULL) {
	    return(FM_IO_ERR);
	}
    }

    if (utime(filename,&chtm) < 0) {
	return(FM_OTHER_ERR);
    }

    if (fp) {
	if (fclose(fp) == EOF) {
	    return(FM_IO_ERR);
	}
    }

    return(FM_OK);
}

int fmfileexist(char *filename) {
   int status;
   struct stat buf;

   if ((status= stat(filename, &buf)) == -1) {
       return(FM_IO_ERR);
   }
   return(FM_OK);
}

char *fmfindfile(char *path, char *datestr, char *timestr, char *areastr) {

    char *where="fmfindfile";
    char what[FMSTRING512];
    static char filename[FMSTRING1024];
    struct dirent *dirpt;
    DIR *dp;
 
    dp = opendir(path);
    if (! dp) {
	sprintf(what,"Could not open %s",path);
	fmerrmsg(where,what);
	return(NULL);
    }

    while ((dirpt = readdir(dp))) {
	if (strstr(dirpt->d_name, datestr) && strstr(dirpt->d_name, timestr) 
	    && strstr(dirpt->d_name,areastr)) {
	    sprintf(filename,"%s/%s",path,dirpt->d_name);
	    closedir(dp);
    	    return(filename);
	}
    }

    return(NULL);
}

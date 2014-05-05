/*
 * NAME:
 * fmfeltfile
 *
 * PURPOSE:
 * To extract some useful information from feltfiles.
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
 * Remember to check byteswapping requirements before using the functions
 * in applications. Default setup is currently with byteswapping.
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, METNO/FOU, 16.10.2007 
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */

#include <fmfeltfile.h>

/*
 * Get time valid time for the HIRLAM feltfile.
 */
int fmfeltfile_gettime(char *filename, fmtime *mytime) {

    char *where = "fmfeltfile_gettime";
    char what[FMSTRING256];
    long position;
    short uspt;
    FILE *fp;

    position = 2*1024*2+4;

    if (!(fp = fopen(filename,"r"))) {
	sprintf(what,"Could not open %s",filename);
	fmerrmsg(where,what);
	return(FM_IO_ERR);
    }

    if (fseek(fp,position,SEEK_SET) < 0) {
	sprintf(what,"Could position within %s",filename);
	fmerrmsg(where,what);
	return(FM_IO_ERR);
    }

    fread(&uspt, sizeof(short), 1, fp);
    fmbs_short(&uspt);
    mytime->fm_year = (int) uspt;
    fread(&uspt, 2, 1, fp);
    fmbs_short(&uspt);
    mytime->fm_mday = (int) (uspt%100);
    mytime->fm_mon = (int) (uspt/100);
    fread(&uspt, 2, 1, fp);
    fmbs_short(&uspt);
    mytime->fm_min = (int) (uspt%100);
    mytime->fm_hour = (int) (uspt/100);
    mytime->fm_sec = 0;

    fclose(fp);

    return(FM_OK);
}

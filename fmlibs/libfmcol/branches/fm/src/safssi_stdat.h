/*
 * NAME:
 * safssi_stdat.h
 * 
 * PURPOSE:
 * Header file for safssi_stdat.c
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * RETURN VALUES:
 *
 * DEPENDENCIES:
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 27/09/2002
 * Øystein Godøy, MI/FOU, 11.04.2003
 */

#ifndef _SAFSSI_STDAT_H
#define _SAFSSI_STDAT_H

#include <dirent.h>

#include <collocate1.h>
#include <safhdf.h>
#include <satimg.h>
#include <fmcoord.h>

#define SAFSSI_MISVAL -999. 

/*
 * This is the structure that will be stored in the database...
 */
typedef struct {
    char source[20];
    int nopix;
    time_t vtime;
    struct ucs nav;
    float data[BOXSIZE2D];
    unsigned short qflg[BOXSIZE2D];
} safssi_data;

int safssi_stdat(struct latlon gpos, fmproj myproj,
	osihdf *safssi, safssi_data *ssi);
int safssi_init(int boxsize, safssi_data *ssi);
int safssi_free(safssi_data *ssi);
char *safssi_prod(char *filename);
int safssi_name(struct dto refdate, char *basename, char *path, char *filename);
int safssi_check(struct imgh refdata, osihdf *safssi);
    
#endif /* _SAFSSI_STDAT_H */


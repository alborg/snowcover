/*
 * NAME:
 * collocate.h
 *
 * PURPOSE:
 * Header file for collocate.c software.
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/12/2001
 * Øystein Godøy, DNMI/FOU, 27/09/2002
 * Øystein Godøy, DNMI/FOU, 29.04.2003
 * Added specification of geographical position and dummy ids
 * Øystein Godøy, met.no/FOU, 02.04.2004
 * Added support for KSPT libraries on Red Hat.
 */

#ifndef _COLLOCATE1_H
#define _COLLOCATE1_H


#ifndef _SPT_BOOL
typedef enum {FALSE, TRUE} boolean;
#define _SPT_BOOL
#endif


#define STIDSLEN 1024
#define FILENAMELEN 256
#define NAMELEN 256
#define FILELEN 256
#define DUMMYLEN 512
#define NOFIELDS 9
#define NOCHAN 6
#define FFNS 4
#define FMCOLMAXFILES 1024

#define BOXSIZE 13
#define BOXSIZE2D 169
#define NWPSIZE 1
#define NWPSIZE2D 1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <satimg.h>

#define DBFILES 20 /* Max allowed classes for process */
#define NOWC 8 /* Number of allowed wildcards */
#define NGPOS 100 /* Initial number of dummy positions allocated */

typedef struct {
    struct latlon pos;
    char id[10];
} gposs;

typedef struct {
    char avhrr_p[FILENAMELEN];
    char synop_p[FILENAMELEN];
    char nwp_p[FILENAMELEN];
    char safcm_p[FILENAMELEN];
    char safssi_p[FILENAMELEN];
    char avhrr_f[NAMELEN];
    char synop_f[NAMELEN];
    char nwp_f[NAMELEN];
    char safcm_f[NAMELEN];
    char safssi_f[NAMELEN];
    char lp_f[NAMELEN];
    char dbfile[DBFILES][FILENAMELEN];
    char classname[DBFILES][NAMELEN];
    char *stids; /* String containing station identifications */
    int ngpos;
    gposs *gpos; /* Struct to hold positions for dummy stations */
    int nocl; /* Number of classes / storage files */
    boolean synop; /* Determines whether to use only synoptic times */
    boolean snow; /* Do I use this?? */
} cfgstruct;

typedef struct {
    time_t unixtime;
    char name[FILENAMELEN];
} fnsstruct;

typedef struct {
    fnsstruct *f;
    int n;
} fstruct;

/* reserve declaration to be used if memory trouble persists...
typedef struct {
    fnsstruct f[FMCOLMAXFILES];
    int n;
} fstruct;
*/

void usage(void);
void error(char *where, char *what);
int decode_cfg(char cfgfile[],cfgstruct *cfg); 
int check_avhrr(char *ipath, char *basename, fstruct *avhrr_f,
	time_t older, time_t newer); 
int check_safct(char *ipath, char *basename, fstruct *safct_f, 
	time_t older, time_t newer); 

#endif /* _COLLOCATE1_H */

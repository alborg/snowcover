/*
 * NAME:
 * fmcol.h
 *
 * PURPOSE:
 * Main header file for libfmcol.
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
 * Øystein Godøy, DNMI/FOU, 21/12/2001
 *
 * MODIFIED:
 * Øystein Godøy, DNMI/FOU, 29.04.2003
 * Added specification of geographical position and dummy ids
 * Øystein Godøy, met.no/FOU, 02.04.2004
 * Added support for KSPT libraries on Red Hat.
 * Øystein Godøy, met.no/FOU, 21.10.2004
 * Øystein Godøy, met.no/FOU, 29.10.2004 
 * Øystein Godøy, met.no/FOU, 01.11.2004 
 * Øystein Godøy, met.no/FOU, 14.12.2004
 * Implemented and updated NSCENES and NPOSITIONS, added elements to
 * control which tiles to read.
 * Øystein Godøy, met.no/FOU, 23.12.2004
 * See decode_cfg.c for details.
 * Øystein Godøy, met.no/FOU, 28.12.2004
 * Changed keyword from GPOS to DPOS for observations from DIANA, added
 * GPOS as keyword for strictly geographical positions (equals STIDS).
 *
 * CVS_ID:
 * $Id$
 */

#ifndef FMCOL_H
#define FMCOL_H

#define STIDSLEN 1024
#define DUMMYIDSLEN 8192
#define FILENAMELEN 256
#define FILELEN 256
#define DUMMYLEN 512
#define NOCHAN 6
#define FMCOLMAXFILES 1024

#define BOXSIZE 13
#define NWPSIZE 1
#define NWPSIZE2D 1

#include <fmcol_config.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fmutil.h>
#ifdef FMCOL_HAVE_LIBFMIO
#include <fmio.h>
#endif
#include <fmcolaccess.h>

#ifndef FMCOL_BOOL
#define FMCOL_BOOL
typedef enum {FMCOL_FALSE,FMCOL_TRUE} fmcol_boolean;
#endif

#define DBFILES 20 /* Max allowed classes for process */
#define NOWC 8 /* Number of allowed wildcards */
#define NSCENES 50 /* Initial number of dummy scenes allocated */
#define NPOSITIONS 256 /* Initial number of postions within dummy scenes allocated */
#define FMCOLIDLEN 50 /* Length of id description in text */
#define FMCOLDESCLEN 50 /* Length of string descriptions */
#define FMCOLAREALEN 10 /* Length of string descriptions for area */

typedef struct {
    fmgeopos pos;
    char desc[FMCOLDESCLEN];
    char area[FMCOLAREALEN];
} gposs;

typedef struct {
    char id[FMCOLIDLEN];
    gposs *pos;
    int npos;
    int nr;
    int ns;
    int gr;
    int at;
} scenes;

typedef struct {
    scenes *scene;
    int nscenes;
} dids;

typedef struct {
    gposs *pos;
    int npos;
} gposses;

typedef struct {
    char avhrr_p[FILENAMELEN];
    char synop_p[FILENAMELEN];
    char nwp_p[FILENAMELEN];
    char safcm_p[FILENAMELEN];
    char safssi_p[FILENAMELEN];
    char safdli_p[FILENAMELEN];
    char avhrr_f[NAMELEN];
    char synop_f[NAMELEN];
    char nwp_f[NAMELEN];
    char safcm_f[NAMELEN];
    char safssi_f[NAMELEN];
    char safdli_f[NAMELEN];
    char lp_f[NAMELEN];
    char dbfile[DBFILES][FILENAMELEN];
    char classname[DBFILES][NAMELEN];
    char *stids; /* String containing station identifications */
    dids did; /* Dummy scenes from UFFDA/DIANA */
    gposses geopos; /* geogr. positions */
    int nocl; /* Number of classes / storage files */
    fmcol_boolean synop; /* Determines whether to use only synoptic times */
    fmcol_boolean snow; /* Do I use this?? */
} cfgstruct;

typedef struct {
    time_t unixtime;
    char name[FILENAMELEN];
} fnsstruct;

typedef struct {
    fnsstruct *f;
    int n;
} fstruct;

void fmcoltst(void);
void error(char *where, char *what);
int decode_cfg(char cfgfile[],cfgstruct *cfg); 
int check_avhrr(char *ipath, char *basename, fstruct *avhrr_f,
	time_t older, time_t newer); 
int check_safct(char *ipath, char *basename, fstruct *safct_f, 
	time_t older, time_t newer); 

#endif /* FMCOL_H */

/*
 * NAME:
 * fmcolaccess.h
 *
 * PURPOSE:
 * Header file for the HDF5 software used to store collocated stattionwise
 * data.
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
 * Øystein Godøy, DNMI/FOU, 27/07/2000
 *
 * MODIFIED:
 * Øystein Godøy, DNMI/FOU, 19/09/2002: Modularized HDF access, and added
 * support for SAFCM...
 * Øystein Godøy, DNMI/FOU, 29.04.2003: Added storage without STDAT
 * Øystein Godøy, MI/FOU, 06.11.2003: Due to errors when extracting
 * safcm_data data, a dummy interface is created to circumvent the problem
 * in the current situation. This involves the use of a slimmer data
 * structure not containing the character strings describing the class
 * names. Functions affected are the high level read* functions accessing
 * the safcm_data structure. In this fix, data are transferred from
 * safcm_data to safcm_data2.
 * Øystein Godøy, met.no/FOU, 20.12.2004: Added dump124.
 * Øystein Godøy, met.no/FOU, 23.12.2004: Added support for SAFDLI.
 * Øystein Godøy, METNO/FOU, 10.09.2007: Rewrote hader file use.
 * Øystein Godøy, METNO/FOU, 28.02.2008: Added read124
 * Øystein Godøy, METNO/FOU, 30.04.2008: Added getclassnames.
 * Øystein Godøy, METNO/FOU, 2013-04-18: Switched to new PPS libraries.
 * Øystein Godøy, METNO/FOU, 2013-05-03: Changed type of as_data vtime
 * from time_t to int as an error weas created earlier when used on 64 bit
 * computers.
 *
 * CVS_ID:
 * $Id$
 */

#include <fmcol_config.h>

#ifndef _HDFACCESS_H
#define _HDFACCESS_H

#ifdef FMCOL_HAVE_LIBHDF5
/*
 * Header files that are required.
 */
#include <string.h>
#include <hdf5.h>
#include <fmutil.h>
/*
#include <avhrr_stdat.h>
#include <safcm_stdat.h>
#include <safssi_stdat.h>
#include <safdli_stdat.h>
#include <nwp_stdat.h>
#include <std_stdat.h>
*/

#define MAXGROUPS 10000
#define MAXDATASETS 10

#define NAMELEN 256
#define BOXSIZE2D 169
#define FMCOL_NO_CLOUDTYPE_VALUES 21

/*
 * Variable declarations...
 */
typedef struct {
    int noobs;
    char **name;
} cnames;

typedef struct {
    char classname[NAMELEN];
    char station[NAMELEN];
    time_t t_start;
    time_t t_end;
} skeys;

typedef struct {
    int noobs;
    skeys *scrit;
    char (*group)[NAMELEN];
} cinfo;

typedef struct {
    char source[20];
    int nochan;
    int nopix;
    int vtime;
    fmucsref nav;
    fmangles ang;
    float ch1[BOXSIZE2D];
    float ch2[BOXSIZE2D];
    float ch3a[BOXSIZE2D];
    float ch3b[BOXSIZE2D];
    float ch4[BOXSIZE2D];
    float ch5[BOXSIZE2D];
} as_data;

typedef struct {
    char stID[10];
    char stType[5]; /* ?? */
    fmgeopos pos;
    int year;
    short month;
    short day;
    short hour;
    short min;
    float TTT;
    float TdTdTd;
    float TwTwTw;
    float PPPP;
    short ff;
    int dd;
    float RRR;
    short E;
    short sss;
    short N;
    short Nh;
    short Cl;
    short Cm;
    short Ch;
    short VV;
    short ww;
    short W1;
    short W2;
} obsstruct;

typedef struct {
    char type[5];
    fmtime timeid0;
    fmtime timeid1;
    fmtime timeid2;
    int unixtime0;
    int unixtime1;
    int unixtime2;
    int noobs;
    int nostat;
    obsstruct *obs;
} stdatstruct;

typedef struct {
    int nopar;
    int nopix;
    time_t vtime;
    int ltime;
    fmucsref nav; 
    float t0m[BOXSIZE2D]; /* 0m temp */
    float t2m[BOXSIZE2D]; /* 2m temp */
    float t950hpa[BOXSIZE2D]; /* temp at 950 hPa */
    float t800hpa[BOXSIZE2D]; /* temp at 800 hPa */
    float t700hpa[BOXSIZE2D]; /* temp at 700 hPa */
    float t500hpa[BOXSIZE2D]; /* temp at 500 hPa */
    float ps[BOXSIZE2D]; /* mean sea level pressure */
    float topo[BOXSIZE2D]; /* model topography */
    float pw[BOXSIZE2D]; /* precipitable water */
    float rh[BOXSIZE2D]; /* relative humidity at surface */
} ns_data;

typedef struct {
    char source[20];
    int nopix;
    time_t vtime;
    fmucsref nav;
    unsigned char data[BOXSIZE2D];
    char description[FMCOL_NO_CLOUDTYPE_VALUES][FMSTRING256];
} safcm_data;

typedef struct {
    char source[20];
    int nopix;
    time_t vtime;
    unsigned char data[BOXSIZE2D];
} safcm_data2;

typedef struct {
    char source[20];
    int nopix; 
    time_t vtime;
    fmucsref nav;
    float data[BOXSIZE2D];
    unsigned short qflg[BOXSIZE2D];
} safssi_data;

typedef struct {
    char source[20];
    int nopix;
    time_t vtime;
    fmucsref nav;
    float data[BOXSIZE2D];
    unsigned short qflg[BOXSIZE2D];
} safdli_data;


/*
 * Top level functions to use in main program
 * The top level functions are named according to a numbering of data
 * sources (see fmcolaccess.c for correct list at any time).
 * 	AVHRR	- 1 - AVHRR data as given by MEOS
 * 	NWP	- 2 - HIRLAM NWP data
 * 	STDAT	- 3 - SYNOP or DRAU data
 * 	SAFCM	- 4 - SAFNWC cloud type product
 * 	SAFSSI	- 5 - SAFOSI SSI product
 * 	SAFDLI	- 6 - SAFOSI DLI product
 */
int dump123(char *filename, char *class, 
	as_data aobs, ns_data nobs, obsstruct sobs);
#ifdef FMCOL_HAVE_LIBSMHI_SAF
int dump1234(char *filename, char *class, 
	as_data aobs, ns_data nobs, obsstruct sobs, safcm_data cobs);
int dump124(char *filename, char *class, 
	as_data aobs, ns_data nobs, safcm_data cobs);
#ifdef FMCOL_HAVE_LIBOSIHDF5
int dump12345(char *filename, char *class, 
	as_data aobs, ns_data nobs, obsstruct sobs, 
	safcm_data cobs, safssi_data ssi);
int dump123456(char *filename, char *class, 
	as_data aobs, ns_data nobs, obsstruct sobs, 
	safcm_data cobs, safssi_data ssi, safdli_data dli);
int dump1245(char *filename, char *class, 
	as_data aobs, ns_data nobs,
	safcm_data cobs, safssi_data ssi);
int dump12456(char *filename, char *class, 
	as_data aobs, ns_data nobs,
	safcm_data cobs, safssi_data ssi, safdli_data dli);
#endif
#endif
#ifdef FMCOL_HAVE_LIBSMHI_SAF
int dump34(char *filename, char *class, 
	obsstruct sobs, safcm_data cobs);
#ifdef FMCOL_HAVE_LIBOSIHDF5
int dump456(char *filename, char *class, 
	safcm_data cobs, safssi_data iobs, safdli_data dobs);
#endif
#endif
#ifdef FMCOL_HAVE_LIBSMHI_SAF
#ifdef FMCOL_HAVE_LIBOSIHDF5
int read12345(char *filename, skeys *scrit, 
	as_data **aobs, ns_data **nobs, obsstruct **sobs, 
	safcm_data2 **cobs, safssi_data **iobs);
int read123456(char *filename, skeys *scrit, 
	as_data **aobs, ns_data **nobs, obsstruct **sobs, 
	safcm_data2 **cobs, safssi_data **iobs, safdli_data **dobs);
int read1245(char *filename, skeys *scrit, 
	as_data **aobs, ns_data **nobs,
	safcm_data2 **cmobs, safssi_data **iobs);
int read12456(char *filename, skeys *scrit, 
	as_data **aobs, ns_data **nobs,
	safcm_data2 **cmobs, safssi_data **iobs, safdli_data **dobs);
#endif
int read1234(char *filename, skeys *scrit, 
	as_data **aobs, ns_data **nobs, obsstruct **sobs,
	safcm_data2 **cobs);
int read124(char *filename, skeys *scrit, 
	as_data **aobs, ns_data **nobs, safcm_data2 **cobs);
#endif
int read123(char *filename, skeys *scrit, 
	as_data **aobs, ns_data **nobs, obsstruct **sobs);
#ifdef FMCOL_HAVE_LIBSMHI_SAF
int read34(char *filename, skeys *scrit, obsstruct **sobs, safcm_data2 **cobs);
#ifdef FMCOL_HAVE_LIBOSIHDF5
int read456(char *filename, skeys *scrit, 
	safcm_data2 **cmobs, safssi_data **iobs, safdli_data **dobs);
#endif
#endif
int checknorec(char *filename, skeys *scrit);
int getclassnames(char *filename, cnames *cn); /* FIXME */

/*
 * Functions below are only used by the top level functions, not in the
 * main program.
 * Data dump to file
 */
hid_t openFile(char *filename);
herr_t read_items(hid_t loc_id, const char *name, void *opdata); 
herr_t count_basegroups(hid_t loc_id, const char *name, void *opdata); 
herr_t count_datasets(hid_t loc_id, const char *name, void *opdata); 
herr_t dumpAVHRR2HDF(hid_t file, hid_t grp, as_data aobs);
herr_t dumpNWP2HDF(hid_t file, hid_t grp, ns_data nobs);
herr_t dumpSTDAT2HDF(hid_t file, hid_t grp, obsstruct sobs);
#ifdef FMCOL_HAVE_LIBSMHI_SAF
herr_t dumpSAFCM2HDF(hid_t file, hid_t grp, safcm_data cmobs);
#endif
#ifdef FMCOL_HAVE_LIBOSIHDF5
herr_t dumpSAFSSI2HDF(hid_t file, hid_t grp, safssi_data ssiobs);
herr_t dumpSAFDLI2HDF(hid_t file, hid_t grp, safdli_data dliobs);
#endif
herr_t readSTDAT(hid_t file, hid_t grp, obsstruct *obs);
herr_t readAVHRR(hid_t file, hid_t grp, as_data *obs);
herr_t readNWP(hid_t file, hid_t grp, ns_data *obs);
#ifdef FMCOL_HAVE_LIBSMHI_SAF
/*herr_t readSAFCM(hid_t file, hid_t grp, safcm_data *obs);*/
herr_t readSAFCM(hid_t file, hid_t grp, safcm_data2 *obs);
herr_t readSAFCMRAW(hid_t file, hid_t grp, safcm_data2 *obs);
#endif
#ifdef FMCOL_HAVE_LIBOSIHDF5
herr_t readSAFSSI(hid_t file, hid_t grp, safssi_data *obs);
herr_t readSAFDLI(hid_t file, hid_t grp, safdli_data *obs);
#endif

/*
 * Compound datatypes required
 */
hid_t compoundAVHRR(int nopix);
hid_t compoundNWP(int nopix);
hid_t compoundSTDAT(void);
hid_t compoundSAFCM(int nopix);
hid_t compoundSAFCMRAW(int nopix);
hid_t compoundSAFSSI(int nopix);
hid_t compoundSAFDLI(int nopix);
hid_t compoundUCS(void);
hid_t compoundIMGA(void);
hid_t compoundLATLON(void);

/*
 * End function prototypes.
 */

#endif /* FMCOL_HAVE_LIBHDF5 */
#endif /* _HDFACCESS_H */

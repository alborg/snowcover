/*
 * NAME:
 * fmio.h
 *
 * PURPOSE:
 * Top include file for libfmio.
 *
 * REQUIREMENTS:
 * o libfmutil
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, METNO/FOU, 08.06.2005 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 16.10.2006
 * Modified name space for libfmio.
 */

#ifndef FMIO_H
#define FMIO_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <fmutil.h>
/* I believe this is MEOS related...
#include <products.h>
*/

#define FMIO_MISSING -32767
#define FMIO_MEMERROR -999
#define FMIO_REARTH 6371.
#define FMIO_SATSIZE 81
#define FMIO_NCHAN 8
#define FMIO_MAXCHANNELS 6
#define FMIO_MAXIMGSIZE 1440000
#define FMIO_TIFFHEAD 1024
#define FMIO_FIELDS 19

typedef struct {
    float latitude;
    float longitude;
} fmio_subtrack;

/*
 * Structure to hold image information from MEOS10 HDF files containing AVHRR
 * data. (or METSAT)...
 */
typedef struct {
    unsigned int iw; /* image width (pixels) */
    unsigned int ih; /* image height (pixels) */
    unsigned int size; /* image size (pixels) */
    fmio_subtrack *track; /* subsatellite track (lat, lon) */
    int numtrack; /* no. of subsattracks */
    float rga; /* albedo gain value */
    float ria; /* albedo intercept value */
    float rgt; /* temperature gain value */
    float rit; /* temperature intercept value */
    char sa[20];  /* satellite name */
    char area[20];  /* area name */
    /*char source[20];*/  /* process name */
    unsigned short int ho; /* satellite hour */
    unsigned short int mi; /* satellite minute */
    unsigned short int dd; /* satellite day */
    unsigned short int mm; /* satellite month */
    unsigned short int yy; /* satellite year */
    unsigned short int z; /* satellite no. of channels */
    unsigned short int ch[FMIO_NCHAN]; /* satellite channels */
    float Bx; /* UCS Bx */
    float By; /* UCS By */
    float Ax; /* UCS Ax */
    float Ay; /* UCS Ay */
    unsigned short *image[FMIO_NCHAN]; /* image data for all channels */
} fmio_img;

/*
 * Old structure to hold information about AVHRR data, this one was originally
 * created for use with DNMI/TIFF files.
 */
typedef struct {
    char satellite[30];
    unsigned short int hour;
    unsigned short int minute;
    unsigned short int day;
    unsigned short int month;
    unsigned short int year;
    unsigned short int satdir;
    unsigned short int ch[FMIO_NCHAN];
    unsigned int zsize;
    unsigned int xsize;
    unsigned int ysize;
    float Ax;
    float Ay;
    float Bx;
    float By;
} fmio_mihead;

/*
 * Structure to hold information for color palette imagery.
 */
typedef struct {
    char *name;
    unsigned short int noofcl;
    char **clname;
    unsigned short cmap[3][256];
} fmio_mihead_pal;


#ifdef HAVE_TIFF
int fm_MITIFF_fillhead(char *asciifield, char *tag, fmio_mihead *ginfo); 
int fm_MITIFF_fillhead_imagepal(char *asciifield, char *tag, fmio_mihead_pal *palinfo); 
#endif
#ifdef HAVE_HDF5
#endif
int fm_readheader(char *filename, fmio_img *h); 
int fm_readheaderMETSAT(char *filename, fmio_img *h); 
int fm_readdata(char *satfile, fmio_img *h);
int fm_readdataMETSAT(char *satfile, fmio_img *h);

#endif /* FMIO_H */

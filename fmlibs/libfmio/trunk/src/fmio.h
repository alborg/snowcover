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
 * �ystein God�y, METNO/FOU, 08.06.2005 
 *
 * MODIFIED:
 * �ystein God�y, METNO/FOU, 16.10.2006: Modified name space for libfmio.
 * Thomas Lavergne, METNO/FOU, ??: Something??
 * �ystein God�y, METNO/FOU, 2011-01-21: Added orbit_no to fmio_mihead and
 * added fm_img2mihead.
 *
 * ID:
 * $Id$
 */

#ifndef FMIO_H
#define FMIO_H

#include <fmio_config.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#include <fmutil.h>

#define FMIO_MISSING -32767
#define FMIO_MEMERROR -999
#define FMIO_REARTH 6371.
#define FMIO_SATSIZE 81
#define FMIO_NCHAN 8
#define FMIO_MAXCHANNELS 6
/*#define FMIO_MAXIMGSIZE 1440000*/
#define FMIO_MAXIMGSIZE 25000000
#define FMIO_TIFFHEAD 1024
#define FMIO_FIELDS 19

typedef struct fmio_subtrack_ {
    float latitude;
    float longitude;
} fmio_subtrack;

/*
 * Structure to hold image information from MEOS10 HDF files containing AVHRR
 * data. (or METSAT)...
 */
typedef struct fmio_img_ {
    unsigned int iw; /* image width (pixels) */
    unsigned int ih; /* image height (pixels) */
    unsigned int size; /* image size (pixels) */
    unsigned int outofimageval; /* value attributed to outofimage */
    fmio_subtrack *track; /* subsatellite track (lat, lon) */
    int numtrack; /* no. of subsattracks */
    float rga; /* albedo gain value */
    float ria; /* albedo intercept value */
    float rgt; /* temperature gain value */
    float rit; /* temperature intercept value */
    float cover; /* % coverage within image */
    char sa[20];  /* satellite name */
    char area[20];  /* area name */
    char processing[20];  /* process name */
    unsigned short int ho; /* satellite hour */
    unsigned short int mi; /* satellite minute */
    unsigned short int dd; /* satellite day */
    unsigned short int mm; /* satellite month */
    unsigned short int yy; /* satellite year */
    unsigned short int z; /* satellite no. of channels */
    unsigned short int ch[FMIO_NCHAN]; /* satellite channels */
    int orbit_no; /* orbit number */
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
typedef struct fmio_mihead_ {
    char satellite[50];
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
    int orbit_no;
    float Ax;
    float Ay;
    float Bx;
    float By;
} fmio_mihead;

/*
 * Structure to hold information for color palette imagery.
 */
typedef struct fmio_mihead_pal_ {
    char *name;
    unsigned short noofcl;
    char **clname;
    unsigned short cmap[3][256];
} fmio_mihead_pal;

#ifdef FMIO_HAVE_LIBTIFF
int fm_MITIFF_read(char *infile, unsigned char *image[], 
    fmio_mihead *ginfo); 
int fm_MITIFF_read_imagepal(char *infile, unsigned char *image[], 
    fmio_mihead *ginfo, fmio_mihead_pal *palinfo); 
int fm_MITIFF_write(char *outfile, unsigned char *image, char newhead[], 
    fmio_mihead ginfo); 
int fm_MITIFF_write_rgb(char *outfile, unsigned char *image[], char newhead[], 
    fmio_mihead ginfo); 
int fm_MITIFF_write_imagepal(char *outfile, unsigned char *class, 
    char newhead[], fmio_mihead ginfo, unsigned short cmap[3][256]); 
int fm_MITIFF_write_multi(char *outfile, unsigned char *image[], 
    char newhead[], fmio_mihead ginfo);
int fm_MITIFF_fillhead(char *asciifield, char *tag, fmio_mihead *ginfo); 
int fm_MITIFF_fillhead_imagepal(char *asciifield, char *tag, fmio_mihead_pal *palinfo); 
void fm_MITIFF_create_head(char imginfo[],char satellite[],char time[],
      short satdir, short zsize, char chdesc[], int xsize, int ysize, 
      char projection[], char truelat[], float rot, int xunit, int yunit, 
      float npx, float npy, float ax, float ay, float bx, float by, char *calib);
#endif
#ifdef FMIO_HAVE_LIBHDF5
int fm_readHLHDFdata(char *filename, fmdataset *d, fmbool headeronly); 
#endif
int fm_init_fmio_img(fmio_img *h);
int fm_clear_fmio_img(fmio_img *h);
int fm_readheader(char *filename, fmio_img *h); 
int fm_readheaderMETSAT(char *filename, fmio_img *h); 
int fm_readheaderMETSATswath(char *filename, fmio_img *h);
int fm_readdata(char *satfile, fmio_img *h);
int fm_readdataMETSAT(char *satfile, fmio_img *h);
int fm_readMETSATdata_swath(char *satfile, fmio_img *h);
int fm_img2slopes(fmio_img imghead, fmscale *newcal);
int fm_img2fmtime(fmio_img imghead, fmtime *newdate);
int fm_img2fmucsref(fmio_img imghead, fmucsref *refucs);
float fm_byte2float(unsigned short value, fmscale byte2float, char *keyword);
int fm_img2mihead(fmio_img orgimg, fmio_mihead *newhead); 
int fm_readfmdataset(char *filename, fmdataset *d, fmbool headeronly); 

#endif /* FMIO_H */

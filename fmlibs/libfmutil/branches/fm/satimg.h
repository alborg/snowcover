/*
 * NAME:
 * satimg.h
 *
 * PURPOSE:
 * Header file for I/O and utility library handling local AVHRR data and
 * to some extent other raster data.
 * 
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 14/01/1999
 * MODIFIED:
 * Steinar Eastwood, met.no/FoU, 27.01.2004
 * Øystein Godøy, met.no/FOU, 23.02.2004
 * Added prototyping of readheaderHDF5 and time functions.
 * Øystein Godøy, met.no/FOU, 04.03.2004
 * Fixed bug report by Vibeke W. Thyness.
 * Øystein Godøy, met.no/FOU, 29.03.2004
 * Added define statement to handle both Red Hat and Debian versions of
 * libmeos (they use FAILURE on Debian and SP_FAILURE on Red Hat).
 * Øystein Godøy, met.no/FOU, 30.04.2004
 * Prototyping of functions for conversion to/from modified julian dates
 * are added.
 * Øystein Godøy, met.no/FOU, 23.12.2004
 * Changed toairrad interface.
 */


#ifndef _FMLIB_H
#define _FMLIB_H

/* 
 * The following is required to avoid warning messages from libmeos 
 * products.h on Debian.
 */
#define lint 0 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <products.h>

#define MISSING -32767
#define MEMERROR -999
#define rms_PI 3.141592654
#define fmPI rms_PI
#define REARTH 6371.
#define SATSIZE 81
#define NCHAN 8
#define MAXCHANNELS 6
#define MAXIMGSIZE 1440000
#define TIFFHEAD 1024
#define FIELDS 19

/* 
 * The following is required because libmeos include files differ for the
 * Debian and Red Hat versions we have got... According to both versions
 * FAILURE is negative (-1) and SUCCESS is positive (1). By using our own
 * names and ensuring they are consistent with the return values of the
 * libmeos functions we avoid this problem.
 */
#define FM_KSPT_FAILURE -1
#define FM_KSPT_SUCCESS 1

typedef unsigned int ui;
typedef unsigned short int usi;
typedef unsigned char uca;

/*
 * Structure to hold geographical positions.
 */
typedef struct latlon {
    float lat;
    float lon;
} strgpos;

/*
 * Structure to hold Cartesian positions.
 */
struct xy {
    int x;
    int y;
};

/*
 * Structure to hold image information from MEOS10 HDF files containing AVHRR
 * data. (or METSAT)...
 */
struct imgh {
    unsigned int iw; /* image width (pixels) */
    unsigned int ih; /* image height (pixels) */
    unsigned int size; /* image size (pixels) */
    struct sat_track *track; /* subsatellite track (lat, lon) */
    int numtrack; /* no. of subsattracks */
    float rga; /* albedo gain value */
    float ria; /* albedo intercept value */
    float rgt; /* temperature gain value */
    float rit; /* temperature intercept value */
    char sa[20];  /* satellite name */
    char area[20];  /* area name */
    /*char source[20];*/  /* process name */
    usi ho; /* satellite hour */
    usi mi; /* satellite minute */
    usi dd; /* satellite day */
    usi mm; /* satellite month */
    usi yy; /* satellite year */
    usi z; /* satellite no. of channels */
    usi ch[NCHAN]; /* satellite channels */
    float Bx; /* UCS Bx */
    float By; /* UCS By */
    float Ax; /* UCS Ax */
    float Ay; /* UCS Ay */
    unsigned short *image[NCHAN]; /* image data for all channels */
};

/*
 * Old structure to hold information about AVHRR data, this one was originally
 * created for use with DNMI/TIFF files.
 */
struct mihead {
    char satellite[30];
    unsigned short int hour;
    unsigned short int minute;
    unsigned short int day;
    unsigned short int month;
    unsigned short int year;
    unsigned short int satdir;
    unsigned short int ch[NCHAN];
    unsigned int zsize;
    unsigned int xsize;
    unsigned int ysize;
    float Ax;
    float Ay;
    float Bx;
    float By;
};

/*
 * Structure to hold information for color palette imagery.
 */
struct miheadpal {
    char *name;
    usi noofcl;
    char **clname;
    unsigned short cmap[3][256];
};

struct sbtr {
    struct sat_track *track; /* subsatellite track (lat, lon) */
    int numtrack; /* no. of subsattracks */
};

struct dto {
    usi ho; /* satellite hour */
    usi mi; /* satellite minute */
    usi dd; /* satellite day */
    usi mm; /* satellite month */
    usi yy; /* satellite year */
};

typedef struct {
    int     fm_sec;    /* seconds */
    int     fm_min;    /* minutes */
    int     fm_hour;   /* hours */
    int     fm_mday;   /* day of the month */
    int     fm_mon;    /* month */
    int     fm_year;   /* year */
    int     fm_wday;   /* day of the week */
    int     fm_yday;   /* day in the year */
} fmtime;

typedef long fmsec1970; /* unix time */

typedef struct ucs {
    float Bx; /* UCS Bx */
    float By; /* UCS By */
    float Ax; /* UCS Ax */
    float Ay; /* UCS Ay */
    unsigned int iw; /* image width (pixels) */
    unsigned int ih; /* image height (pixels) */
} strucs;

struct clb {
    float rga; /* albedo gain value */
    float ria; /* albedo intercept value */
    float rgt; /* temperature gain value */
    float rit; /* temperature intercept value */
};

struct imga {
    float soz; /* solar zenith angle */
    float saz; /* satellite zenith angle */
    float raz; /* relative azimuth angle */
    float aza; /* satellite azimuth angle */
    float azu; /* solar azimuth angle */
};

/* Functions prototypes */
#ifdef __cplusplus
extern "C" {
#endif

int angsat(struct ucs upos, struct latlon gpos, struct dto date,
    struct sbtr subt, struct imga *ang);
float ausqratvar(int jd); 
float bidirref(int jd, float soz, float alb); 
void shortbyteswap(short int *data, int ndata); 
void intbyteswap(int *data, int ndata); 
void bs_int(int *data);
void bs_short(short int *data);
float calib(unsigned short val, int ch, struct clb cp);
int cleardata(struct imgh *h);
int convmeosdate(char s[], struct dto *d); 
int imghead2cal(struct imgh imghead, struct clb *newcal);
int imghead2dto(struct imgh imghead, struct dto *newdate); 
int imghead2fmtime(struct imgh imghead, fmtime *newdate);
int imghead2mihead(struct imgh oldhead, struct mihead *newhead);
int imghead2ucs(struct imgh imghead, struct ucs *newucs);
struct dto unixtime2dto(time_t unixtime);
time_t dto2unixtime(struct dto tidspkt);
int initdata(struct imgh *h);
long ivec(long x, long y, unsigned long ny);
int latlon2UCS(struct ucs upos, struct latlon gpos, 
    float *Bx1, float *By1, struct xy *xypos);
void memerror(void);
void errmsg(char *where, char *what); 
void logmsg(char *where, char *what);
void normalize(struct imgh *img);
char *noline(char *field);
int readheader(char *filename, struct imgh *h); 
int readheaderHDF4(char *filename, struct imgh *h); 
int readheaderHDF5(char *filename, struct imgh *h); 
int readheaderMETSAT(char *filename, struct imgh *h); 
int readdata(char *satfile, struct imgh *h);
int readdataHDF4(char *satfile, struct imgh *h);
int readdataHDF5(char *satfile, struct imgh *h);
int readdataMETSAT(char *satfile, struct imgh *h);
int readdata8(char *satfile, struct imgh *h);
int sbtrclst(struct sbtr st, struct ucs upos, struct latlon gpos, 
    struct xy *pt1, struct xy *pt2);
int sbtritr(struct ucs up, struct xy pt1, struct xy pt2, struct xy pt3, 
    struct xy *stp);
short selalg(struct dto d, struct ucs upos, float hmax, float hmin);
int sphtrigclst(struct latlon st1, struct latlon st2, struct latlon tp,
    struct latlon *st);
void stringchop(char *mystr); 
int JulianDay(usi yy, usi mm, usi dd);
int UCS2latlon(struct ucs upos, struct xy xypos, struct latlon *gpos);
int MITIFF_read(char *infile, unsigned char *image[], struct mihead *ginfo); 
int MITIFF_read_imagepal(char *infile, unsigned char *image[], struct mihead
    *ginfo, struct miheadpal *palinfo); 
int MITIFF_write(char *outfile, unsigned char *image, char newhead[], 
    struct mihead ginfo); 
int MITIFF_write_multi(char *outfile, unsigned char *image[], char newhead[], 
    struct mihead ginfo); 
int MITIFF_write_rgb(char *outfile, unsigned char *image[], char newhead[], 
    struct mihead ginfo); 
int MITIFF_write_imagepal(char *outfile, unsigned char *class, 
    char newhead[], struct mihead ginfo, unsigned short cmap[3][256]); 
void MITIFF_head(char imginfo[TIFFHEAD], char satellite[16], char time[17], 
    short satdir, short zsize, char chdesc[20], int xsize, int ysize, 
    char projection[30], char truelat[10], float rot, int xunit, int yunit, 
    float npx, float npy, float ax, float ay, float bx, float by, char *calib); 
int fillhead(char *asciifield, char *tag, struct mihead *ginfo); 
int fillhead_imagepal(char *asciifield, char *tag, struct miheadpal *palinfo); 
/*double sunz(fmsec1970 tst, double latitude);*/
double sunz(fmsec1970 tst, struct latlon gpos); 
fmsec1970 utc2tst(fmsec1970 utc, double lon);
fmsec1970 cet2tst(fmsec1970 cet, double lon);
fmsec1970 loncorr(double meridian, double lon);
fmsec1970 equationoftime(int doy);
double declination(int doy);
double hourangle(double tst);
double esd(int doy);
double toairrad(fmsec1970 tst, struct latlon gpos, double s0);
int tofmtime(fmsec1970 secs, fmtime *fmt); 
fmsec1970 tofmsec1970(fmtime t); 
fmsec1970 ymdhms2fmsec1970(char *str,int mode);
double deg2rad(double degrees);
double rad2deg(double radians);
void mjd19502fmtime(double mjd, fmtime *cal); 
void fmtime2mjd1950(fmtime cal, double *mjd); 
void fmheatmap(int noclasses,
	unsigned short red[256], 
	unsigned short green[256], 
	unsigned short blue[256]); 
void fmmapb2g(int noclasses,
	unsigned short red[256], 
	unsigned short green[256], 
	unsigned short blue[256]); 
void fmmapr2g(int noclasses,
	unsigned short red[256], 
	unsigned short green[256], 
	unsigned short blue[256]); 

#ifdef __cplusplus
}
#endif

#endif /* _FMLIB_H */

/* 
 * File : fmutil.h
 * Generated automatically on Thu Apr 10 14:08:42 CEST 2014
 * By ./prepare_fmutil_h.sh
 * From: /home/anettelb/prog/satellite/snoflak/fmlibs/libfmutil/trunk/src/
		fmutil_config.h
		fmutil_types.h
		fmcoord.h
		fmtime.h
		fmsolar.h
		fmangles.h
		fmangleconversion.h
		fmstrings.h
		fmstorage.h
		fmimage.h
		fmbyteswap.h
		fmcolormaps.h
		fmerrmsg.h
		fm_ch3b_reflectance.h
		fmtouch.h
		fmfeltfile.h
		fmfilesystem.h
*/

#ifndef FMUTIL_H
#define FMUTIL_H

#ifdef __cplusplus
extern "C" {
#endif
#include <time.h>
#ifndef _SRC_FMUTIL_CONFIG_H
#define _SRC_FMUTIL_CONFIG_H 1
 
/* src/fmutil_config.h. Generated automatically at end of configure. */
/* src/config.h.  Generated from config.h.in by configure.  */
/* src/config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef FMUTIL_HAVE_INTTYPES_H 
#define FMUTIL_HAVE_INTTYPES_H  1 
#endif

/* Define if proj is available */
#ifndef FMUTIL_HAVE_LIBPROJ 
#define FMUTIL_HAVE_LIBPROJ  1 
#endif

/* Define to 1 if you have the <math.h> header file. */
#ifndef FMUTIL_HAVE_MATH_H 
#define FMUTIL_HAVE_MATH_H  1 
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef FMUTIL_HAVE_MEMORY_H 
#define FMUTIL_HAVE_MEMORY_H  1 
#endif

/* Define to 1 if you have the <stdarg.h> header file. */
#ifndef FMUTIL_HAVE_STDARG_H 
#define FMUTIL_HAVE_STDARG_H  1 
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef FMUTIL_HAVE_STDINT_H 
#define FMUTIL_HAVE_STDINT_H  1 
#endif

/* Define to 1 if you have the <stdio.h> header file. */
#ifndef FMUTIL_HAVE_STDIO_H 
#define FMUTIL_HAVE_STDIO_H  1 
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef FMUTIL_HAVE_STDLIB_H 
#define FMUTIL_HAVE_STDLIB_H  1 
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef FMUTIL_HAVE_STRINGS_H 
#define FMUTIL_HAVE_STRINGS_H  1 
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef FMUTIL_HAVE_STRING_H 
#define FMUTIL_HAVE_STRING_H  1 
#endif

/* Define to 1 if you have the `strtof' function. */
#ifndef FMUTIL_HAVE_STRTOF 
#define FMUTIL_HAVE_STRTOF  1 
#endif

/* Define to 1 if you have the `strtol' function. */
#ifndef FMUTIL_HAVE_STRTOL 
#define FMUTIL_HAVE_STRTOL  1 
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef FMUTIL_HAVE_SYS_STAT_H 
#define FMUTIL_HAVE_SYS_STAT_H  1 
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef FMUTIL_HAVE_SYS_TYPES_H 
#define FMUTIL_HAVE_SYS_TYPES_H  1 
#endif

/* Define to 1 if you have the <time.h> header file. */
#ifndef FMUTIL_HAVE_TIME_H 
#define FMUTIL_HAVE_TIME_H  1 
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef FMUTIL_HAVE_UNISTD_H 
#define FMUTIL_HAVE_UNISTD_H  1 
#endif

/* Name of package */
#ifndef FMUTIL_PACKAGE 
#define FMUTIL_PACKAGE  "libfmutil" 
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef FMUTIL_PACKAGE_BUGREPORT 
#define FMUTIL_PACKAGE_BUGREPORT  "o.godoy@met.no" 
#endif

/* Define to the full name of this package. */
#ifndef FMUTIL_PACKAGE_NAME 
#define FMUTIL_PACKAGE_NAME  "METNO Remote Sensing Applications Utility Library" 
#endif

/* Define to the full name and version of this package. */
#ifndef FMUTIL_PACKAGE_STRING 
#define FMUTIL_PACKAGE_STRING  "METNO Remote Sensing Applications Utility Library 1.0" 
#endif

/* Define to the one symbol short name of this package. */
#ifndef FMUTIL_PACKAGE_TARNAME 
#define FMUTIL_PACKAGE_TARNAME  "libfmutil" 
#endif

/* Define to the home page for this package. */
#ifndef FMUTIL_PACKAGE_URL 
#define FMUTIL_PACKAGE_URL  "" 
#endif

/* Define to the version of this package. */
#ifndef FMUTIL_PACKAGE_VERSION 
#define FMUTIL_PACKAGE_VERSION  "1.0" 
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef FMUTIL_STDC_HEADERS 
#define FMUTIL_STDC_HEADERS  1 
#endif

/* Version number of package */
#ifndef FMUTIL_VERSION 
#define FMUTIL_VERSION  "1.0" 
#endif
 
/* once: _SRC_FMUTIL_CONFIG_H */
#endif
/*
 * NAME:
 * NA
 *
 * PURPOSE:
 * NA
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
 * Thomas Lavergne
 *
 * MODIFIED:
 * �ystein God�y, METNO/FOU, 31.08.2007 
 *
 * ID:
 * $Id$
 */

#ifndef FMUTIL_TYPES_H
#define FMUTIL_TYPES_H

#ifndef FMBOOL
#define FMBOOL
typedef enum {FMFALSE, FMTRUE} fmboolean, fmbool;
#endif

#ifndef FMPI
#define FMPI
#define fmPI 3.141592654
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.0174532925199432958
#endif
#ifndef RAD_TO_DEG
#define RAD_TO_DEG 57.29577951308232
#endif

#ifndef FMREARTH
#define FMREARTH 6371.
#endif

#ifndef FMRETURNCODES
typedef enum {
    FM_OK,
    FM_SYNTAX_ERR,
    FM_IO_ERR,
    FM_MEMALL_ERR,
    FM_MEMFREE_ERR,
    FM_VAROUTOFSCOPE_ERR,
    FM_OTHER_ERR
} fmreturncode;
#endif

#endif
/*
 * NAME:
 * fmcoord.h
 *
 * PURPOSE:
 * See fmcoord.c
 *
 * REQUIREMENTS:
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
 * �ystein God�y, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */

#ifndef FMCOORD_H
#define FMCOORD_H

#ifndef FMPROJ
#define FMPROJ
typedef enum {MI,MEOS} fmprojspec;
#endif
/* FMTILES are not in use yet... */
#ifndef FMTILES
#define FMTILES
typedef enum {NR,NS,AT,GR} fmtiles;
#endif

typedef struct {
    int row;
    int col;
} fmindex; /* 2D image index */

typedef struct { /* see fmcoord.c for details */
    double northings;
    double eastings;
} fmucspos; /* User Coordinate System, origo at North Pole */

typedef struct {
    double lat;
    double lon;
} fmgeopos; /* Geographical latitude, longitude */

typedef struct { /* see fmcoord.c for definitions */
    double Ax; /* horizontal pixel size in km */
    double Ay; /* vertical pixel size in km */
    double Bx; /* eastings of upper left corner of image in km */ 
    double By; /* northings of upper left corner of image in km */ 
    int iw; /* image size in horizontal */
    int ih; /* image size in vertical */
} fmucsref; 

typedef struct {
    float truelat; /* True latitude for polar stereographic projection */
    float gridrot; /* grid rotation compare to Greenwich meridian */
    float jnp; /* position of the North Pole, x-coordinate */
    float inp; /* position of the North Pole, y-coordinate */
    float ngridp; /* number of grid points between Equator and NP */
    float dummy; /* only used for rotated grids, not handled herein yet */
} fmdianapos;

#ifdef FMUTIL_HAVE_LIBPROJ
static const char *meosproj[] = {
    "proj=stere",
    "lat_0=90.0",
    "lon_0=0.0",
    "lat_ts=60.0",
    "units=km",
    "a=6378144.0",
    "b=6356759.0"
};

static const char *miproj[] = {
    "proj=stere",
    "lat_0=90.0",
    "lon_0=0.0",
    "lat_ts=60.0",
    "units=km",
    "a=6371000.0",
    "b=6371000.0"
};

fmucspos fmgeo2ucs(fmgeopos ll, fmprojspec myproj); 
fmgeopos fmucs2geo(fmucspos xy, fmprojspec myproj); 
fmucspos fmucsmeos2metno(fmucspos old); 
fmindex  fmucs2ind(fmucsref ref, fmucspos pos);
fmucspos fmind2ucs(fmucsref ref, fmindex ind);
fmgeopos fmind2geo(fmucsref ref, fmprojspec myproj, fmindex ind); 
int fmgeo2tile(fmgeopos ll);
int fmucs2diana(fmucsref ucs, fmdianapos *nwp);
#endif

#endif /* FMCOORD_H */

/*
 * NAME:
 *
 * PURPOSE:
 *
 * REQUIREMENTS:
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
 * �ystein God�y, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * �ystein God�y, METNO/FOU, 29.09.2010: fmstring2fmtime made public
 * available.
 * �ystein God�y, METNO/FOU, 30.09.2010: added fmsec19702isodate.
 * �ystein God�y, METNO/FOU, 2011-11-01: Added fmmonthnum.
 *
 * ID:
 * $Id$
 */


#ifndef FMTIME_H
#define FMTIME_H

#define FMUTIL_ISODATETIME_LENGTH 20 /* To hold 'yyyy-mm-ddThh:mn:ssZ'. Strings should be +1 for '\0' */
#define FMUTIL_CFEPOCH_LENGTH     19 /* To hold 'yyyy-mm-dd hh:mn:ss'.  Strings should be +1 for '\0' */

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

int is_leap_year(int year);
int check_fmtime(fmtime *t, int verbose);
int tofmtime(fmsec1970 secs, fmtime *fmt);
fmsec1970 tofmsec1970(fmtime t);
fmsec1970 ymdhms2fmsec1970(char *str,int mode);
fmsec1970 ymdh2fmsec1970(char *str,int mode);
fmsec1970 isodatetime2fmsec1970(char *str);
void fmsec19702isodatetime(fmsec1970 secs, char *str);
void fmsec19702isodate(fmsec1970 secs, char *str); 
fmsec1970 fmutc2tst(fmsec1970 fmutc, double lon);
fmsec1970 fmcet2tst(fmsec1970 cet, double lon);
fmsec1970 fmloncorr(double meridian, double lon); 
void mjd19502fmtime(double mjd, fmtime *cal);
void fmtime2mjd1950(fmtime cal, double *mjd);
double fmhourangle(double tst);
int fmmeosdate2fmtime(char s[], fmtime *d);

fmsec1970 CFepoch2fmsec1970(char *str);
void      fmsec19702CFepoch(fmsec1970 secs, char *str);

int fmstring2fmtime(char date[], char fmt[], fmtime *res);

fmsec1970 ymdhms2fmsec1970_alt(char *str,int mode);
fmsec1970 ymdh2fmsec1970_alt(char *str,int mode);
fmsec1970 isodatetime2fmsec1970_alt(char *str);

int correct_time_from_unit(double amount_from_epoch, char *unit_string,fmsec1970 *secsFrom1970);

int fmdayofyear(fmtime md); 
int fmdoy2mmdd(int year, int doy, int *m2, int *d2);

int fmmonthnum(char *monthname); 
#endif
/*
 * NAME:
 *
 * PURPOSE:
 *
 * REQUIREMENTS:
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
 * �ystein God�y, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */


#ifndef FMREFLECTIVITY_H
#define FMREFLECTIVITY_H


double fmesd(int doy); 
double fmdeclination(int doy);
fmsec1970 fmequationoftime(int doy);
double fmsolarzenith(fmsec1970 tst, fmgeopos gpos); 
float fmbidirref(int jd, float soz, float alb); 
double fmtoairrad(fmsec1970 tst, fmgeopos gpos, double s0);

#endif
/*
 * NAME:
 * fmangles.h
 *
 * PURPOSE:
 *
 * REQUIREMENTS:
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
 * �ystein God�y, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */

#ifndef FMANGLES_H
#define FMANGLES_H


typedef struct {
    fmgeopos *gpos;
    int npoints;
} fmsubtrack;

typedef struct {
    fmucsref ref;
    fmgeopos ll;
    fmindex ind;
    fmtime date;
    fmsubtrack subtrack;
    fmprojspec myproj;
} fmangreq;

typedef struct {
    fmgeopos st1;
    fmgeopos st2;
    fmgeopos tp;
} fmtriangpoints;

typedef struct {
    double soz; /* solar zenith angle */
    double saz; /* satellite zenith angle */
    double raz; /* relative azimuth angle */
    double aza; /* satellite azimuth angle */
    double azu; /* solar azimuth angle */
} fmangles;

fmbool fmangest(fmangreq rs, fmangles *ang);
fmbool fmsubtrc(fmangreq rs, fmgeopos *p1, fmgeopos *p2);
fmbool fmsubtrn(fmtriangpoints rs, fmgeopos *p);

#endif /* FMANGLES_H */
#ifndef ANGLECONVERSION_H
#define ANGLECONVERSION_H

double fmrad2deg(double radians); 
double fmdeg2rad(double degrees); 

#endif /* ANGLECONVERSION_H */
/*
 * NAME:
 * NA
 *
 * PURPOSE:
 * NA
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
 * �ystein God�y, METNO/FOU, 26.04.2007 
 *
 * MODIFIED:
 * �ystein God�y, METNO/FOU, 28.02.2008: Added string lengths.
 * �ystein God�y, METNO/FOU, 2011-12-19: Added grep-like functionality.
 *
 * ID:
 * $Id$
 */


#ifndef FMSTRINGS_H
#define FMSTRINGS_H

#define FMSTRING16 16
#define FMSTRING32 32
#define FMSTRING64 64
#define FMSTRING128 128
#define FMSTRING256 256
#define FMSTRING512 512
#define FMSTRING1024 1024

void fmremovenewline(char *mystr); 
void fmstrsplit(char *origString, const char *sep, unsigned int *nbTokens,char ***tokens);
char *fmstrjoin(size_t nbStrings, char **strings, char sep);
void fmbasename(char *dirfilename, char **end_dirname, char **start_basename);

int convertString2Float(char *string,float *val);
int convertString2Long(char *string,long *val);

fmbool fmgrep(char *needle, const char *haystack);
fmbool fmgrepstart(char *needle, const char *haystack);
fmbool fmgrepend(char *needle, const char *haystack);
int fmstrlen(char *mystring);

#endif
/*
 * NAME:
 * fmstorage
 *
 * PURPOSE:
 * Useful functions handling storage containers.
 *
 * REQUIREMENTS:
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
 * �ystein God�y, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * �ystein God�y, METNO/FOU, 02.12.2010: Added fmalloc_uchar_vector
 * �ystein God�y, METNO/FOU, 2011-12-14: Added fmfree_uchar_vector
 * �ystein God�y, METNO/FOU, 2011-12-16: Added fmalloc_ubyte_2d and
 * fmfree_ubyte_2d
 *
 * ID:
 * $Id$
 */


#ifndef FMSTORAGE_H
#define FMSTORAGE_H


long fmivec(long x, long y, unsigned long nx);
void fmijmap(long elem, unsigned long nx, long *x, long *y);

void *fmMalloc(size_t length);

int fmalloc_char_vector(char **value, int length); 
int fmalloc_uchar_vector(unsigned char **value, int length); 
int fmalloc_ushort_vector(unsigned short **value, int length); 
int fmalloc_int_vector(int **value, int length); 
int fmalloc_float_vector(float **value, int length); 
int fmalloc_double_vector(double **value, int length); 

int fmalloc_byte_2d(char ***value, int rows, int columns); 
int fmalloc_ubyte_2d(unsigned char ***value, int rows, int columns); 
int fmalloc_byte_2d_contiguous(char ***value, int rows, int columns); 
int fmalloc_ushort_2d(unsigned short ***value, int rows, int columns); 
int fmalloc_int_2d(int ***value, int rows, int columns); 
int fmalloc_float_2d(float ***value, int rows, int columns); 
int fmalloc_double_2d(double ***value, int rows, int columns); 

int fmfree_char_vector(char *value); 
int fmfree_uchar_vector(unsigned char *value); 
int fmfree_ushort_vector(unsigned short *value); 
int fmfree_int_vector(int *value); 
int fmfree_float_vector(float *value); 
int fmfree_double_vector(double *value); 

int fmfree_byte_2d(char **value, int rows); 
int fmfree_ubyte_2d(unsigned char **value, int rows); 
int fmfree_ushort_2d(unsigned short **value, int rows); 
int fmfree_int_2d(int **value, int rows); 
int fmfree_float_2d(float **value, int rows); 
int fmfree_double_2d(double **value, int rows); 

#endif
/*
 * NAME:
 * fmimage.h
 *
 * PURPOSE:
 * Header file to support the data model/structure used for "image" data.
 *
 * REQUIREMENTS:
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
 * �ystein God�y, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * �ystein God�y, METNO/FOU, 2011-11-08: Adapted container to an
 * evaluation of the osisaf HDF5 container and extended this along the
 * lines of HLHDF CMSAF products.
 * �ystein God�y, METNO/FOU, 2012-12-06: Added controlled vocabularies,
 * more should be added (e.g. for constraints, area descriptions, etc).
 * �ystein God�y, METNO/FOU, 2013-04-11: Added static to vocabularies...
 *
 * ID:
 * $Id$ 
 */


#ifndef FMIMAGE_H
#define FMIMAGE_H

#define FMIMAGESTR25 25
#define FMIMAGESTR50 50
#define FMIMAGESTR128 128
#define FMIMAGESTR256 256
#define FMIMAGESTR512 512
#define FMIMAGESTR1024 1024

typedef struct {
    float gain;
    float intercept;
    char description[FMIMAGESTR50];
} fmslope;

/* Still missing something on breakpoints for multiple slopes */
typedef struct {
    fmslope *slope;
    int nslopes;
} fmscale;

/*
 * Data type specification used to identify which part of the datafield to
 * fill and use.
 */
typedef enum fmdatatype {
    FMUCHAR=0,
    FMCHAR,
    FMUSHORT,
    FMSHORT,
    FMUINT,
    FMINT,
    FMFLOAT,
    FMDOUBLE
} fmdatatype;

/*
 * Platform names - controlled vocabulary
 */
#ifndef PLATFORMVOC
#define PLATFORMVOC
#define NPL 2
static struct {
    int nelems;
    char names[NPL][FMSTRING32]; 
} platform_vocab = {
    NPL,
    {
        "Polar Orbiting Satellite",
        "Geostationary Satellite",
    },
};
#endif

/*
 * Satellite names - controlled vocabulary
 */
#ifndef SATPLATVOC
#define SATPLATVOC
#define NSN 12
static struct {
    int nelems;
    char names[NSN][FMSTRING32];
} satellite_vocab = {
    NSN,
    {
        "NOAA-09",
        "NOAA-10",
        "NOAA-11",
        "NOAA-12",
        "NOAA-14",
        "NOAA-15",
        "NOAA-16",
        "NOAA-17",
        "NOAA-18",
        "NOAA-19",
        "MetOp1",
        "MetOp2",
    },
};
#endif

/*
 * Sensor names - controlled vocabulary
 */

/*
 * Use constraints - controlled vocabulary
 */

/*
 * Distribution constraints - controlled vocabulary
 */


/*
 * Header structure for "image" data
 */
typedef struct {
    char platform_name[FMIMAGESTR25];  /* platform name, controlled vocabulary */
    char satellite_name[FMIMAGESTR25];  /* satellite name, controlled vocabulary */
    char sensor_name[FMIMAGESTR25];  /* sensor name, controlled vocabulary */
    char area_description[FMIMAGESTR25];  /* area name, controlled vocabulary */
    char processing_description[FMIMAGESTR50];  /* process name etc */
    char product_description[FMIMAGESTR1024]; /* free text description */
    char distribution_constraints[FMSTRING128]; /* controlled vocabulary */
    char use_constraints[FMSTRING128]; /* controlled vocabulary */
    int orbit_no;
    fmbool map_projected;
    fmbool ucs_positioned;
    fmbool subtrack_added;
    int xsize; /* number of pixels */
    int ysize; /* number of pixels */
    float nominal_grid_resolution_x; /* If map_projected */
    float nominal_grid_resolution_y; /* If map_projected */
    fmtime valid_time; /* Reception or processing time */
    fmtime timespan[2]; /* Start and end time of data */
    fmucsref ucs; /* User Coordinate System, ucs_positioned */
    fmsubtrack subtrack; /* subsatellite track in latlon, if subtrack_added */
    int layers; /* no. of layers */
    char **layer_id; /* layer names, not in use yet!! */
    char projstring[FMIMAGESTR1024]; /* PROJ.4 specification used */
} fmheader;

/*
 * Data structure for "image" data
 * Use storage functions described in fmstorage.h to initialise etc these
 * datafields. Only one storage field is used, as indicated by dtype. 
 */
typedef struct {
    unsigned char **bytearray; /* image data */
    unsigned short **ushortarray; /* image data */
    int **intarray; /* image data */
    float **floatarray; /* image data */
    double **doublearray; /* image data */
    char *description;
    char *unit; /* spell it out in SI */
    fmdatatype dtype;
    int missingdatavalue; /* Must be interpreted according to data field */
    int nodatavalue; /* Must be interpreted according to data field */
    fmbool packed; /* real values packed as int or similar? */
    fmscale scalefactor; /* intercept and slope for packed information */
    fmbool palette; /* true if palette / classed image */
    int number_of_classes;
    char **class_names;
} fmdatafield;

/*
 * Header and data
 */
typedef struct {
    fmheader h;
    fmdatafield *d;
} fmdataset;

int init_fmdataset(fmdataset *d); 
int free_fmdataset(fmdataset *d); 
int allocate_fmdatafield(fmdatafield **d, int layers);
float unpack_fmdatafield_byte(char val, fmscale scale, int use); 
float unpack_fmdatafield_ushort(unsigned short val, fmscale scale, int use); 
float unpack_fmdatafield_short(short val, fmscale scale, int use); 
float unpack_fmdatafield_uint(unsigned int val, fmscale scale, int use); 
float unpack_fmdatafield_int(int val, fmscale scale, int use); 

#endif
/*
 * NAME:
 *
 * PURPOSE:
 *
 * REQUIREMENTS:
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
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */

#ifndef FMBYTESWAP_H
#define FMBYTESWAP_H


void fmshortbyteswap(short int *data, int ndata);
void fmintbyteswap(int *data, int ndata);
void fmbs_int(int *data);
void fmbs_short(short int *data);

#endif
/*
 * NAME:
 *
 * PURPOSE:
 *
 * REQUIREMENTS:
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
 * �ystein God�y, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */

#ifndef FMCOLORMAP_H
#define FMCOLORMAP_H


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

#endif
/*
 * NAME:
 * NA
 *
 * PURPOSE:
 * NA
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
 * NA
 *
 * MODIFIED:
 * Thomas Lavergne, met.no, 03.12.2007: add the optional use of
 * printf-like format and parameters to fmlogmsg() and fmerrmsg()
 *
 * ID:
 * $Id$
 */ 

#ifndef FMERRMSG_H
#define FMERRMSG_H

#ifdef FMUTIL_HAVE_STDARG_H
extern void fmlogmsg(char *where, char *what,...); 
extern void fmerrmsg(char *where, char *what,...); 
#else
extern void fmlogmsg(char *where, char *what); 
extern void fmerrmsg(char *where, char *what); 
#endif


#endif /* FMERRMSG */
/*
 * NAME:
 * fm_ch3b_reflectance.h
 *
 * PURPOSE:
 * Header file for fm_ch3b_reflectance.c
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
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Mari Anne Killie, METNO/FOU, 18.06.2007 
 *
 * MODIFIED:
 * �ystein God�y, METNO/FOU, 18.06.2007: Name space adjustment for use in
 * libfmutil.
 * Mari Anne Killie, METNO/FOU, 05.10.2007: Added the fm_ch3b_const
 * structure member 'solrad' and removed FM_SOLCONSTCH3B.
 * Mari Anne Killie, METNO/FOU, 22.06.2011: bugfix, satID expanded to
 * make room for \0
 * Mari Anne Killie, MET/FOU, 06.08.2013: adding planck coefficients
 * for pre AVHRR/3-sensors (FM_PLANCKCX_v0).
 *
 * ID:
 * $Id$
 */

#ifndef CH3B_H
#define CH3B_H


#define FM_PLANCKC1 1.1910427e-5 /* unit: mW/(m^2 sr cm^-4) */
#define FM_PLANCKC2 1.4387752    /* unit: cm K */
/*added when adding pre-AVHRR/3 sensors*/
#define FM_PLANCKC1_v0 1.1910659e-5 /* unit: mW/(m^2 sr cm^-4) */
#define FM_PLANCKC2_v0 1.438833     /* unit: cm K */

typedef struct { /*Channel 3b constants, uniqe for each spacecraft */
  int avhrr3;    /*1 if AVHRR/3-sensor, 0 if earlier AVHRR sensor*/
  float Aval;    /*const A, needed to calculate "effective bb temperature*/
  float Bval;    /*const B, same as above */
  float cwn;     /*central wave number in cm^-1*/
  char satID[10]; /*satellite identification*/
  float solrad;  /*solar radiance at TOA, units: mW/(m^2 sr cm^-1)*/
} fm_ch3b_const;

float fm_ch3brefl(float bt3b, float bt4, float solang, char *satname, int doy);
float fm_temp2rad(float bt, char *satname);
float fm_rad2temp(float radiance, char *satname);
float fm_findsollum(float solang, char *satname, int doy);
fm_ch3b_const fm_ch3b_identsat(char *satname);


#endif /* CH3B_H */
/*
 * NAME:
 * NA
 *
 * PURPOSE:
 * NA
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
 * �ystein God�y, METNO/FOU, 31.08.2007 
 *
 * MODIFIED:
 * Steinar Eastwood, met.no, 05.09.2007  Added include time.h.
 * �ystein God�y, METNO/FOU, 06.09.2007: Moved some include statements
 * from C-file to headerfile. Whether this should be the general practise
 * is still to be determined.
 * �ystein God�y, METNO/FOU, 05.10.2007: Added fmfindfile.
 *
 * ID:
 * $Id$
 */

#ifndef FMTOUCH_H
#define FMTOUCH_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <utime.h>
#include <sys/stat.h>
#include <time.h>


char *fmfindfile(char *path, char *datestr, char *timestr, char *areastr);
int fmfileexist(char *filename); 
int fmtouch(char *filename, time_t newtime); 

#endif /* FMTOUCH */
/*
 * NAME:
 * NA
 *
 * PURPOSE:
 * NA
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
 * �ystein God�y, METNO/FOU, 16.10.2007
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>

int fmfeltfile_gettime(char *filename, fmtime *mytime); 
/*
 * NAME:
 * NA
 *
 * PURPOSE:
 * NA
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
 * �ystein God�y, METNO/FOU, 16.10.2007 
 *
 * MODIFIED:
 * �ystein God�y, METNO/FOU, 20.10.2009: Added fmreaddir, fmfilelist_sort
 * �ystein God�y, METNO/FOU, 21.10.2009: Added fmstarcdirs
 *
 * ID:
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

typedef struct {
    int nfiles;
    char *path;
    char **filename;
} fmfilelist;

typedef struct {
    int nfiles;
    char **dirname;
} fmstarclist;

int fmfilelist_alloc(fmfilelist *mylist,int nfiles); 
int fmfilelist_sort(fmfilelist *flist); 
int fmfilelist_free(fmfilelist *mylist);
int fmlinkfiles(fmfilelist src, fmfilelist dst); 
int fmremovefiles(fmfilelist mylist); 
int fmreaddir(char *idir, fmfilelist *flist);
int fmstarcdirs(fmtime start, fmtime end, fmstarclist *dirlist);
#ifdef __cplusplus
}
#endif
#endif /* FMUTIL_H */

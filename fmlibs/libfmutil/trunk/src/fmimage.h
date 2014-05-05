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
 * Øystein Godøy, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 2011-11-08: Adapted container to an
 * evaluation of the osisaf HDF5 container and extended this along the
 * lines of HLHDF CMSAF products.
 * Øystein Godøy, METNO/FOU, 2012-12-06: Added controlled vocabularies,
 * more should be added (e.g. for constraints, area descriptions, etc).
 * Øystein Godøy, METNO/FOU, 2013-04-11: Added static to vocabularies...
 *
 * ID:
 * $Id$ 
 */

#include "fmstrings.h"

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

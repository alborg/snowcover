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
 * Øystein Godøy, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 */

#include <fmutil.h>

#ifndef FMIMAGE_H
#define FMIMAGE_H

typedef struct {
    float gain;
    float intercept;
    char description[50];
} fmslope;

typedef struct {
    fmslope *slope;
    int nslopes;
} fmscale;

typedef struct {
    char satellite_name[25];  /* satellite name */
    char area_description[25];  /* area name */
    char processing_source[50];  /* process name */
    fmtime valid_time; /* Reception or processing time */
    fmucsref ucs; /* User Coordinate System */
    fmsubtrack subtrack; /* subsatellite track in latlon */
    fmscale scale_ushort; /* scale information */
    fmscale scale_int; /* scale information */
    int channels; /* no. of channels */
    int fieldsize; /* size of each field */
    char **channel_id; /* channel names */
    char *projcall; /* PROJ.4 specification used */
    char **bytearray; /* image data */
    unsigned short **ushortarray; /* image data */
    int **intarray; /* image data */
    float **floatarray; /* image data */
    double **doublearray; /* image data */
} fmimage;

int init_fmimage(fmimage *d); 
int free_fmimage(fmimage *d); 
float unpack_fmimage_ushort(unsigned short val, fmscale scale, int use); 
float unpack_fmimage_int(int val, fmscale scale, int use); 

#endif

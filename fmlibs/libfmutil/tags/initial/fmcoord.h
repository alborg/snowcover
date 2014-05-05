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
 * Øystein Godøy, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 */

#ifndef FMCOORD_H
#define FMCOORD_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <projects.h>

#include <fmutil.h>

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

static char *meosproj[] = {
    "proj=stere",
    "lat_0=90.0",
    "lon_0=0.0",
    "lat_ts=60.0",
    "units=km",
    "a=6378144.0",
    "b=6356759.0"
};

static char *miproj[] = {
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
fmindex fmucs2ind(fmucsref ref, fmucspos pos);
fmucspos fmind2ucs(fmucsref ref, fmindex ind);
fmgeopos fmind2geo(fmucsref ref, fmprojspec myproj, fmindex ind); 
int fmgeo2tile(fmgeopos ll);
int fmucs2diana(fmucsref ucs, fmdianapos *nwp);

#endif /* FMCOORD_H */

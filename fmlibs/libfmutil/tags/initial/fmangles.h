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
 * Øystein Godøy, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 */

#ifndef FMANGLES_H
#define FMANGLES_H

#include <fmutil.h>

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

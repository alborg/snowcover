/*
 * NAME:
 * deg2rad
 * rad2deg
 *
 * PURPOSE:
 * To convert between degrees and radians.
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 25.02.2004
 */

#include <fmutil.h>

double fmrad2deg(double radians) {

    double degrees;

    degrees = radians*(180./fmPI);

    return(degrees);
}

double fmdeg2rad(double degrees) {

    double radians;

    radians = degrees*(fmPI/180.);

    return(radians);
}

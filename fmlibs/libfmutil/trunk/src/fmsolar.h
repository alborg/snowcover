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

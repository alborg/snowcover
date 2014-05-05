/*
 * NAME:
 * fmselalg
 *
 * PURPOSE:
 * This file contains functions that are used for selecting the correct
 * algoritm according to the available daylight in a satellite scene.
 * The algoritm uses only corner values to chose from.
 *
 * hmax and hmin are input variables to the function determining the
 * maximum and minimum sunheights. A twilight scene is defined as being in
 * between these limits. During daytime scenes all corners of the image
 * have sunheights larger than hmax, during nighttime all corners have
 * sunheights below hmin (usually negative values).
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 *  o fmtime structure with UTC time of image
 *  o ucs reference for the image
 *  o hmax - maximum solar height (reciprocal of solar zenith angle)
 *  o hmin - minimum solar height (reciprocal of solar zenith angle)
 *
 * OUTPUT:
 *   Return values:
 *   -10: Twilight
 *   1: Nighttime
 *   2: daytime
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Oystein Godoy, DNMI/FOU, 23/07/1998
 *
 * MODIFIED:
 * Oystein Godoy, DNMI/FOU, 06/10/1998: Selection conditions changed.
 * Error in nighttime test removed.
 * Oystein Godoy, DNMI/FOU, 19.01.2004: Return value for twilight is
 * changed from 0 to negative value.
 * Øystein Godøy, METNO/FOU, 22.02.2008: Moved from libsatimg to
 * libfmutil.
 *
 * ID:
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fmutil.h>

short fmselalg(fmtime d, fmucsref upos, float hmax, float hmin) {

  int i, countx, county;
  float inclination, hourangle, coszenith, sunh, xval, yval;
  float max = 0., min = 0.;
  float northings,  eastings,  latitude,  longitude;
  float DistPolEkv, daynr, gmttime;
  float Pi = 3.141592654;
  float TrueScaleLat = 60.;
  float CentralMer = 0.;
  float theta0, lat;
  double radian, Rp, TrueLatRad;
  double sunz;
  fmgeopos gpos;
  fmucspos ucs;
  fmsec1970 tst, utc;
  
  /*
   * Decode day and time information for use in formulas.
   */
  daynr = fmdayofyear(d);
  utc = tofmsec1970(d);
    
  /*
   Estimates latitude and longitude for the corner pixels.
   */
  countx = 0;
  county = 0;
  for (i = 0; i < 4; i++) {
    xval = upos.Bx + upos.Ax*((float) countx + 0.5);
    yval = upos.By - fabsf(upos.Ay)*((float) county + 0.5);
 
    countx += upos.iw;
    if (countx > upos.iw) {
      countx = 0;
      county += upos.ih;
    }
    ucs.northings = yval;
    ucs.eastings  = xval;

    gpos = fmucs2geo(ucs, (fmprojspec) miproj);
  
    /*
     * Estimates zenith angle in the pixel.
     */ 
    tst = fmutc2tst(utc, gpos.lon);
    sunz = fmsolarzenith(tst, gpos);

    sunh = 90.-fmrad2deg(sunz);
    
    if (sunh < min) {
	min = sunh;
    } else if ( sunh > max) {
	max = sunh;
    }

  }

  /*
   * Choose algorithm
   */
  if (max > hmax && fabs(max) > (fabs(min)+hmax)) {
    return(2);
  } else if (min < hmin && fabs(min) > (fabs(max)+hmin)) {
    return(1);
  } 
  return(-10);
}



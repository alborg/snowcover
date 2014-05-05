/*
 * NAME:
 * selalg
 * 
 * PURPOSE:
 * This file contains functions that are used for selecting the correct
 * algoritm according to the available daylight in a satellite scene.
 * The algoritm uses only corner values to chose from.
 * 
 * AUTHOR: 
 * Oystein Godoy, DNMI/FOU, 23/07/1998
 * MODIFIED:
 * Oystein Godoy, DNMI/FOU, 06/10/1998
 * Selection conditions changed. Error in nighttime test removed.
 * Oystein Godoy, DNMI/FOU, 19.01.2004
 * Return value for twilight is changed from 0 to negative value.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <satimg.h>

short selalg(struct dto d, struct ucs upos, float hmax, float hmin) {

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
  
  radian = Pi/180.;
  TrueLatRad = TrueScaleLat*radian;
  DistPolEkv = 6378.*(1.+sin(TrueLatRad));  
  
  /*
   * Decode day and time information for use in formulas.
   */
  daynr = (float) JulianDay((int) d.yy, (int) d.mm, (int) d.dd); 
  gmttime = (float) d.ho+((float) d.mi/60.);
  
  theta0 = (2*Pi*daynr)/365;
  inclination = 0.006918-(0.399912*cosf(theta0))+(0.070257*sinf(theta0))
    -(0.006758*cosf(2*theta0))+(0.000907*sinf(2*theta0))
    -(0.002697*cosf(3*theta0))+(0.001480*sinf(3*theta0));
    
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
    northings = yval;
    eastings  = xval;

    Rp = pow((eastings*eastings + northings*northings),0.5);

    latitude = 90.-(1./radian)*atan(Rp/DistPolEkv)*2.;
    longitude = CentralMer+(1./radian)*atan2(eastings,-northings);

    latitude=latitude*radian;
    longitude=longitude*radian;
  
    /*
     * Estimates zenith angle in the pixel.
     */ 
    lat = gmttime+((longitude/radian)*0.0667);
    hourangle = fabsf(lat-12.)*0.2618;    
    
    coszenith = (cos(latitude)*cos(hourangle)*cos(inclination)) +
                   (sin(latitude)*sin(inclination));
    sunh = 90.-(acosf(coszenith)/radian);
    
    if (sunh < min) {
	min = sunh;
    } else if ( sunh > max) {
	max = sunh;
    }

  }

  /*
   hmax and hmin are input variables to the function determining
   the maximum and minimum sunheights. A twilight scene is defined
   as being in between these limits. During daytime scenes all corners
   of the image have sunheights larger than hmax, during nighttime
   all corners have sunheights below hmin (usually negative values).

   Return values, 
   -10= no algorithm chosen (twilight) 
   1= nighttime algorithm
   2= daytime algorithm.
   */
  if (max > hmax && fabs(max) > (fabs(min)+hmax)) {
    return(2);
  } else if (min < hmin && fabs(min) > (fabs(max)+hmin)) {
    return(1);
  } 
  return(-10);
}


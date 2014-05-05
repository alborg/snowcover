/*
 * FUNCTION: 
 * normalize
 * 
 * PURPOSE:
 * This function estimates the solar zenith angle and compensates the
 * visual channels for the zenith angle.
 * According to Liou (1980), the solar zenith angle can be computed
 * from the angles associated with the latitude, the declination
 * of the sun and the hour angle of the sun. Formulas for computing
 * the zenith angle are given in Liou (1980) and Paltridge and Platt
 * (1976).
 * Latitudes and longitudes of pixels are calculated according to the
 * equations given in MISAT program xylb.c by Jens Sunde, DNMI.
 * All estimates of angles are made in radians and distances are given
 * in kilometers.
 * 
 * AUTHOR: 
 * Oystein Godoy, DNMI/FoU, 31.07.1995.
 * MODIFIED:
 * Øystein Godøy, met.no, 23.02.2004
 * Some cleaning of unused variables.
 */

#include <satimg.h>

void normalize(struct imgh *img) {

  int i, ch, countx, county, overcompensated[2];
  int size;
  float inclination, hourangle, coszenith, xval, yval;
  float northings,  eastings,  latitude,  longitude;
  /*float RadPerDay =  0.01721420632;*/
  float DistPolEkv, daynr, gmttime;
  float Pi = 3.141592654;
  float TrueScaleLat = 60.;
  float CentralMer = 0.;
  float tempvar;
  float theta0, lat;
  double radian, Rp, TrueLatRad;
  
  radian = Pi/180.;
  TrueLatRad = TrueScaleLat*radian;
  DistPolEkv = 6378.*(1.+sin(TrueLatRad));  
  size = img->iw*img->ih; 
  
  /*
  printf("\t\tiw: %d ih: %d size: %d\n", img->iw, img->ih, size);
  printf("\t\tAx: %f Ay: %f Bx: %f By: %f\n", 
    img->Ax, img->Ay, img->Bx, img->By);
  */

  /*
   * Decode day and time img->mation for use in formulas.
   */
  daynr = (float) JulianDay((int) img->yy, (int) img->mm, (int) img->dd); 
  gmttime = (float) img->ho+((float) img->mi/60.);
  
  /*
  printf("\t\tdaynr= %f, gmttime= %f\n", daynr, gmttime);
  */

  theta0 = (2*Pi*daynr)/365;
  inclination = 0.006918-(0.399912*cos(theta0))+(0.070257*sin(theta0))
    -(0.006758*cos(2*theta0))+(0.000907*sin(2*theta0))
    -(0.002697*cos(3*theta0))+(0.001480*sin(3*theta0));
    
  /*
  printf("\t\t%s= %f\n", "inclination", inclination);  
  */

  countx = 0;
  county = 0;
  for (i = 0; i < 2; i++) {
      overcompensated[i] = 0;
  }

  for (i = 0; i < size; i++) {
    /*
     * Estimates latitude and longitude for the pixel.
     */
     xval = img->Bx + img->Ax*((float) countx + 0.5);
     yval = img->By - fabsf(img->Ay)*((float) county + 0.5);
 
     countx = countx + 1;
     if (countx == img->iw) {
       countx=0;
       county=county + 1;
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

     /*
     if ((county == 0 && (countx-1) == 0) ||
	(county == 0 && countx == img->iw-1) ||
	(county == img->ih-1 && (countx-1) == 0) ||
	(county == img->ih-1 && countx == img->iw-1)) {
	    printf("\n\t\teastings: %f northings: %f\n", eastings, northings);
	    printf("\t\tlatitude: %f longitude: %f\n", 
		latitude/radian, longitude/radian);
	    printf("\t\tgmttime: %f lat: %f\n", gmttime, lat);
	    printf("\t\tzenith: %f\n", facos(coszenith)/radian);
	}
    */

    /*
     * Normalizes the zenith angle by dividing by cosinus of the zenith
     * angle. This increases the intensity of all pixels by a different
     * factor for different areas instead of reducing the intensity as
     * would be the case if the observed counts were multiplied with the
     * cosine zenith angle. Overcompensated pixels are truncated at 255
     * counts.
     */
     for (ch = 0; ch < 2; ch++) {
	 tempvar=(float)(img->image[ch][i])/coszenith;
	 if (tempvar > 65535.) {
	     overcompensated[ch]=overcompensated[ch]+1;
	     tempvar = 65535.;
	 }
	 img->image[ch][i] = (unsigned short) tempvar;
     }    
  }
  printf("\n");
  for (i = 0; i < 2; i++) {
      printf("\t\t%d pixels of totally %d are truncated\n", 
	overcompensated[i], size);
  }
}


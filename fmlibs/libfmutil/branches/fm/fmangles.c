/*
 * FUNCTION:
 * azimuth
 * 
 * PURPOSE:
 * Estimates the satellite and solar azimuth angles for a given 
 * position in latitude and longitude. Both the individual
 * azimuth angles and the relative are estimated.
 * 
 * NOTES:
 * 
 * RETURN VALUES:
 * Boolean
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 11.04.2003
 */

#include <fmutil.h>

fmbool fmangest(fmangreq rs, fmangles *ang) {

    char *where="fmangest";
    double gmttime, loctime, lat, lon, et;
    double daynr, c;
    double incl, hangle, coszen, theta0, sunaz;
    double sataz, sinsunaz, sinsataz, relaz;
    double phia, phiv, alpha, h, satd = 850000.0, R=6371200.0;
    fmgeopos stpos, tll1, tll2;
    fmtriangpoints lls;
    
    /* 
     * Estimate daynumber from date information. 
     */
    daynr = (double) fmdayofyear(rs.date);
    gmttime = ((double) rs.date.fm_hour)+((double) rs.date.fm_min)/60.;
    
    /* 
     * Estimate inclination. 
     */
    theta0 = (2*PI*(daynr-1.))/365.;
    incl = 0.006918-(0.399912*cos(theta0))+(0.070257*sin(theta0))
	   -(0.006758*cos(2*theta0))+(0.000907*sin(2*theta0))
	   -(0.002697*cos(3*theta0))+(0.001480*sin(3*theta0));
    
    /* 
     * Estimate Local Area Time (loctime), Hourangle (hangle) and 
     * cosinus to the solar zenith angle (coszen).
     */
    lat = rs.ll.lat*DEG_TO_RAD;
    lon = rs.ll.lon*DEG_TO_RAD;
    et = 0.170*sin(4*PI*(daynr-80.)/373.)
	-0.129*sin(2*PI*(daynr-8.)/355.);
    loctime = gmttime+((lon*RAD_TO_DEG)*0.0667)+et;
    hangle = (loctime-12.)*0.2618;
    coszen = (cos(lat)*cos(hangle)*cos(incl))+
	     (sin(lat)*sin(incl));
    ang->soz = acos(coszen)*RAD_TO_DEG;
    
    /* 
     * Estimate solar azimuth angle, test for quadrant of 
     * the observation.
     */
    sinsunaz = (sin((PI/2)-incl)*sin(fabs(hangle)))/sin(ang->soz*DEG_TO_RAD);
    if (sinsunaz <= 1.000 && sinsunaz >= -1.000) {
	sunaz = asin(sinsunaz);
    } else if (sinsunaz > 1.000) {
	sunaz = PI/2;
    } else if (sinsunaz < -1.000) {
	sunaz = -PI/2;
    }

    if (incl < lat && hangle < 0) {
	sunaz = PI - sunaz; /* 2Q */
    } else if (incl < lat && hangle > 0) {
	sunaz = PI + sunaz; /* 3Q */
    } else if (incl > lat && hangle > 0) {
	sunaz = 2*PI - sunaz; /* 4Q */
    }
    ang->azu = sunaz*RAD_TO_DEG;
    
    /* 
     * Estimate satellite zenith angle 
     * First find the 2 closest points in the subsatellite track
     * These are returned as index values within the reference image
     */
    if (!fmsubtrc(rs, &tll1, &tll2)) {
	fmerrmsg(where,"Failure at indicies:");
	fprintf(stderr," %d %d\n",rs.ind.col,rs.ind.row);
	return(FMFALSE);
    }
    /* 
     * Then find the closest point on the subtrack.
     * KSPT have given the subtrack longitude as 0-360 degrees, while
     * our convention (and PROJ) is -180-180 degrees.
     */
    lls.st1 = tll1;
    lls.st2 = tll2;
    lls.tp = rs.ll;;
    if (!fmsubtrn(lls, &stpos)) {
	return(FMFALSE);
    }

    /*
     * Estimate satellite zenith angle. phia is the satellite scan angle, 
     * phiv is the satellite zenith angle adjusted for an curved Earth.
     */
    alpha = ((sin(stpos.lat*DEG_TO_RAD)*sin(rs.ll.lat*DEG_TO_RAD))+
	(cos(stpos.lat*DEG_TO_RAD)*cos(rs.ll.lat*DEG_TO_RAD)*
	cos(DEG_TO_RAD*fabs(stpos.lon-rs.ll.lon))));
    c = acos(alpha);
    if (alpha > 0.99999999999) {
	phiv = 0.;
    } else {
	h = satd;
	phia = atan((R*sin(c))/(h+R*(1-sin(c)/tan(c))));
	phiv = phia + c;
    }
    ang->saz = phiv*DEG_TO_RAD;

    /* 
     * Estimate satellite azimuth angle using spherical geometry.
     */
    sinsataz = (cos(stpos.lat*DEG_TO_RAD)*
	sin(DEG_TO_RAD*fabs(stpos.lon-rs.ll.lon)))/sin(c);
    if (sinsataz < 1.0 && sinsataz > -1.0) {
	sataz = asin(sinsataz);
    } else if (sinsataz >= 1.000) {
	sataz = PI/2.;
    } else if (sinsataz <= -1.000) {
	sataz = -PI/2.;
    }
    /*printf(" tmp: %.2f", sataz);*/

    /*
     * Check quadrant
     */
    if (rs.ll.lat > stpos.lat && rs.ll.lon < stpos.lon) {
	sataz = PI - sataz; /* 2Q */
	/*printf(" 2Q [%.2f, %.2f] ",stpos.lat,stpos.lon);*/
    } else if (rs.ll.lat > stpos.lat && rs.ll.lon > stpos.lon) {
	sataz = PI + sataz; /* 3Q */
	/*printf(" 3Q [%.2f, %.2f] ",stpos.lat,stpos.lon);*/
    } else if (rs.ll.lat < stpos.lat && rs.ll.lon > stpos.lon) {
	sataz = 2*PI - sataz; /* 4Q */
	/*printf(" 4Q [%.2f, %.2f] ",stpos.lat,stpos.lon);*/
    } /*else {
	printf(" 1Q [%.2f, %.2f] ",stpos.lat,stpos.lon);
    }*/
    ang->aza = sataz*RAD_TO_DEG;

    /*
    printf(" %.2f %.2f,%.2f %.2f,%.2f\n", 
	ang->aza, rs.ll.lat,rs.ll.lon,stpos.lat,stpos.lon);
	*/

    /* 
     * Estimate relative azimuth angle
     */
    if (sataz > sunaz) {
	relaz = sataz - sunaz;
    } else {
	relaz = sunaz - sataz;
    }
    ang->raz = relaz*RAD_TO_DEG;

    return(FMTRUE);
}

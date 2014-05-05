/*
 * NAME:
 * fmsubtrack.c
 *
 * PURPOSE:
 * Find the two closest subtrack points to the target pixel, coordinates
 * are returned as latitude, longitude, contrary to the old routine in
 * libsatimg.
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

fmbool fmsubtrc(fmangreq rs, fmgeopos *p1, fmgeopos *p2) {

    char *where="fmsubtrc";
    int i;
    double d, dx, dy, closest1 = 100000.0, closest2 = 100000.0;
    fmucspos tp, sp;
    fmgeopos tll;
    
    /*
     * Estimate cartesian coordinates of target
     */
    tp = fmind2ucs(rs.ref,rs.ind);

    /*
     * Loop through subtrack, estimate cartesian coordinates for each
     * point and return positns with minimum distance to target.
     * Dummy values selected for equator as we probably never will work in
     * that area...
     */
    p1->lat = p2->lat = -10.;
    p1->lon = p2->lon = 179.;
    for (i=0; i<rs.subtrack.npoints; i++) {
	tll.lat = rs.subtrack.gpos[i].lat;
	tll.lon = rs.subtrack.gpos[i].lon-360.;
	sp = fmgeo2ucs(tll, rs.myproj);

	dx = sp.eastings - tp.eastings;
	dy = sp.northings - tp.northings;
	d = sqrt(pow(dx, 2) + pow(dy, 2));
	if (d < closest1) {
	    closest2 = closest1;
	    closest1 = d;
	    p2->lat = p1->lat;
	    p2->lon = p1->lon;
	    p1->lat = tll.lat;
	    p1->lon = tll.lon;
	} else if (d < closest2) {
	    closest2 = d;
	    p2->lat = tll.lat;
	    p2->lon = tll.lon;
	}
    }

    if (p1->lat < 0 || p2->lat < 0 || p1->lon > 170 || p2->lon > 170){ 
	fmerrmsg(where,"Invalid position found at:");
	fprintf(stderr," Geographical position: [%.2f %.2f] [%.2f %.2f]\n",
		p1->lat,p1->lon,p2->lat,p2->lon);
	fprintf(stderr," Image indices: %d, %d\n", rs.ind.col,rs.ind.row);
	return(FMFALSE);
    }
    
    return(FMTRUE);
}

/*
 * FUNCTION:
 * fmsubtrn
 * 
 * PURPOSE:
 * To find the closest point (latitude, longitude) on a subsatellite track to
 * the target point being examined. Spherical geometry is used. Input of
 * the two closest given points on the subtrack are input.
 * 
 * NOTES:
 * Utilizes spherical trigonometry.
 *
 * RETURN VALUES:
 * 0: No problem
 */

fmbool fmsubtrn(fmtriangpoints rs, fmgeopos *p){
    
    char *where="fmsubtrn";
    double c1, c1m, c2, c2m, c3, alpha, alpham, d, e, beta, h, gamma;
    fmgeopos s1, s2, t;

    if (rs.st2.lat > rs.st1.lat) {
	s1.lat = rs.st1.lat*DEG_TO_RAD;
	s1.lon = rs.st1.lon*DEG_TO_RAD;
	s2.lat = rs.st2.lat*DEG_TO_RAD;
	s2.lon = rs.st2.lon*DEG_TO_RAD;
    } else if (rs.st2.lat < rs.st1.lat) {
	s1.lat = rs.st2.lat*DEG_TO_RAD;
	s1.lon = rs.st2.lon*DEG_TO_RAD;
	s2.lat = rs.st1.lat*DEG_TO_RAD;
	s2.lon = rs.st1.lon*DEG_TO_RAD;
    } else {
	fmerrmsg(where,"Geometry setup");
	return(FMFALSE);
    }
    t.lat = rs.tp.lat*DEG_TO_RAD;
    t.lon = rs.tp.lon*DEG_TO_RAD;

    /*
     * Compute length of the sides in the spherical triangle.
     */
    c1m = (sin(s2.lat)*sin(t.lat)+
	cos(s2.lat)*cos(t.lat)*cos(fabs(s2.lon-t.lon)));
    c2m = (sin(s1.lat)*sin(t.lat)+
	cos(s1.lat)*cos(t.lat)*cos(fabs(s1.lon-t.lon)));
    c3 = acos(sin(s2.lat)*sin(s1.lat)+
	cos(s2.lat)*cos(s1.lat)*cos(fabs(s2.lon-s1.lon)));

    /*
     * Check values for possible numerical errors.
     */
    if (c1m < 1.0) {
	c1 = acos(c1m);
    } else {
	c1 = 0.0;
    }
    if (c2m < 1.0) {
	c2 = acos(c2m);
    } else {
	c2 = 0.0;
    }

    /*
     * Check whether target is very close to subtrack points or not, possible
     * source of error in final estiamte.
     */
    if (c1 < 0.0005) {
	p->lat = s2.lat*RAD_TO_DEG;
	p->lon = s2.lon*RAD_TO_DEG;
	return(FMTRUE);
    } else if (c2 < 0.0005) {
	p->lat = s1.lat*RAD_TO_DEG;
	p->lon = s1.lon*RAD_TO_DEG;
	return(FMTRUE);
    }
    
    /*
     * Compute the angle between sides c2 and c3.
     */
    alpham = ((cos(c1)-(cos(c2)*cos(c3)))/(sin(c2)*sin(c3)));
    if (alpham < 1.0 && alpham > -1.0) {
	alpha = acos(alpham);
    } else if (alpham >= 1.000) {
	alpha = 0.0;
    } else {
	alpha = acos(alpham);
    }
    
    /*
     * Compute the length of the line from the target pixel to the subsatellite
     * track that is normal to the subsatellite track. Utilize that sin(90)=1.0.
     */
    d = asin(sin(c2)*sin(alpha)); 
    if (fabs(c2-d) < 0.000001) { /* Too small distance apart */
	p->lat = s1.lat*RAD_TO_DEG;
	p->lon = s1.lon*RAD_TO_DEG;
	return(FMTRUE);
    }
    
    /*
     * Compute the distance from subtrack point 1 to the new subtrack point.
     */
    e = acos(cos(c2)/cos(d));
    /*
     * Compute new angle in new triangle with NP.
     */
    beta = acos((sin(s2.lat)-sin(s1.lat)*cos(c3))/(cos(s1.lat)*sin(c3)));
    /*
     * Compute co-latitude of new subtrack point.
     */
    h = acos(sin(s1.lat)*cos(e)+cos(s1.lat)*sin(e)*cos(beta));
    /*
     * Compute longitude adjustment relative to rs.st1.
     */
    gamma = asin((sin(e)*sin(beta))/sin(h));

    /*
     * Compute new latitude and longitude.
     */
    p->lat = 90.-(h*RAD_TO_DEG);
    if (s2.lon > s1.lon) {
	p->lon = (s1.lon+gamma)*RAD_TO_DEG;
    } else if (s2.lon < s1.lon) {
	p->lon = (s1.lon-gamma)*RAD_TO_DEG;
    } else {
	fmerrmsg(where,"Longitude output");
	return(FMFALSE);
    }
    /*
    printf(" %.2f %.2f %.2f %.2f\n", h*RAD_TO_DEG, gamma*RAD_TO_DEG, p->lat, p->lon);
    printf(" (st1: %.2f %.2f) (st2: %.2f %.2f) (st: %.2f %.2f)\n",
	rs.st1.lat,rs.st1.lon,rs.st2.lat,rs.st2.lon,p->lat,p->lon);
    */

    return(FMTRUE);
}


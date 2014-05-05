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
 *
 * MODIFIED:
 * NA
 */

#include <fmutil.h>

/*
 * NAME:
 * fmheatmap
 *
 * PURPOSE:
 * To generate a RGB matrix that starts at blue and ends at red, by way of
 * green using trigonometric functions to combine the various channels.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 * noclasses - number of classes that colors should be estimated for
 * red - vector containing red values
 * green - vector containing green values
 * blue - vector containing blue values
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 10.11.2004 
 *
 * MODIFIED:
 * NA
 */

void fmheatmap(int noclasses,
	unsigned short red[256], 
	unsigned short green[256], 
	unsigned short blue[256]) {


    int i=0, k=1;
    double angle=0., anglestep;

    red[0] = 0;			/* out of image */
    green[0] = 0;
    blue[0] = 0;

    anglestep = fmPI/(double) noclasses;

    for (i=0;i<noclasses;i++) {

	angle += anglestep;

	red[k] = (-cos(angle) >= 0) ? 
	    (unsigned short) rint(65535.*(-cos(angle))) : 0;
        green[k] = (sin(angle) >= 0) ? 
	    (unsigned short) rint(65535.*sin(angle)) : 0;
        blue[k] = (cos(angle) >= 0) ?
	    (unsigned short) rint(65535.*cos(angle)) : 0;
	
	k++;
    }
    for (i=k;i<256;i++) {
	red[i] = 0;
        green[i] = 0;
        blue[i] = 0;
    }
}

/*
 * NAME:
 * fmmapb2g
 *
 * PURPOSE:
 * To generate a RGB matrix that starts at blue and ends at green, by way of
 * red using trigonometric functions to combine the various channels.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 * noclasses - number of classes that colors should be estimated for
 * red - vector containing red values
 * green - vector containing green values
 * blue - vector containing blue values
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 10.11.2004 
 *
 * MODIFIED:
 * NA
 */

void fmmapb2g(int noclasses,
	unsigned short red[256], 
	unsigned short green[256], 
	unsigned short blue[256]) {


    int i=0, k=1;
    double angle=0., anglestep;

    red[0] = 0;			/* out of image */
    green[0] = 0;
    blue[0] = 0;

    anglestep = fmPI/(double) noclasses;

    for (i=0;i<noclasses;i++) {

	angle += anglestep;

        red[k] = (sin(angle) >= 0) ? 
	    (unsigned short) rint(65535.*sin(angle)) : 0;
	green[k] = (-cos(angle) >= 0) ? 
	    (unsigned short) rint(65535.*(-cos(angle))) : 0;
        blue[k] = (cos(angle) >= 0) ?
	    (unsigned short) rint(65535.*cos(angle)) : 0;
	
	k++;
    }
    for (i=k;i<256;i++) {
	red[i] = 0;
        green[i] = 0;
        blue[i] = 0;
    }
}

/*
 * NAME:
 * fmmapr2g
 *
 * PURPOSE:
 * To generate a RGB matrix that starts at red and ends at green, by way of
 * blue using trigonometric functions to combine the various channels.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 * noclasses - number of classes that colors should be estimated for
 * red - vector containing red values
 * green - vector containing green values
 * blue - vector containing blue values
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 10.11.2004 
 *
 * MODIFIED:
 * NA
 */

void fmmapr2g(int noclasses,
	unsigned short red[256], 
	unsigned short green[256], 
	unsigned short blue[256]) {


    int i=0, k=1;
    double angle=0., anglestep;

    red[0] = 0;			/* out of image */
    green[0] = 0;
    blue[0] = 0;

    anglestep = fmPI/(double) noclasses;

    for (i=0;i<noclasses;i++) {

	angle += anglestep;

        red[k] = (cos(angle) >= 0) ?
	    (unsigned short) rint(65535.*cos(angle)) : 0;
	green[k] = (-cos(angle) >= 0) ? 
	    (unsigned short) rint(65535.*(-cos(angle))) : 0;
        blue[k] = (sin(angle) >= 0) ? 
	    (unsigned short) rint(65535.*sin(angle)) : 0;
	
	k++;
    }
    for (i=k;i<256;i++) {
	red[i] = 0;
        green[i] = 0;
        blue[i] = 0;
    }
}

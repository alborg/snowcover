/*
 * NAME:
 * fm_extractstructs
 *
 * PURPOSE:
 * To create useful structures from the image metadata.
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * NA
 *
 * OUTPUT:
 * NA
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, METNO/FOU, 25.04.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 17.10.2007: Removed some print statements
 *
 * ID:
 * $Id$
 */

#include <fmio.h>

int fm_img2slopes(fmio_img imghead, fmscale *newcal) {

    newcal->slope = malloc(2*sizeof(fmslope));
    if (! newcal->slope) {
	return(FM_MEMALL_ERR);
    }
    newcal->nslopes = 2;

    (newcal->slope[0]).gain = imghead.rga;
    (newcal->slope[0]).intercept = imghead.ria;
    sprintf((newcal->slope[0]).description,"Reflectance");

    (newcal->slope[1]).gain = imghead.rgt;
    (newcal->slope[1]).intercept = imghead.rit;
    sprintf((newcal->slope[1]).description,"Temperature");

    return(FM_OK);
}

int fm_img2fmtime(fmio_img imghead, fmtime *newdate) {

    newdate->fm_hour = (int) imghead.ho;
    newdate->fm_min = (int) imghead.mi;
    newdate->fm_sec = (int) 0;
    newdate->fm_mday = (int) imghead.dd;
    newdate->fm_mon = (int) imghead.mm;
    newdate->fm_year = (int) imghead.yy;

    return(FM_OK);
}

int fm_img2fmucsref(fmio_img imghead, fmucsref *refucs) {

    refucs->Ax = (double) imghead.Ax;
    refucs->Ay = (double) imghead.Ay;
    refucs->Bx = (double) imghead.Bx;
    refucs->By = (double) imghead.By;
    refucs->iw = imghead.iw;
    refucs->ih = imghead.ih;

    return(FM_OK);
}

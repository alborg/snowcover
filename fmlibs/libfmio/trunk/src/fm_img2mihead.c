/*
 * NAME:
 * fm_img2mihead
 *
 * PURPOSE:
 * To copy header information (not data) to a separate structure.
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
 * Øystein Godøy, METNO/FOU, 2011-01-21 
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */

#include <fmio.h>

int fm_img2mihead(fmio_img orgimg, fmio_mihead *newhead) {

    sprintf(newhead->satellite,"%s",orgimg.sa);
    newhead->hour = orgimg.ho;
    newhead->minute = orgimg.mi;
    newhead->day = orgimg.dd;
    newhead->month = orgimg.mm;
    newhead->year = orgimg.yy;
    newhead->zsize = orgimg.z;
    newhead->xsize = orgimg.iw;
    newhead->ysize = orgimg.ih;
    newhead->Ax = orgimg.Ax;
    newhead->Ay = orgimg.Ay;
    newhead->Bx = orgimg.Bx;
    newhead->By = orgimg.By;
    newhead->orbit_no = orgimg.orbit_no;

    return(FM_OK);
}

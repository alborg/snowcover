/*
 * PURPOSE:
 * To convert header structures for imgh structures to mihead.
 *
 * INPUT:
 * imgh struct.
 *
 * OUTPUT:
 * mihead struct
 *
 * RETURN VALUES:
 * 0 - normal and correct ending
 * 1 - i/o problem
 * 2 - memory problem
 *
 * DEPENDENCIES:
 *
 * NOTES:
 * Some parameters are not transferred at present, amongst these are the
 * channel specification as no good code system has been developed for
 * this purpose.
 *
 * BUGS:
 * NA.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 24/07/2001
 */

#include <satimg.h>

int imghead2mihead(struct imgh oldhead, struct mihead *newhead) {

    sprintf(newhead->satellite,"%s",oldhead.sa);
    newhead->hour = oldhead.ho;
    newhead->minute = oldhead.mi;
    newhead->day = oldhead.dd;
    newhead->month = oldhead.mm;
    newhead->year = oldhead.yy;
    newhead->zsize = oldhead.z;
    newhead->xsize = oldhead.iw;
    newhead->ysize = oldhead.ih;
    newhead->Ax = oldhead.Ax;
    newhead->Ay = oldhead.Ay;
    newhead->Bx = oldhead.Bx;
    newhead->By = oldhead.By;

    return(0);
}

/*
 * NAME:
 * fm_init_fmio_img
 *
 * PURPOSE:
 * To initialise the data structure used to hold image data.
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
 * �ystein God�y, METNO/FOU, 26.04.2007 
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */ 

#include <fmio.h>

int fm_init_fmio_img(fmio_img *h) {
    int i;
    
    h->iw = 0;
    h->ih = 0;
    h->size = 0;
    h->track = NULL;
    h->numtrack = 0;
    h->orbit_no = 0;
    h->rga = 0.;
    h->ria = 0.;
    h->rgt = 0.;
    h->rit = 0.;
    sprintf(h->sa, "%s", "");
    sprintf(h->area, "%s", "");
    /*
    sprintf(h->source, "%s", "");
    */
    h->ho = 0;
    h->mi = 0;
    h->dd = 0;
    h->mm = 0;
    h->yy = 0;
    h->z = 0;
    h->Ax = 0.;
    h->Ay = 0.;
    h->Bx = 0.;
    h->By = 0.;
    for (i=0; i<FMIO_NCHAN; i++) {
	h->ch[i] = 0;
	h->image[i] = NULL;
    }
    return(FM_OK);
}

/*
 * NAME:
 * fm_clear_fmio_img
 *
 * PURPOSE:
 * To free allocated memory for reallocation.
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
 * �ystein God�y, met.no/FOU, xx.xx.xxxx
 *
 * MODIFIED:
 * Soren Andersen DMI August 2001: Added freeing of h->track
 * �ystein God�y, METNO/FOU, 03.07.2007 
 *
 * ID:
 * $Id$
 */ 

int fm_clear_fmio_img(fmio_img *h) {
    int i;

    if (h->track != NULL) {
        free(h->track);
        h->track = NULL;
	h->numtrack = 0;
    }
    for (i=0; i<h->z; i++) {
        if (h->image[i] != NULL) {
            free(h->image[i]);
            h->image[i] = NULL;
        } 
    }

    return(FM_OK);
}


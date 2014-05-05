/*
 * NAME:
 * fmimage.c
 *
 * PURPOSE:
 * Defining functions for handling images.
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

/*
 * NAME:
 * init_fmimage
 *
 * PURPOSE:
 * To initialize the fmimage structure containing both image data 
 * and header information. 
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 * FM_OK
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 10.01.2005 
 *
 * MODIFIED:
 * NA
 */
int init_fmimage(fmimage *d) {

    sprintf(d->satellite_name, "%s", "");
    sprintf(d->area_description, "%s", "");
    sprintf(d->processing_source, "%s", "");

    (d->valid_time).fm_year = 0;
    (d->valid_time).fm_mon = 0;
    (d->valid_time).fm_mday = 0;
    (d->valid_time).fm_hour = 0;
    (d->valid_time).fm_min = 0;
    (d->valid_time).fm_sec = 0;

    (d->ucs).Bx = 0;
    (d->ucs).By = 0;
    (d->ucs).Ax = 0;
    (d->ucs).Ay = 0;
    (d->ucs).iw = 0;
    (d->ucs).ih = 0;

    (d->subtrack).npoints = 0;
    (d->subtrack).gpos = NULL;

    (d->scale_ushort).nslopes = 0;
    (d->scale_ushort).slope = NULL;

    (d->scale_int).nslopes = 0;
    (d->scale_int).slope = NULL;

    d->fieldsize = 0;

    d->channel_id = NULL;

    d->projcall = NULL;

    d->bytearray = NULL;
    d->ushortarray = NULL;
    d->intarray = NULL;
    d->floatarray = NULL;
    d->doublearray = NULL;

    return(FM_OK);
}

/*
 * NAME:
 * free_fmimage
 *
 * PURPOSE:
 * To free dynamically allocated memory within the fmimage structure.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 * FM_OK
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
int free_fmimage(fmimage *d) {

    if ((d->subtrack).gpos) {
	free((d->subtrack).gpos);
	(d->subtrack).npoints = 0;
    }

    if ((d->scale_ushort).slope) {
	free((d->scale_ushort).slope);
	(d->scale_ushort).nslopes = 0;
    }

    if ((d->scale_int).slope) {
	free((d->scale_int).slope);
	(d->scale_int).nslopes = 0;
    }

    if (d->channel_id) {
	fmfree_byte_2d((d->channel_id),d->channels);
    }

    if (d->projcall) {
	fmfree_char_vector((d->projcall));
    }

    if (d->bytearray) {
	fmfree_byte_2d((d->bytearray),d->channels);
    }

    if (d->ushortarray) {
	fmfree_ushort_2d((d->ushortarray),d->channels);
    }

    if (d->intarray) {
	fmfree_int_2d((d->intarray),d->channels);
    }

    if (d->floatarray) {
	fmfree_float_2d((d->floatarray),d->channels);
    }

    if (d->doublearray) {
	fmfree_double_2d((d->doublearray),d->channels);
    }

    return(FM_OK);
}

/*
 * NAME:
 * unpack_fmimage_ushort
 *
 * PURPOSE:
 * Unpacks floating point values from unsigned short to float. If e.g.
 * AVHRR bi-directional reflectance or brightness temperatures are stored
 * as unsigned short or integer values by the process generating the
 * files, these values can be unpacked using this function. Typically
 * brightness-temperatures are returned as Kelvin and bi-directional
 * reflectance as percentage (0-100), but this is determined by the
 * process encoding the data (e.g. KSPT MEOS, AAPP4AVHRR, etc.).
 *
 * REQUIREMENTS:
 *
 * INPUT:
 * val - value to decode
 * scale - available slopes to use
 * use - which slope to use
 *
 * OUTPUT:
 * unpacked value as floating point
 *
 * NOTES:
 * The original channel identification for AVHRR data at NMI were:
 * AVHRR Ch 1 - 1
 * AVHRR Ch 2 - 2
 * AVHRR Ch 3B - 3
 * AVHRR Ch 4 - 4
 * AVHRR Ch 5 - 5
 * AVHRR Ch 3A - 6
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 */
float unpack_fmimage_ushort(unsigned short val, fmscale scale, int use) {

    float cval;

    cval = (scale.slope[use]).intercept 
	+ ((scale.slope[use]).gain*((float) val));
    
    return(cval);
}

/*
 * NAME:
 * unpack_fmimage_int
 *
 * PURPOSE:
 * Unpacks floating point values from unsigned short to float. If e.g.
 * AVHRR bi-directional reflectance or brightness temperatures are stored
 * as unsigned short or integer values by the process generating the
 * files, these values can be unpacked using this function. Typically
 * brightness-temperatures are returned as Kelvin and bi-directional
 * reflectance as percentage (0-100), but this is determined by the
 * process encoding the data (e.g. KSPT MEOS, AAPP4AVHRR, etc.).
 *
 * REQUIREMENTS:
 *
 * INPUT:
 * val - value to decode
 * scale - available slopes to use
 * use - which slope to use
 *
 * OUTPUT:
 * unpacked value as floating point
 *
 * NOTES:
 * The original channel identification for AVHRR data at NMI were:
 * AVHRR Ch 1 - 1
 * AVHRR Ch 2 - 2
 * AVHRR Ch 3B - 3
 * AVHRR Ch 4 - 4
 * AVHRR Ch 5 - 5
 * AVHRR Ch 3A - 6
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 */
float unpack_fmimage_int(int val, fmscale scale, int use) {

    float cval;

    cval = (scale.slope[use]).intercept 
	+ ((scale.slope[use]).gain*((float) val));
    
    return(cval);
}

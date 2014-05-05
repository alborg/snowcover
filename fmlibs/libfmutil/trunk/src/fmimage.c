/*
 * NAME:
 * fmimage.c
 *
 * PURPOSE:
 * Defining functions for handling images. The primary use for these are
 * in combination with libfmio, but they are useful as internal
 * datatructures for processing as well as they can be used to add layers
 * during processing.
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
 * Øystein Godøy, METNO/FOU, 07.01.2005
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 2011-11-10: Adapted to the new structures.
 *
 * ID:
 * $Id$
 */

#include <fmutil.h>
/*
#include <stdio.h>
#include <stdlib.h>
#include <fmutil_types.h>
#include <fmtime.h>
#include <fmcoord.h>
#include <fmangles.h>
#include <fmstorage.h>
#include <fmimage.h>
*/

/*
 * NAME:
 * init_fmdataset
 *
 * PURPOSE:
 * To initialize the fmdataset structure containing both image data 
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
int init_fmdataset(fmdataset *d) {

    sprintf(d->h.platform_name, "%s", "");
    sprintf(d->h.satellite_name, "%s", "");
    sprintf(d->h.sensor_name, "%s", "");
    sprintf(d->h.area_description, "%s", "");
    sprintf(d->h.processing_description, "%s", "");
    sprintf(d->h.product_description, "%s", "");
    sprintf(d->h.distribution_constraints, "%s", "");
    sprintf(d->h.use_constraints, "%s", "");

    d->h.orbit_no = 0;

    d->h.map_projected = FMFALSE;
    d->h.ucs_positioned = FMFALSE;
    d->h.subtrack_added = FMFALSE;

    (d->h.valid_time).fm_year = 0;
    (d->h.valid_time).fm_mon = 0;
    (d->h.valid_time).fm_mday = 0;
    (d->h.valid_time).fm_hour = 0;
    (d->h.valid_time).fm_min = 0;
    (d->h.valid_time).fm_sec = 0;

    (d->h.timespan[0]).fm_year = 0;
    (d->h.timespan[0]).fm_mon = 0;
    (d->h.timespan[0]).fm_mday = 0;
    (d->h.timespan[0]).fm_hour = 0;
    (d->h.timespan[0]).fm_min = 0;
    (d->h.timespan[0]).fm_sec = 0;
    (d->h.timespan[1]).fm_year = 0;
    (d->h.timespan[1]).fm_mon = 0;
    (d->h.timespan[1]).fm_mday = 0;
    (d->h.timespan[1]).fm_hour = 0;
    (d->h.timespan[1]).fm_min = 0;
    (d->h.timespan[1]).fm_sec = 0;

    (d->h.ucs).Bx = 0;
    (d->h.ucs).By = 0;
    (d->h.ucs).Ax = 0;
    (d->h.ucs).Ay = 0;
    (d->h.ucs).iw = 0;
    (d->h.ucs).ih = 0;

    (d->h.subtrack).npoints = 0;
    (d->h.subtrack).gpos = NULL;

    d->h.xsize = 0;
    d->h.ysize = 0;

    d->h.layers = 0;

    (d->h).layer_id = NULL;

    sprintf(d->h.projstring,"%s","");

    /*
     * Datafields below
     */
    d->d = NULL;
    /*
    d->d->bytearray = NULL;
    d->d->ushortarray = NULL;
    d->d->intarray = NULL;
    d->d->floatarray = NULL;
    d->d->doublearray = NULL;
    */

    return(FM_OK);
}

/*
 * NAME:
 * free_fmdataset
 *
 * PURPOSE:
 * To free dynamically allocated memory within the fmdataset structure.
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
int free_fmdataset(fmdataset *d) {

    if (((d->h).subtrack).gpos) {
	free(((d->h).subtrack).gpos);
	((d->h).subtrack).npoints = 0;
    }

    if ((d->h).layer_id) {
	fmfree_byte_2d(((d->h).layer_id),(d->h).layers);
    }

    /*
     * Must remember to free the actual data as well...
     */
    /*
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
    */

    return(FM_OK);
}

/*
 * Allocate fmdatafield and initialise all pointers to NULL for further
 * allocation when reading the actual data.
 */
int allocate_fmdatafield(fmdatafield **d, int layers) {
    char *where="allocate_fmdatafield";
    int i;

    *d = malloc(sizeof(fmdatafield)*layers);
    if (*d == NULL) {
        fmerrmsg(where,"Could not allocate the necessary layers for fmdatafield");
        return(FM_MEMALL_ERR);
    }

    for (i=0;i<layers;i++) {
        (*d)[i].bytearray = NULL;
        (*d)[i].ushortarray = NULL;
        (*d)[i].intarray = NULL;
        (*d)[i].floatarray = NULL;
        (*d)[i].doublearray = NULL;
        (*d)[i].class_names = NULL;
        if (fmalloc_char_vector(&((*d)[i].description),50)) {
            fmerrmsg(where,"Could not allocate description at layer %d", i);
            return(FM_MEMALL_ERR);
        }
        if (fmalloc_char_vector(&((*d)[i].unit),50)) {
            fmerrmsg(where,"Could not allocate unit at layer %d", i);
            return(FM_MEMALL_ERR);
        }
        (*d)[i].packed = FMFALSE;
        (*d)[i].palette = FMFALSE;
        (*d)[i].number_of_classes = 0;
        (*d)[i].missingdatavalue = 0;
        (*d)[i].nodatavalue = 0;
    }

    return(FM_OK);
}

/*
 * NAME:
 * unpack_fmdatafield_ushort
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
float unpack_fmdatafield_byte(char val, fmscale scale, int use) {

    float newval;

    use -= 1;

    newval = (scale.slope[use]).intercept 
	+ ((scale.slope[use]).gain*((float) val));
    
    return(newval);
}

float unpack_fmdatafield_ushort(unsigned short val, fmscale scale, int use) {

    float newval;

    use -= 1;

    newval = (scale.slope[use]).intercept 
	+ ((scale.slope[use]).gain*((float) val));
    
    return(newval);
}

float unpack_fmdatafield_short(short val, fmscale scale, int use) {

    float newval;

    use -= 1;

    newval = (scale.slope[use]).intercept 
	+ ((scale.slope[use]).gain*((float) val));
    
    return(newval);
}

/*
 * NAME:
 * unpack_fmdatafield_int
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
float unpack_fmdatafield_int(int val, fmscale scale, int use) {

    float newval;

    use -= 1;

    newval = (scale.slope[use]).intercept 
	+ ((scale.slope[use]).gain*((float) val));

    return(newval);
}

float unpack_fmdatafield_uint(unsigned int val, fmscale scale, int use) {

    float newval;

    use -= 1;

    newval = (scale.slope[use]).intercept 
	+ ((scale.slope[use]).gain*((float) val));

    return(newval);
}

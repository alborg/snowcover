/*
 * PURPOSE:
 * Header file for the HDF5 software used to store collocated stattionwise
 * data.
 *
 * NOTE:
 * See stdats2hdf.c for details.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 27/07/2000
 */

#ifndef _STDATS4HDF_H
#define _STDATS4HDF_H

/*
 * Header files that are required.
 */
#include <hdf5.h>
#include <satimg.h>
#include <avhrr_stdat.h>
#include <std_stdat.h>
#include <nwp_stdat.h>

/*
 * Function prototypes.
 */
int stdats2hdf(char *filename, char *class, 
	obsstruct sobs, as_data aobs, ns_data nobs);
/*
short read_hdf5_product(char *filename, osihdf *f, short mode);
short init_osihdf(osihdf *f);
short malloc_osihdf(osihdf *f, osi_dtype *dfs, char **desc);
short free_osihdf(osihdf *f);
*/

/*
 * End function prototypes.
 */

#endif /* _STDATS4HDF_H */

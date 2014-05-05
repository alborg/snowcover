/*
 * NAME:
 * safcm_stdat.h
 *
 * PURPOSE:
 * Header file for safcm_stdat.c
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
 * Øystein Godøy, DNMI/FOU, 06/11/2000
 *
 * MODIFIED:
 * Øystein Godøy, MI/FOU, 06.11.2003
 * Due to errors when extracting these data, a dummy interface is
 * created to circumvent the problem in the current situation. This
 * involves the use of a slimmer data structure not containing the
 * character strings describing the class names.
 * Øystein Godøy, METNO/FOU, 2013-04-18: Switched to new PPS libraries.
 *
 * CVS_ID:
 * $Id$
 */

#ifndef _SAFCM_STDAT_H
#define _SAFCM_STDAT_H

#include <fmutil.h>
#include <dirent.h>
#include <fmcol.h>
#include <fmcolaccess.h>

#ifdef FMCOL_HAVE_LIBSMHI_SAF
#include <pps_cloudproducts_io.h>

#define M2KM(x) (x/1000.00)

#define SAFCM_MISVAL 255

/*
 * Probably a flag to determine whether debugging info should be output to
 * logs or not, a non-zero number is TRUE probably...
 */
/*int sm_debug = 1;*/

int safcm_stdat(fmgeopos gpos, fmprojspec myproj, 
	CTy_t *ctype, safcm_data *cm);
int safcm_init(int boxsize, safcm_data *cm);
int safcm_free(safcm_data *cm);
char *safcm_prod(char *filename);
int safcm_name(fmtime refdate, char *basename, char *path, char *filename);
int safcm_check(fmio_img refdata, CTy_t *ctype);

#endif /* FMCOL_HAVE_LIBSMHI_SAF */
#endif /* _SAFCM_STDAT_H */


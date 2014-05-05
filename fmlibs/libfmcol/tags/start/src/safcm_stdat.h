/*
 * NAME:
 * safcm_stdat.h
 * 
 * PURPOSE:
 * Header file for safcm_stdat.c
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * RETURN VALUES:
 *
 * DEPENDENCIES:
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 06/11/2000
 * Øystein Godøy, MI/FOU, 11.04.2003
 * Øystein Godøy, MI/FOU, 06.11.2003
 * Due to errors when extracting these data, a dummy interface is
 * created to circumvent the problem in the current situation. This
 * involves the use of a slimmer data structure not containing the
 * character strings describing the class names.
 */

#ifndef _SAFCM_STDAT_H
#define _SAFCM_STDAT_H

#include <dirent.h>
#include <collocate1.h>
#include <pps_cloudproducts_io.h>

#include <satimg.h>
#include <fmcoord.h>

#define M2KM(x) (x/1000.00)

#define SAFCM_MISVAL 255

/*
 * Probably a flag to determine whether debugging info should be output to
 * logs or not, a non-zero number is TRUE probably...
 */
/*int sm_debug = 1;*/

/*
 * This is the structure that will be stored in the database...
 */
typedef struct {
    char source[20];
    int nopix;
    time_t vtime;
    struct ucs nav;
    unsigned char data[BOXSIZE2D];
    char description[SM_NUMBER_OF_CLOUDTYPE_VALUES][MAX_LENGTH_STRING];
} safcm_data;

typedef struct {
    char source[20];
    int nopix;
    time_t vtime;
    unsigned char data[BOXSIZE2D];
} safcm_data2;

int safcm_stdat(struct latlon gpos, fmproj myproj, 
	CTy_t *ctype, safcm_data *cm);
int safcm_init(int boxsize, safcm_data *cm);
int safcm_free(safcm_data *cm);
char *safcm_prod(char *filename);
int safcm_name(struct dto refdate, char *basename, char *path, char *filename);
int safcm_check(struct imgh refdata, CTy_t *ctype);
    
#endif /* _SAFCM_STDAT_H */


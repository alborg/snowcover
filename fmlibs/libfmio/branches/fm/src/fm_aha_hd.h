/*
 * NAME: 
 * aha_hd
 * 
 * PURPOSE:
 * Header file accompanying aha_hd.c
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * RETURN VALUES:
 * NA
 * 
 * AUTHOR:
 * Soren Andersen DMI August 2001 
 * MODIFIED:
 * Øystein Godøy, met.no/FOU, 23.02.2004
 * Added some comments and reindented...
 * Øystein Godøy, METNO/FOU, 16.10.2006
 * Modified name space for libfmio.
 */

/* Modified/created by Soren Andersen DMI August 2001 */

#define FMIO_STRMAXCHARS 200
#define FMIO_MAXNLAY 10
#define FMIO_MAXHDLINES 300
#define FMIO_TRUE 1
#define FMIO_FALSE 0
#define FMIO_IMISS -9999
#define FMIO_DMISS -9999.9

typedef struct {
    char product[FMIO_STRMAXCHARS];
    char satid[FMIO_STRMAXCHARS];
    char channel_id[FMIO_MAXNLAY][FMIO_STRMAXCHARS];
    char version_des[FMIO_STRMAXCHARS];
    char site_id[FMIO_STRMAXCHARS];

    /* For internal use when saving/reading data */
    int  layers;
    char typecode[FMIO_MAXNLAY];
    int  itemsize[FMIO_MAXNLAY];
    char datatypes[FMIO_MAXNLAY][FMIO_STRMAXCHARS];
    char compressed;
    int  start_byte[FMIO_MAXNLAY];
    char little_end;

    /* Different nodata attributes */
    double nodata;
    double missing_data;
    double footprint_nodata; 	/* Probably not used */
    double scan_nodata;		/* Probably not used */
    double spot_nodata; 		/* Probably not used */

    /* Indicators for how many scanlines that were used */
    int  numof_scanlines;
    int  numof_missing_scanlines;

    /* Time stamps */
    int     data_first_year;
    int     data_first_dayofyear;
    double  data_first_secofday;
    int     data_last_year;
    int     data_last_dayofyear;
    double  data_last_secofday;
    double  data_first_sec1970;
    int     orbit_epoch_year;
    int     orbit_epoch_day;

    /* Calibration coefficients (Python dictionaries not used or implem'ted) */
    /*	 'radiance2reflectance',
	 'radtempcnv',
	 'calvis',
	 'tempradcnv',
	 '20equifiltwidth',
	 '20solfiltirrad', */

    /* Area definition */
    double area_extent[4];
    char   pcs[FMIO_STRMAXCHARS];
    char   area[FMIO_STRMAXCHARS];
    int    xsize;
    int    ysize;
    double xscale;
    double yscale;

    /* Intercept and gains for the IR and VIS channels */
    double ir_gain;
    double ir_intercept;
    double vis_gain;
    double vis_intercept;
    char   comment[FMIO_STRMAXCHARS]; 
} ahahd;

int fm_fill_aha_hd(char *fname,ahahd *header);
void fm_split_intarr(int intarr[FMIO_MAXNLAY], int n, char buf[FMIO_STRMAXCHARS]);
void fm_split_chrarr(char chrarr[FMIO_MAXNLAY], int n, char buf[FMIO_STRMAXCHARS]);
void fm_split_strarr(char strarr[FMIO_MAXNLAY][FMIO_STRMAXCHARS], int n, char buf[FMIO_STRMAXCHARS]);
int fm_iget_hd_from_hsa(char hsa[FMIO_MAXHDLINES][FMIO_STRMAXCHARS],int nhsa,char key[FMIO_STRMAXCHARS]);
int fm_get_hd_from_hsa(char hsa[FMIO_MAXHDLINES][FMIO_STRMAXCHARS],int nhsa,char key[FMIO_STRMAXCHARS],char hd[FMIO_STRMAXCHARS]);
int fm_get_aha_hsa(char *fname,char hsa[FMIO_MAXHDLINES][FMIO_STRMAXCHARS]);


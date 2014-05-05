
/*
 * NAME:
 *
 * PURPOSE:
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
 * Øystein Godøy, METNO/FOU, 29.09.2010: fmstring2fmtime made public
 * available.
 * Øystein Godøy, METNO/FOU, 30.09.2010: added fmsec19702isodate.
 * Øystein Godøy, METNO/FOU, 2011-11-01: Added fmmonthnum.
 *
 * ID:
 * $Id$
 */


#ifndef FMTIME_H
#define FMTIME_H

#define FMUTIL_ISODATETIME_LENGTH 20 /* To hold 'yyyy-mm-ddThh:mn:ssZ'. Strings should be +1 for '\0' */
#define FMUTIL_CFEPOCH_LENGTH     19 /* To hold 'yyyy-mm-dd hh:mn:ss'.  Strings should be +1 for '\0' */

typedef struct {
    int     fm_sec;    /* seconds */
    int     fm_min;    /* minutes */
    int     fm_hour;   /* hours */
    int     fm_mday;   /* day of the month */
    int     fm_mon;    /* month */
    int     fm_year;   /* year */
    int     fm_wday;   /* day of the week */
    int     fm_yday;   /* day in the year */
} fmtime;

typedef long fmsec1970; /* unix time */

int is_leap_year(int year);
int check_fmtime(fmtime *t, int verbose);
int tofmtime(fmsec1970 secs, fmtime *fmt);
fmsec1970 tofmsec1970(fmtime t);
fmsec1970 ymdhms2fmsec1970(char *str,int mode);
fmsec1970 ymdh2fmsec1970(char *str,int mode);
fmsec1970 isodatetime2fmsec1970(char *str);
void fmsec19702isodatetime(fmsec1970 secs, char *str);
void fmsec19702isodate(fmsec1970 secs, char *str); 
fmsec1970 fmutc2tst(fmsec1970 fmutc, double lon);
fmsec1970 fmcet2tst(fmsec1970 cet, double lon);
fmsec1970 fmloncorr(double meridian, double lon); 
void mjd19502fmtime(double mjd, fmtime *cal);
void fmtime2mjd1950(fmtime cal, double *mjd);
double fmhourangle(double tst);
int fmmeosdate2fmtime(char s[], fmtime *d);

fmsec1970 CFepoch2fmsec1970(char *str);
void      fmsec19702CFepoch(fmsec1970 secs, char *str);

int fmstring2fmtime(char date[], char fmt[], fmtime *res);

fmsec1970 ymdhms2fmsec1970_alt(char *str,int mode);
fmsec1970 ymdh2fmsec1970_alt(char *str,int mode);
fmsec1970 isodatetime2fmsec1970_alt(char *str);

int correct_time_from_unit(double amount_from_epoch, char *unit_string,fmsec1970 *secsFrom1970);

int fmdayofyear(fmtime md); 
int fmdoy2mmdd(int year, int doy, int *m2, int *d2);

int fmmonthnum(char *monthname); 
#endif

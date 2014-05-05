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
 * NA
 */

#include <fmutil.h>

#ifndef FMTIME_H
#define FMTIME_H

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

int fmdayofyear(fmtime md); 
int tofmtime(fmsec1970 secs, fmtime *fmt);
fmsec1970 tofmsec1970(fmtime t);
fmsec1970 ymdhms2fmsec1970(char *str,int mode);
fmsec1970 fmutc2tst(fmsec1970 fmutc, double lon);
fmsec1970 fmcet2tst(fmsec1970 cet, double lon);
fmsec1970 fmloncorr(double meridian, double lon); 
void mjd19502fmtime(double mjd, fmtime *cal);
void fmtime2mjd1950(fmtime cal, double *mjd);
double fmhourangle(double tst);
int fmmeosdate2fmtime(char s[], fmtime *d);

#endif

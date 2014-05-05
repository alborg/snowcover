/*
 * NAME:
 * tofmtime
 * tofmsect1970
 * ymdhms2fmsec1970
 *
 * PURPOSE:
 * To convert back and forth between the time structure fmtime (similar to
 * struct tm) and fmsectm (similar to time_t) without fear of what locale
 * settings could do to the input. The main intention is to be able to
 * read time in either format from a file and convert to the other without
 * using time_T (always in UTC) or struct tm (locale dependent).
 *
 * NOTES:
 * fmsec1970 is always assumed to be UTC time when conversions are
 * performed, it can however be used for storage of true solar time or
 * CET.
 *
 * BUGS:
 * Failure returns not implemented yet...
 *
 * RETURN VALUES:
 * tofmsec1970 returns seconds since
 * tofmtime returns 0 when success, and 2 when failure
 *
 * REQUIREMENTS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 25.02.2004
 * MODIFIED:
 */

#include <fmutil.h>
#include <time.h>

/*
 * FUNCTION:
 * fmdayofyear.c
 * 
 * PURPOSE:
 * Computes Julian day number (day of the year in the range 1-365).
 * 
 * RETURN VALUES:
 * Returns the Julian Day.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 14/12/2001
 */
int fmdayofyear(fmtime md) {

    static unsigned short int daytab[2][13]=
	{{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, 
	{0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
    unsigned short int i, leap;
    int dn;
     
    leap = 0;
    if ((md.fm_year%4 == 0) && 
	    (md.fm_year%100 != 0 || md.fm_year%400 == 0)) leap=1;
    
    dn = md.fm_mday;
    for (i=1; i<md.fm_mon; i++) {
	dn += (int) daytab[leap][i];
    }
     
    return(dn);
}
fmsec1970 tofmsec1970(fmtime t) {

    long secs, tmpsecs;
    struct tm tmref, tminp;
    time_t tzdelta;

    /*
     * Estimate delta time caused by timezone locale. January 1, 1970,
     * 00:00:00 should return 0 seconds.
     */
    tmref.tm_year = 1970-1900;
    tmref.tm_mon = 1-1;
    tmref.tm_mday = 1;
    tmref.tm_hour = 0;
    tmref.tm_min = 0;
    tmref.tm_sec = 0;
    tmref.tm_isdst = 0;

    tzdelta = mktime(&tmref);

    /*
     * Convert from struct to sec.
     */
    tminp.tm_year = t.fm_year-1900;
    tminp.tm_mon = t.fm_mon-1;
    tminp.tm_mday = t.fm_mday;
    tminp.tm_hour = t.fm_hour;
    tminp.tm_min = t.fm_min;
    tminp.tm_sec = t.fm_sec;
    tminp.tm_isdst = 0;

    tmpsecs = mktime(&tminp);

    /*
     * Apply timezone correction.
     */
    secs = tmpsecs-tzdelta;

    return(secs);
}

int tofmtime(fmsec1970 secs, fmtime *fmt) {

    long tmpsecs;
    struct tm tmref, *tminp;
    time_t tzdelta;

    /*
     * Estimate delta time caused by timezone locale. January 1, 1970,
     * 00:00:00 should return 0 seconds.
     */
    tmref.tm_year = 1970-1900;
    tmref.tm_mon = 1-1;
    tmref.tm_mday = 1;
    tmref.tm_hour = 0;
    tmref.tm_min = 0;
    tmref.tm_sec = 0;
    tmref.tm_isdst = 0;

    tzdelta = mktime(&tmref);

    /*
     * Apply timezone correction.
     */
    tmpsecs = secs+tzdelta;

    /*
     * Create struct
     */
    tminp = localtime(&tmpsecs);

    /*
     * Put into fmtime
     */
    fmt->fm_year = tminp->tm_year+1900;
    fmt->fm_mon = tminp->tm_mon+1;
    fmt->fm_mday = tminp->tm_mday;
    fmt->fm_hour = tminp->tm_hour;
    fmt->fm_min = tminp->tm_min;
    fmt->fm_sec = tminp->tm_sec;
    fmt->fm_wday = tminp->tm_wday;
    fmt->fm_yday = tminp->tm_yday;

    return(0);
}

fmsec1970 ymdhms2fmsec1970(char *str, int mode) {

    char *where="ymdhms2fmsec1970";
    fmsec1970 secs;
    fmtime t;
    int i;
    char *dummy;

    /*
     * Check the input.
     */
    if (strlen(str) < 14) {
	fmerrmsg(where,"Input datetime string is too short");
	return(-1);
    }
    
    dummy = (char *) malloc(5*sizeof(char));
    if (!dummy) return(2);

    /*
     * Initialize
     */
    for (i=0; i<5; i++) {
	dummy[i] = '\0';
    }

    /*
     * Get year
     */
    for (i=0;i<4;i++) {
	dummy[i] = str[i];
    }
    t.fm_year = atoi(dummy);
    dummy[2] = dummy[3] = '\0';

    /*
     * Get month
     */
    for (i=0;i<2;i++) {
	dummy[i] = str[4+i];
    }
    t.fm_mon = atoi(dummy);
    if (t.fm_mon < 1 || t.fm_mon > 12) {
	t.fm_mon = -9; 
    } 

    /*
     * Get day
     */
    for (i=0;i<2;i++) {
	dummy[i] = str[6+i];
    }
    t.fm_mday = atoi(dummy);
    if (t.fm_mday < 1 || t.fm_mday > 31) t.fm_mday = -9; 

    /*
     * Get hour
     */
    for (i=0;i<2;i++) {
	dummy[i] = str[8+i];
    }
    t.fm_hour = atoi(dummy);
    if (t.fm_hour < 0 || t.fm_hour > 23) t.fm_hour = -9; 

    /*
     * Get minute
     */
    for (i=0;i<2;i++) {
	dummy[i] = str[10+i];
    }
    t.fm_min = atoi(dummy);
    if (t.fm_min < 0 || t.fm_min > 59) t.fm_min = -9; 

    /*
     * Get seconds
     */
    for (i=0;i<2;i++) {
	dummy[i] = str[12+i];
    }
    t.fm_sec = atoi(dummy);
    if (t.fm_sec < 0 || t.fm_sec > 59) t.fm_sec = -9; 

    /*
     * Release resiources
     */
    free(dummy);

    if (mode > 0) {
	printf(" %4d-%02d-%02d %02d:%02d:%02d\n",
		t.fm_year,t.fm_mon,t.fm_mday,
		t.fm_hour,t.fm_min,t.fm_sec);
    }
    if (t.fm_sec < 0 || t.fm_min < 0 || t.fm_hour < 0 || t.fm_mday < 0 ||
	t.fm_mon < 0 || t.fm_year < 0) {
	fmerrmsg(where,"Decoding of datetime string failed");
	fmlogmsg(where,"Decoding of datetime string failed");
    }
    
    secs = tofmsec1970(t);

    return(secs);
}

/*
 * NAME:
 * utc2tst
 * cet2tst
 * loncorr
 *
 * PURPOSE:
 * To convert from UTC time to True Solar Time.
 * To convert from CET time to True Solar Time.
 *
 * NOTES:
 * This function operates on time specified as seconds since 1970, ie
 * standard UNIX time but implemented through fmsec1970 to be independent
 * of any locale specific setting affecting mktime and localtime...
 *
 * BUGS:
 * NA
 *
 * REQUIREMENTS:
 * equationoftime
 * tofmtime
 *
 * RETURN VALUES:
 * Converted seconds since 1970 or a negative value in case of failure.
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 25.02.2004
 */
fmsec1970 fmutc2tst(fmsec1970 fmutc, double lon){

    fmsec1970 tst, deltat, tmptime;
    fmtime t;


    /*
     * The difference between local time and UTC is 240 seconds per degree
     * longitude. Longitude is taken positive eastwards.
     */
    tmptime = fmutc+fmloncorr(0.,lon);

    /*
     * In addition the local time and true solar time differs by the
     * equation of time which compensates for the irregular behaviour of
     * the Earth's orbit around the Sun.
     */
    tofmtime(tmptime,&t);
    deltat = fmequationoftime(t.fm_yday);

    tst = tmptime+deltat;
    
    return(tst);
}

fmsec1970 fmcet2tst(fmsec1970 cet, double lon){

    fmsec1970 tst, deltat, tmptime;
    fmtime t;

    /*
     * The difference between local time and UTC is 240 seconds per degree
     * longitude. Longitude is taken positive eastwards.
     */
    tmptime = cet+fmloncorr(15.,lon);

    /*
     * In addition the local time and true solar time differs by the
     * equation of time which compensates for the irregular behaviour of
     * the Earth's orbit around the Sun.
     */
    tofmtime(tmptime,&t);
    deltat = fmequationoftime(t.fm_yday);

    tst = tmptime+deltat;
    
    return(tst);
}

fmsec1970 fmloncorr(double meridian, double lon) {

    double corr;

    /*
     * The time correction is 240 seconds per degree longitude difference,
     * longitude positive eastwards.
     */
    corr = (fmsec1970) floor(((lon-meridian)*240.)+0.5);

    return(corr);
}


/*
 * NAME:
 * mjd19502fmtime
 * fmtime2mjd1950
 *
 * PURPOSE:
 * Functions that convert to and from modified julian date referenced to
 * 1. January 1950 and fmtime structures.
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * See functions for details.
 *
 * OUTPUT:
 * NA
 *
 * NOTES:
 * Functions does not handle milliseconds...
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Original Fortran code from ESA 1.2 3/1/94 ORBLIB
 * Modified for C by Øystein Godøy, met.no/FOU, 28.04.2004
 *
 * MODIFIED:
 * NA
 */


/*
 * Computes calender date from modified julian day
 * for calender dates between 1 jan 1950 and 31 dec 2099
 */
void mjd19502fmtime(double mjd, fmtime *cal) {

    double imjd, l, n, m, jj, remainder, intpart;

    imjd = floor(mjd);
    l = floor((4000.*(imjd+18204.))/1461001.);
    n = floor(imjd-((1461.*l)/4.)+18234.);
    m = floor((80.*n)/2447.);
    cal->fm_mday = (int) floor(n-floor((2447.*m)/80.));
    jj = floor(m/11.);
    cal->fm_mon = (int) floor(m+2.-12.*jj);
    cal->fm_year = (int) floor(1900.+l+jj);
    remainder = modf(mjd,&intpart);
    remainder = modf(remainder*24.,&intpart);
    cal->fm_hour = (int) intpart;
    remainder = modf(remainder*60., &intpart);
    cal->fm_min = (int) intpart;
    cal->fm_sec = (int) (remainder*60.);
}

/*
 * Computes the modified julian day (rel. 1950)
 * for calender dates between 1 jan 1950 and 31 dec 2099
 */
void fmtime2mjd1950(fmtime cal, double *mjd) {

    double jj, l;

    jj = floor((14.-((double) cal.fm_mon))/12.);
    l = floor(((double) cal.fm_year)
	    -1900.*floor(((double) cal.fm_year)/1900.)-jj);
    *mjd = floor(((double) cal.fm_mday)-18234.
	    +(1461.*l)/4.+(367.*(((double) cal.fm_mon)-2.+jj*12.))/12.);
    *mjd += (((((double) cal.fm_hour)*60.+((double) cal.fm_min))*60.
		+((double) cal.fm_sec))/86400.);
}

/*
 * NAME:
 * hourangle
 *
 * PURPOSE:
 * To estimate the hour angle.
 *
 * NOTES:
 * This function is based upon the text given in Paltridge and PLatt
 * (1976), page 62. The input is true solar time given as a decimal
 * number i.e. 12:30 is given 12.5.
 *
 * BUGS:
 * NA
 *
 * REQUIREMENTS:
 * NA
 *
 * RETURN VALUES:
 * hour angle in radians.
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 23.02.2004
 */
double fmhourangle(double tst){

    double ha;

    ha = (tst-12.)*0.2618;

    return(ha);
}

/*
 * FUNCTION:
 * convmeosdate
 *
 * PURPOSE:
 * To convert the date in MEOS MPH structure to midate format.
 */
int fmmeosdate2fmtime(char s[], fmtime *d) {

    char *where = "fmmeosdate2fmtime";
    int i;
    char *pt;
    char *token[6] = {"-", "-", " ", ":", ":", "."};

    pt = s;
    strtok(pt, token[0]);
    d->fm_mday = atoi(pt);
    for (i=1; i<6; i++) {
	pt = strtok(NULL, token[i]);
	switch (i) {
	    case (1): {
		if (strcmp(pt, "JAN") == 0) {
		    d->fm_mon = 1;
		} else if (strcmp(pt, "FEB") == 0) {
		    d->fm_mon = 2;
		} else if (strcmp(pt, "MAR") == 0) {
		    d->fm_mon = 3;
		} else if (strcmp(pt, "APR") == 0) {
		    d->fm_mon = 4;
		} else if (strcmp(pt, "MAY") == 0) {
		    d->fm_mon = 5;
		} else if (strcmp(pt, "JUN") == 0) {
		    d->fm_mon = 6;
		} else if (strcmp(pt, "JUL") == 0) {
		    d->fm_mon = 7;
		} else if (strcmp(pt, "AUG") == 0) {
		    d->fm_mon = 8;
		} else if (strcmp(pt, "SEP") == 0) {
		    d->fm_mon = 9;
		} else if (strcmp(pt, "OCT") == 0) {
		    d->fm_mon = 10;
		} else if (strcmp(pt, "NOV") == 0) {
		    d->fm_mon = 11;
		} else if (strcmp(pt, "DEC") == 0) {
		    d->fm_mon = 12;
		} else {
		    fmerrmsg(where,"Time identification of image failed");
		    return(FM_SYNTAX_ERR);
		}
	    }
	    case (2): {
		d->fm_year = atoi(pt);
		break;
	    }
	    case (3): {
		d->fm_hour = atoi(pt);
		break;
	    }
	    case (4): {
		d->fm_min = atoi(pt);
		break;
	    }
	}
    }
    return(FM_OK);
}

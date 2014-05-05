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
 * �ystein God�y, met.no/FOU, 25.02.2004
 *
 * MODIFIED:
 * �ystein God�y, METNO/FOU, 30.08.2010: Modified use of return values to
 * standard values.
 * �ystein God�y, METNO/FOU, 29.09.2010: fmstring2fmtime made public
 * available.
 * �ystein God�y, METNO/FOU, 30.09.2010: added fmsec19702isodate.
 * �ystein God�y, METNO/FOU, 2011-11-01: Added fmmonthnum.
 * Thomas Lavergne, METNO/FOU, 2012-11-23: Added DOY decoding in fmstring2fmtime(), clean-up return codes.
 * Thomas Lavergne, METNO/FOU, 2012-11-29: Add check of date validity in tofmsec1970() and fmstring2fmtime()
 *
 * ID:
 * $Id$
 */

#include <fmutil.h>

/* Accessible to all routines */
static unsigned short int daytab[2][13]=
{{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, 
   {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

/*
 * NAME:
 * is_leap_year
 *
 * PURPOSE:
 * Check if a year is a leap year
 *
 * NOTES:
 *
 * RETURN VALUES:
 *   1 if year is leap, 0 otherwise
 *
 * REQUIREMENTS:
 *
 * AUTHOR:
 * Thomas Lavergne, met.no/FOU, 28.11.2012
 *
 * MODIFIED:
 */

int is_leap_year(int year) {
   int leap = 0;
   if ((year%4 == 0) && 
         (year%100 != 0 || year%400 == 0)) leap=1;
   return leap;
}

/*
 * NAME:
 * check_fmtime
 *
 * PURPOSE:
 * validate the content of an fmtime struct
 *
 * NOTES:
 *
 * RETURN VALUES:
 *   FM_OK if a valid date, FM_IO_ERR otherwise
 *
 * REQUIREMENTS:
 *
 * AUTHOR:
 * Thomas Lavergne, met.no/FOU, 28.11.2012
 *
 * MODIFIED:
 */
int check_fmtime(fmtime *t, int verbose) {

   /* decide if we have a leap year */
   int leap = is_leap_year(t->fm_year);

   if (verbose) {
      fprintf(stdout,"VERBOSE (%s) %04d is%sa leap year\n",__func__,t->fm_year,(leap?" ":" not "));
   }

   /* test month number */
   if (t->fm_mon <= 0 || t->fm_mon > 12) {
      if (verbose) {
         fprintf(stdout,"VERBOSE (%s) %02d is not a valid month\n",__func__,t->fm_mon);
      }
      return FM_IO_ERR;
   }

   /* test day of month */
   if (t->fm_mday <= 0) {
      if (verbose) {
         fprintf(stdout,"VERBOSE (%s) day %02d cannot be negative\n",__func__,t->fm_mday);
      }
      return FM_IO_ERR;
   }
   if (t->fm_mday > daytab[leap][t->fm_mon]) {
      if (verbose) {
         fprintf(stdout,"VERBOSE (%s) %02d is not a valid day for month %02d\n",__func__,
               t->fm_mday,t->fm_mon);
      }
      return FM_IO_ERR;
   }

   /* test hours */
   if (t->fm_hour < 0 || t->fm_hour > 23) {
      if (verbose) {
         fprintf(stdout,"VERBOSE (%s) %02d is not a valid hour\n",__func__,t->fm_hour);
      }
      return FM_IO_ERR;
   }

   /* test minutes */
   if (t->fm_min < 0 || t->fm_min > 59) {
      if (verbose) {
         fprintf(stdout,"VERBOSE (%s) %02d is not a valid minute\n",__func__,t->fm_min);
      }
      return FM_IO_ERR;
   }

   /* test seconds */
   if (t->fm_sec < 0 || t->fm_sec > 59) {
      if (verbose) {
         fprintf(stdout,"VERBOSE (%s) %02d is not a valid number of second\n",__func__,t->fm_sec);
      }
      return FM_IO_ERR;
   }

   return FM_OK;
}

fmsec1970 tofmsec1970(fmtime t) {

   long secs, tmpsecs;
   struct tm tmref, tminp;
   time_t tzdelta;

   /* First check validity of the fmtime object */
   if (FM_OK != check_fmtime(&t,0)) {
      fprintf(stderr,"ERROR (%s) The input time structure contains %04d.%02d.%02d %02d:%02d:%02d which is not a valid date/time.\n",
            __func__,t.fm_year,t.fm_mon,t.fm_mday,t.fm_hour,t.fm_min,t.fm_sec);
      return -1;
   }

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

   fprintf(stdout," Testing fmtime: %li \n",secs);

   tzdelta = mktime(&tmref);

   /*
    * Apply timezone correction.
    */
   //tmpsecs = secs+tzdelta;
   tmpsecs = secs;
   /*
    * Create struct
    */


   tminp = gmtime(&tmpsecs);
   fprintf(stdout," Testing fmtime: %li \n",secs);

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

fmsec1970 ymdhms2fmsec1970_alt(char *str, int mode) {
   fmtime fmt;
   int ret = fmstring2fmtime(str,"YYYYMMDDhhmmss",&fmt);
   if (ret != FM_OK) return(FM_IO_ERR);
   if (mode > 0) {
      printf(" %4d-%02d-%02d %02d:%02d:%02d\n",
            fmt.fm_year,fmt.fm_mon,fmt.fm_mday,
            fmt.fm_hour,fmt.fm_min,fmt.fm_sec);
   }
   return (tofmsec1970(fmt));
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

fmsec1970 ymdh2fmsec1970_alt(char *str, int mode) {
   fmtime fmt;
   int ret = fmstring2fmtime(str,"YYYYMMDDhh",&fmt);
   if (ret != FM_OK) return(FM_IO_ERR);
   if (mode > 0) {
      printf(" %4d-%02d-%02d %02d:%02d:%02d\n",
            fmt.fm_year,fmt.fm_mon,fmt.fm_mday,
            fmt.fm_hour,fmt.fm_min,fmt.fm_sec);
   }
   return (tofmsec1970(fmt));
}

fmsec1970 ymdh2fmsec1970(char *str, int mode) {

   char str_ymdhms[14+1];
   /* Add the mmss 0 values and call ymdhms2fmsec1970() */
   sprintf(str_ymdhms,"%s0000",str);
   return (ymdhms2fmsec1970(str_ymdhms,mode));

}
/*
 * NAME:
 * isodatetime2fmsec1970
 * fmsec19702isodatetime
 *
 * PURPOSE:
 * To convert back and forth between fmsec1970 and an 'iso date&time strings 
 * whose format is :
 *    yyyy-mm-ddThh-mn-scZ 
 *    with litteral 'T' and 'Z'.
 *
 *    Such a string (\0 *not* included) has length FMUTIL_ISODATETIME_LENGTH
 *    (defined in fmtime.h)
 *
 * NOTES:
 *
 * BUGS:
 *
 * RETURN VALUES:
 *
 * REQUIREMENTS:
 * tofmtime()
 *
 * AUTHOR:
 * Thomas Lavergne, met.no/FOU, 19.12.2007
 * MODIFIED:
 * Thomas Lavergme, met.no/FOU, 25.02.2008 (and 26)  :  Correct a bug in isodatetime2fmsec1970().
 */
void fmsec19702isodatetime(fmsec1970 secs, char *str) {
   /* format is yyyy-mm-ddThh:mn:scZ with litteral 'T' and 'Z' */
   /* the output string is not allocated and should thus be large engouh
    * to hold 20 characters (+ 1 for '\0') */

   fmtime fmt;
   tofmtime(secs,&fmt);
   sprintf(str,"%04d-%02d-%02dT%02d:%02d:%02dZ",
         fmt.fm_year, fmt.fm_mon, fmt.fm_mday,
         fmt.fm_hour, fmt.fm_min, fmt.fm_sec);

}

void fmsec19702isodate(fmsec1970 secs, char *str) {
   /*
    * The output format is YYYY-MM-DD 
    * the output string is not allocated and should thus be large engouh
    * to hold 11 characters (including '\0') 
    * �ystein God�y, METNO/FOU, 30.09.2010 
    */

   fmtime fmt;
   tofmtime(secs,&fmt);
   sprintf(str,"%04d-%02d-%02d",
         fmt.fm_year, fmt.fm_mon, fmt.fm_mday);

}

fmsec1970 isodatetime2fmsec1970_alt(char *str) {
   fmtime fmt;
   int ret = fmstring2fmtime(str,"YYYY-MM-DDThh:mm:ssZ",&fmt);
   if (ret != FM_OK) return(FM_IO_ERR);
   return (tofmsec1970(fmt));
}

fmsec1970 isodatetime2fmsec1970(char *str) {
   /* format is yyyy-mm-ddThh:mn:scZ with litteral 'T' and 'Z' */
   unsigned int nbtokens;
   char **tokens;

   if (strlen(str) != FMUTIL_ISODATETIME_LENGTH) return -1;

   //printf("str[strlen(str)]=<%d>\n",str[strlen(str)]);

   /* split date and time information */
   fmstrsplit(str,"T",&nbtokens,&tokens);
   if (nbtokens != 2) return -1;
   char *datestr,*timestr;
   datestr = tokens[0]; timestr = tokens[1];

   char **dtokens;
   /* split year, month, day */
   fmstrsplit(datestr,"-",&nbtokens,&dtokens);
   if (nbtokens != 3) return -1;
   char *year  = dtokens[0];
   char *month = dtokens[1];
   char *day   = dtokens[2];

   char **ttokens;
   /* split hour, minutes, seconds */
   fmstrsplit(timestr,":",&nbtokens,&ttokens);
   if (nbtokens != 3) return -1;
   char *hour  = ttokens[0];
   char *min   = ttokens[1];
   char *sec   = ttokens[2];


   /* check that the last character in sec is the litteral 'Z' */
   if (sec[strlen(sec)-1] != 'Z') return -1;
   if (strlen(sec) != 3) return -1;
   char msec[2+1];
   strncpy(msec,sec,2);msec[2] = '\0';

   /*
      printf("FMUTIL/FMTIME : hour=<%s>, min=<%s>, msec=<%s>. strlen(msec)=%u\n",hour,min,msec,
      (unsigned int)strlen(msec));
      short e;
      for (e=0; e < strlen(msec)+1 ; e++)
      printf("msec[%d]='%c' <%d>\n",e,msec[e],msec[e]);
      */

   /* check the length of all those character strings */
   if (  (strlen(year)  != 4) ||
         (strlen(month) != 2) ||
         (strlen(day)   != 2) ||
         (strlen(hour)  != 2) ||
         (strlen(min)   != 2) ||
         (strlen(msec)  != 2) 
      ) return -1;

   /* convert the strings to integers */
   char *dummy;
#define convertStrToInt(s) int i##s = strtol(s,&dummy,10); if (*dummy != '\0') return -1;
   convertStrToInt(year)
      convertStrToInt(month)
      convertStrToInt(day)
      convertStrToInt(hour)
      convertStrToInt(min)
      convertStrToInt(msec)
#undef convertStrToInt

      /* free all character buffers */
      free(year);free(month);free(day);
   free(hour);free(min);free(sec);
   free(dtokens);free(ttokens);
   free(datestr);free(timestr);
   free(tokens);

   /* fill a fmtime structure and convert to fmsec1970 */
   fmtime t;
   t.fm_year = iyear;
   t.fm_mon  = imonth;
   t.fm_mday = iday;
   t.fm_hour = ihour;
   t.fm_min  = imin;
   t.fm_sec  = imsec;


   return (tofmsec1970(t));
}

/*
 * NAME:
 * CFepoch2fmsec1970
 * fmsec19702CFepoch
 *
 * PURPOSE:
 * To convert back and forth between fmsec1970 and an 'CF epoch' date&time string 
 * whose format is :
 *    yyyy-mm-dd hh:mn:sc 
 *
 *    Such a string (\0 *not* included) has length FMUTIL_CFEPOCH_LENGTH
 *    (defined in fmtime.h)
 *
 * NOTES:
 *
 * BUGS:
 *
 * RETURN VALUES:
 *
 * REQUIREMENTS:
 *   fmstring2fmtime()
 *
 * AUTHOR:
 * Thomas Lavergne, met.no/FOU, 12.01.2009
 *
 * MODIFIED:
 *
 */
fmsec1970 CFepoch2fmsec1970(char *str) {
   fmtime fmt;
   int ret = fmstring2fmtime(str,"YYYY-MM-DD hh:mm:ss",&fmt);
   if (ret != FM_OK) return(FM_IO_ERR);
   return (tofmsec1970(fmt));
}

void fmsec19702CFepoch(fmsec1970 secs, char *str) {
   /* format is yyyy-mm-dd hh:mn:ss */
   /* the output string is not allocated and should thus be large engouh to hold
    * 19 characters (+ 1 for '\0') (FMUTIL_CFEPOCH_LENGTH+1) */

   fmtime fmt;
   tofmtime(secs,&fmt);
   sprintf(str,"%04d-%02d-%02d %02d:%02d:%02d",
         fmt.fm_year, fmt.fm_mon, fmt.fm_mday,
         fmt.fm_hour, fmt.fm_min, fmt.fm_sec);
}

/*
 * NAME:
 * correct_time_from_unit
 *
 * PURPOSE:
 * To correct a time value which is 'amount from epoch' into seconds from 1 January 1970. 
 *    'amount' could be 'seconds', 'hours', etc...
 *
 * NOTES:
 *  o Currently only implemented with 'seconds';
 *  o epoch must be formatted as yyyy-mm-dd hh:mn:ss
 *
 * BUGS:
 *
 * RETURN VALUES:
 *   1 if an error occurs.
 *
 * REQUIREMENTS:
 *
 * AUTHOR:
 * Thomas Lavergne, met.no/FOU, 15.04.2010
 *
 * MODIFIED:
 *
 */
int correct_time_from_unit(double amount_from_epoch, char *unit_string,fmsec1970 *secsFrom1970) {

   char known_unit[] = "seconds since";
   char *start_refDate;
   if (!(start_refDate = strstr(unit_string,known_unit))) {
      fprintf(stderr,"ERROR (%s) can only handle units that are <%s> (is <%s>).\n",__func__,known_unit,unit_string);
      return 1;
   }
   start_refDate  = strpbrk(start_refDate,"0123456789");
   if (!start_refDate) {
      fprintf(stderr,"ERROR (%s) could not find the epoch date.\n",__func__);
      return 1;
   }
   fmsec1970 refSecs = CFepoch2fmsec1970(start_refDate);
   if (refSecs == -1) {
      fprintf(stderr,"ERROR (%s) cannot transform Epoch string <%s>. Should be yyyy-mm-dd hh:mn:ss\n",__func__,start_refDate);
      return 1;
   }   
   *secsFrom1970 = (fmsec1970)round(amount_from_epoch) + refSecs;
   return 0;
}


/*
 * NAME:
 *   fmstring2fmtime
 *
 * PURPOSE:
 *   To convert from an arbitrarily formatted date-and-time string (+ a
 *   given format string) to a fmtime structure. It is not intended for a
 *   direct usage by the user of fmutil but rather as a common way to code
 *   functions like isodatetime2fmsec1970 or ymdh2fmsec1970
 *
 * NOTES:
 *   1) The separators are not checked. It means that:
 *         fmstring2fmtime("2008-01-03 12:34:56","YYYY-MM-DD hh:mm:ss",&time);
 *      will work as good as:
 *         fmstring2fmtime("2008/01/03 12.34.56","YYYY-MM-DD hh:mm:ss",&time);
 *   2) The fields which are not provided by the format are set to 0.
 *
 * BUGS:
 *
 * RETURN VALUES:
 *    FM_OK upon success.
 *    if (FM_OK != fmstring2fmtime(arg1,arg2,&fmt)) {
 *       PRINT-ERROR-MSG;
 *    }
 *
 * REQUIREMENTS:
 *
 * AUTHOR:
 * Thomas Lavergne, met.no/FOU, 12.09.2008
 * MODIFIED:
 * Thomas Lavergne, met.no/FOU, 12.09.2008      
 * Add a test to check that the date and format strings have the same
 * length
 * Thomas Lavergne, met.no/FOU, 05.11.2009      
 * Add consitency check on the yaer, month, day, etc...
 * �ystein God�y, METNO/FOU, 30.08.2010: Changed return value to be
 * consistent with other functions.
 * Thomas Lavergne, METNO/FOU, 23.11.2012: Clean-up mess in return values.
 * Thomas Lavergne, METNO/FOU, 23.11.2012: Add DOY format
 * Thomas Lavergne, METNO/FOU, 29.11.2012: Apply check_fmtime() before returning
 *
 */
int fmstring2fmtime(char date[], char fmt[], fmtime *res) {


   if (strlen(date) != strlen(fmt)) {
      fprintf(stderr,"ERROR (%s) strings <%s> and <%s> should have the same length.\n",
            __func__,date,fmt);
      return FM_IO_ERR;
   }

   int dyear,dmonth,dday,dhour,dmint,dsecs;

   /* initialize all the fmtime fields to zero */
   dyear    = 0;
   dmonth   = 0;
   dday     = 0;
   dhour    = 0;
   dmint    = 0;
   dsecs    = 0;

   /* find the year. */
   char *start_year4 = NULL;
   start_year4 = strstr(fmt,"YYYY");
   if (start_year4) {
      char year[4+1];year[4]='\0';
      strncpy(year,date+(start_year4-fmt),4);
      long lyear;
      int ok = convertString2Long(year,&lyear);
      if (!ok || (lyear < 1970)) {fprintf(stderr,"ERROR (%s) Invalid year <%s> in %s date-string.\n",__func__,year,fmt); return FM_IO_ERR;}
      dyear = (int)lyear;
   }

   /* find the month. */
   char *start_month2 = NULL;
   start_month2 = strstr(fmt,"MM");
   if (start_month2) {
      char month[2+1];month[2]='\0';
      strncpy(month,date+(start_month2-fmt),2);
      long lmonth;
      int ok = convertString2Long(month,&lmonth);
      if (!ok || (lmonth <= 0) || (lmonth >= 13)) {fprintf(stderr,"ERROR (%s) <%s> Invalid month <%s> in %s date-string.\n",__func__,date,month,fmt); return FM_IO_ERR;}
      dmonth = (int)lmonth;
   }

   /* find the day. */
   char *start_day2 = NULL;
   start_day2 = strstr(fmt,"DD");
   if (start_day2) {
      char day[2+1];day[2]='\0';
      strncpy(day,date+(start_day2-fmt),2);
      long lday;
      int ok = convertString2Long(day,&lday);
      if (!ok || (lday <= 0) || (lday > 31)) {fprintf(stderr,"ERROR (%s) Invalid day <%s> in %s date-string.\n",__func__,day,fmt); return FM_IO_ERR;}
      dday = (int)lday;
   }

   /* look for the DOY */
   char *start_doy3 = NULL;
   start_doy3 = strstr(fmt,"DOY");
   /* it is an error to have both the DOY and MM or DD */
   if (start_doy3 && (start_day2 || start_month2)) {
      fprintf(stderr,"ERROR (%s) Invalid format string %s: cannot have both DOY and MM or DD .\n",
            __func__,fmt); 
      return FM_IO_ERR;
   }
   /* decode DOY */
   if (start_doy3) {
      char day[3+1];day[3]='\0';
      strncpy(day,date+(start_doy3-fmt),3);
      long lday;
      int ok = convertString2Long(day,&lday);
      if (!ok) {fprintf(stderr,"ERROR (%s) Invalid doy <%s> in %s date-string.\n",__func__,day,fmt); return FM_IO_ERR;}
      int err = fmdoy2mmdd(dyear,(int)lday,&dmonth,&dday);
      if (err) {
         fprintf(stderr,"ERROR (%s) Cannot use doy %ld.\n",__func__,lday); 
         return FM_IO_ERR;
      }
   }

   /* find the hour. */
   char *start_hour2 = NULL;
   start_hour2 = strstr(fmt,"hh");
   if (start_hour2) {
      char hour[2+1];hour[2]='\0';
      strncpy(hour,date+(start_hour2-fmt),2);
      long lhour;
      int ok = convertString2Long(hour,&lhour);
      if (!ok || (lhour < 0) || (lhour >= 24)) {fprintf(stderr,"ERROR (%s) Invalid hour <%s> in %s date-string.\n",__func__,hour,fmt); return FM_IO_ERR;}
      dhour = (int)lhour;
   }

   /* find the min. */
   char *start_mint2 = NULL;
   start_mint2 = strstr(fmt,"mm");
   if (start_mint2) {
      char mint[2+1];mint[2]='\0';
      strncpy(mint,date+(start_mint2-fmt),2);
      long lmint;
      int ok = convertString2Long(mint,&lmint);
      if (!ok || (lmint < 0) || (lmint >= 60)) {fprintf(stderr,"ERROR (%s) Invalid minutes <%s> in %s date-string.\n",__func__,mint,fmt); return FM_IO_ERR;}
      dmint = (int)lmint;
   }

   /* find the secs. */
   char *start_secs2 = NULL;
   start_secs2 = strstr(fmt,"ss");
   if (start_secs2) {
      char secs[2+1];secs[2]='\0';
      strncpy(secs,date+(start_secs2-fmt),2);
      long lsecs;
      int ok = convertString2Long(secs,&lsecs);
      if (!ok || (lsecs < 0) || (lsecs >= 60)) {fprintf(stderr,"ERROR (%s) Invalid seconds <%s> in %s date-string.\n",__func__,secs,fmt); return FM_IO_ERR;}
      dsecs = (int)lsecs;
   }

   /*
      printf("YEAR: %d, MONTH: %d, DAY: %d, HOUR: %d, MIN: %d, SEC: %d\n",
      dyear,dmonth,dday,dhour,dmint,dsecs);
      */

   /* fill the fmtime structure's fields */
   res->fm_year  = dyear;
   res->fm_mon   = dmonth;
   res->fm_mday  = dday;
   res->fm_hour  = dhour;
   res->fm_min   = dmint;
   res->fm_sec   = dsecs;

   /* apply check_fmtime() and return */
   if (FM_OK != check_fmtime(res,0)) {
      fprintf(stderr,"ERROR (%s) String %s does not correspond to a valid date/time\n",__func__,date); 
      return FM_IO_ERR;
   }

   return(FM_OK);
}



/*
 * NAME:
 * fmutc2tst
 * fmcet2tst
 * fmloncorr
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
 * �ystein God�y, met.no/FOU, 25.02.2004
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
 * Modified for C by �ystein God�y, met.no/FOU, 28.04.2004
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
 * To estimate the hour angle, i.e the rotation needed to put the Sun in
 * Zenith.
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
 * �ystein God�y, met.no/FOU, 23.02.2004
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

/*
 * Function to convert monthnames to number for various ways of writing
 * the name. Returns monthnumber in the range 1-12.
 * �ystein God�y, METNO/FOU, 2011-11-01 
 */
int fmmonthnum(char *monthname) {
   char *where="fmmonthnum";
   char *monthnames[12]={
      "January","February","March","April","May","June","July","August","September","October","November","December"
   };
   int i, j;

   for (i=0;i<12;i++) {
      printf("%s \n", monthnames[i]);
      if (strstr(monthnames[i],monthname)) {
         return(i++);
      }
   }

   fmerrmsg(where,"Could not recognise the monthname submitted.");
   return(FM_IO_ERR);
}

/*
 * FUNCTION:
 * fmdoy2mmdd
 * 
 * PURPOSE:
 * Computes Month and Day number from a Day of year (1-366) 
 * 
 * RETURN VALUES:
 * FM_IO_ERR upon error.
 *
 * AUTHOR:
 * Thomas Lavergne, met.no/FOU, 23/11/2012
 */
int fmdoy2mmdd(int year, int doy, int *m2, int *d2) {

   int doy_bck = doy;

   /* check input parameters */
   if ((doy <= 0) || (year <= 0)) {
      fprintf(stderr,"ERROR (%s) nor Year (%d), nor DOY (%d) can be negative.\n",
            __func__,year,doy);
      return FM_IO_ERR;
   }
   int max_doy = 365;

   if (is_leap_year(year)) {
      /* leap year */
      max_doy++;
      if (doy > 59) doy--;
   }
   if (doy > max_doy) {
      fprintf(stderr,"ERROR (%s) invalid DOY value (%d) for year %d\n",
            __func__,doy,year);
      return FM_IO_ERR;
   }

   /* start conversion */
   int mm = 12;
   int dd = doy - 334;
   if (doy <= 334) {mm = 11; dd = doy - 304;}
   if (doy <= 304) {mm = 10; dd = doy - 273;}
   if (doy <= 273) {mm =  9; dd = doy - 243;}
   if (doy <= 243) {mm =  8; dd = doy - 212;}
   if (doy <= 212) {mm =  7; dd = doy - 181;}
   if (doy <= 181) {mm =  6; dd = doy - 151;}
   if (doy <= 151) {mm =  5; dd = doy - 120;}
   if (doy <= 120) {mm =  4; dd = doy -  90;}
   if (doy <=  90) {mm =  3; dd = doy -  59;}
   if (doy <=  59) {mm =  2; dd = doy_bck -  31;}
   if (doy <=  31) {mm =  1; dd = doy -   0;}

   *m2 = mm;
   *d2 = dd;

   return(FM_OK);
}

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
 * �ystein God�y, DNMI/FOU, 14/12/2001
 *
 * MODIFIED:
 * TL, met.no/FOU, 28/11/2012   :   Move definition of daytab[][] at file level
 * TL, met.no/FOU, 28/11/2012   :   Use routine is_leap_year()
 *
 */
int fmdayofyear(fmtime md) {

   unsigned short int i, leap;
   int dn;

   leap = is_leap_year(md.fm_year);

   dn = md.fm_mday;
   for (i=1; i<md.fm_mon; i++) {
      dn += (int) daytab[leap][i];
   }

   return(dn);
}

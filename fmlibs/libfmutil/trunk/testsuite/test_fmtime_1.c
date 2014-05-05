/*
 * NAME:
 * test_fmtime_1
 *
 * PURPOSE:
 * Test some routines/tools from fmtime.c/.h
 *
 * NOTES:
 * NA
 *
 * REQUIREMENTS:
 * NA
 *
 * AUTHOR:
 * Thomas Lavergne, met.no/FoU, 28.11.2012
 *
 * MODIFIED:
 *
 * ID:
 * $Id: $
 */

#include <stdlib.h>
#include <fmutil.h>

char progname[] = "test_fmtime_1";

int main(void) {

   /* 1/ test decoding date from a datestring */
   char date_1[] = "2008-04-12T01:02:53Z";
   printf("\t(%s) 01. Decode %s with fmstring2fmtime()\n",progname,date_1);
   fmtime fmt_1;
   if (FM_OK != fmstring2fmtime(date_1,"YYYY-MM-DDThh:mm:ssZ",&fmt_1)) {
      printf("\t\t(%s) 01. fmstring2fmtime() does not return with FM_OK\n",progname);
      exit(EXIT_FAILURE);
   }
   if (fmt_1.fm_year != 2008) {
      printf("\t\t(%s) 01. Year is wrong (%d)\n",progname,fmt_1.fm_year);
      exit(EXIT_FAILURE);
   }
   if (fmt_1.fm_mon != 4) {
      printf("\t\t(%s) 01. Month is wrong (%d)\n",progname,fmt_1.fm_mon);
      exit(EXIT_FAILURE);
   }
   if (fmt_1.fm_mday != 12) {
      printf("\t\t(%s) 01. Day is wrong (%d)\n",progname,fmt_1.fm_mday);
      exit(EXIT_FAILURE);
   }
   if (fmt_1.fm_hour != 1) {
      printf("\t\t(%s) 01. Hours is wrong (%d)\n",progname,fmt_1.fm_hour);
      exit(EXIT_FAILURE);
   }
   if (fmt_1.fm_min != 2) {
      printf("\t\t(%s) 01. Minutes is wrong (%d)\n",progname,fmt_1.fm_min);
      exit(EXIT_FAILURE);
   }
   if (fmt_1.fm_sec != 53) {
      printf("\t\t(%s) 01. Seconds is wrong (%d)\n",progname,fmt_1.fm_sec);
      exit(EXIT_FAILURE);
   }
   fmsec1970 t_1 = tofmsec1970(fmt_1);
   if (t_1 != 1207962173) {
      printf("\t\t(%s) 01. tofmsec1970() does not return expected value (returned %ld)\n",
            progname,t_1);
      exit(EXIT_FAILURE);
   }
   /* 1/ Done */

   /* 2/ test sanity checks while decoding date from a datestring */
   char date_2[] = "2001-02-21T01:02:53Z";
   printf("\t(%s) 02. Decode %s with fmstring2fmtime() [EXPECTS AN ERROR]\n",progname,date_2);
   fmtime fmt_2;
   if (FM_OK == fmstring2fmtime(date_2,"YYYY-MM-DDThh:mm:ssZZZZZ",&fmt_2)) {
      printf("\t\t(%s) 02. fmstring2fmtime() returns with FM_OK\n",progname);
      exit(EXIT_FAILURE);
   }
   /* 2/ Done */

   /* 3/ test decoding date from a datestring (using DOY)*/
   char date_3[] = "098@1987";
   printf("\t(%s) 03. Decode %s with fmstring2fmtime()\n",progname,date_3);
   fmtime fmt_3;
   if (FM_OK != fmstring2fmtime(date_3,"DOY@YYYY",&fmt_3)) {
      printf("\t\t(%s) 03 fmstring2fmtime() does not return with FM_OK\n",progname);
      exit(EXIT_FAILURE);
   }
   if (fmt_3.fm_year != 1987) {
      printf("\t\t(%s) 03 Year is wrong (%d)\n",progname,fmt_3.fm_year);
      exit(EXIT_FAILURE);
   }
   if (fmt_3.fm_mon != 4) {
      printf("\t\t(%s) 03 Month is wrong (%d)\n",progname,fmt_3.fm_mon);
      exit(EXIT_FAILURE);
   }
   if (fmt_3.fm_mday != 8) {
      printf("\t\t(%s) 03 Day is wrong (%d)\n",progname,fmt_3.fm_mday);
      exit(EXIT_FAILURE);
   }
   fmsec1970 t_3 = tofmsec1970(fmt_3);
   if (t_3 != 544838400) {
      printf("\t\t(%s) 03. tofmsec1970() does not return expected value (returned %ld)\n",
            progname,t_3);
      exit(EXIT_FAILURE);
   }
   /* 3/ Done */

   /* 4/ test decoding date from a datestring (using DOY)*/
   char date_4[] = "328@2012";
   printf("\t(%s) 04. Decode %s with fmstring2fmtime()\n",progname,date_4);
   fmtime fmt_4;
   if (FM_OK != fmstring2fmtime(date_4,"DOY@YYYY",&fmt_4)) {
      printf("\t\t(%s) 03 fmstring2fmtime() does not return with FM_OK\n",progname);
      exit(EXIT_FAILURE);
   }
   if (fmt_4.fm_year != 2012) {
      printf("\t\t(%s) 03 Year is wrong (%d)\n",progname,fmt_4.fm_year);
      exit(EXIT_FAILURE);
   }
   if (fmt_4.fm_mon != 11) {
      printf("\t\t(%s) 03 Month is wrong (%d)\n",progname,fmt_4.fm_mon);
      exit(EXIT_FAILURE);
   }
   if (fmt_4.fm_mday != 23) {
      printf("\t\t(%s) 03 Day is wrong (%d)\n",progname,fmt_4.fm_mday);
      exit(EXIT_FAILURE);
   }
   fmsec1970 t_4 = tofmsec1970(fmt_4);
   if (t_4 != 1353628800) {
      printf("\t\t(%s) 04. tofmsec1970() does not return expected value (returned %ld)\n",
            progname,t_4);
      exit(EXIT_FAILURE);
   }
   /* 4/ Done */

   /* 5/ test decoding a wrong date from a datestring */
   char date_5[] = "2007-02-29";
   printf("\t(%s) 05. Decode %s with fmstring2fmtime() [EXPECTS AN ERROR]\n",progname,date_5);
   fmtime fmt_5;
   if (FM_OK == fmstring2fmtime(date_5,"YYYY-MM-DD",&fmt_5)) {
      printf("\t\t(%s) 05. fmstring2fmtime() does not return with FM_IO_ERR\n",progname);
      exit(EXIT_FAILURE);
   }
   /* 5/ Done */

   /* 6/ test transforming an invalid fmtime to fmsec1970 */
   fmtime fmt_6;
   fmt_6.fm_year = 2007;
   fmt_6.fm_mon  = 13;
   fmt_6.fm_mday = 1;
   fmt_6.fm_hour = fmt_6.fm_min = fmt_6.fm_sec = 0;
   printf("\t(%s) 06. Transform an invalid time object to fmsec1970 [EXPECTS AN ERROR]\n",progname);
   fmsec1970 t_6 = tofmsec1970(fmt_6);
   if (-1 != t_6) {
      printf("\t\t(%s) 06. tofmsec1970() does not return -1 on invalid time objects\n",progname);
      exit(EXIT_FAILURE);
   }
   /* 6/ Done */

   /* 7/ test transforming a valid fmtime to fmsec1970 */
   fmtime fmt_7;
   fmt_7.fm_year = 2033;
   fmt_7.fm_mon  = 5;
   fmt_7.fm_mday = 18;
   fmt_7.fm_hour = 3;
   fmt_7.fm_min  = 33;
   fmt_7.fm_sec  = 20;
   printf("\t(%s) 07. Transform a valid time object to fmsec1970\n",progname);
   fmsec1970 t_7 = tofmsec1970(fmt_7);
   if (2000000000 != t_7) {
      printf("\t\t(%s) 07. tofmsec1970() did not find correct value (2,000,000,000)\n",progname);
      exit(EXIT_FAILURE);
   }
   /* 7/ Done */

   exit(EXIT_SUCCESS);
}

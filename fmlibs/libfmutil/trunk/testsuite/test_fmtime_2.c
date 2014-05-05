/*
 * NAME:
 * test_fmtime_2
 *
 * PURPOSE:
 * Test some more routines/tools from fmtime.c/.h
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
 * TL, met.no/FoU, 29.11.2012   :    add test of yyyymmddhh...2fmsec1970()
 *
 * ID:
 * $Id: $
 */

#include <stdlib.h>
#include <fmutil.h>

char progname[] = "test_fmtime_2";

int main(void) {

   /* 1/ test decoding date to/from a datestring */
   char datestr_1[FMUTIL_ISODATETIME_LENGTH+1];
   fmsec1970 t_1 = 1234567890;
   printf("\t(%s) 01. Decode %ld with fmsec19702isodatetime()\n",progname,t_1);
   fmsec19702isodatetime(t_1,datestr_1);
   if (strcmp(datestr_1,"2009-02-13T23:31:30Z")) {
      printf("\t\t(%s) 01. fmsec19702isodatetime() does not return expected string (returned %s)\n",
            progname,datestr_1);
      exit(EXIT_FAILURE);
   }
   fmsec1970 t_1_1 = isodatetime2fmsec1970(datestr_1);
   if (t_1_1 != t_1) {
      printf("\t\t(%s) 01. isodatetime2fmsec1970() did not manage to re-tranform %s to %ld (got %ld instead)\n",
            progname,datestr_1,t_1,t_1_1);
      exit(EXIT_FAILURE);
   }
   fmsec1970 t_1_2 = isodatetime2fmsec1970(datestr_1);
   if (t_1_2 != t_1) {
      printf("\t\t(%s) 01. isodatetime2fmsec1970_alt() did not manage to re-tranform %s to %ld (got %ld instead)\n",
            progname,datestr_1,t_1,t_1_2);
      exit(EXIT_FAILURE);
   }
   /* 1/ Done */

   /* 2/ test decoding date to/from a datestring */
   char datestr_2[FMUTIL_CFEPOCH_LENGTH+1];
   fmsec1970 t_2 = 1000000000;
   printf("\t(%s) 02. Decode %ld with fmsec19702CFepoch()\n",progname,t_2);
   fmsec19702CFepoch(t_2,datestr_2);
   if (strcmp(datestr_2,"2001-09-09 01:46:40")) {
      printf("\t\t(%s) 02. fmsec19702CFepoch() does not return expected string (returned %s)\n",
            progname,datestr_2);
      exit(EXIT_FAILURE);
   }
   fmsec1970 t_2_1 = CFepoch2fmsec1970(datestr_2);
   if (t_2_1 != t_2) {
      printf("\t\t(%s) 02. CFepoch2fmsec1970() did not manage to re-tranform %s to %ld (got %ld instead)\n",
            progname,datestr_2,t_2,t_2_1);
      exit(EXIT_FAILURE);
   }
   /* 2/ Done */

   /* 3/ test decoding date to/from a datestring */
   char datestr_3[] = "20050318015831";
   printf("\t(%s) 03. Decode %s with ymdhms2fmsec1970()\n",progname,datestr_3);
   fmsec1970 t_3 = ymdhms2fmsec1970(datestr_3,0);
   if ( 1111111111 != t_3 ) {
      printf("\t\t(%s) 03. ymdhms2fmsec1970() does not return expected string (returned %ld)\n",
            progname,t_3);
      exit(EXIT_FAILURE);
   }
   fmsec1970 t_3_1 = ymdhms2fmsec1970_alt(datestr_3,0);
   if (t_3_1 != t_3) {
      printf("\t\t(%s) 03. ymdhms2fmsec1970_alt() does not agree with ymdhms2fmsec1970() (returned %ld)\n",
            progname,t_3_1);
      exit(EXIT_FAILURE);
   }
   /* 3/ Done */

   exit(EXIT_SUCCESS);
}

/*
 * NAME:
 * test_fmtime_3
 *
 * PURPOSE:
 * Test some more routines/tools from fmtime.c/.h, 
 *    especially the DOY conversions
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

char progname[] = "test_fmtime_3";

int main(void) {

   printf("\t(%s) 01. Decode back and forth between Day of Year and Day of Month:\n",progname);
   int years[] = {1981, 2008};
   for (int y = 0 ; y < 2 ; y++) {
      printf("\t\t(%s) 01. Year %04d...\n",progname,years[y]);
      int max_days = 365;
      int leap = is_leap_year(years[y]);
      if (leap) max_days++;

      if (y == 0 && leap) {
         printf("\t\t(%s) 01. ERROR: is_leap_year() thinks %04d is leap\n",progname,years[y]);
         exit(EXIT_FAILURE);
      }
      if (y == 1 && !leap) {
         printf("\t\t(%s) 01. ERROR: is_leap_year() does nos see that %04d is leap\n",progname,years[y]);
         exit(EXIT_FAILURE);
      }
      for (int doy = 1 ; doy < max_days ; doy++) {
         /* convert to fmtime structure */
         fmtime fmt;
         int mm,dd;
         if (FM_OK != fmdoy2mmdd(years[y],doy,&mm,&dd)) {
            printf("\t\t(%s) 01. ERROR: fmdoy2mmdd() fails to decode doy %03d (with year %04d)\n",
                  progname,doy,years[y]);
            exit(EXIT_FAILURE);
         }
         fmt.fm_year = years[y];
         fmt.fm_mon  = mm;
         fmt.fm_mday = dd;
         fmt.fm_hour = fmt.fm_min = fmt.fm_sec = 0;
         /* validate this time */
         if (FM_OK != check_fmtime(&fmt,0)) {
            printf("\t\t(%s) 01. ERROR: check_fmtime() does not validate %04d-%03d\n",
                  progname,years[y],doy);
            exit(EXIT_FAILURE);
         }
         /* get to fmsec1970 */
         fmsec1970 t_1 = tofmsec1970(fmt);
         if (t_1 < 0) {
            printf("\t\t(%s) 01. ERROR: tofmsec1970() returns %ld on %04d-%03d\n",
                  progname,t_1,years[y],doy);
            exit(EXIT_FAILURE);
         }
         /* compute a date string */
         char datestr[FMUTIL_CFEPOCH_LENGTH+1];
         fmsec19702CFepoch(t_1,datestr);

         /* compute a doy from the structure */
         int doy_1 = fmdayofyear(fmt);
         if (doy_1 != doy) {
            printf("\t\t(%s) 01. ERROR: fmdayofyear() <%s> did not return expected DOY (got %d instead of %d)\n",
                  progname,datestr,doy_1,doy);
            exit(EXIT_FAILURE);
         }
      }
   }

   exit(EXIT_SUCCESS);
}

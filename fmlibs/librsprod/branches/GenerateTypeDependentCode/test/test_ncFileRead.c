/*
 * NAME: test_ncFileRead.c
 * 
 * PURPOSE: 
 *    Test program to check high-level netcdf interface, particularly the nc_file_read() routine
 *
 * DESCRIPTION:
 *    Datasets, and attributes are read from a netCDF file
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.10.2010
 *
 * MODIFIED:
 *
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "../src/rsprod.h"

char srcFile[] = "test_ncFileRead.c";


int main(int argc, char *argv[]) {

   int ret;

   char *progname = malloc(strlen(argv[0])+1);
   if (!progname) {
      fprintf(stderr,"ERROR (%s) memory allocation problem.\n",srcFile);
      exit(EXIT_FAILURE);
   }
   sprintf(progname,"%s",argv[0]);;

   printf("<DESCRIPTION>\n");
   printf("Test program [%s]\n",progname);
   printf("Compiled from source file [%s]\n",srcFile);
   printf("SYNOPSIS:\n");
   printf("\tRead datasets and attributes from a netCDF file,\n");
   printf("\tusing nc_file_read().\n");
   printf("<START RUNNING>\n");

   char ncName[1028];
   
   librsprod_echo_mode = LIBRSPROD_QUIET;
   /* Read an ice concentration product file */
   sprintf(ncName,"./data/ice_conc_nh_201010071200.nc");
   printf("VERBOSE: read from file <%s>\n",ncName);
   size_t n_conc;
   float  *c_lat,*c_lon;
   float  *conc,conc_fv;
   int ok = nc_file_read(ncName,"lat::f lon::f ice_conc::# ice_conc::f ice_conc:_FillValue:f",&c_lat,&c_lon,&n_conc,&conc,&conc_fv);
   if (!ok) {
      printf("ERROR: Failed to use nc_file_read() on file <%s>\n",ncName);
      exit(EXIT_FAILURE);
   }
   librsprod_echo_mode = LIBRSPROD_QUIET;

   printf("Ice Conc FV: %f\n",conc_fv);
   int size_line = 0;
   for (size_t e = 0 ; e < n_conc ; e++) {
      if ( (c_lat[e] < 86) || (c_lon[e] < -90) || (c_lon[e] > +90) ) continue;
      char line[20];
      if (conc[e] == conc_fv) {
         sprintf(line,"_ , ");
         printf(line);
         size_line += strlen(line);
      } else {
         sprintf(line,"%5.1f , ",conc[e]);
         printf(line);
         size_line += strlen(line);
      }
      if (size_line >= 60) {
         size_line = 0;
         printf("\n");
      }
   }
   printf("\n");

   /* release memory */
   free(c_lat); free(c_lon); free(conc);

   printf("<END %s>\n",progname);
   return(EXIT_SUCCESS);
}

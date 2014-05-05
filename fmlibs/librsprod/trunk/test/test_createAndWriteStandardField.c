/*
 * NAME: test_createAndWriteStandardField.c
 * 
 * PURPOSE: 
 *    Test program to check/demonstrate the creation of a 'standard' dataset
 *    and its output to a netcdf file.
 *
 * DESCRIPTION:
 *    Two 3x5 datasets (1 float and 1 short) are created and subsequentely written
 *    to a netcdf file.
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 15.04.2008
 *
 * MODIFIED:
 *    TL, met.no, 23.04.2008  : link to the proper 'rsprod.h' header file
 *
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "../src/rsprod.h"

char srcFile[] = "test_createAndWriteStandardField.c";

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
   printf("\tCreate 2 3x5 datasets (1 float and 1 short) and write them \n");
   printf("\tto a netCDF file.\n");
   printf("<START RUNNING>\n");

   /* we want to create two 3 x 5 field with type float and short, respectively */
   size_t nx = 3;
   size_t ny = 5;
   size_t ne = nx * ny;

   /* allocate the data field (arrays) and fill them with dummy values */
   float fillvalue_f  = -99.;
   short fillvalue_s  = -50;

   float *field1data = malloc(sizeof(float)*ne);
   short *field2data = malloc(sizeof(short)*ne);
   if (!field1data || !field2data) {
      fprintf(stderr,"ERROR (%s) memory allocation problem.\n",progname);
      exit(EXIT_FAILURE);
   }

   for (size_t e = 0 ; e < ne ; e++) {
      if (e != 8) {
	 field1data[e] = e + 0.5;
	 field2data[e] = e - 8;
      } else {
	 field1data[e] = fillvalue_f;
	 field2data[e] = fillvalue_s;
      }
   }

   /* 
    * Prepare the dimension object for this field object. If we have
    * several fields sharing the same dimensions (as is usually the case)
    * we still need to define this object only once.
    */
   char *dimnames[2];
   dimnames[0] = malloc(strlen("ny")+1); strcpy(dimnames[0],"ny");
   dimnames[1] = malloc(strlen("nx")+1); strcpy(dimnames[1],"nx");
   size_t dimlengths[2];
   dimlengths[0]  = ny;
   dimlengths[1]  = nx;
   short  dimUnlims[2];
   dimUnlims[0] = dimUnlims[1] = 0;
   rsprod_dimensions *outdims;
   ret = rsprod_dims_create(&outdims,2,dimnames,dimlengths,dimUnlims);
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not create the Dimension object for output grid.\n",progname);
      exit(EXIT_FAILURE);
   }
   
   /* 
    * Prepare the dataset/fields. We describe the data with a name,
    * a type, its dimensions and some attributes.
    *
    */

   /* float dataset f1 */
   float fmin = 0.; 
   rsprod_field *f1;
   ret = rsprod_field_createStandard(&f1,
	 "field_1",               /* 'code' name of the field */
	 RSPROD_FLOAT,            /* type */
	 ne,outdims,              /* information on the dimensions */
	 "LongName for field_1",  /* long_name */
	 NULL,                    /* no standard name */
	 "carrots",               /* units */
	 &fillvalue_f,            /* fill value */
	 &fmin,                   /* valid_min */
	 NULL,                    /* valid_max */
	 0,NULL,                  /* no geographical coordinates */
	 field1data               /* the array of data */
	 );
   if (ret) {
      fprintf(stderr,"ERROR (%s) Unable to create the field <%s>\n",progname,"field_1");
      exit(EXIT_FAILURE);
   }

   /* short dataset f2 */
   short smin = -30;
   short smax = +30;
   rsprod_field *f2;
   ret = rsprod_field_createStandard(&f2,
	 "field_2",               /* 'code' name of the field */
	 RSPROD_SHORT,            /* type */
	 ne,outdims,              /* information on the dimensions */
	 "LongName for field_2",  /* long_name */
	 "StdName for field_2",   /* no standard name */
	 "parrots",               /* units */
	 &fillvalue_s,            /* fill value */
	 &smin,                   /* valid_min */
	 &smax,                   /* valid_max */
	 0,NULL,                  /* no geographical coordinates */
	 field2data               /* the array of data */
	 );
   if (ret) {
      fprintf(stderr,"ERROR (%s) Unable to create the field <%s>\n",progname,"field_2");
      exit(EXIT_FAILURE);
   }

   /* Some verbose output */
   rsprod_field_printInfo(f1);
   rsprod_field_printInfo(f2);


   /* Write to netcdf file */
   char ncName[1028];
   sprintf(ncName,"/tmp/rsprod-tmp.nc",progname);
   printf("Write datasets to netcdf file <%s>\n",ncName);

   int ncid;
   ret = nc_create(ncName,NC_CLOBBER,&ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While opening netcdf file <%s>\n\t%s\n",progname,ncName,nc_strerror(ret));
      exit(EXIT_FAILURE);
   }
   ret  = 0;
   ret += rsprod_field_writeToNetCDF(f1,ncid); 
   ret += rsprod_field_writeToNetCDF(f2,ncid); 
   if (ret) {
      fprintf(stderr,"ERROR (%s) While writing the fields to the netcdf file.\n",progname);
      exit(EXIT_FAILURE);
   }
   nc_close(ncid);

   rsprod_field_delete(f1);
   rsprod_field_delete(f2);
   rsprod_dims_delete(outdims);

   printf("<END %s>\n",progname);
   return(EXIT_SUCCESS);

}

/*
 * NAME: test_PackAndUnpack.c
 * 
 * PURPOSE: 
 *    Test program to check/demonstrate the creation of a float dataset
 *    and its packing/unpacking to/from a short field.
 *
 * DESCRIPTION:
 *    A 3x5 dataset (type float) is created. 
 *    It is then packed (to short) and unpacked (to float again)
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 23.04.2008
 *
 * MODIFIED:
 *
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "../src/rsprod.h"

char srcFile[] = "test_PackAndUnpack.c";

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
   printf("\tCreate a 3x5 dataset (type float). Pack it and unpack it\n");
   printf("\t(via calibration coefficients scale&offset).\n");
   printf("<START RUNNING>\n");

   /* we want to create two 3 x 5 field with type float and short, respectively */
   size_t nx = 3;
   size_t ny = 5;
   size_t ne = nx * ny;

   /* allocate the data field (arrays) and fill them with dummy values */
   float fillvalue_f  = -60.;

   float *field1data = malloc(sizeof(float)*ne);
   if (!field1data) {
      fprintf(stderr,"ERROR (%s) memory allocation problem.\n",progname);
      exit(EXIT_FAILURE);
   }

   /* define the calibration coefficients we are going to use */
   float calib_scale  = -0.75;
   float calib_offset = +11.75;

   for (size_t e = 0 ; e < ne ; e++) {
      if (e != 8 && e != 2) {
	 field1data[e] = e + 0.5;
      } else {
	 field1data[e] = fillvalue_f;
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
   short  dimorders[2];
   dimorders[0] = dimorders[1] = 1;
   rsprod_dimensions *outdims;
   ret = rsprod_dims_create(&outdims,2,dimnames,dimlengths,dimorders);
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
	 NULL,                    /* no valid_max */
	 0,NULL,                  /* no geographical coordinates */
	 field1data               /* the array of data */
	 );
   if (ret) {
      fprintf(stderr,"ERROR (%s) Unable to create the field <%s>\n",progname,"field_1");
      exit(EXIT_FAILURE);
   }

   /* Some verbose output */
   rsprod_field_printInfo(f1);

   /* pack the field to RSPROD_SHORT */
   printf("Pack/Compress the dataset to short.\n");
   rsprod_field_pack(f1,&calib_scale,&calib_offset);
   /* Some verbose output */
   rsprod_field_printInfo(f1);

   /* unpack the field to RSPROD_FLOAT */
   printf("UnPack/UnCompress the dataset to float.\n");
   rsprod_field_unpack(f1);
   /* Some verbose output */
   rsprod_field_printInfo(f1);

   rsprod_field_delete(f1);
   rsprod_dims_delete(outdims);

   printf("<END %s>\n",progname);
   return(EXIT_SUCCESS);

}

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

   float *field1data      = malloc(sizeof(float)*ne);
   float *field1data_copy = malloc(sizeof(float)*ne);
   if (!field1data || !field1data_copy) {
      fprintf(stderr,"ERROR (%s) memory allocation problem.\n",progname);
      exit(EXIT_FAILURE);
   }

   /* define the calibration coefficients we are going to use */
   float calib_scale  = 0.01;
   float calib_offset = 0;

   for (size_t e = 0 ; e < ne ; e++) {
      if (e != 8 && e != 2) {
         field1data[e] = e + 0.5;
      } else {
         field1data[e] = fillvalue_f;
      }
   }
   /* copy for later testing of pack/unpack */
   memcpy(field1data_copy,field1data,sizeof(field1data[0])*ne);

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
   short  dimunlims[2];
   dimunlims[0] = dimunlims[1] = 1;
   rsprod_dimensions *outdims;
   ret = rsprod_dims_create(&outdims,2,dimnames,dimlengths,dimunlims);
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
   float fmax = 100.;
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
     &fmax,                   /* valid_max */
     0,NULL,                  /* no geographical coordinates */
     field1data               /* the array of data */
     );
   if (ret) {
      fprintf(stderr,"ERROR (%s) Unable to create the field <%s>\n",progname,"field_1");
      exit(EXIT_FAILURE);
   }

   librsprod_echo_mode = LIBRSPROD_VERBOSE;

   /* pack the field to RSPROD_SHORT */
   printf("Pack/Compress the dataset to short.\n");
   if (rsprod_field_pack(f1,RSPROD_SHORT,&calib_scale,&calib_offset)) {
      fprintf(stderr,"ERROR (%s) Cannot pack field\n",progname);
      exit(EXIT_FAILURE);
   }
   short *packed_data = f1->data->content.values;
   for (size_t e = 0 ; e < ne ; e++) {
      printf("\t%u: %f -> %d\n",e,field1data_copy[e],packed_data[e]);
   }

   exit(1);

   /* unpack the field to RSPROD_FLOAT */
   printf("UnPack/UnCompress the dataset to float.\n");
   rsprod_field_unpack(f1);

   float *unpacked_data = f1->data->content.values;
   for (size_t e = 0 ; e < ne ; e++) {
      printf("\t%u: %f -> %f\n",e,field1data_copy[e],unpacked_data[e]);
   }
   
   /* transform the field to DOUBLE */
   printf("transform the dataset to double.\n");
   librsprod_unpack_to = RSPROD_DOUBLE;
   if (rsprod_field_unpack(f1)) {
      fprintf(stderr,"ERROR (%s) Cannot tranform field to double\n",progname);
      exit(EXIT_FAILURE);
   }
   double *double_unpacked_data = f1->data->content.values;
   for (size_t e = 0 ; e < ne ; e++) {
      printf("\t%u: %f -> %f\n",e,field1data_copy[e],double_unpacked_data[e]);
   }
   
   /* repack it to SHORT */
   printf("Re-Pack/Compress the dataset to short.\n");
   double calib_scale_double  = calib_scale;
   double calib_offset_double = calib_offset;
   if (rsprod_field_pack(f1,RSPROD_SHORT,&calib_scale_double,&calib_offset_double)) {
      fprintf(stderr,"ERROR (%s) Cannot pack field\n",progname);
      exit(EXIT_FAILURE);
   }
   short *repacked_data = f1->data->content.values;
   for (size_t e = 0 ; e < ne ; e++) {
      printf("\t%u: %f -> %d\n",e,field1data_copy[e],repacked_data[e]);
   }

   rsprod_field_delete(f1);
   rsprod_dims_delete(outdims);

   printf("<END %s>\n",progname);
   return(EXIT_SUCCESS);

}

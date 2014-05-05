/*
 * NAME: test_StringData.c
 * 
 * PURPOSE: 
 *    Test program to check/demonstrate the creation of a string dataset
 *    and its writing/reading to/from a netcdf file.
 *
 * DESCRIPTION:
 *    Two datasets, each with 7 strings are created. They are then 
 *    written to a netcdf file.
 *
 * NOTE:
 *    o This examples shows that only 'arrays of strings' are currently
 *      handled by the rsprod library. This should be fixed as the 'long array
 *      of chars' might be very usefull in some cases. The solution might
 *      be through a special RSPROD_STRING type, with different behaviour
 *      than RSPROD_CHAR.
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no/FoU, 09.05.2008
 *
 * MODIFIED:
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "../src/rsprod.h"

char srcFile[] = "test_StringData.c";

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
   printf("\tCreate two datasets containing strings. Write them\n");
   printf("\tto a netcdf file.\n");
   printf("<START RUNNING>\n");

   size_t nx = 7;

   /* prepare the Arrays of strings */
   /* ============================= */
   /* the first is defined as a long array of characters with static maximum length */
   size_t maxchars = 8;
   char *dataset_char1 = malloc( (nx * maxchars + 1) * sizeof(char));
   if (!dataset_char1) {
      fprintf(stderr,"ERROR (%s) with memory allocation.\n",progname);
      return(EXIT_FAILURE);
   }
   /* fill the array with '.'s and terminate it with '\0' */
   memset(dataset_char1,(int)'.',nx*maxchars*sizeof(char));
   dataset_char1[(nx * maxchars)] = '\0';
   /* write the strings in the array, taking care of skipping the right
    * number of characters (maxchars) to always start a string
    * at the correct position.
    * */
   for (size_t xi = 0 ; xi < nx ; xi++) {
      size_t i    = maxchars * xi;
      char *startchar = dataset_char1 + i;

      snprintf(startchar,maxchars,"f1:%02d",xi);
      startchar[strlen(startchar)] = ',';
   }
   printf("VERBOSE (%s) The char dataset 1 is:\n%s\n",progname,dataset_char1);

   /* the second is an array of pointers to the begining of the strings.
    * It is easier a data structure to handle */
   char **dataset_char2 = malloc( nx * sizeof(char *));
   if (!dataset_char2) {
      fprintf(stderr,"ERROR (%s) with memory allocation.\n");
      return(EXIT_FAILURE);
   }
   /* allocate each string and fill it with '.'s */
   for ( size_t e = 0 ; e < nx ; e ++ ) {
      dataset_char2[e] = malloc( maxchars * sizeof(char) );
      if (!dataset_char2[e]) {
	 fprintf(stderr,"PANIC (%s) with memory allocation.\n");
	 return(EXIT_FAILURE);
      }
      memset(dataset_char2[e],(int)'.',maxchars*sizeof(char));
      dataset_char2[e][maxchars-1] = '\0';
   }
   /* give a string value to each element */
   for (size_t xi = 0 ; xi < nx ; xi++) {
      snprintf(dataset_char2[xi],maxchars,"f2:%02d",xi);
   }
   printf("VERBOSE (%s) The char dataset 2 is:<\n",progname);
   for (size_t e = 0; e < nx ; e++) {
      printf("\t%s\n",dataset_char2[e]);
   }
   printf(">\n");

   /* create a dimension object */
   char *dimnames[1];
   dimnames[0] = malloc(strlen("nx")+1); strcpy(dimnames[0],"nx");
   size_t dimlengths[1];
   dimlengths[0]  = nx;
   short  dimorders[1];
   dimorders[0] = 1;
   rsprod_dimensions *outdims;
   ret = rsprod_dims_create(&outdims,1,dimnames,dimlengths,dimorders);
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not create the Dimension object.\n",progname);
      exit(EXIT_FAILURE);
   }

   char fillvalc = '\0';

   /* Create the RSPROD field objects */
   /* =============================== */
   rsprod_field *chars1_out;
   if (rsprod_field_createStandard(
	    &chars1_out,"char1",RSPROD_CHAR,nx,outdims,"character strings dataset 1",NULL,NULL,&fillvalc,NULL,NULL,0,NULL,dataset_char1)) {
      fprintf(stderr,"ERROR (%s) could not create Field object <char1>.\n",progname);
      exit(EXIT_FAILURE);
   }
   rsprod_field_printInfo(chars1_out);
   
   rsprod_field *chars2_out;
   if (rsprod_field_createStandard(
	    &chars2_out,"char2",RSPROD_CHAR,nx,outdims,"character strings dataset 2",NULL,NULL,NULL,NULL,NULL,0,NULL,dataset_char2)) {
      fprintf(stderr,"ERROR (%s) could not create Field object <char2>.\n",progname);
      exit(EXIT_FAILURE);
   }
   rsprod_field_printInfo(chars2_out);


   /* Write to netcdf file */
   /* ==================== */
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
   //ret += rsprod_field_writeToNetCDF(chars1_out,ncid); 
   ret += rsprod_field_writeToNetCDF(chars2_out,ncid); 
   if (ret) {
      fprintf(stderr,"ERROR (%s) While writing the fields to the netcdf file.\n",progname);
      exit(EXIT_FAILURE);
   }
   ret = nc_close(ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) Cannot close file <%s>\n",progname,ncName);
      exit(1);
   }

   /* Read from the netcdf file */
   /* ========================= */
   printf("Read datasets from netcdf file <%s>\n",ncName);
   ret = nc_open(ncName,NC_NOCLOBBER,&ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While opening netcdf file <%s>\n\t%s\n",progname,ncName,nc_strerror(ret));
      exit(EXIT_FAILURE);
   }
   ret  = 0;
   rsprod_field *chars2_in;
   ret += rsprod_field_loadFromNetCDF(&chars2_in,"char2",ncid); 
   if (ret) {
      fprintf(stderr,"ERROR (%s) While reading the fields from the netcdf file.\n",progname);
      exit(EXIT_FAILURE);
   }
   rsprod_field_printInfo(chars2_in);
   char *chars2_in_data;
   if (rsprod_data_getType(chars2_in->data) != RSPROD_CHAR) {
      fprintf(stderr,"ERROR (%s) The 'chars2_in' dataset is not of type CHAR.\n",progname);
      exit(EXIT_FAILURE);;
   }
   if ( (*chars2_in->data->methods->accessValues)(chars2_in->data,&chars2_in_data) )  {
      fprintf(stderr,"ERROR (%s) in accessing the 'chars2_in' values.\n",progname);
      return 1;
   }
   printf("The dataset we read is:\n\t%s\n",chars2_in_data);

   /* clean up memory */
   rsprod_field_delete(chars1_out);
   rsprod_field_delete(chars2_out);
   rsprod_field_delete(chars2_in);

   printf("<END %s>\n",progname);
   return(EXIT_SUCCESS);

}

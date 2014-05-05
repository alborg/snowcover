/*
 * NAME: test_ncInterface.c
 * 
 * PURPOSE: 
 *    Test program to check the netcdf interface, particularly the DEF/DATA mode.
 *
 * DESCRIPTION:
 *    A dataset and some global parameters are created and written to a netcdf file.
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 29.04.2008
 *
 * MODIFIED:
 *
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "../src/rsprod.h"

char srcFile[] = "test_ncInterface.c";

#define FIRST_DATASET    0
#define FIRST_ATTRIBUTES 1

int main(int argc, char *argv[]) {

   int ret;
   short mode = FIRST_DATASET;

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
   printf("\tCreate a dataset and some global attributes and write both\n");
   printf("\tto a netCDF file.\n");
   printf("<START RUNNING>\n");

   char ncName[1028];
   sprintf(ncName,"/tmp/rsprod-tmp.nc");

   size_t nt = 5;
   size_t nh = 2;
   size_t ne = nt * nh;

   /* allocate the data field (arrays) and fill them with dummy values */
   short fillvalue_s  = -50;

   short *fielddata = malloc(sizeof(short)*ne);
   if (!fielddata) {
      fprintf(stderr,"ERROR (%s) memory allocation problem.\n",progname);
      exit(EXIT_FAILURE);
   }

   for (size_t e = 0 ; e < ne ; e++) {
      if (e != 4) {
	 fielddata[e] = e - 8 ;
      } else {
	 fielddata[e] = fillvalue_s;
      }
   }

   /* 
    * Prepare the dimension object for this field object. If we have
    * several fields sharing the same dimensions (as is usually the case)
    * we still need to define this object only once.
    */
   char *dimnames[2];
   dimnames[0] = malloc(strlen("time")+1); strcpy(dimnames[0],"time");
   dimnames[1] = malloc(strlen("nh")+1); strcpy(dimnames[1],"nh");
   size_t dimlengths[2];
   dimlengths[0]  = nh;
   dimlengths[1]  = nt;
   short  dimorders[2];
   dimorders[0] = dimorders[1] = 1;
   rsprod_dimensions *outdims;
   ret = rsprod_dims_create(&outdims,2,dimnames,dimlengths,dimorders);
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not create the Dimension object for output grid.\n",progname);
      exit(EXIT_FAILURE);
   }
   
   /* 
    * Prepare the dataset/field. We describe the data with a name,
    * a type, its dimensions and some attributes.
    *
    */
   rsprod_field *f;
   ret = rsprod_field_createStandard(&f,
	 "field",                 /* 'code' name of the field */
	 RSPROD_SHORT,            /* type */
	 ne,outdims,              /* information on the dimensions */
	 "LongName for field",    /* long_name */
	 "StdName for field",     /* no standard name */
	 "m/s",                   /* units */
	 &fillvalue_s,            /* fill value */
	 NULL,                    /* valid_min */
	 NULL,                    /* no valid_max */
	 0,NULL,                  /* no geographical coordinates */
	 fielddata               /* the array of data */
	 );
   if (ret) {
      fprintf(stderr,"ERROR (%s) Unable to create the field <%s>\n",progname,"field");
      exit(EXIT_FAILURE);
   }

   /* Some verbose output */
   rsprod_field_printInfo(f);


   /* 
    * Prepare some global attributes
    *
    */
   rsprod_attributes glob;
   rsprod_attributes_create(&glob);
   ret = 0;
   rsprod_attr *attr;
   ret += rsprod_attr_createWithCopyValues(&attr,"filename",RSPROD_CHAR,strlen(ncName),ncName) + 
      rsprod_attributes_addCopyAttr(glob,attr);
   rsprod_attr_delete(attr);
   ret += rsprod_attr_createWithCopyValues(&attr,"processing_software",RSPROD_CHAR,strlen(progname),progname) +
      rsprod_attributes_addCopyAttr(glob,attr);
   rsprod_attr_delete(attr);
   int age = 27;
   ret += rsprod_attr_createWithCopyValues(&attr,"age_of_the_captain",RSPROD_INT,1,&age) +
      rsprod_attributes_addCopyAttr(glob,attr);
   rsprod_attr_delete(attr);

   /* Write dataset and attributes to netcdf file */
   printf("Open netcdf file <%s> for writing\n",ncName);
   int ncid;
   ret = nc_create(ncName,NC_CLOBBER,&ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While opening netcdf file <%s>\n\t%s\n",progname,ncName,nc_strerror(ret));
      exit(EXIT_FAILURE);
   }

   if (mode == FIRST_ATTRIBUTES) goto define_and_write_attributes;

define_and_write_dataset:
   printf("Define and write the dataset.\n");
   ret  = 0;
   ret += rsprod_field_writeToNetCDF(f,ncid); 
   if (ret) {
      fprintf(stderr,"ERROR (%s) While writing the dataset to the netcdf file.\n",progname);
      exit(EXIT_FAILURE);
   }

   if (mode == FIRST_ATTRIBUTES) goto close_file;
   
define_and_write_attributes:
   printf("Define and write the global attributes.\n");
   ret  = 0;
   ret += rsprod_attributes_writeToNetCDF(glob,ncid,NC_GLOBAL); 
   if (ret) {
      fprintf(stderr,"ERROR (%s) While writing the global attributes to the netcdf file.\n",progname);
      exit(EXIT_FAILURE);
   }

   if (mode == FIRST_ATTRIBUTES) goto define_and_write_dataset;

close_file:
   ret = nc_close(ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While closing the netcdf file.\n",progname);
      exit(EXIT_FAILURE);
   }


   rsprod_attributes_delete(glob);
   rsprod_field_delete(f);
   rsprod_dims_delete(outdims);

   printf("<END %s>\n",progname);
   return(EXIT_SUCCESS);

}

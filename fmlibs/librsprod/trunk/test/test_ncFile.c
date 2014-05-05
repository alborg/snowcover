/*
 * NAME: test_ncFile.c
 * 
 * PURPOSE: 
 *    Test program to check if a netcdf file can be loaded with rsprod
 *
 * DESCRIPTION:
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 10.11.2010
 *
 * MODIFIED:
 *
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "../src/rsprod.h"

char srcFile[] = "test_ncFile.c";


int main(int argc, char *argv[]) {

   int ret;

   char *progname = malloc(strlen(argv[0])+1);
   if (!progname) {
      fprintf(stderr,"ERROR (%s) memory allocation problem.\n",srcFile);
      exit(EXIT_FAILURE);
   }
   sprintf(progname,"%s",argv[0]);

   if (argc != 2) {
      fprintf(stderr,"USAGE: %s <nc_file_name>\n",progname);
      exit(EXIT_FAILURE);
   }
   char *ncName = argv[1];

   printf("<DESCRIPTION>\n");
   printf("Test program [%s]\n",progname);
   printf("Compiled from source file [%s]\n",srcFile);
   printf("SYNOPSIS:\n");
   printf("\tTest loading of file %s\n",ncName);


   printf("<RSPROD CONFIGURATION>\n");
   librsprod_dump_config();
   
   printf("<START RUNNING>\n");
   int ncid;

   /* read-in this file with the rsprod_file object */
   printf("Open netcdf file <%s> for reading\n",ncName);
   ret = nc_open(ncName,NC_NOCLOBBER,&ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While opening netcdf file <%s> for reading\n\t%s\n",progname,ncName,nc_strerror(ret));
      exit(EXIT_FAILURE);
   }

   printf("\n\n<TRY OPENING FILE WITHOUT UNPACKING>\n");
   librsprod_unpack_datasets = 0;
   rsprod_file *file = NULL;
   ret = rsprod_file_loadFromNetCDF(&file,RSPROD_READALL,NULL,ncid);
   if (ret < 0) {
      fprintf(stderr,"ERROR (%s) Cannot load the file from netCDF.\n",progname);
      exit(EXIT_FAILURE);
   }
   fprintf(stderr,"Content of <%s> is:\n",ncName);
   rsprod_file_printNcdump(file);
   rsprod_file_delete(file);

   printf("\n\n<TRY OPENING FILE WITH AUTOMATIC UNPACKING>\n");
   librsprod_unpack_datasets = 1;
   file = NULL;
   ret = rsprod_file_loadFromNetCDF(&file,RSPROD_READALL,NULL,ncid);
   if (ret < 0) {
      fprintf(stderr,"ERROR (%s) Cannot load the file from netCDF.\n",progname);
      exit(EXIT_FAILURE);
   }
   fprintf(stderr,"Content of <%s> is:\n",ncName);
   rsprod_file_printNcdump(file);
   rsprod_file_delete(file);

   ret = nc_close(ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While closing the netcdf file.\n",progname);
      exit(EXIT_FAILURE);
   }

   printf("<END %s>\n",progname);
   return(EXIT_SUCCESS);

}

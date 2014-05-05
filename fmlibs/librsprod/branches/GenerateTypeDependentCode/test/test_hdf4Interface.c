/*
 * NAME: test_hdf4Interface.c
 * 
 * PURPOSE: 
 *    Test program to check the HDF4 (SDS) interface.
 *
 * DESCRIPTION:
 *    A dataset its attributes are read from a HDF4 file.
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 17.08.2009
 *
 * MODIFIED:
 *
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mfhdf.h>
#include "../src/rsprod.h"

char srcFile[]   = "test_hdf4Interface.c";

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
   printf("\tRead a dataset from a HDF4 file.\n");
   printf("<START RUNNING>\n");

   char hdf4Name[1028];
   sprintf(hdf4Name,"./data/AMSR_E_L2A_BrightnessTemperatures_V10_201002102330_D.hdf");

   intn  status;
   /* open the SDS interface to the file */
   int32 sd_id = SDstart (hdf4Name,DFACC_READ);
   if (sd_id == FAIL) {
      fprintf(stderr,"ERROR (%s) Cannot open HDF4 file '%s'\n",progname,hdf4Name);
      exit(EXIT_FAILURE);
   }

   char *fields[] = {"36.5V_Res.4_TB_(not-resampled)",
                     "36.5H_Res.4_TB_(not-resampled)",
                     "Longitude",
                     "Latitude"};
   int nbFields = sizeof(fields) / sizeof(fields[0]);
  
   librsprod_echo_mode = LIBRSPROD_VERBOSE;
   rsprod_file *file = NULL;
   ret = rsprod_file_loadFromHDF4(&file,nbFields,fields,sd_id);
   printf("nb fields is %d (ret is %d)\n",nbFields,ret);
   if (ret != nbFields) {
      fprintf(stderr,"ERROR (%s) Cannot load the file from HDF4.\n",progname);
      exit(EXIT_FAILURE);
   } 
   librsprod_echo_mode = LIBRSPROD_QUIET;

   rsprod_file_printInfo(file);

   /* close the interface to the file */
   status = SDend (sd_id);

   rsprod_file_delete(file);


}

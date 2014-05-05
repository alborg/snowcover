/*
 * NAME: test_NoType.c
 * 
 * PURPOSE: 
 *    Test program to check/demonstrate the usage of the low-level notype data structure.
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
 *    Thomas Lavergne, met.no/FoU, 18.05.2009
 *
 * MODIFIED:
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "../src/rsprod.h"


char srcFile[] = "test_NoType.c";

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
   printf("\tUse the low-level 'notype' data structure.\n");
   printf("<START RUNNING>\n");

   /* we want to create two 3 x 5 field with type float and short, respectively */
   size_t ne = 5;

   float *data1 = malloc(sizeof(float)*ne);
   if (!data1) {
      fprintf(stderr,"ERROR (%s) memory allocation problem.\n",progname);
      exit(EXIT_FAILURE);
   }

   for (size_t e = 0 ; e < ne ; e++) {
	 data1[e] = e + 0.5;
   }

   rsprod_notype *this;
   ret = rsprod_notype_createWithAssignValues(&this,RSPROD_FLOAT,ne,data1);
   if (ret) {
	  fprintf(stderr,"ERROR (%s) Unable to create the data structure\n",progname);
      exit(EXIT_FAILURE);
   }
  
   rsprod_notype_printInfo(this);
   size_t n = rsprod_notype_getNbvalues(this);

   /* Access the data array by second pointer */
   float *data2;
   ret = (*this->methods->accessValues)(this,&data2);
   if (ret) {
	  fprintf(stderr,"ERROR (%s) Unable to access the array\n",progname);
      exit(EXIT_FAILURE);
   }

   if (data2 == data1)
	  printf("Same memory location for data1 and data2 at %s:%d:\n",srcFile,__LINE__);
   else
	  printf("%s:%d: data1 and data2 point to different locations. That's unexpected.\n",srcFile,__LINE__);

   /* Print the data to screen */
   for (size_t e = 0 ; e < n ; e++) {
	  printf("\t[%1u] %f\n",e,data2[e]);
   }
   
   /* Initialize the data with a given value */
   float fval = -99.5;
   printf("Initialize all data elements to %f\n",fval);
   ret = (*this->methods->initValues)(this,&fval);
   if (ret) {
	  fprintf(stderr,"ERROR (%s) Unable to initialize the content of the array\n",progname);
      exit(EXIT_FAILURE);
   }
   
   /* Print the data to screen */
   for (size_t e = 0 ; e < n ; e++) {
	  printf("\t[%1u] %f\n",e,data2[e]);
   }

   rsprod_notype_delete(this);
   
   free(progname);

   printf("<END %s>\n",progname);
   return(EXIT_SUCCESS);

}

/*
 * NAME: test_ncFile.c
 * 
 * PURPOSE: 
 *    Test program to dump the run-time configuration
 *
 * DESCRIPTION:
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 03.03.2011
 *
 * MODIFIED:
 *
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/rsprod.h"

char srcFile[] = "test_Config.c";


int main(int argc, char *argv[]) {

   int ret;

   char *progname = malloc(strlen(argv[0])+1);
   if (!progname) {
      fprintf(stderr,"ERROR (%s) memory allocation problem.\n",srcFile);
      exit(EXIT_FAILURE);
   }
   sprintf(progname,"%s",argv[0]);

   if (argc != 1) {
      fprintf(stderr,"USAGE: %s\n",progname);
      exit(EXIT_FAILURE);
   }

   printf("<DESCRIPTION>\n");
   printf("Test program [%s]\n",progname);
   printf("Compiled from source file [%s]\n",srcFile);
   printf("SYNOPSIS:\n");
   printf("\tDump Run Time configuration\n");


   printf("<RSPROD CONFIGURATION>\n");
   librsprod_dump_config();

   printf("<END %s>\n",progname);
   return(EXIT_SUCCESS);

}

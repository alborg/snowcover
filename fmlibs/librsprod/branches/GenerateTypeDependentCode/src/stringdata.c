

/* 
 * NAME : stringdata.c
 *
 * PURPOSE:
 *    Tools to transform between an array with several (fixed length) concatenated 
 *    strings into an array of pointers to strings.
 *
 * AUTHOR:    
 *    Thomas Lavergne, met.no, 30.09.2008
 *
 * MODIFIED:
 *    TL, met.no, 24.03.2011   :   Add the 'add_nullchar' so that the routine
 *                                    will also work when the char array does
 *                                    not have '\0' separating each string.
 *
 * */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stringdata.h"

int datastring_format(char *univec, size_t nbstrings, size_t maxchars, char ***array) {

   /* allocate memory */
   char **Parray = malloc(nbstrings * sizeof(char *)); 
   if (!Parray) {
      fprintf(stderr,"ERROR (%s) with memory allocation.\n",__func__);
      return 1;
   }
   int add_nullchars = (strlen(univec) == maxchars?0:1);
   rsprod_echo(stderr,"VERBOSE (%s:%u:) add_nullchars is %d\n",
         __func__,__LINE__,add_nullchars);
   for (size_t s = 0 ; s < nbstrings ; s++) {
      Parray[s] = malloc( (maxchars+add_nullchars)*sizeof(char) );
      if (!Parray[s]) {
         fprintf(stderr,"ERROR (%s) with memory allocation.\n",__func__);
         return 1;
      }
      memset(Parray[s],(int)'\0',(maxchars+add_nullchars)*sizeof(char));
   }

   /* copy the string contents */
   char *sc = univec;
   for (size_t s = 0 ; s < nbstrings ; s ++) {
      snprintf(Parray[s],maxchars+add_nullchars,"%s",sc);
      sc+=(maxchars);
   }

   /* return */
   *array = Parray;

   return 0;

}

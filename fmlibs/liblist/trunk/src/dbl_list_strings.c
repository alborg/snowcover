/*
 * NAME: 
 *    dbl_list_strings
 * 
 * PURPOSE: 
 *    Define a 'character string' type and usefull routines for use with lists. 
 *
 * NOTE:
 *    Normally, the content of the list shousl not be handled by the dbl_list library, 
 *    but lists of strings are so handy that we distribute an implementation here.
 * 
 * AUTHOR:
 *    Thomas Lavergne, met.no/FoU, 07.10.2010
 *
 * MODIFIED: 
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dbl_list.h"


void printString(dbl_list_string *S)  {
   printf("{%s}",S->str);
}
int copyString(dbl_list_string *S1,dbl_list_string *S2) {
   size_t size = strlen(S2->str)+1;
   S1->str = malloc(size);
   if (!(S1->str))
      return 1;
   sprintf(S1->str,"%s",S2->str);
   return 0;
}
int setString(dbl_list_string *S,char *str) {
   size_t size = strlen(str)+1;
   S->str = malloc(size);
   if (!(S->str))
      return 1;
   sprintf(S->str,"%s",str);
   return 0;
}
void deleteString(dbl_list_string *S) {
   free(S->str);
}
int Strings_areSame(dbl_list_string *a, dbl_list_string *b) {
   return !strcmp(a->str,b->str);
}
char dbl_list_stringis_param[100];
int StringIs(dbl_list_string *S) {
   return !strcmp(S->str,dbl_list_stringis_param);
}


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rsprod_memory_utils.h"

int rsprod_string_trim_allNulls(const char *string_orig,char **string_trans) {
   char *res      = strchr(string_orig,'\0'); /* do not work with extended character sets */
   size_t nbchars = res -  string_orig;
   *string_trans   = rsprodMalloc(nbchars);
   char *ret = strncpy(*string_trans,string_orig,nbchars);
   if (!ret) {
      fprintf(stderr,"ERROR (rsprod_string_trim_allNulls) Failed on strncpy.\n");
      return 1;
   }
   return 0;
}

int rsprod_string_trim_allNullsExceptLast(const char *string_orig,char **string_trans) {
   char *res      = strchr(string_orig,'\0'); /* do not work with extended character sets */
   size_t nbchars = res -  string_orig;
   *string_trans   = rsprodMalloc(nbchars+1);
   char *ret = strncpy(*string_trans,string_orig,nbchars+1);
   if (!ret) {
      fprintf(stderr,"ERROR (rsprod_string_trim_allNullsExceptLast) Failed on strncpy.\n");
      return 1;
   }
   return 0;
   
}

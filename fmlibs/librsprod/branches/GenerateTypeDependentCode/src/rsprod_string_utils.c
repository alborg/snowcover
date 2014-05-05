
#include <ctype.h>
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

/* TODO: re-implement this routine with lists */
void rsprod_string_split(char *string, char sep, char ***tokens,size_t *nbTokens) {

   char *sret;
   
   int pass;
   /* pass = 0 : counting the Tokens, pass = 1 : copying them to returned arrays */
   for (pass = 0 ; pass <= 1 ; pass++) {
      if (pass==1) {
         *tokens = rsprodMalloc(*nbTokens * sizeof (char *));
      }
      char *start = string;
      *nbTokens = 0;
      char *commaChar;
      while (commaChar = strchr(start,sep)) {
         size_t nbchar = (commaChar-start);
         if (pass==1) {
            char *oneToken = rsprodMalloc((nbchar+1)*sizeof(char));
            oneToken[nbchar] = '\0';
            sret = strncpy(oneToken,start,nbchar);
            (*tokens)[*nbTokens] = oneToken;
         }
         (*nbTokens)++;
         start += nbchar + 1;
      }
      if (pass==1) {
         size_t nbchar = strlen(start);
         char *oneToken = rsprodMalloc((nbchar+1)*sizeof(char));
         oneToken[nbchar] = '\0';
         sret = strncpy(oneToken,start,nbchar);
         (*tokens)[*nbTokens] = oneToken;
      }
      (*nbTokens)++;
   }

}

int rsprod_string_toLower(char *string) {
   for( int i = 0; string[ i ]; i++)
       string[ i ] = tolower( string[ i ] );
   return 0;
}

int rsprod_string_toUpper(char *string) {
   for( int i = 0; string[ i ]; i++)
      string[ i ] = toupper( string[ i ] );
   return 0;
}

/* 
 * TBD: move this routine elsewhere and implement it in a more
 * generic way, e.g. with a translation dictionary (hash table)
 */
int rsprod_string_fromHDF4_2_NC(char *old, char **new) {

#define Allocate_and_copy_string(dummy) {*new = rsprodMalloc(strlen(ncName)+1); sprintf(*new,"%s",ncName);}
   if (!strcmp(old,"UNIT")) {
      char ncName[] = "units";
      Allocate_and_copy_string()
   } else if (!strcmp(old,"SCALE FACTOR")) {
      char ncName[] = "scale_factor";
      Allocate_and_copy_string()
   } else if (!strcmp(old,"OFFSET")) {
      char ncName[] = "add_offset";
      Allocate_and_copy_string()
   }
   return 0;

#undef allocate_and_copy_string
}

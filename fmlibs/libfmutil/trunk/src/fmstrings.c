/*
 * NAME:
 * NA
 *
 * PURPOSE:
 * NA
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * NA
 *
 * OUTPUT:
 * NA
 *
 * NOTES:
 * All functions should have fm namespace. Ask Thomas to change names of
 * convertString2Float and convertString2Int
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * Thomas Lavergne, met.no/FOU, 07.12.2007: Add the fmstrsplit() routine
 * Øystein Godøy, METNO/FOU, 2011-12-19: Added grep-like functionality.
 *
 * ID:
 * $Id$
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h> /* for size_t */
#include "fmutil_config.h"
#include <fmstorage.h>
#include <fmutil_types.h>
#include <fmstrings.h>

/*
 * PURPOSE:
 * To chop of newline characters in text strings.
 */
void fmremovenewline(char *mystr) {

    int i;

    for (i=0; i<strlen(mystr); i++) {
	if (mystr[i] == '\n') mystr[i] = '\0';
    }
}

/*
 * PURPOSE:
 * To parse through an arbitrarily-delimited string (e.g. "first,second,last")
 * and retrieve separated tokens ("first","second","last")
 *
 * example use:
 * unsigned int nbtokens;
 * char **tokens;
 * fmstrsplit("first:second:last",":",&nbtokens,&tokens);
 * printf("Number of tokens: %u\n",nbtokens);
 * printf("First is : %s\n",tokens[0]);
 *
 */
void fmstrsplit(char *origString, const char *sep, unsigned int *nbTokens,char ***tokens) {
   char *sret;
   
   int pass;
   /* pass = 0 : counting the Tokens, pass = 1 : copying them to returned arrays */
   for (pass = 0 ; pass <= 1 ; pass++) {
      if (pass==1) {
	 *tokens = fmMalloc(*nbTokens * sizeof (char *));
      }
      char *start = origString;
      *nbTokens = 0;
      char *commaChar;
      while (commaChar = strstr(start,sep)) {
         size_t nbchar = (commaChar-start);
         if (pass==1) {
	    char *oneToken = fmMalloc((nbchar+1)*sizeof(char));
	    oneToken[nbchar] = '\0';
	    sret = strncpy(oneToken,start,nbchar);
	    (*tokens)[*nbTokens] = oneToken;
	 }
	 (*nbTokens)++;
	 start += nbchar + 1;
      }
      if (pass==1) {
	 size_t nbchar = strlen(start);
	 char *oneToken = fmMalloc((nbchar+1)*sizeof(char));
	 oneToken[nbchar] = '\0';
	 sret = strncpy(oneToken,start,nbchar);
	 (*tokens)[*nbTokens] = oneToken;
      }
      (*nbTokens)++;
   }

   /*
   fprintf(stdout,"VERBOSE (strSplit). Original string is: <%s>\n",origString);
   for (size_t i=0 ; i < *nbTokens ; i++) {
      fprintf(stdout,"\t\tsubstring #%02d is <%s>\n",i+1,(*tokens)[i]);
   }
   fprintf(stdout,"%02d tokens where found.\n",*nbTokens);
   */

}

/*
 * PURPOSE:
 * To join several independent strings (in an array of strings) into a
 * single one, using the delimiter chosen by the user.
 *
 * Note: the new string is returned as allocated memory that should be
 * released.
 *
 * Example:
 * char *substrings[3] = {"one","two","three"};
 * char *string = fmstrjoin(3,substrings,'.');
 * printf("New string is <%s>\n",string);
 *
 * New string is <one.two.three>
 *
 */
char *fmstrjoin(size_t nbStrings, char **strings, char sep) { 

   size_t w,nbc;
   char *ret,*track;

   /* get the size of the final string: */
   size_t nbChars = 0;
   /*   add-up the size of each individual word */
   for (w = 0 ; w < nbStrings ; w++) {
      nbChars += strlen(strings[w]);
   }
   /* add the delimiters */
   nbChars += (nbStrings-1);

   /* allocate the new string */
   ret = malloc(nbChars+1);
   if (ret) {
      ret[nbChars]='\0';

      track = ret;
      for (w = 0 ; w < nbStrings ; w++) {
	 nbc = strlen(strings[w]);
	 memcpy(track,strings[w],nbc);
	 track+=nbc;
	 if (w != (nbStrings-1)) {
	    track[0]=sep;track++;
	 }
      }
   }
   return ret;
}


/*
 * PURPOSE:
 * To parse through a filename and separate the directory part from
 * the actual filename. No memory is allocated but 2 pointers are 
 * located:
 * 1) dirname_end points to the last character of the directory
 * 2) basename_start points to the first character of the filename
 *
 * Both of the pointers can be returned to NULL if one part is missing.
 *
 */
void fmbasename(char *filename,char **dirname_end,char **basename_start) {
  
   unsigned int nbtokens;
   char **tokens;
   fmstrsplit(filename,"/",&nbtokens,&tokens);
   size_t e;

   if (nbtokens == 0) {
      /* No separator. By convention it is a file name */
      *dirname_end = NULL;
      *basename_start = filename;
   } else if (strlen(tokens[nbtokens-1]) == 0){
      /* The last token is empty: the input filename is a directory */
      *dirname_end    = &(filename[strlen(filename)-1]);
      while (**dirname_end == '/') {
	 *dirname_end-=1;
      }
      *basename_start = NULL;
   } else {
      *basename_start = filename + strlen(filename) - strlen(tokens[nbtokens-1]);
      *dirname_end    = *basename_start - 1 ;
      while (**dirname_end == '/') {
	 *dirname_end-=1;
      }
   }

   for ( e = 0 ; e < nbtokens ; e++ )
      free(tokens[e]);
   free(tokens);

}

/* Convert a string to a floating point value 
 *
 * Two implementations are proposed. 
 * One is able to check if the input string can 
 * be converted to a floating point value and 
 * the other not.
 *
 * The first is compiled if the macros HAVE_STRTOF 
 * is defined but requires the C99 compliant 
 * function strtof(). The alternative version uses
 * atof() which should be available everywhere.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no/FoU, 05.11.2009
 */
int convertString2Float(char *string,float *val) {

#ifdef FMUTIL_HAVE_STRTOF
   char *checkconvert;
   *val = strtof(string,&checkconvert);
   if (*checkconvert == '\0')  {
      return 1;
   } else {
      return 0;
   }
#else
   *val = atof(string);
   return 1;
#endif

}

/* Convert a string to a long integer value 
 *
 * Same as above but using strtol (or atol)
 */
int convertString2Long(char *string,long *val) {

#ifdef FMUTIL_HAVE_STRTOL
   char *checkconvert;
   *val = strtol(string,&checkconvert,10);
   //fprintf(stderr,"VERBOSE (%s) converting <%s> to long, checkconvert is (%s): ",__func__,string,checkconvert);
   if (*checkconvert == '\0')  {
      //fprintf(stderr,"%ld\n",*val);
      return 1;
   } else {
      //fprintf(stderr,"%s\n","wrong");
      return 0;
   }
#else
   *val = atol(string);
   return 1;
#endif

}

/*
 * libfmutil prototype for a grep like function in C. Currently a maximum
 * of 10 subtokens are supported.
 *
 * Use * to indicate whether the text searched for is to appear at the
 * beginning or end of the string searched.
 *
 * Øystein Godøy, METNO/FOU, 2011-10-31
 */
fmbool fmgrep(char *needle, const char *haystack) {

    char *where="fmgrep";
    int maxtokens = 10, ntokens=0, haystacklength, needlelength, i, j;
    char searchtoken[maxtokens][FMSTRING16], *pt;
    const char *tokdiv="*";
    char myneedle[FMSTRING64], myhaystack[FMSTRING1024];

    /*
     * First check whether the needle is complicated with wildcards or
     * not, if not use strstr...
     */
    if (!strchr(needle,'*')) {
        if (strstr(haystack,needle)) {
            return(FMTRUE);
        } else {
            return(FMFALSE);
        }
    }

    sprintf(myneedle,"%s",needle);
    needlelength = fmstrlen(myneedle);
    sprintf(myhaystack,"%s",haystack);
    haystacklength = fmstrlen(myhaystack);
    
    /*
     * If the wildcard is first or last, do a simple search
     */
    if (myneedle[0] == '*') {
        if (fmgrepend(myneedle, myhaystack)) {
            return(FMTRUE);
        } else {
            return(FMFALSE);
        }
    } else if (myneedle[needlelength-1] == '*') {
        if (fmgrepstart(myneedle, myhaystack)) {
            return(FMTRUE);
        } else {
            return(FMFALSE);
        }
    }

    /*
     * Split the needle in the correct number of subtokens
     */
    pt = myneedle;
    strtok(pt,"*");
    if (pt == NULL) {
        sprintf(searchtoken[ntokens],"%s",pt);
        ntokens = 1;
    } else {
        sprintf(searchtoken[ntokens],"%s",pt);
        ntokens=1;
        while (pt = strtok(NULL,tokdiv)) {
            sprintf(searchtoken[ntokens],"%s",pt);
            ntokens++;
        }
        ntokens--;
    }

    /*
     * Use the tokens to perform a inclusive search
     */
    for (i=0;i<=ntokens;i++) {
        if (i==0) {
            if (!fmgrepstart(searchtoken[i],myhaystack)) {
                return(FMFALSE);
            }
            continue;
        }
        if (i==ntokens) {
            if (!fmgrepend(searchtoken[ntokens],myhaystack)) {
                return(FMFALSE);
            }
            continue;
        }
        if (strstr(myhaystack,searchtoken[i]) == NULL) {
            return(FMFALSE);
        }
    }

    return(FMTRUE);
}

fmbool fmgrepend(char *needle, const char *haystack) {
    int i, j;
    char myneedle[FMSTRING64], myhaystack[FMSTRING1024];
    int haystacklength, needlelength;

    sprintf(myneedle,"%s",needle);
    needlelength = fmstrlen(myneedle);
    sprintf(myhaystack,"%s",haystack);
    haystacklength = fmstrlen(myhaystack);

    j=0;
    for (i=haystacklength-1;i>haystacklength-needlelength+1;i--) {
        if (myhaystack[i] == myneedle[needlelength-1-j]) {
            j++;
            continue;
        } else {
            return(FMFALSE);
        }
    }
    return(FMTRUE);
}

fmbool fmgrepstart(char *needle, const char *haystack) {
    int i;
    char myneedle[FMSTRING64], myhaystack[FMSTRING1024];
    int needlelength;

    sprintf(myneedle,"%s",needle);
    needlelength = fmstrlen(myneedle);
    sprintf(myhaystack,"%s",haystack);

    for (i=0;i<needlelength-1;i++) {
        if (myhaystack[i] == myneedle[i]) {
            continue;
        } else {
            return(FMFALSE);
        }
    }
    return(FMTRUE);
}

int fmstrlen(char *mystring) {
    int i;

    i = 0;

    while (mystring[i] != '\0') {
        i++;
    }

    return(i);
}

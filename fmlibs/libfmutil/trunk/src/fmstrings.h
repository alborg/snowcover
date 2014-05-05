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
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, METNO/FOU, 26.04.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 28.02.2008: Added string lengths.
 * Øystein Godøy, METNO/FOU, 2011-12-19: Added grep-like functionality.
 *
 * ID:
 * $Id$
 */


#ifndef FMSTRINGS_H
#define FMSTRINGS_H

#define FMSTRING16 16
#define FMSTRING32 32
#define FMSTRING64 64
#define FMSTRING128 128
#define FMSTRING256 256
#define FMSTRING512 512
#define FMSTRING1024 1024

void fmremovenewline(char *mystr); 
void fmstrsplit(char *origString, const char *sep, unsigned int *nbTokens,char ***tokens);
char *fmstrjoin(size_t nbStrings, char **strings, char sep);
void fmbasename(char *dirfilename, char **end_dirname, char **start_basename);

int convertString2Float(char *string,float *val);
int convertString2Long(char *string,long *val);

fmbool fmgrep(char *needle, const char *haystack);
fmbool fmgrepstart(char *needle, const char *haystack);
fmbool fmgrepend(char *needle, const char *haystack);
int fmstrlen(char *mystring);

#endif

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
 * Øystein Godøy, METNO/FOU, 16.10.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 20.10.2009: Added fmreaddir, fmfilelist_sort
 * Øystein Godøy, METNO/FOU, 21.10.2009: Added fmstarcdirs
 *
 * ID:
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "fmutil_types.h"
#include "fmstorage.h"
#include "fmstrings.h"
#include "fmtime.h"

typedef struct {
    int nfiles;
    char *path;
    char **filename;
} fmfilelist;

typedef struct {
    int nfiles;
    char **dirname;
} fmstarclist;

int fmfilelist_alloc(fmfilelist *mylist,int nfiles); 
int fmfilelist_sort(fmfilelist *flist); 
int fmfilelist_free(fmfilelist *mylist);
int fmlinkfiles(fmfilelist src, fmfilelist dst); 
int fmremovefiles(fmfilelist mylist); 
int fmreaddir(char *idir, fmfilelist *flist);
int fmstarcdirs(fmtime start, fmtime end, fmstarclist *dirlist);

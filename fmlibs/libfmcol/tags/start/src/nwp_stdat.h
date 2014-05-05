/*
 * NAME:
 * nwp_stdat.h
 * 
 * PURPOSE:
 * Header file for nwp_stdat.c
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * RETURN VALUES:
 * NA
 *
 * DEPENDENCIES:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 28/01/2002
 * Øystein Godøy, MI/FOU, 16.04.2003
 */

#include <stdio.h>
#include <stdlib.h>
#include <satimg.h>
#include <collocate1.h>

#include <satimg.h>
#include <fmcoord.h>

#ifndef _NWP_STDAT_H
#define _NWP_STDAT_H

#define OUTOFIMAGE -40100
#define MISVAL -99999
#define UNDEF +1.E+35

typedef struct {
    int nopar;
    int nopix;
    time_t vtime;
    int ltime;
    struct ucs nav; 
    float data[NOFIELDS][NWPSIZE2D];
} ns_data;

int nwp_stdat(struct latlon gpos, fmproj myproj, struct ucs refucs,
	int nwptime[], float *nwpfield, ns_data *n);
int nwp_init(int boxsize, float boxresx, float boxresy, int nopar, ns_data *n);
int nwp_free(ns_data *n);
    
#endif /* _NWP_STDAT_H */


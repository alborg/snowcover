/*
 * NAME:
 * avhrr_stdat.h
 * 
 * PURPOSE:
 * Header file for avhrr_stdat.c
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * RETURN VALUES:
 *
 * DEPENDENCIES:
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 06/11/2000
 * Øystein Godøy, MI/FOU, 11.04.2003
 */

#include <stdio.h>
#include <stdlib.h>

#include <satimg.h>
#include <collocate1.h>
#include <fmcoord.h>
#include <fmsubtrack.h>

#ifndef _AVHRR_STDAT_H
#define _AVHRR_STDAT_H

#define OUTOFIMAGE -40100
#define MISVAL -99999
#define MAXCH 5

typedef struct {
    char source[20];
    int nochan;
    int nopix;
    time_t vtime;
    struct ucs nav;
    struct imga ang;
    char ch3;
    float data[NOCHAN][BOXSIZE2D];
} as_data;

int avhrr_stdat(struct latlon gpos, fmproj myproj, struct imgh img, as_data *a);
int avhrr_init(int boxsize, int nochan, as_data *a);
int avhrr_free(as_data *a);
    
#endif /* _AVHRR_STDAT_H */


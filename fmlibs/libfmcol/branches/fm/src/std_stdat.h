/*
 * NAME:
 * std_stdat.h
 *
 * PURPOSE:
 * Header file for std_stdat.c software.
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 25/01/2002
 */

#ifndef _STD_STDAT_H
#define _STD_STDAT_H

#include <satimg.h>

/*
 * Time differences are given in seconds
 */
#define SDIFF 5400 /* Accepted time difference from synop time */
#define TSDIFF 1800 /* Generally acceppted time difference syno */
#define TDDIFF 2700 /* Generally acceppted time difference drau */
#define STDATSTRLEN 1024

typedef struct {
    char stID[10];
    char stType[5]; /* ?? */
    struct latlon pos;
    int year;
    short month;
    short day;
    short hour;
    short min;
    float TTT;
    float TdTdTd;
    float TwTwTw;
    float PPPP;
    short ff;
    int dd;
    float RRR;
    short E;
    short sss;
    short N;
    short Nh;
    short Cl;
    short Cm;
    short Ch;
    short VV;
    short ww;
    short W1;
    short W2;
} obsstruct;

typedef struct {
    char type[5];
    struct dto timeid0;
    struct dto timeid1;
    struct dto timeid2;
    int unixtime0;
    int unixtime1;
    int unixtime2;
    int noobs;
    int nostat;
    obsstruct *obs;
} stdatstruct;

int read_stdat(char *ipath, char *basename, boolean synop, 
	time_t timeid, stdatstruct *pobs); 
int get_stdat_h(char *infile, stdatstruct *obsl);
int fill_stdat_h(char *infile, time_t vtime, stdatstruct *obsl); 
int init_stdat(stdatstruct *obsl); 
int free_stdat(stdatstruct *obsl);

#endif /*  _STD_STDAT_H */

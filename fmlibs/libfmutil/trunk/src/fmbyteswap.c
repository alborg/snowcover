/*
 * NAME:
 * fmbyteswap
 *
 * PURPOSE:
 * To perform byte swapping when required between Little Endian (e.g. Intel
 * platform) and Big Endian (e.g. SGI/IRIX).
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 * Floats are to be treated as integer values (4 bytes), typecast in and
 * out...
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, MET/FOU, 24.10.2001
 *
 * MODIFIED:
 * Øystein Godøy, MET/FOU, 03.01.2003
 * Øystein Godøy, met.no/FOU, 07.01.2005 
 *
 * ID:
 * $Id$
 */

#include <fmbyteswap.h>

void fmshortbyteswap(short int *data, int ndata) {

    int i;

    for (i=0; i<ndata; i++) {
      fmbs_short(&data[i]);
    }
}

void fmintbyteswap(int *data, int ndata) {

    int i;

    for (i=0; i<ndata; i++) {
      fmbs_int(&data[i]);
    }
}

void fmbs_short(short int *data) {

    short int iswap;

    iswap = *data;
    *data = (((iswap & 255) << 8)) | ((iswap >> 8) & 255);
}

void fmbs_int(int *data) {

    int iswap;

    iswap = *data;
    *data = ((iswap >> 24) & 255) | ((iswap >> 8) & 65280) |
	((iswap & 65280) << 8) | ((iswap & 255) << 24);
}

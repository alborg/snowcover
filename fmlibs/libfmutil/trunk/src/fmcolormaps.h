/*
 * NAME:
 *
 * PURPOSE:
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * Øystein Godøy, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */

#ifndef FMCOLORMAP_H
#define FMCOLORMAP_H


void fmheatmap(int noclasses,
        unsigned short red[256],
        unsigned short green[256],
        unsigned short blue[256]);
void fmmapb2g(int noclasses,
        unsigned short red[256],
        unsigned short green[256],
        unsigned short blue[256]);
void fmmapr2g(int noclasses,
        unsigned short red[256],
        unsigned short green[256],
        unsigned short blue[256]);

#endif

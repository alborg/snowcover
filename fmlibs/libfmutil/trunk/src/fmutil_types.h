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
 * Thomas Lavergne
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 31.08.2007 
 *
 * ID:
 * $Id$
 */

#ifndef FMUTIL_TYPES_H
#define FMUTIL_TYPES_H

#ifndef FMBOOL
#define FMBOOL
typedef enum {FMFALSE, FMTRUE} fmboolean, fmbool;
#endif

#ifndef FMPI
#define FMPI
#define fmPI 3.141592654
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD 0.0174532925199432958
#endif
#ifndef RAD_TO_DEG
#define RAD_TO_DEG 57.29577951308232
#endif

#ifndef FMREARTH
#define FMREARTH 6371.
#endif

#ifndef FMRETURNCODES
typedef enum {
    FM_OK,
    FM_SYNTAX_ERR,
    FM_IO_ERR,
    FM_MEMALL_ERR,
    FM_MEMFREE_ERR,
    FM_VAROUTOFSCOPE_ERR,
    FM_OTHER_ERR
} fmreturncode;
#endif

#endif

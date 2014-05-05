/*
 * FUNCTION:
 * fmivec
 *
 * PURPOSE:
 * Returns a pointer into the datavector containing the matrix with 
 * image data. This funtion is used when a 2 dimensional matrix is 
 * simulated using a 1 dimensional matrix.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, xx.xx.xxxx
 */

#include <fmutil.h>

long fmivec(long x, long y, unsigned long nx) {
    long i;
    
    i = y*nx+x;
    return(i);
}

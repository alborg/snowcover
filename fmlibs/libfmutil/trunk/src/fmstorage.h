/*
 * NAME:
 * fmstorage
 *
 * PURPOSE:
 * Useful functions handling storage containers.
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
 * Øystein Godøy, METNO/FOU, 02.12.2010: Added fmalloc_uchar_vector
 * Øystein Godøy, METNO/FOU, 2011-12-14: Added fmfree_uchar_vector
 * Øystein Godøy, METNO/FOU, 2011-12-16: Added fmalloc_ubyte_2d and
 * fmfree_ubyte_2d
 *
 * ID:
 * $Id$
 */


#ifndef FMSTORAGE_H
#define FMSTORAGE_H


long fmivec(long x, long y, unsigned long nx);
void fmijmap(long elem, unsigned long nx, long *x, long *y);

void *fmMalloc(size_t length);

int fmalloc_char_vector(char **value, int length); 
int fmalloc_uchar_vector(unsigned char **value, int length); 
int fmalloc_ushort_vector(unsigned short **value, int length); 
int fmalloc_int_vector(int **value, int length); 
int fmalloc_float_vector(float **value, int length); 
int fmalloc_double_vector(double **value, int length); 

int fmalloc_byte_2d(char ***value, int rows, int columns); 
int fmalloc_ubyte_2d(unsigned char ***value, int rows, int columns); 
int fmalloc_byte_2d_contiguous(char ***value, int rows, int columns); 
int fmalloc_ushort_2d(unsigned short ***value, int rows, int columns); 
int fmalloc_int_2d(int ***value, int rows, int columns); 
int fmalloc_float_2d(float ***value, int rows, int columns); 
int fmalloc_double_2d(double ***value, int rows, int columns); 

int fmfree_char_vector(char *value); 
int fmfree_uchar_vector(unsigned char *value); 
int fmfree_ushort_vector(unsigned short *value); 
int fmfree_int_vector(int *value); 
int fmfree_float_vector(float *value); 
int fmfree_double_vector(double *value); 

int fmfree_byte_2d(char **value, int rows); 
int fmfree_ubyte_2d(unsigned char **value, int rows); 
int fmfree_ushort_2d(unsigned short **value, int rows); 
int fmfree_int_2d(int **value, int rows); 
int fmfree_float_2d(float **value, int rows); 
int fmfree_double_2d(double **value, int rows); 

#endif

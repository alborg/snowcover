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
 * Øystein Godøy, METNO/FOU, 16.10.2007: Corrected type in byte 2d allocation.
 * Øystein Godøy, METNO/FOU, 02.12.2010: Added fmalloc_uchar_vector
 * Øystein Godøy, METNO/FOU, 2011-12-14: Added fmfree_uchar_vector
 * Øystein Godøy, METNO/FOU, 2011-12-16: Added fmalloc_ubyte_2d and
 * fmfree_ubyte_2d
 *
 * ID:
 * $Id$
 */

#include <stdlib.h>
#include <stdio.h>
#include <fmutil_types.h>
#include <fmerrmsg.h>

/*
 * FUNCTION:
 * fmMalloc
 *
 * PURPOSE:
 * Wrapper around the stdlib's malloc() function that performs
 * its own check on the returned pointer and terminates the
 * execution if necessary.
 *
 * CAUTION: 
 * Do *not* use this wrapper if you do *not* want your program
 * to exit on an memory allocation error.
 *
 * AUTHOR:
 * Thomas Lavergne, METNO/FOU, 26.06.2007
 */
void *fmMalloc(size_t size) {
   void *res;
   res = malloc(size);
   if (!res) {
      fmerrmsg("fmMalloc","Memory allocation problem.\n");
      exit(FM_MEMALL_ERR);
   }
   return res;
}


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
long fmivec(long x, long y, unsigned long nx) {
    long i;
    
    i = y*nx+x;
    return(i);
}
/*
 * FUNCTION:
 * fmijmap
 *
 * PURPOSE:
 * The inverse operation to fmivec: from an index in a 1 dimensional array 
 * (simulating a 2D matrix), retrieve the two index i and j in the original
 * matrix.
 *
 * AUTHOR:
 * Thomas Lavergne METNO/FOU, 04.03.2007
 */
void fmijmap(long elem, unsigned long nx, long *x, long *y) {
   *x = elem/nx; // integers ratio with casting : ceil().
   *y = elem - (*x*nx);
}


int fmalloc_char_vector(char **value, int length) {

    if (length>0) {
	*value = (char *) calloc((size_t) length, sizeof(char));
	if (*value == NULL) return(FM_MEMALL_ERR);
    }
    
    return(FM_OK);
}

/*
 * Added for better support of MITIFF etc...
 *
 * Øystein Godøy, METNO/FOU, 2010-12-02
 */
int fmalloc_uchar_vector(unsigned char **value, int length) {

    if (length>0) {
	*value = (unsigned char *) calloc((size_t) length, sizeof(char));
	if (*value == NULL) return(FM_MEMALL_ERR);
    }
    
    return(FM_OK);
}

int fmalloc_ushort_vector(unsigned short **value, int length) {

    if (length>0) {
	*value = (unsigned short *) calloc((size_t) length, sizeof(unsigned short));
	if (*value == NULL) return(FM_MEMALL_ERR);
    }
    
    return(FM_OK);
}

int fmalloc_int_vector(int **value, int length) {

    if (length>0) {
	*value = (int *) calloc((size_t) length, sizeof(int));
	if (*value == NULL) return(FM_MEMALL_ERR);
    }
    
    return(FM_OK);
}

int fmalloc_float_vector(float **value, int length) {

    if (length>0) {
	*value = (float *) calloc((size_t) length, sizeof(float));
	if (*value == NULL) return(FM_MEMALL_ERR);
    }
    
    return(FM_OK);
}

int fmalloc_double_vector(double **value, int length) {

    if (length>0) {
	*value = (double *) calloc((size_t) length, sizeof(double));
	if (*value == NULL) return(FM_MEMALL_ERR);
    }
    
    return(FM_OK);
}

int fmalloc_byte_2d(char ***value, int rows, int columns) {

    int i=0;

    if (rows>0) {
	*value = (char **) calloc ((size_t) rows, sizeof (char *));
	if (*value == NULL) return(FM_MEMALL_ERR);

	if (columns>0)
	    for (i=0; i<rows; i++) {
		(*value)[i] = 
		    (char *) calloc ((size_t) columns, sizeof (char));
		if ((*value)[i] == NULL ) return(FM_MEMALL_ERR);
	    }
    }

    return(FM_OK);
}

int fmalloc_ubyte_2d(unsigned char ***value, int rows, int columns) {

    int i=0;

    if (rows>0) {
	*value = (unsigned char **) calloc ((size_t) rows, sizeof (unsigned char *));
	if (*value == NULL) return(FM_MEMALL_ERR);

	if (columns>0)
	    for (i=0; i<rows; i++) {
		(*value)[i] = 
		    (unsigned char *) calloc ((size_t) columns, sizeof (unsigned char));
		if ((*value)[i] == NULL ) return(FM_MEMALL_ERR);
	    }
    }

    return(FM_OK);
}

/*
 * NAME:
 * fmalloc_byte_2d_contiguous
 *
 * PURPOSE:
 * To allocate contiguous blocks.
 *
 * AUTHOR:
 * Øystein Godøy, METNO/FOU, 29.03.2009
 */
int fmalloc_byte_2d_contiguous(char ***value, int rows, int columns) {

    int i=0;

    /*
     * Allocate pointers to rows.
     */
    *value = (char **) calloc((size_t) rows, sizeof(char*));
    if (*value == NULL) return(FM_MEMALL_ERR);

    /*
     * Allocate rows and set pointers to them.
     */
    (*value)[0] = (char *) calloc((size_t)(rows*columns),sizeof(char));
    for (i=1;i<rows;i++) {
	(*value)[i] = (*value)[i-1]+(char) columns;
    }

    return(FM_OK);
}

int fmalloc_ushort_2d(unsigned short ***value, int rows, int columns) {

    int i=0;

    if (rows>0) {
	*value = (unsigned short **) calloc ((size_t) rows, sizeof (unsigned short *));
	if (*value == NULL) return(FM_MEMALL_ERR);

	if (columns>0)
	    for (i=0; i<rows; i++) {
		(*value)[i] = 
		    (unsigned short *) calloc ((size_t) columns, sizeof (unsigned short));
		if ((*value)[i] == NULL ) return(FM_MEMALL_ERR);
	    }
    }

    return(FM_OK);
}

int fmalloc_int_2d(int ***value, int rows, int columns) {

    int i=0;

    if (rows>0) {
	*value = (int **) calloc ((size_t) rows, sizeof (int *));
	if (*value == NULL) return(FM_MEMALL_ERR);

	if (columns>0)
	    for (i=0; i<rows; i++) {
		(*value)[i] = 
		    (int *) calloc ((size_t) columns, sizeof (int));
		if ((*value)[i] == NULL ) return(FM_MEMALL_ERR);
	    }
    }

    return(FM_OK);
}

int fmalloc_float_2d(float ***value, int rows, int columns) {

    int i=0;

    if (rows>0) {
	*value = (float **) calloc ((size_t) rows, sizeof (float *));
	if (*value == NULL) return(FM_MEMALL_ERR);

	if (columns>0)
	    for (i=0; i<rows; i++) {
		(*value)[i] = 
		    (float *) calloc ((size_t) columns, sizeof (float));
		if ((*value)[i] == NULL ) return(FM_MEMALL_ERR);
	    }
    }

    return(FM_OK);
}

int fmalloc_double_2d(double ***value, int rows, int columns) {

    int i=0;

    if (rows>0) {
	*value = (double **) calloc ((size_t) rows, sizeof (double *));
	if (*value == NULL) return(FM_MEMALL_ERR);

	if (columns>0)
	    for (i=0; i<rows; i++) {
		(*value)[i] = 
		    (double *) calloc ((size_t) columns, sizeof (double));
		if ((*value)[i] == NULL ) return(FM_MEMALL_ERR);
	    }
    }

    return(FM_OK);
}

int fmfree_char_vector(char *value) {

    if (value) {
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fmfree_uchar_vector(unsigned char *value) {

    if (value) {
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fmfree_ushort_vector(unsigned short *value) {

    if (value) {
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fmfree_int_vector(int *value) {

    if (value) {
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fmfree_float_vector(float *value) {

    if (value) {
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fmfree_double_vector(double *value) {

    if (value) {
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fmfree_byte_2d(char **value, int rows) {

    int i;

    if (value) {
	for (i=0;i<rows;i++) {
	    free(value[i]);
	}
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fmfree_ubyte_2d(unsigned char **value, int rows) {

    int i;

    if (value) {
	for (i=0;i<rows;i++) {
	    free(value[i]);
	}
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fmfree_ushort_2d(unsigned short **value, int rows) {

    int i;

    if (value) {
	for (i=0;i<rows;i++) {
	    free(value[i]);
	}
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fmfree_int_2d(int **value, int rows) {

    int i;

    if (value) {
	for (i=0;i<rows;i++) {
	    free(value[i]);
	}
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fmfree_float_2d(float **value, int rows) {

    int i;

    if (value) {
	for (i=0;i<rows;i++) {
	    free(value[i]);
	}
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fmfree_double_2d(double **value, int rows) {

    int i;

    if (value) {
	for (i=0;i<rows;i++) {
	    free(value[i]);
	}
	free(value);
    } else {
	return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}


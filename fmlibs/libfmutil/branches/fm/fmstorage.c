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
 */

#include <fmutil.h>


/*
 * FUNCTION:
 * ivec
 *
 * PURPOSE:
 * Returns a pointer into the datavector containing the matrix with 
 * image data. This funtion is used when a 2 dimensional matrix is 
 * simulated using a 1 dimensional matrix.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, xx.xx.xxxx
 */
long ivec(long x, long y, unsigned long nx) {
    long i;
    
    i = y*nx+x;
    return(i);
}

int fmalloc_char_vector(char **value, int length) {

    if (length>0) {
	*value = (char *) calloc((size_t) length, sizeof(char));
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

int fmalloc_byte_2D(char ***value, int rows, int columns) {

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


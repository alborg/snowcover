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
 * Øystein Godøy, METNO/FOU, 13.10.2006 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 16.10.2006: Modified name space for libfmio.
 *
 * ID:
 * $Id$
 */ 

#include <fmio.h>
#include <fm_aha_hd.h>
#ifdef FMIO_HAVE_LIBHDF5
#include <hdf5.h>
#endif

int fm_readheader(char *filename, fmio_img *h) {

    char *where="fm_readheader";
    int typefile, ret;
    char buf[FMIO_STRMAXCHARS];
#ifdef FMIO_HAVE_LIBHDF5
    htri_t this_is_hdf5;
#endif
    FILE *fpi;

    ret = 0;
    typefile = 0;

    fpi = fopen(filename,"r");
    if (!fpi) {
	typefile = -1;
    }
    else {
	fgets(buf,FMIO_STRMAXCHARS,fpi);
	if (strncmp(buf,"AHA_METSAT",10) == 0) {  
	    typefile = 1;  
	}
	else {  
	    typefile = 0;  
	}
	fclose(fpi);
    }

#ifdef FMIO_HAVE_LIBHDF5
    if (typefile == 0) {
	this_is_hdf5= H5Fis_hdf5(filename);
	if (this_is_hdf5) {  
	    typefile = 2;  
	} else { 
	    typefile = 0; 
	}
    }
#endif

    if (typefile == 1) {
	if (fm_readMETSATheader(filename, h)) {
	    return(FM_IO_ERR);
	}
    } else if (typefile == 2) {
	/*
	ret = readMIHDF5header(filename, h);
	*/
    } else if (typefile == -1) {
	fprintf(stderr,"\n\tERROR! File %s does not exist.\n",filename);
	return(FM_IO_ERR);
    } else {
	fprintf(stderr,
		"\n\tCould not recognize format of AVHRR file %s \n",filename);
	return(FM_IO_ERR);
    }

    return(FM_OK);
}


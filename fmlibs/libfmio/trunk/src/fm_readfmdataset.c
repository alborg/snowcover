/*
 * NAME:
 * fm_readfmdataset
 *
 * PURPOSE:
 * To read data from various file formats into a fm_dataset structure as
 * defined in libfmutil.
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
 * Øystein Godøy, METNO/FOU, 2011-11-10 
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */ 

#include <fmio.h>
#include <fm_aha_hd.h>
#ifdef FMIO_HAVE_LIBHDF5
#include <hdf5.h>
#endif

int fm_readfmdataset(char *filename, fmdataset *d, fmbool headeronly) {

    int typefile, ret;
    char buf[FMIO_STRMAXCHARS];
#ifdef FMIO_HAVE_LIBHDF5
    htri_t this_is_hdf5;
#endif
    FILE *fpi;
    int i, valpix;

    ret = 0;
    typefile = 0;

    fpi = fopen(filename,"r");
    if (!fpi) {
	typefile = 0;
    }
    else {
	fgets(buf,FMIO_STRMAXCHARS,fpi);
	if (strncmp(buf,"AHA_METSAT",10) == 0) {  
	    typefile = 1;  
	} else {  
	    typefile = 0;  
	}
	fclose(fpi);
    }

#ifdef FMIO_HAVE_LIBHDF5
    this_is_hdf5 = 0;
    if (typefile == 0) {
	this_is_hdf5= H5Fis_hdf5(filename);
	if (this_is_hdf5 == 1) {  
	    typefile = 2;  
	}
	else if (this_is_hdf5 == -1) {  
	    typefile = -1;  
	}
	else { 
	    typefile = 0; 
	}
    }
#endif

    if (typefile == 1) {
        return(FM_IO_ERR); /* while testing */
	if (fm_readMETSATdata(filename, d)) {
	    return(FM_IO_ERR);
	}
    } else if (typefile == 2) {
	if (fm_readCMSAFdata(filename, d, headeronly)) {
	    return(FM_IO_ERR);
	}
    }  else if (typefile == -1) {
	fprintf(stderr,
		"\n  Could not open file or does not exist, %s \n",filename);
	return(FM_IO_ERR);
    }
    else {
	fprintf(stderr,
		"\n  Could not recognize format of AVHRR file %s \n",filename);
	return(FM_IO_ERR);
    }

    return(FM_OK);
}


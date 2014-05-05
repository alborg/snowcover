/*
 * NAME:
 * fm_readdata
 *
 * PURPOSE:
 * To read various satellite formats being used at METNO.
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
 * Øystein Godøy, METNO/FOU, 22.06.2007: Typo corrected when calling
 * fm_readMETSATdata.
 *
 * ID:
 * $Id$
 */ 

#include <fmio.h>
#include <fm_aha_hd.h>
#ifdef FMIO_HAVE_LIBHDF5
#include <hdf5.h>
#endif

int fm_readdata(char *filename, fmio_img *h) {

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
	if (fm_readMETSATdata(filename, h)) {
	    return(FM_IO_ERR);
	}
    } else if (typefile == 2) {
	/*
	if (readMIHDF5data(filename, h)) {
	    return(FM_IO_ERR);
	}
	*/
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

    /*
     * Check the number of valid pixels
     */
    valpix = 0;
    for (i=0;i<h->size;i++) {
	if ((h->image[0])[i] == h->outofimageval) continue;
	valpix++;
    }
    h->cover = valpix*100./h->size;

    return(FM_OK);
}


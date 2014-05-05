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
 * Øystein Godøy, METNO/FOU, 13.10.2006 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 16.10.2006
 * Modified name space for libfmio.
 */

#include <fmio.h>
#include <fm_aha_hd.h>
#ifdef HAVE_HDF5
#include <hdf5.h>
#endif

int fm_readdata(char *filename, fmio_img *h) {

    int typefile, ret;
    char buf[FMIO_STRMAXCHARS];
#ifdef HAVE_HDF5
    htri_t this_is_hdf5;
#endif
    FILE *fpi;

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

#ifdef HAVE_HDF5
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
	if (readMETSATdata(filename, h)) {
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

    return(FM_OK);
}


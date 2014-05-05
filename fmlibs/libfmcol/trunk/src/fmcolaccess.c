/*
 * NAME:
 * fmcolaccess.c
 *
 * PURPOSE:
 * To store collocated stationwise data in NCSA HDF5 structures/objects.
 * This file contains several subroutines, not all of them are properly
 * documented yet, but the sole purpose is either dump or retrieval of
 * stationwise data from HDF5 files.
 *
 * Main dump functions are named by numbers according to which data types
 * to dump. The list of data types supported and their numbers are:
 * 	AVHRR	- 1 - AVHRR data as given by MEOS
 * 	NWP	- 2 - HIRLAM NWP data
 * 	STDAT	- 3 - SYNOP or DRAU data
 * 	SAFCM	- 4 - SAFNWC cloud type product
 * 	SAFSSI	- 5 - SAFOSI SSI product
 * 	SAFDLI	- 6 - SAFOSI DLI product
 * NA
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * NA
 *
 * OUTPUT:
 * RETURN VALUES:
 * 0 - Normal and correct ending.
 * 2 - input/output problem
 * 3 - memory allocation problem
 * 4 - nwp_id problem
 *
 * NOTES:
 * Only adapted for Little endian UNIX time...
 * 
 * Memory leakage will occur when failure is experienced as return to main
 * program sometimes occur before all resources are released, but this
 * should not be major problem...
 *
 * Datatype commit is used, but no compression as this increased file size
 * these data, probably due to increased overhead...
 *
 * BUGS:
 * When read functions are applied to a file not containing the objects
 * requested, it core dumps!!!!!!
 *
 * AUTHOR:
 * Øystein godøy, DNMI/FOU, 11/05/2000
 *
 * MODIFIED:
 * Øystein Godøy, DNMI/FOU, 19/09/2002: Modularized HDF access, and added
 * support for SAFCM...
 * Øystein Godøy, DNMI/FOU, 27.01.2003: Reading implemented, some
 * modifications concerning flexibility should be added later. In future
 * software should be able to proceed even if some objects are missing...
 * Freeing of allocated datastructures are missing currently...
 * Øystein Godøy, DNMI/FOU, 29.04.2003: Added storage without STDAT
 * Øystein Godøy, DNMI/FOU, 09.10.2003: Corrected bug in estimating size
 * of cloud mask/type structures.
 * Øystein Godøy, MI/FOU, 06.11.2003: Due to fmerrmsgs when extracting
 * safcm_data data, a dummy interface is created to circumvent the problem
 * in the current situation. This involves the use of a slimmer data
 * structure not containing the character strings describing the class
 * names. Functions affected are the high level read* functions accessing
 * the safcm_data structure. In this fix, data are transferred from
 * safcm_data to safcm_data2.
 * Øystein Godøy, met.no/FOU, 22.10.2004: Changed NWP interface.
 * Øystein Godøy, met.no/FOU, 20.12.2004: Added dump124.
 * Øystein Godøy, met.no/FOU, 21.12.2004: Changed file handling for no
 * file scenario.
* Øystein Godøy, met.no/FOU, 22.12.2004: Changed compoundAVHRR to reflect
* the new structure...
* Øystein Godøy, met.no/FOU, 23.12.2004: Added support for SAFDLI.
* Øystein Godøy, METNO/FOU, 07.09.2007: Modified count_basegroups due to
* failure when testing in R (the static variables did not get reset when
        * they should)
* Øystein Godøy, METNO/FOU, 28.02.2008: Updated according to the enw
* format used by UFFDA.
* Øystein Godøy, METNO/FOU, 28.02.2008: Added read124.
* Øystein Godøy, METNO/FOU, 30.04.2008: Added getclassnames.
* Øystein Godøy, METNO/FOU, 01.03.2009: Started proper adaption for
* libfmutil, but this will take time...
* Øystein Godøy, METNO/FOU, 2013-04-12: Changed count_basegroups to handle
* old an new (UFFDA) files.
* Øystein Godøy, METNO/FOU, 2013-04-18: Switched to new PPS libraries.
*
* ID:
* $Id$
*/

#include <fmcolaccess.h> 
#ifdef FMCOL_HAVE_LIBHDF5

/*
 * PURPOSE:
 * To dump objects of type AVHRR, NWP and STDAT to a HDF5 file.
 *
 * RETURN:
 * Nonzero values on failure, 0 is success. See code for details on return
 * values...
 */
int dump123(char *filename, char *class, 
        as_data aobs, ns_data nobs, obsstruct sobs){

    char *where="dump123";
    hid_t file, grp;
    herr_t status;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Create a group with data. This group contains all necessary data
     * for all sources (AVHRR, NWP, STDAT, etc). 
     * Need to add attributes...
     */
    grp = H5Gcreate(file, class, 0);
    if (grp < 0) {
        fmerrmsg(where,
                "Could not create a data group in the HDF5 file");
        return(2);
    };

    /*
     * The actual dumping of objects comes here...
     * AVHRR, NWP and STDAT
     */
    if (dumpAVHRR2HDF(file, grp, aobs) < 0) {
        fmerrmsg(where,
                "Could not generate AVHRR object in this group/file");
        return(2);
    }
    if (dumpNWP2HDF(file, grp, nobs) < 0) {
        fmerrmsg(where,
                "Could not generate NWP object in this group/file");
        return(2);
    }
    if (dumpSTDAT2HDF(file, grp, sobs) < 0) {
        fmerrmsg(where,
                "Could not generate STDAT object in this group/file");
        return(2);
    }

    /*
     * Close resources
     */
    status = H5Gclose(grp);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close data group in HDF5 file.");
        return(2);
    };
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(2);
    };

    return(0);
}

/*
 * PURPOSE:
 * To dump objects of type AVHRR, NWP, STDAT and SAFCM to a HDF5 file.
 *
 * RETURN:
 * Nonzero values on failure, 0 is success. See code for details on return
 * values...
 */
#ifdef FMCOL_HAVE_LIBSMHI_SAF
int dump1234(char *filename, char *class, 
        as_data aobs, ns_data nobs, obsstruct sobs, safcm_data cobs){

    char *where="dump1234";
    hid_t file, grp;
    herr_t status;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */
    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Create a group with data. This group contains all necessary data
     * for all sources (AVHRR, NWP, STDAT, etc). 
     * Need to add attributes...
     */
    grp = H5Gcreate(file, class, 0);
    if (grp < 0) {
        fmerrmsg(where,
                "Could not create a data group in the HDF5 file");
        return(2);
    };

    /*
     * The actual dumping of objects comes here...
     * AVHRR, NWP, STDAT and SAFCM
     */
    if (dumpAVHRR2HDF(file, grp, aobs) < 0) {
        fmerrmsg(where,
                "Could not generate AVHRR object in this group/file");
        return(2);
    }
    if (dumpNWP2HDF(file, grp, nobs) < 0) {
        fmerrmsg(where,
                "Could not generate NWP object in this group/file");
        return(2);
    }
    if (dumpSTDAT2HDF(file, grp, sobs) < 0) {
        fmerrmsg(where,
                "Could not generate STDAT object in this group/file");
        return(2);
    }
    if (dumpSAFCM2HDF(file, grp, cobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFCM object in this group/file");
        return(2);
    }

    /*
     * Close resources
     */
    status = H5Gclose(grp);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close data group in HDF5 file.");
        return(2);
    };
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(2);
    };

    return(0);
}

/*
 * PURPOSE:
 * To dump objects of type AVHRR, NWP, STDAT, SAFCM and SAFSSI to a HDF5 file.
 *
 * RETURN:
 * Nonzero values on failure, 0 is success. See code for details on return
 * values...
 */
#ifdef FMCOL_HAVE_LIBOSIHDF5
int dump12345(char *filename, char *class, 
        as_data aobs, ns_data nobs, obsstruct sobs, safcm_data cobs,
        safssi_data iobs){

    char *where="dump12345";
    hid_t file, grp;
    herr_t status;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */
    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Create a group with data. This group contains all necessary data
     * for all sources (AVHRR, NWP, STDAT, etc). 
     * Need to add attributes...
     */
    grp = H5Gcreate(file, class, 0);
    if (grp < 0) {
        fmerrmsg(where,
                "Could not create data group in the HDF5 file");
        return(2);
    };

    /*
     * The actual dumping of objects comes here...
     * AVHRR, NWP, STDAT, SAFCM and SAFSSI
     */
    if (dumpAVHRR2HDF(file, grp, aobs) < 0) {
        fmerrmsg(where,
                "Could not generate AVHRR object in this group/file");
        return(2);
    }
    if (dumpNWP2HDF(file, grp, nobs) < 0) {
        fmerrmsg(where,
                "Could not generate NWP object in this group/file");
        return(2);
    }
    if (dumpSTDAT2HDF(file, grp, sobs) < 0) {
        fmerrmsg(where,
                "Could not generate STDAT object in this group/file");
        return(2);
    }
    if (dumpSAFCM2HDF(file, grp, cobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFCM object in this group/file");
        return(2);
    }
    if (dumpSAFSSI2HDF(file, grp, iobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFSSI object in this group/file");
        return(2);
    }

    /*
     * Close resources
     */
    status = H5Gclose(grp);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close data group in HDF5 file.");
        return(2);
    };
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(2);
    };

    return(0);
}

/*
 * PURPOSE:
 * To dump objects of type AVHRR, NWP, STDAT, SAFCM, SAFSSI SAFDLI to a HDF5 file.
 *
 * RETURN:
 * Nonzero values on failure, 0 is success. See code for details on return
 * values...
 */
int dump123456(char *filename, char *class, 
        as_data aobs, ns_data nobs, obsstruct sobs, safcm_data cobs,
        safssi_data iobs, safdli_data dobs){

    char *where="dump123456";
    hid_t file, grp;
    herr_t status;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */
    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Create a group with data. This group contains all necessary data
     * for all sources (AVHRR, NWP, STDAT, etc). 
     * Need to add attributes...
     */
    grp = H5Gcreate(file, class, 0);
    if (grp < 0) {
        fmerrmsg(where,
                "Could not create a data group in the HDF5 file");
        return(2);
    };

    /*
     * The actual dumping of objects comes here...
     * AVHRR, NWP, STDAT, SAFCM and SAFSSI
     */
    if (dumpAVHRR2HDF(file, grp, aobs) < 0) {
        fmerrmsg(where,
                "Could not generate AVHRR object in this group/file");
        return(2);
    }
    if (dumpNWP2HDF(file, grp, nobs) < 0) {
        fmerrmsg(where,
                "Could not generate NWP object in this group/file");
        return(2);
    }
    if (dumpSTDAT2HDF(file, grp, sobs) < 0) {
        fmerrmsg(where,
                "Could not generate STDAT object in this group/file");
        return(2);
    }
    if (dumpSAFCM2HDF(file, grp, cobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFCM object in this group/file");
        return(2);
    }
    if (dumpSAFSSI2HDF(file, grp, iobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFSSI object in this group/file");
        return(2);
    }
    if (dumpSAFDLI2HDF(file, grp, dobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFDLI object in this group/file");
        return(2);
    }

    /*
     * Close resources
     */
    status = H5Gclose(grp);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close data group in HDF5 file.");
        return(2);
    };
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(2);
    };

    return(0);
}

/*
 * PURPOSE:
 * To dump objects of type AVHRR, NWP, SAFCM and SAFSSI to a HDF5 file.
 *
 * RETURN:
 * Nonzero values on failure, 0 is success. See code for details on return
 * values...
 */
int dump1245(char *filename, char *class, 
        as_data aobs, ns_data nobs, safcm_data cobs,
        safssi_data iobs){

    char *where="dump1245";
    hid_t file, grp;
    herr_t status;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */
    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Create a group with data. This group contains all necessary data
     * for all sources (AVHRR, NWP, STDAT, etc). 
     * Need to add attributes...
     */
    grp = H5Gcreate(file, class, 0);
    if (grp < 0) {
        fmerrmsg(where,
                "Could not create a data group in the HDF5 file");
        return(2);
    };

    /*
     * The actual dumping of objects comes here...
     * AVHRR, NWP, STDAT, SAFCM and SAFSSI
     */
    if (dumpAVHRR2HDF(file, grp, aobs) < 0) {
        fmerrmsg(where,
                "Could not generate AVHRR object in this group/file");
        return(2);
    }
    if (dumpNWP2HDF(file, grp, nobs) < 0) {
        fmerrmsg(where,
                "Could not generate NWP object in this group/file");
        return(2);
    }
    if (dumpSAFCM2HDF(file, grp, cobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFCM object in this group/file");
        return(2);
    }
    if (dumpSAFSSI2HDF(file, grp, iobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFSSI object in this group/file");
        return(2);
    }

    /*
     * Close resources
     */
    status = H5Gclose(grp);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close data group in HDF5 file.");
        return(2);
    };
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(2);
    };

    return(0);
}

/*
 * PURPOSE:
 * To dump objects of type AVHRR, NWP, SAFCM, SAFSSI and SAFDLI to a HDF5 file.
 *
 * RETURN:
 * Nonzero values on failure, 0 is success. See code for details on return
 * values...
 */
int dump12456(char *filename, char *class, 
        as_data aobs, ns_data nobs, safcm_data cobs,
        safssi_data iobs, safdli_data dobs){

    char *where="dump12456";
    hid_t file, grp;
    herr_t status;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */
    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Create a group with data. This group contains all necessary data
     * for all sources (AVHRR, NWP, STDAT, etc). 
     * Need to add attributes...
     */
    grp = H5Gcreate(file, class, 0);
    if (grp < 0) {
        fmerrmsg(where,
                "Could not create a data group in the HDF5 file");
        return(2);
    };

    /*
     * The actual dumping of objects comes here...
     * AVHRR, NWP, STDAT, SAFCM and SAFSSI
     */
    if (dumpAVHRR2HDF(file, grp, aobs) < 0) {
        fmerrmsg(where,
                "Could not generate AVHRR object in this group/file");
        return(2);
    }
    if (dumpNWP2HDF(file, grp, nobs) < 0) {
        fmerrmsg(where,
                "Could not generate NWP object in this group/file");
        return(2);
    }
    if (dumpSAFCM2HDF(file, grp, cobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFCM object in this group/file");
        return(2);
    }
    if (dumpSAFSSI2HDF(file, grp, iobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFSSI object in this group/file");
        return(2);
    }
    if (dumpSAFDLI2HDF(file, grp, dobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFDLI object in this group/file");
        return(2);
    }

    /*
     * Close resources
     */
    status = H5Gclose(grp);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close data group in HDF5 file.");
        return(2);
    };
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(2);
    };

    return(0);
}
#endif /* FMCOL_HAVE_LIBOSIHDF5 */


/*
 * PURPOSE:
 * To dump objects of type AVHRR, NWP, and SAFCM to a HDF5 file.
 *
 * RETURN:
 * Nonzero values on failure, 0 is success. See code for details on return
 * values...
 */
int dump124(char *filename, char *class, 
        as_data aobs, ns_data nobs, safcm_data cobs){

    char *where="dump124";
    hid_t file, grp;
    herr_t status;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */
    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Create a group with data. This group contains all necessary data
     * for all sources (AVHRR, NWP, STDAT, etc). 
     * Need to add attributes...
     */
    grp = H5Gcreate(file, class, 0);
    if (grp < 0) {
        fmerrmsg(where,
                "Could not create data group %s in %s", class, file);
        return(FM_IO_ERR);
    };

    /*
     * The actual dumping of objects comes here...
     * AVHRR, NWP, STDAT, SAFCM and SAFSSI
     */
    if (dumpAVHRR2HDF(file, grp, aobs) < 0) {
        fmerrmsg(where,
                "Could not generate AVHRR object in this group/file");
        return(FM_IO_ERR);
    }
    if (dumpNWP2HDF(file, grp, nobs) < 0) {
        fmerrmsg(where,
                "Could not generate NWP object in this group/file");
        return(FM_IO_ERR);
    }
    if (dumpSAFCM2HDF(file, grp, cobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFCM object in this group/file");
        return(FM_IO_ERR);
    }

    /*
     * Close resources
     */
    status = H5Gclose(grp);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close data group in HDF5 file.");
        return(FM_IO_ERR);
    };
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(FM_IO_ERR);
    };

    return(FM_OK);
}

/*
 * PURPOSE:
 * To dump objects of type STDAT and SAFCM to a HDF5 file.
 *
 * RETURN:
 * Nonzero values on failure, 0 is success. See code for details on return
 * values...
 */
int dump34(char *filename, char *class, 
        obsstruct sobs, safcm_data cobs){

    char *where="dump34";
    hid_t file, grp;
    herr_t status;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Create a group with data. This group contains all necessary data
     * for all sources (AVHRR, NWP, STDAT, etc). 
     * Need to add attributes...
     */
    grp = H5Gcreate(file, class, 0);
    if (grp < 0) {
        fmerrmsg(where,
                "Could not create a data group in the HDF5 file");
        return(2);
    };

    /*
     * The actual dumping of objects comes here...
     * STDAT and SAFCM
     */
    if (dumpSTDAT2HDF(file, grp, sobs) < 0) {
        fmerrmsg(where,
                "Could not generate STDAT object in this group/file");
        return(2);
    }
    if (dumpSAFCM2HDF(file, grp, cobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFCM object in this group/file");
        return(2);
    }

    /*
     * Close resources
     */
    status = H5Gclose(grp);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close data group in HDF5 file.");
        return(2);
    };
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(2);
    };

    return(0);
}

/*
 * PURPOSE:
 * To dump objects of type SAFCM, SAFSSI SAFDLI to a HDF5 file.
 *
 * RETURN:
 * Nonzero values on failure, 0 is success. See code for details on return
 * values...
 */
#ifdef FMCOL_HAVE_LIBOSIHDF5
int dump456(char *filename, char *class, 
        safcm_data cobs, safssi_data iobs, safdli_data dobs){

    char *where="dump456";
    hid_t file, grp;
    herr_t status;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */
    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Create a group with data. This group contains all necessary data
     * for all sources (AVHRR, NWP, STDAT, etc). 
     * Need to add attributes...
     */
    grp = H5Gcreate(file, class, 0);
    if (grp < 0) {
        fmerrmsg(where,
                "Could not create a data group in the HDF5 file");
        return(2);
    };

    /*
     * The actual dumping of objects comes here...
     * SAFCM and SAFSSI
     */
    if (dumpSAFCM2HDF(file, grp, cobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFCM object in this group/file");
        return(2);
    }
    if (dumpSAFSSI2HDF(file, grp, iobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFSSI object in this group/file");
        return(2);
    }
    if (dumpSAFDLI2HDF(file, grp, dobs) < 0) {
        fmerrmsg(where,
                "Could not generate SAFDLI object in this group/file");
        return(2);
    }

    /*
     * Close resources
     */
    status = H5Gclose(grp);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close data group in HDF5 file.");
        return(2);
    };
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(2);
    };

    return(0);
}

/*
 * PURPOSE:
 * To read objects of type AVHRR, NWP, STDAT, SAFCM and SAFSSI from HDF5 file.
 *
 * RETURN:
 * Number of records read, negative on failure.
 */
int read12345(char *filename, skeys *scrit, 
        as_data **aobs, ns_data **nobs, obsstruct **sobs, 
        safcm_data2 **cobs, safssi_data **iobs){

    char *where="read12345";
    char group2check[250];
    int noobs, i, j;
    hid_t file, grp;
    herr_t status;
    cinfo ginfo,dinfo;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Check contents, ie number of base groups (station data sets)
     */
    /*ginfo.scrit = NULL;*/
    if ((*scrit).t_end == 0) (*scrit).t_end = UINT_MAX;
    ginfo.scrit = scrit;
    ginfo.group = (char (*)[NAMELEN]) 
        malloc(MAXGROUPS*sizeof(*(ginfo.group)));
    if (!ginfo.group) {
        fmerrmsg(where,
                "Malloc trouble ginfo");
        return(2);
    }
    if (H5Giterate(file, "/", NULL, count_basegroups, (cinfo *) &ginfo) != 0) {
        fmerrmsg(where,"Max number of groups reached...");
        return(-1);
    }
    if (ginfo.noobs == 0) {
        fmerrmsg(where,"No data found");
        return(-1);
    }

    *aobs = (as_data *) malloc(ginfo.noobs*sizeof(as_data));
    if (! *aobs) {
        fmerrmsg(where,
                "Malloc trouble aobs");
        return(-2);
    }
    *nobs = (ns_data *) malloc(ginfo.noobs*sizeof(ns_data));
    if (! *nobs) {
        fmerrmsg(where,
                "Malloc trouble nobs");
        return(-2);
    }
    *sobs = (obsstruct *) malloc(ginfo.noobs*sizeof(obsstruct));
    if (! *sobs) {
        fmerrmsg(where,
                "Malloc trouble sobs");
        return(-2);
    }
    *cobs = (safcm_data2 *) malloc(ginfo.noobs*sizeof(safcm_data2));
    if (! *cobs) {
        fmerrmsg(where,
                "Malloc trouble cobs");
        return(-2);
    }
    *iobs = (safssi_data *) malloc(ginfo.noobs*sizeof(safssi_data));
    if (! *iobs) {
        fmerrmsg(where,
                "Malloc trouble iobs");
        return(-2);
    }

    /*
     * Iterate on subgroups.
     */
    dinfo.group = (char (*)[NAMELEN]) 
        malloc(MAXDATASETS*sizeof(*(dinfo.group)));
    if (!dinfo.group) {
        fmerrmsg(where,
                "Malloc trouble dinfo");
        return(-2);
    }
    for (i=0;i<ginfo.noobs;i++) {
        grp = H5Gopen(file, ginfo.group[i]);
        if (grp < 0) {
            fmerrmsg(where,
                    "Could not create a data group in the HDF5 file");
            return(-2);
        };

        /*
         * Iterate over the members of the group to get the number of
         * datasets to read and dump, and check whether reading is correct.
         */
        sprintf(group2check,"/%s",ginfo.group[i]);
        if (H5Giterate(grp, group2check, NULL, 
                    count_datasets, (cinfo *) &dinfo) < 0) {
            fmerrmsg(where,"Max number of groups reached...");
            return(-1);
        }
        if (readAVHRR(file, grp, &((*aobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read AVHRR object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readNWP(file, grp, &((*nobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read NWP object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSTDAT(file, grp, &((*sobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read STDAT object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFCMRAW(file, grp, &((*cobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFCM object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFSSI(file, grp, &((*iobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFSSI object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }

        /*
         * Close resources
         */
        status = H5Gclose(grp);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close data group in HDF5 file.");
            return(-2);
        };
    }
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(status);
    };
    status = ginfo.noobs;
    free(dinfo.group);
    free(ginfo.group);

    return(status);
}

/*
 * PURPOSE:
 * To read objects of type AVHRR, NWP, STDAT, SAFCM, SAFSSI and SAFDLI
 * from HDF5 file.
 *
 * RETURN:
 * Number of records read, negative on failure.
 */
int read123456(char *filename, skeys *scrit, 
        as_data **aobs, ns_data **nobs, obsstruct **sobs, 
        safcm_data2 **cobs, safssi_data **iobs, safdli_data **dobs){

    char *where="read123456";
    char group2check[250];
    int noobs, i, j;
    hid_t file, grp;
    herr_t status;
    cinfo ginfo,dinfo;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Check contents, ie number of base groups (station data sets)
     */
    /*ginfo.scrit = NULL;*/
    if ((*scrit).t_end == 0) (*scrit).t_end = UINT_MAX;
    ginfo.scrit = scrit;
    ginfo.group = (char (*)[NAMELEN]) 
        malloc(MAXGROUPS*sizeof(*(ginfo.group)));
    if (!ginfo.group) {
        fmerrmsg(where,
                "Malloc trouble ginfo");
        return(2);
    }
    if (H5Giterate(file, "/", NULL, count_basegroups, (cinfo *) &ginfo) != 0) {
        fmerrmsg(where,"Max number of groups reached...");
        return(-1);
    }
    if (ginfo.noobs == 0) {
        fmerrmsg(where,"No data found");
        return(-1);
    }

    *aobs = (as_data *) malloc(ginfo.noobs*sizeof(as_data));
    if (! *aobs) {
        fmerrmsg(where,
                "Malloc trouble aobs");
        return(-2);
    }
    *nobs = (ns_data *) malloc(ginfo.noobs*sizeof(ns_data));
    if (! *nobs) {
        fmerrmsg(where,
                "Malloc trouble nobs");
        return(-2);
    }
    *sobs = (obsstruct *) malloc(ginfo.noobs*sizeof(obsstruct));
    if (! *sobs) {
        fmerrmsg(where,
                "Malloc trouble sobs");
        return(-2);
    }
    *cobs = (safcm_data2 *) malloc(ginfo.noobs*sizeof(safcm_data2));
    if (! *cobs) {
        fmerrmsg(where,
                "Malloc trouble cobs");
        return(-2);
    }
    *iobs = (safssi_data *) malloc(ginfo.noobs*sizeof(safssi_data));
    if (! *iobs) {
        fmerrmsg(where,
                "Malloc trouble iobs");
        return(-2);
    }
    *dobs = (safdli_data *) malloc(ginfo.noobs*sizeof(safdli_data));
    if (! *dobs) {
        fmerrmsg(where,
                "Malloc trouble dobs");
        return(-2);
    }

    /*
     * Iterate on subgroups.
     */
    dinfo.group = (char (*)[NAMELEN]) 
        malloc(MAXDATASETS*sizeof(*(dinfo.group)));
    if (!dinfo.group) {
        fmerrmsg(where,
                "Malloc trouble dinfo");
        return(-2);
    }
    for (i=0;i<ginfo.noobs;i++) {
        grp = H5Gopen(file, ginfo.group[i]);
        if (grp < 0) {
            fmerrmsg(where,
                    "Could not create a data group in the HDF5 file");
            return(-2);
        };

        /*
         * Iterate over the members of the group to get the number of
         * datasets to read and dump, and check whether reading is correct.
         */
        sprintf(group2check,"/%s",ginfo.group[i]);
        if (H5Giterate(grp, group2check, NULL, 
                    count_datasets, (cinfo *) &dinfo) < 0) {
            fmerrmsg(where,"Max number of groups reached...");
            return(-1);
        }
        if (readAVHRR(file, grp, &((*aobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read AVHRR object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readNWP(file, grp, &((*nobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read NWP object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSTDAT(file, grp, &((*sobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read STDAT object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFCMRAW(file, grp, &((*cobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFCM object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFSSI(file, grp, &((*iobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFSSI object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFDLI(file, grp, &((*dobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFDLI object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }

        /*
         * Close resources
         */
        status = H5Gclose(grp);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close data group in HDF5 file.");
            return(-2);
        };
    }
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(status);
    };
    status = ginfo.noobs;
    free(dinfo.group);
    free(ginfo.group);

    return(status);
}

/*
 * PURPOSE:
 * To read objects of type AVHRR, NWP, SAFCM, SAFSSI and SAFDLI from HDF5 file.
 *
 * RETURN:
 * Number of records read, negative on failure.
 */
int read12456(char *filename, skeys *scrit, 
        as_data **aobs, ns_data **nobs,
        safcm_data2 **cmobs, safssi_data **iobs, safdli_data **dobs){

    char *where="read12456";
    char group2check[250];
    int noobs, i, j;
    hid_t file, grp;
    herr_t status;
    cinfo ginfo,dinfo;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    /*
       printf("%s\n",(*scrit).station);
       printf("%s\n",(*scrit).classname);
       printf("%ld\n",(*scrit).t_start);
       printf("%ld\n",(*scrit).t_end);
       */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Check contents, ie number of base groups (station data sets)
     */
    /*ginfo.scrit = NULL;*/
    if ((*scrit).t_end == 0) (*scrit).t_end = INT_MAX;
    ginfo.scrit = scrit;
    ginfo.group = (char (*)[NAMELEN]) 
        malloc(MAXGROUPS*sizeof(*(ginfo.group)));
    if (!ginfo.group) {
        fmerrmsg(where,
                "Malloc trouble ginfo");
        return(2);
    }
    if (H5Giterate(file, "/", NULL, count_basegroups, (cinfo *) &ginfo) != 0) {
        fmerrmsg(where,"Max number of groups reached...");
        return(-1);
    }
    if (ginfo.noobs == 0) {
        fmerrmsg(where,"No data found");
        return(-1);
    }

    *aobs = (as_data *) malloc(ginfo.noobs*sizeof(as_data));
    if (! *aobs) {
        fmerrmsg(where,
                "Malloc trouble aobs");
        return(-2);
    }
    *nobs = (ns_data *) malloc(ginfo.noobs*sizeof(ns_data));
    if (! *nobs) {
        fmerrmsg(where,
                "Malloc trouble nobs");
        return(-2);
    }
    *cmobs = (safcm_data2 *) malloc(ginfo.noobs*sizeof(safcm_data2));
    if (! *cmobs) {
        fmerrmsg(where,
                "Malloc trouble cmobs");
        return(-2);
    }
    *iobs = (safssi_data *) malloc(ginfo.noobs*sizeof(safssi_data));
    if (! *iobs) {
        fmerrmsg(where,
                "Malloc trouble iobs");
        return(-2);
    }
    *dobs = (safdli_data *) malloc(ginfo.noobs*sizeof(safdli_data));
    if (! *dobs) {
        fmerrmsg(where,
                "Malloc trouble dobs");
        return(-2);
    }

    /*
     * Iterate on subgroups.
     */
    dinfo.group = (char (*)[NAMELEN]) 
        malloc(MAXDATASETS*sizeof(*(dinfo.group)));
    if (!dinfo.group) {
        fmerrmsg(where,
                "Malloc trouble dinfo");
        return(-2);
    }
    for (i=0;i<ginfo.noobs;i++) {
        grp = H5Gopen(file, ginfo.group[i]);
        if (grp < 0) {
            fmerrmsg(where,
                    "Could not create a data group in the HDF5 file");
            return(-2);
        };

        /*
         * Iterate over the members of the group to get the number of
         * datasets to read and dump, and check whether reading is correct.
         */
        sprintf(group2check,"/%s",ginfo.group[i]);
        if (H5Giterate(grp, group2check, NULL, 
                    count_datasets, (cinfo *) &dinfo) < 0) {
            fmerrmsg(where,"Max number of groups reached...");
            return(-1);
        }
        if (readAVHRR(file, grp, &((*aobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read AVHRR object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readNWP(file, grp, &((*nobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read NWP object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFCMRAW(file, grp, &((*cmobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFCMRAW object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFSSI(file, grp, &((*iobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFSSI object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFDLI(file, grp, &((*dobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFDLI object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }

        /*
         * Close resources
         */
        status = H5Gclose(grp);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close data group in HDF5 file.");
            return(-2);
        };
    }
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(status);
    };
    status = ginfo.noobs;
    free(dinfo.group);
    free(ginfo.group);

    return(status);
}


/*
 * PURPOSE:
 * To read objects of type AVHRR, NWP, SAFCM and SAFSSI from HDF5 file.
 *
 * RETURN:
 * Number of records read, negative on failure.
 */
int read1245(char *filename, skeys *scrit, 
        as_data **aobs, ns_data **nobs,
        safcm_data2 **cmobs, safssi_data **iobs) {

    char *where="read1245";
    char group2check[250];
    int noobs, i, j;
    hid_t file, grp;
    herr_t status;
    cinfo ginfo,dinfo;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    /*
       printf("%s\n",(*scrit).station);
       printf("%s\n",(*scrit).classname);
       printf("%ld\n",(*scrit).t_start);
       printf("%ld\n",(*scrit).t_end);
       */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Check contents, ie number of base groups (station data sets)
     */
    /*ginfo.scrit = NULL;*/
    if ((*scrit).t_end == 0) (*scrit).t_end = INT_MAX;
    ginfo.scrit = scrit;
    ginfo.group = (char (*)[NAMELEN]) 
        malloc(MAXGROUPS*sizeof(*(ginfo.group)));
    if (!ginfo.group) {
        fmerrmsg(where,
                "Malloc trouble ginfo");
        return(2);
    }
    if (H5Giterate(file, "/", NULL, count_basegroups, (cinfo *) &ginfo) != 0) {
        fmerrmsg(where,"Max number of groups reached...");
        return(-1);
    }
    if (ginfo.noobs == 0) {
        fmerrmsg(where,"No data found");
        return(-1);
    }

    *aobs = (as_data *) malloc(ginfo.noobs*sizeof(as_data));
    if (! *aobs) {
        fmerrmsg(where,
                "Malloc trouble aobs");
        return(-2);
    }
    *nobs = (ns_data *) malloc(ginfo.noobs*sizeof(ns_data));
    if (! *nobs) {
        fmerrmsg(where,
                "Malloc trouble nobs");
        return(-2);
    }
    *cmobs = (safcm_data2 *) malloc(ginfo.noobs*sizeof(safcm_data2));
    if (! *cmobs) {
        fmerrmsg(where,
                "Malloc trouble cmobs");
        return(-2);
    }
    *iobs = (safssi_data *) malloc(ginfo.noobs*sizeof(safssi_data));
    if (! *iobs) {
        fmerrmsg(where,
                "Malloc trouble iobs");
        return(-2);
    }

    /*
     * Iterate on subgroups.
     */
    dinfo.group = (char (*)[NAMELEN]) 
        malloc(MAXDATASETS*sizeof(*(dinfo.group)));
    if (!dinfo.group) {
        fmerrmsg(where,
                "Malloc trouble dinfo");
        return(-2);
    }
    for (i=0;i<ginfo.noobs;i++) {
        grp = H5Gopen(file, ginfo.group[i]);
        if (grp < 0) {
            fmerrmsg(where,
                    "Could not create a data group in the HDF5 file");
            return(-2);
        };

        /*
         * Iterate over the members of the group to get the number of
         * datasets to read and dump, and check whether reading is correct.
         */
        sprintf(group2check,"/%s",ginfo.group[i]);
        if (H5Giterate(grp, group2check, NULL, 
                    count_datasets, (cinfo *) &dinfo) < 0) {
            fmerrmsg(where,"Max number of groups reached...");
            return(-1);
        }
        if (readAVHRR(file, grp, &((*aobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read AVHRR object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readNWP(file, grp, &((*nobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read NWP object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFCMRAW(file, grp, &((*cmobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFCMRAW object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        /* ØG test at home */
        /*
           printf(" %s\n",cobs.source);
           printf(" %d\n",cobs.vtime);
           printf(" %d\n",cobs.nopix);
           for (j=0;j<FMCOL_NO_CLOUDTYPE_VALUES;j++) {
           printf(" %d - %s\n",j,cobs.description[j]);
           }
           sprintf((*cmobs)[i].source,"%s",cobs.source);
           (*cmobs)[i].vtime = cobs.vtime;
           (*cmobs)[i].nopix = cobs.nopix;
           for (j=0;j<cobs.nopix;j++) {
           (*cmobs)[i].data[j] = cobs.data[j];
           printf(" %d",cobs.data[j]);
           }
           */

        if (readSAFSSI(file, grp, &((*iobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFSSI object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }

        /*
         * Close resources
         */
        status = H5Gclose(grp);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close data group in HDF5 file.");
            return(-2);
        };
    }
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(status);
    };
    status = ginfo.noobs;
    free(dinfo.group);
    free(ginfo.group);

    return(status);
}
#endif /* FMCOL_HAVE_LIBOSIHDF5 */

/*
 * PURPOSE:
 * To read objects of type AVHRR, NWP, STDAT, and SAFCM from HDF5 file.
 *
 * RETURN:
 * Number of records read, negative on failure.
 */
int read1234(char *filename, skeys *scrit, 
        as_data **aobs, ns_data **nobs, obsstruct **sobs, 
        safcm_data2 **cobs){

    char *where="read1234";
    char group2check[250];
    int noobs, i, j;
    hid_t file, grp;
    herr_t status;
    cinfo ginfo,dinfo;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Check contents, ie number of base groups (station data sets)
     */
    /*ginfo.scrit = NULL;*/
    if ((*scrit).t_end == 0) (*scrit).t_end = UINT_MAX;
    ginfo.scrit = scrit;
    ginfo.group = (char (*)[NAMELEN]) 
        malloc(MAXGROUPS*sizeof(*(ginfo.group)));
    if (!ginfo.group) {
        fmerrmsg(where,
                "Malloc trouble ginfo");
        return(2);
    }
    if (H5Giterate(file, "/", NULL, count_basegroups, (cinfo *) &ginfo) != 0) {
        fmerrmsg(where,"Max number of groups reached...");
        return(-1);
    }
    if (ginfo.noobs == 0) {
        fmerrmsg(where,"No data found");
        return(-1);
    }

    *aobs = (as_data *) malloc(ginfo.noobs*sizeof(as_data));
    if (! *aobs) {
        fmerrmsg(where,
                "Malloc trouble aobs");
        return(-2);
    }
    *nobs = (ns_data *) malloc(ginfo.noobs*sizeof(ns_data));
    if (! *nobs) {
        fmerrmsg(where,
                "Malloc trouble nobs");
        return(-2);
    }
    *sobs = (obsstruct *) malloc(ginfo.noobs*sizeof(obsstruct));
    if (! *sobs) {
        fmerrmsg(where,
                "Malloc trouble sobs");
        return(-2);
    }
    *cobs = (safcm_data2 *) malloc(ginfo.noobs*sizeof(safcm_data2));
    if (! *cobs) {
        fmerrmsg(where,
                "Malloc trouble cobs");
        return(-2);
    }

    /*
     * Iterate on subgroups.
     */
    dinfo.group = (char (*)[NAMELEN]) 
        malloc(MAXDATASETS*sizeof(*(dinfo.group)));
    if (!dinfo.group) {
        fmerrmsg(where,
                "Malloc trouble dinfo");
        return(-2);
    }
    for (i=0;i<ginfo.noobs;i++) {
        grp = H5Gopen(file, ginfo.group[i]);
        if (grp < 0) {
            fmerrmsg(where,
                    "Could not create a data group in the HDF5 file");
            return(-2);
        };

        /*
         * Iterate over the members of the group to get the number of
         * datasets to read and dump, and check whether reading is correct.
         */
        sprintf(group2check,"/%s",ginfo.group[i]);
        if (H5Giterate(grp, group2check, NULL, 
                    count_datasets, (cinfo *) &dinfo) < 0) {
            fmerrmsg(where,"Max number of groups reached...");
            return(-1);
        }
        if (readAVHRR(file, grp, &((*aobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read AVHRR object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readNWP(file, grp, &((*nobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read NWP object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSTDAT(file, grp, &((*sobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read STDAT object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFCMRAW(file, grp, &((*cobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFCM object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }

        /*
         * Close resources
         */
        status = H5Gclose(grp);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close data group in HDF5 file.");
            return(-2);
        };
    }
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(status);
    };
    status = ginfo.noobs;
    free(dinfo.group);
    free(ginfo.group);

    return(status);
}

/*
 * PURPOSE:
 * To read objects of type AVHRR, NWP, and SAFCM from HDF5 file.
 *
 * RETURN:
 * Number of records read, negative on failure.
 */
int read124(char *filename, skeys *scrit, 
        as_data **aobs, ns_data **nobs, safcm_data2 **cobs) {

    char *where="read1234";
    char group2check[250];
    int noobs, i, j;
    hid_t file, grp;
    herr_t status;
    cinfo ginfo,dinfo;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Check contents, ie number of base groups (station data sets)
     */
    /*ginfo.scrit = NULL;*/
    if ((*scrit).t_end == 0) (*scrit).t_end = UINT_MAX;
    ginfo.scrit = scrit;
    ginfo.group = (char (*)[NAMELEN]) 
        malloc(MAXGROUPS*sizeof(*(ginfo.group)));
    if (!ginfo.group) {
        fmerrmsg(where,
                "Malloc trouble ginfo");
        return(2);
    }
    if (H5Giterate(file, "/", NULL, count_basegroups, (cinfo *) &ginfo) != 0) {
        fmerrmsg(where,"Max number of groups reached...");
        return(-1);
    }
    if (ginfo.noobs == 0) {
        fmerrmsg(where,"No data found");
        return(-1);
    }

    *aobs = (as_data *) malloc(ginfo.noobs*sizeof(as_data));
    if (! *aobs) {
        fmerrmsg(where, "Malloc trouble aobs");
        return(-2);
    }
    *nobs = (ns_data *) malloc(ginfo.noobs*sizeof(ns_data));
    if (! *nobs) {
        fmerrmsg(where, "Malloc trouble nobs");
        return(-2);
    }
    *cobs = (safcm_data2 *) malloc(ginfo.noobs*sizeof(safcm_data2));
    if (! *cobs) {
        fmerrmsg(where, "Malloc trouble cobs");
        return(-2);
    }

    /*
     * Iterate on subgroups.
     */
    dinfo.group = (char (*)[NAMELEN]) 
        malloc(MAXDATASETS*sizeof(*(dinfo.group)));
    if (!dinfo.group) {
        fmerrmsg(where, "Malloc trouble dinfo");
        return(-2);
    }
    for (i=0;i<ginfo.noobs;i++) {
        grp = H5Gopen(file, ginfo.group[i]);
        if (grp < 0) {
            fmerrmsg(where,
                    "Could not create a data group in the HDF5 file");
            return(-2);
        };

        /*
         * Iterate over the members of the group to get the number of
         * datasets to read and dump, and check whether reading is correct.
         */
        sprintf(group2check,"/%s",ginfo.group[i]);
        if (H5Giterate(grp, group2check, NULL, 
                    count_datasets, (cinfo *) &dinfo) < 0) {
            fmerrmsg(where,"Max number of groups reached...");
            return(-1);
        }
        if (readAVHRR(file, grp, &((*aobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read AVHRR object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readNWP(file, grp, &((*nobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read NWP object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFCMRAW(file, grp, &((*cobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFCM object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }

        /*
         * Close resources
         */
        status = H5Gclose(grp);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close data group in HDF5 file.");
            return(-2);
        };
    }
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(status);
    };
    status = ginfo.noobs;
    free(dinfo.group);
    free(ginfo.group);

    return(status);
}

#endif /* FMCOL_HAVE_LIBSMHI_SAF */


/*
 * PURPOSE:
 * To read objects of type AVHRR, NWP and STDAT from HDF5 file.
 *
 * RETURN:
 * Number of records read, negative on failure.
 */
int read123(char *filename, skeys *scrit, 
        as_data **aobs, ns_data **nobs, obsstruct **sobs) {

    char *where="read123";
    char group2check[250];
    int noobs, i, j;
    hid_t file, grp;
    herr_t status;
    cinfo ginfo,dinfo;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,"Could not open %s", filename);
        return(file);
    }

    /*
     * Check contents, ie number of base groups (station data sets)
     */
    /*ginfo.scrit = NULL;*/
    if ((*scrit).t_end == 0) (*scrit).t_end = UINT_MAX;
    ginfo.scrit = scrit;
    ginfo.group = (char (*)[NAMELEN]) 
        malloc(MAXGROUPS*sizeof(*(ginfo.group)));
    if (!ginfo.group) {
        fmerrmsg(where,"Trouble allocating ginfo");
        return(2);
    }
    if (H5Giterate(file, "/", NULL, count_basegroups, (cinfo *) &ginfo) != 0) {
        fmerrmsg(where,"Max number of groups reached...");
        return(-1);
    }
    if (ginfo.noobs == 0) {
        fmerrmsg(where,"No data found");
        return(-1);
    }
    fmlogmsg(where,"Found %d groups...", ginfo.noobs);

    *aobs = (as_data *) malloc(ginfo.noobs*sizeof(as_data));
    if (! *aobs) {
        fmerrmsg(where, "Malloc trouble aobs");
        return(-2);
    }
    *nobs = (ns_data *) malloc(ginfo.noobs*sizeof(ns_data));
    if (! *nobs) {
        fmerrmsg(where, "Malloc trouble nobs");
        return(-2);
    }
    *sobs = (obsstruct *) malloc(ginfo.noobs*sizeof(obsstruct));
    if (! *sobs) {
        fmerrmsg(where, "Malloc trouble sobs");
        return(-2);
    }

    /*
     * Iterate on subgroups.
     */
    dinfo.group = (char (*)[NAMELEN]) 
        malloc(MAXDATASETS*sizeof(*(dinfo.group)));
    if (!dinfo.group) {
        fmerrmsg(where, "Malloc trouble dinfo");
        return(-2);
    }
    fmlogmsg(where,"Iterating and reading subgroups...");
    for (i=0;i<ginfo.noobs;i++) {
        /*
           printf("%d ",i);
           */
        grp = H5Gopen(file, ginfo.group[i]);
        if (grp < 0) {
            fmerrmsg(where,
                    "Could not create a data group in the HDF5 file");
            return(-2);
        };

        /*
         * Iterate over the members of the group to get the number of
         * datasets to read and dump, and check whether reading is correct.
         */
        sprintf(group2check,"/%s",ginfo.group[i]);
        if (H5Giterate(grp, group2check, NULL, 
                    count_datasets, (cinfo *) &dinfo) < 0) {
            fmerrmsg(where,"Max number of groups reached...");
            return(-1);
        }
        if (readAVHRR(file, grp, &((*aobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read AVHRR object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readNWP(file, grp, &((*nobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read NWP object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSTDAT(file, grp, &((*sobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read STDAT object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }

        /*
         * Close resources
         */
        status = H5Gclose(grp);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close data group in HDF5 file.");
            return(-2);
        };
    }
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(status);
    };
    status = ginfo.noobs;
    free(dinfo.group);
    free(ginfo.group);

    return(status);
}

/*
 * PURPOSE:
 * To read objects of type STDAT and SAFCM from HDF5 file.
 *
 * RETURN:
 * Number of records read, negative on failure.
 */
#ifdef FMCOL_HAVE_LIBSMHI_SAF
int read34(char *filename, skeys *scrit, obsstruct **sobs, safcm_data2 **cobs){

    char *where="read34";
    char group2check[250];
    int noobs, i, j;
    hid_t file, grp;
    herr_t status;
    cinfo ginfo,dinfo;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Check contents, ie number of base groups (station data sets)
     */
    if ((*scrit).t_end == 0) (*scrit).t_end = UINT_MAX;
    ginfo.scrit = scrit;
    ginfo.group = (char (*)[NAMELEN]) 
        malloc(MAXGROUPS*sizeof(*(ginfo.group)));
    if (!ginfo.group) {
        fmerrmsg(where,
                "Malloc trouble ginfo");
        return(2);
    }
    if (H5Giterate(file, "/", NULL, count_basegroups, (cinfo *) &ginfo) != 0) {
        fmerrmsg(where,"Max number of groups reached...");
        return(-1);
    }
    if (ginfo.noobs == 0) {
        fmerrmsg(where,"No data found");
        return(-1);
    }

    *sobs = (obsstruct *) malloc(ginfo.noobs*sizeof(obsstruct));
    if (! *sobs) {
        fmerrmsg(where,
                "Malloc trouble sobs");
        return(-2);
    }
    *cobs = (safcm_data2 *) malloc(ginfo.noobs*sizeof(safcm_data2));
    if (! *cobs) {
        fmerrmsg(where,
                "Malloc trouble cobs");
        return(-2);
    }

    /*
     * Iterate on subgroups.
     */
    dinfo.group = (char (*)[NAMELEN]) 
        malloc(MAXDATASETS*sizeof(*(dinfo.group)));
    if (!dinfo.group) {
        fmerrmsg(where,
                "Malloc trouble dinfo");
        return(-2);
    }
    for (i=0;i<ginfo.noobs;i++) {
        grp = H5Gopen(file, ginfo.group[i]);
        if (grp < 0) {
            fmerrmsg(where,
                    "Could not create a data group in the HDF5 file");
            return(-2);
        };

        /*
         * Iterate over the members of the group to get the number of
         * datasets to read and dump, and check whether reading is correct.
         */
        sprintf(group2check,"/%s",ginfo.group[i]);
        if (H5Giterate(grp, group2check, NULL, 
                    count_datasets, (cinfo *) &dinfo) < 0) {
            fmerrmsg(where,"Max number of groups reached...");
            return(-1);
        }
        if (readSTDAT(file, grp, &((*sobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read STDAT object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFCMRAW(file, grp, &((*cobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFCM object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }

        /*
         * Close resources
         */
        status = H5Gclose(grp);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close data group in HDF5 file.");
            return(-2);
        };
    }
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(status);
    };
    status = ginfo.noobs;
    free(dinfo.group);
    free(ginfo.group);

    return(status);
}

/*
 * PURPOSE:
 * To read objects of type SAFCM, SAFSSI and SAFDLI from HDF5 file.
 *
 * RETURN:
 * Number of records read, negative on failure.
 */
#ifdef FMCOL_HAVE_LIBOSIHDF5
int read456(char *filename, skeys *scrit, 
        safcm_data2 **cobs, safssi_data **iobs, safdli_data **dobs){

    char *where="read456";
    char group2check[250];
    int noobs, i, j;
    hid_t file, grp;
    herr_t status;
    cinfo ginfo,dinfo;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,
                "Could not open file");
        return(file);
    }

    /*
     * Check contents, ie number of base groups (station data sets)
     */
    /*ginfo.scrit = NULL;*/
    if ((*scrit).t_end == 0) (*scrit).t_end = UINT_MAX;
    ginfo.scrit = scrit;
    ginfo.group = (char (*)[NAMELEN]) 
        malloc(MAXGROUPS*sizeof(*(ginfo.group)));
    if (!ginfo.group) {
        fmerrmsg(where,
                "Malloc trouble ginfo");
        return(2);
    }
    if (H5Giterate(file, "/", NULL, count_basegroups, (cinfo *) &ginfo) != 0) {
        fmerrmsg(where,"Max number of groups reached...");
        return(-1);
    }
    if (ginfo.noobs == 0) {
        fmerrmsg(where,"No data found");
        return(-1);
    }

    *cobs = (safcm_data2 *) malloc(ginfo.noobs*sizeof(safcm_data2));
    if (! *cobs) {
        fmerrmsg(where,
                "Malloc trouble cobs");
        return(-2);
    }
    *iobs = (safssi_data *) malloc(ginfo.noobs*sizeof(safssi_data));
    if (! *iobs) {
        fmerrmsg(where,
                "Malloc trouble iobs");
        return(-2);
    }
    *dobs = (safdli_data *) malloc(ginfo.noobs*sizeof(safdli_data));
    if (! *dobs) {
        fmerrmsg(where,
                "Malloc trouble dobs");
        return(-2);
    }

    /*
     * Iterate on subgroups.
     */
    dinfo.group = (char (*)[NAMELEN]) 
        malloc(MAXDATASETS*sizeof(*(dinfo.group)));
    if (!dinfo.group) {
        fmerrmsg(where,
                "Malloc trouble dinfo");
        return(-2);
    }
    for (i=0;i<ginfo.noobs;i++) {
        grp = H5Gopen(file, ginfo.group[i]);
        if (grp < 0) {
            fmerrmsg(where,
                    "Could not create a data group in the HDF5 file");
            return(-2);
        };

        /*
         * Iterate over the members of the group to get the number of
         * datasets to read and dump, and check whether reading is correct.
         */
        sprintf(group2check,"/%s",ginfo.group[i]);
        if (H5Giterate(grp, group2check, NULL, 
                    count_datasets, (cinfo *) &dinfo) < 0) {
            fmerrmsg(where,"Max number of groups reached...");
            return(-1);
        }
        if (readSAFCMRAW(file, grp, &((*cobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFCM object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFSSI(file, grp, &((*iobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFSSI object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }
        if (readSAFDLI(file, grp, &((*dobs)[i])) < 0) {
            fmerrmsg(where,
                    "Could not read SAFDLI object in this group/file");
            fprintf(stderr," Objects in this group:\n");
            for (j=0;j<dinfo.noobs;j++) {
                fprintf(stderr," %s\n", dinfo.group[j]);
            }
            return(-2);
        }

        /*
         * Close resources
         */
        status = H5Gclose(grp);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close data group in HDF5 file.");
            return(-2);
        };
    }
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,
                "Could not create (new?) HDF5 file");
        fprintf(stderr," %s\n", filename);
        return(status);
    };
    status = ginfo.noobs;
    free(dinfo.group);
    free(ginfo.group);

    return(status);
}
#endif /* FMCOL_HAVE_LIBOSIHDF5 */
#endif /* FMCOL_HAVE_LIBSMHI_SAF */

/*
 * PURPOSE:
 * To check number of groups in HDF5 file. The main use of this is in the
 * interface with R. 
 *
 * RETURN:
 * Number of records read, negative on failure.
 */
int checknorec(char *filename, skeys *scrit){

    char *where="checknorec";
    hid_t file;
    herr_t status;
    cinfo ginfo;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    /*
       fmlogmsg(where,"Trying to open file %s...", filename);
       */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,"Could not open %s", filename);
        return(file);
    }

    /*
     * Check contents, ie number of base groups (station data sets)
     */
    /*
       fmlogmsg(where,"Looping through contents...");
       */
    if ((*scrit).t_end == 0) (*scrit).t_end = UINT_MAX;
    ginfo.scrit = scrit;
    ginfo.group = (char (*)[NAMELEN]) malloc(MAXGROUPS*sizeof(*(ginfo.group)));
    if (!ginfo.group) {
        fmerrmsg(where,
                "Malloc trouble ginfo");
        return(2);
    }
    if (H5Giterate(file, "/", NULL, count_basegroups, (cinfo *) &ginfo) != 0) {
        fmerrmsg(where,"Max number of groups reached...");
        return(-1);
    }
    /*
       fmlogmsg(where,"Looping through contents after iterate...");
       */
    if (ginfo.noobs == 0) {
        fmlogmsg(where,
                "No data found for \n\t\tclassname [%s] \n\tand \n\t\tstation [%s]",
                (*scrit).classname,(*scrit).station);
        return(-1);
    }
    fmlogmsg(where,"Found %d groups in file", ginfo.noobs);

    /*
       fmlogmsg(where,"Closing file %s...", filename);
       */
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,"Could not close %s",filename);
        return(status);
    };
    status = ginfo.noobs;
    free(ginfo.group);

    return(status);
}

/*
 * PURPOSE:
 * To get all classnames within the HDF5 file.
 *
 * RETURN:
 * Number of records read, negative on failure.
 */
int getclassnames(char *filename, cnames *cn){

    char *where="getclassnames";
    hid_t file;
    herr_t status;
    cinfo ginfo;
    char **mychars;
    char *mypt;
    int i, j, k;

    /*
     * Control fmerrmsg handling.
     */
    /*
       H5Eset_auto(NULL,NULL);
       */

    /*
     * Open file
     */
    if ((file = openFile(filename)) < 0) {
        fmerrmsg(where,"Could not open %s", filename);
        return(file);
    }

    /*
     * Check contents, ie number of base groups (station data sets), and
     * extract classnames...
     */
    ginfo.scrit = NULL;
    ginfo.group = (char (*)[NAMELEN]) malloc(MAXGROUPS*sizeof(*(ginfo.group)));
    if (!ginfo.group) {
        fmerrmsg(where,
                "Malloc trouble ginfo");
        return(2);
    }
    if (H5Giterate(file, "/", NULL, count_basegroups, (cinfo *) &ginfo) != 0) {
        fmerrmsg(where,"Max number of groups reached...");
        return(-1);
    }
    if (ginfo.noobs == 0) {
        fmlogmsg(where,"No classnames found...");
        return(-1);
    }
    fmlogmsg(where,"Found %d groups in file", ginfo.noobs);

    if (fmalloc_byte_2d(&mychars,256,NAMELEN) != FM_OK) {
        fmerrmsg(where,"Could not allocate name array");
        return(FM_MEMALL_ERR);
    }
    cn->noobs = 0;
    for (i=0;i<ginfo.noobs;i++) {
        mypt = strtok(ginfo.group[i]," ");
        if (i==0) {
            sprintf(mychars[0],"%s",mypt);
            cn->noobs++;
            continue;
        }
        for (j=0;j<cn->noobs;j++) {
            if (strstr(mychars[j],mypt)) {
                break;
            }
            if (j == cn->noobs-1){
                sprintf(mychars[cn->noobs],"%s",mypt);
                cn->noobs++;
                if (cn->noobs == 256) {
                    fmerrmsg(where,"Increase mychars array");
                    return(FM_MEMALL_ERR);
                }
            }
        }
    }
    if (fmalloc_byte_2d(&(cn->name),cn->noobs,NAMELEN) != FM_OK) {
        fmerrmsg(where,"Could not allocate name array");
        return(FM_MEMALL_ERR);
    }
    for (i=0;i<cn->noobs;i++) {
        sprintf((cn->name)[i],"%s",mychars[i]);
    }
    fmfree_byte_2d(mychars,256);

    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,"Could not close %s",filename);
        return(status);
    };
    status = ginfo.noobs;
    free(ginfo.group);

    return(status);
}

/*
 * PURPOSE:
 * To dump AVHRR station data through a compound data type for 
 * AVHRR data structures to a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
herr_t dumpAVHRR2HDF(hid_t file, hid_t grp, as_data aobs){

    char *where="dumpAVHRR2HDF";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status = -1;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(status);
    };

    /* 
     * Create compound datatype
     */
    if ((datatype = H5Topen(file,"avhrr_dt")) < 0) {
        datatype = compoundAVHRR(aobs.nopix);
        if (datatype < 0) {
            fmerrmsg(where,
                    "Could not create datatype");
            return(status);
        };
        if (H5Tcommit(file,"avhrr_dt",datatype) < 0) {
            fmerrmsg(where,
                    "Could not commit avhrr_dt");
            return(status);
        }
    } 

    /*
     * Create a dataset for the AVHRR data structure.
     */
    dataset = H5Dcreate(grp, "AVHRR", datatype, dataspace,
            H5P_DEFAULT);
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Write AVHRR station data to the newly created dataset.
     */
    status = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, &aobs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not write data structure to file");
        return(status);
    };

    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}

/*
 * PURPOSE:
 * To dump NWP station data through a compound data type for 
 * NWP data structures to a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
herr_t dumpNWP2HDF(hid_t file, hid_t grp, ns_data nobs){

    char *where="dumpNWP2HDF";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(status);
    };

    /* 
     * Create compound datatype
     */
    if ((datatype = H5Topen(file,"nwp_dt")) < 0) {
        datatype = compoundNWP(nobs.nopix);
        if (datatype < 0) {
            fmerrmsg(where,
                    "Could not create datatype");
            return(status);
        };
        if (H5Tcommit(file,"nwp_dt",datatype) < 0) {
            fmerrmsg(where,
                    "Could not commit nwp_dt");
            return(status);
        }
    } 

    /*
     * Create a dataset for the NWP data structure.
     */
    dataset = H5Dcreate(grp, "NWP", datatype, dataspace,
            H5P_DEFAULT);
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Write NWP station data to the newly created dataset.
     */
    status = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, &nobs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not write data structure to file");
        return(status);
    };

    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}

/*
 * PURPOSE:
 * To dump STDAT station data through a compound data type for 
 * STDAT data structures to a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
herr_t dumpSTDAT2HDF(hid_t file, hid_t grp, obsstruct sobs){

    char *where="dumpSTDAT2HDF";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(status);
    };

    /* 
     * Create compound datatype
     */
    if ((datatype = H5Topen(file,"stdat_dt")) < 0) {
        datatype = compoundSTDAT();
        if (datatype < 0) {
            fmerrmsg(where,
                    "Could not create datatype");
            return(status);
        };
        if (H5Tcommit(file,"stdat_dt",datatype) < 0) {
            fmerrmsg(where,
                    "Could not commit stdat_dt");
            return(status);
        }
    } 

    /*
     * Create a dataset for the STDAT data structure.
     */
    dataset = H5Dcreate(grp, "STDAT", datatype, dataspace,
            H5P_DEFAULT);
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Write STDAT station data to the newly created dataset.
     */
    status = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, &sobs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not write data structure to file");
        return(status);
    };

    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}

/*
 * PURPOSE:
 * To dump SAFCM station data through a compound data type for 
 * SAFCM data structures to a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
#ifdef FMCOL_HAVE_LIBSMHI_SAF
herr_t dumpSAFCM2HDF(hid_t file, hid_t grp, safcm_data cmobs){

    char *where="dumpSAFCM2HDF";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(status);
    };

    /* 
     * Create compound datatype
     */
    if ((datatype = H5Topen(file,"safcm_dt")) < 0) {
        datatype = compoundSAFCM(cmobs.nopix);
        if (datatype < 0) {
            fmerrmsg(where,
                    "Could not create datatype");
            return(status);
        };
        if (H5Tcommit(file,"safcm_dt",datatype) < 0) {
            fmerrmsg(where,
                    "Could not commit safcm_dt");
            return(status);
        }
    } 

    /*
     * Create a dataset for the SAFCM data structure.
     */
    dataset = H5Dcreate(grp, "SAFCM", datatype, dataspace,
            H5P_DEFAULT);
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Write SAFCM station data to the newly created dataset.
     */
    status = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, &cmobs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not write data structure to file");
        return(status);
    };

    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}
#endif /* FCMOL_HAVE_LIBSMHI_SAF */

/*
 * PURPOSE:
 * To dump SAFSSI station data through a compound data type for 
 * SAFCM data structures to a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
#ifdef FMCOL_HAVE_LIBOSIHDF5
herr_t dumpSAFSSI2HDF(hid_t file, hid_t grp, safssi_data ssiobs){

    char *where="dumpSAFSSI2HDF";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(status);
    };

    /* 
     * Create compound datatype
     */
    if ((datatype = H5Topen(file,"safssi_dt")) < 0) {
        datatype = compoundSAFSSI(ssiobs.nopix);
        if (datatype < 0) {
            fmerrmsg(where,
                    "Could not create datatype");
            return(status);
        };
        if (H5Tcommit(file,"safssi_dt",datatype) < 0) {
            fmerrmsg(where,
                    "Could not commit safssi_dt");
            return(status);
        }
    } 

    /*
     * Create a dataset for the SAFSSI data structure.
     */
    dataset = H5Dcreate(grp, "SAFSSI", datatype, dataspace,
            H5P_DEFAULT);
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Write SAFSSI station data to the newly created dataset.
     */
    status = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, &ssiobs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not write data structure to file");
        return(status);
    };

    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}

/*
 * PURPOSE:
 * To dump SAFDLI station data through a compound data type for 
 * SAFCM data structures to a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
herr_t dumpSAFDLI2HDF(hid_t file, hid_t grp, safdli_data dliobs){

    char *where="dumpSAFDLI2HDF";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(status);
    };

    /* 
     * Create compound datatype
     */
    if ((datatype = H5Topen(file,"safdli_dt")) < 0) {
        datatype = compoundSAFDLI(dliobs.nopix);
        if (datatype < 0) {
            fmerrmsg(where,
                    "Could not create datatype");
            return(status);
        };
        if (H5Tcommit(file,"safdli_dt",datatype) < 0) {
            fmerrmsg(where,
                    "Could not commit safdli_dt");
            return(status);
        }
    } 

    /*
     * Create a dataset for the SAFDLI data structure.
     */
    dataset = H5Dcreate(grp, "SAFDLI", datatype, dataspace,
            H5P_DEFAULT);
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Write SAFDLI station data to the newly created dataset.
     */
    status = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, &dliobs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not write data structure to file");
        return(status);
    };

    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}
#endif /* FCMOL_HAVE_LIBOSIHDF5 */

/*
 * PURPOSE:
 * To read STDAT station data through a compound data type for 
 * STDAT data structures from a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
herr_t readSTDAT(hid_t file, hid_t grp, obsstruct *obs){

    char *where="readSTDAT";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(dataspace);
    };

    /* 
     * Get compound datatype from file
     */
    if ((datatype = H5Topen(file,"stdat_dt")) < 0) {
        fmerrmsg(where,
                "Could not get datatype");
        return(datatype);
    } 

    /*
     * Get dataset for the STDAT data structure.
     */
    dataset = H5Dopen(grp, "STDAT");
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Read STDAT station data to the data structuredata structure
     */
    status = H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, obs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not read data structure from file");
        return(status);
    };

    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}

/*
 * PURPOSE:
 * To read AVHRR station data through a compound data type for 
 * AVHRR data structures from a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
herr_t readAVHRR(hid_t file, hid_t grp, as_data *obs){

    char *where="readAVHRR";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(dataspace);
    };

    /* 
     * Get compound datatype from file
     */
    if ((datatype = H5Topen(file,"avhrr_dt")) < 0) {
        fmerrmsg(where,
                "Could not get datatype");
        return(datatype);
    } 

    /*
     * Get dataset for the AVHRR data structure.
     */
    dataset = H5Dopen(grp, "AVHRR");
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Read AVHRR station data to the data structuredata structure
     */
    status = H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, obs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not read data structure from file");
        return(status);
    };

    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}

/*
 * PURPOSE:
 * To read NWP station data through a compound data type for 
 * NWP data structures from a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
herr_t readNWP(hid_t file, hid_t grp, ns_data *obs){

    char *where="readNWP";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(dataspace);
    };

    /* 
     * Get compound datatype from file
     */
    if ((datatype = H5Topen(file,"nwp_dt")) < 0) {
        fmerrmsg(where,
                "Could not get datatype");
        return(datatype);
    } 

    /*
     * Get dataset for the NWP data structure.
     */
    dataset = H5Dopen(grp, "NWP");
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Read NWP station data to the data structuredata structure
     */
    status = H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, obs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not read data structure from file");
        return(status);
    };

    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}

/*
 * PURPOSE:
 * To read SAFCM station data through a compound data type for 
 * SAFCM data structures from a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
#ifdef FMCOL_HAVE_LIBSMHI_SAF
herr_t readSAFCM(hid_t file, hid_t grp, safcm_data2 *obs){

    int j;
    char *where="readSAFCM";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;
    safcm_data myobs;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(dataspace);
    };

    /* 
     * Get compound datatype from file
     */
    if ((datatype = H5Topen(file,"safcm_dt")) < 0) {
        fmerrmsg(where,
                "Could not get datatype");
        return(datatype);
    } 

    /*
     * Get dataset for the SAFCM data structure.
     */
    dataset = H5Dopen(grp, "SAFCM");
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Read SAFCM station data to the data structuredata structure
     */
    status = H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, &myobs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not read data structure from file");
        return(status);
    };
    sprintf((*obs).source,"%s",myobs.source);
    (*obs).vtime = myobs.vtime;
    (*obs).nopix = myobs.nopix;
    for (j=0;j<myobs.nopix;j++) {
        (*obs).data[j] = myobs.data[j];
    }


    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}

/*
 * PURPOSE:
 * To read SAFCM station data through a compound data type for 
 * SAFCM data structures from a HDF5 file already opened and containing a
 * defined group.
 * This is created due to memory trouble when using the standard interface
 * readSAFCM. It is currently a test...
 *
 * RETURN:
 * Negative values on failure, else success.
 */
herr_t readSAFCMRAW(hid_t file, hid_t grp, safcm_data2 *obs){

    int j;
    char *where="readSAFCMRAW";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(dataspace);
    };

    /* 
     * Get compound datatype from file
     */
    /*
       if ((datatype = H5Topen(file,"safcm_dt")) < 0) {
       fmerrmsg(where,
       "Could not get datatype");
       return(datatype);
       }
       */
    if ((datatype = compoundSAFCMRAW(BOXSIZE2D)) < 0) {
        fmerrmsg(where,
                "Could not create datatype");
        return(datatype);
    } 


    /*
     * Get dataset for the SAFCM data structure.
     */
    dataset = H5Dopen(grp, "SAFCM");
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Read SAFCM station data to the data structuredata structure
     * This is the part that should be converted to low level function
     * calls obtaining the individual elements of the data structure, one
     * at the time.
     */
    status = H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, obs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not read data structure from file");
        return(status);
    };
    /*
       sprintf((*obs).source,"%s",myobs.source);
       (*obs).vtime = myobs.vtime;
       (*obs).nopix = myobs.nopix;
       for (j=0;j<myobs.nopix;j++) {
       (*obs).data[j] = myobs.data[j];
       }
       */


    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}
#endif /* FMCOL_HAVE_LIBSMHI_SAF */

/*
 * PURPOSE:
 * To read SAFSSI station data through a compound data type for 
 * SAFSSI data structures from a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
#ifdef FMCOL_HAVE_LIBOSIHDF5
herr_t readSAFSSI(hid_t file, hid_t grp, safssi_data *obs){

    char *where="readSAFSSI";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(dataspace);
    };

    /* 
     * Get compound datatype from file
     */
    if ((datatype = H5Topen(file,"safssi_dt")) < 0) {
        fmerrmsg(where,
                "Could not get datatype");
        return(datatype);
    } 

    /*
     * Get dataset for the SAFSSI data structure.
     */
    dataset = H5Dopen(grp, "SAFSSI");
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Read SAFSSI station data to the data structuredata structure
     */
    status = H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, obs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not read data structure from file");
        return(status);
    };

    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}

/*
 * PURPOSE:
 * To read SAFDLI station data through a compound data type for 
 * SAFDLI data structures from a HDF5 file already opened and containing a
 * defined group.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
herr_t readSAFDLI(hid_t file, hid_t grp, safdli_data *obs){

    char *where="readSAFDLI";
    hid_t dataspace, datatype, dataset;
    hsize_t dimh[]={1};
    herr_t status;

    /*
     * Create dataspace
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
        fmerrmsg(where,
                "Could not create dataspace");
        return(dataspace);
    };

    /* 
     * Get compound datatype from file
     */
    if ((datatype = H5Topen(file,"safdli_dt")) < 0) {
        fmerrmsg(where,
                "Could not get datatype");
        return(datatype);
    } 

    /*
     * Get dataset for the SAFDLI data structure.
     */
    dataset = H5Dopen(grp, "SAFDLI");
    if (dataset < 0) {
        fmerrmsg(where,
                "Could not create dataset");
        return(status);
    };

    /*
     * Read SAFDLI station data to the data structuredata structure
     */
    status = H5Dread(dataset, datatype, H5S_ALL, H5S_ALL, 
            H5P_DEFAULT, obs);
    if (status < 0) {
        fmerrmsg(where,
                "Could not read data structure from file");
        return(status);
    };

    /*
     * Close/release resources
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close field dataspace in HDF5 file");
        return(status);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close datatype");
        return(status);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close dataset");
        return(status);
    };

    return(status);
}
#endif /* FMCOL_HAVE_LIBOSIHDF5 */

/*
 * PURPOSE:
 * To create a compound data type for AVHRR data structures.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
hid_t compoundAVHRR(int nopix) {

    char *where="compoundAVHRR";
    herr_t status;
    hid_t avhrr_id, ucs_id, ang_id, array_id, str20;
    hsize_t arraydim[1];

    /*
     * Create string datatype
     */
    str20 = H5Tcopy(H5T_C_S1);
    if (str20 < 0) {
        fmerrmsg(where,
                "Could not create str20");
        return(-4);
    }
    status = H5Tset_size(str20,20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not set size of str20");
        return(-4);
    }
    status = H5Tset_strpad(str20,H5T_STR_NULLTERM);
    if (status < 0) {
        fmerrmsg(where,
                "Could not pad str20");
        return(-4);
    }

    /*
     * Create compound datatype for AVHRR
     */
    avhrr_id = H5Tcreate(H5T_COMPOUND, sizeof(as_data));
    if (avhrr_id < 0) {
        fprintf(stderr,"%s%s\n",where, 
                "Could not define compound datatype for AVHRR data");
        return(-2);
    };
    status = H5Tinsert(avhrr_id, "source", HOFFSET(as_data, source), str20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert source element in AVHRR compound datatype");
        return(-2);
    }
    status = H5Tinsert(avhrr_id, "nochan", HOFFSET(as_data, nochan), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nochan element in AVHRR compound datatype");
        return(-2);
    }
    status = H5Tinsert(avhrr_id, "nopix", HOFFSET(as_data, nopix), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nopix element in AVHRR compound datatype");
        return(-2);
    }
    status = H5Tinsert(avhrr_id, "vtime", HOFFSET(as_data, vtime), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert vtime element in AVHRR compound datatype");
        return(-2);
    }

    /* 
     * Create compound datatype for UCS and insert it in the AVHRR
     * datatype
     */
    ucs_id = compoundUCS();
    if (ucs_id < 0) {
        fmerrmsg(where, 
                "Could not define compound datatype for ucs data");
        return(-2);
    };
    status = H5Tinsert(avhrr_id, "nav", HOFFSET(as_data, nav), 
            ucs_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nav element in AVHRR compound datatype");
        return(-2);
    }
    status = H5Tclose(ucs_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close ucs_id in avhrr_id in HDF5 file");
        return(-2);
    };

    /* 
     * Create compound datatype for IMGA and insert it in the AVHRR
     * datatype.
     */
    ang_id = compoundIMGA();
    if (ang_id < 0) {
        fmerrmsg(where, 
                "Could not define compound datatype for imga data");
        return(-2);
    };
    status = H5Tinsert(avhrr_id, "ang", HOFFSET(as_data, ang), 
            ang_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert imga element in AVHRR compound datatype");
        return(-2);
    }
    status = H5Tclose(ang_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close ang_id in avhrr_id in HDF5 file");
        return(-2);
    };

    /*
     * AVHRR channels are created below
     */
    arraydim[0] = nopix;
    array_id = H5Tarray_create(H5T_NATIVE_FLOAT,1,arraydim,NULL); 
    if (status < 0) {
        fmerrmsg(where,
                "Could not create array datatype");
        return(-2);
    }
    status = H5Tinsert(avhrr_id, "ch1", HOFFSET(as_data, ch1), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert ch1 array element in AVHRR compound datatype");
        return(-2);
    }
    status = H5Tinsert(avhrr_id, "ch2", HOFFSET(as_data, ch2), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert ch2 array element in AVHRR compound datatype");
        return(-2);
    }
    status = H5Tinsert(avhrr_id, "ch3a", HOFFSET(as_data, ch3a), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert ch3a array element in AVHRR compound datatype");
        return(-2);
    }
    status = H5Tinsert(avhrr_id, "ch3b", HOFFSET(as_data, ch3b), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert ch3b array element in AVHRR compound datatype");
        return(-2);
    }
    status = H5Tinsert(avhrr_id, "ch4", HOFFSET(as_data, ch4), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert ch4 array element in AVHRR compound datatype");
        return(-2);
    }
    status = H5Tinsert(avhrr_id, "ch5", HOFFSET(as_data, ch5), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert ch5 array element in AVHRR compound datatype");
        return(-2);
    }
    status = H5Tclose(array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close array_id");
        return(-2);
    };

    /*
     * Close resources used
     */
    status = H5Tclose(str20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close str20");
        return(-2);
    };

    return(avhrr_id);
}

/*
 * PURPOSE:
 * To create a compound data type for NWP data structures.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
hid_t compoundNWP(int nopix) {

    char *where="compoundNWP";
    herr_t status;
    hid_t nwp_id, ucs_id, array_id;
    hsize_t arraydim[1];

    nwp_id = H5Tcreate(H5T_COMPOUND, sizeof(ns_data));
    if (nwp_id < 0) {
        fprintf(stderr,"%s%s\n",where, 
                "Could not define compound datatype for NWP data");
        return(-2);
    };
    status = H5Tinsert(nwp_id, "nopar", HOFFSET(ns_data, nopar), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nopar element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "nopix", HOFFSET(ns_data, nopix), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nopix element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "vtime", HOFFSET(ns_data, vtime), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert vtime element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "ltime", HOFFSET(ns_data, ltime), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert ltime element in NWP compound datatype");
        return(-2);
    }
    /* 
     * Create UCS compound datatype and insert in NWP compound datatype.
     */
    ucs_id = compoundUCS();
    if (ucs_id < 0) {
        fmerrmsg(where, 
                "Could not define compound datatype for ucs data");
        return(-2);
    };
    status = H5Tinsert(nwp_id, "nav", HOFFSET(ns_data, nav), 
            ucs_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nav element in NWP compound datatype");
        return(-2);
    }
    status = H5Tclose(ucs_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close ucs_id in nwp_id in HDF5 file");
        return(-2);
    };
    /* 
     * Create array datatype and insert it in the NWP compound datatype.
     */
    arraydim[0] = nopix;
    array_id = H5Tarray_create(H5T_NATIVE_FLOAT,1,arraydim,NULL); 
    if (status < 0) {
        fmerrmsg(where,
                "Could not create array datatype");
        return(-2);
    }

    status = H5Tinsert(nwp_id, "t950hpa", HOFFSET(ns_data, t950hpa), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert t950hpa element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "t800hpa", HOFFSET(ns_data, t800hpa), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert t800hpa element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "t700hpa", HOFFSET(ns_data, t700hpa), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert t700hpa element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "t500hpa", HOFFSET(ns_data, t500hpa), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert t500hpa element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "t0m", HOFFSET(ns_data, t0m), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert t0m element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "t2m", HOFFSET(ns_data, t2m), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert t2m element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "ps", HOFFSET(ns_data, ps), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert ps element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "pw", HOFFSET(ns_data, pw), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert pw element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "rh", HOFFSET(ns_data, rh), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert rh element in NWP compound datatype");
        return(-2);
    }
    status = H5Tinsert(nwp_id, "topo", HOFFSET(ns_data, topo), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert topo element in NWP compound datatype");
        return(-2);
    }

    status = H5Tclose(array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close array_id");
        return(-2);
    };

    return(nwp_id);
}

/*
 * PURPOSE:
 * To create a compound data type for STDAT data structures.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
hid_t compoundSTDAT(void) {

    char *where="compoundSTDAT";
    herr_t status;
    hid_t synop_id, latlon_id, str5, str10;

    /*
     * Create string datatypes to be used
     */
    str5 = H5Tcopy(H5T_C_S1);
    if (str5 < 0) {
        fmerrmsg(where,
                "Could not create str5");
        return(-4);
    }
    status = H5Tset_size(str5,5);
    if (status < 0) {
        fmerrmsg(where,
                "Could not set size of str5");
        return(-4);
    }
    status = H5Tset_strpad(str5,H5T_STR_NULLTERM);
    if (status < 0) {
        fmerrmsg(where,
                "Could not pad str5");
        return(-4);
    }
    str10 = H5Tcopy(H5T_C_S1);
    if (str10 < 0) {
        fmerrmsg(where,
                "Could not create str10");
        return(-4);
    }
    status = H5Tset_size(str10,10);
    if (status < 0) {
        fmerrmsg(where,
                "Could not set size of str10");
        return(-4);
    }
    status = H5Tset_strpad(str10,H5T_STR_NULLTERM);
    if (status < 0) {
        fmerrmsg(where,
                "Could not pad str10");
        return(-4);
    }

    /*
     * Create compound datatype for STDAT
     */
    synop_id = H5Tcreate(H5T_COMPOUND, sizeof(obsstruct));
    if (synop_id < 0) {
        fprintf(stderr,"%s%s\n",where, 
                "Could not define compound datatype for STDAT data");
        return(-2);
    };
    status = H5Tinsert(synop_id, "stID", HOFFSET(obsstruct, stID), str10);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert stID element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "stType", HOFFSET(obsstruct, stType), str5);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert stType element in STDAT compound datatype");
        return(-2);
    }
    /* 
     * Create compund synop_id for latlon data structure
     */
    latlon_id = compoundLATLON();
    if (latlon_id < 0) {
        fmerrmsg(where, 
                "Could not define compound datatype for latlon data");
        return(-2);
    };
    status = H5Tinsert(synop_id, "pos", HOFFSET(obsstruct, pos), 
            latlon_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert pos element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tclose(latlon_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close latlon_id in synop_id in HDF5 file");
        return(-2);
    };
    /* 
     * Continue after latlon 
     */
    status = H5Tinsert(synop_id, "year", HOFFSET(obsstruct, year), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert year element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "month", HOFFSET(obsstruct, month), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert month element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "day", HOFFSET(obsstruct, day), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert day element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "hour", HOFFSET(obsstruct, hour), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert hour element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "min", HOFFSET(obsstruct, min), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert min element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "TTT", HOFFSET(obsstruct, TTT), 
            H5T_NATIVE_FLOAT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert TTT element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "TdTdTd", HOFFSET(obsstruct, TdTdTd), 
            H5T_NATIVE_FLOAT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert TdTdTd element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "TwTwTw", HOFFSET(obsstruct, TwTwTw), 
            H5T_NATIVE_FLOAT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert TwTwTw element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "PPPP", HOFFSET(obsstruct, PPPP), 
            H5T_NATIVE_FLOAT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert PPPP element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "ff", HOFFSET(obsstruct, ff), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert ff element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "dd", HOFFSET(obsstruct, dd), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert dd element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "RRR", HOFFSET(obsstruct, RRR), 
            H5T_NATIVE_FLOAT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert RRR element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "E", HOFFSET(obsstruct, E), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert E element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "sss", HOFFSET(obsstruct, sss), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert sss element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "N", HOFFSET(obsstruct, N), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert N element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "Nh", HOFFSET(obsstruct, Nh), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert Nh element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "Cl", HOFFSET(obsstruct, Cl), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert Cl element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "Cm", HOFFSET(obsstruct, Cm), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert Cm element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "Ch", HOFFSET(obsstruct, Ch), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert Ch element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "VV", HOFFSET(obsstruct, VV), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert VV element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "ww", HOFFSET(obsstruct, ww), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert ww element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "W1", HOFFSET(obsstruct, W1), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert W1 element in STDAT compound datatype");
        return(-2);
    }
    status = H5Tinsert(synop_id, "W2", HOFFSET(obsstruct, W2), 
            H5T_NATIVE_SHORT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert W2 element in STDAT compound datatype");
        return(-2);
    }

    /*
     * Close string datatypes
     */
    status = H5Tclose(str5);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close str5");
        return(-2);
    };

    status = H5Tclose(str10);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close str10");
        return(-2);
    };

    return(synop_id);
}

/*
 * PURPOSE:
 * To create a compound data type for SAFCM data structures.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
#ifdef FMCOL_HAVE_LIBSMHI_SAF
hid_t compoundSAFCM(int nopix){

    char *where="compoundSAFCM";
    herr_t status;
    hid_t safcm_id, ucs_id, array_id, array2_id, str20, strMLS, vstr;
    hsize_t arraydim[1],arraydim2[1];

    /*
     * Create string datatype
     */
    str20 = H5Tcopy(H5T_C_S1);
    if (str20 < 0) {
        fmerrmsg(where,
                "Could not create str20");
        return(-4);
    }
    status = H5Tset_size(str20,20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not set size of str20");
        return(-4);
    }
    status = H5Tset_strpad(str20,H5T_STR_NULLTERM);
    if (status < 0) {
        fmerrmsg(where,
                "Could not pad str20");
        return(-4);
    }
    strMLS = H5Tcopy(H5T_C_S1);
    if (strMLS < 0) {
        fmerrmsg(where,
                "Could not create strMLS");
        return(-4);
    }
    status = H5Tset_size(strMLS,FMSTRING256);
    if (status < 0) {
        fmerrmsg(where,
                "Could not set size of strMLS");
        return(-4);
    }
    status = H5Tset_strpad(strMLS,H5T_STR_NULLTERM);
    if (status < 0) {
        fmerrmsg(where,
                "Could not pad strMLS");
        return(-4);
    }

    /*
     * Create compound datatype for SAFCM
     */
    safcm_id = H5Tcreate(H5T_COMPOUND, sizeof(safcm_data));
    if (safcm_id < 0) {
        fprintf(stderr,"%s%s\n",where, 
                "Could not define compound datatype for SAFCM data");
        return(-2);
    };
    status = H5Tinsert(safcm_id, "source", HOFFSET(safcm_data, source), str20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert source element in SAFCM compound datatype");
        return(-2);
    }
    status = H5Tinsert(safcm_id, "nopix", HOFFSET(safcm_data, nopix), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nopix element in SAFCM compound datatype");
        return(-2);
    }
    status = H5Tinsert(safcm_id, "vtime", HOFFSET(safcm_data, vtime), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert vtime element in SAFCM compound datatype");
        return(-2);
    }
    /* 
     * Create compound datatype for UCS and insert it in the SAFCM
     * datatype
     */
    ucs_id = compoundUCS();
    if (ucs_id < 0) {
        fmerrmsg(where, 
                "Could not define compound datatype for ucs data");
        return(-2);
    };
    status = H5Tinsert(safcm_id, "nav", HOFFSET(safcm_data, nav), 
            ucs_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nav element in SAFCM compound datatype");
        return(-2);
    }
    status = H5Tclose(ucs_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close ucs_id in safcm_id");
        return(-2);
    };
    /*
     * Continue with standard elements
     */
    arraydim[0] = nopix;
    array_id = H5Tarray_create(H5T_NATIVE_UCHAR,1,arraydim,NULL); 
    if (status < 0) {
        fmerrmsg(where,
                "Could not create array element in SAFCM compound datatype");
        return(-2);
    }
    status = H5Tinsert(safcm_id, "data", HOFFSET(safcm_data, data), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert array element in SAFCM compound datatype");
        return(-2);
    }
    status = H5Tclose(array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close array_id");
        return(-2);
    };
    arraydim2[0] = FMCOL_NO_CLOUDTYPE_VALUES;
    array2_id = H5Tarray_create(strMLS,1,arraydim2,NULL); 
    if (status < 0) {
        fmerrmsg(where,
                "Could not create string array element");
        return(-2);
    }
    status = H5Tinsert(safcm_id, "description", 
            HOFFSET(safcm_data, description), 
            array2_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert string array");
        return(-2);
    }
    status = H5Tclose(array2_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close array2_id");
        return(-2);
    };

    /*
     * Close string datatype
     */
    status = H5Tclose(str20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close str20");
        return(-2);
    };
    status = H5Tclose(strMLS);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close strMLS");
        return(-2);
    };

    return(safcm_id);
}

/*
 * PURPOSE:
 * To create a compound data type for SAFCM data structures.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
hid_t compoundSAFCMRAW(int nopix){

    char *where="compoundSAFCMSMALL";
    herr_t status;
    hid_t safcm_id, ucs_id, array_id, array2_id, str20, strMLS, vstr;
    hsize_t arraydim[1],arraydim2[1];

    /*
     * Create string datatype
     */
    str20 = H5Tcopy(H5T_C_S1);
    if (str20 < 0) {
        fmerrmsg(where,
                "Could not create str20");
        return(-4);
    }
    status = H5Tset_size(str20,20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not set size of str20");
        return(-4);
    }
    status = H5Tset_strpad(str20,H5T_STR_NULLTERM);
    if (status < 0) {
        fmerrmsg(where,
                "Could not pad str20");
        return(-4);
    }

    /*
     * Create compound datatype for SAFCM
     */
    safcm_id = H5Tcreate(H5T_COMPOUND, sizeof(safcm_data2));
    if (safcm_id < 0) {
        fprintf(stderr,"%s%s\n",where, 
                "Could not define compound datatype for SAFCM data");
        return(-2);
    };
    status = H5Tinsert(safcm_id, "source", HOFFSET(safcm_data2, source), str20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert source element in SAFCM compound datatype");
        return(-2);
    }
    status = H5Tinsert(safcm_id, "nopix", HOFFSET(safcm_data2, nopix), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nopix element in SAFCM compound datatype");
        return(-2);
    }
    status = H5Tinsert(safcm_id, "vtime", HOFFSET(safcm_data2, vtime), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert vtime element in SAFCM compound datatype");
        return(-2);
    }
    /*
     * Continue with standard elements
     */
    arraydim[0] = nopix;
    array_id = H5Tarray_create(H5T_NATIVE_UCHAR,1,arraydim,NULL); 
    if (status < 0) {
        fmerrmsg(where,
                "Could not create array element in SAFCM compound datatype");
        return(-2);
    }
    status = H5Tinsert(safcm_id, "data", HOFFSET(safcm_data2, data), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert array element in SAFCM compound datatype");
        return(-2);
    }
    status = H5Tclose(array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close array_id");
        return(-2);
    };

    /*
     * Close string datatype
     */
    status = H5Tclose(str20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close str20");
        return(-2);
    };

    return(safcm_id);
}
#endif /* FMCOL_HAVE_LIBSMHI_SAF */

/*
 * PURPOSE:
 * To create a compound data type for SAFSSI data structures.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
#ifdef FMCOL_HAVE_LIBOSIHDF5
hid_t compoundSAFSSI(int nopix){

    char *where="compoundSAFSSI";
    herr_t status;
    hid_t safssi_id, ucs_id, array_id, str20;
    hsize_t arraydim[1];

    /*
     * Create string datatype
     */
    str20 = H5Tcopy(H5T_C_S1);
    if (str20 < 0) {
        fmerrmsg(where,
                "Could not create str20");
        return(-4);
    }
    status = H5Tset_size(str20,20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not set size of str20");
        return(-4);
    }
    status = H5Tset_strpad(str20,H5T_STR_NULLTERM);
    if (status < 0) {
        fmerrmsg(where,
                "Could not pad str20");
        return(-4);
    }

    /*
     * Create compound datatype for SAFSSI
     */
    safssi_id = H5Tcreate(H5T_COMPOUND, sizeof(safssi_data));
    if (safssi_id < 0) {
        fprintf(stderr,"%s%s\n",where, 
                "Could not define compound datatype for SAFSSI data");
        return(-2);
    };
    status = H5Tinsert(safssi_id, "source", HOFFSET(safssi_data, source), str20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert source element in SAFSSI compound datatype");
        return(-2);
    }
    status = H5Tinsert(safssi_id, "nopix", HOFFSET(safssi_data, nopix), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nopix element in SAFSSI compound datatype");
        return(-2);
    }
    status = H5Tinsert(safssi_id, "vtime", HOFFSET(safssi_data, vtime), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert vtime element in SAFSSI compound datatype");
        return(-2);
    }
    /* 
     * Create compound datatype for UCS and insert it in the SAFSSI
     * datatype
     */
    ucs_id = compoundUCS();
    if (ucs_id < 0) {
        fmerrmsg(where, 
                "Could not define compound datatype for ucs data");
        return(-2);
    };
    status = H5Tinsert(safssi_id, "nav", HOFFSET(safssi_data, nav), 
            ucs_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nav element in SAFSSI compound datatype");
        return(-2);
    }
    status = H5Tclose(ucs_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close ucs_id in safssi_id");
        return(-2);
    };
    /*
     * Continue with standard elements
     */
    arraydim[0] = nopix;
    array_id = H5Tarray_create(H5T_NATIVE_FLOAT,1,arraydim,NULL); 
    if (status < 0) {
        fmerrmsg(where,
                "Could not create data element in SAFSSI compound datatype");
        return(-2);
    }
    status = H5Tinsert(safssi_id, "data", HOFFSET(safssi_data, data), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert data element in SAFSSI compound datatype");
        return(-2);
    }
    status = H5Tclose(array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close array_id for data");
        return(-2);
    };
    array_id = H5Tarray_create(H5T_NATIVE_UCHAR,1,arraydim,NULL); 
    if (status < 0) {
        fmerrmsg(where,
                "Could not create qflg element in SAFSSI compound datatype");
        return(-2);
    }
    status = H5Tinsert(safssi_id, "qflg", HOFFSET(safssi_data, qflg), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert qflg element in SAFSSI compound datatype");
        return(-2);
    }
    status = H5Tclose(array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close array_id for qflg");
        return(-2);
    };

    /*
     * Close string datatype
     */
    status = H5Tclose(str20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close str20");
        return(-2);
    };

    return(safssi_id);
}

/*
 * PURPOSE:
 * To create a compound data type for SAFDLI data structures.
 *
 * RETURN:
 * Negative values on failure, else success.
 */
hid_t compoundSAFDLI(int nopix){

    char *where="compoundSAFDLI";
    herr_t status;
    hid_t safdli_id, ucs_id, array_id, str20;
    hsize_t arraydim[1];

    /*
     * Create string datatype
     */
    str20 = H5Tcopy(H5T_C_S1);
    if (str20 < 0) {
        fmerrmsg(where,
                "Could not create str20");
        return(-4);
    }
    status = H5Tset_size(str20,20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not set size of str20");
        return(-4);
    }
    status = H5Tset_strpad(str20,H5T_STR_NULLTERM);
    if (status < 0) {
        fmerrmsg(where,
                "Could not pad str20");
        return(-4);
    }

    /*
     * Create compound datatype for SAFDLI
     */
    safdli_id = H5Tcreate(H5T_COMPOUND, sizeof(safdli_data));
    if (safdli_id < 0) {
        fprintf(stderr,"%s%s\n",where, 
                "Could not define compound datatype for SAFDLI data");
        return(-2);
    };
    status = H5Tinsert(safdli_id, "source", HOFFSET(safdli_data, source), str20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert source element in SAFDLI compound datatype");
        return(-2);
    }
    status = H5Tinsert(safdli_id, "nopix", HOFFSET(safdli_data, nopix), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nopix element in SAFDLI compound datatype");
        return(-2);
    }
    status = H5Tinsert(safdli_id, "vtime", HOFFSET(safdli_data, vtime), 
            H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert vtime element in SAFDLI compound datatype");
        return(-2);
    }
    /* 
     * Create compound datatype for UCS and insert it in the SAFDLI
     * datatype
     */
    ucs_id = compoundUCS();
    if (ucs_id < 0) {
        fmerrmsg(where, 
                "Could not define compound datatype for ucs data");
        return(-2);
    };
    status = H5Tinsert(safdli_id, "nav", HOFFSET(safdli_data, nav), 
            ucs_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert nav element in SAFDLI compound datatype");
        return(-2);
    }
    status = H5Tclose(ucs_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close ucs_id in safdli_id");
        return(-2);
    };
    /*
     * Continue with standard elements
     */
    arraydim[0] = nopix;
    array_id = H5Tarray_create(H5T_NATIVE_FLOAT,1,arraydim,NULL); 
    if (status < 0) {
        fmerrmsg(where,
                "Could not create data element in SAFDLI compound datatype");
        return(-2);
    }
    status = H5Tinsert(safdli_id, "data", HOFFSET(safdli_data, data), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert data element in SAFDLI compound datatype");
        return(-2);
    }
    status = H5Tclose(array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close array_id for data");
        return(-2);
    };
    array_id = H5Tarray_create(H5T_NATIVE_UCHAR,1,arraydim,NULL); 
    if (status < 0) {
        fmerrmsg(where,
                "Could not create qflg element in SAFDLI compound datatype");
        return(-2);
    }
    status = H5Tinsert(safdli_id, "qflg", HOFFSET(safdli_data, qflg), 
            array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert qflg element in SAFDLI compound datatype");
        return(-2);
    }
    status = H5Tclose(array_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close array_id for qflg");
        return(-2);
    };

    /*
     * Close string datatype
     */
    status = H5Tclose(str20);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close str20");
        return(-2);
    };

    return(safdli_id);
}
#endif /* FMCOL_HAVE_LIBOSIHDF5 */

/*
 * PURPOSE:
 * Create a compound data structure for ucs structures found in
 * libsatimg.
 *
 * RETURN:
 * negative values on failure, else useful values.
 */
hid_t compoundUCS(void) {

    char *where="compoundUCS";
    herr_t status;
    hid_t ucs_id;

    ucs_id = H5Tcreate(H5T_COMPOUND, sizeof(fmucsref));
    if (ucs_id < 0) {
        fmerrmsg(where, 
                "Could not define compound datatype for UCS data");
        return(-2);
    };
    status = H5Tinsert(ucs_id, "iw", HOFFSET(fmucsref, iw), 
            H5T_NATIVE_UINT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert iw element in UCS compound datatype");
        return(-2);
    }
    status = H5Tinsert(ucs_id, "ih", HOFFSET(fmucsref, ih), 
            H5T_NATIVE_UINT);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert ih element in UCS compound datatype");
        return(-2);
    }
    status = H5Tinsert(ucs_id, "Ax", HOFFSET(fmucsref, Ax), 
            H5T_NATIVE_DOUBLE);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert Ax element in UCS compound datatype");
        return(-2);
    }
    status = H5Tinsert(ucs_id, "Ay", HOFFSET(fmucsref, Ay), 
            H5T_NATIVE_DOUBLE);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert Ay element in UCS compound datatype");
        return(-2);
    }
    status = H5Tinsert(ucs_id, "Bx", HOFFSET(fmucsref, Bx), 
            H5T_NATIVE_DOUBLE);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert Ay element in UCS compound datatype");
        return(-2);
    }
    status = H5Tinsert(ucs_id, "By", HOFFSET(fmucsref, By), 
            H5T_NATIVE_DOUBLE);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert By element in UCS compound datatype");
        return(-2);
    }

    return(ucs_id);
}

/*
 * PURPOSE:
 * Create a compound data structure for imga structures found in
 * libsatimg.
 *
 * RETURN:
 * negative values on failure, else useful values.
 */
hid_t compoundIMGA(void) {

    char *where="compoundIMGA";
    herr_t status;
    hid_t imga_id;

    imga_id = H5Tcreate(H5T_COMPOUND, sizeof(fmangles));
    if (imga_id < 0) {
        fmerrmsg(where,
                "Could not define compound datatype for imga data");
        return(-2);
    };
    status = H5Tinsert(imga_id, "soz", HOFFSET(fmangles, soz), 
            H5T_NATIVE_DOUBLE);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert soz element in imga compound datatype");
        return(-2);
    }
    status = H5Tinsert(imga_id, "saz", HOFFSET(fmangles, saz), 
            H5T_NATIVE_DOUBLE);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert saz element in imga compound datatype");
        return(-2);
    }
    status = H5Tinsert(imga_id, "raz", HOFFSET(fmangles, raz), 
            H5T_NATIVE_DOUBLE);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert raz element in imga compound datatype");
        return(-2);
    }
    status = H5Tinsert(imga_id, "aza", HOFFSET(fmangles, aza), 
            H5T_NATIVE_DOUBLE);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert aza element in imga compound datatype");
        return(-2);
    }
    status = H5Tinsert(imga_id, "azu", HOFFSET(fmangles, azu), 
            H5T_NATIVE_DOUBLE);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert azu element in imga compound datatype");
        return(-2);
    }

    return(imga_id);
}

/*
 * PURPOSE:
 * Create a compound data structure for latlon structures found in
 * libsatimg.
 *
 * RETURN:
 * negative values on failure, else useful values.
 */
hid_t compoundLATLON(void) {

    char *where="compoundLATLON";
    herr_t status;
    hid_t latlon_id;

    latlon_id = H5Tcreate(H5T_COMPOUND, sizeof(fmgeopos));
    if (latlon_id < 0) {
        fmerrmsg(where, 
                "Could not define compound datatype for latlon data");
        return(-2);
    };
    status = H5Tinsert(latlon_id, "lat", HOFFSET(fmgeopos, lat), 
            H5T_NATIVE_DOUBLE);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert lat element in latlon compound datatype");
        return(-2);
    }
    status = H5Tinsert(latlon_id, "lon", HOFFSET(fmgeopos, lon), 
            H5T_NATIVE_DOUBLE);
    if (status < 0) {
        fmerrmsg(where,
                "Could not insert lon element in latlon compound datatype");
        return(-2);
    }

    return(latlon_id);
}

/*
 * PURPOSE:
 * To open an HDF5 file for use, if not existing, create a new.
 * The use of set_istore and set_meta_block is recommended by Daniel
 * Michelson, testing showed that it greatly reduced the size of the files
 * alongside using datatype committing...
 *
 * RETURN:
 * File identifier, negative on failure.
 */
hid_t openFile(char *filename) {

    char *where="openFile";
    hid_t file, fcpl=-1, fapl=-1;
    FILE *fp;

    /*
     * Check if file exists, if not create new HDF file.
     */
    fp = fopen(filename,"r");
    if (fp == NULL) {
        if ((fcpl=H5Pcreate(H5P_FILE_CREATE)) == -1) {
            fmerrmsg(where,
                    "Could not create fcpl");
            return(-2);
        }
        if ((fapl=H5Pcreate(H5P_FILE_ACCESS)) == -1) {
            fmerrmsg(where,
                    "Could not create fapl");
            return(-2);
        }
        if (H5Pset_istore_k(fcpl,(int)1) < 0) {
            fmerrmsg(where,
                    "Could not set istore on fcpl");
            return(-2);
        }
        if (H5Pset_meta_block_size(fapl,(hsize_t)0) < 0) {
            fmerrmsg(where,
                    "Could not set meta_block on fapl");
            return(-2);
        }
        file = H5Fcreate(filename, H5F_ACC_TRUNC, fcpl, fapl);
        if (H5Pclose(fcpl) < 0) {
            fmerrmsg(where,
                    "Could close fcpl");
            return(-2);
        }
        if (H5Pclose(fapl) < 0) {
            fmerrmsg(where,
                    "Could close fapl");
            return(-2);
        }
        if (file < 0) {
            fmerrmsg(where, 
                    "Could not create new HDF5 file");
            fprintf(stderr," %s\n", filename);
            return(file);
        };
    } else {
        fclose(fp);

        if (H5Fis_hdf5(filename) == 0) {
            fmerrmsg(where,
                    "File exists, but not in HDF5 format");
            return(-2);
        }
        if (H5Fis_hdf5(filename)>0) {
            if ((fapl=H5Pcreate(H5P_FILE_ACCESS)) == -1) {
                fmerrmsg(where,
                        "Could not create fapl");
                return(-2);
            }
            if (H5Pset_meta_block_size(fapl,(hsize_t)0) < 0) {
                fmerrmsg(where,
                        "Could not set meta_block on fapl");
                return(-2);
            }
            file = H5Fopen(filename, H5F_ACC_RDWR, fapl);
            if (H5Pclose(fapl) < 0) {
                fmerrmsg(where,
                        "Could close fapl");
                return(-2);
            }
            if (file < 0) {
                fmerrmsg(where, 
                        "Could not open existing HDF5 file");
                fprintf(stderr," %s\n", filename);
                return(file);
            };
        }
    }

    return(file);
}

/*
 * PURPOSE:
 * Operator function to be used within H5Giterate to determine the number
 * of base groups in the file. By definition a group is defined for each
 * station stored, the name of the group defines which class, stationid
 * and timeid the datat stored belong to.
 *
 * RETURN:
 * 0  on success, negative on failure.
 */
herr_t count_basegroups(hid_t loc_id, const char *name, void *opdata) {

    char *where="count_basegroups";
    char tmpstr[FMSTRING1024], *cpt;
    char tmpstrstation[FMSTRING64];
    char classname[FMSTRING256], station[FMSTRING64], timeid[FMSTRING32];
    char satid[FMSTRING32], areadef[FMSTRING32];
    H5G_stat_t statbuf;
    size_t t_tmp;
    struct tm tm_tmp;
    int l, uffda=0;
    static int i=0;
    static int k=1;

    H5Gget_objinfo(loc_id, name, FMFALSE, &statbuf);
    switch (statbuf.type) {
        case H5G_GROUP:
            if ((* (cinfo *) opdata).scrit == NULL) {
                sprintf(((* (cinfo *) opdata).group[i]),"%s",name);
                /*
                   printf("count_basegroups: %s\n",name);
                   */
                i++;
            } else {
                /*
                   printf("%s\n", name);
                   printf("%s\n",(*(* (cinfo *) opdata).scrit).station);
                   printf("%s\n",(*(* (cinfo *) opdata).scrit).classname);
                   printf("%ld\n",(*(* (cinfo *) opdata).scrit).t_start);
                   printf("%ld\n",(*(* (cinfo *) opdata).scrit).t_end);
                   */

                /*
                 * Decode group string
                 */

                /*
                 * Collect classname e.g. cloud, water, ...
                 */
                sprintf(tmpstr,"%s",name);
                sprintf(classname,"%s",strtok(tmpstr," "));
                /*
                 * Collect station name, either SYNOP id or possibly a
                 * geographical position (latitude, longitude), positive
                 * northwards and eastwards. I.e. this could either be 1
                 * or 2 words.
                 */
                sprintf(station,"%s",strtok(NULL," "));
                if (strchr(station,'.')) {
                    strcat(station," ");
                    strcat(station,strtok(NULL," "));
                    uffda = 1;
                }
                if (uffda) {
                    /*
                     * Collect satellite id
                     */
                    sprintf(satid,"%s",strtok(NULL," "));
                    /*
                     * Collect area definition
                     */
                    sprintf(areadef,"%s",strtok(NULL," "));
                    /*
                     * Collect date and time information.
                     */
                    sprintf(timeid,"%s",strtok(NULL," "));
                    sprintf(tmpstr,"%c%c%c%c",
                            timeid[0],timeid[1],timeid[2],timeid[3]);
                    tm_tmp.tm_year = atoi(tmpstr) - 1900;
                    sprintf(tmpstr,"%c%c",
                            timeid[5],timeid[6]);
                    tm_tmp.tm_mon = atoi(tmpstr) - 1;
                    sprintf(tmpstr,"%c%c",
                            timeid[8],timeid[9]);
                    tm_tmp.tm_mday = atoi(tmpstr);
                    sprintf(timeid,"%s",strtok(NULL," "));
                    sprintf(tmpstr,"%c%c",
                            timeid[0],timeid[1]);
                    tm_tmp.tm_hour = atoi(tmpstr);
                    sprintf(tmpstr,"%c%c",
                            timeid[3],timeid[4]);
                    tm_tmp.tm_min = atoi(tmpstr);
                    sprintf(tmpstr,"%c%c",
                            timeid[6],timeid[7]);
                    tm_tmp.tm_sec = atoi(tmpstr);
                    tm_tmp.tm_isdst = 0;
                } else {
                    sprintf(timeid,"%s",strtok(NULL," "));
                    sprintf(tmpstr,"%c%c%c%c",
                            timeid[0],timeid[1],timeid[2],timeid[3]);
                    tm_tmp.tm_year = atoi(tmpstr) - 1900;
                    sprintf(tmpstr,"%c%c",
                            timeid[4],timeid[5]);
                    tm_tmp.tm_mon = atoi(tmpstr) - 1;
                    sprintf(tmpstr,"%c%c",
                            timeid[6],timeid[7]);
                    tm_tmp.tm_mday = atoi(tmpstr);
                    sprintf(tmpstr,"%c%c",
                            timeid[8],timeid[9]);
                    tm_tmp.tm_hour = atoi(tmpstr);
                    sprintf(tmpstr,"%c%c",
                            timeid[10],timeid[11]);
                    tm_tmp.tm_min = atoi(tmpstr);
                    tm_tmp.tm_sec = 0;
                    tm_tmp.tm_isdst = 0;
                }
                /*
                   printf("%4d-%02d-%02d %02d:%02d:%02d\n",
                   tm_tmp.tm_year,tm_tmp.tm_mon,tm_tmp.tm_mday,
                   tm_tmp.tm_hour,tm_tmp.tm_min,tm_tmp.tm_sec);
                   */
                t_tmp = mktime(&tm_tmp);
                if (strstr((*(* (cinfo *) opdata).scrit).station,"NA")) {
                    if (strstr(name,(*(* (cinfo *) opdata).scrit).classname)) {
                        if (t_tmp > ((*(* (cinfo *) opdata).scrit).t_start) && 
                                t_tmp < ((*(* (cinfo *) opdata).scrit).t_end)) {
                            sprintf(((* (cinfo *) opdata).group[i]),"%s",name);
                            i++;
                        }
                    }
                } else {
                    if (strstr(name,(*(* (cinfo *) opdata).scrit).classname)) {
                        if (t_tmp > ((*(* (cinfo *) opdata).scrit).t_start) && 
                                t_tmp < ((*(* (cinfo *) opdata).scrit).t_end) &&
                                strstr(station,
                                    (*(* (cinfo *) opdata).scrit).station)) {
                            sprintf(((* (cinfo *) opdata).group[i]),"%s",name);
                            i++;
                        }
                    }
                }
            }
            break;
        default:
            i = k = 0;
            return(FM_OK);
            break;
    }
    /*
       fmlogmsg(where,"Antall grupper funnet %d", i);
       */
    (* (cinfo *) opdata).noobs = i;
    if (i == (k*MAXGROUPS)) {
        k++;
        (* (cinfo *) opdata).group = (char (*)[NAMELEN]) 
            realloc((* (cinfo *) opdata).group,
                    k*MAXGROUPS*sizeof(*((* (cinfo *) opdata).group)));
        if (!((* (cinfo *) opdata).group)) {
            fmerrmsg(where,
                    "Malloc trouble groupnumbers");
            return(-2);
        }
        printf(" Another %d elements allocated for group control\n",MAXGROUPS);
        printf(" Total now is %d*%d\n",k,MAXGROUPS);
    }

    return(0);
}

/*
 * PURPOSE:
 * Operator function to be used within H5Giterate to determine the number
 * of datasets in a group. By definition a group is defined for each
 * station stored, the name of the group defines which class, stationid
 * and timeid the datat stored belong to.
 *
 * RETURN:
 * 0  on success, negative on failure.
 */
herr_t count_datasets(hid_t loc_id, const char *name, void *opdata) {

    H5G_stat_t statbuf;
    static hid_t old_loc_id = -1;
    static int i=0;

    if (loc_id != old_loc_id) {
        i = 0;
        old_loc_id = loc_id;
    }

    H5Gget_objinfo(loc_id, name, FMFALSE, &statbuf);
    switch (statbuf.type) {
        case H5G_DATASET:
            sprintf(((* (cinfo *) opdata).group[i]),"%s",name);
            i++;
            break;
        default:
            /*printf(" Could not recognize object\n");*/
            break;
    }
    (* (cinfo *) opdata).noobs = i;
    if (i == MAXDATASETS) return(-1);

    return(0);
}

/*
 * PURPOSE:
 * Operator function to be used within H5Giterate to dump the items.
 *
 * RETURN:
 * 0  on success, negative on failure.
 */
herr_t read_items(hid_t loc_id, const char *name, void *opdata) {

    H5G_stat_t statbuf;
    static int i=0;

    H5Gget_objinfo(loc_id, name, FMFALSE, &statbuf);
    switch (statbuf.type) {
        case H5G_GROUP:
            /*printf(" %6d Group object %s\n", i, name);*/
            i++;
            break;
        default:
            /*printf(" Could not recognize object\n");*/
            break;
    }
    * (int *) opdata = i;

    return(0);
}

#endif /* FMCOL_HAVE_LIBHDF5 */

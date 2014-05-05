/*
 * NAME:
 * stdats2hdf
 * 
 * PURPOSE:
 * To store collocated stationwise data in NCSA HDF5 structures/objects.
 *
 * RETURN VALUES:
 * 0 - Normal and correct ending.
 * 2 - input/output problem
 * 3 - memory allocation problem
 * 4 - datatype problem
 * 
 * NOTES:
 * Only adapted for Little endian UNIX time...
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein godøy, DNMI/FOU, 11/05/2000
 */

#include <stdats4hdf.h> 

int stdats2hdf(char *filename, char *class, 
	obsstruct sobs, as_data aobs, ns_data nobs) {
    
    hid_t file, dataset, datatype, dataspace;
    hid_t str5, str10, str20;
    hid_t ucs_id, imga_id, latlon_id, farr_id;
    hid_t grp;
    hsize_t dimh[] = {1}, dima[2];
    herr_t status;
    char *errmsg=" ERROR(stdats2hdf): ";

    /*
     * Control error handling.
     */
    /*
    H5Eset_auto(NULL,NULL);
    */

    /*
     * Check if file exists, if not create new HDF file.
     */
    if (H5Fis_hdf5(filename)>0) {
	if (H5Fis_hdf5(filename) == 0) {
	    fprintf(stderr," File exists, but not in HDF5 format...\n");
	    return(2);
	}
	file = H5Fopen(filename, H5F_ACC_RDWR, H5P_DEFAULT);
	if (file < 0) {
	    fprintf(stderr,"%s%s\n %s\n",errmsg, 
		"Could not open existing HDF5 file:", filename);
	    return(2);
	};
    } else {
	file = H5Fcreate(filename, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
	if (file < 0) {
	    fprintf(stderr,"%s%s\n %s\n",errmsg, 
		"Could not create new HDF5 file:", filename);
	    return(2);
	};
    }

    /*
     * Create string data types for use in compound datatype header.
     */
    str5 = H5Tcopy(H5T_C_S1);
    if (str5 < 0) return(4);
    status = H5Tset_size(str5,5);
    if (status < 0) return(4);
    status = H5Tset_strpad(str5,H5T_STR_NULLTERM);
    if (status < 0) return(4);
    str10 = H5Tcopy(H5T_C_S1);
    if (str10 < 0) return(4);
    status = H5Tset_size(str10,10);
    if (status < 0) return(4);
    status = H5Tset_strpad(str10,H5T_STR_NULLTERM);
    if (status < 0) return(4);
    str20 = H5Tcopy(H5T_C_S1);
    if (str20 < 0) return(4);
    status = H5Tset_size(str20,20);
    if (status < 0) return(4);
    status = H5Tset_strpad(str20,H5T_STR_NULLTERM);
    if (status < 0) return(4);

    /*
     * Create a group with data. This group contains all necessary data
     * for all sources (AVHRR, NWP, SYNOP, etc). 
     * Need to add attributes...
     */
    grp = H5Gcreate(file, class, 0);
    if (grp < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not create a data group in the HDF5 file");
	return(2);
    };

    /*
     * Create and write a compound data type containing AVHRR information.
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not create dataspace for AVHRR data in HDF5 file");
	return(2);
    };

    /*
     * Create compound datatype for AVHRR data (see avhrr_stdat.h).
     */
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(as_data));
    if (datatype < 0) {
	fprintf(stderr,"%s%s\n",errmsg, 
	    "Could not define compound datatype for AVHRR data");
	return(2);
    };
    status = H5Tinsert(datatype, "source", HOFFSET(as_data, source), str20);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert source element in AVHRR compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "nochan", HOFFSET(as_data, nochan), 
	    H5T_NATIVE_INT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert nochan element in AVHRR compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "nopix", HOFFSET(as_data, nopix), 
	    H5T_NATIVE_INT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert nopix element in AVHRR compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "vtime", HOFFSET(as_data, vtime), 
	    H5T_NATIVE_INT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert vtime element in AVHRR compound datatype");
	return(2);
    }
    /* Compound datatype for ucs */
    ucs_id = H5Tcreate(H5T_COMPOUND, sizeof(struct ucs));
    if (ucs_id < 0) {
	fprintf(stderr,"%s%s\n",errmsg, 
	    "Could not define compound datatype for UCS data");
	return(2);
    };
    status = H5Tinsert(ucs_id, "iw", HOFFSET(struct ucs, iw), 
	    H5T_NATIVE_UINT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert iw element in UCS compound datatype");
	return(2);
    }
    status = H5Tinsert(ucs_id, "ih", HOFFSET(struct ucs, ih), 
	    H5T_NATIVE_UINT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert ih element in UCS compound datatype");
	return(2);
    }
    status = H5Tinsert(ucs_id, "Ax", HOFFSET(struct ucs, Ax), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert Ax element in UCS compound datatype");
	return(2);
    }
    status = H5Tinsert(ucs_id, "Ay", HOFFSET(struct ucs, Ay), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert Ay element in UCS compound datatype");
	return(2);
    }
    status = H5Tinsert(ucs_id, "Bx", HOFFSET(struct ucs, Bx), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert Ay element in UCS compound datatype");
	return(2);
    }
    status = H5Tinsert(ucs_id, "By", HOFFSET(struct ucs, By), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert By element in UCS compound datatype");
	return(2);
    }
    /* Put UCS in AVHRR */
    status = H5Tinsert(datatype, "nav", HOFFSET(as_data, nav), 
	    ucs_id);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert nav element in AVHRR compound datatype");
	return(2);
    }
    /* Compound datatype for imga */
    imga_id = H5Tcreate(H5T_COMPOUND, sizeof(struct imga));
    if (imga_id < 0) {
	fprintf(stderr,"%s%s\n",errmsg, 
	    "Could not define compound datatype for imga data");
	return(2);
    };
    status = H5Tinsert(imga_id, "soz", HOFFSET(struct imga, soz), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert soz element in imga compound datatype");
	return(2);
    }
    status = H5Tinsert(imga_id, "saz", HOFFSET(struct imga, saz), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert saz element in imga compound datatype");
	return(2);
    }
    status = H5Tinsert(imga_id, "raz", HOFFSET(struct imga, raz), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert raz element in imga compound datatype");
	return(2);
    }
    status = H5Tinsert(imga_id, "aza", HOFFSET(struct imga, aza), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert aza element in imga compound datatype");
	return(2);
    }
    status = H5Tinsert(imga_id, "azu", HOFFSET(struct imga, azu), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert azu element in imga compound datatype");
	return(2);
    }
    /* Put imga in AVHRR */
    status = H5Tinsert(datatype, "ang", HOFFSET(as_data, ang), 
	    imga_id);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert imga element in AVHRR compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "ch3", HOFFSET(as_data, ch3), 
	    H5T_NATIVE_CHAR);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert ch3 element in AVHRR compound datatype");
	return(2);
    }
    /* Insert actual image data */
    dima[0] = MAXCH;
    dima[1] = aobs.nopix;
    farr_id = H5Tarray_create(H5T_NATIVE_FLOAT,2,dima,NULL); 
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert imga element in AVHRR compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "data", HOFFSET(as_data, data), 
	    farr_id);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert imga element in AVHRR compound datatype");
	return(2);
    }

    /*
     * Create dataset for AVHRR.
     */
    dataset = H5Dcreate(grp, "AVHRR", datatype, dataspace,
	H5P_DEFAULT);
    if (dataset < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not create dataset for header in HDF5 file");
	return(2);
    };

    /*
     * Write AVHRR station data to dataset.
     */
    status = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, 
	H5P_DEFAULT, &aobs);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not write header to HDF5 file\n");
	return(2);
    };

    /*
     * Close/release resources for AVHRR.
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close field dataspace in HDF5 file");
	return(2);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close field datatype in HDF5 file");
	return(2);
    };
    status = H5Tclose(ucs_id);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close field datatype in HDF5 file");
	return(2);
    };
    status = H5Tclose(imga_id);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close field datatype in HDF5 file");
	return(2);
    };
    status = H5Tclose(farr_id);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close farr_id datatype in HDF5 file");
	return(2);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close dataset in HDF5 file");
	return(2);
    };

    /*
     * ************************************************************************
     * Add data for NWP stationwise datasets the same way as AVHRR data
     * were added.
     */
    /*
     * Create and write a compound data type containing NWP information.
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not create dataspace for NWP data in HDF5 file");
	return(2);
    };

    /*
     * Create compound datatype for NWP data (see nwp_stdat.h).
     */
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(ns_data));
    if (datatype < 0) {
	fprintf(stderr,"%s%s\n",errmsg, 
	    "Could not define compound datatype for NWP data");
	return(2);
    };
    status = H5Tinsert(datatype, "nopar", HOFFSET(ns_data, nopar), 
	    H5T_NATIVE_INT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert nopar element in NWP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "nopix", HOFFSET(ns_data, nopix), 
	    H5T_NATIVE_INT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert nopix element in NWP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "vtime", HOFFSET(ns_data, vtime), 
	    H5T_NATIVE_INT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert vtime element in NWP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "ltime", HOFFSET(ns_data, ltime), 
	    H5T_NATIVE_INT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert ltime element in NWP compound datatype");
	return(2);
    }
    /* Compound datatype for ucs */
    ucs_id = H5Tcreate(H5T_COMPOUND, sizeof(struct ucs));
    if (ucs_id < 0) {
	fprintf(stderr,"%s%s\n",errmsg, 
	    "Could not define compound datatype for UCS data");
	return(2);
    };
    status = H5Tinsert(ucs_id, "iw", HOFFSET(struct ucs, iw), 
	    H5T_NATIVE_UINT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert iw element in UCS compound datatype");
	return(2);
    }
    status = H5Tinsert(ucs_id, "ih", HOFFSET(struct ucs, ih), 
	    H5T_NATIVE_UINT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert ih element in UCS compound datatype");
	return(2);
    }
    status = H5Tinsert(ucs_id, "Ax", HOFFSET(struct ucs, Ax), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert Ax element in UCS compound datatype");
	return(2);
    }
    status = H5Tinsert(ucs_id, "Ay", HOFFSET(struct ucs, Ay), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert Ay element in UCS compound datatype");
	return(2);
    }
    status = H5Tinsert(ucs_id, "Bx", HOFFSET(struct ucs, Bx), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert Ay element in UCS compound datatype");
	return(2);
    }
    status = H5Tinsert(ucs_id, "By", HOFFSET(struct ucs, By), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert By element in UCS compound datatype");
	return(2);
    }
    /* Put UCS in NWP */
    status = H5Tinsert(datatype, "nav", HOFFSET(ns_data, nav), 
	    ucs_id);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert nav element in NWP compound datatype");
	return(2);
    }
    /* Insert actual image data */
    dima[0] = nobs.nopar;
    dima[1] = nobs.nopix;
    farr_id = H5Tarray_create(H5T_NATIVE_FLOAT,2,dima,NULL); 
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not array datatype in NWP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "data", HOFFSET(ns_data, data), 
	    farr_id);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert array element in NWP compound datatype");
	return(2);
    }

    /*
     * Create dataset for NWP.
     */
    dataset = H5Dcreate(grp, "NWP", datatype, dataspace,
	H5P_DEFAULT);
    if (dataset < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not create NWP dataset in HDF5 file");
	return(2);
    };

    /*
     * Write NWP station data to dataset.
     */
    status = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, 
	H5P_DEFAULT, &nobs);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not write NWP to HDF5 file\n");
	return(2);
    };

    /*
     * Close/release resources for NWP.
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close field dataspace in HDF5 file");
	return(2);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close field datatype in HDF5 file");
	return(2);
    };
    status = H5Tclose(ucs_id);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close field datatype in HDF5 file");
	return(2);
    };
    status = H5Tclose(farr_id);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close farr_id datatype in HDF5 file");
	return(2);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close dataset in HDF5 file");
	return(2);
    };

    /*
     * ************************************************************************
     * Add data for SYNOP like datasets the same way as AVHRR and NWP data
     * were added.
     */
    /*
     * Create and write a compound data type containing SYNOP information.
     */
    dataspace = H5Screate_simple(1, dimh, NULL);
    if (dataspace < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not create dataspace for SYNOP data in HDF5 file");
	return(2);
    };

    /*
     * Create compound datatype for SYNOP data (see std_stdat.h).
     */
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(obsstruct));
    if (datatype < 0) {
	fprintf(stderr,"%s%s\n",errmsg, 
	    "Could not define compound datatype for SYNOP data");
	return(2);
    };
    status = H5Tinsert(datatype, "stID", HOFFSET(obsstruct, stID), str10);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert stID element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "stType", HOFFSET(obsstruct, stType), str5);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert stType element in SYNOP compound datatype");
	return(2);
    }
    /* Compund for latlon */
    latlon_id = H5Tcreate(H5T_COMPOUND, sizeof(struct latlon));
    if (latlon_id < 0) {
	fprintf(stderr,"%s%s\n",errmsg, 
	    "Could not define compound datatype for latlon data");
	return(2);
    };
    status = H5Tinsert(latlon_id, "lat", HOFFSET(struct latlon, lat), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert lat element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(latlon_id, "lon", HOFFSET(struct latlon, lon), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert lon element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "pos", HOFFSET(obsstruct, pos), 
	    latlon_id);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert pos element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tclose(latlon_id);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close latlon_id datatype in HDF5 file");
	return(2);
    };
    /* Continue after latlon */
    status = H5Tinsert(datatype, "year", HOFFSET(obsstruct, year), 
	    H5T_NATIVE_INT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert year element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "month", HOFFSET(obsstruct, month), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert month element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "day", HOFFSET(obsstruct, day), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert day element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "hour", HOFFSET(obsstruct, hour), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert hour element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "min", HOFFSET(obsstruct, min), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert min element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "TTT", HOFFSET(obsstruct, TTT), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert TTT element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "TdTdTd", HOFFSET(obsstruct, TdTdTd), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert TdTdTd element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "TwTwTw", HOFFSET(obsstruct, TwTwTw), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert TwTwTw element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "PPPP", HOFFSET(obsstruct, PPPP), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert PPPP element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "ff", HOFFSET(obsstruct, ff), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert ff element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "dd", HOFFSET(obsstruct, dd), 
	    H5T_NATIVE_INT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert dd element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "RRR", HOFFSET(obsstruct, RRR), 
	    H5T_NATIVE_FLOAT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert RRR element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "E", HOFFSET(obsstruct, E), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert E element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "sss", HOFFSET(obsstruct, sss), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert sss element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "N", HOFFSET(obsstruct, N), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert N element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "Nh", HOFFSET(obsstruct, Nh), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert Nh element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "Cl", HOFFSET(obsstruct, Cl), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert Cl element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "Cm", HOFFSET(obsstruct, Cm), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert Cm element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "Ch", HOFFSET(obsstruct, Ch), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert Ch element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "VV", HOFFSET(obsstruct, VV), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert VV element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "ww", HOFFSET(obsstruct, ww), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert ww element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "W1", HOFFSET(obsstruct, W1), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert W1 element in SYNOP compound datatype");
	return(2);
    }
    status = H5Tinsert(datatype, "W2", HOFFSET(obsstruct, W2), 
	    H5T_NATIVE_SHORT);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not insert W2 element in SYNOP compound datatype");
	return(2);
    }
    /*
     * Create dataset for SYNOP.
     */
    dataset = H5Dcreate(grp, "STDAT", datatype, dataspace,
	H5P_DEFAULT);
    if (dataset < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not create STDAT dataset in HDF5 file");
	return(2);
    };

    /*
     * Write STDAT station data to dataset.
     */
    status = H5Dwrite(dataset, datatype, H5S_ALL, H5S_ALL, 
	H5P_DEFAULT, &sobs);
    if (status < 0) {
	fprintf(stderr, "%s%s\n",errmsg,
	    "Could not write STDAT to HDF5 file\n");
	return(2);
    };

    /*
     * Close/release resources for STDAT.
     */
    status = H5Sclose(dataspace);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close field dataspace in HDF5 file");
	return(2);
    };
    status = H5Tclose(datatype);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close field datatype in HDF5 file");
	return(2);
    };
    status = H5Dclose(dataset);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	    "Could not close dataset in HDF5 file");
	return(2);
    };

    /*
     **************************************************************************
     * Close group.
     */
    status = H5Gclose(grp);
    if (status < 0) {
	fprintf(stderr,"%s%s\n",errmsg,
	"Could not close data group in HDF5 file.");
	return(2);
    };

    /*
     * Close HDF file.
     */
    status = H5Fclose(file);
    if (status < 0) {
	fprintf(stderr, 
	    "%s%s\n %s\n",errmsg,
	    "Could not create new HDF5 file:", filename);
	return(2);
    };

    return(0);
}

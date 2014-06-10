/*
 * fm_readdataMETSATswath.c
 *
 *  Created on: 11. apr. 2014
 *      Author: anettelb
 */


#include <fmutil.h>
#include <fmio.h>
#include <hdf5.h>


#define PPSCTDATASETS 2
#define PPSCMDATASETS 2
#define GACDATASETS 4

//Internal functions
int fm_readH5data(char *filename, fmdataset *d, fmbool headeronly);
int fm_create_hdf5_string(hid_t *str, size_t size);
int fm_create_hdf5_vlstring(hid_t *str);
int fm_extracthow(hid_t grp, fmheader *h);
int fm_extractwhat(hid_t grp, fmheader *h);
int fm_extractwhere(hid_t grp, fmheader *h);
int fm_extractimagedata(hid_t file_id, fmdataset *d);
int fmget_hdf5_int_att(hid_t grp_id, char *attname, int *outbuf);
int fmget_hdf5_long_att(hid_t grp_id, char *attname, long *outbuf);
int fmget_hdf5_ulong_att(hid_t grp_id, char *attname, unsigned long long *outbuf);
int fmget_hdf5_string_att(hid_t grp_id, char *attname, char *outbuf);
int fm_extractppsdata(hid_t file_id, fmdataset *d);
int fm_extractppsregion(hid_t file_id, fmheader *h);
int fm_extractppsvaltab(hid_t dset_id, fmdatafield *d);

typedef struct {
    double area_extent[4];
    int xsize;
    int ysize;
    float xscale;
    float yscale;
    float lat_0;
    float lon_0;
    float lat_ts;
    char id[64];
    char name[64];
    char pcs_id[64];
    char pcs_def[128];
} ppsregion;


int fm_readMETSATdata_swath(char *filename, fmdataset *fd) {
	//Input: File name, struct fd to be filled

    char *where="fm_readdataMETSATswath",mymsg[255];
    char mystring[FMSTRING128];

    hid_t file, dataset, datatype, dataspace, attr_space;
    hid_t grp, attr_id;
    H5T_class_t datatype_class;
    H5T_sign_t datatype_sign;
    hsize_t datatype_size, dataset_size, group_size;
    hsize_t dsd_d[2];
    herr_t status;
    fmbool isviirs = FMFALSE, isavhrr = FMFALSE;
    H5G_info_t gr_slash_info;

    /*
     * Control error handling.
     */
    H5Eset_auto(NULL,NULL);


    /*
     * Open H5 file and check contents.
     */

    file = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file < 0) {
    	fmerrmsg(where,"Could not open %s", filename);
    	return(FM_IO_ERR);
    };

    grp = H5Gopen(file,"how"); //Try to open group "how" in file
    if(grp >= 0) { //If the group exists = the file is a avhrr/viirs data file or sun zenith angle file


    	//Read "how" group data (satellite name, instrument, orbit number, epoch)
    	if (fm_extracthow(grp, &(fd->h))) {
    		fmerrmsg(where,"Could not decode HOW group in file");
    		return(FM_IO_ERR);
    	}
    	status = H5Gclose(grp);
    	if (status < 0) {
    		fmerrmsg(where,"Could not close group in %s", filename);
    		return(FM_IO_ERR);
    	};


    	//Read "what" group data (date, time, number of layers (channels/images))
    	grp = H5Gopen(file,"what");
    	if (grp < 0) {fmlogmsg(where,"Could not find group WHAT."); return(FM_IO_ERR);}
    	if (fm_extractwhat(grp, &(fd->h))) {
    		fmerrmsg(where,"Could not decode WHAT group in file");
    		return(FM_IO_ERR);
    	}
    	status = H5Gclose(grp);
    	if (status < 0) {
    		fmerrmsg(where,"Could not close group in %s", filename);
    		return(FM_IO_ERR);
    	};



    	//Read "where" group data (xsize, ysize, xscale, yscale)
    	grp = H5Gopen(file,"where");
    	if (grp < 0) {fmlogmsg(where,"Could not find group WHERE."); return(FM_IO_ERR);}
    	if (fm_extractwhere(grp, &(fd->h))) {
    		fmerrmsg(where,"Could not decode WHERE group in file");
    		return(FM_IO_ERR);
    	}
    	status = H5Gclose(grp);
    	if (status < 0) {
    		fmerrmsg(where,"Could not close group in %s", filename);
    		return(FM_IO_ERR);
    	};



    	//Read actual data content (the channel/image data), variable number of channels/images
    	if (fm_extractimagedata(file, fd)) {
    		fmerrmsg(where,"Could not decode image data");
    		return(FM_IO_ERR);
    	}


    	//Check which satellite/instrument
    	if (strstr(fd->h.sensor_name,"avhrr") || strstr(fd->h.sensor_name,"viirs")) {
    		if (strstr(fd->h.platform_name,"noaa")) {
    			char name[6]; strncpy(name,fd->h.platform_name,6);
    			sprintf(fd->h.platform_name, "NOAA-%s",&(name[4])); //Re-spell name (e.g. NOAA-18)
    		}
    		if (strstr(fd->h.platform_name,"npp")) {
    			sprintf(fd->h.platform_name, "NPP"); //Re-spell name
    		}
    		if (strstr(fd->h.platform_name,"metop")) {
    			char name[7]; strncpy(name,fd->h.platform_name,7);
    			sprintf(fd->h.platform_name, "MetOp-0%s",&(name[6])); //Re-spell name (e.g. MetOp-01)
    		}
    		fmlogmsg(where,"This is a H5 file containing data from %s: %s",fd->h.platform_name,fd->h.sensor_name);
    	}    else { fmerrmsg(where, "Do not recognize instrument, bailing out!"); return(FM_OTHER_ERR); }

    }
    else { //Physiography file
    	fmlogmsg(where,"Could not find group HOW. Not a regular data file.");

    	hid_t ds_id; //Dataset id
    	hid_t dataset_id;
    	unsigned char *mydata;
    	hsize_t dsize;

    	hid_t str, dataspace, dataset, datatype;
    	hsize_t dsd_d[2];
    	int **intarray;
    	int xsize, ysize;

    	//Read "region" dataset (xsize, ysize, xscale, yscale), insert info into struct header
    	if (fm_extractppsregion(file,  &(fd->h))) {
    		fmerrmsg(where,"Could not read region");
    		return(FM_IO_ERR);
    	}


    	//Set data space sizes
    	dsd_d[0] = fd->h.xsize;
    	dsd_d[1] = fd->h.ysize;
    	fd->h.layers = 2; //There are two data sets (fracofland and the palette) in this file that we want to read into a datafield struct.


    	dataset = H5Dopen(file,"fracofland"); //Open dataset "fracofland"
    	if(dataset >= 0) { //Physiography file
    		fmlogmsg(where,"Physiography file detected.");

    		//Allocate dataspace, a data field in the struct, a 2d array in the data field and a 1d temp array (for reading data into).
    		dataspace = H5Screate_simple(2, dsd_d, NULL);
    		if (dataspace < 0) {
    			fmerrmsg(where,"Could not create dataspace");
    			return(FM_IO_ERR);
    		};
    		if (allocate_fmdatafield(&(fd->d),fd->h.layers)) {
    			fmerrmsg(where,"Could not allocate the necessary number of layers");
    			return(FM_MEMALL_ERR);
    		}
    		if (fmalloc_int_2d(&((fd->d)[0].intarray),fd->h.ysize,fd->h.xsize)) {
    			fmerrmsg(where,"Could not allocate data array");
    			return(FM_MEMALL_ERR);
    		}
//    		if (fmalloc_int_vector(&mydata, (fd->h.xsize*fd->h.ysize))) {
    		if (fmalloc_uchar_vector(&mydata, (fd->h.xsize*fd->h.ysize))) {
    			fmerrmsg(where,"Could not allocate data array");
    			return(FM_MEMALL_ERR);
    		}


    		fd->d[0].dtype = FMINT; //The data type is int

    		//Read data into temp 1d array mydata
    		status = H5Dread(dataset, H5T_STD_U8LE, H5S_ALL, H5S_ALL, H5P_DEFAULT,mydata);
    		if (status < 0) {
    			fmerrmsg(where,"Could not read data field");
    			return(FM_IO_ERR);
    		};
    		status = H5Dclose(dataset);
    		if (status < 0) {
    			fmerrmsg(where,"Could not close dataset in HDF5 file");
    			return(FM_IO_ERR);
    		};
    		status = H5Sclose(dataspace);
    		if (status < 0) {
    			fmerrmsg(where,"Could not close field dataspace in HDF5 file");
    			return(FM_IO_ERR);
    		};

    		//Transfer data from temporary array to data field in struct
    		int j,k;
    		for (j=0;j<fd->h.ysize; j++) {
    			for (k=0;k<fd->h.xsize; k++) {
    				(fd->d)[0].intarray[j][k] = +mydata[fmivec(k, j, fd->h.xsize)];
    			}
    		}

    		//Free the temporary array
    		if (fmfree_uchar_vector(mydata)) {
    			fmerrmsg(where,"Could not free mydata");
    			return(FM_MEMALL_ERR);
    		}



//    		dataset = H5Dopen(file,"FractionOfLandPALETTE"); //Open dataset palette
//    		if (dataset < 0) {
//    			fmerrmsg(where,"Could not find dataset palette");
//    			return(FM_IO_ERR);
//    		};
//
//    		dsd_d[0] = 256;
//    		dsd_d[1] = 3;
//
//    		//Allocate dataspace, a data field in the struct, a 2d array in the data field and a 1d temp array (for reading data into).
//    		dataspace = H5Screate_simple(2, dsd_d, NULL);
//    		if (dataspace < 0) {
//    			fmerrmsg(where,"Could not create dataspace");
//    			return(FM_IO_ERR);
//    		};
//    		if (fmalloc_int_2d(&((fd->d)[1].intarray),3,256)) {
//    			fmerrmsg(where,"Could not allocate data array");
//    			return(FM_MEMALL_ERR);
//    		}
//    		if (fmalloc_uchar_vector(&mydata, (3*256))) {
//    			fmerrmsg(where,"Could not allocate data array");
//    			return(FM_MEMALL_ERR);
//    		}
//
//    		fd->d[1].dtype = FMINT; //The data type is int
//
//    		//Read data into temp 1d array mydata
//    		status = H5Dread(dataset, H5T_STD_U8LE, H5S_ALL, H5S_ALL, H5P_DEFAULT,mydata);
//    		if (status < 0) {
//    			fmerrmsg(where,"Could not read data field");
//    			return(FM_IO_ERR);
//    		};
//    		status = H5Dclose(dataset);
//    		if (status < 0) {
//    			fmerrmsg(where,"Could not close dataset in HDF5 file");
//    			return(FM_IO_ERR);
//    		};
//    		status = H5Sclose(dataspace);
//    		if (status < 0) {
//    			fmerrmsg(where,"Could not close field dataspace in HDF5 file");
//    			return(FM_IO_ERR);
//    		};
//
//    		//Transfer data from temporary array to data field in struct
//    		for (j=0;j<3; j++) {
//    			for (k=0;k<256; k++) {
//    				(fd->d)[1].intarray[j][k] = +mydata[fmivec(k, j, 256)];
//    			}
//    		}
//
//    		//Free the temporary array
//    		if (fmfree_uchar_vector(mydata)) {
//    			fmerrmsg(where,"Could not free mydata");
//    			return(FM_MEMALL_ERR);
//    		}
    	}
    }

    //Close the file
    status = H5Fclose(file);
    if (status < 0) {
    	fmerrmsg(where,"Could not close file %s", filename);
    	return(FM_IO_ERR);
    };

    return(0);
}

int fm_readH5data(char *filename, fmdataset *d, fmbool headeronly) {


	char *where="fm_readHLHDFdata";
	hid_t file, dataset, datatype, dataspace, attr_space;
	hid_t str17;
	hid_t grp, attr_id;
    H5T_class_t datatype_class;
    H5T_sign_t datatype_sign;
    hsize_t datatype_size, dataset_size;
    hsize_t dsd_d[2];
    herr_t status;
    char *ppsctdatasets[PPSCTDATASETS]= {"cloudtype","region"};
    char *ppscmdatasets[PPSCMDATASETS]= {"cloudmask","region"};
    char *gacdatasets[GACDATASETS]={"how","what","where","image1"};
    char *gacdescriptions[2]={"AVHRR ch1","sun zenith angle"};
    char mystring[FMSTRING128];
    int i, j, k;
    fmbool isppsct = FMFALSE, isppscm = FMFALSE, isavhrr = FMFALSE, isangles = FMFALSE;

    /*
     * Control error handling.
     */
    H5Eset_auto(NULL,NULL);

    /*
     * Create string data types.
     */
    if (fm_create_hdf5_string(&str17,17)) {
        fmerrmsg(where,"Could not create HDF5 string.");
        return(FM_OTHER_ERR);
    }

    /*
     * Open HDF file and check contents. While doing this we also fill the
     * contents of the header if the required elements are found.
     */
    file = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
    if (file < 0) {
        fmerrmsg(where,"Could not open %s", filename);
        return(FM_IO_ERR);
    };
    for (i=0;i<3;i++) {
        if (i==0) {
            for (j=0;j<GACDATASETS;j++) {
                grp = H5Gopen(file,gacdatasets[j]);
                if (grp < 0) {
                    fmlogmsg(where,"Could not find group %s, not AVHRR or geometry data", gacdatasets[j]);
                    isavhrr = isangles = FMFALSE;
                    break;
                };
                if (strstr(gacdatasets[j],"image1")) {
                	fmlogmsg(where,"Checking contents of description");
                    attr_id = H5Aopen_name(grp,"description");
                    if (attr_id < 0) {
                        fmerrmsg(where,"Could not open attribute description.");
                        break;
                    }
                    status = H5Aread(attr_id,str17,mystring);
                    if (status < 0) {
                        fmerrmsg(where,"Could not read attribute description.");
                        break;
                    }
                    status = H5Aclose(attr_id);
                    if (status < 0) {
                        fmerrmsg(where,
                            "Could not close attribute description, bailing out!");
                        return(FM_IO_ERR);
                    }
                    if (strstr(mystring,gacdescriptions[0])) {
                        isavhrr = FMTRUE;
                        fmlogmsg(where,
                            "This is a HLHDF file containing AVHRR data.");
                        break;
                    } else if (strstr(mystring,gacdescriptions[1])) {
                        isangles = FMTRUE;
                        fmlogmsg(where,
                            "This is a HLHDF file containing geometry data.");
                        break;
                    }
                } else if (strstr(gacdatasets[j],"how")) {
                	if (fm_extracthow(grp, &(d->h))) {
                    	fmerrmsg(where,"Could not decode HOW group in file");
                        return(FM_IO_ERR);
                    }
                } else if (strstr(gacdatasets[j],"what")) {
                    if (fm_extractwhat(grp, &(d->h))) {
                        fmerrmsg(where,"Could not decode WHAT group in file");
                        return(FM_IO_ERR);
                    }
                } else if (strstr(gacdatasets[j],"where")) {
                    if (fm_extractwhere(grp, &(d->h))) {
                        fmerrmsg(where,"Could not decode WHERE group in file");
                        return(FM_IO_ERR);
                    }
                }
                status = H5Gclose(grp);
                if (status < 0) {
                    fmerrmsg(where,"Could not close group in %s", filename);
                    return(FM_IO_ERR);
                };
            }
        } else if (i==1 && !(isavhrr || isangles)) {
            for (j=0;j<PPSCTDATASETS;j++) {
                fmlogmsg(where,"Checking for dataset: %s", ppsctdatasets[j]);
                dataset = H5Dopen(file,ppsctdatasets[j]);
                if (dataset < 0) {
                    fmlogmsg(where,
                            "Could not open %s, this is not a PPS CT dataset",
                            ppsctdatasets[j]);
                    isppsct = FMFALSE;
                    break;
                };
                isppsct = FMTRUE;
            }
            if (isppsct) {
                fmlogmsg(where,
                    "This is a HLHDF file containing PPS cloudtype data");
            }
        } else if (i==2 && !(isavhrr || isangles || isppsct)) {
            for (j=0;j<PPSCMDATASETS;j++) {
                fmlogmsg(where,"Checking for dataset: %s", ppscmdatasets[j]);
                dataset = H5Dopen(file,ppscmdatasets[j]);
                if (dataset < 0) {
                    fmlogmsg(where,
                            "Could not open %s, this is not a PPS CM dataset",
                            ppscmdatasets[j]);
                    isppscm = FMFALSE;
                    break;
                };
                isppscm = FMTRUE;
            }
            if (isppscm) {
                fmlogmsg(where,
                    "This is a HLHDF file containing PPS cloudmask data");
            }

        }
    }
    if (!(isavhrr || isangles || isppsct || isppscm)) {
        fmerrmsg(where,"Could not determine the type of this HLHDF file.\n");
        return(FM_IO_ERR);
    }
    if (H5Tclose(str17)<0) {
        fmerrmsg(where,"Could not release str16.");
        return(FM_OTHER_ERR);
    }

    if (headeronly) {
        status = H5Fclose(file);
        if (status < 0) {
            fmerrmsg(where,"Could not close %s", filename);
            return(FM_IO_ERR);
        };

        return(FM_OK);
    }

    /*
     * If data are requested, we also read the data content
     */
    if (isavhrr || isangles) {
        if (fm_extractimagedata(file, d)) {
            fmerrmsg(where,"Could not decode image data");
            return(FM_IO_ERR);
        }
    } else if (isppsct || isppscm) {
        if (fm_extractppsdata(file, d)) {
            fmerrmsg(where,"Could not decode cloud data");
            return(FM_IO_ERR);
        }
    } else {
        fmerrmsg(where,"Somehow the type of this file has yet not been determined and is not supported.");
        if (status < 0) {
            fmerrmsg(where,"Could not close %s", filename);
            return(FM_IO_ERR);
        };
        return(FM_IO_ERR);
    }

    /*
     * Finally finished, now we close the file...
     */
    status = H5Fclose(file);
    if (status < 0) {
        fmerrmsg(where,"Could not close %s", filename);
        return(FM_IO_ERR);
    };

    return(FM_OK);
}

int fm_create_hdf5_string(hid_t *str, size_t size) {
    char *where="fm_create_hdf5_string";
    herr_t status;

    *str = H5Tcopy(H5T_C_S1);
    if (*str < 0) {
        fmerrmsg(where,"Could not initialize str");
        return(FM_OTHER_ERR);
    }
    status = H5Tset_size(*str,size);
    if (status < 0) {
        fmerrmsg(where,"Could not set size for str.");
        return(FM_OTHER_ERR);
    }
    status = H5Tset_strpad(*str,H5T_STR_NULLTERM);
    if (status < 0) {
        fmerrmsg(where,"Could not terminate str.");
        return(FM_OTHER_ERR);
    }

    return(FM_OK);
}

int fm_create_hdf5_vlstring(hid_t *str) {
    char *where="fm_create_hdf5_vlstring";
    herr_t status;

    *str = H5Tcopy(H5T_C_S1);
    if (*str < 0) {
        fmerrmsg(where,"Could not initialize str");
        return(FM_OTHER_ERR);
    }
    status = H5Tset_size(*str,H5T_VARIABLE);
    if (status < 0) {
        fmerrmsg(where,"Could not set H5T_VARIABLE for str.");
        return(FM_OTHER_ERR);
    }
    status = H5Tset_strpad(*str,H5T_STR_NULLTERM);
    if (status < 0) {
        fmerrmsg(where,"Could not terminate str.");
        return(FM_OTHER_ERR);
    }

    return(FM_OK);
}

/*
 * Missing geographical positions so far, but that is part of the data and
 * not the header...
 */
int fm_extractwhere(hid_t grp, fmheader *h) {
    char *where="fm_extractwhere";
    hid_t attr_id, grp_id, grp_id2;
    hid_t str;
    herr_t status;
    float gain, offset;
    hid_t dataspace, dataset;
    H5A_info_t ainfo;
    hsize_t dsd_d[2];
    int i, j, k, *mydata;


    /*
     * Decode size (rows and columns)
     */
    attr_id = H5Aopen_name(grp,"num_of_lines");
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute num_of_lines.");
        return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,H5T_NATIVE_INT,&(h->ysize));
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute num_of_lines.");
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute num_of_lines, bailing out!");
        return(FM_IO_ERR);
    }
    attr_id = H5Aopen_name(grp,"num_of_pixels");
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute num_of_pixels.");
        return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,H5T_NATIVE_INT,&(h->xsize));
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute num_of_pixels.");
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute num_of_pixels, bailing out!");
        return(FM_IO_ERR);
    }

    /*
     * Decode nominal grid size
     */
    attr_id = H5Aopen_name(grp,"xscale");
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute xscale.");
        return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,H5T_NATIVE_FLOAT,&(h->nominal_grid_resolution_x));
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute xscale.");
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute xscale, bailing out!");
        return(FM_IO_ERR);
    }
    attr_id = H5Aopen_name(grp,"yscale");
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute yscale.");
        return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,H5T_NATIVE_FLOAT,&(h->nominal_grid_resolution_y));
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute yscale.");
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute yscale, bailing out!");
        return(FM_IO_ERR);
    }


    /*
     * Decode position of upper left corner
     * Read latitude
     */
    grp_id = H5Gopen(grp,"lat");
    if (grp_id < 0) {fmlogmsg(where,"Could not find group LAT in group WHERE."); return(FM_IO_ERR);}

    grp_id2 = H5Gopen(grp_id,"what");
    if (grp_id2 < 0) {
    	fmerrmsg(where,
    			"Could not find group what in LAT");
    	return(FM_IO_ERR);
    };

    attr_id = H5Aopen_name(grp_id2,"gain");
    if (attr_id < 0) {
    	fmerrmsg(where,"Could not open attribute gain.");
    	return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,H5T_NATIVE_FLOAT,&gain);
    if (status < 0) {
    	fmerrmsg(where,"Could not read attribute gain.");
    	return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
    	fmerrmsg(where,
    			"Could not close attribute gain, bailing out!");
    	return(FM_IO_ERR);
    }
    attr_id = H5Aopen_name(grp_id2,"offset");
    if (attr_id < 0) {
    	fmerrmsg(where,"Could not open attribute offset.");
    	return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,H5T_NATIVE_FLOAT,&offset);
    if (status < 0) {
    	fmerrmsg(where,"Could not read attribute offset.");
    	return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
    	fmerrmsg(where,
    			"Could not close attribute offset, bailing out!");
    	return(FM_IO_ERR);
    }

    status = H5Gclose(grp_id2);
    if (status < 0) {
    	fmerrmsg(where,"Could not close group WHAT");
    	return(FM_IO_ERR);
    };

    /*
     * Read the actual data
     */
    dsd_d[0] = h->xsize;
    dsd_d[1] = h->ysize;
    dataspace = H5Screate_simple(2, dsd_d, NULL);
    if (dataspace < 0) {
    	fmerrmsg(where,"Could not create dataspace");
    	return(FM_IO_ERR);
    };

    dataset = H5Dopen(grp_id,"data");

    if (dataset < 0) {
    	fmerrmsg(where,"Could not open dataset data [%d]", dataset);
    	return(FM_IO_ERR);
    };

    if (fmalloc_int_vector(&mydata, (h->xsize*h->ysize))) {
    	fmerrmsg(where,"Could not allocate data array");
    	return(FM_MEMALL_ERR);
    }


    status = H5Dread(dataset,
    		H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
    		mydata);


    if (status < 0) {
    	fmerrmsg(where,"Could not read data field");
    	return(FM_IO_ERR);
    };

    status = H5Dclose(dataset);
    if (status < 0) {
    	fmerrmsg(where,"Could not close dataset in HDF5 file");
    	return(FM_IO_ERR);
    };
    status = H5Sclose(dataspace);
    if (status < 0) {
    	fmerrmsg(where,"Could not close field dataspace in HDF5 file");
    	return(FM_IO_ERR);
    };

    h->ucs.Bx = mydata[0]*gain + offset;
    float delta1 = (mydata[1]*gain + offset) - (mydata[0]*gain + offset);


    if (fmfree_int_vector(mydata)) {
    	fmerrmsg(where,"Could not free mydata");
    	return(FM_MEMALL_ERR);
    }


    status = H5Gclose(grp_id);
    if (status < 0) {
    	fmerrmsg(where,"Could not close group LAT");
    	return(FM_IO_ERR);
    };

    /*
     * Read longitude
     */

    grp_id = H5Gopen(grp,"lon");
        if (grp_id < 0) {fmlogmsg(where,"Could not find group LAT in group WHERE."); return(FM_IO_ERR);}

        grp_id2 = H5Gopen(grp_id,"what");
        if (grp_id2 < 0) {
        	fmerrmsg(where,
        	    			"Could not find group what in LON");
        	    	return(FM_IO_ERR);
        };

        attr_id = H5Aopen_name(grp_id2,"gain");
        if (attr_id < 0) {
        	fmerrmsg(where,"Could not open attribute gain.");
        	return(FM_IO_ERR);
        }
        status = H5Aread(attr_id,H5T_NATIVE_FLOAT,&gain);
        if (status < 0) {
        	fmerrmsg(where,"Could not read attribute gain.");
        	return(FM_IO_ERR);
        }
        status = H5Aclose(attr_id);
        if (status < 0) {
        	fmerrmsg(where,
        			"Could not close attribute gain, bailing out!");
        	return(FM_IO_ERR);
        }
        attr_id = H5Aopen_name(grp_id2,"offset");
        if (attr_id < 0) {
        	fmerrmsg(where,"Could not open attribute offset.");
        	return(FM_IO_ERR);
        }
        status = H5Aread(attr_id,H5T_NATIVE_FLOAT,&offset);
        if (status < 0) {
        	fmerrmsg(where,"Could not read attribute offset.");
        	return(FM_IO_ERR);
        }
        status = H5Aclose(attr_id);
        if (status < 0) {
        	fmerrmsg(where,
        			"Could not close attribute offset, bailing out!");
        	return(FM_IO_ERR);
        }

        status = H5Gclose(grp_id2);
        if (status < 0) {
        	fmerrmsg(where,"Could not close group WHAT");
        	return(FM_IO_ERR);
        };

        /*
         * Read the actual data
         */
        dsd_d[0] = h->xsize;
        dsd_d[1] = h->ysize;
        dataspace = H5Screate_simple(2, dsd_d, NULL);
        if (dataspace < 0) {
        	fmerrmsg(where,"Could not create dataspace");
        	return(FM_IO_ERR);
        };

        dataset = H5Dopen(grp_id,"data");

        if (dataset < 0) {
        	fmerrmsg(where,"Could not open dataset data [%d]", dataset);
        	return(FM_IO_ERR);
        };

        if (fmalloc_int_vector(&mydata, (h->xsize*h->ysize))) {
        	fmerrmsg(where,"Could not allocate data array");
        	return(FM_MEMALL_ERR);
        }

        status = H5Dread(dataset,
        		H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
        		mydata);

        if (status < 0) {
        	fmerrmsg(where,"Could not read data field");
        	return(FM_IO_ERR);
        };

        status = H5Dclose(dataset);
        if (status < 0) {
        	fmerrmsg(where,"Could not close dataset in HDF5 file");
        	return(FM_IO_ERR);
        };
        status = H5Sclose(dataspace);
        if (status < 0) {
        	fmerrmsg(where,"Could not close field dataspace in HDF5 file");
        	return(FM_IO_ERR);
        };

        h->ucs.By = mydata[0]*gain + offset;
        float delta2 = mydata[1]-mydata[0];

        if (fmfree_int_vector(mydata)) {
        	fmerrmsg(where,"Could not free mydata");
        	return(FM_MEMALL_ERR);
        }


        status = H5Gclose(grp_id);
        if (status < 0) {
        	fmerrmsg(where,"Could not close group LAT");
        	return(FM_IO_ERR);
        };


        h->ucs.Ax = h->nominal_grid_resolution_x*0.001;
        h->ucs.Ay = h->nominal_grid_resolution_y*0.001;

//
//        fprintf(stdout,"UCS: %f %f %f %f %f %f\n",h->ucs.Ax,h->ucs.Ay,h->ucs.Bx,h->ucs.By,delta1,delta2);


    return(FM_OK);
}

int fm_extractwhat(hid_t grp, fmheader *h) {
    char *where="fm_extractwhat";
    hid_t attr_id;
    hid_t str;
    herr_t status;
    char mydate[FMSTRING16], mytime[FMSTRING16];

    /*
     * Decode valid time
     */
    if (fm_create_hdf5_string(&str, 9)) {
        fmerrmsg(where,"Could not create str");
        return(FM_OTHER_ERR);
    }
    attr_id = H5Aopen_name(grp,"date");
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute date.");
        return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,str,mydate);
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute date.");
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute date, bailing out!");
        return(FM_IO_ERR);
    }
    if (H5Tclose(str) < 0) {
        fmerrmsg(where,"Could not release str");
        return(FM_OTHER_ERR);
    }
    if (fm_create_hdf5_string(&str, 7)) {
        fmerrmsg(where,"Could not create str");
        return(FM_OTHER_ERR);
    }
    attr_id = H5Aopen_name(grp,"time");
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute time.");
        return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,str,mytime);
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute time.");
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute time, bailing out!");
        return(FM_IO_ERR);
    }
    if (H5Tclose(str) < 0) {
        fmerrmsg(where,"Could not release str");
        return(FM_OTHER_ERR);
    }
    strcat(mydate,mytime);
    if (fmstring2fmtime(mydate, "YYYYMMDDhhmmss", &(h->valid_time))) {
        fmerrmsg(where,"Could not decode date and time string.");
        return(FM_IO_ERR);
    }

    /*
     * Decode number of layers
     */
    attr_id = H5Aopen_name(grp,"sets");
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute sets.");
        return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,H5T_NATIVE_INT,&(h->layers));
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute sets.");
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute sets, bailing out!");
        return(FM_IO_ERR);
    }

    return(FM_OK);
}

/*
 * Missing yaw, pitch and roll mainly...
 */
int fm_extracthow(hid_t grp, fmheader *h) {
    char *where="fm_extracthow";
    hid_t attr_id;
    hid_t str;
    herr_t status;
    fmsec1970 timespan[2];


    /*
     * Decode platform information
     */
    if (fm_create_hdf5_string(&str, 10)) {
        fmerrmsg(where,"Could not create str");
        return(FM_OTHER_ERR);
    }
    attr_id = H5Aopen_name(grp,"platform");
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute platform.");
        return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,str,h->platform_name);
    fprintf(stdout,"platform: %s %i\n",h->platform_name,status);
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute platform.");
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute platform, bailing out!");
        return(FM_IO_ERR);
    }
    if (H5Tclose(str) < 0) {
        fmerrmsg(where,"Could not release str");
        return(FM_OTHER_ERR);
    }



    /*
     * Decode instrument information
     */
    if (fm_create_hdf5_string(&str, 6)) {
    	fmerrmsg(where,"Could not create str");
    	return(FM_OTHER_ERR);
    }


    attr_id = H5Aopen_name(grp,"instrument");
    if (attr_id < 0) {
    	fmerrmsg(where,"Could not open attribute instrument.");
    	return(FM_IO_ERR);
    }

    status = H5Aread(attr_id,str,h->sensor_name);

    fprintf(stdout,"sensor: %s %i\n",h->sensor_name,status);


    if (status < 0) {
    	fmerrmsg(where,"Could not read attribute instrument.");
    	return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
    	fmerrmsg(where,
    			"Could not close attribute instrument, bailing out!");
    	return(FM_IO_ERR);
    }
    if (H5Tclose(str) < 0) {
    	fmerrmsg(where,"Could not release str");
    	return(FM_OTHER_ERR);
    }


    /*
     * Decode orbit_no
     */
    attr_id = H5Aopen_name(grp,"orbit_number");
    if (attr_id < 0) {
    	fmerrmsg(where,"Could not open attribute orbit_number.");
    	return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,H5T_NATIVE_INT,&(h->orbit_no));


    if (status < 0) {
    	fmerrmsg(where,"Could not read attribute orbit_number.");
    	return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
    	fmerrmsg(where,
    			"Could not close attribute orbit_number, bailing out!");
    	return(FM_IO_ERR);
    }



    /*
     * Decode time span
     */
    attr_id = H5Aopen_name(grp,"startepochs");
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute startepochs.");
        return(FM_IO_ERR);
    }
    status = H5Aread(attr_id,H5T_NATIVE_LONG,&timespan[0]);
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute startepochs.");
        return(FM_IO_ERR);
    }
        status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute startepochs, bailing out!");
        return(FM_IO_ERR);
    }
        attr_id = H5Aopen_name(grp,"endepochs");
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute endepochs.");
        return(FM_IO_ERR);
    }
        status = H5Aread(attr_id,H5T_NATIVE_LONG,&timespan[1]);
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute endepochs.");
        return(FM_IO_ERR);
    }
        status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute endepochs, bailing out!");
        return(FM_IO_ERR);
    }

        if (tofmtime(timespan[0], &(h->timespan)[0])) {
        fmerrmsg(where,"Could decode timespan");
        return(FM_IO_ERR);
    }
            if (tofmtime(timespan[1], &(h->timespan)[1])) {
        fmerrmsg(where,"Could decode timespan");
        return(FM_IO_ERR);
    }



    return(FM_OK);
}


/*
 * Decode the groups named image within sunsatangles and AVHRR files...
 * Only the group attributes and the elements of subgroup what is decoded
 * in addition to the dataset. These elements are common between files
 * containing sunsatangles as well as avhrr data.
 *
 * the main elements to decode are attribute "description", group "what"
 * containing attributes "gain", "offset", "nodata", "missingdata",
 * "product", "quantity". elements "start" and "end" date are ignored as
 * these are read at file level before. Data are contaned in the dataset
 * named "data".
 *
 * Geographical positions for all pixels are located in the main where
 * group. In order to fit the data model, these data are added as extra
 * layers to the layers read from the image groups. To support this, the
 * layers value is increased by two and extra layers are allocated.
 */
int (fm_extractimagedata(hid_t file_id, fmdataset *d)) {
    char *where="fm_extract_imagedata";
    char *groupname,*quantity, *product;
    char *wheresets[2]={"lat","lon"};
    hid_t grp_id, grp_id2, grp_id3, attr_id;
    hid_t str, dataspace, dataset;
    H5A_info_t ainfo;
    hsize_t dsd_d[2];
    herr_t status;
    int i, j, k, *mydata;


    if (fmalloc_char_vector(&groupname,25)) {
        fmerrmsg(where,"Could not allocate groupname");
        return(FM_MEMALL_ERR);
    }
    if (fmalloc_char_vector(&quantity,25)) {
        fmerrmsg(where,"Could not allocate groupname");
        return(FM_MEMALL_ERR);
    }
    if (fmalloc_char_vector(&product,25)) {
        fmerrmsg(where,"Could not allocate groupname");
        return(FM_MEMALL_ERR);
    }


    /*
     * Allocate the number of layers (fmdatafields) to include
     * Add two extra fields for geographical positions, remember to reduce
     * layers with 2 when manipulating if not positions are requested.
     */
    d->h.layers += 2;
    if (allocate_fmdatafield(&(d->d),d->h.layers)) {
        fmerrmsg(where,"Could not allocate the necessary number of layers");
        return(FM_MEMALL_ERR);
    }


    for (i=0;i<d->h.layers-2;i++) {
        sprintf(groupname,"image%d", i+1);
        //fprintf(stdout,"%d %s\n", i+1, groupname);
        /*
         * Open image group and read attribute description
         */
        grp_id = H5Gopen(file_id,groupname);
        if (grp_id < 0) {
            fmlogmsg(where,
            "Could not find group %s", groupname);
            break;
        };

        if (fmget_hdf5_string_att(grp_id,"description", ((d->d)[i]).description)) {
            fmerrmsg(where,"Could not retrieve description");
            return(FM_IO_ERR);
        }


        /*
         * Open subgroup and read codes for no or missing data, decode
         * information etc.
         */
        grp_id2 = H5Gopen(grp_id,"what");
        if (grp_id2 < 0) {
            fmlogmsg(where,
            "Could not find group what in %s", groupname);
            break;
        };

        if (fmget_hdf5_string_att(grp_id2,"quantity", quantity)) {
            fmerrmsg(where,"Could not retrieve quantity");
            return(FM_IO_ERR);
        }
        if (strstr(quantity,"REFL")) {
            sprintf(((d->d)[i]).unit,"bidirectional reflectivity as percentage");
        } else if (strstr(quantity,"TB")) {
            sprintf(((d->d)[i]).unit,"brightness temperature in Kelvin");
        } else if (strstr(quantity,"DEG")) {
            sprintf(((d->d)[i]).unit,"degrees");
        }

        if (fmget_hdf5_string_att(grp_id2,"product", product)) {
            fmerrmsg(where,"Could not retrieve product");
            return(FM_IO_ERR);
        }

        attr_id = H5Aopen_name(grp_id2,"missingdata");
        if (attr_id < 0) {
            fmerrmsg(where,"Could not open attribute missingdata.");
            return(FM_IO_ERR);
        }
        status = H5Aread(attr_id,H5T_NATIVE_INT,&(((d->d)[i]).missingdatavalue));
        if (status < 0) {
            fmerrmsg(where,"Could not read attribute missingdata.");
            return(FM_IO_ERR);
        }
        status = H5Aclose(attr_id);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close attribute missingdata, bailing out!");
            return(FM_IO_ERR);
        }
        attr_id = H5Aopen_name(grp_id2,"nodata");
        if (attr_id < 0) {
            fmerrmsg(where,"Could not open attribute nodata.");
            return(FM_IO_ERR);
        }
        status = H5Aread(attr_id,H5T_NATIVE_INT,&(((d->d)[i]).nodatavalue));
        if (status < 0) {
            fmerrmsg(where,"Could not read attribute nodata.");
            return(FM_IO_ERR);
        }
        status = H5Aclose(attr_id);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close attribute nodata, bailing out!");
            return(FM_IO_ERR);
        }


        ((d->d)[i]).packed = FMTRUE;
        ((d->d)[i]).scalefactor.nslopes = 1;
        ((d->d)[i]).scalefactor.slope = malloc(sizeof(fmslope)*1);
        if (!((d->d)[i]).scalefactor.slope) {
            fmerrmsg(where,"Could not allocate scalefactor for image layers");
            return(FM_MEMALL_ERR);
        }
        attr_id = H5Aopen_name(grp_id2,"gain");
        if (attr_id < 0) {
            fmerrmsg(where,"Could not open attribute gain.");
            return(FM_IO_ERR);
        }
        status = H5Aread(attr_id,H5T_NATIVE_FLOAT,&((((d->d)[i]).scalefactor.slope)[0].gain));
        if (status < 0) {
            fmerrmsg(where,"Could not read attribute gain.");
            return(FM_IO_ERR);
        }
        status = H5Aclose(attr_id);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close attribute gain, bailing out!");
            return(FM_IO_ERR);
        }
        attr_id = H5Aopen_name(grp_id2,"offset");
        if (attr_id < 0) {
            fmerrmsg(where,"Could not open attribute offset.");
            return(FM_IO_ERR);
        }
        status = H5Aread(attr_id,H5T_NATIVE_FLOAT,&((((d->d)[i]).scalefactor.slope)[0].intercept));
        if (status < 0) {
            fmerrmsg(where,"Could not read attribute offset.");
            return(FM_IO_ERR);
        }
        status = H5Aclose(attr_id);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close attribute offset, bailing out!");
            return(FM_IO_ERR);
        }

        status = H5Gclose(grp_id2);
        if (status < 0) {
            fmerrmsg(where,"Could not close group WHAT");
            return(FM_IO_ERR);
        };

        /*
         * Read the actual data
         */
        dsd_d[0] = d->h.xsize;
        dsd_d[1] = d->h.ysize;
        dataspace = H5Screate_simple(2, dsd_d, NULL);
        if (dataspace < 0) {
            fmerrmsg(where,"Could not create dataspace");
            return(FM_IO_ERR);
        };

        dataset = H5Dopen(grp_id,"data");

        if (dataset < 0) {
            fmerrmsg(where,"Could not open dataset data [%d]", dataset);
            return(FM_IO_ERR);
        };

        (d->d)[i].dtype = FMINT;
        if (fmalloc_int_2d(&(((d->d)[i]).intarray),d->h.ysize,d->h.xsize)) {
            fmerrmsg(where,"Could not allocate data array");
            return(FM_MEMALL_ERR);
        }
        if (fmalloc_int_vector(&mydata, (d->h.xsize*d->h.ysize))) {
            fmerrmsg(where,"Could not allocate data array");
            return(FM_MEMALL_ERR);
        }

        status = H5Dread(dataset,
                H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                mydata);

        if (status < 0) {
            fmerrmsg(where,"Could not read data field");
            return(FM_IO_ERR);
        };

        status = H5Dclose(dataset);
        if (status < 0) {
            fmerrmsg(where,"Could not close dataset in HDF5 file");
            return(FM_IO_ERR);
        };
        status = H5Sclose(dataspace);
        if (status < 0) {
            fmerrmsg(where,"Could not close field dataspace in HDF5 file");
            return(FM_IO_ERR);
        };

        for (j=0;j<d->h.ysize; j++) {
            for (k=0;k<d->h.xsize; k++) {
                ((d->d)[i]).intarray[j][k] = mydata[fmivec(k, j, d->h.xsize)];
            }
        }

        if (fmfree_int_vector(mydata)) {
            fmerrmsg(where,"Could not free mydata");
            return(FM_MEMALL_ERR);
        }

        /*
         * Close image group
         */
        status = H5Gclose(grp_id);
        if (status < 0) {
            fmerrmsg(where,"Could not close group %s", groupname);
            return(FM_IO_ERR);
        };
    }

    /*
     * Extract the geographical positions from two datasets included in
     * the main where group read earlier. remember that these layers has
     * to be added to the already read image data.
     */
    for (i=0;i<2;i++) {
        /*
         * Open image group and read attribute description
         */
        grp_id = H5Gopen(file_id,"where");
        if (grp_id < 0) {
            fmlogmsg(where,
            "Could not find group WHERE");
            return(FM_IO_ERR);
        };
        if (strstr(wheresets[i],"lat")) {
            sprintf(((d->d)[d->h.layers-2+i]).description,"geographical latitude");
            sprintf(((d->d)[d->h.layers-2+i]).unit,"degrees north");
        } else if (strstr(wheresets[i],"lon")) {
            sprintf(((d->d)[d->h.layers-2+i]).description,"geographical longitude");
            sprintf(((d->d)[d->h.layers-2+i]).unit,"degrees east");
        }
        grp_id2 = H5Gopen(grp_id,wheresets[i]);
        if (grp_id < 0) {
            fmlogmsg(where,
            "Could not find group %s", wheresets[i]);
            return(FM_IO_ERR);
        };
        dataset = H5Dopen(grp_id2,"data");
        if (dataset < 0) {
            fmerrmsg(where,"Could not open dataset data [%d]", dataset);
            return(FM_IO_ERR);
        };
        (d->d)[d->h.layers-2+i].dtype = FMINT;
        if (fmalloc_int_2d(&(((d->d)[d->h.layers-2+i]).intarray),d->h.ysize,d->h.xsize)) {
            fmerrmsg(where,"Could not allocate data array");
            return(FM_MEMALL_ERR);
        }
        if (fmalloc_int_vector(&mydata, (d->h.xsize*d->h.ysize))) {
            fmerrmsg(where,"Could not allocate data array");
            return(FM_MEMALL_ERR);
        }
        status = H5Dread(dataset,
                H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                mydata);
        if (status < 0) {
            fmerrmsg(where,"Could not read data field");
            return(FM_IO_ERR);
        };
        status = H5Dclose(dataset);
        if (status < 0) {
            fmerrmsg(where,"Could not close dataset in HDF5 file");
            return(FM_IO_ERR);
        };

        for (j=0;j<d->h.ysize; j++) {
            for (k=0;k<d->h.xsize; k++) {
                ((d->d)[d->h.layers-2+i]).intarray[j][k] = mydata[fmivec(k, j, d->h.xsize)];
            }
        }
        if (fmfree_int_vector(mydata)) {
            fmerrmsg(where,"Could not free mydata");
            return(FM_MEMALL_ERR);
        }
        grp_id3 = H5Gopen(grp_id2,"what");
        if (grp_id3 < 0) {
            fmlogmsg(where,
            "Could not find group what in %s", groupname);
            break;
        };
        attr_id = H5Aopen_name(grp_id3,"missingdata");
        if (attr_id < 0) {
            fmerrmsg(where,"Could not open attribute missingdata.");
            return(FM_IO_ERR);
        }
        status = H5Aread(attr_id,H5T_NATIVE_INT,&(((d->d)[d->h.layers-2+i]).missingdatavalue));
        if (status < 0) {
            fmerrmsg(where,"Could not read attribute missingdata.");
            return(FM_IO_ERR);
        }
        status = H5Aclose(attr_id);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close attribute missingdata, bailing out!");
            return(FM_IO_ERR);
        }
        attr_id = H5Aopen_name(grp_id3,"nodata");
        if (attr_id < 0) {
            fmerrmsg(where,"Could not open attribute nodata.");
            return(FM_IO_ERR);
        }
        status = H5Aread(attr_id,H5T_NATIVE_INT,&(((d->d)[d->h.layers-2+i]).nodatavalue));
        if (status < 0) {
            fmerrmsg(where,"Could not read attribute nodata.");
            return(FM_IO_ERR);
        }
        status = H5Aclose(attr_id);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close attribute nodata, bailing out!");
            return(FM_IO_ERR);
        }
        ((d->d)[d->h.layers-2+i]).packed = FMTRUE;
        ((d->d)[d->h.layers-2+i]).scalefactor.nslopes = 1;
        ((d->d)[d->h.layers-2+i]).scalefactor.slope = malloc(sizeof(fmslope)*1);
        if (!((d->d)[d->h.layers-2+i]).scalefactor.slope) {
            fmerrmsg(where,"Could not allocate scalefactor for positions");
            return(FM_MEMALL_ERR);
        }
        attr_id = H5Aopen_name(grp_id3,"gain");
        if (attr_id < 0) {
            fmerrmsg(where,"Could not open attribute gain.");
            return(FM_IO_ERR);
        }
        status = H5Aread(attr_id,H5T_NATIVE_FLOAT,&((((d->d)[d->h.layers-2+i]).scalefactor.slope)[0].gain));
        if (status < 0) {
            fmerrmsg(where,"Could not read attribute gain.");
            return(FM_IO_ERR);
        }
        status = H5Aclose(attr_id);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close attribute gain, bailing out!");
            return(FM_IO_ERR);
        }
        attr_id = H5Aopen_name(grp_id3,"offset");
        if (attr_id < 0) {
            fmerrmsg(where,"Could not open attribute offset.");
            return(FM_IO_ERR);
        }
        status = H5Aread(attr_id,H5T_NATIVE_FLOAT,&((((d->d)[d->h.layers-2+i]).scalefactor.slope)[0].intercept));
        if (status < 0) {
            fmerrmsg(where,"Could not read attribute offset.");
            return(FM_IO_ERR);
        }
        status = H5Aclose(attr_id);
        if (status < 0) {
            fmerrmsg(where,
                    "Could not close attribute offset, bailing out!");
            return(FM_IO_ERR);
        }

        status = H5Gclose(grp_id3);
        if (status < 0) {
            fmerrmsg(where,"Could not close group WHAT");
            return(FM_IO_ERR);
        };
        status = H5Gclose(grp_id2);
        if (status < 0) {
            fmerrmsg(where,"Could not close group %s", wheresets[i]);
            return(FM_IO_ERR);
        };
        /*
         * Close where group
         */
        status = H5Gclose(grp_id);
        if (status < 0) {
            fmerrmsg(where,"Could not close group WHERE");
            return(FM_IO_ERR);
        };
    }

    if (fmfree_char_vector(groupname)) {
        fmerrmsg(where,"Could not free groupname");
        return(FM_MEMALL_ERR);
    }
    if (fmfree_char_vector(quantity)) {
        fmerrmsg(where,"Could not free quantity");
        return(FM_MEMALL_ERR);
    }
    if (fmfree_char_vector(product)) {
        fmerrmsg(where,"Could not free product");
        return(FM_MEMALL_ERR);
    }

    return(FM_OK);
}

int fmget_hdf5_int_att(hid_t grp_id, char *attname, int *outbuf) {
    char *where="fmget_hdf5_int_att";
    hid_t attr_id, attr_type;
    H5A_info_t ainfo;
    herr_t status;

    attr_id = H5Aopen_name(grp_id,attname);
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute %s", attname);
        return(FM_IO_ERR);
    }
    /*
    attr_type = H5Aget_type(attr_id);
    if (attr_type < 0) {
        fmerrmsg(where,"Could not get attribute type");
        return(FM_IO_ERR);
    }
    printf("Attribute type: %d\n", attr_type);
    printf("Attribute type: %d\n", H5T_NATIVE_INT);
    */
    status = H5Aread(attr_id,H5T_NATIVE_INT,outbuf);
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute %s", attname);
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute %s, bailing out!", attname);
        return(FM_IO_ERR);
    }
    return(FM_OK);
}

int fmget_hdf5_long_att(hid_t grp_id, char *attname, long *outbuf) {
    char *where="fmget_hdf5_long_att";
    hid_t attr_id, attr_type;
    H5A_info_t ainfo;
    herr_t status;

    attr_id = H5Aopen_name(grp_id,attname);
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute %s", attname);
        return(FM_IO_ERR);
    }
    /*
    attr_type = H5Aget_type(attr_id);
    if (attr_type < 0) {
        fmerrmsg(where,"Could not get attribute type");
        return(FM_IO_ERR);
    }
    printf("Attribute type: %d\n", attr_type);
    printf("Attribute type: %d\n", H5T_NATIVE_INT);
    */
    status = H5Aread(attr_id,H5T_NATIVE_LONG,outbuf);
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute %s", attname);
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute %s, bailing out!", attname);
        return(FM_IO_ERR);
    }
    return(FM_OK);
}

int fmget_hdf5_ulong_att(hid_t grp_id, char *attname, unsigned long long *outbuf) {
    char *where="fmget_hdf5_ulong_att";
    hid_t attr_id, attr_type;
    H5A_info_t ainfo;
    herr_t status;

    attr_id = H5Aopen_name(grp_id,attname);
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute %s", attname);
        return(FM_IO_ERR);
    }
    /*
    attr_type = H5Aget_type(attr_id);
    if (attr_type < 0) {
        fmerrmsg(where,"Could not get attribute type");
        return(FM_IO_ERR);
    }
    printf("Attribute type: %d\n", attr_type);
    printf("Attribute type: %d\n", H5T_NATIVE_INT);
    status = H5Aread(attr_id,H5T_NATIVE_ULONG,outbuf);
    */
    status = H5Aread(attr_id,H5T_STD_U64LE,outbuf);
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute %s", attname);
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute %s, bailing out!", attname);
        return(FM_IO_ERR);
    }
    return(FM_OK);
}

int fmget_hdf5_string_att(hid_t grp_id, char *attname, char *outbuf) {
	char *where="fmget_hdf5_string_att";
    hid_t attr_id;
    hid_t str;
    H5A_info_t ainfo;
    herr_t status;
    attr_id = H5Aopen_name(grp_id,attname);
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute %s", attname);
        return(FM_IO_ERR);
    }
    status = H5Aget_info(attr_id, &ainfo);
    if (status < 0) {
        fmerrmsg(where,"Could not get info on attribute");
        return(FM_IO_ERR);
    }

    if (fm_create_hdf5_string(&str, ainfo.data_size+1)) {
        fmerrmsg(where,"Could not create str");
        return(FM_OTHER_ERR);
    }
    status = H5Aread(attr_id,str,outbuf);
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute %s", attname);
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute %s, bailing out!", attname);
        return(FM_IO_ERR);
    }
    if (H5Tclose(str) < 0) {
        fmerrmsg(where,"Could not release str");
        return(FM_OTHER_ERR);
    }

    return(FM_OK);
}

/*
 * Should work with both cloudtype and cloudmask. CMSAF data lack
 * positioning information, but this should be extracted anyway to support
 * ordinary PPS products.
 *
 * datasets are named either cloudtype or cloudmask and contain attributes
 * with additional information (e.g. classnames). In addition the dataset
 * region should also be read as this contains localisation information
 * (not for swath files...).
 *
 * The PPS HLHDF files do not contain enough information to fill in the
 * complete structure. Furthermore, CMSAF products have insufficient
 * description of map projection, date etc and must be handled with
 * care... Øystein Godøy, METNO/FOU, 2011-12-16
 */
int fm_extractppsdata(hid_t file_id, fmdataset *d) {
    char *where="fm_extractppsdata";
    char *groupname,*quantity, *product;
    char *datasets[5]={"cloudtype","cloudmask","phase_flag","quality_flag","test_flag"};
    hid_t grp_id, grp_id2, grp_id3, attr_id;
    hid_t str, dataspace, dataset;
    H5A_info_t ainfo;
    hsize_t dsd_d[2];
    herr_t status;
    int i, j, k, l;
    unsigned char *mydata=NULL;
    unsigned short *mydatashort=NULL;
    unsigned long long valtime;
    char **classnames;



    /*
     * Read orbit number
     */
    if (fmget_hdf5_int_att(file_id,"orbit_number", &(d->h.orbit_no))) {
    	fmerrmsg(where,"Could not retrieve orbit number");
    	return(FM_IO_ERR);
    }
    printf("orbit number: %d\n", d->h.orbit_no);



    /*
     * Read description
     */
    if (fmget_hdf5_string_att(file_id,"description", (d->h).product_description)) {
    	fmerrmsg(where,"Could not retrieve description");
    	return(FM_IO_ERR);
    }

    printf("Description: %s\n",(d->h.product_description));


    /*
     * Read satellite id
     */
    if (fmget_hdf5_string_att(file_id,"satellite_id", (d->h).satellite_name)) {
        fmerrmsg(where,"Could not retrieve satellite_id");
        return(FM_IO_ERR);
    }
    sprintf(d->h.platform_name,"%s",d->h.satellite_name);
    printf("Satellite id: %s\n",(d->h.satellite_name));

    /*
     * Read time specification
     * This seems to be wrong in the CMSAF products, works fine with local
     * PPS-products...
     */
    if (fmget_hdf5_ulong_att(file_id,"sec_1970", &valtime)) {
        fmerrmsg(where,"Could not retrieve time");
        return(FM_IO_ERR);
    }
    printf("time: %lld\n", valtime);
    /*
     * Read region
     */

    if (fm_extractppsregion(file_id, &(d->h))) {
        fmerrmsg(where,"Could not read region");
        return(FM_IO_ERR);
    }


    /*
     * Read actual data and attributes
     * cloudtype, cloudmask, and phase_flag are stored as unsigned char,
     * while quality_flag and test_flag are stored as unsigned short.
     * Whether test_flag is required/useful or not is still uncertain.
     */
    k=0;
    d->h.layers = 3;
    if (allocate_fmdatafield(&(d->d),d->h.layers)) {
        fmerrmsg(where,"Could not allocate the necessary number of layers");
        return(FM_MEMALL_ERR);
    }

    for (i=0;i<5;i++) {

        fmlogmsg(where,"Checking %s",datasets[i]);
        dataset = H5Dopen(file_id,datasets[i]);
        if (dataset < 0) {
            fmlogmsg(where,"Could not find [%s] data ", datasets[i]);
            continue;
        };
        if (i<3) {
            (d->d)[k].dtype = FMUCHAR;
            if (fmalloc_ubyte_2d(&(((d->d)[k]).bytearray),d->h.ysize,d->h.xsize)) {
                fmerrmsg(where,"Could not allocate data array");
                return(FM_MEMALL_ERR);
            }
            if (fmalloc_uchar_vector(&mydata, (d->h.xsize*d->h.ysize))) {
                fmerrmsg(where,"Could not allocate data array");
                return(FM_MEMALL_ERR);
            }
            status = H5Dread(dataset,
                    H5T_NATIVE_UCHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                    mydata);
            if (status < 0) {
                fmerrmsg(where,"Could not read data field");
                return(FM_IO_ERR);
            };
            if (fm_extractppsvaltab(dataset, &(d->d)[k])) {
                fmerrmsg(where,"Could not get classnames");
                return(FM_IO_ERR);
            }
            if (fmget_hdf5_string_att(dataset,"description",((d->d)[k]).description)) {
                fmerrmsg(where,"Could not read string attribute");
                return(FM_IO_ERR);
            }


            for (j=0;j<d->h.ysize; j++) {
                for (l=0;l<d->h.xsize; l++) {
                    ((d->d)[k]).bytearray[j][l] = mydata[fmivec(l, j, d->h.xsize)];
                }
            }
            if (mydata) {
                if (fmfree_uchar_vector(mydata)) {
                    fmerrmsg(where,"Could not free mydata");
                    return(FM_MEMALL_ERR);
                }
            }
            mydata = NULL;
        } else {
            /* fill in for quality_flag and test_flag both U16BE */
            (d->d)[k].dtype = FMUSHORT;
            if (fmalloc_ushort_2d(&(((d->d)[k]).ushortarray),d->h.ysize,d->h.xsize)) {
                fmerrmsg(where,"Could not allocate data array for output structure ushort");
                return(FM_MEMALL_ERR);
            }
            if (fmalloc_ushort_vector(&mydatashort, (d->h.xsize*d->h.ysize))) {
                fmerrmsg(where,"Could not allocate data array");
                return(FM_MEMALL_ERR);
            }
            status = H5Dread(dataset,
                    H5T_NATIVE_USHORT, H5S_ALL, H5S_ALL, H5P_DEFAULT,
                    mydatashort);
            if (status < 0) {
                fmerrmsg(where,"Could not read data field");
                return(FM_IO_ERR);
            };
            if (fm_extractppsvaltab(dataset, &(d->d)[k])) {
                fmerrmsg(where,"Could not get classnames");
                return(FM_IO_ERR);
            }
            if (fmget_hdf5_string_att(dataset,"description",((d->d)[k]).description)) {
                fmerrmsg(where,"Could not read string attribute");
                return(FM_IO_ERR);
            }
            for (j=0; j<d->h.ysize; j++) {
                for (l=0; l<d->h.xsize; l++) {
                    ((d->d)[k]).ushortarray[j][l] = mydatashort[fmivec(l, j, d->h.xsize)];
                }
            }
            if (mydatashort) {
                if (fmfree_ushort_vector(mydatashort)) {
                    fmerrmsg(where,"Could not free mydata");
                    return(FM_MEMALL_ERR);
                }
            }
            mydatashort = NULL;
        }
        printf("Layer: %s have been read\n", d->d[k].description);
        status = H5Dclose(dataset);
        if (status < 0) {
            fmerrmsg(where,"Could not close dataset in HDF5 file");
            return(FM_IO_ERR);
        };
        k++;
        printf("Number of layers read: %d\n", k);

    }

    return(FM_OK);
}

/*
 * Currently this function is used to extract descriptions for both
 * quality_flag and test_flag. This should work as it adapts to the size
 * of strings in file.
 */
int fm_extractppsvaltab(hid_t dset_id, fmdatafield *d) {
    char *where="fm_extractppsvaltab";
    hid_t dataset, datatype, arraytype;
    hid_t attr_id, arrayid, dataspaceid;
    hsize_t arraydim[] = {4};
    hsize_t dims[4], maxdims[4];
    int ndims, i;
    herr_t status;
    char *attname="output_value_namelist";
    H5A_info_t ainfo;
    char *myclassnames, *cp;
    size_t strsize;

    attr_id = H5Aopen_name(dset_id,attname);
    if (attr_id < 0) {
        fmerrmsg(where,"Could not open attribute %s", attname);
        return(FM_IO_ERR);
    }
    dataspaceid = H5Aget_space(attr_id);
    if (dataspaceid< 0) {
        fmerrmsg(where,"Could not get dataspace for attribute");
        return(FM_IO_ERR);
    }
    ndims = H5Sget_simple_extent_dims(dataspaceid, dims, maxdims);
    if (ndims < 0) {
        fmerrmsg(where,"Could not get dimensions");
        return(FM_IO_ERR);
    }
    datatype = H5Aget_type(attr_id);
    if (datatype < 0) {
        fmerrmsg(where,"Could not get datatype for attribute");
        return(FM_IO_ERR);
    }
    strsize = H5Tget_size(datatype);
    if (strsize < 0) {
        fmerrmsg(where,"Could not get the size of datatype");
        return(FM_IO_ERR);
    }
    if (fmalloc_char_vector(&myclassnames, strsize*dims[0])) {
        fmerrmsg(where,"Could not allocate buffer for classnames");
        return(FM_IO_ERR);
    }
    if (fmalloc_byte_2d(&(d->class_names), dims[0], strsize)) {
        fmerrmsg(where,"Could not allocate buffer for class_names");
        return(FM_IO_ERR);
    }
    d->number_of_classes = dims[0];
    d->palette = FMTRUE;
    status = H5Aread(attr_id, datatype, myclassnames);
    if (status < 0) {
        fmerrmsg(where,"Could not read attribute %s", attname);
        return(FM_IO_ERR);
    }
    cp = &myclassnames[0];
    for (i=0;i<d->number_of_classes-1;i++) {
        sprintf(d->class_names[i],"%s",cp);
        cp += strsize;
    }
    status = H5Sclose(dataspaceid);
    if (status < 0) {
        fmerrmsg(where, "Could not close dataspace");
        return(FM_IO_ERR);
    }
    status = H5Tclose(datatype);
    if (status < 0) {
        fmerrmsg(where, "Could not close datatype");
        return(FM_IO_ERR);
    }
    status = H5Aclose(attr_id);
    if (status < 0) {
        fmerrmsg(where,
                "Could not close attribute %s, bailing out!", attname);
        return(FM_IO_ERR);
    }
    if (fmfree_char_vector(myclassnames)) {
        fmerrmsg(where,"Could not free myclassnames");
        return(FM_MEMFREE_ERR);
    }

    return(FM_OK);
}

int fm_extractppsregion(hid_t file_id, fmheader *h) {
    char *where="fm_extractppsregion";
    hid_t str64, str128, dataset, datatype, arraytype;
    hid_t arrayid;
    hsize_t arraydim[] = {4};
    herr_t status;
    ppsregion mydata;


    /*
     * Create compound data type
     */
    if (fm_create_hdf5_string(&str64, 64)) {
        fmerrmsg(where,"Could not generate str64");
        return(FM_OTHER_ERR);
    }
    if (fm_create_hdf5_string(&str128, 128)) {
        fmerrmsg(where,"Could not generate str128");
        return(FM_OTHER_ERR);
    }
    datatype = H5Tcreate(H5T_COMPOUND, sizeof(ppsregion));
    if (datatype < 0) {
        fmerrmsg(where,"Could not define compound datatype for header");
        return(FM_OTHER_ERR);;
    };
    arrayid = H5Tarray_create(H5T_NATIVE_DOUBLE, 1, arraydim, NULL);
    if (arrayid < 0) {
        fmerrmsg(where,"Could not create arrayid");
        return(FM_OTHER_ERR);
    }
    status = H5Tinsert(datatype, "area_extent",
            HOFFSET(ppsregion, area_extent), arrayid);
    if (status < 0) {
        fmerrmsg(where,"Could not insert xsize in compound datatype");
        return(FM_OTHER_ERR);;
    }
    status = H5Tinsert(datatype, "xsize",
            HOFFSET(ppsregion, xsize), H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,"Could not insert xsize in compound datatype");
        return(FM_OTHER_ERR);;
    }
    status = H5Tinsert(datatype, "ysize",
            HOFFSET(ppsregion, ysize), H5T_NATIVE_INT);
    if (status < 0) {
        fmerrmsg(where,"Could not insert ysize in compound datatype");
        return(FM_OTHER_ERR);;
    }
    status = H5Tinsert(datatype, "xscale",
            HOFFSET(ppsregion, xscale), H5T_NATIVE_FLOAT);
    if (status < 0) {
        fmerrmsg(where,"Could not insert xscale in compound datatype");
        return(FM_OTHER_ERR);;
    }
    status = H5Tinsert(datatype, "yscale",
            HOFFSET(ppsregion, yscale), H5T_NATIVE_FLOAT);
    if (status < 0) {
        fmerrmsg(where,"Could not insert yscale in compound datatype");
        return(FM_OTHER_ERR);;
    }
    status = H5Tinsert(datatype, "lat_0",
            HOFFSET(ppsregion, lat_0), H5T_NATIVE_FLOAT);
    if (status < 0) {
        fmerrmsg(where,"Could not insert lat_0 in compound datatype");
        return(FM_OTHER_ERR);;
    }
    status = H5Tinsert(datatype, "lon_0",
            HOFFSET(ppsregion, lon_0), H5T_NATIVE_FLOAT);
    if (status < 0) {
        fmerrmsg(where,"Could not insert lon_0 in compound datatype");
        return(FM_OTHER_ERR);;
    }
    status = H5Tinsert(datatype, "lat_ts",
            HOFFSET(ppsregion, lat_ts), H5T_NATIVE_FLOAT);
    if (status < 0) {
        fmerrmsg(where,"Could not insert lat_ts in compound datatype");
        return(FM_OTHER_ERR);;
    }
    status = H5Tinsert(datatype, "id",
            HOFFSET(ppsregion, id), str64);
    if (status < 0) {
        fmerrmsg(where,"Could not insert id in compound datatype");
        return(FM_OTHER_ERR);;
    }
    status = H5Tinsert(datatype, "name",
            HOFFSET(ppsregion, name), str64);
    if (status < 0) {
        fmerrmsg(where,"Could not insert name in compound datatype");
        return(FM_OTHER_ERR);;
    }
    status = H5Tinsert(datatype, "pcs_id",
            HOFFSET(ppsregion, pcs_id), str64);
    if (status < 0) {
        fmerrmsg(where,"Could not insert pcs_id in compound datatype");
        return(FM_OTHER_ERR);;
    }
    status = H5Tinsert(datatype, "pcs_def",
            HOFFSET(ppsregion, pcs_def), str128);
    if (status < 0) {
        fmerrmsg(where,"Could not insert pcs_def in compound datatype");
        return(FM_OTHER_ERR);;
    }


    /*
     * Read dataset
     */

    fmlogmsg(where,"Reading region");

    dataset = H5Dopen(file_id,"region");
    if (dataset < 0) {
        fmerrmsg(where,"Could not open dataset region");
        return(FM_IO_ERR);
    };

    status = H5Dread(dataset, datatype,
            H5S_ALL, H5S_ALL, H5P_DEFAULT,
            &mydata);
    if (status < 0) {
        fmerrmsg(where,"Could not read data field [%d]", status);
        return(FM_IO_ERR);
    };

    status = H5Dclose(dataset);
    if (status < 0) {
        fmerrmsg(where,"Could not close dataset in HDF5 file");
        return(FM_IO_ERR);
    };

    if (H5Tclose(datatype)<0) {
        fmerrmsg(where,"Could not release datatype.");
        return(FM_OTHER_ERR);
    }
    if (H5Tclose(str64)<0) {
        fmerrmsg(where,"Could not release str64.");
        return(FM_OTHER_ERR);
    }
    if (H5Tclose(str128)<0) {
        fmerrmsg(where,"Could not release str128.");
        return(FM_OTHER_ERR);
    }

    printf("Region dataset:\n");
    printf("\txsize: %d\n", mydata.xsize);
    printf("\tysize: %d\n", mydata.ysize);
//    printf("\txscale: %f\n", mydata.xscale);
//    printf("\tyscale: %f\n", mydata.yscale);
//    printf("\tid: %s\n", mydata.id);
//    printf("\tname: %s\n", mydata.name);
//    printf("\tpcs_id: %s\n", mydata.pcs_id);
//    printf("\tpcs_def: %s\n", mydata.pcs_def);


    h->xsize = mydata.xsize;
    h->ysize = mydata.ysize;


    return(FM_OK);
}

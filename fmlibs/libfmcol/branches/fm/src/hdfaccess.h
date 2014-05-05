/*
 * PURPOSE:
 * Header file for the HDF5 software used to store collocated stattionwise
 * data.
 *
 * NOTE:
 * See hdfaccess.c for details.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 27/07/2000
 * Øystein Godøy, DNMI/FOU, 19/09/2002
 * Modularized HDF access, and added support for SAFCM...
 * Øystein Godøy, DNMI/FOU, 29.04.2003
 * Added storage without STDAT
 * Øystein Godøy, MI/FOU, 06.11.2003
 * Due to errors when extracting safcm_data data, a dummy interface is
 * created to circumvent the problem in the current situation. This
 * involves the use of a slimmer data structure not containing the
 * character strings describing the class names. Functions affected are
 * the high level read* functions accessing the safcm_data structure. In
 * this fix, data are transferred from safcm_data to safcm_data2.
 */

#ifndef _HDFACCESS_H
#define _HDFACCESS_H

/*
 * Header files that are required.
 */
#include <hdf5.h>
#include <satimg.h>
#include <avhrr_stdat.h>
#include <safcm_stdat.h>
#include <safssi_stdat.h>
#include <nwp_stdat.h>
#include <std_stdat.h>

#define MAXGROUPS 10000
#define MAXDATASETS 10
/*
 * Variable declarations...
 */
typedef struct {
    char classname[NAMELEN];
    char station[NAMELEN];
    time_t t_start;
    time_t t_end;
} skeys;

typedef struct {
    int noobs;
    skeys *scrit;
    char (*group)[NAMELEN];
} cinfo;

/*
 * Top level functions to use in main program
 * The top level functions are named according to a numbering of data
 * sources (see hdfaccess.c for correct list at any time).
 * 	AVHRR	- 1 - AVHRR data as given by MEOS
 * 	NWP	- 2 - HIRLAM NWP data
 * 	STDAT	- 3 - SYNOP or DRAU data
 * 	SAFCM	- 4 - SAFNWC cloud type product
 * 	SAFSSI	- 5 - SAFOSI SSI product
 */
int dump123(char *filename, char *class, 
	as_data aobs, ns_data nobs, obsstruct sobs);
int dump1234(char *filename, char *class, 
	as_data aobs, ns_data nobs, obsstruct sobs, safcm_data cobs);
int dump12345(char *filename, char *class, 
	as_data aobs, ns_data nobs, obsstruct sobs, 
	safcm_data cobs, safssi_data ssi);
int dump1245(char *filename, char *class, 
	as_data aobs, ns_data nobs,
	safcm_data cobs, safssi_data ssi);
int dump34(char *filename, char *class, 
	obsstruct sobs, safcm_data cobs);
int read12345(char *filename, skeys *scrit, 
	as_data **aobs, ns_data **nobs, obsstruct **sobs, 
	safcm_data2 **cobs, safssi_data **iobs);
int read1245(char *filename, skeys *scrit, 
	as_data **aobs, ns_data **nobs,
	safcm_data2 **cmobs, safssi_data **iobs);
int read1234(char *filename, skeys *scrit, 
	as_data **aobs, ns_data **nobs, obsstruct **sobs,
	safcm_data2 **cobs);
int read123(char *filename, skeys *scrit, 
	as_data **aobs, ns_data **nobs, obsstruct **sobs);
int read34(char *filename, skeys *scrit, obsstruct **sobs, safcm_data2 **cobs);
int checknorec(char *filename, skeys *scrit);

/*
 * Functions below are only used by the top level functions, not in the
 * main program.
 * Data dump to file
 */
hid_t openFile(char *filename);
herr_t read_items(hid_t loc_id, const char *name, void *opdata); 
herr_t count_basegroups(hid_t loc_id, const char *name, void *opdata); 
herr_t count_datasets(hid_t loc_id, const char *name, void *opdata); 
herr_t dumpAVHRR2HDF(hid_t file, hid_t grp, as_data aobs);
herr_t dumpNWP2HDF(hid_t file, hid_t grp, ns_data nobs);
herr_t dumpSTDAT2HDF(hid_t file, hid_t grp, obsstruct sobs);
herr_t dumpSAFCM2HDF(hid_t file, hid_t grp, safcm_data cmobs);
herr_t dumpSAFSSI2HDF(hid_t file, hid_t grp, safssi_data ssiobs);
herr_t readSTDAT(hid_t file, hid_t grp, obsstruct *obs);
herr_t readAVHRR(hid_t file, hid_t grp, as_data *obs);
herr_t readNWP(hid_t file, hid_t grp, ns_data *obs);
/*herr_t readSAFCM(hid_t file, hid_t grp, safcm_data *obs);*/
herr_t readSAFCM(hid_t file, hid_t grp, safcm_data2 *obs);
herr_t readSAFCMRAW(hid_t file, hid_t grp, safcm_data2 *obs);
herr_t readSAFSSI(hid_t file, hid_t grp, safssi_data *obs);

/*
 * Compound datatypes required
 */
hid_t compoundAVHRR(int nopix);
hid_t compoundNWP(int nopar, int nopix);
hid_t compoundSTDAT(void);
hid_t compoundSAFCM(int nopix);
hid_t compoundSAFCMRAW(int nopix);
hid_t compoundSAFSSI(int nopix);
hid_t compoundUCS(void);
hid_t compoundIMGA(void);
hid_t compoundLATLON(void);

/*
 * End function prototypes.
 */

#endif /* _HDFACCESS_H */

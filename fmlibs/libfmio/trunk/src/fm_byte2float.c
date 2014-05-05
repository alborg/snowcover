/*
 * NAME:
 * fm_byte2float
 *
 * PURPOSE:
 * To convert byte values to float values for satellite data.
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * value: an unsigned short integer value containing the packed data
 * byte2float: a structure containing slope and intercept values for
 * visible end thermal infrared channels.
 * keyword: a word telling whether this channel should be handled as a
 * visible or infrared channel. The keyword should be Temperature or
 * reflectance at present.
 *
 * OUTPUT:
 * NA
 *
 * NOTES:
 * Modification of calib within libsatimg.
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, METNO/FOU, 26.04.2007 
 *
 * MODIFIED:
 * NA
 *
 * ID:
 * $Id$
 */
#include <fmio.h>

float fm_byte2float(unsigned short value, fmscale byte2float, char *keyword) {

    char *where="fm_byte2float";
    char what[FMSTRING512];
    int i, match;
    float unpacked;

    /*
     * Check that fmscale input is as expected
     */
    if (byte2float.nslopes != 2) {
	fmerrmsg(where,"Wrong nslopes value within fmscale input");
	return(FM_SYNTAX_ERR);
    }

    unpacked = 0;
    match = 0;

    /*
     * Unpack
     */
    for (i=0;i<byte2float.nslopes;i++) {
	/*
	printf(" %s - %s\n",byte2float.slope[i].description,keyword);
	*/
	if (strstr(byte2float.slope[i].description,keyword)) {
	    unpacked = byte2float.slope[i].intercept+(byte2float.slope[i].gain*value);
	    match = 1;
	} 
    }

    /*
     * Check that it worked
     */
    if (! match) {
	sprintf(what,"No channel match in fmscale input, check input...");
	fmerrmsg(where,what);
	return(FM_IO_ERR);
    }
    
    return(unpacked);
}

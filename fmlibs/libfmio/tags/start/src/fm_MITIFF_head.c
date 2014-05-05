/*
 * PURPOSE:
 * To define the ASCII textstring that is used to carry header information in
 * the DNMI/TIFF image files. This text string is a specially formatted string
 * that is appended to the image through the TIFF tag IMAGEDESCRIPTION. The
 * first line of the string contains the keyword <Satellite:>.
 *
 * NOTES:
 * The textstring which is defined by this function is decoded into data
 * structures by functions MITIFF_read and MITIFF_read_imagepal.
 * The calibration information may be used to create classnames for
 * Palette-color images. A function that creates this part autmoatically is
 * however not defined yet.
 *
 * AUTHOR:
 * �ystein God�y, DNMI/TIFF, 01/08/95.
 * MODIFIED:
 * �ystein God�y, met.no/TIFF, 23.02.2004
 * Some cleaning of unused variables.
 * �ystein God�y, met.no/TIFF, 04.03.2004
 * Code adapted for use with MODIS imagery after requirement issued by
 * Vibeke W. Thyness.
 * �ystein God�y, METNO/FOU, 16.10.2006
 * Modified name space for libfmio.
 */

#include <fmio.h>

void fm_MITIFF_create_head(char imginfo[FMIO_TIFFHEAD], char satellite[16], char time[17], 
	short satdir, short zsize, char chdesc[20], int xsize, int ysize, 
	char projection[30], char truelat[10], float rot, int xunit, int yunit, 
	float npx, float npy, float ax, float ay, float bx, float by, 
	char *calib) {

    char next[256];

    sprintf(imginfo,"%12s%-12s\n"," Satellite: ",satellite);
    sprintf(next,"%16s%-17s\n", " Date and Time: ", time);
    strcat(imginfo, next);
    sprintf(next,"%9s%d\n"," SatDir: ", satdir);    
    strcat(imginfo, next);
    sprintf(next,"%10s%d"," Channels: ",zsize);    
    strcat(imginfo, next);
    sprintf(next,"%15s%-20s\n"," In this file: ", chdesc);
    strcat(imginfo, next);
    sprintf(next,"%8s%d%8s%d\n"," Xsize: ",xsize," Ysize: ",ysize);
    strcat(imginfo, next);

    /*
     * Write information for map projection and
     * combination with NWP.
     */
    sprintf(next,"%17s%-30s\n"," Map projection: ", projection);
    strcat(imginfo, next);    
    sprintf(next,"%s%s%s%.3f\n", " TrueLat: ", truelat, " GridRot: ",rot);
    strcat(imginfo, next);    
    sprintf(next,"%s%d%s%d%s\n"," Xunit: ", xunit, " m Yunit: ", yunit, " m"); 
    strcat(imginfo, next);  
    sprintf(next,"%s%-.6f%s%-.6f\n"," NPX: ", npx, " NPY: ", npy); 
    strcat(imginfo, next);  

    /*
     * Write UCS information
     */ 
    sprintf(next,"%s%-.6f"," Ax: ",ax);
    strcat(imginfo, next);    
    sprintf(next,"%s%-.6f"," Ay: ",ay);
    strcat(imginfo, next);    
    sprintf(next,"%s%-.6f"," Bx: ",bx);
    strcat(imginfo, next);    
    sprintf(next,"%s%-.6f\n"," By: ",by);
    strcat(imginfo, next);    

    /*
     * Write calibration information
     */
    /* while testing...
    strcat(imginfo, calib);
    */
}

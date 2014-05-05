/*
 * NAME:
 * fm_MITIFF_write
 * fm_MITIFF_write_rgb
 * 
 * PURPOSE:
 * Writes image data on TIFF formatted file, ready for visualization
 * on any standard image viewer. TIFF tag number 262 is photometric
 * interpretation. This tag is 1 for grayscale images and 3 for
 * palette images. In situations when grayscale images are wanted
 * TIFF tag number 320 can be added. In present version only single
 * page grayscale images are supported.
 *
 * REQUIRES:
 * Calls to other libraries:
 *	This routine require libtiff version 3.0.
 * 
 * NOTES:
 * Return values and error handling is not properly implemented yet.
 *
 * AUTHOR: 
 * Oystein Godoy, DNMI, 1/8/95.
 *
 * MODIFIED:
 * Øystein Godøy, met.no/FOU, 23.02.2004
 * Cleaning of code, removed some parts not used...
 * Øystein Godøy, METNO/FOU, 16.10.2006
 * Modified name space for libfmio.
 *
 * ID:
 * $Id$
 */
 
#include <fmio.h>

#ifdef FMIO_HAVE_LIBTIFF
#include <tiffio.h>

int fm_MITIFF_write(char *outfile, unsigned char *image, char newhead[], 
    fmio_mihead ginfo) {

    char *where="fm_MITIFF_write";
    int ret, size;
    TIFF *out;
 
    /*
    * Processed image information are dumped on TIFF file using libtiff.
    */
    out = TIFFOpen(outfile, "wc");
    if (!out) {
	fmerrmsg(where,"Couldn't open TIFF file for writing...");
	return(FM_IO_ERR);
    }
    ret = TIFFSetField(out, 256, ginfo.xsize);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 257, ginfo.ysize);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 258, 8);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 259, 1);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 262, 1);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 270, newhead);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret=TIFFSetField(out, 274, 1);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret=TIFFSetField(out, 277, 1);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret=TIFFSetField(out, 278, ginfo.ysize);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 284, 1);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");

    size=ginfo.xsize*ginfo.ysize;

    ret = TIFFWriteRawStrip(out, 0, image, size); 
    if (ret != size) {
	fmerrmsg(where,"Error in TIFFWriteRawStrip.");
	return(FM_IO_ERR);
    }

    TIFFClose(out);
    return(FM_OK);
}

/*
 * PURPOSE:
 * To write TIFF 6.0 files in RGB mode with DNMI/TIFF header if required.
 * Otherwise this function is similar to the version that writes ordinary
 * grayscale images MITIFF_write.
 *
 * NOTES:
 * There is at present no function in the library that reads DNMI/TIFF files
 * with the special header. The present use of these files are for presentation
 * on the web and/or in standard image processing software like xv, Photoshop
 * etc.
 */
 
int fm_MITIFF_write_rgb(char *outfile, unsigned char *image[], char newhead[], 
    fmio_mihead ginfo) {

    char *where="MITIFF_write_rgb";
    int ret, i, size, elem;
    unsigned char *obuf;
    unsigned int strip = 0;
    TIFF *out;
 
    /*
     * Processed image information are dumped on TIFF file using libtiff.
     */

    out=TIFFOpen(outfile, "wc");
    if (!out) {
	fmerrmsg(where,"Couldn't open TIFF file for writing...");
	return(FM_IO_ERR);
    }
    ret=TIFFSetField(out, 256, ginfo.xsize);
    ret=TIFFSetField(out, 257, ginfo.ysize);
    ret=TIFFSetField(out, 258, 8);
    ret=TIFFSetField(out, 259, 1);
    ret=TIFFSetField(out, 262, 2);
    ret=TIFFSetField(out, 270, newhead);
    ret=TIFFSetField(out, 274, 1);
    ret=TIFFSetField(out, 277, 3);
    ret=TIFFSetField(out, 278, ginfo.ysize);
    ret=TIFFSetField(out, 284, 1);
    ret=TIFFSetField(out, 315, "Copyright, DNMI. All rights reserved");

    obuf = (unsigned char *) malloc(TIFFStripSize(out));
    if (!obuf) {
	fmerrmsg(where,"Memory problem in MITIFF_write, returns to main...");
	return(FM_OK);     
    }
 
    size = ginfo.xsize * ginfo.ysize;
    elem=0;
    for (i=0; i<size; i++) {
	 obuf[elem++] = *image[0]++;
         obuf[elem++] = *image[1]++;
         obuf[elem++] = *image[2]++;
    }

    TIFFWriteRawStrip(out, strip, obuf, 3*size);

    TIFFClose(out);
    return(FM_OK);
}

/*
 * PURPOSE:
 * Writes image data on TIFF formatted file, ready for visualization
 * on any standard image viewer. TIFF tag number 262 is photometric
 * interpretation. This tag is 1 for grayscale images and 3 for
 * palette images. In situations when grayscale images are wanted
 * TIFF tag number 320 can be commented out.
 * 
 * REQUIRES:
 * Calls to other libraries: This routine use libtiff version 3.0 to write 
 * TIFF colormap images.
 */
int fm_MITIFF_write_imagepal(char *outfile, unsigned char *class, 
    char newhead[], fmio_mihead ginfo, unsigned short cmap[3][256]) {
    
    char *where="MITIFF_write_imagepal";
    int ret,size;
    unsigned short bps=8, pmi=3;
    TIFF *out;

    /*
     * Processed image information are dumped on TIFF file using libtiff.
     */
    out = TIFFOpen(outfile, "wc");
    if (!out) {
	fmerrmsg(where,"Couldn't open TIFF file for writing...");
	return(FM_IO_ERR);
    }
  
    ret = TIFFSetField(out, 256, ginfo.xsize);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 257, ginfo.ysize);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 258, bps);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 259, COMPRESSION_NONE);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 262, pmi);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 270, newhead);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 284, 1);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");
    ret = TIFFSetField(out, 320, cmap[0], cmap[1], cmap[2]);
    if (ret != 1) fmerrmsg(where,"Error in TIFFSetField");

    size = ginfo.xsize*ginfo.ysize;
    ret = TIFFWriteRawStrip(out, 0, class, size); 

    TIFFClose(out);
    return(FM_OK);
}

/*
 * Writes multilevel images in grayscale
 */
int fm_MITIFF_write_multi(char *outfile, unsigned char *image[], 
    char newhead[], fmio_mihead ginfo) {

    char *where="MITIFF_write_multi";
    int ret, i, size;
    TIFF *out;

    /*
     * Processed image information are dumped on TIFF file using libtiff.
     */
    out = TIFFOpen(outfile, "wc");
    if (!out) {
	fmerrmsg(where,"Couldn't open TIFF file for writing...");
	return(FM_IO_ERR);
    }
    for (i=0; i<ginfo.zsize; i++) {
	ret = TIFFSetField(out, 256, ginfo.xsize);
	if (ret != 1) fmerrmsg(where," Error in TIFFSetField");
	ret = TIFFSetField(out, 257, ginfo.ysize);
	if (ret != 1) fmerrmsg(where," Error in TIFFSetField");
	ret = TIFFSetField(out, 258, 8);
	if (ret != 1) fmerrmsg(where," Error in TIFFSetField");
	ret = TIFFSetField(out, 259, 1);
	if (ret != 1) fmerrmsg(where," Error in TIFFSetField");
	ret = TIFFSetField(out, 262, 1);
	if (ret != 1) fmerrmsg(where," Error in TIFFSetField");
	if (i == 0) {
	    ret = TIFFSetField(out, 270, newhead);
	    if (ret != 1) fmerrmsg(where," Error in TIFFSetField");
	}
	ret=TIFFSetField(out, 274, 1);
	if (ret != 1) fmerrmsg(where," Error in TIFFSetField");
	ret=TIFFSetField(out, 277, 1);
	if (ret != 1) fmerrmsg(where," Error in TIFFSetField");
	ret=TIFFSetField(out, 278, ginfo.ysize);
	if (ret != 1) fmerrmsg(where," Error in TIFFSetField");
	ret = TIFFSetField(out, 284, 1);
	if (ret != 1) fmerrmsg(where," Error in TIFFSetField");

	size=ginfo.xsize*ginfo.ysize;

	ret = TIFFWriteRawStrip(out, 0, image[i], size); 
	if (ret != size) {
	    fmerrmsg(where," Error in TIFFWriteRawStrip");
	    fprintf(stderr, "Size: %d\n Ret: %d\n", size, ret);
	    return(FM_IO_ERR);
	}
	ret = TIFFWriteDirectory(out); 
	if (ret == 0) {
	    fmerrmsg(where," Error in TIFFWriteDirectory");
	    return(FM_IO_ERR);
	}
    }

    TIFFClose(out);
    return(FM_OK);
}

#endif

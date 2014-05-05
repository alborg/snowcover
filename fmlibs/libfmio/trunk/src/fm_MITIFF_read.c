/*
 * NAME:
 * NA
 *
 * PURPOSE:
 * Read and decode TIFF image files with satellite data on the
 * multichannel format.
 *
 * REQUIREMENTS:
 * Calls to other libraries:
 * The routine use the libtiff version 3.0 to read TIFF files.
 *
 * INPUT:
 * NA
 *
 * OUTPUT:
 * NA
 *
 * RETURN VALUES:
 * 0 - Normal and correct ending
 * 2 - This is a Palette-color image
 * negative value : an error occured
 *
 * NOTES:
 * At present only single strip images are supported.
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 05/05/1995
 *
 * MODIFIED:
 * Øystein Godøy, DNMI/FOU, 27/03/2001: Corrected some memory allocation
 * and freeing problems connected to strtok actually changing the string
 * it operates on.
 * Øystein Godøy, met.no/FOU, 23.03.2004: Changed satellite name check to
 * avoid software stopping when processing unknown satellites. Only first
 * image of multiimage files is presented.
 * Øystein Godøy, METNO/FOU, 16.10.2006: Modified name space for libfmio.
 * Øystein Godøy, METNO/FOU, 22.02.2008: Some bugfixing reading headers.
 * Øystein Godøy, METNO/FOU, 16.10.2008: Changes in reading headers.
 *
 * ID:
 * $Id$
 */

#include <fmio.h>
#ifdef FMIO_HAVE_LIBTIFF
#include <tiffio.h>

int fm_MITIFF_read(char *infile, unsigned char *image[], 
    fmio_mihead *ginfo) {
    
    char *where="MITIFF_read";
    TIFF *in;
    int i, status, size;
    short pmi;
    unsigned int fieldlen, currlen, nextlen, taglen;
    char *description, *o_description;
    char *currfield, *nextfield, *field, *pt;
    char *o_currfield, *o_nextfield, *o_field;
    char *fieldname[FMIO_FIELDS]={
	"Satellite:", 
	"Date and Time:", 
	"SatDir:", 
	"Channels:", 
	"In this file:", 
	"Xsize:", 
	"Ysize:", 
	"Map projection:", 
	"TrueLat:", 
	"GridRot:", 
	"Xunit:", 
	"Yunit:", 
	"NPX:", 
	"NPY:", 
	"Ax:", 
	"Ay:", 
	"Bx:", 
	"By:", 
	"Calibration"
    };

    /*
     * Open TIFF files and initialize IFD
     */
    
    in=TIFFOpen(infile, "rc");
    if (!in) {
	printf(" This is no TIFF file! \n");
	return(FM_IO_ERR);
    }

    /*
     * Test whether this is a color palette image or not. If so another
     * function should be used.
     */
    status = TIFFGetField(in, 262, &pmi);
    if (pmi == 3) {
	return(FM_IO_ERR);
    }

    description = (char *) malloc(1024*sizeof(char));
    if (!description) fmerrmsg(where,"Memory allocation failed");
    o_description = description;
    TIFFGetField(in, 270, &description);
    currfield = (char *) malloc(FMIO_TIFFHEAD*sizeof(char));
    if (!currfield) fmerrmsg(where,"Memory allocation failed"); 
    o_currfield = currfield;
    nextfield = (char *) malloc(FMIO_TIFFHEAD*sizeof(char));
    if (!nextfield) fmerrmsg(where,"Memory allocation failed"); 
    o_nextfield = nextfield;
    for (i=0; i<FMIO_FIELDS-1; i++) {
	pt = strstr(description, fieldname[i]);
	sprintf(currfield, "%s", pt);
	currlen = strlen(currfield);
	pt = strstr(description, fieldname[i+1]);
	sprintf(nextfield, "%s", pt);
	nextlen = strlen(nextfield);
	taglen = strlen(fieldname[i]);
	fieldlen = currlen-nextlen-taglen;
	field = (char *) calloc(fieldlen+1, sizeof(char));
	if (!field) fmerrmsg(where,"Memory allocation failed");
	o_field = field;
	currfield += taglen;
	strncpy(field, currfield, fieldlen);
	if (fm_MITIFF_fillhead(field, fieldname[i], ginfo)) {
	    return(FM_IO_ERR);
	}
	free(o_field);
    }
    free(o_currfield);
    free(o_nextfield);
    free(o_description); 
    
    /*
     * Read image data into matrix.
     */
    TIFFGetField(in, 256, &ginfo->xsize);
    TIFFGetField(in, 257, &ginfo->ysize);
    size = ginfo->xsize*ginfo->ysize;
  
    /*
     * Memory allocated for image data in this function (*image) is freed 
     * in function main process.
     */
    if (ginfo->zsize > FMIO_MAXCHANNELS) {
	printf("\n\tNOT ENOUGH POINTERS AVAILABLE TO HOLD DATA!\n");
	return(FM_IO_ERR);
    }
    for (i=0; i<ginfo->zsize; i++) {
	image[i] = (unsigned char *) malloc((size+1)*sizeof(char));
	if (!image[i]) fmerrmsg(where,"Memory allocation failed");
	status = TIFFReadRawStrip(in, 0, image[i], size);
	if (status == -1) return(FM_IO_ERR);
	if (TIFFReadDirectory(in) == 0) break;
    }

    if (ginfo->zsize != (i+1)) {
	printf("\n\tERROR READING MULTIPLE SUBFILES!\n");
	return(FM_IO_ERR);
    }
    
    TIFFClose(in);
    return(FM_OK);
}

/*
 * PURPOSE:
 * To read DNMI/TIFF palette color files containing either classed satellite
 * imagery or radar imagery.
 *
 * RETURN VALUES:
 * 0 - Normal and correct ending
 * 2 - This is not a Palette-color image
 *
 * NOTE:
 * Requires access to libtiff.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/07/1999
 * MODIFICATION:
 * Øystein Godøy, DNMI/FOU, 27/03/2001
 * Corrected some memory allocation and freeing problems connected to
 * strtok actually changing the string it operates on.
 */
int fm_MITIFF_read_imagepal(char *infile, unsigned char *image[], 
    fmio_mihead *ginfo, fmio_mihead_pal *palinfo) {
    
    char *where="MITIFF_read_imagepal";
    TIFF *in;
    int i, status, size;
    short pmi;
    unsigned int fieldlen, currlen, nextlen, taglen;
    uint16 *red, *green, *blue;
    char *description, *o_description;
    char *currfield, *nextfield, *field, *pt;
    char *o_currfield, *o_nextfield, *o_field;
    char *fieldname[FMIO_FIELDS]={
	"Satellite:", 
	"Date and Time:", 
	"SatDir:", 
	"Channels:", 
	"In this file:", 
	"Xsize:", 
	"Ysize:", 
	"Map projection:", 
	"TrueLat:", 
	"GridRot:", 
	"Xunit:", 
	"Yunit:", 
	"NPX:", 
	"NPY:", 
	"Ax:", 
	"Ay:", 
	"Bx:", 
	"By:", 
	"COLOR INFO:"
    };

    /*
     * Open TIFF files and initialize IFD
     */
    
    in=TIFFOpen(infile, "rc");
    if (!in) {
	printf(" This is no TIFF file! \n");
	return(FM_IO_ERR);
    }

    /*
     * Test whether this is a color palette image or not. If so another
     * function should be used.
     */
    status = TIFFGetField(in, 262, &pmi);
    if (pmi != 3) {
	return(FM_IO_ERR);
    }

    status = TIFFGetField(in, 320, &red, &green, &blue);
    if (status != 1) {
	return(FM_IO_ERR);
    }
    for (i=0; i<256; i++) {
	palinfo->cmap[0][i] = red[i];
	palinfo->cmap[1][i] = green[i];
	palinfo->cmap[2][i] = blue[i];
    }

    description = (char *) malloc(FMIO_TIFFHEAD*sizeof(char));
    if (!description) fmerrmsg(where,"Memory allocation failed"); 
    o_description = description;
    TIFFGetField(in, 270, &description);
    /*
     * Lead through the filed tags defined, except for the last one which will
     * create a segmentation fault if it is used. This is processed after the
     * loop.
     */
    currfield = (char *) malloc(FMIO_TIFFHEAD*sizeof(char));
    if (!currfield) fmerrmsg(where,"Memory allocation failed"); 
    o_currfield = currfield;
    nextfield = (char *) malloc(FMIO_TIFFHEAD*sizeof(char));
    if (!nextfield) fmerrmsg(where,"Memory allocation failed"); 
    o_nextfield = nextfield;
    for (i=0; i<FMIO_FIELDS-1; i++) {
	pt = strstr(description, fieldname[i]);
	sprintf(currfield, "%s", pt);
	currlen = strlen(currfield);
	pt = strstr(description, fieldname[i+1]);
	sprintf(nextfield, "%s", pt);
	nextlen = strlen(nextfield);
	taglen = strlen(fieldname[i]);
	fieldlen = currlen-nextlen-taglen;
	field = (char *) malloc((fieldlen+1)*sizeof(char));
	if (!field) fmerrmsg(where,"Memory allocation failed");
	o_field = field;
	currfield += taglen;
	strncpy(field, currfield, fieldlen);
	fm_MITIFF_fillhead(field, fieldname[i], ginfo);
	free(o_field);
    }
    /*
     * The last part of the information header is treated as one single string
     * and is extracted as the remaining part and processed in a suitable way
     * later...
     */
    pt = strstr(description, fieldname[FMIO_FIELDS-1]);
    sprintf(currfield, "%s", pt);
    currlen = strlen(currfield);
    nextlen = strlen(description);
    taglen = strlen(fieldname[FMIO_FIELDS-1]);
    /*
    fieldlen = nextlen-currlen-taglen;
    field = (char *) malloc((fieldlen+1)*sizeof(char));
    */
    fieldlen = nextlen-currlen-taglen;
    field = (char *) malloc((currlen+1)*sizeof(char));
    if (!field) fmerrmsg(where,"Memory allocation failed");
    o_field = field;
    /*
     * Beware here, in order to help C keep track of which memory to free
     * later, currfield should be reduced by taglen imediately or better a new
     * work string should be used, but for now this solution is chosen...
     */
    currfield += taglen;
    /*
    strncpy(field, currfield, fieldlen);
    */
    strncpy(field, currfield, currlen);
    currfield -= taglen;
    /*
    printf(" %d-%d-%d-%d\n",currlen,nextlen,taglen,fieldlen);
    printf("%s\n\n%s\n",currfield, field);
    printf("%d - %d\n",strlen(currfield),strlen(field));
    */
    fm_MITIFF_fillhead_imagepal(field, fieldname[FMIO_FIELDS-1], palinfo); 
    free(o_field);
    free(o_currfield);
    free(o_nextfield);
    free(o_description);
    /*
     * Read image data into matrix.
     */
    TIFFGetField(in, 256, &ginfo->xsize);
    TIFFGetField(in, 257, &ginfo->ysize);
    size = ginfo->xsize*ginfo->ysize;
  
    /*
     * Memory allocated for image data in this function (*image) is freed 
     * in function main process.
     */
    if (ginfo->zsize > FMIO_MAXCHANNELS) {
	printf("\n\tNOT ENOUGH POINTERS AVAILABLE TO HOLD DATA!\n");
	return(FM_IO_ERR);
    }
    for (i=0; i<ginfo->zsize; i++) {
	image[i] = (unsigned char *) malloc((size+1)*sizeof(char));
	if (!image[i]) fmerrmsg(where,"Memory allocation failed");
	status = TIFFReadRawStrip(in, 0, *image, size);
	if (status == -1) return(FM_IO_ERR);
	if (TIFFReadDirectory(in) == 0) break;
    }

    if (ginfo->zsize != (i+1)) {
	printf("\n\tERROR READING MULTIPLE SUBFILES!\n");
	return(FM_IO_ERR);
    }
    
    TIFFClose(in);
    return(FM_OK);
}

/*
 * PURPOSE:
 * To extract the necessary information from the textstring of the DNMI/TIFF
 * files header. This header contains information about date, size etc.
 *
 * RETURN VALUES:
 * 0: OK
 * 2: I/O problem
 * 3: Memory problem
 *
 * NOTES:
 * Function updated for handling of processed imagery. Processed channels are
 * identified by numbers, but this function do not perform any consistency
 * checking of input data. This is left to the user. At present the
 * function has to be recompiled if a satellite or some other specific
 * information is not recognised, this should be changed.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 24/08/1999
 */

int fm_MITIFF_fillhead(char *asciifield, char *tag, fmio_mihead *ginfo) {

    int i;
    char *where="fm_MITIFF_fillhead";
    
    fmremovenewline(asciifield);

    if (!strcmp(tag, "Satellite:")) {
	sprintf(ginfo->satellite, "%s", asciifield);
    }
    else if (!strcmp(tag, "Date and Time:")) {
	ginfo->hour = (unsigned short int) atoi(strtok(asciifield, ":"));
	ginfo->minute = (unsigned short int) atoi(strtok(NULL, " "));
	ginfo->day = (unsigned short int) atoi(strtok(NULL, "/"));
	ginfo->month = (unsigned short int) atoi(strtok(NULL, "-"));
	ginfo->year = (unsigned short int) atoi(strtok(NULL, "\0"));
    }
    else if (!strcmp(tag, "Channels:")) 
	ginfo->zsize = (unsigned short int) atoi(asciifield);
    else if (!strcmp(tag,  "In this file:")) {
	if (strncmp(ginfo->satellite," NOAA",5) == 0 ||
	    strncmp(ginfo->satellite," PROC",5) == 0 ||
	    strncmp(ginfo->satellite," MetOP",6) == 0 ||
	    strstr(ginfo->satellite,"SAF")) {
	    ginfo->ch[0] = (unsigned short int) atoi(strtok(asciifield, " "));
	    for (i=1; i<ginfo->zsize; i++) {
		if (ginfo->zsize>1) 
		    ginfo->ch[i] = (unsigned short int) atoi(strtok(NULL, " "));
	    }
	} else if (strncmp(ginfo->satellite," METE",5) == 0) {
	    strtok(asciifield, " ");
	    if (strncmp(asciifield, " VIS_RAW", 8) == 0) {
		ginfo->ch[0] = 7;
	    } else if (strncmp(asciifield, " WV_CAL", 7) == 0) {
		ginfo->ch[0] = 8;
	    } else if (strncmp(asciifield, " IR_CAL", 7) == 0) {
		ginfo->ch[0] = 9;
	    } else {
		fmerrmsg(where,"Does not recognize channel calibration %s", 
		asciifield);
		return(FM_IO_ERR);
	    }
	} else if (strncmp(ginfo->satellite," WR",3) == 0) {
	    strtok(asciifield, " ");
	    if (strncmp(asciifield, " PSC_dBz", 9) == 0) {
		ginfo->ch[0] = 1;
	    } else if (strncmp(asciifield, " PSC_RR", 7) == 0) {
		ginfo->ch[0] = 2;
	    } else {
		fmerrmsg(where,"Does not recognize channel calibration %s",
		asciifield);
		return(FM_IO_ERR);
	    }
	} else if (strncmp(ginfo->satellite," O.E.",5) == 0 ||
	    strncmp(ginfo->satellite," GTOP",5) == 0) {
	    strtok(asciifield, " ");
	    ginfo->ch[0] = 0;
	} else {
	    fmlogmsg(where,
	    "Could not recognize satellite [%s]",
	    ginfo->satellite);
	    strtok(asciifield, " ");
	    ginfo->ch[0] = 0;
	    return(FM_IO_ERR);
	}
    }
    else if (!strcmp(tag, "Xsize:")) ginfo->xsize = atoi(asciifield);
    else if (!strcmp(tag, "Ysize:")) ginfo->ysize = atoi(asciifield);
    else if (!strcmp(tag, "Bx:")) ginfo->Bx = (float) atof(asciifield);
    else if (!strcmp(tag, "By:")) ginfo->By = (float) atof(asciifield);
    else if (!strcmp(tag, "Ax:")) ginfo->Ax = (float) atof(asciifield);
    else if (!strcmp(tag, "Ay:")) ginfo->Ay = (float) atof(asciifield);

    return(FM_OK);
}

/*
 * PURPOSE:
 * To be used after fillhead. fillhead extracts the standard information in the
 * header, while fillhead_imagepal extracts the information connected to the
 * classes defined in the image, which color they are represented by and the
 * classnames.
 *
 * NOTE:
 * At present any NWP information that might be at the end of the header is not
 * decoded.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/07/1999
 */

int fm_MITIFF_fillhead_imagepal(char *asciifield, char *tag, fmio_mihead_pal *palinfo) {

    int i;
    char *pt;
    char *where="fm_MITIFF_fillhead_imagepal";
    
    pt = strtok(asciifield, "\n");
    palinfo->name = (char *) malloc(strlen(pt)+1);
    if (!palinfo->name) {
	fmerrmsg(where,"Memory allocation failed");
    }
    sprintf(palinfo->name, "%s", pt);
    pt = strtok(NULL, "\n");
    palinfo->noofcl = atoi(pt);
    palinfo->clname = (char **) malloc(palinfo->noofcl*sizeof(char *));
    if (!palinfo->clname) {
	fmerrmsg(where,"Memory allocation failed");
    }
    for (i=0; i<palinfo->noofcl; i++) {
	pt = strtok(NULL, "\n");
	/*
	printf("%s %d\n",pt,strlen(pt));
	*/
	palinfo->clname[i] = (char *) malloc((strlen(pt)+1)*sizeof(char));
	if (!palinfo->clname[i]) {
	    fmerrmsg(where,"Memory allocation failed");
	}
	sprintf(palinfo->clname[i], "%s", pt);
    }
    return(FM_OK);
}

#endif

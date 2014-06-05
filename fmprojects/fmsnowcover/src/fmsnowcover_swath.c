/*
 * PURPOSE:
 * To determine whether a pixel is covered by snow for AVHRR pixels.
 *
 * INPUT:
 * o Multichannel AVHRR satellite imagery in MEOS/HDF format, filename
 *   specified on commandline. Full path is required.
 * o Mask file in DNMI TIFF format specified by symbolic link.
 *
 * OUTPUT:
 * o OSIHDF5 file containing floating point values for 3 classes.
 * o DNMI TIFF file containing colortable.
 *   Filename has to be specified at commandline. Full path is required.
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * �ystein God�y, DNMI/FOU, 13/12/2000
 *
 * MODIFICATIONS:
 * �ystein God�y, DNMI/FOU, 02/03/2001
 * Datastream altered, removed some parts which were only used for testing.
 * Output format MITIFF was updated. Still a lot of cleaning of code is
 * required.
 * �ystein God�y, DNMI/FOU, 25/03/2001
 * Implemented use of configuration file, additional changes in user
 * interface and operation is required for easy porting to other sites.
 * �ystein God�y, met.no/FOU, 27.09.2004: Modification for full Bayes
 * approach started...
 * �ystein God�y, METNO/FOU, 30.10.2006: Adapting software to the results
 * achieved by Vibeke W. Thyness and Hanne Heiberg.  First the use of
 * NWCSAF PPS cloud mask etc is omitted, then add Bayes handling of
 * clouds, and increase number of output variables (add probability of
 * cloud and probability of open water), finally check if some other
 * statistics is better to use than the present one.
 * �ystein God�y, METNO/FOU, 11.01.2007: Changed some names, and some
 * other modifications.
 * Mari Anne Killie, METNO/FOU, 31.01.2008: Some modifications in main
 * and decode_cfg + added functions rdstatcoeffs, locstatcoeffs and
 * putcoeffs, all so that the statistical coeffs needed in probest can
 * be read from text file rather than being hardcoded in probest.c.
 * Mari Anne Killie, METNO/FOU, 13.10.2008: introduced #ifdef
 * AVHRRICE_HAVE_NWP as quick way to comment out nwp.
 * �ystein God�y, METNO/FOU, 07.04.2009: The statement above has been
 * changed to FMSNOWCOVER_HAVE_LIBUSENWP for compliance with the Autoconf
 * setup.
 * Mari Anne Killie, METNO/FOU, 08.05.2009: some modifications for
 * landmask + surface. d34 removed.
 * Mari Anne Killie, METNO/FOU, 02.07.2010: replacing
 * store_mitiff_result with store_snow.
 * Anette Lauen Borg, METNO/Metklim, 05.2014: Adapt to swath input files from PPS
 * Input files: Data file, physiography file, sunsatangle file (all from PPS) + NWP file (hirlam12)
 * Program reads data from noaa/avhrr and npp/viirs instruments. Instrument dependent part in pix_proc_swath.c! (process_pixels4ice_swath)
 *
 * CVS_ID:
 * $Id: fmsnowcover.c,v 1.12 2010-07-02 15:07:18 mariak Exp $
 */

//./fmsnowcover -c ../etc/cfgfile_fmsnowcover_new.txt -i S_NWC_viirs_npp_09364_20130818T1055446Z_20130818T1109575Z.h5 -l S_NWC_physiography_npp_09364_20130818T1055446Z_20130818T1109575Z.h5 -s S_NWC_sunsatangles_npp_09364_20130818T1055446Z_20130818T1109575Z.h5

//./fmsnowcover -c ../etc/cfgfile_fmsnowcover_new.txt -i noaa19_20140520_0644_27205_satproj_00000_02106_avhrr.h5 -l noaa19_20140520_0644_27205_satproj_00000_02106_physiography.h5 -s noaa19_20140520_0644_27205_satproj_00000_02106_sunsatangles.h5

//./fmsnowcover -c ../etc/cfgfile_fmsnowcover_new.txt -i noaa18_20140528_0442_46474_satproj_00000_04670_avhrr.h5 -l noaa18_20140528_0442_46474_satproj_00000_04670_physiography.h5 -s noaa18_20140528_0442_46474_satproj_00000_04670_sunsatangles.h5

//./fmsnowcover -c ../etc/cfgfile_fmsnowcover_new.txt -i noaa19_20140530_1128_27349_satproj_00000_04623_avhrr.h5 -l noaa19_20140530_1128_27349_satproj_00000_04623_physiography.h5 -s noaa19_20140530_1128_27349_satproj_00000_04623_sunsatangles.h5

//./fmsnowcover -c ../etc/cfgfile_fmsnowcover_new.txt -i noaa18_20140602_0527_46545_satproj_00000_03775_avhrr.h5 -l noaa18_20140602_0527_46545_satproj_00000_03775_physiography.h5 -s noaa18_20140602_0527_46545_satproj_00000_03775_sunsatangles.h5
//




#include <fmsnowcover.h>
#include <unistd.h>
#include <fmaccusnow.h>
/*#undef FMSNOWCOVER_HAVE_LIBUSENWP*/


int main2(int argc, char *argv[]) {


    char *where="fmsnowcover_swath";
    char what[FMSNOWCOVER_MSGLENGTH];
    extern char *optarg;
    int ret;
    short errflg = 0, iflg = 0, cflg = 0, lflg = 0, sflg = 0;
    short status;
    unsigned int size;
    char fname[100],lname[120],sname[120],datestr[25];
    char *lmaskf, *opfn1, *opfn2, *opfn3;
    char *infile, *cfgfile, *coffile, *sunzenf;
    char *fnwc[3]={"h12sf","h12pl","h12ml"};
    unsigned char *classed, *cat;
    cfgstruct cfg; //fmsnowcover.h?
    FILE *lmask_located, *sunzen_located; /*Can be removed later*/
    fmio_mihead clinfo = {  //fmio.h
	"Not known",
	00, 00, 00, 00, 0000, -9,
	{0, 0, 0, 0, 0, 0, 0, 0},
	0, 0, 0, 0., 0., -999., -999.
    };
    fmdataset img, lm, sz;  //fmutil.h
    fmucsref refucs;  //fmutil.h
    fmtime reftime;  //fmtime.h
    nwpice nwp; //getnwp.h
    osihdf ice;  //safhdf.h
    osi_dtype ice_ft[FMSNOWCOVER_OLEVELS]={OSI_FLOAT,OSI_FLOAT,OSI_FLOAT};   //safhdf.h
    char *ice_desc[FMSNOWCOVER_OLEVELS]={"P(ice/snow)","P(water/land)","P(cloud)"};
    float cloudfree;

    statcoeffstr coeffs = {{{0}}}; //fmsnowcover.h
    int image_type;



    /*
     * Interpret command line arguments. c=config file, i=input data file, l=land mask input file, s=sun angle file
     */
     while ((ret = getopt(argc, argv, "c:i:l:s:")) != EOF) {
    	 switch (ret) {
    	 case 'c':
    		 cfgfile = (char *) malloc(FILELEN);
    		 if (!cfgfile) {
    			 fmerrmsg(where,"Memory trouble.");
    			 exit(FM_MEMALL_ERR);
    		 }
    		 if (!strcpy(cfgfile, optarg)) exit(FM_OK);
    		 cflg++;
    		 break;
    	 case 'i':
    		 if (!strcpy(fname, optarg)) exit(FM_OK);
    		 iflg++;
    		 break;
    	 case 'l':
    		 if (!strcpy(lname, optarg)) exit(FM_OK);
    		 lflg++;
    		 break;
    	 case 's':
    		 if (!strcpy(sname, optarg)) exit(FM_OK);
    		 sflg++;
    		 break;
    	 default:
    		 usage();
    		 break;
    	 }
     }



    if (!iflg || !cflg || !lflg || !sflg) errflg++; //If input arguments are missing
    if (errflg) usage();

    fprintf(stdout,"\n");
    fprintf(stdout," ================================================\n");
    fprintf(stdout," |                  FMSNOWCOVER                 |\n");
    fprintf(stdout," ================================================\n");
    fprintf(stdout,"\n");

    /*
     * Decode configuration file, put file names etc in struct type "cfgstruct" (defined in fmsnowcover.h?)
     */
    if (decode_cfg_swath(cfgfile,&cfg) != 0) {
	fmerrmsg(where,"Could not decode configuration");
	exit(FM_IO_ERR);
    }

    /*
     * Set up datapaths etc.
     */
    //Input data file
    infile = (char *) malloc(FILELEN);
    if (!infile) {
    	fprintf(stderr,"%s\n"," Trouble processing:");
    	fprintf(stderr,"%s\n",infile);
    	fmerrmsg(where,"Could not allocate memory for infile");
    	exit(FM_MEMALL_ERR);
    }
    sprintf(infile,"%s/%s",cfg.imgpath,fname);

    //Landmask file
    lmaskf = (char *) malloc(FILELEN);
    if (!lmaskf) {
    	fprintf(stderr,"%s\n"," Trouble processing:");
    	fprintf(stderr,"%s\n",infile);
    	fmerrmsg(where,"Could not allocate memory for lmaskf");
    	exit(FM_MEMALL_ERR);
    }
    sprintf(lmaskf,"%s/%s",cfg.lmpath,lname);

    //Sun zenith angle file
        sunzenf = (char *) malloc(FILELEN);
        if (!sunzenf) {
        	fprintf(stderr,"%s\n"," Trouble processing:");
        	fprintf(stderr,"%s\n",infile);
        	fmerrmsg(where,"Could not allocate memory for sunzenf");
        	exit(FM_MEMALL_ERR);
        }
        sprintf(sunzenf,"%s/%s",cfg.szpath,sname);


    /*setting path to file containing probability coeffs*/
    coffile = (char *) malloc(FILELEN);
    if (!coffile) {
    	fprintf(stderr,"%s\n"," Trouble processing:");
    	fprintf(stderr,"%s\n",coffile);
    	fmerrmsg(where,"Could not allocate memory for coffile");
    	exit(FM_MEMALL_ERR);
    }
    sprintf(coffile,"%s",cfg.probtabname);



    /*
     * Open data file and read image
     * data and information
     */
    fprintf(stdout," Reading input data...\n");
    fprintf(stdout," %s\n", fname);

    if (init_fmdataset(&img)) { //Initialize struct img (fmdataset)
    	fmerrmsg(where,"Could not initialize fmdataset");
    	exit(FM_OTHER_ERR);
    }

    if(fm_readMETSATdata_swath(infile, &img)){ //Read data into struct img
    	fmerrmsg(where,"Could not open file...\n");
    	exit(FM_IO_ERR);
    }

    /*
     * Check the number of valid pixels
     *
     * Checking channel 1
     */

    size = img.h.xsize*img.h.ysize;
    int valpix = 0;
    int i,j;
    for (i=0;i<img.h.ysize;i++) {
    	for(j=0;j<img.h.xsize;j++) {
    		if ((img.d)->intarray[i][j] == (img.d)[0].missingdatavalue || (img.d)->intarray[i][j] == (img.d)[0].nodatavalue) continue; //If data value = default value (no data), skip to next data point
    		valpix++; //... or, if valid data value, add to valpix.
    	}
    }
    float cover = valpix*100./size; //Find % of valid data
    printf(" Image cover: %.2f\n",cover);
    if ((cover < 20. && (strstr(fname,"NoA") == NULL))) { //If less than 20% of data is valid, end program.
    	fmlogmsg(where,
    			"The percentage coverage (%.0f%) of this scene is too small for further processing.",cover);
    	exit(FM_OK);
    }

    /*
     * Get land/sea mask produced by PPS.
     */

    lm.d = NULL;
    if (lmask_located = fopen(lmaskf,"r")) {
    	fprintf(stdout," Reading land/sea mask (GTOPO30 based):\n %s\n", lmaskf);
    	//status = read_hdf5_product(lmaskf, &lm, 0);
    	status = fm_readMETSATdata_swath(lmaskf, &lm);
    	fclose(lmask_located);
    	if (status != 0) {
    		fprintf(stderr,"%s\n"," Trouble processing:");
    		fprintf(stderr,"%s\n",lmaskf);
    		fprintf(stderr,"%s %s\n", fmerrmsg,"Could not read land/sea mask");
    		return(FM_IO_ERR);
    	}

    }
    else {
    	fmlogmsg(where,"No landmask is available, continuing without.");
    }


    /*
     * Get sun zenith angle, produced by PPS
     */
    if (sunzen_located = fopen(sunzenf,"r")) {
    	fprintf(stdout," Reading sun zenith angle: %s\n", sunzenf);
    	status = fm_readMETSATdata_swath(sunzenf, &sz);
    	fclose(sunzen_located);
    	if (status != 0) {
    		fprintf(stderr,"%s\n"," Trouble processing:");
    		fprintf(stderr,"%s\n",sunzenf);
    		fprintf(stderr,"%s %s\n", fmerrmsg,"Could not read sun zenith angle");
    		return(FM_IO_ERR);
    	}
    }
    else {
    	fmerrmsg(where,"No sun zenith angle file available!\n");
    	exit(FM_IO_ERR);
    }



    //Transfer info
    printf(" Satellite: %s\n", img.h.platform_name);
    printf(" Time: %02d/%02d/%4d %02d:%02d\n", img.h.valid_time.fm_mday, img.h.valid_time.fm_mon, img.h.valid_time.fm_year, //img.dd, img.mm, img.yy,
    		img.h.valid_time.fm_hour, img.h.valid_time.fm_min);//img.ho, img.mi);
    reftime.fm_hour = img.h.valid_time.fm_hour;
    reftime.fm_min = img.h.valid_time.fm_min;
    reftime.fm_sec = 0;
    reftime.fm_mon = img.h.valid_time.fm_mon;
    reftime.fm_mday = img.h.valid_time.fm_mday;
    reftime.fm_year = img.h.valid_time.fm_year;
    refucs.Ax = (double) img.h.ucs.Ax;
    refucs.Ay = (double) img.h.ucs.Ay;
    refucs.Bx = (double) img.h.ucs.Bx;
    refucs.By = (double) img.h.ucs.By;
    refucs.iw =  img.h.xsize;
    refucs.ih = img.h.ysize;

    /*
     * Get NWP data (model)...
     * This is probably not necessary in the future, but is kept until it
     * is clear whether T4 will be used to avoid cloud contamination or not.
     */

    nwpice_init(&nwp);

#ifdef FMSNOWCOVER_HAVE_LIBUSENWP
    if (nwpice_read(cfg.nwppath,fnwc,3,4,reftime,refucs,&nwp)) {
    	fmerrmsg(where,"No NWP data available.");
    	free_fmdataset(&img);
    	nwpice_free(&nwp);
    	exit(FM_IO_ERR);
    }
#endif


        /*
     * Loading the statistical coeffs into statcoeffs struct
     */
    fmlogmsg(where,"Loading statistical coefficients from \n\t%s", coffile);
    ret = rdstatcoeffs(coffile,&coeffs);
    if (ret) {
    	/*fmerrmsg(where," Trouble reading statistical coefficients, exiting..");
	exit(FM_IO_ERR);*/
    	printf(" WARNING: %d potential issues encountered ",ret);
    	printf("when loading coefficients\n");
    }

    /*
     * Function "process_pixels4ice" is called to perform the objective
     * classification of the present satellite scene. Further description
     * of the function is given in the code.
     */
    init_osihdf(&ice); //Initialize struct ice, and set header info
    sprintf(ice.h.source, "%s", img.h.platform_name);
    sprintf(ice.h.product, "%s", where);
    ice.h.iw = img.h.xsize;
    ice.h.ih = img.h.ysize;
    ice.h.z = FMSNOWCOVER_OLEVELS;
    ice.h.Ax = img.h.ucs.Ax;
    ice.h.Ay = img.h.ucs.Ay;
    ice.h.Bx = img.h.ucs.Bx;
    ice.h.By = img.h.ucs.By;
    ice.h.year = img.h.valid_time.fm_year;
    ice.h.month = img.h.valid_time.fm_mon;
    ice.h.day = img.h.valid_time.fm_mday;
    ice.h.hour = img.h.valid_time.fm_hour;
    ice.h.minute = img.h.valid_time.fm_min;
    status = malloc_osihdf(&ice,ice_ft,ice_desc);

    classed = (unsigned char *) malloc(size*sizeof(char)); //Allocate array
    if (!classed) {
	sprintf(what,
	"Could not allocate memory for classed array while processing : %s\n",
		infile);
	fmerrmsg(where,what);
	exit(FM_MEMALL_ERR);
    }
    /*MAK added 22/9-09*/
    cat = (unsigned char *) malloc(size*sizeof(char)); //Allocate array
    if (!cat) {
	sprintf(what,
	"Could not allocate memory for cat array while processing : %s\n",
		infile);
	fmerrmsg(where,what);
	exit(FM_MEMALL_ERR);
    }


    fmlogmsg(where,"Estimating ice probability");

    //Input parameters of process_pixels4ice_swath: data struct, cmask?, landmask struct, model data (NWP) struct, sun zenith angles struct, struct with info,
    //array to be filled with classifications, array to be filled with categories, algo, coefficient struct
    if (lm.d == NULL) { //If no land mask
      status = process_pixels4ice_swath(img, NULL, NULL, nwp, sz,
				  ice.d, classed, cat, 2, coeffs);
    } else {
      status = process_pixels4ice_swath(img, NULL, (int **)(lm.d[0].intarray), nwp, sz,
				  ice.d, classed, cat, 2, coeffs);
    }

    //Check status of result
    if ((status) && (status != 10)) {
	sprintf(what,"Something failed while processing pixels of %s",infile);
	fmerrmsg(where,what);
    } else if (status == 10) {
        fmlogmsg(where,
	   "Finished estimating ice probability for swath");
    } else {
        fmlogmsg(where,"Finished estimating ice probability for swath");
    }


    /*
     * Clean up memory used.
     *
     * Should add freeing of lmask here if needed in future...
     */
    fmlogmsg(where,"Cleaning memory");
    free_fmdataset(&img);
    if (lm.d != NULL) {
    	free_fmdataset(&lm);
    }

    /*
     * Write results to files, HDF5 file for internal use and TIFF 6.0
     * (MITIFF) file for visual presentation on Internet/DIANA etc.
     *
     * MITIFF generation will be moved to a separate application in
     * time...
     */
    sprintf(clinfo.satellite,"%s",img.h.platform_name);
    clinfo.hour = img.h.valid_time.fm_hour;
    clinfo.minute = img.h.valid_time.fm_min;
    clinfo.day = img.h.valid_time.fm_mday;
    clinfo.month = img.h.valid_time.fm_mon;
    clinfo.year = img.h.valid_time.fm_year;
    clinfo.zsize = 1;
    clinfo.xsize = img.h.xsize;
    clinfo.ysize = img.h.ysize;
    clinfo.Ax = img.h.ucs.Ax;
    clinfo.Ay = img.h.ucs.Ay;
    clinfo.Bx = img.h.ucs.Bx;
    clinfo.By = img.h.ucs.By;

    fmlogmsg(where,"Write to product files");

    opfn1 = (char *) malloc(FILELEN+5);
    if (!opfn1) exit(FM_IO_ERR);
    sprintf(opfn1,"%s/fmsnow_%4d%02d%02d%02d%02d.hdf5",
	cfg.productpath,
	clinfo.year, clinfo.month, clinfo.day, clinfo.hour, clinfo.minute);
    sprintf(what,"Creating output file: %s", opfn1);
    fmlogmsg(where,what);
    status = store_hdf5_product(opfn1,ice);
    if (status != 0) {
	sprintf(what,"Trouble processing: %s",infile);
	fmerrmsg(where,what);
    }

    opfn2 = (char *) malloc(FILELEN+5);
    if (!opfn2) exit(FM_IO_ERR);
    sprintf(opfn2,"%s/fmsnow_%4d%02d%02d%02d%02d.mitiff",
	cfg.productpath,
	clinfo.year, clinfo.month, clinfo.day, clinfo.hour, clinfo.minute);
    sprintf(what,"Creating output file: %s", opfn2);
    fmlogmsg(where,what);
    image_type = 0;
    store_snow(opfn2,classed,clinfo,image_type);


    /*Can be helpful when trying to improve the product*/
    /*Must make some changes in subroutines as well. */
    opfn3 = (char *) malloc(FILELEN+5);
    if (!opfn3) exit(FM_IO_ERR);
    sprintf(opfn3,"%s/fmsnow_cat_%4d%02d%02d%02d%02d.mitiff",
	cfg.productpath,
	clinfo.year, clinfo.month, clinfo.day, clinfo.hour, clinfo.minute);
    sprintf(what,"Creating output file: %s", opfn3);
    fmlogmsg(where,what);
    image_type = 1;
    store_snow(opfn3,cat,clinfo,image_type);


    /*
     * Add information on processed scenes, time and area
     * identifications as well as valid image data coverage within the
     * tile and estimated cloud free coverage of the scene.
     */

    printf(" cover: %f\n",cover);
    cloudfree = findcloudfree(ice.d,img.h.xsize,img.h.ysize);
    fmsec19702isodatetime(tofmsec1970(reftime), datestr);
//    if (updateindexfile(cfg.indexfile,fname,opfn1,datestr,cover,cloudfree)) {
//	fmerrmsg(where,"Could not update %s", cfg.indexfile);
//    }

    fprintf(stdout," ================================================\n");
    free(opfn1);
    free(opfn2);
    free(opfn3);

    /*M� vel med?*/
    free(classed);
    free(cat);
    free_osihdf(&ice);

    exit(FM_OK);
}



/*
 * NAME:
 * decode_cfg
 *
 * PURPOSE:
 * To decode configuration file.
 */

int decode_cfg_swath(char cfgfile[],cfgstruct *cfg) {
    FILE *fp;
    char *where="decode_cfg";
    char *dummy,*pt;
    char *token=" ";

    dummy = (char *) malloc(FILELEN*sizeof(char));
    if (!dummy) {
	fmerrmsg(where,"%s","Could not allocate memory");
	return(FM_MEMALL_ERR);
    }

    fp = fopen(cfgfile,"r");
    if (!fp) {
	fmerrmsg(where,"%s","Could not open config file.");
	return(FM_IO_ERR);
    }

    while (fgets(dummy,FILELEN,fp) != NULL) {
	if (strncmp(dummy,"#",1) == 0) continue;
	if (strlen(dummy) > (FILELEN-50)) {
	    fmerrmsg(where,"%s",
		    "Input string larger than FILELEN");
	    free(dummy);
	    return(FM_IO_ERR);
	}

	pt = strtok(dummy,token);

	if (!pt) {
	    fmerrmsg(where,"%s","strtok trouble.");
	    free(dummy);
	    return(FM_IO_ERR);
	}
	if (strncmp(pt,"IMGPATH",7) == 0) {
	    pt = strtok(NULL,token);
	    if (!pt) {
		fmerrmsg(where,"%s","strtok trouble for imgpath.");
		free(dummy);
		return(FM_IO_ERR);
	    }
	    fmremovenewline(pt);
	    sprintf(cfg->imgpath,"%s",pt);
	} else if (strncmp(pt,"NWPPATH",7) == 0) {
	    pt = strtok(NULL,token);
	    if (!pt) {
		fmerrmsg(where,"%s","strtok trouble for nwppath.");
		free(dummy);
		return(FM_IO_ERR);
	    }
	    fmremovenewline(pt);
	    sprintf(cfg->nwppath,"%s",pt);
	} else if (strncmp(pt,"CMPATH",6) == 0) {
	    pt = strtok(NULL,token);
	    if (!pt) {
		fmerrmsg(where,"%s","strtok trouble for cmpath.");
		free(dummy);
		return(FM_IO_ERR);
	    }
	    fmremovenewline(pt);
	    sprintf(cfg->cmpath,"%s",pt);
	} else if (strncmp(pt,"LMPATH",6) == 0) {
	    pt = strtok(NULL,token);
	    if (!pt) {
		fmerrmsg(where,"%s","strtok trouble for lmpath.");
		free(dummy);
		return(FM_IO_ERR);
	    }
	    fmremovenewline(pt);
	    sprintf(cfg->lmpath,"%s",pt);
	} else if (strncmp(pt,"SZPATH",6) == 0) {
	    pt = strtok(NULL,token);
	    if (!pt) {
		fmerrmsg(where,"%s","strtok trouble for szpath.");
		free(dummy);
		return(FM_IO_ERR);
	    }
	    fmremovenewline(pt);
	    sprintf(cfg->szpath,"%s",pt);
	}else if (strncmp(pt,"PRODUCTPATH",11) == 0) {
	    pt = strtok(NULL,token);
	    if (!pt) {
		fmerrmsg(where,"%s","strtok trouble for productpath.");
		free(dummy);
		return(FM_IO_ERR);
	    }
	    fmremovenewline(pt);
	    sprintf(cfg->productpath,"%s",pt);
	} else if (strncmp(pt,"PROBTABNAME",11) == 0) {
	    pt = strtok(NULL,token);
	    if (!pt) {
		fmerrmsg(where,"%s","strtok trouble for probtabname.");
		free(dummy);
		return(FM_IO_ERR);
	    }
	    fmremovenewline(pt);
	    sprintf(cfg->probtabname,"%s",pt);
	} else if (strncmp(pt,"INDEXFILE",9) == 0) {
	    pt = strtok(NULL,token);
	    if (!pt) {
		fmerrmsg(where,"%s","strtok trouble for indexfile.");
		free(dummy);
		return(FM_IO_ERR);
	    }
	    fmremovenewline(pt);
	    sprintf(cfg->indexfile,"%s",pt);
	}
    }

    fclose(fp);

    free(dummy);

    return(FM_OK);
}

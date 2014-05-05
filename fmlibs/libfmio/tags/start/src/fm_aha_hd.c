/*
 * FUNCTION: 
 * fm_get_aha_hsa
 * fm_get_hd_from_hsa
 * fm_iget_hd_from_hsa
 * fm_dget_hd_from_hsa
 * fm_split_strarr
 * fm_split_chrarr
 * fm_split_intarr
 * fm_fill_aha_hd
 * 
 * PURPOSE:
 * NA
 *
 * NOTES:
 * This file contains several functions used for accessing the METSAT
 * format. It should be reqritten in time, or at least better documented.
 *
 * BUGS:
 * NA
 *
 * RETURN VALUES:
 * NA
 * 
 * AUTHOR:
 * Soren Andersen DMI August 2001 
 * MODIFIED:
 * Øystein Godøy, met.no/FOU, 23.02.2004
 * Added some comments and reindented and cleaned some unused variables...
 * Steinar Eastwood, met.no/FOU, 11.03.2004
 * Removed printing of parameters to stdout.
 * Øystein Godøy, METNO/FOU, 16.10.2006
 * Modified name space for libfmio.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fm_aha_hd.h>

/* Read in the header and store as a string array */
int fm_get_aha_hsa(char *fname,char hsa[FMIO_MAXHDLINES][FMIO_STRMAXCHARS]) {
    FILE *f1;
    int i, j;
    char line[FMIO_STRMAXCHARS]="";

    f1=fopen(fname,"r");
    i=0;
    while ((strncmp(line,"EOH",3)!=0) && !feof(f1) && (i<FMIO_MAXHDLINES)){ 
	fgets(line,FMIO_STRMAXCHARS,f1);
	strcpy(hsa[i],line);
	i++;
    }
    /*
    printf(">>i: %d\n",i);
    for (j=0;j<i;j++) printf("%02d %s",j,hsa[j]);
    */
    fclose(f1);
    return(i);
}


int fm_get_hd_from_hsa(char hsa[FMIO_MAXHDLINES][FMIO_STRMAXCHARS],int nhsa,char key[FMIO_STRMAXCHARS],char hd[FMIO_STRMAXCHARS]) {

    int i,lnkey,retval=FMIO_FALSE;

    i=0;
    lnkey=strlen(key);
    while ((i<nhsa) && (strncmp(hsa[i],key,lnkey)!=0)) {
	i++;
    }
    /*
    printf(" %s lnkey: %d nhsa: %d i: %d %s\n",key,lnkey,nhsa,i,hsa[i]);
    */
    hd[0]='\0';
    if (i<nhsa) {
	strcpy(hd,strstr(hsa[i],"= ")+2);
	hd[strlen(hd)-1]='\0';
	retval=FMIO_TRUE;
    }
    return(retval);
}


int fm_iget_hd_from_hsa(char hsa[FMIO_MAXHDLINES][FMIO_STRMAXCHARS],int nhsa,char key[FMIO_STRMAXCHARS]) {

    int retval=FMIO_IMISS;
    char hd[FMIO_STRMAXCHARS];

    if (fm_get_hd_from_hsa(hsa,nhsa,key,hd)) sscanf(hd,"%d",&retval);
    return(retval);
}

double fm_dget_hd_from_hsa(char hsa[FMIO_MAXHDLINES][FMIO_STRMAXCHARS],int nhsa,char key[FMIO_STRMAXCHARS]) {

    double retval=FMIO_DMISS;
    char hd[FMIO_STRMAXCHARS];

    if (fm_get_hd_from_hsa(hsa,nhsa,key,hd)) sscanf(hd,"%lf",&retval);
    return(retval);
}


void fm_split_strarr(char strarr[FMIO_MAXNLAY][FMIO_STRMAXCHARS], int n, char buf[FMIO_STRMAXCHARS]) {

    int i;
    char *pt;

    pt=buf;
    strtok(pt,"'");
    for (i=0;i<n;i++) {
	pt=strtok(NULL,"'");
	if (pt != NULL) strcpy(strarr[i],pt);
	else strarr[i][0]='\0';
	pt=strtok(NULL,"'");
    }
}


void fm_split_chrarr(char chrarr[FMIO_MAXNLAY], int n, char buf[FMIO_STRMAXCHARS]) {

    int i;
    char *pt;

    pt=buf;
    strtok(pt,"'");
    for (i=0;i<n;i++) {
	pt=strtok(NULL,"'");
	if (pt != NULL) chrarr[i]=*pt;
	else chrarr[i]='\0';
	pt=strtok(NULL,"'");
    }
}


void fm_split_intarr(int intarr[FMIO_MAXNLAY], int n, char buf[FMIO_STRMAXCHARS]) {

    int i;
    char *pt,buf2[FMIO_STRMAXCHARS];

    sprintf(buf2," %s\n",buf);
    pt=buf2;
    strtok(pt,"[");
    for (i=0;i<n;i++) {
	pt=strtok(NULL,",]");
	if (pt != NULL) intarr[i]=atoi(pt);
	else intarr[i]=FMIO_IMISS;
    }
}


int fm_fill_aha_hd(char *fname,ahahd *header) {

    char hsa[FMIO_MAXHDLINES][FMIO_STRMAXCHARS],buf[FMIO_STRMAXCHARS];
    int i,j,nhsa,pos;
    double dtmp;
    ahahd h;

    if (!(nhsa=get_aha_hsa(fname, hsa))) {
	fprintf(stderr,"Error: Could not read header from file %s\n",fname);
	return FMIO_FALSE;
    }

    /* First get number of layers to be ready to split string arrays */
    h.layers=fm_iget_hd_from_hsa(hsa,nhsa,"layers");

    /* Now we can get on with the rest */
    fm_get_hd_from_hsa(hsa,nhsa,"product",h.product); /* 1 */
    fm_get_hd_from_hsa(hsa,nhsa,"satid",h.satid);
    fm_get_hd_from_hsa(hsa,nhsa,"channel_id",buf);
    split_strarr(h.channel_id,h.layers,buf);
    fm_get_hd_from_hsa(hsa,nhsa,"version_des",h.version_des);
    fm_get_hd_from_hsa(hsa,nhsa,"site_id",h.site_id);

    fm_get_hd_from_hsa(hsa,nhsa,"typecode",buf); /* 2 */
    split_chrarr(h.typecode,h.layers,buf);
    fm_get_hd_from_hsa(hsa,nhsa,"itemsize",buf);
    split_intarr(h.itemsize,h.layers,buf);
    fm_get_hd_from_hsa(hsa,nhsa,"datatypes",buf);
    split_strarr(h.datatypes,h.layers,buf);
    fm_get_hd_from_hsa(hsa,nhsa,"compressed",buf);
    h.compressed=buf[0];
    fm_get_hd_from_hsa(hsa,nhsa,"start_byte",buf);
    split_intarr(h.start_byte,h.layers,buf);
    fm_get_hd_from_hsa(hsa,nhsa,"little_end",buf);
    h.little_end=buf[0];

    h.nodata=fm_dget_hd_from_hsa(hsa,nhsa,"nodata"); /* 4 */
    h.missing_data=fm_dget_hd_from_hsa(hsa,nhsa,"missing_data");
    h.footprint_nodata=fm_dget_hd_from_hsa(hsa,nhsa,"footprint_nodata");
    h.scan_nodata=fm_dget_hd_from_hsa(hsa,nhsa,"scan_nodata");
    h.spot_nodata=fm_dget_hd_from_hsa(hsa,nhsa,"spot_nodata");

    h.numof_scanlines=fm_iget_hd_from_hsa(hsa,nhsa,"numof_scanlines"); /* 4 */
    h.numof_missing_scanlines=fm_iget_hd_from_hsa(hsa,nhsa,"numof_missing_scanlines"); 

    h.data_first_year=fm_iget_hd_from_hsa(hsa,nhsa,"data_first_year"); /* 5 */
    h.data_first_dayofyear=fm_iget_hd_from_hsa(hsa,nhsa,"data_first_dayofyear"); 
    h.data_first_secofday=fm_dget_hd_from_hsa(hsa,nhsa,"data_first_secofday"); 
    h.data_last_year=fm_iget_hd_from_hsa(hsa,nhsa,"data_last_year"); 
    h.data_last_dayofyear=fm_iget_hd_from_hsa(hsa,nhsa,"data_last_dayofyear"); 
    h.data_last_secofday=fm_dget_hd_from_hsa(hsa,nhsa,"data_last_secofday"); 
    h.data_first_sec1970=fm_dget_hd_from_hsa(hsa,nhsa,"data_first_sec1970"); 
    h.orbit_epoch_year=fm_iget_hd_from_hsa(hsa,nhsa,"orbit_epoch_year"); 
    h.orbit_epoch_day=fm_iget_hd_from_hsa(hsa,nhsa,"orbit_epoch_day"); 

    fm_get_hd_from_hsa(hsa,nhsa,"area_extent",buf); /* 7 */
    pos=0;
    for (i=0;i<4;i++) {
	sscanf(&(buf[pos]),"%lf%n",&dtmp,&j);
	pos+=j;
	h.area_extent[i]=dtmp;
    }

    fm_get_hd_from_hsa(hsa,nhsa,"pcs",h.pcs);
    fm_get_hd_from_hsa(hsa,nhsa,"area",h.area);
    h.xsize=fm_iget_hd_from_hsa(hsa,nhsa,"xsize");
    h.ysize=fm_iget_hd_from_hsa(hsa,nhsa,"ysize");
    h.xscale=fm_dget_hd_from_hsa(hsa,nhsa,"xscale");
    h.yscale=fm_dget_hd_from_hsa(hsa,nhsa,"yscale");

    h.ir_gain=fm_dget_hd_from_hsa(hsa,nhsa,"ir_gain"); /* 8 */
    h.ir_intercept=fm_dget_hd_from_hsa(hsa,nhsa,"ir_intercept");
    h.vis_gain=fm_dget_hd_from_hsa(hsa,nhsa,"vis_gain");
    h.vis_intercept=fm_dget_hd_from_hsa(hsa,nhsa,"vis_intercept");
    fm_get_hd_from_hsa(hsa,nhsa,"comment",h.comment);

    *header=h;
    return FMIO_TRUE;
}


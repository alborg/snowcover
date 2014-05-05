/*
 * NAME:
 * fm_little_end
 * fm_swap_dmi
 * fm_julday
 * fm_caldat
 * fm_get_subtrack
 * fm_get_channeldata
 * fm_cnvtm
 * fm_readdataMETSAT
 * fm_readheaderMETSAT
 *
 * PURPOSE: 
 * Functions to read AVHRR data on METSAT format into met.no imgh struct.
 *
 * RETURN VALUES:
 * Negative on failure else dependent on function
 *
 * NOTES:
 * Some of the functions within this file should be extracted to more
 * general functions in time.
 *
 * BUGS:
 * NA
 * 
 * AUTHOR: 
 * Soren Andersen, DMI, August 2001
 * MODIFIED: 
 * Soren Andersen, DMI, 21.03.2003  
 * Added copy of "area" to img struct
 * Steinar Eastwood, met.no, 29.12.2003  
 * Changed structure and names of functions.
 * Steinar Eastwood, met.no, 30.12.2003  
 * Bug in calculating value h->By in readdataMETSAT.
 * Bug in get_channeldata (must map to unsigned short).
 * Changed order in int2ch.
 * Steinar Eastwood, met.no, 09.02.2004  
 * Bug fix, updated version from DMI.
 * Steinar Eastwood, met.no, 23.02.2004  
 * Checking byte order on METSAT files and do byte swap if necessary.
 * Øystein Godøy, met.no, 12.03.2004  
 * Added some comments and note possible conflict/duplicate on the use of
 * julday... 
 * Øystein Godøy, met.no/FOU, 29.03.2004
 * exit -> return + messages
 * Øystein Godøy, met.no/FOU, 30.09.2004
 * Removed some memory leaks that appeared when the routines were called
 * several times...
 * Steinar Eastwood, met.no, 18.10.2006  
 * Added filling orbit_no in readheaderMETSAT and readdataMETSAT.
 * Øystein Godøy, METNO/FOU, 16.10.2006
 * Modified name space for libfmio.
 * Mari Anne Killie, METNO/FOU, 08.05.2009
 * Added Metop
 *
 * ID:
 * $Id$
 */ 

#include <fmio.h>
#include <fm_aha_hd.h>

#define SIGN2UNSIGN 32768

/* DMI sequence of channels to DNMI sequence, not in use */
int fm_int2ch[]={0,1,2,3,4,5}; 


/* Utilities to check byte order */
int fm_little_end( void ) {
    /* will return 1 if machine is little endian */
    int i = 1;
    return *((char *) &i );
}

/* Utilities to swap data */
size_t fm_swap_dmi( void *x, size_t size, size_t nitems ) {

    char *where="fm_swap_dmi",mymsg[255];
    size_t i;
    char c0, c1;
    unsigned char *b = (unsigned char *)x;

    switch( size ) {

        case 2:
            for ( i=0; i<nitems; ++i, b += size ) {
                c0 = b[0];
                b[0] = b[1];
                b[1] = c0;
            }
            break;

        case 4:
            for ( i=0; i<nitems; ++i, b += size ) {
                c0 = b[0]; c1 = b[1];
                b[0] = b[3]; b[1] = b[2];
                b[2] = c1; b[3] = c0;
            }
            break;

        default:
            sprintf(mymsg,"%s %d","can not handle size:", size );
            fmerrmsg(where,mymsg);
            return 0;    
    }

    return nitems;  
}


/* 
 * Program til udregning af Juliansk dag. Jul. dag starter kl. 12 middag.
 *
 * Such a function is also available within the core libsatimg, care
 * should be taken when unsigned short intng/changing these functions...
 */

double fm_julday(int id, int mm, int iyyy, double hour) {

    char *where="dmijulday";
    int igreg = 588829;
    long int ja, jm, jy;
    long int jul,jyyy;
    double juld;

    if (iyyy == 0) 
    { 
        fmerrmsg(where,"There is no year zero");
        return(-2);
    }

    if (iyyy < 0) iyyy++;

    if (mm > 2) 
    {
        jy = iyyy;
        jm = mm+1;
    }
    else
    {
        jy = iyyy-1;
        jm = mm+13;
    }
    jul = (long int)(365.25*jy)+ (long int)(30.6001*jm)+id+1720995;
    jyyy = iyyy;

    if (id+31*(mm+(12*jyyy)) >= igreg)
    {
        ja = (int)(0.01*jy);
        jul += 2-ja+(long int)(0.25*ja);
    }
    juld = jul + (hour-12)/24;
    return juld;

}


void fm_caldat(int *d, int *m, int *y, double *h, double julian) {

    int igreg = 2299161;
    long int je, jd, jc, jb, jalpha, ja;
    double fday;

    if (julian >= igreg)
    {
        jalpha = (long int)((((long int)julian-1867216)-0.25)/36524.25);
        ja = (long int)julian+1+jalpha-(long int)(0.25*jalpha);
    }
    else
    {
        ja = (long int)julian;
    }
    fday = julian - (long int)julian; 
    if (fday >= 0.5) ja++;
    jb = ja+1524;
    jc = (long int)(6680.0+((jb-2439870)-122.1)/365.25);
    jd = 365*jc+(long int)(0.25*jc);
    je = (long int)((jb-jd)/30.6001);
    *d = jb-jd-(long int)(30.6001*je);
    *m = je-1;
    if (*m > 12) *m += -12;
    *y = jc-4715;
    if (*m > 2) (*y)--;
    if (*y <= 0) (*y)--;
    if (fday <0.5)
    {
        *h=(fday*24)+12;
    }
    else
    {
        *h=(fday*24)-12;
    }
}

int fm_get_subtrack(char *filename, fmio_subtrack **st, int doswap) {

    char *where="fm_get_subtrack",mymsg[255];
    int numtrack,hdsz,dtsz,numread,k;
    char buf[FMIO_STRMAXCHARS];
    FILE *f1;

    if (*st) {
        free(*st);
    }

    f1=fopen(filename,"r");
    if (!f1) {
        sprintf(mymsg,"%s %s",
                "Could not open",filename);
        fmerrmsg(where,mymsg);
        return(-1);
    }
    fgets(buf,FMIO_STRMAXCHARS,f1);
    sscanf(buf,"AHA_METSAT %d %d",&hdsz,&dtsz);
    fseek(f1,hdsz+dtsz,SEEK_CUR);
    fread(&numtrack,sizeof(numtrack),1,f1);
    if (doswap) { fm_swap_dmi((void *)&numtrack,sizeof(int),1); }
    *st = (fmio_subtrack *) malloc(numtrack*sizeof(fmio_subtrack));
    if (*st == NULL) {
        fmerrmsg(where,"Memory allocation failed");
        return(-1);
    }
    numread = fread(*st,sizeof(fmio_subtrack),numtrack,f1);
    if (numread != numtrack) {
        sprintf(mymsg,"%s (%d %d)",
                "Could not read all sat_track records",
                numread,numtrack);
        fmerrmsg(where,mymsg);
        return(-1);
    }
    if (doswap) {
        for (k=0;k<numtrack;k++) {
            fm_swap_dmi((void *)&(((*st)[k]).latitude),sizeof(float),1);
            fm_swap_dmi((void *)&(((*st)[k]).longitude),sizeof(float),1);
        }
    }

    fclose(f1);
    return numtrack;
}

void fm_get_channeldata(char *filename,ahahd h, int layer, unsigned short *img,
        int doswap) {

    char *where="get_channeldata",mymsg[255];
    long i, imgsize;
    int hdsz,dtsz;
    FILE *f1;
    char buf[FMIO_STRMAXCHARS];
    signed short *tmpimg;

    imgsize = h.xsize*h.ysize;
    tmpimg = (signed short *) malloc(imgsize*sizeof(signed short));

    f1=fopen(filename,"r");
    if (!f1) {
        sprintf(mymsg,"%s %s",
                "Could not open",filename);
        fmerrmsg(where,mymsg);
    }
    fgets(buf,FMIO_STRMAXCHARS,f1);
    sscanf(buf,"AHA_METSAT %d %d",&hdsz,&dtsz);
    fseek(f1,hdsz+h.start_byte[layer],SEEK_CUR);
    fread(tmpimg,sizeof(signed short),imgsize,f1);
    fclose(f1);

    if (doswap) {
        fm_swap_dmi((void *)tmpimg,sizeof(signed short),imgsize);
    }

    for (i=0;i<imgsize;i++) {
        img[i] = (unsigned short) (tmpimg[i] + SIGN2UNSIGN);
    }

    free(tmpimg);
}

void fm_cnvtm(int data_first_year,int data_first_dayofyear, double
        data_first_secofday,unsigned short int *dd, unsigned short int *mm,
        unsigned short int *yy, unsigned short int *ho, unsigned short int *mi) {

    double jd1,jdnow,h;
    int d,m,y;

    jd1=fm_julday(1,1,data_first_year,12.0);
    jdnow=jd1+(double)data_first_dayofyear-1.0;
    fm_caldat(&d,&m,&y,&h,jdnow);
    *yy=(unsigned short int)y;
    *mm=(unsigned short int)m;
    *dd=(unsigned short int)d;
    *ho=(unsigned short int)(data_first_secofday/3600.0);
    *mi=(unsigned short int)( (data_first_secofday-(double)(*ho)*3600.0)/60);
}

int fm_readMETSATdata(char *filename, fmio_img *h) {

    char *where="fm_readdataMETSAT",mymsg[255];
    ahahd ha;
    int ch_lays[FMIO_MAXNLAY],i,k,doswap;
    char *pt,buf[FMIO_STRMAXCHARS],chr;


    /* Read the AHA header info */
    if (!(fm_fill_aha_hd(filename,&ha))) {
        sprintf(mymsg,"%s %s","Error could not read file",filename);
        fmerrmsg(where,mymsg);
        return(-1);
    }

    doswap = 0;
    if ( (fm_little_end() && ha.little_end == 'T') || /* bigend file, little_end machine */
            (!fm_little_end && ha.little_end == 'F') ) { /* fm_little_end file, bigend machine */
        doswap = 1;
    }

    /* Do the mph part */
    if (strstr(ha.satid,"metop")) {
        sprintf(h->sa, "MetOp-%s",&((ha.satid)[5]));
    } else{
        sprintf(h->sa, "NOAA-%s",&((ha.satid)[4]));
    }
    strncpy(h->area,ha.area,19);
    h->orbit_no = (int) ha.orbit_no;

    fm_cnvtm(ha.data_first_year,ha.data_first_dayofyear,ha.data_first_secofday,
            &(h->dd),&(h->mm),&(h->yy),&(h->ho),&(h->mi));

    /* Do the sph part */
    h->rga=(float)ha.vis_gain;
    h->ria=(float)ha.vis_intercept - h->rga*((float)SIGN2UNSIGN);
    h->rgt=(float)ha.ir_gain;
    h->rit=(float)ha.ir_intercept - h->rgt*((float)SIGN2UNSIGN);

    h->Bx=(float)ha.area_extent[0]*0.001;
    h->By=(float)ha.area_extent[3]*0.001;
    h->Ax=(float)ha.xscale*0.001;
    h->Ay=(float)ha.yscale*0.001;
    h->z = 0;
    for (i=0;i<ha.layers;i++) {
        if (strncmp(ha.datatypes[i],"data",4)==0) {
            strcpy(buf,ha.channel_id[i]);
            chr=buf[strlen(buf)-1];
            pt=&(buf[strlen(buf)-1]);
            if (! isdigit(chr)) {
                if (chr == 'b') k=3;
                if (chr == 'a') k=6;
            }
            else if (isdigit(chr)) k=atoi(pt);
            /* Translation from DNMI layer to aha layer numbers */
            ch_lays[k-1]=i;
            /* DNMI Channel numbers */
            h->ch[h->z]=h->z+1;
            (h->z)++;
        }
    }
    h->iw=(unsigned short int) ha.xsize;
    h->ih=(unsigned short int) ha.ysize;
    h->size=h->iw*h->ih;
    h->outofimageval = 0; /* Not robust... FIXME */

    h->numtrack=fm_get_subtrack(filename,&(h->track),doswap);

    for (k=0;k<h->z;k++) {
        h->image[k]=(unsigned short *) malloc(h->size*sizeof(unsigned short));
        if (h->image[k] == NULL) {
            fmerrmsg(where,"Allocation error in readdataMETSAT.c");
            return(-1);
        }
        fm_get_channeldata(filename,ha,ch_lays[k],h->image[k],doswap);
    }

    return(0);
}

int fm_readMETSATheader(char *filename, fmio_img *h) {

    char *where="fm_readMETSATheader",mymsg[255];
    ahahd ha;
    int ch_lays[FMIO_MAXNLAY],i,k,doswap;
    char *pt,buf[FMIO_STRMAXCHARS],chr;


    /* Read the AHA header info */
    if (!(fm_fill_aha_hd(filename,&ha))) {
        sprintf(mymsg,"%s %s",
                "Error could not read file",filename);
        fmerrmsg(where,mymsg);
        return(-1);
    }

    /*
     * First check if this is a big endian file being used on a little
     * endian machine or vice versa (first and second test in the
     * following if test respectively)
     */
    doswap = 0;
    if ((fm_little_end() && ha.little_end == 'T') || 
            (!fm_little_end && ha.little_end == 'F')) { 
        doswap = 1;
    }

    /* Do the mph part */
    if (strstr(ha.satid,"metop")) {
        sprintf(h->sa, "MetOP-%s",&((ha.satid)[5]));
    } else{
        sprintf(h->sa, "NOAA-%s",&((ha.satid)[4]));
    }
    strncpy(h->area,ha.area,19);
    h->orbit_no = (int) ha.orbit_no;

    fm_cnvtm(ha.data_first_year,ha.data_first_dayofyear,ha.data_first_secofday,
            &(h->dd),&(h->mm),&(h->yy),&(h->ho),&(h->mi));

    /* Do the sph part */
    h->rga=(float)ha.vis_gain;
    h->ria=(float)ha.vis_intercept - h->rga*((float)SIGN2UNSIGN);
    h->rgt=(float)ha.ir_gain;
    h->rit=(float)ha.ir_intercept - h->rgt*((float)SIGN2UNSIGN);

    h->Bx=(float)ha.area_extent[0]*0.001;
    h->By=(float)ha.area_extent[3]*0.001;
    h->Ax=(float)ha.xscale*0.001;
    h->Ay=(float)ha.yscale*0.001;
    h->z = 0;
    for (i=0;i<ha.layers;i++) {
        if (strncmp(ha.datatypes[i],"data",4)==0) {
            strcpy(buf,ha.channel_id[i]);
            chr=buf[strlen(buf)-1];
            pt=&(buf[strlen(buf)-1]);
            if (! isdigit(chr)) {
                if (chr == 'b') k=3;
                if (chr == 'a') k=6;
            }
            else if (isdigit(chr)) k=atoi(pt);
            /* Translation from DNMI layer to aha layer numbers */
            ch_lays[k-1]=i;
            /* DNMI Channel numbers */
            h->ch[h->z]=h->z+1;
            (h->z)++;
        }
    }
    h->iw=(unsigned short int) ha.xsize;
    h->ih=(unsigned short int) ha.ysize;
    h->size=h->iw*h->ih;

    h->numtrack=fm_get_subtrack(filename,&(h->track),doswap);

    return(0);
}

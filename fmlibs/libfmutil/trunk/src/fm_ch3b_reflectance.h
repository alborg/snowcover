/*
 * NAME:
 * fm_ch3b_reflectance.h
 *
 * PURPOSE:
 * Header file for fm_ch3b_reflectance.c
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * NA
 *
 * OUTPUT:
 * NA
 *
 * NOTES:
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Mari Anne Killie, METNO/FOU, 18.06.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 18.06.2007: Name space adjustment for use in
 * libfmutil.
 * Mari Anne Killie, METNO/FOU, 05.10.2007: Added the fm_ch3b_const
 * structure member 'solrad' and removed FM_SOLCONSTCH3B.
 * Mari Anne Killie, METNO/FOU, 22.06.2011: bugfix, satID expanded to
 * make room for \0
 * Mari Anne Killie, MET/FOU, 06.08.2013: adding planck coefficients
 * for pre AVHRR/3-sensors (FM_PLANCKCX_v0).
 *
 * ID:
 * $Id$
 */

#ifndef CH3B_H
#define CH3B_H


#define FM_PLANCKC1 1.1910427e-5 /* unit: mW/(m^2 sr cm^-4) */
#define FM_PLANCKC2 1.4387752    /* unit: cm K */
/*added when adding pre-AVHRR/3 sensors*/
#define FM_PLANCKC1_v0 1.1910659e-5 /* unit: mW/(m^2 sr cm^-4) */
#define FM_PLANCKC2_v0 1.438833     /* unit: cm K */

typedef struct { /*Channel 3b constants, uniqe for each spacecraft */
  int avhrr3;    /*1 if AVHRR/3-sensor, 0 if earlier AVHRR sensor*/
  float Aval;    /*const A, needed to calculate "effective bb temperature*/
  float Bval;    /*const B, same as above */
  float cwn;     /*central wave number in cm^-1*/
  char satID[10]; /*satellite identification*/
  float solrad;  /*solar radiance at TOA, units: mW/(m^2 sr cm^-1)*/
} fm_ch3b_const;

float fm_ch3brefl(float bt3b, float bt4, float solang, char *satname, int doy);
float fm_temp2rad(float bt, char *satname);
float fm_rad2temp(float radiance, char *satname);
float fm_findsollum(float solang, char *satname, int doy);
fm_ch3b_const fm_ch3b_identsat(char *satname);


#endif /* CH3B_H */

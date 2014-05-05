/*
 * NAME:
 * NA
 *
 * PURPOSE:
 * Functions for estimation of the channel 3b reflectance. Functions
 * contained within this file:
 * 	o fm_ch3brefl
 * 	o fm_temp2rad
 * 	o fm_rad2temp
 * 	o fm_findsollum
 * 	o fm_ch3b_identsat
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
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Mari Anne Killie, METNO/FOU, 18.06.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 18.06.2007: Name space modification for
 * implementation within libfmutil.
 * Mari Anne Killie, METNO/FOU, 13.10.2008: included fmsolar.h
 * through fmutil.h to fix problem with undefined fmesd returning
 * wrong value when used in fm_findsollum.
 * Mari Anne Killie, METNO/FOU, 30.09.2011: updated fm_ch3b_const with
 * 'solrad' values for NOAA-19 and all values for MetOP-02.
 * Mari Anne Killie, METNO/FOU, 13.02.2013: MetOP-01 added,
 * temporarily borrowing values from MetOP-02.
 *
 * ID:
 * $Id$
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <fmutil.h> 

/*
 * NAME:

 * fm_ch3brefl
 *
 * PURPOSE:
 * Computes CH3B reflectance for given temperatures in CH3B and CH4 (K),
 * solar zenith angle (degrees), satellite ID (to provide satellite
 * specific constants) and day of year (for the Earth-Sun distance)
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * o brightness temperature of channel 3b
 * o brightness temperature of channel 4
 * o solar zenith angle IN DEGREES
 * o satellite name, on the form "NOAA-XX"
 * o day of year in the range 0-364
 *
 * OUTPUT:
 * The reflectance in CH3 in percent!
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Mari Anne Killie, METNO/FOU, 18.06.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 18.06.2007: Name space modification for
 * implementation within libfmutil.
 * Mari Anne Killie, METNO/FOU, 22.10.2007: Adjustments so that the TOA
 * solar radiance depends on the instrument response function.
 *
 * ID:
 * $Id$
 */

float fm_ch3brefl(float bt3b, float bt4, float solang, char *satname,int doy) {

    float ch3brefl, ch3bthermrad;
    float lumch3b, lumsolar;

    /*
     * The measured radiance in CH3B
     */
    lumch3b= fm_temp2rad(bt3b,satname); 

    /*
     * The "real" thermal radiance from CH3B, assuming the same temperature
     * as in CH4
     */
    ch3bthermrad = fm_temp2rad(bt4,satname); 

    /*
     * Get the solar radiance
     */
    lumsolar = fm_findsollum(solang,satname,doy);

    /*
     * Estimate the reflectivity
     */
    ch3brefl = (lumch3b - ch3bthermrad)/(lumsolar - ch3bthermrad);

    /*
     * Convert value to %, the same as for the values in CH1 and CH2!
     */
    ch3brefl *= 100.; 

    return(ch3brefl); 
} 

/*
 * NAME:
 * fm_temp2rad
 *
 * PURPOSE:
 * Computes the Planck function radiance for a given temperature and 
 * satellite. Follows equations in NOAA KLM user guide.
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * o brightness temperature
 * o satellite name (NOAA-??)
 *
 * OUTPUT:
 * The Planck function radiance for given temperature, "U.G."units:
 * mW/(m^2 sr cm^-1) 
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Mari Anne Killie, METNO/FOU, 18.06.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 18.06.2007: Name space modification for
 * implementation within libfmutil.
 *
 * ID:
 * $Id$
 */ 
float fm_temp2rad(float bt, char *satname) {

    float efftemp, radiance;
    fm_ch3b_const satCs;
    float planck1, planck2;

    satCs = fm_ch3b_identsat(satname);
    if (satCs.avhrr3) {
      planck1 = FM_PLANCKC1;
      planck2 = FM_PLANCKC2;
    } else {
      planck1 = FM_PLANCKC1_v0;
      planck2 = FM_PLANCKC2_v0;
    }

    /*
     * Find the "effective brightness temperature"
     */
    efftemp =  satCs.Aval+satCs.Bval*bt; 
    radiance = powf(satCs.cwn,3.)*planck1/
        (expf(planck2*satCs.cwn/efftemp)-1.);

    return(radiance);
}

/*
 * NAME:
 * fm_rad2temp
 *
 * PURPOSE:
 * Computes the temperature that corresponds to a radiance value for the
 * Planck function, again according to equations in the NOAA KLM user
 * guide
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * o radiance
 * o satellite identification (NOAA-??)
 *
 * OUTPUT:
 * o brightness temperature [K]
 *
 * NOTES:
 * NA
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Mari Anne Killie, METNO/FOU, 18.06.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 18.06.2007: Name space modification for
 * implementation within libfmutil.
 *
 * ID:
 * $Id$
 */ 
float fm_rad2temp(float radiance,char *satname) { 

    float efftemp, temp;
    fm_ch3b_const satCs;
    float planck1, planck2;

    satCs = fm_ch3b_identsat(satname);
    if (satCs.avhrr3) {
      planck1 = FM_PLANCKC1;
      planck2 = FM_PLANCKC2;
    } else {
      planck1 = FM_PLANCKC1_v0;
      planck2 = FM_PLANCKC2_v0;
    }

    efftemp = planck2*satCs.cwn/
        logf(planck1*powf(satCs.cwn,3.)/radiance+1.);
    temp = (efftemp - satCs.Aval)/satCs.Bval;

    return(temp); 
} 

/*
 * NAME:
 * fm_findsollum
 *
 * PURPOSE:
 * Computes the solar radiance at top of atm for CH3B, L3*cos(theta)
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * o solar zenith angle (in degrees)
 * o satellite name (NOAA-??)
 * o day of year in the range 0-364
 *
 * OUTPUT:
 * o solar radiance at top of the atmosphere - L3*cos(theta) - for the
 * given satellite. Units: mW/(m^2 sr cm^-1)
 *
 * NOTES:
 * The platform-dependent values for the solar radiance are derived
 * from irradiance values found in "Model Calculations of Solar
 * Spectral Irradiance in the 3.7 micrometer Band for Earth Remote
 * Sensing Applications" by Platnick and Fontenla (submitted 2006).
 *
 * BUGS:
 * NA
 *
 * AUTHOR:
 * Mari Anne Killie, METNO/FOU, 18.06.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 18.06.2007: Name space modification for
 * implementation within libfmutil.
 * Mari Anne Killie, METNO/FOU, 22.10.2007: Minor changes made to make the
 * channel 3b solar radiance at TOA correspond to the satellite
 * response function.
 *
 * ID:
 * $Id$
 */

float fm_findsollum(float solang, char *satname, int doy) {

    float sollum, solangrad, dcorr; 
    fm_ch3b_const satCs;

    satCs = fm_ch3b_identsat(satname);
    dcorr = fmesd(doy);
    solangrad = solang*DEG_TO_RAD;
    sollum = dcorr*satCs.solrad*cosf(solangrad);  /*units: mW/(m^2 sr cm^-1)*/

    return(sollum);  
}  

/*
 * NAME:
 * fm_ch3b_identsat
 *
 * PURPOSE:
 * Identify the satellite as one of the NOAA satellites, and return a
 * structure with the constants for that particular satellite
 *
 * REQUIREMENTS:
 * NA
 *
 * INPUT:
 * o satellite identification (NOAA-??)
 *
 * OUTPUT:
 * o A structure containing constants A and B and the central wave
 * number (all taken from the NOAA KLM user guide, needed to
 * convert between brightness temperatures and radiance for the
 * channel 3b sensor at various satellite platforms), and
 * a value for the TOA solar radiance (units:
 * mW/(m^2 sr cm^-1), needed to estimate the solar contribution to
 * the measured channel 3b radiance).
 *
 * NOTES:
 * The platform specific values for the solar radiance are based on
 * irradiance values taken from table 3 of "Model Calculations of
 * Solar Spectral Irradiance in the 3.7 micrometer Band for Earth
 * Remote Sensing Applications" by Platnick and Fontenla
 * (J.App.Met.Clim. vol 47, 124). A factor of 10^(-2) is missing from
 * the table (confirmed by email correspondence with Platnick May
 * 2009), and the result is then divided by pi to be converted to
 * radiance.
 * NB: values for NOAA-19 and MetOP-02 are not in the table in the
 * reference above, but was received from Platnick on e-mail september
 * 2011.
 * NB2: MetOP-01 temporarily use the same values as MetOP-02.
 *
 * BUGS:
 * Should not exit if satellite is not found, change in time... Øystein
 * Godøy, METNO/FOU, 2013-02-12 
 *
 * AUTHOR:
 * Mari Anne Killie, METNO/FOU, 18.06.2007 
 *
 * MODIFIED:
 * Øystein Godøy, METNO/FOU, 18.06.2007: Name space modification for
 * implementation within libfmutil.
 * Mari Anne Killie, METNO/FOU, 22.10.2007: Included the structure
 * member solrad.
 * Mari Anne Killie, METNO/FOU, 03.09.2009: Added NOAA-19
 * Mari Anne Killie, METNO/FOU, 30.09.2011: updated fm_ch3b_const with
 * 'solrad' values for NOAA-19 and all values for MetOP-02.
 * Øystein Godøy, METNO/FOU, 2012-12-17: Minor edit to remove warning for
 * missing string specfication.
 * Øystein Godøy, METNO/FOU, 2013-02-12: Made satellite identification
 * case insensitive due to varying policies internally.
 * Mari Anne Killie, METNO/FOU, 13.02.2013: MetOP-01 added, at this
 * point borrowing values from MetOP-02.
 * Mari Anne Killie, MET/FOU, 17.07.2013: adding pre-AVHRR/3
 * platforms. These don't have one value for central wave number and
 * effective temperature constants (A and B). Therefore setting A to 0
 * and B to 1 (effective temperature becomes equal to
 * temperature). Solrad constants are collected from Platnik Fontenla,
 * and one of the central wavenumbers are chosen. For now, we chose
 * the value for central wave number corresponding to the interval
 * (230-270). Quick tests for NOAA-14 shows that error is ~1.5% or
 * less when using central wave number corresponding to (230-270)K for
 * temperatures in the (270-310)K range. MetOP-01 still borrows from
 * MetOP-02.
 *
 * ID:
 * $Id$
 */

fm_ch3b_const fm_ch3b_identsat(char *satname) {
    fm_ch3b_const satCs = {0,0,0,0,"",0};
    char *where="fm_ch3b_identsat";
    char what[FMSTRING256];

    sprintf(satCs.satID,"%s",satname);

    if (strcasestr(satCs.satID,"NOAA-7")){  
        satCs.avhrr3 = 0;
        satCs.Aval= 0.;
        satCs.Bval= 1.;
        satCs.cwn= 2670.3; /*using that for 225-275 (K) temp. range*/
        satCs.solrad= 5.112;
    }
    else if (strcasestr(satCs.satID,"NOAA-9")){  
        satCs.avhrr3 = 0;
	satCs.Aval= 0.;
        satCs.Bval= 1.;
        satCs.cwn= 2674.81; /*using that for 225-275 (K) temp. range*/
        satCs.solrad= 5.112;
    }
    else if (strcasestr(satCs.satID,"NOAA-10")){  
        satCs.avhrr3 = 0;
	satCs.Aval= 0.;
        satCs.Bval= 1.;
        satCs.cwn= 2657.60; /*using that for 225-275 (K) temp. range*/
        satCs.solrad= 5.112;
    }
    else if (strcasestr(satCs.satID,"NOAA-11")){  
        satCs.avhrr3 = 0;
        satCs.Aval= 0.;
        satCs.Bval= 1.;
        satCs.cwn= 2668.15; /*using that for 225-275 (K) temp. range*/
        satCs.solrad= 5.112;
    }
    else if (strcasestr(satCs.satID,"NOAA-12")){ 
        satCs.avhrr3 = 0;
        satCs.Aval= 0.;
        satCs.Bval= 1.;
	satCs.cwn= 2636.669; /*using that for 230-270 (K) temp. range*/
        satCs.solrad= 4.983;
    }
    else if (strcasestr(satCs.satID,"NOAA-14")){ 
        satCs.avhrr3 = 0;
        satCs.Aval= 0.;
        satCs.Bval= 1.;
        satCs.cwn= 2642.807; /*using that for 230-270 (K) temp. range*/
        satCs.solrad= 5.016;
    }
    else if (strcasestr(satCs.satID,"NOAA-15")){  
        satCs.avhrr3 = 1;
        satCs.Aval= 1.621256;
        satCs.Bval= 0.998015;
        satCs.cwn= 2695.9743;
        satCs.solrad= 5.153;
    }
    else if (strcasestr(satCs.satID,"NOAA-16")){ 
        satCs.avhrr3 = 1;
        satCs.Aval= 1.592459;
        satCs.Bval= 0.998147;
        satCs.cwn= 2700.1148;
        satCs.solrad= 5.099;
    }
    else if (strcasestr(satCs.satID,"NOAA-17")){ 
        satCs.avhrr3 = 1;
	satCs.Aval= 1.702380;
        satCs.Bval= 0.997378;
        satCs.cwn= 2669.3554;
        satCs.solrad= 5.070;
    }
    else if (strcasestr(satCs.satID,"NOAA-18")){ 
        satCs.avhrr3 = 1;
        satCs.Aval= 1.698704;
        satCs.Bval= 0.996960;
        satCs.cwn= 2659.7952;
        satCs.solrad= 5.043;
    }
    else if (strcasestr(satCs.satID,"NOAA-19")){ 
        satCs.avhrr3 = 1;
        satCs.Aval= 1.67396;
        satCs.Bval= 0.997364;
        satCs.cwn= 2670.0;
        satCs.solrad= 5.073;
    }
    else if (strcasestr(satCs.satID,"MetOp-01")){ 
        satCs.avhrr3 = 1;
        satCs.Aval= 2.06699;
        satCs.Bval= 0.996577;
        satCs.cwn= 2687.0;
        satCs.solrad= 5.1370;
    }
    else if (strcasestr(satCs.satID,"MetOp-02")){ 
        satCs.avhrr3 = 1;
        satCs.Aval= 2.06699;
        satCs.Bval= 0.996577;
        satCs.cwn= 2687.0;
        satCs.solrad= 5.1370;
    } else {
        fmerrmsg(where, "Satellite %s is not recognised", satCs.satID);
        exit(0); /* change behaviour in time... */
    }

    return(satCs);
}



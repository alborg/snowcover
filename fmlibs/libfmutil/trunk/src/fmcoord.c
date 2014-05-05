/*
 * NAME:
 * fmcoord.c
 *
 * PURPOSE:
 * To perform transformations between various coordinate systems (e.g.
 * latitude/longitude and Cartesian coordinates) using USGS software PROJ.
 *
 * Geographical longitude, latitude refers to a specific geoide, this
 * should be kept in mind when using the functions.
 *
 * User Coordinate System (UCS) refers to a Cartesian Coordinate system
 * with Origo at the North Pole, eastings along the X-axis and northings
 * along the Y-axis. These are given as km.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * �ystein God�y, met.no/FOU, 07.01.2005 
 *
 * MODIFIED:
 * �ystein God�y, METNO/FOU, 12.03.2007: Some minor adjustements to ensure
 * proper operation of the projection transformations.
 *
 * ID:
 * $Id$
 */

#include <fmutil.h>

#ifdef FMUTIL_HAVE_LIBPROJ
#include <projects.h>
#endif

/*
 * NAME:
 * fmgeo2ucs
 *
 * PURPOSE:
 * Computes User Coordinate Values (northings, eastings) from geographical
 * latitude, longitude.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * �ystein God�y, met.no/FOU, 10.01.2005 
 *
 * MODIFIED:
 * NA
 */
fmucspos fmgeo2ucs(fmgeopos ll, fmprojspec myproj) {

    char *where="fmgeo2ucs";
    char what[FMSTRING512];
    fmucspos xy;
    projPJ ref;
    projUV cnat;

    /*
     * Select predefined projection, and initialize interface to PROJ
     */
    if (myproj == MI) {
	if (!(ref=pj_init(sizeof(miproj)/sizeof(char *), miproj))) {
	    fprintf(stderr,"ERROR: PROJ initialization failed.\n");
	}
    } else if (myproj == MEOS) {
	if (!(ref=pj_init(sizeof(meosproj)/sizeof(char *), meosproj))) {
	    fprintf(stderr,"ERROR: PROJ initialization failed.\n");
	}
    } else {
	fprintf(stdout,"ERROR: Projection not supported\n");
    }

    /*
     * Estimate xy
     */
    cnat.u = (ll.lon)*DEG_TO_RAD;
    cnat.v = (ll.lat)*DEG_TO_RAD;
    cnat = pj_fwd(cnat,ref);
    xy.eastings = cnat.u;
    xy.northings = cnat.v;
    /*
    printf(" ll2xy (lat/lon): %.2f %.2f %.2f %.2f\n", 
	    ll.lat, ll.lon, xy.x, xy.y);
    */
    if (cnat.u == HUGE_VAL) {
	/*
	fprintf(stderr, "ERROR: pj_fwd conversion\n");
	*/
	sprintf(what,"pj_fwd failed using %.2fN %.2fE",ll.lat,ll.lon);
	fmerrmsg(where,what);

    }
    pj_free(ref);

    return(xy);
}

/*
 * NAME:
 * fmucs2geo
 *
 * PURPOSE:
 * Computes geographical latitude, longitude from UCS northings, eastings.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * �ystein God�y, met.no/FOU, 10.01.2005 
 *
 * MODIFIED:
 * NA
 */
fmgeopos fmucs2geo(fmucspos xy, fmprojspec myproj) {
    fmgeopos ll;
    projPJ ref;
    projUV cnat;

    /*
     * Select predefined projection, and initialize interface to PROJ
     */
    if (myproj == MI) {
    	if (!(ref=pj_init(sizeof(miproj)/sizeof(char *), miproj))) {
    		fprintf(stderr,"ERROR: PROJ initialization failed.\n");
    	}
    } else if (myproj == MEOS) {
    	if (!(ref=pj_init(sizeof(meosproj)/sizeof(char *), meosproj))) {
    		fprintf(stderr,"ERROR: PROJ initialization failed.\n");
    	}
    } else {
    	fprintf(stdout,"ERROR: Projection not supported\n");
    }

    /*
     * Estimate ll
     */
    cnat.u = xy.eastings;
    cnat.v = xy.northings;
    cnat = pj_inv(cnat,ref);
    ll.lon = cnat.u*RAD_TO_DEG;
    ll.lat = cnat.v*RAD_TO_DEG;
    if (cnat.u == HUGE_VAL) {
	fprintf(stderr, "ERROR: pj_inv conversion\n");
    }
    pj_free(ref);

    return(ll);
}

/*
 * NAME:
 * fmucsmeos2metno
 *
 * PURPOSE:
 * Converts KSPT MEOS UCS coordinates (elliptical earth) to NMI UCS
 * coordinates (spherical earth).
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * �ystein God�y, met.no/FOU, 10.01.2005 
 *
 * MODIFIED:
 * NA
 */
fmucspos fmucsmeos2metno(fmucspos old) {
    fmucspos new;
    projPJ ref;
    projUV cnat;

    if (!(ref=pj_init(sizeof(meosproj)/sizeof(char *), meosproj))) {
	fprintf(stderr,"ERROR: PROJ initialization failed.\n");
    }

    cnat.u = old.eastings;
    cnat.v = old.northings;
    cnat = pj_inv(cnat,ref);
    if (cnat.u == HUGE_VAL) {
	fprintf(stderr, "ERROR: pj_inv conversion\n");
    }

    pj_free(ref);
    
    if (!(ref=pj_init(sizeof(miproj)/sizeof(char *), miproj))) {
	fprintf(stderr,"ERROR: PROJ initialization failed.\n");
    }

    cnat = pj_fwd(cnat,ref);
    if (cnat.u == HUGE_VAL) {
	fprintf(stderr, "ERROR: pj_fwd conversion\n");
    }
    new.eastings = cnat.u;
    new.northings = cnat.v;

    pj_free(ref);

    return(new);
}

/*
 * NAME:
 * fmucs2ind
 *
 * PURPOSE:
 * Estimates index values of a specified UCS position within the image
 * being studied.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * �ystein God�y, met.no/FOU, 10.01.2005 
 *
 * MODIFIED:
 * NA
 */
fmindex fmucs2ind(fmucsref ref, fmucspos pos) {

    fmindex index;

    index.col = (int) rint((pos.eastings-ref.Bx)/ref.Ax);
    index.row = (int) rint((ref.By-pos.northings)/ref.Ay);

    /* 
     Test whether pixel coordinates are outside the image
     */
    if (index.col < 0 || index.row < 0 ||
	index.col > ref.iw || index.row > ref.ih) {
	index.col = -1;
	index.row = -1;
    }

    return(index);
}

/*
 * NAME:
 * fmgeo2tile
 *
 * PURPOSE:
 * Estimates which tile a given geographical position belongs to.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * �ystein God�y, met.no/FOU, 10.01.2005 
 *
 * MODIFIED:
 * NA
 */
int fmgeo2tile(fmgeopos ll) {

    char *where="fmgeo2tile";
    int i, tile = -1;
    fmucspos ne;
    fmindex ixy;
    fmucsref iref[4]={ /* these should probably be def somewhere else */
	{1.500,1.500,-335.000,-740.000,1200,1200}, /* nr */
	{1.500,1.500,-335.000,-2540.000,1200,1200}, /* ns */
	{1.500,1.500,-2135.000,-2540.000,1200,1200}, /* at */
	{1.500,1.500,-2135.000,-740.000,1200,1200} /* gr */
    };

    ne = fmgeo2ucs(ll, MI);

    for (i=0;i<4;i++) {
	ixy = fmucs2ind(iref[i], ne);
	if (ixy.col >= 0 && ixy.row >= 0 && ixy.col < iref[i].iw && ixy.row < iref[i].ih) {
	    tile = i+1;
	    break;
	}
    }

    if (tile<1 || tile>4) {
	fmerrmsg(where,
	    "Tile determination failed for UCS coordinates %.2f %.2f (%04d %04d)",
	    ne.northings,ne.eastings,ixy.col,ixy.row);
    }

    return(tile);
}

/*
 * NAME:
 * fmind2geo
 *
 * PURPOSE:
 * Computes geographical position from image indices.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * �ystein God�y, met.no/FOU, 10.01.2005 
 *
 * MODIFIED:
 * NA
 */
fmgeopos fmind2geo(fmucsref ref, fmprojspec myproj, fmindex ind) {

    fmgeopos ll;
    fmucspos xy;

    xy.eastings = ref.Bx + (((double) ind.col)*ref.Ax);
    xy.northings = ref.By - (((double) ind.row)*ref.Ay);

    ll = fmucs2geo(xy, myproj);

    return(ll);
}

/*
 * NAME:
 * fmind2ucs
 *
 * PURPOSE:
 * Computes UCS position from image indices.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 *
 * BUGS:
 *
 * AUTHOR:
 * �ystein God�y, met.no/FOU, 10.01.2005 
 *
 * MODIFIED:
 * NA
 */
fmucspos fmind2ucs(fmucsref ref, fmindex ind) {

    fmucspos xy;

    xy.eastings = ref.Bx + (((double) ind.col)*ref.Ax);
    xy.northings = ref.By - (((double) ind.row)*ref.Ay);

    return(xy);
}

/*
 * NAME:
 * fmucs2diana
 *
 * PURPOSE:
 * Computes NWP coordinate specifications from UCS.
 *
 * The UCS coordinate system is described above.
 * 
 * The NWP coordinate system is a Cartesian coordinate system with Origo
 * in the lower left corner of the image.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 * This relate sonly to Polar Stereographic map projections currently in
 * use at NMI.
 *
 * Code transferred from Fortran code written by Anstein Foss.
 *
 * BUGS:
 *
 * AUTHOR:
 * �ystein God�y, met.no/FOU, 10.01.2005 
 *
 * MODIFIED:
 * NA
 */
int fmucs2diana(fmucsref ucs, fmdianapos *nwp) {

    float rearth = 6.371e+6;
    float truelat = 60.;
    float gridrot = 0.;
    float xunit, yunit;
    float zpi, zpir18, h, fp, fq;
    float bxsat, bysat, dxsat, dysat;

    xunit = yunit = 1000.;

    nwp->dummy = 0; /* Not used for Polar Stereographic grids */
    nwp->truelat = truelat;
    nwp->gridrot = gridrot;
    nwp->inp = 0;
    nwp->jnp = 0;

    /*
     * Avoid misunderstandings caused by signs, set pixel size
     */
    dxsat = fabsf(ucs.Ax);
    dysat = fabsf(ucs.Ay);
    
    /*
     * Number of pixels between North Pole and upper left corner of image
     */
    bxsat=ucs.Bx/dxsat;
    bysat=ucs.By/dysat;

    /*
     * Estimate number of grid points between Equator and North Pole
     */
    zpi = 2.0*asin(1.0);
    zpir18 = zpi/180.;
    h = (dxsat*xunit+dysat*yunit)*0.5;
    fp = truelat;
    fq = 1.0+sin(fp*zpir18);
    nwp->ngridp = rearth*fq/h;

    /*
     * Adjust grid parameters to get x=y=1 at the lower left corner
     * (centre of pixel).
     */
    nwp->jnp = nwp->jnp-bxsat+0.5+1.;
    nwp->inp = nwp->inp-bysat-ucs.ih+0.5+1.;
    
    return(FM_OK);
}


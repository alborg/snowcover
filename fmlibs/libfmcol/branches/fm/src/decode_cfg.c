/*
 * NAME:
 * decode_cfg.c
 *
 * PURPOSE:
 * To decode the configuration file used by the software collocate.
 *
 * NOTES:
 * Standard return values 0, 2 and 3, OK, I/O and Memory trouble.
 * Lines beginning with # marks comments in configuration file.
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/12/2001
 * MODIFIED:
 * Øystein Godøy, DNMI/FOU, 04/09/2002
 * Added support for multiple DBFILES and SAF CM path etc.
 * Øystein Godøy, DNMI/FOU, 27/09/2002
 * Added support for SAF SSI path etc.
 * Øystein Godøy, DNMI/FOU, 10.02.2003
 * Added station listing and modified NWP interface.
 * Øystein Godøy, DNMI/FOU, 29.04.2003
 * Added specification of geographical position and dummy ids
 */

#include <collocate1.h>

int decode_cfg(char cfgfile[], cfgstruct *cfg) {

    FILE *fp;
    int dbi=0, cli=0, ngi=0;
    char *errmsg=" ERROR(decode_cfg): ";
    char *dummy, *dummycopy, *keyw, *pt;

    /*
     * Initialize config structure.
     */
    sprintf(cfg->avhrr_p,"NONE");
    sprintf(cfg->synop_p,"NONE");
    sprintf(cfg->nwp_p,"NONE");
    sprintf(cfg->safcm_p,"NONE");
    sprintf(cfg->safssi_p,"NONE");
    sprintf(cfg->avhrr_f,"NONE");
    sprintf(cfg->synop_f,"NONE");
    sprintf(cfg->nwp_f,"NONE");
    sprintf(cfg->safcm_f,"NONE");
    sprintf(cfg->safssi_f,"NONE");
    sprintf(cfg->lp_f,"NONE");
    cfg->synop = FALSE;
    cfg->stids = NULL;
    cfg->gpos = NULL;

    dummy = (char *) malloc(DUMMYLEN*sizeof(char));
    if (!dummy) {
	error(errmsg,"Could not allocate memory for dummy");
	return(3);
    }
    keyw = (char *) malloc(DUMMYLEN*sizeof(char));
    if (!keyw) {
	error(errmsg,"Could not allocate memory for keyw");
	return(3);
    }

    fp = fopen(cfgfile,"r");
    if (!fp) {
	error(errmsg,"Could not open config file.");
	return(2);
    }

    ngi = 0;
    while (fgets(dummy,DUMMYLEN,fp) != NULL) {
	if (strncmp(dummy,"#",1) == 0) continue;
	if (strlen(dummy) > (DUMMYLEN-50)) {
	    error(errmsg, 
		    "Input string larger than FILELEN");
	    free(dummy);
	    return(2);
	}

	/*
	 * Decode path specifications for input files.
	 */
	if (strstr(dummy,"AVHRR_P") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->avhrr_p);
	} else if (strstr(dummy,"SYNOP_P") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->synop_p);
	} else if (strstr(dummy,"NWP_P") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->nwp_p);
	} else if (strstr(dummy,"SAFCM_P") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->safcm_p);
	} else if (strstr(dummy,"SAFSSI_P") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->safssi_p);

	    /*
	     * Decode filename wildcard specifications for input files.
	     */
	} else if (strstr(dummy,"AVHRR_F") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->avhrr_f);
	} else if (strstr(dummy,"SYNOP_F") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->synop_f);
	} else if (strstr(dummy,"NWP_F") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->nwp_f);
	} else if (strstr(dummy,"SAFCM_F") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->safcm_f);
	} else if (strstr(dummy,"SAFSSI_F") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->safssi_f);
	} else if (strstr(dummy,"LPFILE") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->lp_f);
	} else if (strstr(dummy,"DBFILE") != NULL) {
	    if (dbi > DBFILES)  {
		error(errmsg,"Too many DBFILE(S)");
		return(2);
	    }
	    sscanf(dummy,"%s %s", keyw, cfg->dbfile[dbi]);
	    dbi++;

	    /*
	     * Decode class names.
	     */
	} else if (strstr(dummy,"CLASS") != NULL) {
	    if (dbi > DBFILES) {
		error(errmsg,"Too many DBFILE(S)");
		return(2);
	    }
	    sscanf(dummy,"%s %s", keyw, cfg->classname[cli]);
	    cli++;

	    /*
	     * Check if only synoptic times should be used
	     */
	} else if (strstr(dummy,"SYNOP") != NULL) {
	    if (strstr(dummy,"TRUE") != NULL) cfg->synop = TRUE;

	    /*
	     * Decode station identifications to store data from
	     */
	} else if (strstr(dummy,"STIDS") != NULL) {
	    if (cfg->stids == NULL) {
		cfg->stids = (char *) malloc(STIDSLEN*sizeof(char));
		if (cfg->stids == NULL) {
		    error(errmsg,"Allocation trouble");
		    return(2);
		}
	    }
	    /*
	    sscanf(dummy,"%s %1024s", keyw, cfg->stids);
	    pt = NULL;
	    */
	    pt = strtok(dummy," ");
	    pt = strtok(NULL,"\n");
	    /*printf(" ====> %s\n",pt);*/
	    if (pt) strcat(cfg->stids,pt);
	    if (strlen(cfg->stids) > (STIDSLEN-10)) {
		error(errmsg,"Increase STIDSLEN");
		return(2);
	    }

	    /*
	     * Decode position and identificators for dummy stations (not WMO
	     * stations)
	     */
	} else if (strstr(dummy,"GPOS") != NULL) {
	    if (ngi==0) {
		cfg->gpos = (gposs *) malloc(NGPOS*sizeof(gposs));
		if (cfg->gpos == NULL) {
		    error(errmsg,"Allocation trouble cfg->gpos");
		    return(2);
		}
	    }
	    if (ngi==NGPOS) {
		error(errmsg,"NGPOS must be increased");
		return(2);
	    }
	    sscanf(dummy,"%s %f %f %s", 
		    keyw, 
		    &(cfg->gpos[ngi].pos.lon),&(cfg->gpos[ngi].pos.lat),
		    (cfg->gpos[ngi]).id);
	    ngi++;
	    cfg->ngpos = ngi;
	}

    }

    if (dbi != cli) {
	error(errmsg,
	    "Inconsistent no of storage files and classes");
	return(2);
    }

    cfg->nocl = dbi;

    fclose(fp);

    free(dummy);
    free(keyw);

    return(0);
}

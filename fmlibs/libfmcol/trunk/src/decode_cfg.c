/*
 * NAME:
 * decode_cfg.c
 *
 * PURPOSE:
 * To decode the configuration file used by the software collocate.
 *
 * REQUIREMENTS:
 *
 * INPUT:
 *
 * OUTPUT:
 *
 * NOTES:
 * Standard return values 0, 2 and 3, OK, I/O and Memory trouble.
 * Lines beginning with # marks comments in configuration file.
 *
 * BUGS:
 * The code is sensitive to typos as strtok is used with " " as separation
 * sign. This should possibly be changed in the future...
 *
 * AUTHOR:
 * Øystein Godøy, DNMI/FOU, 21/12/2001
 *
 * MODIFIED:
 * Øystein Godøy, DNMI/FOU, 04/09/2002: Added support for multiple DBFILES
 * and SAF CM path etc.
 * Øystein Godøy, DNMI/FOU, 27/09/2002: Added support for SAF SSI path etc.
 * Øystein Godøy, DNMI/FOU, 10.02.2003: Added station listing and modified
 * NWP interface.
 * Øystein Godøy, DNMI/FOU, 29.04.2003: Added specification of
 * geographical position and dummy ids
 * Øystein Godøy, met.no/FOU, 29.10.2004:
 * Øystein Godøy, met.no/FOU, 14.12.2004: Working implementation of dummy
 * stations. Currently the memory allocation for dummy stations are
 * determined by collocate1.h NSCENES and NPOSITIONS, but this should be
 * refined in the future to ensure reallocation if necessary...
 * Furthermore, in the present configuration the check of which a
 * particular position belongs to is performed outside libfmcol, this
 * should be implemented within decode_cfg in the future...
 * Øystein Godøy, met.no/FOU, 23.12.2004: Changed data structures used for
 * dummy stations and implemented checks to identify problematic string
 * allocations.
 * Øystein Godøy, met.no/FOU, 28.12.2004: Changed keyword from GPOS to
 * DPOS for observations from DIANA, added GPOS as keyword for strictly
 * geographical positions (equals STIDS).
 * Øystein Godøy, METNO/FOU, 25.04.2007: Modified for libfmutil/libfmio.
 * Øystein Godøy, METNO/FOU, 05.10.2007: Modifications in handling of tile
 * specification for GPOS and DPOS.
 * Øystein Godøy, METNO/FOU, 24.09.2008: Bug fix on memory handling
 * concerning cfg->stids.
 *
 * CVS_ID:
 * $Id$
 */

#include <fmcol.h>
/*
#include <fmcoord.h>
*/

int decode_cfg(char cfgfile[], cfgstruct *cfg) {

    char *where="decode_cfg";
    char what[FMSTRING128];
    char *dummy, *dummycopy, *keyw, *pt, numstr[15];
    int i, j, dbi=0, cli=0, ngi=0, nsce=0, npos=0, storesceneinfo=1;
    int tile;
    float swsign;
    fmgeopos ll;
    FILE *fp;

    /*
     * Initialize config structure.
     */
    sprintf(cfg->avhrr_p,"NONE");
    sprintf(cfg->synop_p,"NONE");
    sprintf(cfg->nwp_p,"NONE");
    sprintf(cfg->safcm_p,"NONE");
    sprintf(cfg->safssi_p,"NONE");
    sprintf(cfg->safdli_p,"NONE");
    sprintf(cfg->avhrr_f,"NONE");
    sprintf(cfg->synop_f,"NONE");
    sprintf(cfg->nwp_f,"NONE");
    sprintf(cfg->safcm_f,"NONE");
    sprintf(cfg->safssi_f,"NONE");
    sprintf(cfg->safdli_f,"NONE");
    sprintf(cfg->lp_f,"NONE");
    cfg->synop = FMFALSE;
    cfg->stids = NULL;
    (cfg->did).scene = NULL;
    (cfg->did).nscenes = 0;
    (cfg->geopos).pos = NULL;
    (cfg->geopos).npos = 0;

    dummy = (char *) malloc(DUMMYLEN*sizeof(char));
    if (!dummy) {
	fmerrmsg(where,"Could not allocate memory for dummy");
	return(3);
    }
    keyw = (char *) malloc(DUMMYLEN*sizeof(char));
    if (!keyw) {
	fmerrmsg(where,"Could not allocate memory for keyw");
	return(3);
    }

    fp = fopen(cfgfile,"r");
    if (!fp) {
	fmerrmsg(where,"Could not open config file.");
	return(2);
    }

    ngi = 0;
    while (fgets(dummy,DUMMYLEN,fp) != NULL) {
	if (strncmp(dummy,"#",1) == 0) continue;
	if (strlen(dummy) > (DUMMYLEN-50)) {
	    fmerrmsg(where, 
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
	} else if (strstr(dummy,"SAFDLI_P") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->safdli_p);
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
	} else if (strstr(dummy,"SAFDLI_F") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->safdli_f);
	} else if (strstr(dummy,"LPFILE") != NULL) {
	    sscanf(dummy,"%s %s", keyw, cfg->lp_f);
	} else if (strstr(dummy,"DBFILE") != NULL) {
	    if (dbi > DBFILES)  {
		fmerrmsg(where,"Too many DBFILE(S)");
		return(2);
	    }
	    sscanf(dummy,"%s %s", keyw, cfg->dbfile[dbi]);
	    dbi++;

	} else if (strstr(dummy,"CLASS") != NULL) {
	    /*
	     * Decode class names.
	     */
	    if (dbi > DBFILES) {
		fmerrmsg(where,"Too many DBFILE(S)");
		return(2);
	    }
	    sscanf(dummy,"%s %s", keyw, cfg->classname[cli]);
	    cli++;

	} else if (strstr(dummy,"SYNOP") != NULL) {
	    /*
	     * Check if only synoptic times should be used
	     */
	    if (strstr(dummy,"TRUE") != NULL) cfg->synop = FMTRUE;
	    
	} else if (strstr(dummy,"STIDS") != NULL) {
	    /*
	     * Decode station identifications to store data from
	     */
	    if (cfg->stids == NULL) {
		cfg->stids = (char *) malloc(STIDSLEN*sizeof(char));
		if (cfg->stids == NULL) {
		    fmerrmsg(where,"Allocation trouble");
		    return(2);
		}
		cfg->stids[0]='\0';
	    }
	    pt = strtok(dummy," ");
	    pt = strtok(NULL,"\n");
	    if (pt != NULL) strcat(cfg->stids,pt);
	    if (strlen(cfg->stids) > (STIDSLEN-10)) {
		fmerrmsg(where,"Increase STIDSLEN");
		return(2);
	    }

	} else if (strstr(dummy,"GPOS") != NULL) {
	    /*
	     * Decode geographical positions
	     */
	    if ((cfg->geopos).pos == NULL && (cfg->geopos).npos == 0) {
		(cfg->geopos).pos = (gposs *) malloc(NPOSITIONS*sizeof(gposs));
		if ((cfg->geopos).pos == NULL) {
		    fmerrmsg(where,"Allocation trouble");
		    return(2);
		}
	    }

	    pt = strtok(dummy," ");
	    for (i=0;i<3;i++) {
		pt = strtok(NULL," ");
		if (i==0) {
		    strcpy((cfg->geopos).pos[(cfg->geopos).npos].desc,pt);
		} else if (i==1) {
		    (cfg->geopos).pos[(cfg->geopos).npos].pos.lat = atof(pt);
		} else if (i==2) {
		    (cfg->geopos).pos[(cfg->geopos).npos].pos.lon = atof(pt);
		}
	    }
	    /*
	     * Determine the tile of each position
	     */
	    ll.lat = (cfg->geopos).pos[(cfg->geopos).npos].pos.lat,
	    ll.lon = (cfg->geopos).pos[(cfg->geopos).npos].pos.lon,
	    tile = fmgeo2tile(ll);
	    if (tile == 1) {
		sprintf((cfg->geopos).pos[(cfg->geopos).npos].area,"nr");
	    } else if (tile == 2) {
		sprintf((cfg->geopos).pos[(cfg->geopos).npos].area,"ns");
	    } else if (tile == 3) {
		sprintf((cfg->geopos).pos[(cfg->geopos).npos].area,"at");
	    } else if (tile == 4) {
		sprintf((cfg->geopos).pos[(cfg->geopos).npos].area,"gr");
	    } else {
		fmerrmsg(where,
			"Could not identify tile properly, skips record");
		continue;
	    }

	    /*
	     * Prepare for next record
	     */
	    (cfg->geopos).npos++;

	} else if (strstr(dummy,"DPOS") != NULL) {
	    /*
	     * Decode dummy station identifications to store data from
	     *
	     * Perform the initial allocation of memory, this has to be
	     * revisited using realloc if necessary, but that is not
	     * implemented yet...
	     */
	    if ((cfg->did).scene == NULL && (cfg->did).nscenes == 0) {
		(cfg->did).scene = (scenes *) malloc(NSCENES*sizeof(scenes));
		if ((cfg->did).scene == NULL) {
		    fmerrmsg(where,"Allocation trouble");
		    return(FM_MEMALL_ERR);
		}
		for (i=0;i<NSCENES;i++) {
		    ((cfg->did).scene[i]).pos = 
			(gposs *) malloc(NPOSITIONS*sizeof(gposs));
		    if (((cfg->did).scene[i]).pos == NULL) {
			fmerrmsg(where,"Allocation trouble");
			return(FM_MEMALL_ERR);
		    }
		    ((cfg->did).scene[i]).nr = 0;
		    ((cfg->did).scene[i]).ns = 0;
		    ((cfg->did).scene[i]).gr = 0;
		    ((cfg->did).scene[i]).at = 0;
		}
	    }
	    /*
	     * Check that scene have not been stored...
	     */
	    for (i=0;i<nsce;i++) {
		if (strstr(dummy,(cfg->did).scene[i].id) != NULL) {
		    storesceneinfo = 0;
		}
	    }
	    if (storesceneinfo) {
		nsce++;
		if (nsce == NSCENES) {
		    fmerrmsg(where,"Increase NSCENES.");
		    return(3);
		}
	    }

	    /*
	     * Store scene, including satellite name
	     */
	    pt = strtok(dummy," ");
	    for (i=0;i<5;i++) {
		pt = strtok(NULL," ");
		if (storesceneinfo) {
		    if (i==0) {
			strcpy(((cfg->did).scene)[nsce-1].id,pt);
			strcpy((cfg->did).scene[nsce-1].id," ");
		    } else {
			/* This is not proof safe... */
			if (strlen((cfg->did).scene[nsce-1].id)>(FMCOLIDLEN-1)) {
			    fmerrmsg(where,
				    "Increase FMCOLIDLEN or decrease string.");
			    return(FM_IO_ERR);
			}
			strcat(((cfg->did).scene)[nsce-1].id,pt);
			strcat((cfg->did).scene[nsce-1].id," ");
		    }
		    npos = 0;
		}
	    }

	    /*
	     * Store position within scene and class
	     */
	    for (i=0;i<2;i++) {
		pt = strtok(NULL," ");
		strcpy(numstr,pt);
		for (j=0;j<strlen(pt);j++) {
		    if (isalpha(numstr[j])) {
			if (numstr[j] == 'W' || numstr[j] == 'S') {
			    swsign = -1.;
			} else {
			    swsign = 1.;
			}
			numstr[j]=' ';
		    }
		}
		if (i==0) {
		    (cfg->did).scene[nsce-1].pos[npos].pos.lat = swsign*atof(numstr);
		} else if (i==1) {
		    (cfg->did).scene[nsce-1].pos[npos].pos.lon = swsign*atof(numstr);
		}
	    }
	    pt = strtok(NULL,"\n");
	    if (strlen(pt)>(FMCOLDESCLEN-1)) {
		fmerrmsg(where,"Increase FMCOLDESCLEN or reduce text strings.");
		return(FM_IO_ERR);
	    }
	    strcpy((cfg->did).scene[nsce-1].pos[npos].desc,pt);
	    strcat((cfg->did).scene[nsce-1].pos[npos].desc,"\0");
	    /*
	     * Determine the tile of each position
	     */
	    ll.lat = (cfg->did).scene[nsce-1].pos[npos].pos.lat,
	    ll.lon = (cfg->did).scene[nsce-1].pos[npos].pos.lon,
	    tile = fmgeo2tile(ll);
	    if (tile == 1) {
		(cfg->did).scene[nsce-1].nr = 1;
		sprintf((cfg->did).scene[nsce-1].pos[npos].area,"nr");
	    } else if (tile == 2) {
		(cfg->did).scene[nsce-1].ns = 1;
		sprintf((cfg->did).scene[nsce-1].pos[npos].area,"ns");
	    } else if (tile == 3) {
		(cfg->did).scene[nsce-1].at = 1;
		sprintf((cfg->did).scene[nsce-1].pos[npos].area,"at");
	    } else if (tile == 4) {
		(cfg->did).scene[nsce-1].gr = 1;
		sprintf((cfg->did).scene[nsce-1].pos[npos].area,"gr");
	    } else {
		sprintf(what,
			"Could not properly identify tile, skips record %s and position %03.2fN %04.2fE (%d) (%d)",
			(cfg->did).scene[nsce-1].id,
			(cfg->did).scene[nsce-1].pos[npos].pos.lat,
			(cfg->did).scene[nsce-1].pos[npos].pos.lon,npos,tile);
		fmerrmsg(where, what);
		continue;
	    }
	    /*
	     * Prepare for next positions
	     */
	    npos++;
	    if (npos == NPOSITIONS) {
		fmerrmsg(where,"Increase NPOSITIONS.");
		return(3);
	    }
	    (cfg->did).scene[nsce-1].npos = npos;
	    (cfg->did).nscenes = nsce;

	    storesceneinfo = 1;
	}
    }

    if (dbi != cli) {
	fmerrmsg(where,
	    "Inconsistent no of storage files and classes");
	return(2);
    }

    cfg->nocl = dbi;

    fclose(fp);

    free(dummy);
    free(keyw);

    return(0);
}

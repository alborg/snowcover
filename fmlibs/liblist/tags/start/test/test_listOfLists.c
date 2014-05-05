/*
 * NAME:
 *    test_listOfLists.c
 *
 * PURPOSE:
 *    test if/how the list of lists works
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no/FoU, 28.05.2008
 *
 * MODIFIED:
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/dbl_list.h"

/* define our data types: 
 * - a buoyTrajectory contains a list of records;
 * - a record is a time and a position 
 * we will load lists of buoyTrajectories from a file
 */
#define BUOY_ID_LENGTH 10
#define BUOY_TREF_LENGTH 10
struct buoyTrajectory {
   char     id[BUOY_ID_LENGTH];
   char     tref[BUOY_TREF_LENGTH];
   dbl_list *records;
};

struct buoyRecord {
   size_t  time;
   double  position[2];
};

/* support functions for the buoyRecord datasets */
void printBuoyRecord(struct buoyRecord *b);
void setBuoyRecord(struct buoyRecord *b, double lat, double lon, size_t time);

/* support functions for the buoyTrajectory datasets */
void setBuoyTrajectory(struct buoyTrajectory *t,char *name,char *tref,dbl_list *oneTrajectory);
void printBuoyTrajectory(struct buoyTrajectory *t);
int  copyBuoyTrajectory(struct buoyTrajectory *to,struct buoyTrajectory *from);
void deleteBuoyTrajectory(struct buoyTrajectory *t);


int main(void) {

   char listName[] = "./data/drau_200802160600.txt";
   printf("START TEST by loading the list in %s.\n",listName);

   /* open the file */
   FILE *fp;
   if (!(fp = fopen(listName,"r"))) {
      fprintf(stderr,"ERROR (%s) cannot open file %s.\n",__func__,listName);
      exit(EXIT_FAILURE);
   }

   dbl_list *allTrajectories = dbl_list_init(sizeof(struct buoyTrajectory));
   struct buoyTrajectory buoyTraj;
   dbl_list *oneTrajectory;

   /* fill it with each line of data */
   int ret;
   size_t line = 1;
   size_t nbTrajectories = 0;
   char   previousID[BUOY_ID_LENGTH+1];
   sprintf(previousID,"none");
   char   name[BUOY_ID_LENGTH+1];
   char   timeReference[] = "NA";
   while (!feof(fp)) {


      struct buoyRecord buoyRec;

      /* read the buoyID and decide if it is a new buoy or the 
       * continuation of the trajectory */
      ret = fscanf(fp,"%s",name);
      if (ret != 1) {
	 fprintf(stderr,"WARNING (%s) Line %u is invalid. ret = %d\n",__func__,line,ret);
	 continue;
      }
      printf("New line. Name is <%s> and previous is <%s>\n",name,previousID);
      if (!strcmp(name,previousID)) {
	 /* same ID than previous: load the new record in the current list (later) */
	 printf("Same ID <%s>\n",name);
	 ;
      } else {
	 /* new ID. If previousID. Report on previous list and setup a new one */
	 printf("New ID <%s>\n",name);
	 if (!strcmp(previousID,"none")) {
	    /* first ever list. Nothing to report on */
	    printf("Start first list\n");
	    ;
	 } else {
	    /* store in the current list of trajectories */
	    setBuoyTrajectory(&buoyTraj,previousID,timeReference,oneTrajectory);
	    ret = dbl_list_addNode(allTrajectories,dbl_node_createCopyContent(allTrajectories,&buoyTraj,&copyBuoyTrajectory),LIST_POSITION_LAST);
	    deleteBuoyTrajectory(&buoyTraj);

	    printf("Just read the following trajectory: \n");
	    printBuoyTrajectory(allTrajectories->head->p->c);

	 }
	 if (nbTrajectories == 10) break;
	 oneTrajectory = dbl_list_init(sizeof(struct buoyRecord));
	 sprintf(previousID,"%s",name);
	 //nbTrajectories++;
      }

      double lat,lon;
      int    y,m,d,h,mn;
      /* read the lat,lon and time values */
      ret = fscanf(fp,"%lf%lf%d%d%d%d%d\n",&lat,&lon,&y,&m,&d,&h,&mn);
      if (ret != 7) {
	 fprintf(stderr,"WARNING (%s) Line %u. Could not load position and time information (ret=%d).\n",__func__,line,ret);
      }
      size_t time = h*100 + mn;

      setBuoyRecord(&buoyRec,lat,lon,time);

      /* add in the list */
      if (dbl_list_addNode(oneTrajectory,dbl_node_createCopyContent(oneTrajectory,&buoyRec,NULL),LIST_POSITION_LAST)) {
	 fprintf(stderr,"WARNING (%s) Did not manage to add the new node (%s).\n",__func__,"struct buoyRecord");
	 continue;
      }

      line++;

   }

   /* last list in the file */
   if (feof(fp)) {
      setBuoyTrajectory(&buoyTraj,name,timeReference,oneTrajectory);
      ret = dbl_list_addNode(allTrajectories,dbl_node_createCopyContent(allTrajectories,&buoyTraj,&copyBuoyTrajectory),LIST_POSITION_LAST);
      deleteBuoyTrajectory(&buoyTraj);
   }

   fclose(fp);
   printf("PRINT ALL TRAJECTORIES:\n");
   dbl_list_browse(allTrajectories,&printBuoyTrajectory);
   
   printf("DELETE ALL TRAJECTORIES:\n");
   dbl_list_delete(allTrajectories,&deleteBuoyTrajectory);

   exit(EXIT_SUCCESS);

}

/* support functions for the buoyRecord datasets */
void printBuoyRecord(struct buoyRecord *b) {

   double lat = b->position[0];
   double lon = b->position[1];
   
   char latc='N';if (lat < 0) latc = 'S';
   char lonc='E';if (lon < 0) lonc = 'W';

   printf("<Pos: {%05.2f%c,%06.2f%c}; Time: %u>\n",lat,latc,lon,lonc,b->time);

}
void setBuoyRecord(struct buoyRecord *b, double lat, double lon, size_t time) {
   b->position[0] = lat;
   b->position[1] = lon;
   b->time        = time;
}

/* support functions for the buoyTrajectory datasets */
void setBuoyTrajectory(struct buoyTrajectory *t,char *name,char *tref,dbl_list *oneTrajectory) {

   strcpy(t->id,name);
   strcpy(t->tref,tref);
   t->records = oneTrajectory;

}
void printBuoyTrajectory(struct buoyTrajectory *t) {
   printf("<Buoy %s. refTime is %s>\n",t->id,t->tref);
   dbl_list_browse(t->records,&printBuoyRecord);
}
int copyBuoyTrajectory(struct buoyTrajectory *to,struct buoyTrajectory *from) {

   strcpy(to->id,  from->id);
   strcpy(to->tref,from->tref);
   to->records = dbl_list_copy(from->records,NULL);
   
   if ( !(to->records) )
      return 1;
   return 0;
}

void deleteBuoyTrajectory(struct buoyTrajectory *t) {
   dbl_list_delete(t->records,NULL);
}

/*
 * NAME:
 *    test_listOfComplexStruct.c
 *
 * PURPOSE:
 *    test the handling of lists which contains complex structur'd 
 *    data types. Of particular interest are structures containing 
 *    dinamically allocated arrays.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no/FoU, 28.05.2008
 *
 * MODIFIED:
 *    Thomas Lavergne, met.no/FoU, 29.05.2008
 *       Adapt to the fact that the list now knows the size of its content.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/dbl_list.h"

#define PERSON_NAME_LENGTH  15
#define PERSON_VALUES_NBMAX 10
struct Person {
   char    name[PERSON_NAME_LENGTH];
   size_t  nbvalues;
   double  *values;
};

void printPerson(struct  Person *);
void deletePerson(struct Person *);
int  namesAreEquals(struct Person *,struct Person *);
int copyPerson(struct Person *p1,struct Person *p2);
void setPerson(struct Person *,char *,size_t,double *);
int compareNbValues(struct Person *a,struct Person *b);

int main(void) {

   char listName[] = "./data/list3.txt";
   printf("START TEST by loading the list in %s.\n",listName);

   /* open the file */
   FILE *fp;
   if (!(fp = fopen(listName,"r"))) {
      fprintf(stderr,"ERROR (%s) cannot open file %s.\n",__func__,listName);
      exit(EXIT_FAILURE);
   }

   /* create the lists */
   dbl_list *list   = dbl_list_init(sizeof(struct Person));

   /* fill it with each line of data */
   int ret;
   size_t line = 0;
   while (!feof(fp)) {

      struct Person person;

      char   name[PERSON_NAME_LENGTH];
      unsigned int nbvals;

      /* read the name and number of values (first 2 fields).
       * NOTE: we do NOT read the '\n' */
      ret = fscanf(fp,"%s%u",name,&nbvals);
      printf("Line: %u contains <%s><%u>\n",line,name,nbvals);
      if (ret != 2) {
	 fprintf(stderr,"WARNING (%s) Line %u is invalid. ret = %d\n",__func__,line,ret);
	 continue;
      }
      if (nbvals > PERSON_VALUES_NBMAX) {
	 fprintf(stderr,"WARNING (%s) Line %u. Only %u values will be read.\n",__func__,line,PERSON_VALUES_NBMAX);
	 nbvals = PERSON_VALUES_NBMAX;
      }

      /* dinamically allocate an array of values */
      double *values = malloc(sizeof(*values)*nbvals);
      if (!values) {
	 fprintf(stderr,"ERROR (%s) Failed to allocate memory.\n",__func__,line);
	 exit(EXIT_FAILURE);
      }
      /* read each value */
      size_t fields = 0;
      while (fields != nbvals) {
	 ret = fscanf(fp,"%lf",&(values[fields]));
	 if (ret != 1) {
	    fprintf(stderr,"WARNING (%s) Line %u. Could not load value #%d.\n",__func__,line,fields);
	    values[fields] = -99;
	 }
	 fields++;
      }
      fscanf(fp,"\n");line++;

      setPerson(&person,name,nbvals,values);

      /* add in the list */
      if (dbl_list_addNode(list,dbl_node_createCopyContent(list,&person,&copyPerson),LIST_POSITION_LAST)) {
	 fprintf(stderr,"WARNING (%s) Did not manage to add the new node (%s).\n",__func__,"struct Person");
	 continue;
      }

      /* free the local memory (has been copied into the list) */
      free(values);

   }
   fclose(fp);

   /* Use the list */

   /* sort the list (by number of values) */
   printf("BEFORE SORTING: \n");
   ret = dbl_list_browse(list,&printPerson);
   printf("START SORTING: \n");
   dbl_list_sort(list,&compareNbValues,&copyPerson,&deletePerson);
   printf("AFTER SORTING: \n");
   ret = dbl_list_browse(list,&printPerson);

   /* copy the list */
   printf("START COPYING: \n");
   dbl_list *copy = dbl_list_copy(list,&copyPerson);
   dbl_list_delete(list,&deletePerson);
   ret = dbl_list_browse(copy,&printPerson);

   /* Delete the list */
   dbl_list_delete(copy,&deletePerson);

   exit(EXIT_SUCCESS);

}

/* support functions for the Person datasets */
void printPerson(struct Person *p) {
   printf("<Name:%s, [",p->name);
   for (size_t f = 0 ; f < p->nbvalues ; f++) {
      printf("%+04.1f",p->values[f]);
      if (f != (p->nbvalues-1))
	 printf(", ");
   }
   printf("]\n");
}
void deletePerson(struct Person *p) {
   free(p->values);
}
int namesAreEquals(struct Person *p1,struct Person *p2) {
   return !strcmp(p1->name,p2->name);
}
void setPerson(struct Person *p1,char *name,size_t nbvalues, double *vals) {
   strcpy(p1->name,name);
   p1->nbvalues = nbvalues;
   p1->values   = vals;
}

int copyPerson(struct Person *p1,struct Person *p2) {
   strcpy(p1->name,p2->name);
   p1->nbvalues = p2->nbvalues;
   size_t size = sizeof (*(p1->values)) * p1->nbvalues;
   p1->values   = malloc( size );
   if (!(p1->values))
      return 1;
   memcpy(p1->values,p2->values,size);
   return 0;
}

int compareSumValues(struct Person *a,struct Person *b) {
   double sum1 = 0, sum2 = 0;
   for (size_t i = 0 ; i < a->nbvalues ; i++) 
      sum1 += a->values[i];
   for (size_t i = 0 ; i < b->nbvalues ; i++) 
      sum2 += b->values[i];
   if (sum1 == sum2) return 0;
   if (sum1 < sum2) return -1;
   return 1;
}
int compareNbValues(struct Person *a,struct Person *b) {
   if (a->nbvalues == b->nbvalues) return compareSumValues(a,b);
   if (a->nbvalues < b->nbvalues) return -1;
   return 1;
}

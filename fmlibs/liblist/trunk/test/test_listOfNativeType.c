/*
 * NAME:
 *    test_listOfNativeType.c
 *
 * PURPOSE:
 *    test the handling of simple lists which contains native
 *    data types (floats, strings, ...)
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no/FoU, 19.05.2008
 *
 * MODIFIED:
 *    Thomas Lavergne, met.no/FoU, 29.05.2008
 *       Adapt to the fact that the list now knows the size of its content.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../src/dbl_list.h"


#define NAME_LENGTH 20

void printString(char *str);
int stringsAreEquals(char *str1,char *str2);
void printValue(float *val);
int valuesAreEquals(float *val1,float *val2);
int valueIsNegative(float *val1);
int compareFloats(float *a,float *b);
int compareAbsFloats(float *a,float *b);
int neg_compareFloats(float *a,float *b);

int main(void) {

   liblist_running_mode = LIBLIST_VERBOSE;

   char listName[] = "./data/list1.txt";
   printf("START TEST by loading the list in %s.\n",listName);

   /* open the file */
   FILE *fp;
   if (!(fp = fopen(listName,"r"))) {
      fprintf(stderr,"ERROR (%s) cannot open file %s.\n",__func__,listName);
      exit(EXIT_FAILURE);
   }

   /* create the lists */
   dbl_list *list_names  = dbl_list_init(sizeof(char[NAME_LENGTH+1]));
   dbl_list *list_values = dbl_list_init(sizeof(float));

   /* fill it with each line of data */
   int ret;
   size_t line = 0;
   while (!feof(fp)) {
      char  name[NAME_LENGTH+1];
      float value;

      ret = fscanf(fp,"%s%f\n",name,&value);line++;
      if (ret != 2) {
	 fprintf(stderr,"WARNING (%s) Line %u is invalid.\n",__func__,line);
	 continue;
      }
      printf("Line: %u contains <%s><%f>\n",line,name,value);
      if (dbl_list_addNode(list_names,dbl_node_createCopyContent(list_names,name,NULL),LIST_POSITION_LAST)) {
	 fprintf(stderr,"WARNING (%s) Did not manage to add the new node (%s).\n",__func__,"name");
	 continue;
      }
      if (dbl_list_addNode(list_values,dbl_node_createCopyContent(list_values,&value,NULL),LIST_POSITION_LAST)) {
	 fprintf(stderr,"WARNING (%s) Did not manage to add the new node (%s).\n",__func__,"value");
	 continue;
      }
   }
   fclose(fp);
  
   /* =========================================== */
   /* Play around with the list of names (char *) */
   /* =========================================== */
   printf("START PLAYING AROUND WITH THE LIST OF STRINGS\n");
   ret = dbl_list_print(list_names,&printString);

   /* search for 'Steinar' */
   char searchString[] = "Steinar";
   dbl_node *found = dbl_list_find(list_names,NULL,&stringsAreEquals,searchString);
   if (found) {
      printf("Found %s!\n",searchString);
   } else {
      printf("Not a single %s in this list.\n",searchString);
   }
   
   /* apply a uniq filter */
   printf("Keeps only one instance of each name.\n");
   ret = dbl_list_uniq(list_names,&stringsAreEquals,NULL);
   ret = dbl_list_print(list_names,&printString);

   /* search and delete 'Roger' */
   char searchString2[20];
   sprintf(searchString2,"Roger");
   ret = dbl_list_removeNode(list_names,dbl_list_find(list_names,NULL,&stringsAreEquals,searchString2),NULL);
   if (!ret) 
      printf("Roger was removed.\n");
   /* search and delete 'Mari-Anne' */
   sprintf(searchString2,"Mari-Anne");
   ret = dbl_list_removeNode(list_names,dbl_list_find(list_names,NULL,&stringsAreEquals,searchString2),NULL);
   if (!ret) 
      printf("Mari-Anne was removed.\n");

   ret = dbl_list_print(list_names,&printString);

   /* transform the list in an array */
   size_t nbelems1;
   printf("TRANSFORM THE LIST INTO AN ARRAY: \n");
   char **array_names = dbl_list_list2array(list_names,NULL,&nbelems1);
   for (size_t e = 0 ; e < nbelems1 ; e++) {
      printf("Array element %u: <%s>\n",e,array_names[e]);
   }
   free(array_names);
   dbl_list_delete(list_names,NULL);
   
   /* delete all the others as well as the list object */
   //dbl_list_delete(list_names,NULL);


   /* ============================================ */
   /* Play around with the list of values (floats) */
   /* ============================================ */
   printf("START PLAYING AROUND WITH THE LIST OF FLOATS\n");
   ret = dbl_list_print(list_values,&printValue);
   /* search for an element which is equal to '8.3' */
   float val1 = 8.30;
   dbl_node *node = dbl_list_find(list_values,NULL,&valuesAreEquals,&val1);
   if (!node) {
      printf("Did not find a value of %f in the list.\n",val1);
   } else {
      printf("Found %f!\n",val1);
   }
   /* search for all elements which are negative */
   dbl_node *startSearch = list_values->head->p;
   printf("Search for negative values in the list.\n");
   while ( (startSearch = dbl_list_findIf(list_values,startSearch->n,&valueIsNegative)) ) {
      printf("\tFound negative value: "); printValue(startSearch->c); printf("\n");
      if (startSearch->n == list_values->head)
	 break;
   }
   /* search for all elements which are negative (and delete them) */
   printf("Search for negative values in the list (and delete them).\n");
   int nbdeleted = dbl_list_removeAllThose(list_values,NULL,&valueIsNegative);
   printf("Removed %d values which were negative.\n",nbdeleted);
  
   printf("The list is now: \n");
   ret = dbl_list_print(list_values,&printValue); 
   
   printf("Apply a uniq filter on the remaining values.\n");
   ret = dbl_list_uniq(list_values,&valuesAreEquals,NULL);
   
   printf("The list is now: \n");
   ret = dbl_list_print(list_values,&printValue);

   if (list_values->nbnodes == 0) {
      dbl_list_delete(list_values,NULL);
      exit (0);
   }
   /* sort the list */
   printf("BEFORE SORTING: \n");
   ret = dbl_list_print(list_values,&printValue);
   printf("START SORTING: \n");
   dbl_list_sort(list_values,&compareFloats,NULL,NULL);
   printf("AFTER SORTING: \n");
   ret = dbl_list_print(list_values,&printValue);

   /* copy the list */
   printf("COPY THE LIST: \n");
   dbl_list *list_values_copy = dbl_list_copy(list_values,NULL);
   dbl_list_delete(list_values,NULL);
   ret = dbl_list_print(list_values_copy,&printValue);

   /* transform the list in an array */
   size_t nbelems;
   printf("TRANSFORM THE LIST INTO AN ARRAY: \n");
   float *array_values = dbl_list_list2array(list_values_copy,NULL,&nbelems);
   dbl_list_delete(list_values_copy,NULL);
   for (size_t e = 0 ; e < nbelems ; e++) {
      printf("Array element %u: <%f>\n",e,array_values[e]);
   }
   free(array_values);


   exit(EXIT_SUCCESS);

}

/* support functions for the string datasets */
void printString(char *str) {
   printf("<%s>",str);
}
int stringsAreEquals(char *str1,char *str2) {
   return !strcmp(str1,str2);
}

/* support functions for the values (float) datasets */
void printValue(float *val) {
   printf("<%+05.2f>",*val);
}
int valuesAreEquals(float *val1,float *val2) {
   return (*val1 == *val2);
}
int valueIsNegative(float *val1) {
   return (*val1 < 0);
}

int compareFloats(float *a,float *b) {
   if (*a == *b) return 0;
   if (*a < *b)  return -1;
   return 1;
}

int compareAbsFloats(float *a,float *b) {
   if (*a == *b) return 0;
   if (fabsf(*a) < fabsf(*b)) return -1;
   return 1;
}

int neg_compareFloats(float *a,float *b) {
   return compareFloats(b,a);
}


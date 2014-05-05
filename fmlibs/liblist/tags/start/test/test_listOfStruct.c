/*
 * NAME:
 *    test_listOfStruct.c
 *
 * PURPOSE:
 *    test the handling of lists which contains structur'd 
 *    data types.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no/FoU, 19.05.2008
 *
 * MODIFIED:
 *    Thomas Lavergne, met.no/FoU, 19.05.2008  : 
 *       have the person a bit more complicated to force padding of structure
 *    Thomas Lavergne, met.no/FoU, 20.05.2008  : 
 *       build 2 lists from the file: 'Girls' and 'Boys' and test the join routine
 *    Thomas Lavergne, met.no/FoU, 29.05.2008  : 
 *       adapt to the fact the a list knows the size of its content.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/dbl_list.h"

#define PERSON_NAME_LENGTH 15
struct Person {
   char   name[PERSON_NAME_LENGTH];
   float  value;
   char   sex;
   double value2;
};

void printPerson(struct  Person *);
void assignPerson(struct Person *,struct Person *);
int namesAreEquals(struct Person *,struct Person *);
void setPerson(struct Person *,char *,float,char,double);
int  personIsGirl(struct Person *p1);
int personIsBoy(struct Person *p1);

int main(void) {

   /* check the padding state of Person: */
   struct Person p1;
   size_t sizeOfElems  = PERSON_NAME_LENGTH * sizeof(char) + sizeof(float) + sizeof(char) + sizeof(double);
   size_t sizeOfStruct = sizeof(p1);

   printf("SIZES : %u %u\n",sizeOfElems,sizeOfStruct);

   char listName[] = "./data/list2.txt";
   printf("START TEST by loading the list in %s.\n",listName);

   /* open the file */
   FILE *fp;
   if (!(fp = fopen(listName,"r"))) {
      fprintf(stderr,"ERROR (%s) cannot open file %s.\n",__func__,listName);
      exit(EXIT_FAILURE);
   }

   /* create the lists */
   dbl_list *list_boys   = dbl_list_init(sizeof(struct Person));
   dbl_list *list_girls  = dbl_list_init(sizeof(struct Person));

   /* fill it with each line of data */
   int ret;
   size_t line = 0;
   while (!feof(fp)) {

      struct Person person;

      char   name[PERSON_NAME_LENGTH];
      float  val;
      char   sex;
      double val2;
	
      ret = fscanf(fp,"%s%f %c%lf\n",name,&val,&sex,&val2);line++; // the ' ' (space) before the %c
                                                                   // must match the data file format.
      printf("Line: %u contains <%s><%f><%c><%f>\n",line,name,val,sex,val2);
      if (ret != 4) {
	 fprintf(stderr,"WARNING (%s) Line %u is invalid. ret = %d\n",__func__,line,ret);
	 continue;
      }
      setPerson(&person,name,val,sex,val2);

      /* add in the correct list */
      if (personIsGirl(&person)) {
	 if (dbl_list_addNode(list_girls,dbl_node_createCopyContent(list_girls,&person,NULL),LIST_POSITION_LAST)) {
	    fprintf(stderr,"WARNING (%s) Did not manage to add the new node (%s).\n",__func__,"struct Person");
	    continue;
	 }
      } else if (personIsBoy(&person)) {
	 if (dbl_list_addNode(list_boys,dbl_node_createCopyContent(list_boys,&person,NULL),LIST_POSITION_LAST)) {
	    fprintf(stderr,"WARNING (%s) Did not manage to add the new node (%s).\n",__func__,"struct Person");
	    continue;
	 }
      }

   }
   fclose(fp);

   /* ============================================ */
   /* Join the two lists (girls and boys) to form  */
   /* a list of persons                            */
   /* ============================================ */
   printf("LIST OF GIRLS:\n");
   ret = dbl_list_browse(list_girls,&printPerson);
   printf("LIST OF BOYS:\n");
   ret = dbl_list_browse(list_boys,&printPerson);

   printf("JOIN THE TWO IN A LIST OF PERSONS:\n");
   dbl_list *list_persons   = dbl_list_join(list_girls,list_boys);
   ret = dbl_list_browse(list_persons,&printPerson);

   /* =========================================== */
   /* Play around with the list of Persons        */
   /* =========================================== */
   printf("START PLAYING AROUND WITH THE LIST OF PERSONS\n");

   /* search for the person named 'Roger' */
   char searchName[] = "Roger";
   dbl_node *found = dbl_list_find(list_persons,&namesAreEquals,searchName); 
   if (!found) {
      printf("Did not find %s in this list.\n",searchName);
   } else {
      printf("Found %s! Full info is ",searchName);
      printPerson(found->c);
      printf("\n");
   }

   /* delete all the others as well as the list object */
   dbl_list_delete(list_persons,NULL);

   /* ============================================ */
   /* Play around with the list of values (floats) */
   /* ============================================ */

   exit(EXIT_SUCCESS);

}

/* support functions for the Person datasets */
void printPerson(struct Person *p) {
   printf("<Name:%s, Value:%+05.2f, Sex:%c, Value2:%010.7f>\n",
	 p->name,p->value,p->sex,p->value2);
}
int namesAreEquals(struct Person *p1,struct Person *p2) {
   return !strcmp(p1->name,p2->name);
}
void setPerson(struct Person *p1,char *name,float val,char sex,double val2) {
   strcpy(p1->name,name);
   p1->value  = val;
   p1->sex    = sex;
   p1->value2 = val2;
}
void assignPerson(struct Person *p1,struct Person *p2) {
   setPerson(p1,p2->name,p2->value,p2->sex,p2->value2);
}
int personIsGirl(struct Person *p1) {
   return (p1->sex == 'F');
}
int personIsBoy(struct Person *p1) {
   return (p1->sex == 'M');
}

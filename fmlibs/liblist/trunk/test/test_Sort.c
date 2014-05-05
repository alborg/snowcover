
#include <stdlib.h>
#include <stdio.h>
#include "../src/dbl_list.h"
#include "../src/tool_qsort.h"

int compare(int *a,int *b);
int neg_compare(int *a,int *b);

void printInt(int *e) {
   printf("< %02d >",*e);
}

int main ( void )
{

   int ret;

   /* initialize the list object */
   dbl_list *L = dbl_list_init(sizeof(int));

   /* add some random integers */
   size_t nbvals = 15;
   for ( size_t i = 0; i < nbvals ; i++ ) {
      int randI =  (int)( ((double)rand() / RAND_MAX) * 100 );
      dbl_list_addNode(L,dbl_node_createCopyContent(L,&randI,NULL),LIST_POSITION_LAST);
   }

   /* display the list elements */
   printf("LIST BEFORE SORTING\n");
   ret = dbl_list_print(L,&printInt);

   /* sort the list elements */
   dbl_list_sort(L,&neg_compare,NULL,NULL);

   /* display the list elements again */
   printf("LIST AFTER SORTING\n");
   ret = dbl_list_print(L,&printInt);

  return 0;
}

int compare(int *a,int *b) {
   if (*a == *b) return 0;
   if (*a < *b)  return -1;
   return 1;
}

int neg_compare(int *a,int *b) {
   return compare(b,a);
}

/*
 * NAME : 
 *   tool_qsort.c
 *
 * PURPOSE :
 *   implement the quick sort algorithm
 *
 * AUTHOR :
 *    TL, adapted from http://www.daniweb.com/code/snippet61.html
 *
 * NOTE :
 *    Only the insertion sort routine has been adapted so far. (2008-09-25).
 *
 * MODIFIED :
 *    Thomas Lavergne, met.no/FoU, 19.05.2008  :  
 *       add a sorting routine (*fsort)() in the call to quicksort so that
 *       we can specify our own sorting criteria and order.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dbl_list.h"
#include "tool_qsort.h"


//void swap ( int *a, int *b )
//{
//  int save = *a;
//  *a = *b;
//  *b = save;
//}
//
//void swapContent ( void *a, void *b, void *tmp )
//{
//  memcpy(tmp,a);
//  memcpy(a,b);
//  memcpy(b,tmp);
//}

/* 
 * int fsort(a,b) {
 *    return -1; if order between a and b is ok.
 *    return  0: if (a==b) (the order is ok)
 *    return  1: if order must be changed.
 * };
 */
void insertion_sort( dbl_list *list, dbl_node *left, dbl_node *right, int (*fsort)(void *,void *), 
      size_t sizeofcontent, void (*copy)(void *,void *), void (*delete)(void *) ) {

   /* search for the first misplaced element between left+1 and right */
   dbl_node *i = left->n;
   while ( (i != right) ) {
	 
      /* save the current i value */
      void *save = malloc(sizeofcontent);
      if (copy) 
	 (*copy)(save,i->c);
      else 
	 memcpy(save,i->c,sizeofcontent);

      /* j is supposed to go backwards from i until we reach :
       *    1) the left boundary, or
       *    2) a portion of the list that is already sorted.
       */
      dbl_node *j = i;
      while ( (j != left) && ( (*fsort)(j->p->c,save) > 0 ) ) {
	 /* j is misplaced. we will thus make point the current content
	  * to the previous' one */
	 if ( j == i ) {
	    /* we need to delete the first of these as it will be lost */
	    if (delete)
	       (*delete)(i->c);
	    free(i->c);
	 }
	 j->c = j->p->c;
	 /* go to next j on the left */
	 j = j->p;
      }
      if (i == j) {
	 /* if i==j, then i was at the beginning of a sorted section. so we
	  * allocated save for nothing and must free it. */
	 if (delete)
	    (*delete)(save);
	 free(save);
      } else {
	 j->c = save;
      }

      i = i->n;
   }

}

//int median ( void *list, int left, int right, int (*fsort)(void *,void *) )
//{
//  /* Find the median of three values in list, use it as the pivot */
//  int mid = ( left + right ) / 2;
//
//  if ( list[left] > list[mid] )
//  if ( (*fsort)(&(list[left]),&(list[mid])) > 0 )
//    swap ( &list[left], &list[mid] );
//
//  if ( (*fsort)(&(list[left]),&(list[right])) > 0 )
//    swap ( &list[left], &list[right] );
//
//  if ( (*fsort)(&(list[mid]),&(list[right])) > 0 )
//    swap ( &list[mid], &list[right] );
//
//  swap ( &list[mid], &list[right - 1] );
//
//  return list[right - 1];
//}
//
//
//struct pivots partition ( void *list, int left, int right, int (*fsort)(void *,void *) )
//{
//  int k;
//  int i = left, j = right - 1;
//  int m = left, n = right - 1;
//  int pivot = median ( list, left, right, fsort);
//  struct pivots ret;
//
//  /* Three way partition <,==,> */
//  for ( ; ; ) {
//    while ( (*fsort)(&(list[++i]),&(pivot)) < 0 )
//      ;
//    while ( (*fsort)(&(list[--j]),&(pivot)) > 0 ) {
//      if ( j == left )
//        break;
//    }
//
//    if ( i >= j )
//      break;
//
//    swap ( &list[i], &list[j] );
//
//    if ( (*fsort)(&(list[i]),&(pivot)) == 0 )
//      swap ( &list[++m], &list[i] );
//
//    if ( (*fsort)(&(list[j]),&(pivot)) == 0 )
//      swap ( &list[--n], &list[j] );
//  }
//
//  swap ( &list[i], &list[right - 1] );
//
//  j = i - 1;
//  i = i + 1;
//
//  for ( k = left; k < m; k++, j-- )
//    swap ( &list[k], &list[j] );
//
//  for ( k = right - 1; k > n; k--, i++ )
//    swap ( &list[k], &list[i] );
//
//  ret.left = i;
//  ret.right = j;
//
//  return ret;
//}
//
//
//void quicksort_r ( void *list, int left, int right, int (*fsort)(void *,void *) )
//{
//  /* Terminate on small subfiles */
//  if ( left + CUTOFF <= right ) {
//    struct pivots pivot = partition ( list, left, right , fsort);
//
//    quicksort_r ( list, left, pivot.right, fsort );
//    quicksort_r ( list, pivot.left, right, fsort );
//  }
//}
//
//void quicksort ( void *list , int left, int right, int (*fsort)(void *,void *) )
//{
//   quicksort_r ( list, left, right - 1, fsort );
//
//   /* Insertion sort on almost sorted list */
//   insertion_sort ( list, left, right, fsort );
//}
//
//


#include <stdlib.h>
#include <stdio.h>
#include "../src/dbl_list.h"
#include "../src/tool_qsort.h"

int compare(int *a,int *b);
int neg_compare(int *a,int *b);

int main ( void )
{
  int list[30];
  int i;

  for ( i = 0; i < length ( list ); i++ )
    list[i] = (int)( (double)rand() / RAND_MAX * 100 );

  for ( i = 0; i < length ( list ); i++ )
    printf ( "%-4d", list[i] );
  printf ( "\n\n" );

  quicksort ( list, 0, length ( list ) , &neg_compare);

  for ( i = 0; i < length ( list ); i++ )
    printf ( "%-4d", list[i] );
  printf ( "\n" );

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

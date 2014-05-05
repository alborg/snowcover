
#ifndef TOOL_QSORT_H
#define TOOL_QSORT_H

#define CUTOFF 10
#define length(x) ( sizeof x / sizeof *x )

typedef int (*sortSub)(void *,void *);

/*
struct pivots {
  int left, right;
};
*/

//void swap ( int *a, int *b );
void insertion_sort ( dbl_list *list,  dbl_node *left, dbl_node *right, sortSub fsort, 
      size_t sizeofcontent, void (*copy)(void *,void *), void (*delete)(void *) );
//int median ( int list[], int left, int right, sortSub fsort );
//struct pivots partition ( int list[], int left, int right, sortSub fsort );
//void quicksort_r ( int list[], int left, int right, sortSub fsort );
//void quicksort ( int list[], int left, int right, sortSub fsort );

#endif /* TOOL_QSORT_H */

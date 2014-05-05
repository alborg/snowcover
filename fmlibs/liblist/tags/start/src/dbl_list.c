/*
 * NAME : dbl_list.c
 * 
 * PURPOSE :
 *    Implement the double chained circular list. 
 *
 * AUTHOR:
 *   Thomas Lavergne, met.no/FoU, 16.05.2008
 *
 * MODIFIED:
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "dbl_list.h"
#include "tool_qsort.h"

/* global variable to control the verbose level of the library. */
int liblist_running_mode = LIBLIST_QUIET;


/* ******************************************************************* */
/*    ROUTINES PERTAINING TO INDIVIDUAL NODES IN THE LIST              */
/* ******************************************************************* */

/* Nodes (dbl_node) are defined in file 'dbl_list.h'. A model for them is:
 *
 *          ____________
 *         |            |
 *         |    NODE    |
 * prev <--+ p        n +--> next
 *         |     c      |
 *         |_____|______|
 *               |
 *            content
 *
 */

/* 
 * dbl_node_create
 *
 * Create an empty node in the list.
 *
 */
dbl_node *dbl_node_create(void) {
   dbl_node *new  = malloc(sizeof(dbl_node));
   if (!new) return NULL;
   new->n = NULL;
   new->p = NULL;
   new->c = NULL;
   return (new);
}

/* 
 * dbl_node_createAssignContent
 *
 * Create an empty node in the list then adds the content
 * by assigning the pointer. The memory for the content 
 * must then stay available for the node to later use 
 * and release it.
 *
 */
dbl_node *dbl_node_createAssignContent(void *content) {
   dbl_node *new = dbl_node_create();
   if (!new) return NULL;
   new->c = content;
   return(new);
}

/* 
 * dbl_node_createCopyContent
 *
 * Create an empty node in the list then adds the content
 * by copying the memory. The node thus has its own memory
 * for the content and the memory space passed as a parameter
 * can be modified without modifying the node's.
 *
 * A 'copy' function must be given as soon as the 'content' itself
 * contains dynamically allocated data. As long as the content is
 * with static memory only (a single scalar, a string, a structure
 * containing only static memory), the 'copy' function can be NULL
 * in which case a plain memcopy() will be used.
 *
 */
dbl_node *dbl_node_createCopyContent(dbl_list *l,void *content,copyFunc copy) {
   dbl_node *new = dbl_node_create();
   if (!new) return NULL;
   new->c = malloc(l->sizeofc);
   if (!(new->c)) return NULL;
   if (copy) {
      if ((*copy)(new->c,content))
	 return NULL;
   }
   else
      memcpy(new->c,content,l->sizeofc);
   return(new);
}


/* ******************************************************************* */
/*    ROUTINES PERTAINING TO THE LIST ITSELF (several nodes)           */
/* ******************************************************************* */

/*
 * dbl_list_init
 *
 * Inititializes a new list object. This is a mandatory
 * step before using the list and, e.g., add elements to it.
 *
 */
dbl_list *dbl_list_init(size_t sizeofc) {
   dbl_list *new = malloc(sizeof(dbl_list));
   if (!new) return NULL;
   new->nbnodes = 0;
   new->sizeofc = sizeofc;
   return new;
}

/*
 * dbl_list_addNode
 *
 * Add a node n (returned by one of the 'dbl_node_create' routines)
 * into the list l. The node is currently added *as the last element*
 * in the list (see NOTE below).
 *
 * NOTE:
 * the last parameter (pos) is currently ignored. The plan is to
 * later have the possibility to choose if we want to place the 
 * node at the first, last or at an intermediate position in the
 * list.
 *
 */
int dbl_list_addNode(dbl_list *l,dbl_node *n,short pos) {

   if (!n) {
      fprintf(stderr,"ERROR (%s) cannot add a NULL node.\n",__func__);
      return 1;
   }

   if (!(l->nbnodes)) {
      /* first element */
      l->head = n;
      n->p = n->n    = n;
   } else {
      if (l->nbnodes == 1) {
	 /* 2nd element */
         n->p = n->n = l->head;
      } else {
	 /* next elements */
	 n->p = l->head->p;
	 n->n = n->p->n;
      }
      n->p->n = n->n->p = n;
   } 
   l->nbnodes++;

   return 0;
}

/*
 * dbl_list_removeNode
 *
 * Given a list l and a pointer to a node in l, remove this node from the list.
 * A delete function must be given if the node's content itself contians dynamically 
 * allocated memory. Otherwise, the delete function can be NULL and a simple 
 * free is applied.
 *
 */
int dbl_list_removeNode(dbl_list *l,dbl_node *nd,deleteFunc delete) {

   if (!nd) {
      return 0;
   }


   if (!l->nbnodes)
      return 0;
   else if (l->nbnodes > 1) {
      nd->p->n = nd->n;
      nd->n->p = nd->p;
      if (nd == l->head) {
	 l->head = nd->n;
      }
   } 
     
   if (delete) 
      (*delete)(nd->c);
   free(nd->c);
   free(nd);
   l->nbnodes--;

   return 0;
}


/* 
 * dbl_list_findIf
 *
 * In a list l, locate a node whose content matches a condition function (itself
 * one of the function parameters). A start node can be given to indicate
 * where the search should be started. Specify NULL (or l->head) if you
 * wnat to search from the beginning of the list.
 * 
 * NOTE: 
 * whatever the value for start, the search will stop when it reaches
 * the end of the list.
 */
dbl_node *dbl_list_findIf(dbl_list *l,dbl_node *start,condFunc condition) {
   
   if (!l->nbnodes) {
      printf("This list is empty.\n");
      return NULL;
   }

   size_t nnode = 1;
   /* start position in the list */
   dbl_node *node = (start==NULL?l->head:start);
   do {
      ifverbose {
         printf("Check against node %u.\n",nnode);
      }
      if ((*condition)(node->c)) {
	 return node;
      }

      node = node->n; nnode++;
   } while (node != l->head);

   return NULL;
}

/*
 * dbl_list_find
 *
 * Another way to find a node in list l (see dbl_list_findIf function). A
 * content pointer is provided ('search') as well as a areSame function.
 * Each node's content is, in turn, compared to search object with the areSame
 * routine. If the latter returns true, then a pointer to the corresponding
 * node is returned by the dbl_list_find routine.
 *
 * NOTE:
 * Same remark and note apply as in dbl_list_findIf.
 *
 */
dbl_node *dbl_list_find(dbl_list *l,dbl_node *start,areEqualsFunc areSame, void *search) {
   
   if (!l->nbnodes) {
      printf("This list is empty.\n");
      return NULL;
   }

   size_t nnode = 1;
   dbl_node *node = (start==NULL?l->head:start);
   do {
      ifverbose {
	 printf("Check against node %u.\n",nnode);
      }
      if ((*areSame)(
	       node->c,
	       search)) {
	 return node;
      }

      node = node->n; nnode++;
   } while (node != l->head);

   return NULL;
}


/*
 * dbl_list_browse
 *
 * Simplest way to act on all elements of a list
 * by running a routine 'void doit(content)' on
 * each node, from the first to the last.
 *
 */
int dbl_list_browse(dbl_list *l,doitFunc doit) {

   if (!l->nbnodes) {
      printf("This list is empty.\n");
      return 1;
   }

   size_t nnode = 1;
   dbl_node *node = l->head;
   do {
      if (doit) 
	 (*doit)(node->c);

      node = node->n; nnode++;
   } while (node != l->head);

   return 0;
}

/*
 * dbl_list_print
 *
 * A 'void print(content)' routine is applied
 * on each node, from the first to the last.
 *
 */
int dbl_list_print(dbl_list *l,doitFunc print) {

   if (!l->nbnodes) {
      printf("This list is empty.\n");
      return 1;
   }

   ifverbose {
      printf("This list has %u nodes.",l->nbnodes);
      printf (" Head is %p",l->head);
      printf("\n");
   }
   
   size_t nnode = 1;
   dbl_node *node = l->head;
   do {
      printf("Node %03u.\t",nnode);
      if (print) {
	 ifverbose
	    printf(" content: %p. ",node->c);
	 (*print)(node->c);
      }
      ifverbose
	 printf("Next is %p",node->n);

      printf("\n");

      node = node->n; nnode++;
   } while (node != l->head);

   return 0;
}

/*
 * dbl_list_delete
 *
 * Remove all the nodes in a list (and then delete the list object 
 * itself). A 'delete' routine should be provided if the node's
 * content itself contains dynamically allocated memory. Use NULL
 * if not the case (and a simple call to 'free' will be issued).
 *
 */
int dbl_list_delete(dbl_list *l,deleteFunc delete) {

   if (l->nbnodes) {
      size_t nnode = 1;
      dbl_node *node = l->head;
      do {
	 ifverbose
	    printf("Delete one element (out of %u)\n",l->nbnodes);
	 node = node->n;
	 dbl_list_removeNode(l,node->p,delete);
	 nnode++;
      } while (l->nbnodes);
   }
   free(l);

}

/*
 * dbl_list_copy
 *
 * Return a copy of a list object. A 'copy' routine should be provided 
 * if the node's content itself contains dynamically allocated memory. 
 * Use NULL if not the case (and a simple call to 'memcpy' will be issued).
 *
 */

dbl_list *dbl_list_copy(dbl_list *from,copyFunc copy) {
   
   int ret;
   
   if (!from->nbnodes) {
      printf("This list is empty.\n");
      return NULL;
   }
   dbl_list *new = dbl_list_init(from->sizeofc);

   size_t nnode = 1;
   dbl_node *node = from->head;
   do {
      if (dbl_list_addNode(new,dbl_node_createCopyContent(new,node->c,copy),0)) {
	 fprintf(stderr,"ERROR (%s) could not copy element %u.\n",nnode);
	 return NULL;
      }

      node = node->n; nnode++;
   } while (node != from->head);

   return new;
}

/*
 * dbl_list_sort
 *
 * Sort the nodes of a list according to a 'compare' ranking function. Because
 * some copying and deleting are involved, a 'copy' and a 'delete' routines are
 * to be provided in the case where the node's content itself contains dynamically 
 * allocated memory. Use NULL for both otherwise.
 *
 * NOTE:
 * + The 'compare' routine has prototype "int compare(content1, content2)" and returns
 *   -1 (+1) if the node having content1 must be placed before (after) the node with 
 *   content2. 0 can be returned if both nodes are not ordered w.r.t. to 'compare'.
 * 
 * + The algorithm used is only an 'insertion' sort. A quicksort should be made available
 *   if large lists are to be processed.
 *
 */
int dbl_list_sort(dbl_list *l,rankingFunc compare,copyFunc copy, deleteFunc delete) {

   if (!l->nbnodes) {
      printf("This list is empty.\n");
      return 1;
   } else if (l->nbnodes == 1) {
      return 0;
   } 
   
   insertion_sort(l,l->head,l->head,compare,l->sizeofc,copy,delete);
   return 0;
}

/* 
 * dbl_list_uniq
 *
 * Remove multiple occurence of identic nodes. The comparison function 
 * is provided by the user, as well as a delete routine (if the content
 * of the nodes uses dynamically allocated memory).
 *
 */
int dbl_list_uniq(dbl_list *l,areEqualsFunc areEquals, deleteFunc delete) {

   int ret;

   if (!l->nbnodes) {
      printf("This list is empty.\n");
      return 1;
   } else if (l->nbnodes == 1) {
      return 0;
   } 

   dbl_node *start = l->head;
   do {

      /* go to the end of the list and delete elements
       * which are similar to the current node. */
      dbl_node *startSearch = start;
      while ( (startSearch = dbl_list_find(l,startSearch->n,areEquals,start->c) ) ) {
	 dbl_node *tbd = startSearch;
	 startSearch = tbd->p;
	 short continueSearch = ( tbd == l->head );
	 ret = dbl_list_removeNode(l,tbd,delete); 
	 if (l->nbnodes == 0)
	    break;
	 else if (continueSearch) 
	    continue;
	 else if ( startSearch->n == l->head ) 
	    break;
      }

      /* only go to the next node if it was not already removed */
      start = start->n;

   } while (start != l->head->p); /* the last element cannot be the start for duplicates */

   return 0;
}

/* 
 * dbl_list_removeAllThose
 *
 * Remove all nodes which pass the provided 'condition' routine. A delete routine 
 * should also be given if the content of the nodes uses dynamically allocated 
 * memory. Specify NULL otherwise.
 *
 */
int dbl_list_removeAllThose(dbl_list *l,deleteFunc delete,condFunc condition) {

   int ret;
   int nbdeleted = 0;

   dbl_node *startSearch = l->head->p;
   while ( (startSearch = dbl_list_findIf(l,startSearch->n,condition) ) ) {
      dbl_node *tbd = startSearch;
      startSearch = tbd->p;
      short continueSearch = ( tbd == l->head );
      ret = dbl_list_removeNode(l,tbd,delete); nbdeleted++;
      if (l->nbnodes == 0)
	 break;
      else if (continueSearch) 
	 continue;
      else if ( startSearch->n == l->head ) 
	 break;
   }

   return nbdeleted;

}

/* ******************************************************************* */
/*    ROUTINES PERTAINING TO SEVERAL LISTS                             */
/* ******************************************************************* */

/*
 * dbl_list_join
 *
 * Join 2 lists so that the first node of l2 comes right after the 
 * last node of l1.
 *
 */
dbl_list *dbl_list_join(dbl_list *l1,dbl_list *l2) {
   /* sanity check: we cannot join lists if they
    * do not have the same sizeof(content) */
   if (l1->sizeofc != l2->sizeofc) {
      fprintf(stderr,"ERROR (%s) the two lists are not containing the same thing. Cannot join.",__func__);
      return NULL;
   }

   dbl_node *lastOfL1 = l1->head->p;
   dbl_node *lastOfL2 = l2->head->p;

   lastOfL1->n    = l2->head;
   lastOfL1->n->p = lastOfL1;
   lastOfL2->n    = l1->head;
   lastOfL2->n->p = lastOfL2;

   l1->nbnodes += l2->nbnodes;
   free(l2);

   return l1;
}

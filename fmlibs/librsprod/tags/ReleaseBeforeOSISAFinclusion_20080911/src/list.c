/* 
 * NAME: list.c
 *
 * PURPOSE:
 *    Implement a generic single-pointed list.
 *
 * NOTE: 
 *    Should be re-implemented from scratch to offer better flexibility
 *    (and remove the use of an 'empty' base elements as is currently 
 *    the case).
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008   (adapted from, see below)
 *   
 * MODIFIED:
 *    
 */

/*
 *  Adapted from:
 *  FILE   : list.c
 *  AUTHOR : Jeffrey Hunter
 *  WEB    : http://www.iDevelopment.info
 *  NOTES  : Implement all functions required
 *           for a Linked List data structure.
 */

#include <stdlib.h>
#include <stdio.h>
#include "list.h"

/* Define Linked List Node */
struct Node {
  ElementType Element;
  Position    Next;
};

void deleteNode(PtrToNode node,void (*delete)(ElementType)) {
  (*delete)(Retrieve(node));
  free(node);
}

List MakeEmpty(List L) {

  L = malloc( sizeof(struct Node) );
  if (L == NULL) {
    fprintf(stderr,"MakeEmpty Error: Unable to allocate more memory.\n");
    exit(1);
  }

  L->Next = NULL;
  L->Element = NULL;
  return L;

}


/* Return "true" if L is empty */
int IsEmpty(List L) {
  return L->Next == NULL;
}

/*
 * Return "true" if P is the last position in list L.
 * (Parameter L is unused in this implementation)
 */
int IsLast(Position P, List L) {
  return P->Next == NULL;
}

/* Return Position of X in L; NULL if not found */
Position Find(ElementType X, List L,int (*compare)(ElementType,ElementType)) {

  Position P;

  P = L->Next;
  while(P != NULL && (*compare)(P->Element,X)) {
    P = P->Next;
  }
  return P;
}

/*
 * Delete a node from the passed in list L.
 * Cell pointed to by P->Next is deleted.
 * This implementation assumes that the position
 * is legal.
 * This implementation also assumes the use
 * of a header (dummy) node.
 */
void Delete( Position Q, List L, void (*delete)(ElementType)) {

  Position P, TmpCell;

  P = FindPrevious(Q, L);

  /* Assumption use of a header node */
  if (!IsLast(P, L)) {

    /* Position (X) is found; delete it */
    TmpCell = P->Next;

    /* Bypass deleted cell */
    P->Next = TmpCell->Next;
    deleteNode(TmpCell,delete);

  }

}

void PutInPlace( Position Q, ElementType E, List L, void (*delete)(ElementType)) {

   (*delete)(Retrieve(Q));
   Q->Element = E;

}

/*
 * If X is not found, then Next field of returned
 * value is NULL
 * This implementation assumes the use of
 * a header node.
 */
Position FindPrevious( Position Q, List L ) {

  Position P = Header(L);
  do {
     P = Advance(P);
     if (P->Next == Q) return P;
  } while (!IsLast(P, L));

  return P;
}

/*
 * Insert (after legal position P)
 * This implementation assumes the use of
 * a header node.
 */
void Insert( ElementType X, List L, Position P ) {

  Position TmpCell;

  TmpCell = malloc( sizeof( struct Node ) );
  if (TmpCell == NULL) {
    fprintf(stderr,"Out of space!!!\n");
    exit(1);
  }

  TmpCell->Element = X;
  TmpCell->Next = P->Next;
  P->Next = TmpCell;

}


/* Correct DeleteList algorithm */
void DeleteList(List L,void (*delete)(ElementType)) {

  Position P, Tmp;

  /* Header assumed */
  P = L->Next;
  L->Next = NULL;
  while(P != NULL) {
    Tmp = P->Next;
    deleteNode(P,delete);
    P = Tmp;
  }
  free(L);

}

Position Header(List L) {
  return L;
}

Position First(List L) {
  return L->Next;
}

Position Last(List L) {
   Position P = Header(L);
   while (!IsLast(P,L)) {
      P = Advance(P);
   }
   return P;
}

Position Advance(Position P) {
  return P->Next;
}

ElementType Retrieve(Position P) {
  return P->Element;
}

void BrowseList(const List L,void (*action)(ElementType)) {
   Position P = Header(L);
   
   if ( !IsEmpty(L) ) {
      do {
	 P = Advance(P);
	 (*action)(Retrieve(P));
      } while (!IsLast(P, L));
   }
}
   

int copyList(const List orig,List *dest, ElementType (*fcopy)(ElementType)) {

   List L = MakeEmpty(NULL);
   *dest = L;

   if ( !IsEmpty(orig) ) {

      Position P,Q;
      P = Header(orig);
      Q = Header(L);
      ElementType copy;

      do {

	 P = Advance(P);
	 copy = (*fcopy)(Retrieve(P));
	 if (copy == NULL) {
	    fprintf(stderr,"ERROR (copyList) cannot copy list element.\n");
	    return 1;
	 }
	 Insert(copy,L,Q);
	 Q = Advance(Q);
      } while (!IsLast(P, orig));
   
   }

   return 0;

}

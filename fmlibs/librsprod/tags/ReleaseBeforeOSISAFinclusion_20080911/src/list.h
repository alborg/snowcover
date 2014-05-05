/*
 *  FILE   : list.h
 *  AUTHOR : Jeffrey Hunter
 *  WEB    : http://www.iDevelopment.info
 *  NOTES  : Define Linked List record structure and
 *           all forward declarations.
 */

typedef void* ElementType;

#ifndef RSPROD_LIST_H
#define RSPROD_LIST_H

struct Node;
typedef struct Node *PtrToNode;
typedef PtrToNode List;
typedef PtrToNode Position;

List        MakeEmpty(List L);
int         IsEmpty(List L);
int         IsLast(Position P, List L);
Position    Find(ElementType X, List L,int (*compare)(ElementType,ElementType));
void        Delete(Position Q, List L, void (*delete)(ElementType));
void        PutInPlace(Position Q, ElementType E, List L, void (*delete)(ElementType));
Position    FindPrevious(Position Q, List L);
void        Insert(ElementType X, List L, Position P);
void        DeleteList(List L,void (*delete)(ElementType));
Position    Header(List L);
Position    First(List L);
Position    Last(List L);
Position    Advance(Position P);
ElementType Retrieve(Position P);
void        BrowseList(List L,void (*action)(ElementType));
int         copyList(const List orig,List *dest, ElementType (*fcopy)(ElementType));

#endif /* RSPROD_LIST_H */

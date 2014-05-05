/*
 * NAME: listattributes.c
 * 
 * PURPOSE: 
 *    Implement the 'list of attributes' object.
 *
 * DESCRIPTION:
 *    An rsprod_attributes is a list of rsprod_attr (as implemented in file attributes.c).
 *
 * NOTE:
 *    Only 'list of attributes' are implemented here. For 'single' attributes refer to 
 *    file 'attributes.c'.
 *    
 * DEPENDENCIES:
 *    The 'list' low-level implementation is taken from file 'list.c'.
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    Thomas Lavergne, met.no, 21.10.2010   :   Move from the local 'list' implementation to using external liblist.
 *
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include "rsprod_string_utils.h"
#include "rsprod_report_utils.h"
#include "rsprod_intern.h"

/* 
 * PURPOSE: Go through the list of attributes and 
 *    display some information on each of them.
 */
//void rsprod_attributes_printInfo(rsprod_attributes this) {
//   fprintf(stderr,"Attributes:\n");
//   BrowseList(this,&rsprod_attr_printInfo);
//}
void rsprod_attributes_printInfo(rsprod_attributes *this) {
   dbl_list_print(this,&rsprod_attr_printInfo);
}

//void rsprod_attributes_printDetails(rsprod_attributes this) {
//   fprintf(stderr,"Attributes:\n");
//   BrowseList(this,&rsprod_attr_printDetails);
//}
void rsprod_attributes_printNcdump(rsprod_attributes *this) {
   dbl_list_print(this,&rsprod_attr_printNcdump);
}

/* 
 * PURPOSE: Go through the list of attributes and 
 *    change the names (upper case, lower case, rename, etc...).
 */
int rsprod_attributes_modifyNames(rsprod_attributes *this) {
   rsprod_echo(stdout,"In %s: Modify names of all attributes in the list\n",__func__);
   return dbl_list_browseAndCheck(this,&rsprod_attr_modifyName,LIST_CHECK_KEEPMAX);
}

/*
 * NAME: rsprod_attributes_getAttr()
 *
 * PURPOSE: 
 *    Find (in the current list) the attribute whose name is matching a
 *    user-provided string and return a pointer to it.
 *
 * INPUT: 
 *    o this: list of attributes;
 *    o attrname: the name of the attribute we search for.
 * OUTPUT:
 *    o attr_p : at exit: object is pointing to the attribute whose name is attrname.
 *               (if found).
 * RETURN VALUE:
 *    0 if ok;
 *    1 if could not find the name.
 */
//int rsprod_attributes_getAttr(rsprod_attributes this, rsprod_attr **attr_p, char *attrname) {
//   Position P;
//   rsprod_attr attr;
//   if (rsprod_string_trim_allNullsExceptLast(attrname,&(attr.content.name))) {
//      fprintf(stderr,"ERROR (rsprod_attributes_getAttr) Problem dealing with null characters in name <%s>\n",attrname);
//      return 1;
//   }
//   P = Find(&attr,this,&rsprod_attr_compareName);
//   if (P == NULL) {
//      return 1;
//   }
//   *attr_p = Retrieve(P);
//   return 0;
//}
int rsprod_attributes_getAttr(rsprod_attributes *this, rsprod_attr **attr_p, char *attrname) {
   /* create an attribute object and set its name to 'attrname' */
   rsprod_attr attr;
   if (rsprod_string_trim_allNullsExceptLast(attrname,&(attr.content.name))) {
      fprintf(stderr,"ERROR (%s) Problem dealing with null characters in name <%s>\n",__func__,attrname);
      return 1;
   }
   /* search in the list for an other attribute whose name will compare the locally created one */
   dbl_node *node = dbl_list_find(this,NULL,&rsprod_attr_compareName,&attr);
   /* act on return value */
   if (node == NULL) 
      return 1;
   *attr_p = node->c;
   return 0;
}

/* 
 * DEFINE AND EXPAND SOME ROUTINES FROM MACROS
 */

/* NAME: 
 *    rsprod_attributes_accessValues_Double(),
 *    rsprod_attributes_accessValues_Float(),
 *    etc...
 *
 *    (note the plural: 'Values').
 *    
 * PURPOSE:
 *    Return a casted pointer to the value of the attribute whose name is 'attrname'. 
 *    This routine is mainly used to access 'multiple valued' attributes (those created
 *    via:
 *       rsprod_attr_createWith[Copy|Assign]Values(&attr,someType,N,values));
 *    with N>1.
 *
 *    
 * PROTOTYPE for 'double': 
 *    int rsprod_attributes_accessValues_double(rsprod_attributes this, char *attrname, double **casted);
 *
 * NOTE:
 *    o Some type-mismatch tests are embedded: 
 *      a) it is (for example) illegal (and sends return value 1) to use rsprod_attributes_accessValues_double()
 *         to acess an attribute whose type is short.
 *      b) it is *not* illegal to use (for example) rsprod_attributes_accessValues_short() with 
 *         parameter 'casted' being a pointer to float variable. The user is responsible for
 *         that when he writes his software.
 *    o Mind the scope of the rsprod_attributes object: casted is only *pointing* to its value field.
 *
 * EXAMPLE:
 *    // listattr is defined earlier
 *    float params[4] ;
 *    ret = rsprod_attributes_accessValues_Float(listattr,"someFloatParams",&params);
 *    if (ret) { ERROR-HANDLING};
 *    for (int i = 0 ; i < 4 ; i ++) {
 *       printf("[%d] = %f\n",i,params[i]);
 *    }
 */

//#define DefineAccessValuesFunction(t) \
//   int rsprod_attributes_accessValues_ ## t (rsprod_attributes this, char *attrname, t **casted) { \
//      rsprod_attr *attr; \
//      if (rsprod_attributes_getAttr(this,&attr,attrname)) { \
//	 fprintf(stderr,"ERROR (rsprod_attributes_accessValues_" #t ") cannot find attribute <%s>.\n",attrname); \
//         return 1; \
//      } \
//      if ((*(attr->methods->accessValues))(attr,casted)) { \
//         fprintf(stderr,"ERROR (rsprod_attributes_accessValues_" #t ") type mismatch for attribute <%s>.\n",attrname); \
//	 return 1; \
//      } \
//      return 0; \
//   }
//
//DefineAccessValuesFunction(Double)  // rsprod_attributes_accessValues_Double
//DefineAccessValuesFunction(Float)   // rsprod_attributes_accessValues_Float
//DefineAccessValuesFunction(Short)   // rsprod_attributes_accessValues_Short
//DefineAccessValuesFunction(Int)     // rsprod_attributes_accessValues_Int
//DefineAccessValuesFunction(Char)    // rsprod_attributes_accessValues_Char
//DefineAccessValuesFunction(Byte)    // rsprod_attributes_accessValues_Byte
//#undef DefineAccessValuesFunction

#define DefineAccessValuesFunction(t) \
   int rsprod_attributes_accessValues_ ## t (rsprod_attributes *this, char *attrname, t **casted) { \
      rsprod_attr *attr; \
      if (rsprod_attributes_getAttr(this,&attr,attrname)) { \
	 fprintf(stderr,"ERROR (%s) cannot find attribute <%s>.\n",__func__,attrname); \
         return 1; \
      } \
      if ((*(attr->methods->accessValues))(attr,casted)) { \
         fprintf(stderr,"ERROR (%s) type mismatch for attribute <%s>.\n",__func__,attrname); \
         return 1; \
      } \
      return 0; \
   }
DefineAccessValuesFunction(Double)  // rsprod_attributes_accessValues_Double
DefineAccessValuesFunction(Float)   // rsprod_attributes_accessValues_Float
DefineAccessValuesFunction(Short)   // rsprod_attributes_accessValues_Short
DefineAccessValuesFunction(Int)     // rsprod_attributes_accessValues_Int
DefineAccessValuesFunction(Char)    // rsprod_attributes_accessValues_Char
DefineAccessValuesFunction(Byte)    // rsprod_attributes_accessValues_Byte
#undef DefineAccessValuesFunction

/* NAME: 
 *    rsprod_attributes_accessValue_Double(),
 *    rsprod_attributes_accessValue_Float(),
 *    etc...
 *
 *    (note the singular: 'Value').
 *    
 * PURPOSE:
 *    Same as the rsprod_attributes_accessValues_type() functions see above except that:
 *       only the first (and sometimes only) element of the attribute's value is *copied*
 *       to the parameter 'casted'.
 *
 *    This routine is mainly used to access 'single valued' attributes (those created
 *    via:
 *       rsprod_attr_createWith[Copy|Assign]Values(&attr,someType,1,value));
 *
 *    
 * PROTOTYPE for 'double': 
 *    int rsprod_attributes_accessValue_double(rsprod_attributes this, char *attrname, double *casted);
 *
 * NOTE:
 *    o Same as rsprod_attributes_accessValues_() for the type-mismatch tests.
 *    o The returned 'casted' parameter is a copy of the single/first value in the attribute.
 *
 * EXAMPLE:
 *    // listattr is defined earlier
 *    int fillvali;
 *    ret = rsprod_attributes_accessValues_int(listattr,"_FillValue",&fillvali);
 *    printf("_FillValue is %d\n",fillvali);
 */

//#define DefineAccessValueFunction(t) \
//   int rsprod_attributes_accessValue_ ## t (rsprod_attributes this, char *attrname, t *casted) { \
//      rsprod_attr *attr; \
//      if (rsprod_attributes_getAttr(this,&attr,attrname)) { \
//	 fprintf(stderr,"ERROR (rsprod_attributes_accessValue_" #t ") cannot find attribute <%s>.\n",attrname); \
//         return 1; \
//      } \
//      if ((*(attr->methods->accessValue))(attr,0,casted)) { \
//         fprintf(stderr,"ERROR (rsprod_attributes_accessValue_" #t ") type mismatch for attribute <%s>.\n",attrname); \
//	 return 1; \
//      } \
//      return 0; \
//   }
//
//DefineAccessValueFunction(Double) // rsprod_attributes_accessValue_Double
//DefineAccessValueFunction(Float)  // rsprod_attributes_accessValue_Float
//DefineAccessValueFunction(Short)  // rsprod_attributes_accessValue_Short
//DefineAccessValueFunction(Int)    // rsprod_attributes_accessValue_Int
//DefineAccessValueFunction(Char)   // rsprod_attributes_accessValue_Char
//DefineAccessValueFunction(Byte)   // rsprod_attributes_accessValue_Byte
//#undef DefineAccessValueFunction

#define DefineAccessValueFunction(t) \
   int rsprod_attributes_accessValue_ ## t (rsprod_attributes *this, char *attrname, t *casted) { \
      rsprod_attr *attr; \
      if (rsprod_attributes_getAttr(this,&attr,attrname)) { \
	 fprintf(stderr,"ERROR (%s) cannot find attribute <%s>.\n",__func__,attrname); \
         return 1; \
      } \
      if ((*(attr->methods->accessValue))(attr,0,casted)) { \
         fprintf(stderr,"ERROR (%s) type mismatch for attribute <%s>.\n",__func__,attrname); \
        return 1; \
      } \
      return 0; \
   }

DefineAccessValueFunction(Double) // rsprod_attributes_accessValue_Double
DefineAccessValueFunction(Float)  // rsprod_attributes_accessValue_Float
DefineAccessValueFunction(Short)  // rsprod_attributes_accessValue_Short
DefineAccessValueFunction(Int)    // rsprod_attributes_accessValue_Int
DefineAccessValueFunction(Char)   // rsprod_attributes_accessValue_Char
DefineAccessValueFunction(Byte)   // rsprod_attributes_accessValue_Byte
#undef DefineAccessValueFunction

/* 
 * PURPOSE : add an attribute to the list. No copying.
 */
//int rsprod_attributes_addAttr(rsprod_attributes this, rsprod_attr *attr) {
//   Position P = Last(this);
//   Insert(attr,this,P);
//   return 0;
//}
int rsprod_attributes_addAttr(rsprod_attributes *this, rsprod_attr *attr) {
   /* create a node */
   dbl_node *new = dbl_node_create();
   if (!new) return 1;
   /* link the content: that is not a copy */
   new->c = attr;
   /* add the new node into the list. */
   if (dbl_list_addNode(this,new,LIST_POSITION_LAST)) return 1;
   return 0;
}

/* 
 * PURPOSE : add an attribute to the list. With copy.
 */
//int rsprod_attributes_addCopyAttr(rsprod_attributes this, rsprod_attr *attr) {
//   Position P = Last(this);
//   Insert(rsprod_attr_copy(attr),this,P);
//   return 0;
//}
int rsprod_attributes_addCopyAttr(rsprod_attributes *this, rsprod_attr *attr) {
   /* create a node */
   dbl_node *new = dbl_node_create();
   if (!new) return 1;
   /* copy the attribute to the 'content' of the new node (new memory allocated) */
   new->c = rsprod_attr_copy(attr);
   if (!(new->c)) return 1;
   /* add the new node into the list. */
   if (dbl_list_addNode(this,new,LIST_POSITION_LAST)) return 1;
   return 0;
}

/* 
 * PURPOSE : replace in the list, the attribute whose name is
 *     matching the one given as a 2nd parameter.
 *
 *     The original attribute is deleted and the new
 *     one is copied at the same position.
 *
 *     If their is no equivalent attribute in the list, we add the new one.
 *
 */
//int rsprod_attributes_replaceAttr(rsprod_attributes this,rsprod_attr *attr) {
//   Position P;
//   rsprod_attr nattr;
//   if (rsprod_string_trim_allNullsExceptLast(rsprod_attr_getName(attr),&(nattr.content.name))) {
//      fprintf(stderr,"ERROR (rsprod_attributes_replaceAttr) Problem dealing with null characters in name <%s>\n",
//	    rsprod_attr_getName(attr));
//      return 1;
//   }
//   P = Find(&nattr,this,&rsprod_attr_compareName);
//   if (P == NULL) {
//      /* no attribute with this name in the list: add a new one */
//      rsprod_attributes_addCopyAttr(this,attr);
//   } else {
//      /* found the equivalent attribute. Replace it.*/
//      PutInPlace(P,attr,this,&rsprod_attr_delete);
//   }
//     
//   return 0;
//}

int rsprod_attributes_replaceAttr(rsprod_attributes *this,rsprod_attr *attr) {

   /* create a local attribute object, with the same name as the attribute in parameter */
   rsprod_attr nattr;
   if (rsprod_string_trim_allNullsExceptLast(rsprod_attr_getName(attr),&(nattr.content.name))) {
      fprintf(stderr,"ERROR (%s) Problem dealing with null characters in name <%s>\n",__func__,
	    rsprod_attr_getName(attr));
      return 1;
   }
   /* search in the list for an other attribute whose name will compare the locally created one */
   dbl_node *node = dbl_list_find(this,NULL,&rsprod_attr_compareName,&nattr);
   /* act on return value */
   if (node == NULL) {
      /* no attribute with this name in the list: add a new one */
      rsprod_echo(stdout,"NOT FOUND\n");
      rsprod_attributes_addCopyAttr(this,attr);
   } else {
      /* found the equivalent attribute. Replace it.*/
      rsprod_echo(stdout,"FOUND\n");
      rsprod_attr_delete(node->c);
      node->c = attr; /* no copy */
   }
     
   return 0;
}

/* 
 * PURPOSE : test if an attribute (with name 'name') is found in the list.
 *
 * RETURN: 
 *    0 if attribute was not found
 *    1 otherwise
 *
 */
//int rsprod_attributes_existsAttr(rsprod_attributes this, const char *name) {
//   Position P;
//   rsprod_attr attr;
//   if (rsprod_string_trim_allNullsExceptLast(name,&(attr.content.name))) {
//      fprintf(stderr,"ERROR (rsprod_attributes_existsAttr) Problem dealing with null characters in name <%s>\n",name);
//      return 1;
//   }
//   P = Find(&attr,this,&rsprod_attr_compareName);
//   if (P == NULL) {
//      return 0;
//   }
//   return 1;
//}
int rsprod_attributes_existsAttr(rsprod_attributes *this, char *name) {
   rsprod_attr *attr;
   if (rsprod_attributes_getAttr(this,&attr,name)) return 0;
   return 1;
}

/* 
 * NAME : rsprod_attributes_deleteAttr
 *
 * PURPOSE : 
 *    remove from the list the attribute (with name 'name'),
 *    if found in the list. If not, do nothing.
 *
 * AUTHOR :
 *    Thomas Lavergne, met.no, 15.04.2008
 *
 * MODIFIED :
 *    Thomas Lavergne, met.no, 24.04.2008    :    correct a bug in the return value. 
 *
 */
//int rsprod_attributes_deleteAttr(rsprod_attributes this, const char *name) {
//   Position P;
//   rsprod_attr attr;
//   if (rsprod_string_trim_allNullsExceptLast(name,&(attr.content.name))) {
//      fprintf(stderr,"ERROR (%s) Problem dealing with null characters in name <%s>\n",__func__,name);
//      return 1;
//   }
//   P = Find(&attr,this,&rsprod_attr_compareName);
//   if (P == NULL) {
//      /* attribute not in the list. This is not an error. */
//   } else {
//      Delete(P,this,&rsprod_attr_delete);
//   }
//   return 0;
//}
int rsprod_attributes_deleteAttr(rsprod_attributes *this, char *name) {
   rsprod_attr attr;
   if (rsprod_string_trim_allNullsExceptLast(name,&(attr.content.name))) {
      fprintf(stderr,"ERROR (%s) Problem dealing with null characters in name <%s>\n",__func__,name);
      return 1;
   }
   dbl_node *node = dbl_list_find(this,NULL,&rsprod_attr_compareName,&attr);
   /* act on return value */
   if (node == NULL) {
      /* attribute not in the list. This is not an error. */
   } else {
      if (dbl_list_removeNode(this,node,&rsprod_attr_delete)) {
         fprintf(stderr,"ERROR (%s) Problem removing node <%s> from list of attributes\n",__func__,name);
         return 1;
      }
   }
   return 0;
}

/* 
 * PURPOSE : copy the whole list of attributes
 */
//int rsprod_attributes_copy(rsprod_attributes orig, rsprod_attributes *dest) {
//   return copyList(orig,dest,&rsprod_attr_copy); 
//}
int rsprod_attributes_copy(rsprod_attributes *orig, rsprod_attributes **dest) {
   *dest = dbl_list_copy(orig,&rsprod_attr_copy);
   return (*dest == NULL); 
}
/* 
 * PURPOSE : create an empty list of attributes
 */
//void rsprod_attributes_create(rsprod_attributes *attr) {
//   *attr = MakeEmpty(NULL);
//}
void rsprod_attributes_create(rsprod_attributes **attr) {
   *attr = dbl_list_init(sizeof(rsprod_attr *));
}

/* 
 * PURPOSE : de-allocate the list of attributes.
 *    each attribute in the list is also de-allocated.
 */
//void rsprod_attributes_delete(rsprod_attributes this) {
//   DeleteList(this,&rsprod_attr_delete);
//}
void rsprod_attributes_delete(rsprod_attributes *this) {
   if (this)
      dbl_list_delete(this,&rsprod_attr_delete);
}


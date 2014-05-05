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
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include "rsprod_string_utils.h"
#include "rsprod_intern.h"

/* 
 * PURPOSE: Go through the list of attributes and 
 *    display some information on each of them.
 */
void rsprod_attributes_printInfo(rsprod_attributes this) {
   fprintf(stderr,"Attributes:\n");
   BrowseList(this,&rsprod_attr_printInfo);
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
int rsprod_attributes_getAttr(rsprod_attributes this, rsprod_attr **attr_p, char *attrname) {
   Position P;
   rsprod_attr attr;
   if (rsprod_string_trim_allNullsExceptLast(attrname,&(attr.content.name))) {
      fprintf(stderr,"ERROR (rsprod_attributes_getAttr) Problem dealing with null characters in name <%s>\n",attrname);
      return 1;
   }
   P = Find(&attr,this,&rsprod_attr_compareName);
   if (P == NULL) {
      return 1;
   }
   *attr_p = Retrieve(P);
   return 0;
}

/* 
 * DEFINE AND EXPAND SOME ROUTINES FROM MACROS
 */

/* NAME: 
 *    rsprod_attributes_accessValues_double(),
 *    rsprod_attributes_accessValues_float(),
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
 *    ret = rsprod_attributes_accessValues_float(listattr,"someFloatParams",&params);
 *    if (ret) { ERROR-HANDLING};
 *    for (int i = 0 ; i < 4 ; i ++) {
 *       printf("[%d] = %f\n",i,params[i]);
 *    }
 */

#define DefineAccessValuesFunction(t) \
   int rsprod_attributes_accessValues_ ## t (rsprod_attributes this, char *attrname, t **casted) { \
      rsprod_attr *attr; \
      if (getAttributes(this,&attr,attrname)) { \
	 fprintf(stderr,"ERROR (rsprod_attributes_accessValues_" #t ") cannot find attribute <%s>.\n",attrname); \
         return 1; \
      } \
      if ((*(attr->methods->accessValues))(attr,casted)) { \
         fprintf(stderr,"ERROR (rsprod_attributes_accessValues_" #t ") type mismatch for attribute <%s>.\n",attrname); \
	 return 1; \
      } \
      return 0; \
   }

DefineAccessValuesFunction(double)  // rsprod_attributes_accessValues_double
DefineAccessValuesFunction(float)   // rsprod_attributes_accessValues_float
DefineAccessValuesFunction(short)   // rsprod_attributes_accessValues_short
DefineAccessValuesFunction(int)     // rsprod_attributes_accessValues_int
DefineAccessValuesFunction(char)    // rsprod_attributes_accessValues_char

#undef DefineAccessValuesFunction

/* NAME: 
 *    rsprod_attributes_accessValue_double(),
 *    rsprod_attributes_accessValue_float(),
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

#define DefineAccessValueFunction(t) \
   int rsprod_attributes_accessValue_ ## t (rsprod_attributes this, char *attrname, t *casted) { \
      rsprod_attr *attr; \
      if (getAttributes(this,&attr,attrname)) { \
	 fprintf(stderr,"ERROR (rsprod_attributes_accessValue_" #t ") cannot find attribute <%s>.\n",attrname); \
         return 1; \
      } \
      if ((*(attr->methods->accessValue))(attr,0,casted)) { \
         fprintf(stderr,"ERROR (rsprod_attributes_accessValue_" #t ") type mismatch for attribute <%s>.\n",attrname); \
	 return 1; \
      } \
      return 0; \
   }

DefineAccessValueFunction(double) // rsprod_attributes_accessValue_double
DefineAccessValueFunction(float)  // rsprod_attributes_accessValue_float
DefineAccessValueFunction(short)  // rsprod_attributes_accessValue_short
DefineAccessValueFunction(int)    // rsprod_attributes_accessValue_int
DefineAccessValueFunction(char)   // rsprod_attributes_accessValue_char

#undef DefineAccessValueFunction

/* 
 * PURPOSE : add an attribute to the list. No copying.
 */
int rsprod_attributes_addAttr(rsprod_attributes this, rsprod_attr *attr) {
   Position P = Last(this);
   Insert(attr,this,P);
   return 0;
}

/* 
 * PURPOSE : add an attribute to the list. With copy.
 */
int rsprod_attributes_addCopyAttr(rsprod_attributes this, rsprod_attr *attr) {
   Position P = Last(this);
   Insert(rsprod_attr_copy(attr),this,P);
   return 0;
}

/* 
 * PURPOSE : replace in the list, the attribute whose name is
 *     matching the one given as a 2nd parameter.
 *
 *     The original attribute is deleted and the new
 *     one is copied at the same position.
 */
int rsprod_attributes_replaceAttr(rsprod_attributes this,rsprod_attr *attr) {
   Position P;
   rsprod_attr nattr;
   if (rsprod_string_trim_allNullsExceptLast(rsprod_attr_getName(attr),&(nattr.content.name))) {
      fprintf(stderr,"ERROR (rsprod_attributes_replaceAttr) Problem dealing with null characters in name <%s>\n",
	    rsprod_attr_getName(attr));
      return 1;
   }
   P = Find(&nattr,this,&rsprod_attr_compareName);
   if (P == NULL) {
      fprintf(stderr,"ERROR (rsprod_attributes_replaceAttr) Could not find %s in the list.\n",
	    rsprod_attr_getName(attr));
      return 1;
   }
     
   PutInPlace(P,attr,this,&rsprod_attr_delete);

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
int rsprod_attributes_existsAttr(rsprod_attributes this, const char *name) {
   Position P;
   rsprod_attr attr;
   if (rsprod_string_trim_allNullsExceptLast(name,&(attr.content.name))) {
      fprintf(stderr,"ERROR (rsprod_attributes_existsAttr) Problem dealing with null characters in name <%s>\n",name);
      return 1;
   }
   P = Find(&attr,this,&rsprod_attr_compareName);
   if (P == NULL) {
      return 0;
   }
   return 1;
}

/* 
 * NAME : rsprod_attributes_deleteAttr
 *
 * PURPOSE : 
 *    remove from the list the attribute (with name 'name'),
 *    if found in the list.
 *
 * AUTHOR :
 *    Thomas Lavergne, met.no, 15.04.2008
 *
 * MODIFIED :
 *
 */
int rsprod_attributes_deleteAttr(rsprod_attributes this, const char *name) {
   Position P;
   rsprod_attr attr;
   if (rsprod_string_trim_allNullsExceptLast(name,&(attr.content.name))) {
      fprintf(stderr,"ERROR (rsprod_attributes_deleteAttr) Problem dealing with null characters in name <%s>\n",name);
      return 1;
   }
   P = Find(&attr,this,&rsprod_attr_compareName);
   if (P == NULL) {
      /* attribute not in the list. This is not an error. */
      return 0;
   }
   Delete(P,this,&rsprod_attr_delete);

   return 1;
}

/* 
 * PURPOSE : copy the whole list of attributes
 */
int rsprod_attributes_copy(rsprod_attributes orig, rsprod_attributes *dest) {
   return copyList(orig,dest,&rsprod_attr_copy); 
}

/* 
 * PURPOSE : create an empty list of attributes
 */
void rsprod_attributes_create(rsprod_attributes *attr) {
   *attr = MakeEmpty(NULL);
}

/* 
 * PURPOSE : de-allocate the list of attributes.
 *    each attribute in the list is also de-allocated.
 */
void rsprod_attributes_delete(rsprod_attributes this) {
   DeleteList(this,&rsprod_attr_delete);
}



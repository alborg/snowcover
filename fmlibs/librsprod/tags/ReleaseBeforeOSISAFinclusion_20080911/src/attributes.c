/*
 * NAME: attributes.c
 * 
 * PURPOSE: 
 *    Implement the low-level actions of 'single' attributes object (typed information
 *    qualifying a field). Example of attributes are : 'fillValue', 'valid_min', 
 *    'long_name', etc...
 *
 * DESCRIPTION:
 *    A single attribute is a 'rsprod_attr' (as opposed to a 'rsprod_attributes' which is 
 *    a list of rsprod_attr - and is implemented in 'listattributes.c').
 *    A rsprod_attr is composed of a 'content' and some 'methods'. The 'methods' ensure 
 *    type-safe access to the 'content', which is composed of:
 *       o type;
 *       o nb of values;
 *       o (pointer to the) values (of type 'type'); 
 *       o name.
 *    Because the rsprod_attr is typed, several of its routines had to be generated from 
 *    macros (see below rsprod_attr_accessValues_#) in order to easily implement the 
 *    type-dependent routines.
 *    
 *
 * NOTE:
 *    Only 'single' attributes are implemented here. For list of attributes (a field 
 *    generally having several attributes), refer to listattributes.c
 *    
 * DEPENDENCIES:
 *    The 'content' of an rsprod_attr is coded as a 'notype' object (see notype.c).
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */ 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "rsprod_intern.h"
#include "rsprod_string_utils.h"

/* 
 *   DEFINE SOME 'TYPED' MACROS (which expands to methods)
 */
#define DefineAccessValuesFunction(t,T) \
   int rsprod_attr_accessValues_ ## t (rsprod_attr *this, t **casted) {\
      return rsprod_notype_accessValues_ ## t (this->content.notype, casted); \
   }

#define DefineAccessValueFunction(t,T) \
   int rsprod_attr_accessValue_ ## t (rsprod_attr *this, size_t ind, t *casted) {\
      return rsprod_notype_accessValue_ ## t (this->content.notype, ind, casted); \
   }

#define DefineAttrMethods(t) \
   static rsprod_attr_methods iAttrMethods_ ## t = { \
      &rsprod_attr_accessValues_ ## t, \
      &rsprod_attr_accessValue_ ## t}

/* 
 *   EXPAND MACROS
 */
DefineAccessValuesFunction(double,DOUBLE) // rsprod_attr_accessValues_double
DefineAccessValuesFunction(float,FLOAT)   // rsprod_attr_accessValues_float
DefineAccessValuesFunction(short,SHORT)   // rsprod_attr_accessValues_short
DefineAccessValuesFunction(int,INT)       // rsprod_attr_accessValues_int
DefineAccessValuesFunction(char,CHAR)     // rsprod_attr_accessValues_char

/* define _accessValue_ functions */
DefineAccessValueFunction(double,DOUBLE) // rsprod_attr_accessValue_double
DefineAccessValueFunction(float,FLOAT)   // rsprod_attr_accessValue_float
DefineAccessValueFunction(short,SHORT)   // rsprod_attr_accessValue_short
DefineAccessValueFunction(int,INT)       // rsprod_attr_accessValue_int
DefineAccessValueFunction(char,CHAR)     // rsprod_attr_accessValue_char

/* define 'method' objects (scope is global): all 'float' attributes
 * refer to the same 'method_float' object. */
DefineAttrMethods(double);
DefineAttrMethods(float);
DefineAttrMethods(short);
DefineAttrMethods(int);
DefineAttrMethods(char);

/* 
 *   UNDEFINE THE MACROS
 */
#undef DefineAccessValuesFunction
#undef DefineAccessValueFunction
#undef DefineAttrMethods

/* 
 * USER-FRIENDLY 'GET' ROUTINES:
 *    implement the access to the various elements of 
 *    the attribute's 'content': 
 *       o type;
 *       o nb of values;
 *       o (pointer to the) values; 
 *       o name.
 */
rsprod_type rsprod_attr_getType(rsprod_attr *this) {
   return rsprod_notype_getType(this->content.notype);
}
size_t rsprod_attr_getNbvalues(rsprod_attr *this) {
   return rsprod_notype_getNbvalues(this->content.notype);
}
void *rsprod_attr_getValues(rsprod_attr *this) {
   return rsprod_notype_getValues(this->content.notype);
}
char *rsprod_attr_getName(rsprod_attr *this) {
   return this->content.name;
}

/* DEVELOPER-FRIENDLY 'GET' ROUTINES: (shortcuts valid only in this file) */
#define getType rsprod_attr_getType
#define getNbvalues rsprod_attr_getNbvalues
#define getValues rsprod_attr_getValues
#define getName rsprod_attr_getName

/* 
 * NAME: rsprod_attr_create()
 *
 * PURPOSE:
 *    Allocate and initialize a new rsprod_attr object (with appropriate methods)
 *    with the user-provided:
 *       o name;
 *       o type;
 *       o length (number of elements).
 *
 * NOTE:
 *    Only the 3 elements above are set. The actual 'value' is *not*. Use routines
 *    rsprod_attr_createWithCopyValues() or rsprod_attr_createWithAssignValues()
 *    for this behaviour.
 *
 * EXAMPLE:
 *    rsprod_attr *Attr;
 *    ret = rsprod_attr_create(&Attr,"_FillValue",RSPROD_FLOAT,1);
 *    if (ret) { ERROR-HANDLING; }
 *    
 */
int rsprod_attr_create(rsprod_attr **this,char *name, rsprod_type type,size_t length) {

   /* allocate the memory space */
   rsprod_attr *attr = malloc(sizeof(rsprod_attr));
   if (attr == NULL) {
      fprintf(stderr,"ERROR (rsprod_attr_create) Memory allocation problem.\n");
      return 1;
   }
  
   /* create the 'content' part */
   rsprod_notype *notype;
   if (rsprod_notype_create(&notype,type,length)) {
      fprintf(stderr,"ERROR (rsprod_attr_create) Cannot create a notype object with given parameters.\n");
      return 1;
   }

   attr->content.notype   = notype;

   if (rsprod_string_trim_allNullsExceptLast(name,&(attr->content.name))) {
      fprintf(stderr,"ERROR (rsprod_attr_create) Problem dealing with null characters in name <%s>\n",name);
      return 1;
   }

   /* assign the 'methods', depending on the type */ 
   switch (type) {
      case RSPROD_DOUBLE: attr->methods = &iAttrMethods_double;break;
      case RSPROD_FLOAT:  attr->methods = &iAttrMethods_float;break;
      case RSPROD_SHORT:  attr->methods = &iAttrMethods_short;break;
      case RSPROD_INT:    attr->methods = &iAttrMethods_int;break;
      case RSPROD_CHAR:   attr->methods = &iAttrMethods_char;break;
      default:
         fprintf(stderr,"WARNING (rsprod_attr_create) unknown action for type <%d>.\n",type);
	 attr->methods = NULL;
	 break;
   }

   /* return the (pointer to ) attribute to the calling routine */
   *this = attr;
   return 0;
}

/* 
 * NAME: rsprod_attr_createWithCopyValues()
 *
 * PURPOSE:
 *    Allocate and initialize a new rsprod_attr object (with appropriate methods)
 *    with the user-provided:
 *       o name;
 *       o type;
 *       o length (number of elements in the value);
 *       o value;
 *
 * NOTE:
 *    o The value is *copied* into the 'content': we are then no more dependent on
 *      the existence (and thus scope) of the original value.
 *    o Use rsprod_attr_create() (see above).
 *
 * EXAMPLE:
 *    rsprod_attr *Attr;
 *    float fillvalf = -99.;
 *    ret = rsprod_attr_create(&Attr,"_FillValue",RSPROD_FLOAT,1,&fillvalf);
 *    if (ret) { ERROR-HANDLING; }
 *
 *    rsprod_attr *Attr;
 *    float intarray[3];
 *    intarray[0] = 0; intarray[1] = 10; intarray[2] = 5;
 *    ret = rsprod_attr_create(&Attr,"some_int_attribute",RSPROD_INT,3,intarray);
 *    if (ret) { ERROR-HANDLING; }
 *    
 */

int rsprod_attr_createWithCopyValues(rsprod_attr **this,char *name, rsprod_type type, size_t length, void *values) {
   int ret;

   ret = rsprod_attr_create(this,name,type,length);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_attr_createWithCopyValues) Cannot create the attribute object with name %s.\n",name);
      return 1;
   }
   void *newvalue = getValues(*this);
   newvalue = memcpy(newvalue,values,length * SizeOf[type]);
   return 0;
}

/* 
 * NAME: rsprod_attr_createWithAssignValues()
 *
 * PURPOSE:
 *    Same as above (rsprod_attr_createWithCopyValues), but we point to 
 *    the original value (and not copy it).
 *
 * NOTE:
 *    o The value of the attribute is a *pointer to* the original one. Mind the life-time
 *      (scope) of the original value.
 */
int rsprod_attr_createWithAssignValues(rsprod_attr **this,char *name, rsprod_type type, size_t length, void *values) {
   int ret;
   ret = rsprod_attr_create(this,name,type,length);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_attr_createWithAssignValues) Cannot create the attribute object with name %s.\n",name);
      return 1;
   }
   (*this)->content.notype->content.values = values;
   return 0;
}

/* 
 * PURPOSE: compare if two attributes have the same name 
 */
int rsprod_attr_compareName(rsprod_attr *A,rsprod_attr *B) {
   return strcmp(getName(A),getName(B));
}


/* 
 * PURPOSE: return a copy of an 'original' rsprod_attr object
 */
rsprod_attr *rsprod_attr_copy(rsprod_attr *orig) {
   rsprod_attr *tmp;
   int ret = rsprod_attr_createWithCopyValues(&tmp,getName(orig),
	 getType(orig),getNbvalues(orig),getValues(orig));
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_attr_copy) cannot create a new attribute with original parameters.\n");
      return NULL;
   }
   return tmp;
}

/* 
 * PURPOSE: free the memory space of the attribute.
 */
void rsprod_attr_delete(rsprod_attr *this) {
   free(getName(this));
   rsprod_notype_delete(this->content.notype);
}

/* 
 * PURPOSE: print some information about the attribute (name, nbvalues and type).
 */
void rsprod_attr_printInfo(rsprod_attr *attr) {
   fprintf(stderr,"\t%s (%u x %s)\n",getName(attr),getNbvalues(attr),TypeName[getType(attr)]);
}


/* UNDEF DEVELOPER-FRIENDLY 'GET' ROUTINES */
#undef getType 
#undef getNbvalues 
#undef getValues 
#undef getName 

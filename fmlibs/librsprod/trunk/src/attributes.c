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
#define DefineSprintfFunction(t,T) \
   int rsprod_attr_sprintf_ ## t (rsprod_attr *this, size_t ind, char *buff) {\
      return rsprod_notype_sprintf_ ## t (this->content.notype, ind, buff); \
   }

#define DefineAttrMethods(t) \
   static rsprod_attr_methods iAttrMethods_ ## t = { \
      &rsprod_attr_accessValues_ ## t, \
      &rsprod_attr_accessValue_ ## t, \
      &rsprod_attr_sprintf_ ## t,\
   }

/* 
 *   EXPAND MACROS
 */
DefineAccessValuesFunction(Double,DOUBLE) // rsprod_attr_accessValues_Double
DefineAccessValuesFunction(Float,FLOAT)   // rsprod_attr_accessValues_Float
DefineAccessValuesFunction(Short,SHORT)   // rsprod_attr_accessValues_Short
DefineAccessValuesFunction(Int,INT)       // rsprod_attr_accessValues_Int
DefineAccessValuesFunction(Char,CHAR)     // rsprod_attr_accessValues_Char
DefineAccessValuesFunction(Byte,BYTE)     // rsprod_attr_accessValues_Byte

/* define _accessValue_ functions */
DefineAccessValueFunction(Double,DOUBLE) // rsprod_attr_accessValue_Double
DefineAccessValueFunction(Float,FLOAT)   // rsprod_attr_accessValue_Float
DefineAccessValueFunction(Short,SHORT)   // rsprod_attr_accessValue_Short
DefineAccessValueFunction(Int,INT)       // rsprod_attr_accessValue_Int
DefineAccessValueFunction(Char,CHAR)     // rsprod_attr_accessValue_Char
DefineAccessValueFunction(Byte,BYTE)     // rsprod_attr_accessValue_Byte

/* define _sprintf_ functions */
DefineSprintfFunction(Double,DOUBLE) // rsprod_attr_sprintf_Double
DefineSprintfFunction(Float,FLOAT)   // rsprod_attr_sprintf_Float
DefineSprintfFunction(Short,SHORT)   // rsprod_attr_sprintf_Short
DefineSprintfFunction(Int,INT)       // rsprod_attr_sprintf_Int
DefineSprintfFunction(Char,CHAR)     // rsprod_attr_sprintf_Char
DefineSprintfFunction(Byte,BYTE)     // rsprod_attr_sprintf_Byte

/* define 'method' objects (scope is global): all 'float' attributes
 * refer to the same 'method_Float' object. */
DefineAttrMethods(Double);
DefineAttrMethods(Float);
DefineAttrMethods(Short);
DefineAttrMethods(Int);
DefineAttrMethods(Char);
DefineAttrMethods(Byte);

/* 
 *   UNDEFINE THE MACROS
 */
#undef DefineAccessValuesFunction
#undef DefineAccessValueFunction
#undef DefineSprintfFunction
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

int rsprod_attr_assignMethods(rsprod_attr *this,rsprod_type type) {

   int retcode = 0;
   switch (type) {
      case RSPROD_DOUBLE: this->methods = &iAttrMethods_Double;break;
      case RSPROD_FLOAT:  this->methods = &iAttrMethods_Float;break;
      case RSPROD_SHORT:  this->methods = &iAttrMethods_Short;break;
      case RSPROD_INT:    this->methods = &iAttrMethods_Int;break;
      case RSPROD_CHAR:   this->methods = &iAttrMethods_Char;break;
      case RSPROD_BYTE:   this->methods = &iAttrMethods_Byte;break;
      default:
         fprintf(stderr,"WARNING (%s) unknown action for type <%d>.\n",__func__,type);
	     this->methods = NULL;
         retcode = 1;
	     break;
   }

   return retcode;
}

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

   int ret;

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
   ret = rsprod_attr_assignMethods(attr,type);

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
 * PURPOSE: given a valid attribute object (one that already has a name),
 *    replace the name by a new one.
 */
int rsprod_attr_replaceName(rsprod_attr *this, char *new_name) {
   free(getName(this));
   if (rsprod_string_trim_allNullsExceptLast(new_name,&(this->content.name))) {
      fprintf(stderr,"ERROR (%s) Problem dealing with null characters in new name <%s>\n",new_name);
      return 1;
   }
   return 0;
}

/* 
 * PURPOSE: compare if two attributes have the same name 
 */
int rsprod_attr_compareName(rsprod_attr *A,rsprod_attr *B) {
   return !strcmp(getName(A),getName(B));
}

/* 
 * PURPOSE: modify the name of an attribute 
 *
 * For now : Hard code the tranformation from HDF4 to NC/CF attribute names.
 *           
 */
int rsprod_attr_modifyName(rsprod_attr *this) {
   int ret;
   char *new_name;
   ret = rsprod_string_fromHDF4_2_NC(getName(this),&new_name);
   if (ret) return 1;
   ret = rsprod_attr_replaceName(this,new_name);
   if (ret) return 1;
   return 0;
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
 * PURPOSE: access the (void *) contained in the attribute object by allocating
 *    and copying to the provided voidstar variable.
 */
int rsprod_attr_getVoidStar (rsprod_attr *this, void **voidstar) {
   return rsprod_notype_getVoidStar(this->content.notype,voidstar);
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
   fprintf(stderr,"\t%s (%u x %s)",getName(attr),getNbvalues(attr),TypeName[getType(attr)]);
}
void rsprod_attr_printNcdump(rsprod_attr *attr) {

   fprintf(stderr,"\t:%s (%u x %s) = ",getName(attr),getNbvalues(attr),TypeName[getType(attr)]);
   char buff[32];
   fprintf(stderr,"\"");
   for (size_t n = 0 ; n < getNbvalues(attr) ; n++) {
      int sret = (*attr->methods->sprintfValue)(attr,n,buff);
      if (sret) sprintf(buff,"<err>");
      fprintf(stderr,"%s",buff);
      if ((getType(attr) != RSPROD_CHAR) && (n != (getNbvalues(attr)-1))) fprintf(stderr,", ");
   }
   fprintf(stderr,"\"");
}

/* UNDEF DEVELOPER-FRIENDLY 'GET' ROUTINES */
#undef getType 
#undef getNbvalues 
#undef getValues 
#undef getName 

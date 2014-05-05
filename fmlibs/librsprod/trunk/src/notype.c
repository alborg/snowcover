/*
 * NAME: notype.c
 * 
 * PURPOSE: 
 *    Core of the 'typed' data implementation of rsprod. In this
 *    file is implemented most of the interface to a notype object
 *    which is composed of :
 *    A content:
 *      o a (void *) pointer to non typed 'data';
 *      o a number of elements in the (void *).
 *      o a type information (rsprod_type defined in rsprod.h) 
 *    Some methods:
 *      o access_value(s);
 *      o castToType();
 *
 * DESCRIPTION:
 *    
 * NOTE:
 *    
 * DEPENDENCIES:
 *    Uses the 'calibration' routines from file calibration.c.
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rsprod_report_utils.h"
#include "calibration.h"
#include "rsprod_intern.h"

/* USER-FRIENDLY ACCESS ROUTINES */
rsprod_type rsprod_notype_getType (rsprod_notype *this) {
   if (!this) {
      return RSPROD_INT;
   }
   return this->content.type;
}
size_t rsprod_notype_getNbvalues (rsprod_notype *this) {
   return this->content.nbvalues;
}
void *rsprod_notype_getValues (rsprod_notype *this) {
   return this->content.values;
}
void rsprod_notype_setType (rsprod_notype *this,rsprod_type newtype) {
   this->content.type = newtype;
}
void rsprod_notype_setNbvalues (rsprod_notype *this,size_t newnbvalues) {
   this->content.nbvalues = newnbvalues;
}
void rsprod_notype_setValues (rsprod_notype *this,void *newvalues) {
   this->content.values = newvalues;
}

/* DEVELOPER-FRIENDLY ACCESS ROUTINES (only available in this file) */
#define getType     rsprod_notype_getType
#define getNbvalues rsprod_notype_getNbvalues
#define getValues   rsprod_notype_getValues
#define setType     rsprod_notype_setType
#define setNbvalues rsprod_notype_setNbvalues
#define setValues   rsprod_notype_setValues

/* 
 * NAME : rsprod_notype_cast_Double(), 
 *        rsprod_notype_cast_Float(), 
 *        etc...
 *
 * PURPOSE : 
 *    Access the (void *) through a casted pointer variable. A type-mismatch
 *    is implemented as it is not possible use rsprod_notype_cast_Float() 
 *    on something else than a RSPROD_FLOAT notype object.
 */
#define DefineCastFunction(t,T) \
   int rsprod_notype_cast_ ## t (rsprod_notype *this, t **casted) {\
      TestValidType(getType(this)); \
      if (getType(this) == RSPROD_##T) *casted = getValues(this); else return 1;\
      return 0; \
   }

DefineCastFunction(Double,DOUBLE)   // rsprod_notype_cast_Double
DefineCastFunction(Float,FLOAT)     // rsprod_notype_cast_Float
DefineCastFunction(Short,SHORT)     // rsprod_notype_cast_Short
DefineCastFunction(Int,INT   )      // rsprod_notype_cast_Int
DefineCastFunction(Char,CHAR)       // rsprod_notype_cast_Char
DefineCastFunction(Byte,BYTE)       // rsprod_notype_cast_Byte
#undef DefineCastFunction


/* 
 * NAME : rsprod_notype_cast_double_to_Double(), 
 *        rsprod_notype_cast_double_to_Float(), 
 *        etc...
 *        rsprod_notype_cast_type1_to_type2().
 *
 * PURPOSE :
 *        Transform a notype object from type1 to type2. This
 *        involves changing the type, the (void *) and the 
 *        methods. The routines are designed to cope with
 *        a 'calibration' routine which would apply some
 *        calibration attributes (ratio and offset) to the data.
 *        
 * DESCRIPTION:
 *        The embedded steps from type1 to type2 are:
 *           1) cast the (void *) to type1 (pointed by origValues[]);
 *           2) allocate enough space for newValues[]: an array of nbvalues * sizeof(type2);
 *           3) loop (index e) through each (type1) element in origValues and: 
 *              if (origValues[e] == fillvalue_type1) then 
 *                 newValues[e] <- fillvalue_type2;
 *              else 
 *                 transform (*) origValues[e] into a (type2) and 
 *                 assign it to newValues[e].
 *              endif
 *           4) replace the (void *)/origValues[] by newValues[];
 *           5) change the type and methods for the object.
 *           
 *        (*) The transform operation is given as a pointer-to-calibration-function.
 *            It can be a plain cast or involve some calibration attributes.
 */
#define DefineCastFromToFunction(f,F,t,T) \
   int rsprod_notype_cast_ ## f ## _to_ ## t (rsprod_notype *this, void (*transf)(),void *fv,void *tv, void *p1, void *p2) {\
      f *origValues; \
      if (rsprod_notype_cast_ ## f (this,&origValues)) { \
     fprintf(stderr,"ERROR (rsprod_notype_cast_" #f "_to_" #t ") could not cast void* to " #f ".\n"); \
     return 1; \
      } \
      size_t nbvalues = rsprod_notype_getNbvalues(this); \
      t *newValues = malloc(SizeOf[RSPROD_##T]*nbvalues); if (newValues == NULL) {return 1;} \
      for (size_t e = 0 ; e < nbvalues ; e++) { \
     if ((fv != NULL) && (origValues[e] == *(f *)fv)) newValues[e] = *(t *)tv;     \
     else { \
        (*transf)(&(newValues[e]),&(origValues[e]),p1,p2); \
     }\
      } \
      free(getValues(this)); setType(this,RSPROD_##T); setValues(this,newValues); \
      extern rsprod_notype_methods iNotypeMethods_##t; this->methods = &iNotypeMethods_##t; \
      return 0; \
   }

/* 
 * NOTE: all cast_From_To are defined ('instantiated') although only
 *       some are not permitted (e.g. from/to char).
 */
DefineCastFromToFunction(Float,FLOAT,Float,FLOAT)
DefineCastFromToFunction(Float,FLOAT,Short,SHORT)
DefineCastFromToFunction(Float,FLOAT,Char,CHAR)
DefineCastFromToFunction(Float,FLOAT,Double,DOUBLE)
DefineCastFromToFunction(Float,FLOAT,Int,INT)
DefineCastFromToFunction(Float,FLOAT,Byte,BYTE)
DefineCastFromToFunction(Short,SHORT,Float,FLOAT)
DefineCastFromToFunction(Short,SHORT,Short,SHORT)
DefineCastFromToFunction(Short,SHORT,Char,CHAR)
DefineCastFromToFunction(Short,SHORT,Double,DOUBLE)
DefineCastFromToFunction(Short,SHORT,Int,INT)
DefineCastFromToFunction(Short,SHORT,Byte,BYTE)
DefineCastFromToFunction(Char,CHAR,Float,FLOAT)
DefineCastFromToFunction(Char,CHAR,Short,SHORT)
DefineCastFromToFunction(Char,CHAR,Char,CHAR)
DefineCastFromToFunction(Char,CHAR,Double,DOUBLE)
DefineCastFromToFunction(Char,CHAR,Int,INT)
DefineCastFromToFunction(Char,CHAR,Byte,BYTE)
DefineCastFromToFunction(Double,DOUBLE,Float,FLOAT)
DefineCastFromToFunction(Double,DOUBLE,Short,SHORT)
DefineCastFromToFunction(Double,DOUBLE,Char,CHAR)
DefineCastFromToFunction(Double,DOUBLE,Double,DOUBLE)
DefineCastFromToFunction(Double,DOUBLE,Int,INT)
DefineCastFromToFunction(Double,DOUBLE,Byte,BYTE)
DefineCastFromToFunction(Int,INT,Float,FLOAT)
DefineCastFromToFunction(Int,INT,Short,SHORT)
DefineCastFromToFunction(Int,INT,Char,CHAR)
DefineCastFromToFunction(Int,INT,Double,DOUBLE)
DefineCastFromToFunction(Int,INT,Int,INT)
DefineCastFromToFunction(Int,INT,Byte,BYTE)
DefineCastFromToFunction(Byte,BYTE,Float,FLOAT)
DefineCastFromToFunction(Byte,BYTE,Short,SHORT)
DefineCastFromToFunction(Byte,BYTE,Char,CHAR)
DefineCastFromToFunction(Byte,BYTE,Double,DOUBLE)
DefineCastFromToFunction(Byte,BYTE,Int,INT)
DefineCastFromToFunction(Byte,BYTE,Byte,BYTE)
#undef DefineCastFromToFunction

/* 
 * NAME: rsprod_notype_accessValues_Double,
 *       rsprod_notype_accessValues_Float,
 *       etc...
 *
 *       (note the plural: Values )
 *
 * PURPOSE:
 *    To access the (void *) through a casted pointer. 
 *
 * NOTE: 
 *    This is equivalent to the rsprod_notype_cast_() routines (see above) 
 *    but we give another name, compatible to what we use with the rsprod_attr 
 *    objects. This aliasing would be cleaner with some macros.
 */

#define DefineAccessValuesFunction(t,T) \
   int rsprod_notype_accessValues_ ## t (rsprod_notype *this, t **casted) {\
      TestValidType(getType(this)); \
      if (getType(this) == RSPROD_##T) *casted = getValues(this); else return 1;\
      return 0; \
   }

DefineAccessValuesFunction(Double,DOUBLE) // rsprod_notype_accessValues_Double
DefineAccessValuesFunction(Float,FLOAT)   // rsprod_notype_accessValues_Float
DefineAccessValuesFunction(Short,SHORT)   // rsprod_notype_accessValues_Short
DefineAccessValuesFunction(Int,INT)       // rsprod_notype_accessValues_Int
DefineAccessValuesFunction(Char,CHAR)     // rsprod_notype_accessValues_Char
DefineAccessValuesFunction(Byte,BYTE)     // rsprod_notype_accessValues_Byte
#undef DefineAccessValuesFunction

#define DefineAccessNValuesFunction(t,T) \
int rsprod_notype_accessNValues_ ## t (rsprod_notype *this, size_t ind, t **casted) {\
      TestValidType(getType(this)); \
      if (getType(this) == RSPROD_##T) { \
         if (ind < getNbvalues(this)) \
            *casted = &(((t *)(getValues(this)))[ind]); \
         else {\
            fprintf(stderr,"ERROR (rsprod_notype_accessNValues_"#t") index %u is not a valid element.\n",ind); \
            return 1; \
         } \
      } else return 1;\
      return 0; \
   }

DefineAccessNValuesFunction(Double,DOUBLE) // rsprod_notype_accessNValues_Double
DefineAccessNValuesFunction(Float,FLOAT)   // rsprod_notype_accessNValues_Float
DefineAccessNValuesFunction(Short,SHORT)   // rsprod_notype_accessNValues_Short
DefineAccessNValuesFunction(Int,INT)       // rsprod_notype_accessNValues_Int
DefineAccessNValuesFunction(Char,CHAR)     // rsprod_notype_accessNValues_Char
DefineAccessNValuesFunction(Byte,BYTE)     // rsprod_notype_accessNValues_Byte
#undef DefineAccessNValuesFunction

/* 
 * NAME: rsprod_notype_accessValue_Double,
 *       rsprod_notype_accessValue_Float,
 *       etc...
 *
 *       (note the singular: Value )
 *
 * PURPOSE:
 *    To access the n-th element of the (void *) through a typed variable. 
 *
 * NOTE: 
 *    o Once the (void *) is casted into the desired type, we can access
 *      the wished element, *by copying* its value.
 *    o The element index (parameter 'ind') has to be a valid index in the 
 *      array, or 1 is returned.
 */
#define DefineAccessValueFunction(t,T) \
   int rsprod_notype_accessValue_ ## t (rsprod_notype *this, size_t ind, t *casted) {\
      TestValidType(getType(this)); \
      if (getType(this) == RSPROD_##T) {\
     if (ind < getNbvalues(this)) \
        *casted = ((t *)(getValues(this)))[ind]; \
     else {\
        fprintf(stderr,"ERROR (rsprod_notype_accessValue_"#t") index %u is not a valid element.\n",ind); \
        return 1; \
     } \
      } else return 1;\
      return 0; \
   }
DefineAccessValueFunction(Double,DOUBLE) // rsprod_notype_accessValue_Double
DefineAccessValueFunction(Float,FLOAT)   // rsprod_notype_accessValue_Float
DefineAccessValueFunction(Short,SHORT)   // rsprod_notype_accessValue_Short
DefineAccessValueFunction(Int,INT)       // rsprod_notype_accessValue_Int
DefineAccessValueFunction(Char,CHAR)     // rsprod_notype_accessValue_Char
DefineAccessValueFunction(Byte,BYTE)     // rsprod_notype_accessValue_Byte
#undef DefineAccessValueFunction

/* 
 * NAME: rsprod_notype_returnValue_Double,
 *       rsprod_notype_returnValue_Float,
 *       etc...
 *
 *       (note the singular: Value )
 *
 * PURPOSE:
 *    To access the n-th element of the (void *) through a typed variable.
 *    Same as above but the prototype is different.
 *
 * NOTE: 
 *    o A NULL pointer is returned in case of errors.
 *    o The prototype defines a (void *) return that should be casted to true type when called.
 */
#define DefineReturnValueFunction(t,T) \
t *rsprod_notype_returnValue_ ## t (rsprod_notype *this, size_t ind) {\
      t *val; \
      if (!rsprod_notype_accessNValues_ ## t (this,ind,&val)) { \
         return val;\
      } else return NULL;\
   }

DefineReturnValueFunction(Double,DOUBLE) // rsprod_notype_returnValue_Double
DefineReturnValueFunction(Float,FLOAT)   // rsprod_notype_returnValue_Float
DefineReturnValueFunction(Short,SHORT)   // rsprod_notype_returnValue_Short
DefineReturnValueFunction(Int,INT)       // rsprod_notype_returnValue_Int
DefineReturnValueFunction(Char,CHAR)     // rsprod_notype_returnValue_Char
DefineReturnValueFunction(Byte,BYTE)     // rsprod_notype_returnValue_Byte
#undef DefineReturnValueFunction

/* 
 * NAME: rsprod_notype_initValues_Double,
 *       rsprod_notype_initValues_Float,
 *       etc...
 *
 *       (note the plural: Values )
 *
 * PURPOSE:
 *    To initialize all elements in the array with the value given as parameter. 
 *
 * NOTE: 
 */

#define DefineInitValuesFunction(t,T) \
   int rsprod_notype_initValues_ ## t (rsprod_notype *this, t *value) {\
      TestValidType(getType(this)); \
      if (getType(this) == RSPROD_##T) { \
         size_t nbValues = getNbvalues(this); \
         t *array = getValues(this); \
         for (size_t e = 0 ; e < nbValues ; e++) \
            array[e] = *value; \
         return 0; \
      } else return 1; \
   }

DefineInitValuesFunction(Double,DOUBLE) // rsprod_notype_initValues_Double
DefineInitValuesFunction(Float,FLOAT)   // rsprod_notype_initValues_Float
DefineInitValuesFunction(Short,SHORT)   // rsprod_notype_initValues_Short
DefineInitValuesFunction(Int,INT)       // rsprod_notype_initValues_Int
DefineInitValuesFunction(Char,CHAR)     // rsprod_notype_initValues_Char
DefineInitValuesFunction(Byte,BYTE)     // rsprod_notype_initValues_Byte
#undef DefineInitValuesFunction

/* 
 * NAME: rsprod_notype_sprintf_Double,
 *       rsprod_notype_sprintf_Float,
 *       etc...
 *
 *       (note the singular: Value )
 *
 * PURPOSE:
 *    To access the n-th element of the (void *) through a typed variable and use
 *    appropriate sprintf format to fill-in a char buffer.
 *
 * NOTE: 
 *    The character buffer must be allocated before call to these routines
 */
#define DefineSprintfFunction(t,T) \
   int rsprod_notype_sprintf_ ## t (rsprod_notype *this, size_t ind, char *buff) {\
      t *val = rsprod_notype_returnValue_ ## t (this,ind); \
      if (val) {\
         sprintf(buff,TypePrintFmt[getType(this)],*val); return 0; \
      } else return 1; \
   }
DefineSprintfFunction(Double,DOUBLE) // rsprod_notype_sprintf_Double
DefineSprintfFunction(Float,FLOAT)   // rsprod_notype_sprintf_Float
DefineSprintfFunction(Short,SHORT)   // rsprod_notype_sprintf_Short
DefineSprintfFunction(Int,INT)       // rsprod_notype_sprintf_Int
DefineSprintfFunction(Char,CHAR)     // rsprod_notype_sprintf_Char
DefineSprintfFunction(Byte,BYTE)     // rsprod_notype_sprintf_Byte
#undef DefineSprintfFunction

   /*
int    (*rsprod_notype_accessValues[RSPROD_NBTYPES])() = {&rsprod_notype_accessValues_Float,\
                                                          &rsprod_notype_accessValues_Short,\
                                                          &rsprod_notype_accessValues_Char,\
                                                          &rsprod_notype_accessValues_Double,\
                                                          &rsprod_notype_accessValues_Int};

int    (*rsprod_notype_accessValue[RSPROD_NBTYPES])() = {&rsprod_notype_accessValue_Float,\
                                                         &rsprod_notype_accessValue_Short,\
                                                         &rsprod_notype_accessValue_Char,\
                                                         &rsprod_notype_accessValue_Double,\
                                                         &rsprod_notype_accessValue_Int};
                  */

/* 
 * define global 'methods' object for each type 
 *
 * With such an object, a type1 object knows how to:
 *    o access all its (casted) values (accessValues);     
 *    o access one of its (casted) values (accessValue);
 *    o cast/transform one element from type1 to any other type (calibration_type1From_x_to_y);
 *    o cast/transform all its elements from type1 to any other type (cast_x_to_y).
 */
#define DefineNotypeMethods(t) \
   rsprod_notype_methods iNotypeMethods_ ## t = { \
      &rsprod_notype_accessValues_ ## t, \
      &rsprod_notype_accessValue_ ## t, \
      &rsprod_notype_initValues_ ## t, \
      &rsprod_notype_sprintf_ ## t, \
      { \
     &rsprod_notype_cast_ ## t ##_to_Float, \
     &rsprod_notype_cast_ ## t ##_to_Short, \
     &rsprod_notype_cast_ ## t ##_to_Char,  \
     &rsprod_notype_cast_ ## t ##_to_Double, \
     &rsprod_notype_cast_ ## t ##_to_Int, \
     &rsprod_notype_cast_ ## t ##_to_Byte \
      } , \
      {\
     &calibration_noeffectFrom_ ## t ##_to_Float, \
     &calibration_noeffectFrom_ ## t ##_to_Short, \
     &calibration_noeffectFrom_ ## t ##_to_Char, \
     &calibration_noeffectFrom_ ## t ##_to_Double, \
     &calibration_noeffectFrom_ ## t ##_to_Int, \
     &calibration_noeffectFrom_ ## t ##_to_Byte \
      } , \
      {\
     &calibration_type1From_ ## t ##_to_Float, \
     &calibration_type1From_ ## t ##_to_Short, \
     &calibration_type1From_ ## t ##_to_Char, \
     &calibration_type1From_ ## t ##_to_Double, \
     &calibration_type1From_ ## t ##_to_Int, \
     &calibration_type1From_ ## t ##_to_Byte \
      } , \
      {\
     &calibration_type2From_ ## t ##_to_Float, \
     &calibration_type2From_ ## t ##_to_Short, \
     &calibration_type2From_ ## t ##_to_Char, \
     &calibration_type2From_ ## t ##_to_Double, \
     &calibration_type2From_ ## t ##_to_Int, \
     &calibration_type2From_ ## t ##_to_Byte \
      } \
   }

DefineNotypeMethods(Float);
DefineNotypeMethods(Double);
DefineNotypeMethods(Short);
DefineNotypeMethods(Int);
DefineNotypeMethods(Char);
DefineNotypeMethods(Byte);
#undef DefineNotypeMethods

/*
 * NAME: rsprod_notype_create
 *
 * PURPOSE:
 *    Allocate an initialise a new notype object. 
 *
 * NOTE:
 *    o All is initialised except for the actual data (see below 
 *      rsprod_notype_createWith[Copy|Assign]Values() ).
 *    o The 'create' has to cope with 'empty' data as such
 *      dummy fields can be sometimes used only for their attributes (like
 *      the projection_information in netCDF's CF-1.x conventions).
 */
int rsprod_notype_create (rsprod_notype **this, rsprod_type type, size_t nbelems) {

   /* check the parameters */
   TestValidType(type);

   /* allocate the memory for the object */
   rsprod_notype *tmp = malloc(sizeof(rsprod_notype));
   if (tmp == NULL) {
      fprintf(stderr,"ERROR (rsprod_notype_create) Memory allocation issue.\n");
      return 1;
   }

   /* assign the components of the object */
   setType(tmp,type);
   setNbvalues(tmp,nbelems);
   setValues(tmp,malloc(SizeOf[type] * (nbelems + (type==RSPROD_CHAR?1:0))));
   if (getValues(tmp) == NULL) {
      fprintf(stderr,"ERROR (rsprod_notype_create) Memory allocation issue.\n");
      free(tmp);
      return 1;
   }
   /* special case for CHARs: we allocate one char more and null-terminate it. 
    * NOTE: the nbelems is still *not* including the '\0'. */
   if (getType(tmp) == RSPROD_CHAR) {
      char *stringvalue = getValues(tmp);
      stringvalue[getNbvalues(tmp)] = '\0';
   }

   /* assign the methods, depending on the type */ 
   switch (type) {
      case RSPROD_DOUBLE: tmp->methods = &iNotypeMethods_Double;break;
      case RSPROD_FLOAT:  tmp->methods = &iNotypeMethods_Float;break;
      case RSPROD_SHORT:  tmp->methods = &iNotypeMethods_Short;break;
      case RSPROD_INT:    tmp->methods = &iNotypeMethods_Int;break;
      case RSPROD_CHAR:   tmp->methods = &iNotypeMethods_Char;break;
      case RSPROD_BYTE:   tmp->methods = &iNotypeMethods_Byte;break;
      default:
         fprintf(stderr,"WARNING (rsprod_notype_create) unknown action for type <%d>.\n",type);
     tmp->methods = NULL;
     break;
   }

   /* point 'this' to our new object */
   *this = tmp;

   return 0;
}

/*
 * PURPOSE : 
 *    create a new notype object by copying the provided values.
 *
 * NOTE : 
 *    this will *not* work for RSPROD_CHAR datasets!!!
 */
int rsprod_notype_createWithCopyValues (rsprod_notype **this, rsprod_type type, size_t nbelems, void *values) {
   if (rsprod_notype_create (this,type,nbelems))
      return 1;
   memcpy(getValues(*this),values,nbelems*SizeOf[type]);
   return 0;
}

/*
 * PURPOSE : 
 *    create a new notype object by assigning the provided values (mind the scope of your original values).
 */
int rsprod_notype_createWithAssignValues (rsprod_notype **this, rsprod_type type, size_t nbelems, void *values) {
   if (rsprod_notype_create (this,type,nbelems))
      return 1;
   setValues(*this,values);
   return 0;
}

/*
 * PURPOSE : 
 *    free the memory space of a notype.
 */
void rsprod_notype_delete (rsprod_notype *this) {
   free(getValues(this));
}

/*
 * PURPOSE : 
 *    access the (void *) of a notype object by allocating and copying to
 *    the provided voidstar variable.
 */
int rsprod_notype_getVoidStar (rsprod_notype *this, void **voidstar) {
  
   size_t totsize = SizeOf[getType(this)] * getNbvalues(this);
   size_t size_alloc = totsize;
   if (getType(this) == RSPROD_CHAR)
      size_alloc += 1;
   *voidstar = malloc(size_alloc);
   if (*voidstar == NULL) {
      fprintf(stderr,"ERROR (%s) Memory allocation issue.\n",__func__);
      return 1;
   }
   if (getType(this) == RSPROD_CHAR) 
      memset(*voidstar,'\0',size_alloc);
   memcpy(*voidstar,getValues(this),totsize);
   return 0;

}

/*
 * PURPOSE : 
 *    allocate and copy a whole notype object.
 */
rsprod_notype *rsprod_notype_copy (rsprod_notype *orig) {
   rsprod_notype *dest;
   int ret = rsprod_notype_createWithCopyValues(&dest,getType(orig),getNbvalues(orig),getValues(orig));
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_notype_copy) Cannot create a copy of given object.\n");
      return NULL;
   }
   return dest;
}

/*
 * PURPOSE : 
 *    display some information about the notype element.
 */
void rsprod_notype_printInfo(rsprod_notype *this) {
   fprintf(stderr,"\tData is of type %s and has %lu values.\n",TypeName[getType(this)],(unsigned long)getNbvalues(this));
}
void rsprod_notype_printNcdump(rsprod_notype *this, long int n_elems) {
   if ( (n_elems < 0) || (n_elems > getNbvalues(this)) ) {
      n_elems = getNbvalues(this);
   }
   if ( n_elems == 0 ) {
      ; /* print nothing */
   } else { 
      char buff[100];
      fprintf(stderr,"\tVALUES 0..%ld: ",n_elems-1);
      for (size_t e = 0 ; e < n_elems ; e++) {
         int ret = (*this->methods->sprintfValue)(this,e,buff);
         fprintf(stderr,"%s,",buff);
      }
      fprintf(stderr,"\b\n");
   }
}

/* 
 * NAME: rsprod_notype_castToType
 *
 * PURPOSE:
 *    Transform a notype object to another type, using a calibration routine.
 *
 * NOTE:
 *    Currently, this function is not fully implemented and most of the flexibility it
 *    could have is not available. This is routine is used is a good entry point
 *    to pack/unpack data from (e.g) int to float, using a Type1 calibration. 
 *
 */
int rsprod_notype_castToType(rsprod_notype *this,rsprod_type newtype,short calibrationType,void *fval1,void *fval2,void *p1,void *p2) {

   /* test that the new type we ask for is valid */
   TestValidType(newtype);

   /* check if the cast we are asked to perform is valid */
   if (!TypeConvert[getType(this)][newtype]) {
      fprintf(stderr,"ERROR (rsprod_notype_castToType) it is not allowed to cast from %s to %s.\n",TypeName[getType(this)],TypeName[newtype]);
      return 1;
   }

   switch (calibrationType) {
      case RSPROD_CALIBRATION_ONLYCAST: 
         if ((*this->methods->castTo[newtype])(this,(this->methods->calibrationNoEffect[newtype]),fval1,fval2,p1,p2)) {
            fprintf(stderr,"ERROR (%s) while casting from %s to %s (Type1)\n",__func__,TypeName[getType(this)],TypeName[newtype]);
            return 1;
         }
         break;
      case RSPROD_CALIBRATION_TYPE1: 
         if ((*this->methods->castTo[newtype])(this,(this->methods->calibrationType1[newtype]),fval1,fval2,p1,p2)) {
            fprintf(stderr,"ERROR (%s) while casting from %s to %s (Type1)\n",__func__,TypeName[getType(this)],TypeName[newtype]);
            return 1;
         }
         break;
      case RSPROD_CALIBRATION_TYPE2:
         if ((*this->methods->castTo[newtype])(this,(this->methods->calibrationType2[newtype]),fval1,fval2,p1,p2)) {
            fprintf(stderr,"ERROR (%s) while casting from %s to %s (Type2)\n",__func__,TypeName[getType(this)],TypeName[newtype]);
            return 1;
         }
         break;
      default:
        fprintf(stderr,"ERROR (%s) Unknown calibration type (%d).\n",__func__,calibrationType);
        return 1;
   }


   return 0;
}

#undef getType
#undef getNbvalues
#undef getValues   
#undef setType
#undef setNbvalues
#undef setValues   

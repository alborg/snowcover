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
#include "rsprod_intern.h"

/* USER-FRIENDLY ACCESS ROUTINES */
rsprod_type rsprod_notype_getType (rsprod_notype *this) {
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
 * NAME : rsprod_notype_cast_double(), 
 *        rsprod_notype_cast_float(), 
 *        etc...
 *
 * PURPOSE : 
 *    Access the (void *) through a casted pointer variable. A type-mismatch
 *    is implemented as it is not possible use rsprod_notype_cast_float() 
 *    on something else than a RSPROD_FLOAT notype object.
 */
#define DefineCastFunction(t,T) \
   int rsprod_notype_cast_ ## t (rsprod_notype *this, t **casted) {\
      TestValidType(getType(this)); \
      if (getType(this) == RSPROD_##T) *casted = getValues(this); else return 1;\
      return 0; \
   }

DefineCastFunction(double,DOUBLE)   // rsprod_notype_cast_double
DefineCastFunction(float,FLOAT)     // rsprod_notype_cast_float
DefineCastFunction(short,SHORT)     // rsprod_notype_cast_short
DefineCastFunction(int,INT   )      // rsprod_notype_cast_int
DefineCastFunction(char,CHAR)       // rsprod_notype_cast_char
#undef DefineCastFunction


/* 
 * NAME : rsprod_notype_cast_double_to_double(), 
 *        rsprod_notype_cast_double_to_float(), 
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
   int rsprod_notype_cast_ ## f ## _to_ ## t (rsprod_notype *this, t (*transf)(),void *fv,void *tv, void *p1, void *p2) {\
      f *origValues; \
      if (rsprod_notype_cast_ ## f (this,&origValues)) { \
	 fprintf(stderr,"ERROR (rsprod_notype_cast_" #f "_to_" #t ") could not cast void* to " #f ".\n"); \
	 return 1; \
      } \
      size_t nbvalues = rsprod_notype_getNbvalues(this); \
      t *newValues = malloc(SizeOf[RSPROD_##T]*nbvalues); if (newValues == NULL) {return 1;}\
      for (size_t e = 0 ; e < getNbvalues(this) ; e++) { \
	 if ((fv != NULL) && (origValues[e] == *(f *)fv)) newValues[e] = *(t *)tv;     \
	 else (*transf)(&(newValues[e]),origValues[e],p1,p2); \
      } free(getValues(this)); setType(this,RSPROD_##T); setValues(this,newValues); \
      extern rsprod_notype_methods iNotypeMethods_##t; this->methods = &iNotypeMethods_##t; \
      return 0; \
   }

/* 
 * NOTE: all cast_From_To are defined ('instantiated') although only
 *       some are not permitted (e.g. from/to char).
 */
DefineCastFromToFunction(float,FLOAT,float,FLOAT)
DefineCastFromToFunction(float,FLOAT,short,SHORT)
DefineCastFromToFunction(float,FLOAT,char,CHAR)
DefineCastFromToFunction(float,FLOAT,double,DOUBLE)
DefineCastFromToFunction(float,FLOAT,int,INT)
DefineCastFromToFunction(short,SHORT,float,FLOAT)
DefineCastFromToFunction(short,SHORT,short,SHORT)
DefineCastFromToFunction(short,SHORT,char,CHAR)
DefineCastFromToFunction(short,SHORT,double,DOUBLE)
DefineCastFromToFunction(short,SHORT,int,INT)
DefineCastFromToFunction(char,CHAR,float,FLOAT)
DefineCastFromToFunction(char,CHAR,short,SHORT)
DefineCastFromToFunction(char,CHAR,char,CHAR)
DefineCastFromToFunction(char,CHAR,double,DOUBLE)
DefineCastFromToFunction(char,CHAR,int,INT)
DefineCastFromToFunction(double,DOUBLE,float,FLOAT)
DefineCastFromToFunction(double,DOUBLE,short,SHORT)
DefineCastFromToFunction(double,DOUBLE,char,CHAR)
DefineCastFromToFunction(double,DOUBLE,double,DOUBLE)
DefineCastFromToFunction(double,DOUBLE,int,INT)
DefineCastFromToFunction(int,INT,float,FLOAT)
DefineCastFromToFunction(int,INT,short,SHORT)
DefineCastFromToFunction(int,INT,char,CHAR)
DefineCastFromToFunction(int,INT,double,DOUBLE)
DefineCastFromToFunction(int,INT,int,INT)
#undef DefineCastFromToFunction

/* 
 * NAME: rsprod_notype_accessValues_double,
 *       rsprod_notype_accessValues_float,
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
 *    objects.
 */

   /*
#define DefineAccessValuesFunction(t,T) \
   int rsprod_notype_accessValues_ ## t (rsprod_notype *this, t **casted) {\
      TestValidType(getType(this)); \
      if (getType(this) == RSPROD_##T) *casted = getValues(this); else return 1;\
      return 0; \
   }

DefineAccessValuesFunction(double,DOUBLE) // rsprod_notype_accessValues_double
DefineAccessValuesFunction(float,FLOAT)   // rsprod_notype_accessValues_float
DefineAccessValuesFunction(short,SHORT)   // rsprod_notype_accessValues_short
DefineAccessValuesFunction(int,INT)       // rsprod_notype_accessValues_int
DefineAccessValuesFunction(char,CHAR)     // rsprod_notype_accessValues_char
#undef DefineAccessValuesFunction
   */

#define rsprod_notype_accessValues_double    rsprod_notype_cast_double
#define rsprod_notype_accessValues_float     rsprod_notype_cast_float
#define rsprod_notype_accessValues_short     rsprod_notype_cast_short
#define rsprod_notype_accessValues_int       rsprod_notype_cast_int
#define rsprod_notype_accessValues_char      rsprod_notype_cast_char

/* 
 * NAME: rsprod_notype_accessValue_double,
 *       rsprod_notype_accessValue_float,
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
DefineAccessValueFunction(double,DOUBLE) // rsprod_notype_accessValue_double
DefineAccessValueFunction(float,FLOAT)   // rsprod_notype_accessValue_float
DefineAccessValueFunction(short,SHORT)   // rsprod_notype_accessValue_short
DefineAccessValueFunction(int,INT)       // rsprod_notype_accessValue_int
DefineAccessValueFunction(char,CHAR)     // rsprod_notype_accessValue_char
#undef DefineAccessValueFunction

   /*
int    (*rsprod_notype_accessValues[RSPROD_NBTYPES])() = {&rsprod_notype_accessValues_float,\
                                                          &rsprod_notype_accessValues_short,\
                                                          &rsprod_notype_accessValues_char,\
                                                          &rsprod_notype_accessValues_double,\
                                                          &rsprod_notype_accessValues_int};

int    (*rsprod_notype_accessValue[RSPROD_NBTYPES])() = {&rsprod_notype_accessValue_float,\
                                                         &rsprod_notype_accessValue_short,\
                                                         &rsprod_notype_accessValue_char,\
                                                         &rsprod_notype_accessValue_double,\
                                                         &rsprod_notype_accessValue_int};
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
      { \
	 &rsprod_notype_cast_ ## t ##_to_float, \
	 &rsprod_notype_cast_ ## t ##_to_short, \
	 &rsprod_notype_cast_ ## t ##_to_char,  \
	 &rsprod_notype_cast_ ## t ##_to_double, \
	 &rsprod_notype_cast_ ## t ##_to_int \
      } , \
      {\
	 &calibration_type1From_ ## t ##_to_float, \
	 &calibration_type1From_ ## t ##_to_short, \
	 &calibration_type1From_ ## t ##_to_char, \
	 &calibration_type1From_ ## t ##_to_double, \
	 &calibration_type1From_ ## t ##_to_int \
      } \
   }

DefineNotypeMethods(double);
DefineNotypeMethods(float);
DefineNotypeMethods(short);
DefineNotypeMethods(int);
DefineNotypeMethods(char);
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
      case RSPROD_DOUBLE: tmp->methods = &iNotypeMethods_double;break;
      case RSPROD_FLOAT:  tmp->methods = &iNotypeMethods_float;break;
      case RSPROD_SHORT:  tmp->methods = &iNotypeMethods_short;break;
      case RSPROD_INT:    tmp->methods = &iNotypeMethods_int;break;
      case RSPROD_CHAR:   tmp->methods = &iNotypeMethods_char;break;
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
   
   if (getType(this) == RSPROD_CHAR) {
      fprintf(stderr,"ERROR (rsprod_notype_getVoidStar) Special case for CHAR notype is not implemented.\n");
      return 1;
   }
   size_t totsize = SizeOf[getType(this)] * getNbvalues(this);
   *voidstar = malloc(totsize);
   if (*voidstar == NULL) {
      fprintf(stderr,"ERROR (rsprod_notype_getVoidStar) Memory allocation issue.\n");
      return 1;
   }
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
   fprintf(stderr,"Data is of type %s and has %lu values.\n",TypeName[getType(this)],(unsigned long)getNbvalues(this));
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
int rsprod_notype_castToType(rsprod_notype *this,rsprod_type newtype,void *fval1,void *fval2,void *p1,void *p2) {

   /* test that the new type we ask for is valid */
   TestValidType(newtype);

   /* check if the cast we are asked to perform is valid */
   if (!TypeConvert[getType(this)][newtype]) {
      fprintf(stderr,"ERROR (rsprod_notype_castToType) it is not allowed to cast from %s to %s.\n",TypeName[getType(this)],TypeName[newtype]);
      return 1;
   }


   if ((*this->methods->castTo[newtype])(this,(this->methods->calibrationType1[newtype]),fval1,fval2,p1,p2)) {
      fprintf(stderr,"ERROR (rsprod_notype_castToType) while casting from %s to %s\n",TypeName[getType(this)],TypeName[newtype]);
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

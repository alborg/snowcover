

#ifndef RSPROD_INTERN_H
#define RSPROD_INTERN_H

#define STARTMACRO do 
#define ENDMACRO while (0)

#include "rsprod_types.h"

/* ============================================================
 *    RSPROD   TYPE
 * ============================================================ */
/* macro to wrap the test to a vaild/invalid type with some error reporting */
#define TestValidType(t) STARTMACRO \
      {if (!isValidType(t)) {fprintf(stderr,"ERROR : invalid type.\n"); return 1;}} \
      ENDMACRO
/* global macros */
extern const size_t SizeOf[RSPROD_NBTYPES];
extern const char * TypeName[RSPROD_NBTYPES];
extern const short  TypeConvert[RSPROD_NBTYPES][RSPROD_NBTYPES];
/* routines */
int isValidType(rsprod_type type);
int typeToFormat(rsprod_type type, char **string);



/* =================================================================== 
 *         NOTYPE      
 * =================================================================== */
rsprod_type rsprod_notype_getType(rsprod_notype *);
size_t rsprod_notype_getNbvalues(rsprod_notype *);
void *rsprod_notype_getValues(rsprod_notype *);
int  rsprod_notype_create(rsprod_notype **,rsprod_type,size_t);
int  rsprod_notype_createWithCopyValues(rsprod_notype **,rsprod_type,size_t,void *);
int  rsprod_notype_createWithAssignValues(rsprod_notype **,rsprod_type,size_t,void *);
void rsprod_notype_delete(rsprod_notype *);
int  rsprod_notype_getVoidStar(rsprod_notype *, void **);
rsprod_notype  *rsprod_notype_copy(rsprod_notype *);
void rsprod_notype_printInfo(rsprod_notype *);
int rsprod_notype_castToType(rsprod_notype *this,rsprod_type newtype,short calType, void *fval1,void *fval2,void *p1,void *p2);

#define DefineAccessValuesFunctionPrototype(t) int  rsprod_notype_accessValues_ ## t (rsprod_notype *, t **)
DefineAccessValuesFunctionPrototype(double);
DefineAccessValuesFunctionPrototype(float);
DefineAccessValuesFunctionPrototype(char);
DefineAccessValuesFunctionPrototype(short);
DefineAccessValuesFunctionPrototype(int);
#undef DefineAccessValuesFunctionPrototype
#define DefineAccessValueFunctionPrototype(t) int  rsprod_notype_accessValue_ ## t (rsprod_notype *,size_t, t *)
DefineAccessValueFunctionPrototype(double);
DefineAccessValueFunctionPrototype(float);
DefineAccessValueFunctionPrototype(char);
DefineAccessValueFunctionPrototype(short);
DefineAccessValueFunctionPrototype(int);
#undef DefineAccessValueFunctionPrototype

/* =====================================================================
 *    RSPROD    DATA
 * ===================================================================== */
#define rsprod_data_create(d,t,s)                             rsprod_notype_create((rsprod_notype **)d,t,s)
#define rsprod_data_createWithCopyValues(d,t,s,v)             rsprod_notype_createWithCopyValues((rsprod_notype **)d,t,s,v)
#define rsprod_data_createWithAssignValues(d,t,s,v)           rsprod_notype_createWithAssignValues((rsprod_notype **)d,t,s,v)
#define rsprod_data_delete(d)                                 rsprod_notype_delete((rsprod_notype *)d)
#define rsprod_data_getVoidStar(d,v)                          rsprod_notype_getVoidStar((rsprod_notype *)d,v)
#define rsprod_data_copy(d)                                   rsprod_notype_copy((rsprod_notype *)d)
#define rsprod_data_printInfo(d)                              rsprod_notype_printInfo((rsprod_notype *)d)
#define rsprod_data_castToType(d,t,c,v1,v2,p1,p2)             rsprod_notype_castToType((rsprod_notype *)d,t,c,v1,v2,p1,p2)
#define rsprod_data_getType(d)                                rsprod_notype_getType((rsprod_notype *)d)
#define rsprod_data_getNbvalues(d)                            rsprod_notype_getNbvalues((rsprod_notype *)d)
#define rsprod_data_getValues(d)                              rsprod_notype_getValues((rsprod_notype *)d)


/* =================================================================== 
 *         RSPROD    DIMENSIONS
 * =================================================================== */
int rsprod_dims_addOne(rsprod_dimensions *this, short position, char *dimName, unsigned int dimLength, short dimOrder);
int rsprod_dims_setNames(rsprod_dimensions *this,const unsigned short nbNames, char *names[]);
int rsprod_dims_setLengths(rsprod_dimensions *this,const unsigned short nbLengths, unsigned int *lengths);
int rsprod_dims_setOrders(rsprod_dimensions *this,const unsigned short nbOrders, short *orders);
int rsprod_dims_create(rsprod_dimensions **this, const unsigned short nbdims, char **dimsName, 
      unsigned int *dimsLength, short *dimsOrder);
void rsprod_dims_delete(rsprod_dimensions *this);
int rsprod_dims_getListNames(rsprod_dimensions *this, int startInd, char **list, char delim);
rsprod_dimensions *rsprod_dims_copy(rsprod_dimensions *orig);
void rsprod_dims_printInfo(rsprod_dimensions *this);
int rsprod_dims_compare(rsprod_dimensions *one,rsprod_dimensions *two);
int  rsprod_dims_compare(rsprod_dimensions *one,rsprod_dimensions *two);
unsigned long vecIndex(rsprod_dimensions *this, unsigned short nbMatIndex, long *MatIndex);
unsigned long vecIndexRelative(rsprod_dimensions *this, unsigned long current, unsigned short nbMatIndex, long *MatIndex);


/* ====================================================================
 *     RSPROD   ATTR   ('single' attribute)
 * ==================================================================== */
int rsprod_attr_compareName(rsprod_attr *A,rsprod_attr *B);
rsprod_attr *rsprod_attr_copy(rsprod_attr *orig);
void rsprod_attr_delete(rsprod_attr *this);
void rsprod_attr_printInfo(rsprod_attr *attr);
int rsprod_attr_createWithAssignValues(rsprod_attr **this,char *name, rsprod_type type, size_t length, void *values);
int rsprod_attr_createWithCopyValues(rsprod_attr **this,char *name, rsprod_type type, size_t length, void *values);
int rsprod_attr_create(rsprod_attr **this,char *name, rsprod_type type,size_t length);
char *rsprod_attr_getName(rsprod_attr *this);
void *rsprod_attr_getValues(rsprod_attr *this);
size_t rsprod_attr_getNbvalues(rsprod_attr *this);
rsprod_type rsprod_attr_getType(rsprod_attr *this);
#define DefineAccessValuesFunctionPrototype(t) int  rsprod_attr_accessValues_ ## t (rsprod_attr *, t **)
DefineAccessValuesFunctionPrototype(double);
DefineAccessValuesFunctionPrototype(float);
DefineAccessValuesFunctionPrototype(char);
DefineAccessValuesFunctionPrototype(short);
DefineAccessValuesFunctionPrototype(int);
#undef DefineAccessValuesFunctionPrototype
#define DefineAccessValueFunctionPrototype(t) int  rsprod_attr_accessValue_ ## t (rsprod_attr *,size_t, t *)
DefineAccessValueFunctionPrototype(double);
DefineAccessValueFunctionPrototype(float);
DefineAccessValueFunctionPrototype(char);
DefineAccessValueFunctionPrototype(short);
DefineAccessValueFunctionPrototype(int);
#undef DefineAccessValueFunctionPrototype

/* ==============================================================
 *    RSPROD ATTRIBUTES  (list of attributes)
 * ============================================================== */
void rsprod_attributes_printInfo(rsprod_attributes this);
int  rsprod_attributes_getAttr(rsprod_attributes this, rsprod_attr **attr_p, char *attrname);
int  rsprod_attributes_addAttr(rsprod_attributes this, rsprod_attr *attr);
int  rsprod_attributes_addCopyAttr(rsprod_attributes this, rsprod_attr *attr);
int  rsprod_attributes_replaceAttr(rsprod_attributes this, rsprod_attr *attr);
int  rsprod_attributes_existsAttr(rsprod_attributes this, const char *name);
int  rsprod_attributes_deleteAttr(rsprod_attributes this, const char *name);
int  rsprod_attributes_copy(rsprod_attributes orig, rsprod_attributes *dest);
void rsprod_attributes_create(rsprod_attributes *attr);
void rsprod_attributes_delete(rsprod_attributes this);
#define DefineAccessValuesFunctionPrototype(t) int  rsprod_attributes_accessValues_ ## t (rsprod_attributes,char *,t **)
DefineAccessValuesFunctionPrototype(double);
DefineAccessValuesFunctionPrototype(float);
DefineAccessValuesFunctionPrototype(char);
DefineAccessValuesFunctionPrototype(short);
DefineAccessValuesFunctionPrototype(int);
#undef DefineAccessValuesFunctionPrototype
#define DefineAccessValueFunctionPrototype(t) int  rsprod_attributes_accessValue_ ## t (rsprod_attributes,char *,t *)
DefineAccessValueFunctionPrototype(double);
DefineAccessValueFunctionPrototype(float);
DefineAccessValueFunctionPrototype(char);
DefineAccessValueFunctionPrototype(short);
DefineAccessValueFunctionPrototype(int);
#undef DefineAccessValueFunctionPrototype

/* =================================================================== 
 *         RSPROD   FIELD  
 * =================================================================== */
int  rsprod_field_create(rsprod_field **this,char *name,rsprod_dimensions *dims,rsprod_attributes attr, rsprod_data *data);
void rsprod_field_delete(rsprod_field *this);
void rsprod_field_printInfo(rsprod_field *this);
int  rsprod_field_setName(rsprod_field *this, char *string);
rsprod_field *rsprod_field_copy(rsprod_field *orig);
int  rsprod_field_unpack(rsprod_field *this);
int  rsprod_field_pack(rsprod_field *this,void *scaleF,void *offsetF);
int rsprod_field_createStandard(rsprod_field **this, char *name, rsprod_type type, size_t nbvalues,
      rsprod_dimensions *dims, char *longName, char *stdName, char *units, void *fillv, void *min, void *max, 
      short addcoords, char *mapInfo, void *values);


#endif /* RSPROD_INTERN */

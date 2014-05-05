

#ifndef RSPROD_INTERN_H
#define RSPROD_INTERN_H

#define STARTMACRO do 
#define ENDMACRO while (0)

#include "rsprod.h"

#include "netcdf-3/netcdf.h"

/* 
 *    RSPROD_TYPE
 */

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

/* Define calibration functions */
#define DefineNoCalibrationFunctionPrototype(f,t) \
   void calibration_noeffectFrom_##f##_to_##t (t *r, f v,void *p1,void *p2)
DefineNoCalibrationFunctionPrototype(float,float);
DefineNoCalibrationFunctionPrototype(float,short);
DefineNoCalibrationFunctionPrototype(float,char);
DefineNoCalibrationFunctionPrototype(float,double);
DefineNoCalibrationFunctionPrototype(float,int);
DefineNoCalibrationFunctionPrototype(short,float);
DefineNoCalibrationFunctionPrototype(short,short);
DefineNoCalibrationFunctionPrototype(short,char);
DefineNoCalibrationFunctionPrototype(short,double);
DefineNoCalibrationFunctionPrototype(short,int);
DefineNoCalibrationFunctionPrototype(char,float);
DefineNoCalibrationFunctionPrototype(char,short);
DefineNoCalibrationFunctionPrototype(char,char);
DefineNoCalibrationFunctionPrototype(char,double);
DefineNoCalibrationFunctionPrototype(char,int);
DefineNoCalibrationFunctionPrototype(double,float);
DefineNoCalibrationFunctionPrototype(double,short);
DefineNoCalibrationFunctionPrototype(double,char);
DefineNoCalibrationFunctionPrototype(double,double);
DefineNoCalibrationFunctionPrototype(double,int);
DefineNoCalibrationFunctionPrototype(int,float);
DefineNoCalibrationFunctionPrototype(int,short);
DefineNoCalibrationFunctionPrototype(int,char);
DefineNoCalibrationFunctionPrototype(int,double);
DefineNoCalibrationFunctionPrototype(int,int);
#undef DefineNoCalibrationFunctionPrototype

#define DefineCalibrationType1FunctionPrototype(f,t) \
   void calibration_type1From_##f##_to_##t (t *r, f v, f *p1, f *p2)
DefineCalibrationType1FunctionPrototype(float,float);
DefineCalibrationType1FunctionPrototype(float,short);
DefineCalibrationType1FunctionPrototype(float,char);
DefineCalibrationType1FunctionPrototype(float,double);
DefineCalibrationType1FunctionPrototype(float,int);
DefineCalibrationType1FunctionPrototype(short,float);
DefineCalibrationType1FunctionPrototype(short,short);
DefineCalibrationType1FunctionPrototype(short,char);
DefineCalibrationType1FunctionPrototype(short,double);
DefineCalibrationType1FunctionPrototype(short,int);
DefineCalibrationType1FunctionPrototype(char,float);
DefineCalibrationType1FunctionPrototype(char,short);
DefineCalibrationType1FunctionPrototype(char,char);
DefineCalibrationType1FunctionPrototype(char,double);
DefineCalibrationType1FunctionPrototype(char,int);
DefineCalibrationType1FunctionPrototype(double,float);
DefineCalibrationType1FunctionPrototype(double,short);
DefineCalibrationType1FunctionPrototype(double,char);
DefineCalibrationType1FunctionPrototype(double,double);
DefineCalibrationType1FunctionPrototype(double,int);
DefineCalibrationType1FunctionPrototype(int,float);
DefineCalibrationType1FunctionPrototype(int,short);
DefineCalibrationType1FunctionPrototype(int,char);
DefineCalibrationType1FunctionPrototype(int,double);
DefineCalibrationType1FunctionPrototype(int,int);
#undef DefineCalibrationType1FunctionPrototype

#define DefineCalibrationType2FunctionPrototype(f,t) \
   void calibration_type2From_##f##_to_##t (t *r, f v, t *p1, t *p2)
DefineCalibrationType2FunctionPrototype(float,float);
DefineCalibrationType2FunctionPrototype(float,short);
DefineCalibrationType2FunctionPrototype(float,char);
DefineCalibrationType2FunctionPrototype(float,double);
DefineCalibrationType2FunctionPrototype(float,int);
DefineCalibrationType2FunctionPrototype(short,float);
DefineCalibrationType2FunctionPrototype(short,short);
DefineCalibrationType2FunctionPrototype(short,char);
DefineCalibrationType2FunctionPrototype(short,double);
DefineCalibrationType2FunctionPrototype(short,int);
DefineCalibrationType2FunctionPrototype(char,float);
DefineCalibrationType2FunctionPrototype(char,short);
DefineCalibrationType2FunctionPrototype(char,char);
DefineCalibrationType2FunctionPrototype(char,double);
DefineCalibrationType2FunctionPrototype(char,int);
DefineCalibrationType2FunctionPrototype(double,float);
DefineCalibrationType2FunctionPrototype(double,short);
DefineCalibrationType2FunctionPrototype(double,char);
DefineCalibrationType2FunctionPrototype(double,double);
DefineCalibrationType2FunctionPrototype(double,int);
#undef DefineCalibrationType2FunctionPrototype


/*         
 *         NOTYPE      
 */

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
int rsprod_notype_castToType(rsprod_notype *this,rsprod_type newtype,void *fval1,void *fval2,void *p1,void *p2);

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

/* Internal functions relative to netcdf interface */
nc_type convertType_rsprod2nc(rsprod_type type);
rsprod_type convertType_nc2rsprod(nc_type type);

/* Internal functions relative to dimensions */
int rsprod_dims_addOne(rsprod_dimensions *this, short position, char *dimName, unsigned int dimLength, short dimOrder);
int rsprod_dims_setNames(rsprod_dimensions *this,const unsigned short nbNames, char *names[]);
int rsprod_dims_setLengths(rsprod_dimensions *this,const unsigned short nbLengths, unsigned int *lengths);
int rsprod_dims_setOrders(rsprod_dimensions *this,const unsigned short nbOrders, short *orders);

/* 
 *     RSPROD   ATTR   ('single' attribute)
 */
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

/*
 *    RSPROD ATTRIBUTES  (list of attributes)
 */

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
#define DefineAccessValuesFunctionPrototype(t) int  rsprod_attributes_accessValues_ ## t (rsprod_attributes, t **)
DefineAccessValuesFunctionPrototype(double);
DefineAccessValuesFunctionPrototype(float);
DefineAccessValuesFunctionPrototype(char);
DefineAccessValuesFunctionPrototype(short);
DefineAccessValuesFunctionPrototype(int);
#undef DefineAccessValuesFunctionPrototype
#define DefineAccessValueFunctionPrototype(t) int  rsprod_attributes_accessValue_ ## t (rsprod_attributes, t *)
DefineAccessValueFunctionPrototype(double);
DefineAccessValueFunctionPrototype(float);
DefineAccessValueFunctionPrototype(char);
DefineAccessValueFunctionPrototype(short);
DefineAccessValueFunctionPrototype(int);
#undef DefineAccessValueFunctionPrototype


#endif

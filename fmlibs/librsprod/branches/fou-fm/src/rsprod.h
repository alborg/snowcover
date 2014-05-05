
#ifndef RSPROD_H
#define RSPROD_H

typedef unsigned char byte;

/*
#define TypedefObj(b) \
   typedef struct b { \
      ObjData_   ## b \
      b ## _methods *methods;\
   } b

#define TypedefObjData(a) \
   typedef struct a ## _data { \
      ObjData_ ## a \
   } a ## _data

#define TypedefObjMethods(a) \
   typedef struct a ## _methods { \
      ObjMethods_ ## a \
   } a ## _methods
*/

#include "list.h"

/* internal types */
typedef unsigned short rsprod_type;
#define RSPROD_FLOAT         0
#define RSPROD_SHORT         1
#define RSPROD_CHAR          2
#define RSPROD_DOUBLE        3
#define RSPROD_INT           4
#define RSPROD_NBTYPES       5
#define RSPROD_NAT           RSPROD_NBTYPES

/* fill values */
#define RSPROD_FILLVAL_FLOAT    -999.999
#define RSPROD_FILLVAL_SHORT    -999
#define RSPROD_FILLVAL_CHAR     '\0'
#define RSPROD_FILLVAL_DOUBLE   RSPROD_FILLVAL_FLOAT
#define RSPROD_FILLVAL_INT      RSPROD_FILLVAL_SHORT


typedef struct rsprod_notype_ {
   void        *values; 
   rsprod_type  type; 
   size_t       nbvalues;
} rsprod_notype_;

typedef struct rsprod_notype_methods {
   int   (*accessValues)();              
   int   (*accessValue)();               
   int   (*castTo[RSPROD_NBTYPES])();
   void  (*calibrationType1[RSPROD_NBTYPES])();
} rsprod_notype_methods;

typedef struct rsprod_notype {
   struct rsprod_notype_        content;
   struct rsprod_notype_methods *methods;
} rsprod_notype;

typedef struct rsprod_dimensions {
   unsigned short  nbdims;
   char            **name;    
   unsigned int    *length;
   unsigned short  *rank;
   short           *order;
   unsigned long   totelems;
} rsprod_dimensions;


typedef struct rsprod_attr_ {
   char                *name; 
   rsprod_notype       *notype;
} rsprod_attr_;

typedef struct rsprod_attr_methods {
   int  (*accessValues)();              
   int  (*accessValue)();               
} rsprod_attr_methods;

typedef struct rsprod_attr {
   rsprod_attr_         content;
   rsprod_attr_methods *methods;
} rsprod_attr;

/* the data part of a field is no more than a notype */
typedef rsprod_notype rsprod_data;
/* fields usually have several attributes, stored in a List */
typedef List rsprod_attributes;

typedef struct rsprod_field {
   char               *name;
   rsprod_dimensions  *dims;
   rsprod_attributes   attr;
   rsprod_data        *data;
} rsprod_field;

int  rsprod_field_create(rsprod_field **this,char *name,rsprod_dimensions *dims,rsprod_attributes attr, rsprod_data *data);
void rsprod_field_delete(rsprod_field *this);
void rsprod_field_printInfo(rsprod_field *this);
int  rsprod_field_setName(rsprod_field *this, char *string);
rsprod_field *rsprod_field_copy(rsprod_field *orig);
int  rsprod_field_unpack(rsprod_field *this);
int rsprod_field_createStandard(rsprod_field **this, char *name, rsprod_type type, size_t nbvalues,
      rsprod_dimensions *dims, char *longName, char *stdName, char *units, void *fillv, void *min, void *max, 
      short addcoords, char *mapInfo, void *values);

/* NETCDF interface */
int  rsprod_field_loadFromNetCDF(rsprod_field **this,char *fieldname,const int ncid);
int  rsprod_field_writeToNetCDF(rsprod_field *this,const int ncid);




/* Data */
#define rsprod_data_create(d,t,s)                             rsprod_notype_create((rsprod_notype **)d,t,s)
#define rsprod_data_createWithCopyValues(d,t,s,v)             rsprod_notype_createWithCopyValues((rsprod_notype **)d,t,s,v)
#define rsprod_data_createWithAssignValues(d,t,s,v)           rsprod_notype_createWithAssignValues((rsprod_notype **)d,t,s,v)
#define rsprod_data_delete(d)                                 rsprod_notype_delete((rsprod_notype *)d)
#define rsprod_data_getVoidStar(d,v)                          rsprod_notype_getVoidStar((rsprod_notype *)d,v)
#define rsprod_data_copy(d)                                   rsprod_notype_copy((rsprod_notype *)d)
#define rsprod_data_printInfo(d)                              rsprod_notype_printInfo((rsprod_notype *)d)
#define rsprod_data_castToType(d,t,v1,v2,p1,p2)               rsprod_notype_castToType((rsprod_notype *)d,t,v1,v2,p1,p2)
#define rsprod_data_getType(d)                                rsprod_notype_getType((rsprod_notype *)d)
#define rsprod_data_getNbvalues(d)                            rsprod_notype_getNbvalues((rsprod_notype *)d)
#define rsprod_data_getValues(d)                              rsprod_notype_getValues((rsprod_notype *)d)

/* Dimensions */
int rsprod_dims_create(rsprod_dimensions **this,const unsigned short nbdims,char **dimsName,unsigned int *dimsLength,short *dimsOrder);
void rsprod_dims_delete(rsprod_dimensions *this);
rsprod_dimensions *rsprod_dims_copy(rsprod_dimensions *orig);
void rsprod_dims_printInfo(rsprod_dimensions *this);
int  rsprod_dims_compare(rsprod_dimensions *one,rsprod_dimensions *two);
unsigned long vecIndex(rsprod_dimensions *this, unsigned short nbMatIndex, long *MatIndex);
unsigned long vecIndexRelative(rsprod_dimensions *this, unsigned long current, unsigned short nbMatIndex, long *MatIndex);


#endif

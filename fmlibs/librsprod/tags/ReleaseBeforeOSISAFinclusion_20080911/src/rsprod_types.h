
#ifndef RSPROD_TYPES_H
#define RSPROD_TYPES_H

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
#define RSPROD_FILLVAL_CHAR     '~'
#define RSPROD_FILLVAL_DOUBLE   RSPROD_FILLVAL_FLOAT
#define RSPROD_FILLVAL_INT      RSPROD_FILLVAL_SHORT

/* =========================================================
 *     RSPROD   NOTYPE   
 * ========================================================= */
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
   void  (*calibrationType2[RSPROD_NBTYPES])();
} rsprod_notype_methods;

typedef struct rsprod_notype {
   struct rsprod_notype_        content;
   struct rsprod_notype_methods *methods;
} rsprod_notype;

/* =========================================================
 *     RSPROD   DIMENSIONS
 * ========================================================= */
typedef struct rsprod_dimensions {
   unsigned short  nbdims;
   char            **name;    
   unsigned int    *length;
   unsigned short  *rank;
   short           *order;
   unsigned long   totelems;
} rsprod_dimensions;

/* =========================================================
 *     RSPROD   ATTR
 * ========================================================= */
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

/* =========================================================
 *     RSPROD   DATA
 * ========================================================= */
/* the data object is no more than a notype */
typedef rsprod_notype rsprod_data;

/* =========================================================
 *     RSPROD   ATTRIBUTES
 * ========================================================= */
/* fields usually have several attributes, stored in a List */
typedef List rsprod_attributes;

/* =========================================================
 *     RSPROD   FIELD
 * ========================================================= */
typedef struct rsprod_field {
   char               *name;
   rsprod_dimensions  *dims;
   rsprod_attributes   attr;
   rsprod_data        *data;
} rsprod_field;

#endif

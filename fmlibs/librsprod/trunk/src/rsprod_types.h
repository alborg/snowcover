
#ifndef RSPROD_TYPES_H
#define RSPROD_TYPES_H

#include <dbl_list.h>
#include "rsprod_typedefs.h"

/* internal types */
typedef unsigned short rsprod_type;
#define RSPROD_FLOAT         0
#define RSPROD_SHORT         1
#define RSPROD_CHAR          2
#define RSPROD_DOUBLE        3
#define RSPROD_INT           4
#define RSPROD_BYTE          5
#define RSPROD_NBTYPES       6
#define RSPROD_NAT           RSPROD_NBTYPES

/* fill values */
#define RSPROD_FILLVAL_FLOAT    -999.999
#define RSPROD_FILLVAL_SHORT    -999
#define RSPROD_FILLVAL_BYTE     -128
#define RSPROD_FILLVAL_CHAR     '~'
#define RSPROD_FILLVAL_DOUBLE   RSPROD_FILLVAL_FLOAT
#define RSPROD_FILLVAL_INT      RSPROD_FILLVAL_SHORT
extern void *FillValues[];

/* unity values */
#define RSPROD_UNITY_FLOAT    1.
#define RSPROD_UNITY_SHORT    1
#define RSPROD_UNITY_BYTE     1
#define RSPROD_UNITY_CHAR     1 /* anything could go here, we will never attempt to handle a char as numeric value*/
#define RSPROD_UNITY_DOUBLE   RSPROD_UNITY_FLOAT
#define RSPROD_UNITY_INT      RSPROD_UNITY_SHORT
extern void *UnityValues[];

/* zero values */
#define RSPROD_ZERO_FLOAT    0.
#define RSPROD_ZERO_SHORT    0
#define RSPROD_ZERO_BYTE     0
#define RSPROD_ZERO_CHAR     0 /* anything could go here, we will never attempt to handle a char as numeric value*/
#define RSPROD_ZERO_DOUBLE   RSPROD_ZERO_FLOAT
#define RSPROD_ZERO_INT      RSPROD_ZERO_SHORT
extern void *ZeroValues[];

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
   int   (*initValues)();               
   int   (*sprintfValue)();               
   int   (*castTo[RSPROD_NBTYPES])();
   void  (*calibrationNoEffect[RSPROD_NBTYPES])();
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
   short           *unlimited;
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
   int  (*sprintfValue)();               
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
typedef dbl_list rsprod_attributes;

/* =========================================================
 *     RSPROD   FIELD
 * ========================================================= */
typedef struct rsprod_field {
   char               *name;
   rsprod_dimensions  *dims;
   rsprod_attributes  *attr;
   rsprod_data        *data;
} rsprod_field;

/* =========================================================
 *     RSPROD   FILE
 * ========================================================= */
typedef struct rsprod_file {
   dbl_list          *datasets;
   rsprod_attributes *glob_attr;
} rsprod_file;
#endif

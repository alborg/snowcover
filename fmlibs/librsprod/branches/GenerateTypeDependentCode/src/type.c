/*
 * NAME: type.c
 * 
 * PURPOSE: 
 *    Implement the 'rsprod_type' actions and variables. Define if a type is
 *    valid, get its 'sizeof()' value, etc...
 *
 * DESCRIPTION:
 *    The 'rsprod_type' is seen as a higher level implementation of interface dependent
 *    NC_* and H5_* (for example). It is an extra layer between the user and the 
 *    formatted interface (netcdf, hdf4, hdf5, etc...) which allows a better 
 *    flexibility when reading/writing from/to those files.
 *
 *    A type information is needed each time we want to use the value of a (void *)
 *    data, either the dataset itself (rsprod_data) or for attributes to 
 *    a dataset (rsprod_attr).
 *    
 * NOTE:
 *    As a first version, each RSPROD_ type has been directly linked to native C type (e.g. 
 *    RSPROD_DOUBLE is implemented as a 'double'). This is enough as long as we 
 *    stay on a 32bits architecture and we avoid types which have no direct
 *    equivalent in C (for example NC_BYTE). Later, a 'configure' step could/should
 *    be implemented to match each type in the various interfaces and to a native 
 *    C type on the current platform.
 *    
 * DEPENDENCIES:
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
#include "rsprod_string_utils.h"

/* Global arrays which can always be referred to to get the sizeof(), name and conversion possibility */
/* WARNING!!!! Those arrays are initialised in the same order as the RSPROD_ macros have been written
 *             in header rsprod.h. This is to ensure that the values correspond to right indexes
 */

/* sizeof() operator */
const size_t SizeOf[RSPROD_NBTYPES] = {sizeof(Float),sizeof(Short),sizeof(Char),sizeof(Double),sizeof(Int),sizeof(Byte)};
/* name of the (native C) type associated to RSPROD_ */
const char*  TypeName[RSPROD_NBTYPES] = {"Float","Short","Char","Double","Int","Byte"};
/* binary answer to the question: Can I convert (through calibration_factor, calibration_offset and type casting) 
 *    type RSPROD_TYPE1 to RSPROD_TYPE2.
 *    Most (currently all) numeric types can be converted from/to but not RSPROD_CHAR.
 */
const short  TypeConvert[RSPROD_NBTYPES][RSPROD_NBTYPES] = {{1,1,0,1,1},{1,1,0,1,1},{0,0,1,0,0},{1,1,0,1,1},{1,1,0,1,1},{1,1,0,1,1}};
/* simple type-to-string formatting codes for printf-like commandoes */
const char*  TypePrintFmt[RSPROD_NBTYPES] = {"%f","%d","%c","%f","%d","%d"};
void *FillValues[RSPROD_NBTYPES]  = {NULL,NULL,NULL,NULL,NULL,NULL};
void *UnityValues[RSPROD_NBTYPES] = {NULL,NULL,NULL,NULL,NULL,NULL};
void *ZeroValues[RSPROD_NBTYPES]  = {NULL,NULL,NULL,NULL,NULL,NULL};

/*
 * NAME : isValidType()
 *
 * PURPOSE:
 *    returns 1 is the rsprod_type is valid.
 *    returns 0 otherwise.
 *
 * NOTE:
 *    Although RSPROD_NAT (not-a-type) is valid per se, no processing
 *    operation can be performed on it and we thus return 0.
 */
int isValidType(rsprod_type type) {

   if (type == RSPROD_NAT) return 0;
   if (type != RSPROD_FLOAT && 
	 type != RSPROD_SHORT &&
	 type != RSPROD_CHAR &&
	 type != RSPROD_DOUBLE &&
	 type != RSPROD_INT && 
     type != RSPROD_BYTE) return 0;
   return 1;
}

/*
 * NAME : typeToFormat()
 *
 * PURPOSE:
 *    Return a string corresponding to the default printf-like format
 *    for each type.
 *
 * NOTE: 
 *    o The string has been allocated: the user should free() it when he is
 *      done using it.
 *    o This could be implemented in an array (as SizeOf[] and Name[] see above).
 *
 */
int typeToFormat(rsprod_type type, char **format) {

   int ret = 0;
   if (!isValidType(type))
      return 1;
   
   switch (type) {
      case RSPROD_DOUBLE:
	 ret = rsprod_string_trim_allNullsExceptLast("%05.3f",format);
	 break;
      case RSPROD_FLOAT:
	 ret = rsprod_string_trim_allNullsExceptLast("%05.3f",format);
	 break;
      case RSPROD_SHORT:
	 ret = rsprod_string_trim_allNullsExceptLast("%03d",format);
	 break;
      case RSPROD_CHAR:
	 ret = rsprod_string_trim_allNullsExceptLast("%s",format);
	 break;
      case RSPROD_INT:
	 ret = rsprod_string_trim_allNullsExceptLast("%03d",format);
	 break;   
   }
   return ret;

}

rsprod_type qTypeToken(char qtt) {
   
   rsprod_type rtype;
   
   switch (qtt) {
      case 'f':
         rtype = RSPROD_FLOAT;
         break;
      case 'd':
         rtype = RSPROD_DOUBLE;
         break;
      case 's':
         rtype = RSPROD_SHORT;
         break;
      case 'i':
         rtype = RSPROD_INT;
         break;
      case 'c':
         rtype = RSPROD_CHAR;
         break;
      case 'b':
         rtype = RSPROD_BYTE;
         break;
      default:
         rtype = RSPROD_NAT;
         break;
   }
   return rtype;
}

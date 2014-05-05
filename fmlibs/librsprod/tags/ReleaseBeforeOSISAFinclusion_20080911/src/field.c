/*
 * NAME: field.c
 * 
 * PURPOSE: 
 *    Implement the functionalities of a field object. 
 *
 * DESCRIPTION:
 *    In rsprod, a field is the top-level object as it is composed of dimensions, 
 *    attributes and data, plus a name. 
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    Thomas Lavergne, met.no, 23.04.2008    :    add the rsprod_field_pack() routine.
 *
 */ 

#include <stdio.h> 
#include <stdlib.h> 
#include <string.h>
#include "calibration.h"
#include "rsprod_intern.h"
#include "rsprod_memory_utils.h"
#include "rsprod_string_utils.h"

/* 
 * NAME : rsprod_field_create
 *
 * PURPOSE:
 *    Allocate and initialize a rsprod_field object with the given name, dimensions, attributes and data.
 *
 * NOTE:
 *    The various parameters are 'pointed to' so they must not be de-allocated by the calling routine.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */ 
int rsprod_field_create(rsprod_field **this,char *name,rsprod_dimensions *dims,rsprod_attributes attr,rsprod_data *data) {

   rsprod_field *tmp = rsprodMalloc(sizeof(rsprod_field));
   if (rsprod_field_setName(tmp,name)) {
      fprintf(stderr,"ERROR (rsprod_field_create) Could not set name %s to field object.\n",name);
      return 1;
   }
   tmp->attr = attr;
   tmp->dims = dims;
   tmp->data = data;

   if (!(tmp->dims) && (tmp->data)) {
      fprintf(stderr,"ERROR (rsprod_field_create) Field %s has no dimension but some data.\n",name);
      return 1;
   }
   if ((tmp->dims) && !(tmp->data)) {
      fprintf(stderr,"ERROR (rsprod_field_create) Field %s has some dimensions but no data.\n",name);
      return 1;
   }

   *this = tmp;

   return 0;
}

/* 
 * NAME : rsprod_field_delete
 *
 * PURPOSE:
 *    De-allocate the memory space of the field (and of its elements).
 *
 * NOTE:
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */ 
void rsprod_field_delete(rsprod_field *this) {
   rsprod_data_delete(this->data);
   rsprod_attributes_delete(this->attr);
   rsprod_dims_delete(this->dims);
   free(this->name);

}

/* 
 * NAME : rsprod_field_printInfo
 *
 * PURPOSE:
 *    Print some info about the field (and its elements)
 *
 * NOTE:
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */ 
void rsprod_field_printInfo(rsprod_field *this) {
   fprintf(stderr,"Field (%s):\n",this->name);
   rsprod_dims_printInfo(this->dims);
   rsprod_attributes_printInfo(this->attr);
   rsprod_data_printInfo(this->data);
}

/* 
 * NAME : rsprod_field_setName
 *
 * PURPOSE:
 *    Copy the name (char *string) in the 'name' element of the field.
 *
 * NOTE:
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */ 
int rsprod_field_setName(rsprod_field *this, char *string) {

   char *name;
   if (rsprod_string_trim_allNullsExceptLast(string,&name)) {
      fprintf(stderr,"ERROR (rsprod_field_setName) Problems dealing with null-characters in name.\n");
      return 1;
   }
   this->name = name;
   return 0;
}

/* 
 * NAME : rsprod_field_unpack
 *
 * PURPOSE:
 *    'Unpack' the given field to a float-typed field, using the calibration
 *    information in attributes 'scale_factor' and 'add_offset'. This routine
 *    modifies the type of the data, the data value itself and the list of 
 *    attributes (the 'scale_factor' and 'add_offset' are removed).
 *
 * NOTE:
 *    We should provide a way to customize this routine. For example, the user
 *    should be able to choose the 'output' fillvalue and type. Either through
 *    specific subroutines entry point (e.g. _unpack_to_double()) or through
 *    a global "rsprod_options" object: (set_unpack_outputType(double); 
 *                                       rsprod_field_unpack(f);)
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */ 
int rsprod_field_unpack(rsprod_field *this) {

   /* data object */
   rsprod_data *data = this->data;

   /* we need 4 parameters: the 2 fill values, the scale and the offset */
   rsprod_attributes list = this->attr;
  
   /* Get the SCALE FACTOR (scale_factor) and the OFFSET (add_offset) of the calibration */
   rsprod_attr *attrScaleFactor;
   void *scaleFac = NULL;
   if (rsprod_attributes_getAttr(list,&attrScaleFactor,"scale_factor")) {
      fprintf(stderr,"WARNING (rsprod_field_unpack) cannot find the scale_factor for field %s.\n",this->name);
      size_t sizeType = SizeOf[RSPROD_FLOAT];
      scaleFac  = rsprodMalloc(1*sizeType);
      float unity = 1.;
      memcpy(scaleFac,&unity,sizeType);
      attrScaleFactor = NULL;
   } else {
      scaleFac = rsprod_attr_getValues(attrScaleFactor);
   }
   
   rsprod_attr *attrOffset = NULL;
   void *offset = NULL;
   if (rsprod_attributes_getAttr(list,&attrOffset,"offset")) {
      size_t sizeType = SizeOf[(attrScaleFactor==NULL?RSPROD_FLOAT:rsprod_attr_getType(attrScaleFactor))];
      offset  = rsprodMalloc(1*sizeType);
      memset(offset,0,sizeType);
   } else {
       offset = rsprod_attr_getValues(attrOffset);
   }

   /* check types */
   if ( (attrOffset!=NULL) && (attrScaleFactor!=NULL) && (rsprod_attr_getType(attrOffset) != rsprod_attr_getType(attrScaleFactor))) {
      fprintf(stderr,"ERROR (rsprod_field_unpack) The scale factor and offset do not have the same type.\n");
      return 1;
   }

   /* Get the 'original' fillvalue */
   rsprod_attr *attrFillVal1;
   void *fillval1 = NULL;
   if (rsprod_attributes_getAttr(list,&attrFillVal1,"_FillValue")) {
      fprintf(stderr,"WARNING (rsprod_field_unpack) cannot find the _FillValue for field %s.\n",this->name);
   } else if (rsprod_attr_getType(attrFillVal1) != rsprod_data_getType(data)) {
      fprintf(stderr,"ERROR (rsprod_field_unpack) the _FillValue has not the same type as the data object.\n");
      return 1;
   } else {
      fillval1 = rsprod_attr_getValues(attrFillVal1);
   }
   float fillval_float = RSPROD_FILLVAL_FLOAT;
   void *fillval2 = &fillval_float;

   /* for now, only 'type1' calibration is implemented: scale and offset should have the same type as data */
   if ((attrScaleFactor!=NULL) && (rsprod_attr_getType(attrScaleFactor) == rsprod_data_getType(data))) {
      fprintf(stderr,"ERROR (rsprod_field_unpack) 'Type 1' calibration demands type(scale) == type(offset) == type(data).\n");
      return 1;
   }

   /* now we can apply the unpacking */
   if (rsprod_data_castToType(data,RSPROD_FLOAT,RSPROD_CALIBRATION_TYPE2,fillval1,fillval2,scaleFac,offset)) {
      fprintf(stderr,"ERROR (rsprod_field_unpack) while performing unpacking of %s.\n",this->name);
      return 1;
   }

   /* delete the now un-necessary scale and offset attributes and update the fill value */
   rsprod_attr *newFillValue;
   if (rsprod_attr_createWithCopyValues(&newFillValue,"_FillValue",RSPROD_FLOAT,1,fillval2)) {
       fprintf(stderr,"ERROR (rsprod_field_unpack) cannot create the new fill value parameter.\n");
       return 1;
   }
   if (rsprod_attributes_replaceAttr(list,newFillValue)) {
       fprintf(stderr,"ERROR (rsprod_field_unpack) cannot insert the new fill value attribute in the list.\n");
       return 1;
   }
   if (rsprod_attributes_deleteAttr(list,"scale_factor")) {
       fprintf(stderr,"WARNING (rsprod_field_unpack) cannot remove the old 'scale_factor' from the list of attributes.\n");
   }
   if (rsprod_attributes_deleteAttr(list,"add_offset")) {
       fprintf(stderr,"WARNING (rsprod_field_unpack) cannot remove the old 'add_offset' from the list of attributes.\n");
   }

   return 0;
}

/* 
 * NAME : rsprod_field_pack
 *
 * PURPOSE:
 *    'Pack' (compress) the given field to a short-typed field, using the calibration
 *    information given as parameters. Attributes 'scale_factor' and 'add_offset' are added
 *    to those of the field. This routine modifies the type of the data, the data value itself 
 *    and the list of attributes (the 'scale_factor' and 'add_offset' are added).
 *
 * NOTE:
 *    o Notes from rsprod_field_unpack() apply here.
 *    o Only RSPROD_SHORT is accepted as output type.
 *    o The calibration parameters are interpreted as FLOATs.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 23.04.2008
 *
 * MODIFIED:
 *
 */ 
int rsprod_field_pack(rsprod_field *this, void *scaleP, void *offsetP) {

   /* check that the 'destination' type is valid */
   rsprod_type toType = RSPROD_SHORT;
   if (toType != RSPROD_SHORT) {
      fprintf(stderr,"ERROR (rsprod_field_pack) unpacking is currently only to type short (RSPROD_SHORT).\n");
      return 1;
   }

   /* data object */
   rsprod_data *data = this->data;
   rsprod_type fromType = rsprod_data_getType(data);
   if (fromType != RSPROD_FLOAT) {
      fprintf(stderr,"ERROR (rsprod_field_pack) The packing is currently only from type float (RSPROD_FLOAT).\n");
   }


   /* Get the SCALE FACTOR (scale_factor) and the OFFSET (add_offset) of the calibration. */
   /* Interpret them as 'fromType' (FLOAT) */
   printf("Sizeof fromType is %u (%u %u)\n",SizeOf[fromType],sizeof(float),sizeof(short));
   float *scaleFac = rsprodMalloc(SizeOf[fromType]);
   memcpy(scaleFac,scaleP,SizeOf[fromType]);   
   float *offset = rsprodMalloc(SizeOf[fromType]);
   memcpy(offset,offsetP,SizeOf[fromType]);   
   printf("ScaleFac: %f\n",*scaleFac);
   printf("offset: %f\n",*offset);

   rsprod_attributes list = this->attr;
   /* Get the 'original' fillvalue */
   rsprod_attr *attrFillVal1;
   void *fillval1 = NULL;
   if (rsprod_attributes_getAttr(list,&attrFillVal1,"_FillValue")) {
      fprintf(stderr,"WARNING (rsprod_field_pack) cannot find the _FillValue for field %s.\n",this->name);
   } else if (rsprod_attr_getType(attrFillVal1) != rsprod_data_getType(data)) {
      fprintf(stderr,"ERROR (rsprod_field_pack) the _FillValue has not the same type as the data object.\n");
      return 1;
   } else {
      fillval1 = rsprod_attr_getValues(attrFillVal1);
   }
   short fillval_short = RSPROD_FILLVAL_SHORT;
   void *fillval2 = &fillval_short;

   printf("PACKING: (float->short) parameters {%f %f} are: %f %f (%f : %d)\n",
	 *((float *)scaleP),
	 *((float *)offsetP),
	 *scaleFac,
	 *offset, 
	 *((float *)fillval1),
	 *((short *)fillval2));


   /* now we can apply the packing/compressing */
   if (rsprod_data_castToType(data,RSPROD_SHORT,RSPROD_CALIBRATION_TYPE1,fillval1,fillval2,scaleP,offsetP)) {
      fprintf(stderr,"ERROR (rsprod_field_pack) while performing packing of %s.\n",this->name);
      return 1;
   }

   /* replace/create the scale and offset attributes and update the fill value */
   rsprod_attr *newFillValue;
   if (rsprod_attr_createWithCopyValues(&newFillValue,"_FillValue",RSPROD_SHORT,1,fillval2)) {
       fprintf(stderr,"ERROR (rsprod_field_pack) cannot create the new fill value parameter.\n");
       return 1;
   }
   if (rsprod_attributes_replaceAttr(list,newFillValue)) {
       fprintf(stderr,"ERROR (rsprod_field_pack) cannot insert the new fill value attribute in the list.\n");
       return 1;
   }

   rsprod_attr *newScaleFactor;
   if (rsprod_attr_createWithCopyValues(&newScaleFactor,"scale_factor",RSPROD_FLOAT,1,scaleFac)) {
       fprintf(stderr,"ERROR (rsprod_field_pack) cannot create the new scale_factor parameter.\n");
       return 1;
   }
   if (rsprod_attributes_replaceAttr(list,newScaleFactor)) {
       fprintf(stderr,"WARNING (rsprod_field_pack) cannot insert the new 'scale_factor' from the list of attributes.\n");
       return 1;
   }

   rsprod_attr *newAddOffset;
   if (rsprod_attr_createWithCopyValues(&newAddOffset,"add_offset",RSPROD_FLOAT,1,offset)) {
       fprintf(stderr,"ERROR (rsprod_field_pack) cannot create the new add_offset parameter.\n");
       return 1;
   }
   if (rsprod_attributes_replaceAttr(list,newAddOffset)) {
       fprintf(stderr,"WARNING (rsprod_field_pack) cannot insert the new 'add_offset' from the list of attributes.\n");
       return 1;
   }
   return 0;

}

/* 
 * NAME : rsprod_field_copy
 *
 * PURPOSE:
 *    Copy a whole field object.
 *
 * NOTE:
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */ 
rsprod_field *rsprod_field_copy(rsprod_field *orig) {

   int ret = 0;

   rsprod_dimensions *dims;
   if (orig->dims) {
      dims = rsprod_dims_copy(orig->dims);
      if (dims == NULL) {
	 fprintf(stderr,"ERROR (rsprod_field_copy) cannot generate copies of the Dimensions object.\n");
	 return NULL;
      }
   } else {
      dims = NULL;
   }
   rsprod_attributes attr;
   ret += rsprod_attributes_copy(orig->attr,&attr);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_copy) cannot generate copies of the Attributes object.\n");
      return NULL;
   }
   rsprod_data *data;
   data = rsprod_data_copy(orig->data);
   if (data == NULL) {
      fprintf(stderr,"ERROR (rsprod_field_copy) cannot generate copies of the Data object.\n");
      return NULL;
   }

   rsprod_field *dest;
   ret = rsprod_field_create (&dest,orig->name,dims,attr,data);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_copy) cannot create a new Field object.\n");
      return NULL;
   }

   return dest;
}


unsigned long getVecElement(rsprod_field *this,unsigned short nbMatIndex, unsigned long *MatIndex) {
   return (vecIndex(this->dims,nbMatIndex,MatIndex));
}

/* 
 * NAME : rsprod_field_createStandard
 *
 * PURPOSE :
 *    Create a 'standard' field with many attributes, the dimensions and the data.
 *    This 'standard' is very close to netCDF CF-1.x.
 *
 * INPUT :
 *    name             -> name of the dataset/field ("ice_conc", "tb85v", etc...)
 *    type             -> type of the data (RSPROD_FLOAT, RSPROD_SHORT, etc...)
 *    nbvalues         -> number of values in data
 *    dims             -> pointer to an already formatted rsprod_dimensions object.
 *    longName  [opt]  -> Attribute 'long_name'. Not put if NULL.
 *    stdName   [opt]  -> Attribute 'std_name'. Not put if NULL.
 *    units     [opt]  -> Attribute 'units'. Not put if NULL.
 *    fillv     [opt]  -> Attribute '_FillValue'. Not put if NULL.
 *    min       [opt]  -> Attribute 'valid_min'. Not put if NULL.
 *    min       [opt]  -> Attribute 'valid_max'. Not put if NULL.
 *    addCoords [y/n]  -> Attribute 'coordinates' is added (if addCoords is 1) or not. 
 *                        Its value is the list of the dimensions (e.g. coordinates: "yc xc").
 *    mapInfo   [opt]  -> Attribute 'grid_mapping'. Not put if NULL.
 *    values           -> pointer to the data array.
 *
 * OUTPUT :
 *    after the call, 'this' points to the new rsprod_field object.
 *
 * EXAMPLE :
 *
 *    rsprod_field *f;
 *    ret = rsprod_field_createStandard(&f,...);
 *    if (ret) { [ERROR-HANDLING]; }
 *
 * NOTE :
 *
 * AUTHOR :
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED :
 *    TL, met.no, 02.04.2008      : Added 'mapInfo' and 'addCoords' to conform to CF-1.x (projection information).
 *                                  Most of the attributes are now 'optional' (nothing happens if given NULL).
 *
 *
 */
int rsprod_field_createStandard(rsprod_field **this, char *name, rsprod_type type, size_t nbvalues,
      rsprod_dimensions *dims, char *longName, char *stdName, char *units, void *fillv, void *min, void *max, 
      short addCoords, char *mapInfo, void *values) {

   int ret;

   TestValidType(type);

   /* dimensions */
   rsprod_dimensions *cdims = rsprod_dims_copy(dims);
   if (cdims == NULL) {
      fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot duplicate Dimension object.\n");
      return 1;
   }

   /* attributes */
   rsprod_attributes list;
   rsprod_attributes_create(&list);
   rsprod_attr *attr;
   ret = 0;
   /** long_name [opt] **/
   if (longName) {
      if (rsprod_attr_createWithCopyValues(&attr,"long_name",RSPROD_CHAR,strlen(longName),longName)) {
	 fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create attribute long_name.\n");
	 ret++;
      }
      ret += rsprod_attributes_addCopyAttr(list,attr); rsprod_attr_delete(attr);
   }
   /** standard_name [opt] **/
   if (stdName) {
      if (rsprod_attr_createWithCopyValues(&attr,"standard_name",RSPROD_CHAR,strlen(stdName),stdName)) {
	 fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create attribute std_name.\n");
	 ret++;
      }
      ret += rsprod_attributes_addCopyAttr(list,attr); rsprod_attr_delete(attr);
   }
   /** units [opt] **/
   if (units) {
      if (rsprod_attr_createWithCopyValues(&attr,"units",RSPROD_CHAR,strlen(units),units)) {
	 fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create attribute units.\n");
	 ret++;
      }
      ret += rsprod_attributes_addCopyAttr(list,attr);rsprod_attr_delete(attr);
   }
   /** _FillValue [opt] **/
   if (fillv) {
      if (rsprod_attr_createWithCopyValues(&attr,"_FillValue",type,1,fillv)) {
	 fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create attribute _FillValue.\n");
	 ret++;
      }
      ret += rsprod_attributes_addCopyAttr(list,attr);rsprod_attr_delete(attr);
   }
   /** valid_min [opt] **/
   if (min) {
      if ( rsprod_attr_createWithCopyValues(&attr,"valid_min",type,1,min) ) {
         fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create attribute valid_min.\n");
         ret++;
      }
      ret += rsprod_attributes_addCopyAttr(list,attr);rsprod_attr_delete(attr);
   }
   /** valid_max [opt] **/
   if (max) {
      if ( rsprod_attr_createWithCopyValues(&attr,"valid_max",type,1,max) ) {
	 fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create attribute valid_max.\n");
	 ret++;
      }
      ret += rsprod_attributes_addCopyAttr(list,attr);rsprod_attr_delete(attr);
   }
   /** grid_mapping [opt] **/
   if (mapInfo) {
      if ( rsprod_attr_createWithCopyValues(&attr,"grid_mapping",RSPROD_CHAR,strlen(mapInfo),mapInfo) ) {
	 fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create attribute grid_mapping.\n");
	 ret++;
      }
      ret += rsprod_attributes_addCopyAttr(list,attr);rsprod_attr_delete(attr);
   }

   /** coordinates [opt, from dimension object] **/
   if (addCoords) {
      char *listcoords;
      int FirstDim = 0;
      if (addCoords == 2) { /* addCoords = 2 implies that we do not write the first dimension (which is supposedly 'time')*/
	 FirstDim = 1;
      }
      rsprod_dims_getListNames(cdims,FirstDim,&listcoords,' ');
      if (rsprod_attr_createWithCopyValues(&attr,"coordinates",RSPROD_CHAR,strlen(listcoords),listcoords)) {
	 fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create attribute coordinates.\n");
	 ret++;
      }
      ret += rsprod_attributes_addCopyAttr(list,attr);rsprod_attr_delete(attr);
   }
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create the list of attributes.\n");
      return 1;
   }

   /* data */ 
   rsprod_data *data;
   if (rsprod_data_createWithAssignValues(&data,type,nbvalues,values)) {
      fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create the Data object.\n");
      return 1;
   }

   /* field object */
   if (rsprod_field_create(this,name,cdims,list,data)) {
      fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create the Field object.\n");
      return 1;
   }

   return 0;
}

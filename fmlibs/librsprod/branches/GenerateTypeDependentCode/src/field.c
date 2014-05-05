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
#include <dbl_list.h>
#include "calibration.h"
#include "rsprod_intern.h"
#include "runtime_config.h"
#include "rsprod_report_utils.h"
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
int rsprod_field_create(rsprod_field **this,char *name,rsprod_dimensions *dims,rsprod_attributes *attr,rsprod_data *data) {

   rsprod_field *tmp = rsprodMalloc(sizeof(rsprod_field));
   if (rsprod_field_setName(tmp,name)) {
      fprintf(stderr,"ERROR (%s) Could not set name %s to field object.\n",__func__,name);
      return 1;
   }
   tmp->attr = attr;
   tmp->dims = dims;
   tmp->data = data;

   if (!(tmp->dims) && (tmp->data)) {
      fprintf(stderr,"ERROR (%s) Field %s has no dimension but some data.\n",__func__,name);
      return 1;
   }
   if ((tmp->dims) && !(tmp->data)) {
      fprintf(stderr,"ERROR (%s) Field %s has some dimensions but no data.\n",__func__,name);
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
   if (this->data) 
      rsprod_data_delete(this->data);
   rsprod_attributes_delete(this->attr);
   if (this->dims)
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
 *    TL, met.no, 04.09.2009    :   attributes are written with 'details' (thus including values)
 *
 */ 
void rsprod_field_printInfo(rsprod_field *this) {
   char *list_dims;
   if ((this->dims) && (this->dims->totelems > 0)) {
      int ret = rsprod_dims_getListNames(this->dims,0,&list_dims,',');
      if (ret) list_dims = NULL;
   } else {
      list_dims = NULL;
   }
   fprintf(stderr,"%s %s (%s);\n",TypeName[rsprod_data_getType(this->data)],this->name,(list_dims?list_dims:"unkown"));
   rsprod_attributes_printInfo(this->attr);
   if (this->data) {
      rsprod_data_printInfo(this->data);
   }
}

void rsprod_field_printNcdump(rsprod_field *this) {
   char *list_dims;
   if (this->dims && (this->dims->totelems > 0)) {
      int ret = rsprod_dims_getListNames(this->dims,0,&list_dims,',');
      if (ret) list_dims = NULL;
   } else {
      list_dims = NULL;
   }
   fprintf(stderr,"%s %s (%s);\n",TypeName[rsprod_data_getType(this->data)],this->name,(list_dims?list_dims:"unkown"));
   rsprod_attributes_printNcdump(this->attr);
   if (this->data)
      rsprod_data_printNcdump(this->data,15);
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
 *    attributes (the 'scale_factor' and 'add_offset' are removed, the valid_min, 
 *    valid_max and valid_range are modified).
 *
 * NOTE:
 *    The type to which the dataset will be unpacked is controlled by the following rules:
 *    1) If one of 'scale_factor' or 'add_offset' are in the list of attributes _and_ they
 *       have a data-type which differs from the data itself, then the destination type will be
 *       the type of the 'scale_factor' and 'add_offset' attribute. Those two must be of the
 *       same data type. 'Type2' calibration is triggered.
 *    2) If one of 'scale_factor' or 'add_offset' are in the list of attributes _and_ they
 *       have the same data-type as the data itself, then the destination type is the one of
 *       the (packed) data. A 'Type1' calibration is triggered.
 *    NOTE for 1) and 2), if only one of the two attributes is given, the second is set to a 
 *       'unity' or 'zero' value of the same type as the attribute which is given. 
 *       Case 1) and 2) are the only case mentioned in CF-1 standard.
 *    3) If neither 'scale_factor' nor 'add_offset' are found in the list of attributes, the 
 *       output type is controlled by 'librsprod_unpack_to' variable (runtime_config.c). It
 *       is just like a cast during which the fillvalues are taken care of. 
 *       To set 'librsprod_unpack_to' as RSPROD_NAT (default) will leave the dataset untouched.
 *
 * TODO:
 *    o Decide on a strategy for when 'librsprod_unpack_to' is of the same type as the input
 *      data. Currently, we cast from, say, Float to Float (no effect) _but change the fill values_
 *      to the defaults in the library. Is this what the user really wants?
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    TL, met.no, 04.09.2009   :   For choosing the destination type, change to the behavior
 *                                    cited in NOTE section above.
 *    TL, met.no, 08.10.2010   :   Change the default behaviour when librsprod_unpack_to is NAT,
 *                                 Ameliorate the initial test for 0-length datasets.
 *                                 Correct a bug in converting the _FillValue to new type.
 *
 */ 
int rsprod_field_unpack(rsprod_field *this) {

   if ((!this->dims) || (this->dims->totelems == 0)) {
      rsprod_echo(stderr,"VERBOSE (%s) Do not unpack field %s since it does not have any data.\n",__func__,this->name);
      return 0;
   }

   rsprod_echo(stderr,"Entering %s.",__func__); 

   /* The FromType is the data type of the original data */
   rsprod_type fromType = rsprod_data_getType(this->data);

   if (librsprod_echo_mode == LIBRSPROD_VERBOSE) {
      fprintf(stderr,"Field at entry:\n");
      rsprod_field_printInfo(this);
   }

   /* data object */
   rsprod_data *data = this->data;

   /* we need 4 parameters: the 2 fill values, the scale and the offset */
   rsprod_attributes *list = this->attr;
  
   /* Get the SCALE FACTOR (scale_factor) and the OFFSET (add_offset) of the calibration */
   rsprod_attr *attrScaleFactor;
   void *scale = NULL;
   if (!rsprod_attributes_getAttr(list,&attrScaleFactor,"scale_factor")) {
      scale = rsprod_attr_getValues(attrScaleFactor);
   } else {
      attrScaleFactor = NULL; /* make sure it is to NULL */
   }
   
   rsprod_attr *attrAddOffset;
   void *offset = NULL;
   if (!rsprod_attributes_getAttr(list,&attrAddOffset,"add_offset")) {
      offset = rsprod_attr_getValues(attrAddOffset);
   } else {
      attrAddOffset = NULL; /* make sure it is to NULL */
   }

   /* if both attributes are found, test that they have the same type */
   if (attrScaleFactor && attrAddOffset) {
      rsprod_echo(stderr,"VERBOSE (%s) Both 'scale_factor' and 'add_offset' were found for %s.\n",__func__,this->name);
      if (rsprod_attr_getType(attrScaleFactor) != rsprod_attr_getType(attrAddOffset)) {
         fprintf(stderr,"ERROR (%s) For %s the 'scale_factor' (%s) and 'add_offset' (%s) do not have the same type.\n",
               __func__,this->name,TypeName[rsprod_attr_getType(attrScaleFactor)],TypeName[rsprod_attr_getType(attrAddOffset)]);
         return 1;
      }
   }

   /* 
    * Decide upon which calibration formula we should apply (as well as the 
    * destination type). */
   rsprod_type toType;
   short calibration_method = RSPROD_CALIBRATION_UNKNOWN;
   if (!scale && !offset) {
      rsprod_echo(stderr,"VERBOSE (%s) No 'scale_factor' and no 'add_offset' for %s.\n",__func__,this->name);
      toType             = librsprod_unpack_to;
      calibration_method = RSPROD_CALIBRATION_ONLYCAST;
      /* IF neither the scale_factor nor the add_offset are to be found, 
       *    AND the 'librsprod_unpack_to' variable (via runtime config) is RSPROD_NAT, 
       * THEN leave the dataset unchanged. */
      if (toType == RSPROD_NAT) {
         rsprod_echo(stderr,"VERBOSE (%s) 'librsprod_unpack_to' (runtime config) is set to RSPROD_NAT. Leave dataset %s as it is.\n",
               __func__,this->name);
         return 0;
      }
   } else if (scale) {
      rsprod_echo(stderr,"VERBOSE (%s) 'scale_factor' for %s is given.\n",__func__,this->name);
      if (rsprod_attr_getType(attrScaleFactor) == rsprod_data_getType(data)) {
         toType             = rsprod_data_getType(data);
         calibration_method = RSPROD_CALIBRATION_TYPE1;
      } else {
         toType             = rsprod_attr_getType(attrScaleFactor);
         calibration_method = RSPROD_CALIBRATION_TYPE2;
      }
      rsprod_echo(stderr,"VERBOSE (%s) toType is %s\n",__func__,TypeName[toType]);
      if (!offset) {
         rsprod_echo(stderr,"VERBOSE (%s) 'scale_factor' for %s is given, but not 'add_offset'. Use a zero (%s).\n",
               __func__,this->name,TypeName[toType]);
         if ( !ZeroValues[toType] ) {
            rsprod_echo(stderr,"VERBOSE (%s) The zero value (%s) is not given, let's initialize it.\n",__func__,
               TypeName[toType]);
            librsprod_init_zerovalues();
         }
         offset = ZeroValues[toType];
      }
   } else if (offset) {
      rsprod_echo(stderr,"VERBOSE (%s) 'add_offset' for %s is given.\n",__func__,this->name);
      if (rsprod_attr_getType(attrAddOffset) == rsprod_data_getType(data)) {
         toType             = rsprod_data_getType(data);
         calibration_method = RSPROD_CALIBRATION_TYPE1;
      } else {
         toType             = rsprod_attr_getType(attrAddOffset);
         calibration_method = RSPROD_CALIBRATION_TYPE2;
      }
      rsprod_echo(stderr,"VERBOSE (%s) toType is %s\n",__func__,TypeName[toType]);
      if (!scale) {
         rsprod_echo(stderr,"VERBOSE (%s) 'add_offset' for %s is given, but not 'scale_factor'. Use unity (%s).\n",
               __func__,this->name,TypeName[toType]);
         if ( !UnityValues[toType] ) {
            rsprod_echo(stderr,"VERBOSE (%s) The unity value (%s) is not given, let's initialize it.\n",__func__,
               TypeName[toType]);
            librsprod_init_unityvalues();
         }
         scale = UnityValues[toType];
      }
   }
   /* Now there is only two possibilities : either we have both offset and scale or none of them. */
   /* Make sure of the above... */
   if ((scale && !offset) || (!scale && offset)) {
      fprintf(stderr,"ERROR (%s) For %s, we have only one of 'scale_factor' or 'add_offset'. It should never happen.\n",
         __func__,this->name);
         return 1;
   }
   if (calibration_method == RSPROD_CALIBRATION_UNKNOWN) {
      fprintf(stderr,"ERROR (%s) The calibration method could not be decided upon.\n",__func__);
      return 1;
   }


   /* Now deal with the fill values (always optional). fillval1 is the 'from' fillvalue and fillval2 is the destination one. */
   rsprod_attr *attrFillVal1;
   void *fillval1 = NULL;
   void *fillval2 = NULL;
   if (rsprod_attributes_getAttr(list,&attrFillVal1,"_FillValue")) {
      rsprod_echo(stderr,"WARNING (%s) cannot find the _FillValue for field %s.\n",__func__,this->name);
   } else if (rsprod_attr_getType(attrFillVal1) != rsprod_data_getType(data)) {
      fprintf(stderr,"ERROR (%s) the _FillValue has not the same type as the data object.\n",__func__);
      return 1;
   } else {
      fillval1 = rsprod_attr_getValues(attrFillVal1);
   }
   if (fillval1 && !(FillValues[toType])) {
      rsprod_echo(stderr,"VERBOSE (%s) The 'destination' fill value (%s) is not given, let's initialize it.\n",__func__,
            TypeName[toType]);
      librsprod_init_fillvalues();
   }
   fillval2 = FillValues[toType];
   if (fillval1 && !fillval2) {
      fprintf(stderr,"ERROR (%s) The 'destination' fill value (%s) is NULL.\n",__func__,TypeName[toType]);
      return 1;
   }

   /* Now we are ready to call the appropriate calibration function */
   rsprod_echo(stderr,"VERBOSE (%s) Ready to transform %s from %s into %s.\n",__func__,
         this->name,TypeName[rsprod_data_getType(data)],TypeName[toType]);
   if (rsprod_data_castToType(data,toType,calibration_method,fillval1,fillval2,scale,offset)) {
      fprintf(stderr,"ERROR (%s) while transforming %s from %f into %s.\n",__func__,
            this->name,TypeName[rsprod_data_getType(data)],TypeName[toType]);
      return 1;
   }
   rsprod_echo(stderr,"VERBOSE (%s) OK.\n",__func__);

   /* adapt the values of the valid_min, valid_max and valid_range attributes */
   rsprod_attr *ValidityAttr;
   char *validity_range_attributes[] = {"valid_min","valid_max","valid_range"};
   for (size_t i_attr = 0 ; i_attr < 3 ; i_attr++) {
      if (rsprod_attributes_getAttr(list,&ValidityAttr,validity_range_attributes[i_attr])) {
         rsprod_echo(stderr,"VERBOSE (%s) did not find the '%s' for field %s.\n",__func__,validity_range_attributes[i_attr],this->name);
      } else {
         /* check type of attribute */
         if (rsprod_attr_getType(ValidityAttr) != fromType) {
            fprintf(stderr,"ERROR (%s) the '%s' attribute does not have the same type as the original data object (%s).\n",
                  __func__,rsprod_attr_getName(ValidityAttr),TypeName[fromType]);
            return 1;
         }
         rsprod_echo(stderr,"VERBOSE (%s) Ready to transform %s from %s into %s.\n",__func__,
               rsprod_attr_getName(ValidityAttr),TypeName[fromType],TypeName[toType]);
         /* apply the unpacking */
         if (rsprod_data_castToType(ValidityAttr->content.notype,toType,calibration_method,fillval1,fillval2,scale,offset)) {
            fprintf(stderr,"ERROR (%s) while transforming %s from %f into %s.\n",__func__,
                  rsprod_attr_getName(ValidityAttr),TypeName[fromType],TypeName[toType]);
            return 1;
         }
         /* redirect the methods to those of the new type */
         if (rsprod_attr_assignMethods(ValidityAttr,toType)) {
            fprintf(stderr,"ERROR (%s) Failed to redefine the methods of the '%s' attribute after packing.\n",__func__,
                  rsprod_attr_getName(ValidityAttr));
            return 1;
         }
         rsprod_echo(stderr,"VERBOSE (%s) OK.\n",__func__);
      }
   }

   /* delete the now un-necessary scale and offset attributes and update the fill value */
   if (attrScaleFactor) {
      rsprod_echo(stderr,"VERBOSE (%s) Ready to remove 'scale_factor' from the list of attributes.\n",__func__);
      if (rsprod_attributes_deleteAttr(list,"scale_factor")) {
          fprintf(stderr,"ERROR (%s) cannot remove the old 'scale_factor' from the list of attributes.\n",__func__);
          return 1;
      }
   }
   if (attrAddOffset) {
      rsprod_echo(stderr,"VERBOSE (%s) Ready to remove 'add_offset' from the list of attributes.\n",__func__);
      if (rsprod_attributes_deleteAttr(list,"add_offset")) {
          fprintf(stderr,"ERROR (%s) cannot remove the old 'add_offset' from the list of attributes.\n",__func__);
          return 1;
      }
   }
   if (fillval1) {
      /* have a fill value only if there was one in the original dataset */
      rsprod_echo(stderr,"VERBOSE (%s) Ready to transform %s from %s into %s.\n",__func__,
               "_FillValue",TypeName[fromType],TypeName[toType]);
      rsprod_attr *newFillValue;
      if (rsprod_attr_createWithCopyValues(&newFillValue,"_FillValue",toType,1,fillval2)) {
          fprintf(stderr,"ERROR (%s) cannot create the new fill value parameter.\n",__func__);
          return 1;
      }
      if (rsprod_attributes_replaceAttr(list,newFillValue)) {
          fprintf(stderr,"ERROR (%s) cannot insert the new fill value attribute in the list.\n",__func__);
          return 1;
      }
      rsprod_echo(stderr,"VERBOSE (%s) OK.\n",__func__);
   }

   if (librsprod_echo_mode == LIBRSPROD_VERBOSE) {
      fprintf(stderr,"Field at exit:\n");
      rsprod_field_printInfo(this);
   }
   rsprod_echo(stderr,"Exiting %s\n",__func__);
   return 0;
}

/* 
 * NAME : rsprod_field_pack
 *
 * PURPOSE:
 *    'Pack' (compress) the given field to a new type field, using the calibration
 *    information given as parameters. Attributes 'scale_factor' and 'add_offset' are added
 *    to those of the field. This routine modifies the type of the data, the data value itself 
 *    and the list of attributes (the 'scale_factor' and 'add_offset' are added). The attributes
 *    valid_min and valid_max are modified.
 *    The destination type should be provided on the parameter line. The type of 'scale_factor'
 *    and 'add_offset' is the same as the current data type. The packed values will be computed
 *    as <new> = (<current> + <offset>) / <scale> 
 *
 * NOTE:
 *    o   scaleP and offsetP can given as NULL. In that case, a unity or zero value will be used.
 *    o   if both scale and offset are NULL, then a ONLYCAST calibration will be used.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 23.04.2008
 *
 * MODIFIED:
 *    TL, met.no, 04.09.2009     :   Possibility to choose the output type.
 *    TL, met.no, 25.01.2010     :   (hopefully) fixed the packing routine.
 *
 */ 
int rsprod_field_pack(rsprod_field *this, rsprod_type toType, void *scale, void *offset) {

   if (!this->data) {
      rsprod_echo(stderr,"WARNING (%s) Cannot pack field %s since it does not have any data.\n",__func__,this->data);
      return 0;
   }

   /* check that toType is ok */
   TestValidType(toType);

   /* data object */
   rsprod_data *data = this->data;
   rsprod_type fromType = rsprod_data_getType(data);

   /* attributes object */
   rsprod_attributes *list = this->attr;

   rsprod_echo(stderr,"Entering %s. Packing %s from %s to %s.\n",__func__,this->name,TypeName[fromType],TypeName[toType]);
   if (librsprod_echo_mode == LIBRSPROD_VERBOSE) {
      fprintf(stderr,"Field at entry:\n");
      rsprod_field_printInfo(this);
   }

   /* Cannot pack a dataset which already has 'scale_factor' or 'add_offset' */
   rsprod_attr *tmp1,*tmp2; 
   if (!rsprod_attributes_getAttr(list,&tmp1,"scale_factor") || 
         !rsprod_attributes_getAttr(list,&tmp2,"add_offset")) {
      fprintf(stderr,"ERROR (%s) the dataset already has 'scale_factor' and/or 'add_offset': cannot pack it further.\n",__func__);
      return 1;
   }

   /* Decide upon which type of calibration we should use */
   short calibration_method = RSPROD_CALIBRATION_UNKNOWN;
   if (!scale && !offset) {
      calibration_method = RSPROD_CALIBRATION_ONLYCAST;
   } else {
      calibration_method = RSPROD_CALIBRATION_TYPE1;
      if (!scale) {
         rsprod_echo(stderr,"VERBOSE (%s) 'scale_factor' is not provided (NULL). Use unity (%s).\n",
               __func__,TypeName[fromType]);
         if ( !UnityValues[fromType] ) {
            rsprod_echo(stderr,"VERBOSE (%s) The unity value (%s) is not given, let's initialize it.\n",__func__,
               TypeName[fromType]);
            librsprod_init_unityvalues();
         }
         scale = UnityValues[fromType];
      } else if (!offset) {
         rsprod_echo(stderr,"VERBOSE (%s) 'add_offset' is not provided (NULL). Use a zero (%s).\n",
               __func__,TypeName[fromType]);
         if ( !ZeroValues[fromType] ) {
            rsprod_echo(stderr,"VERBOSE (%s) The zero value (%s) is not given, let's initialize it.\n",__func__,
               TypeName[fromType]);
            librsprod_init_zerovalues();
         }
         offset = ZeroValues[fromType];
      }
   }

   /* Deal with the fill values (always optional). fillval1 is the 'from' fillvalue and fillval2 is the destination one. */
   rsprod_attr *attrFillVal1;
   void *fillval1 = NULL;
   void *fillval2 = NULL;
   if (rsprod_attributes_getAttr(list,&attrFillVal1,"_FillValue")) {
      rsprod_echo(stderr,"WARNING (%s) cannot find the _FillValue for field %s.\n",__func__,this->name);
   } else if (rsprod_attr_getType(attrFillVal1) != rsprod_data_getType(data)) {
      fprintf(stderr,"ERROR (%s) the _FillValue has not the same type as the data object.\n",__func__);
      return 1;
   } else {
      fillval1 = rsprod_attr_getValues(attrFillVal1);
   }
   if (fillval1 && !(FillValues[toType])) {
      rsprod_echo(stderr,"VERBOSE (%s) The 'destination' fill value (%s) is not given, let's initialize it.\n",__func__,
            TypeName[toType]);
      librsprod_init_fillvalues();
   }
   fillval2 = FillValues[toType];
   if (fillval1 && !fillval2) {
      fprintf(stderr,"ERROR (%s) The 'destination' fill value (%s) is NULL.\n",__func__,TypeName[toType]);
      return 1;
   }

   /* now we can apply the packing/compressing */
   rsprod_echo(stderr,"VERBOSE (%s) Ready to transform %s from %s into %s.\n",__func__,
         this->name,TypeName[fromType],TypeName[toType]);
   if (rsprod_data_castToType(data,toType,calibration_method,fillval1,fillval2,scale,offset)) {
      fprintf(stderr,"ERROR (%s) while transforming %s from %f into %s.\n",__func__,
            this->name,TypeName[fromType],TypeName[toType]);
      return 1;
   }

   /* replace/create the scale and offset attributes and update the fill value. The scale and offset
    * attributes are setup so that they can be used for later unpacking. */
   if (fillval1) {
      rsprod_echo(stderr,"VERBOSE (%s) Replace the _FillValue.\n",__func__);
      rsprod_attr *newFillValue;
      if (rsprod_attr_createWithCopyValues(&newFillValue,"_FillValue",toType,1,fillval2)) {
         fprintf(stderr,"ERROR (%s) cannot create the new fill value parameter.\n",__func__);
         return 1;
      }
      if (rsprod_attributes_replaceAttr(list,newFillValue)) {
         fprintf(stderr,"ERROR (%s) cannot insert the new fill value attribute in the list.\n",__func__);
          return 1;
      }
   }
   if (scale) {
      rsprod_attr *newScaleFactor;
      rsprod_echo(stderr,"VERBOSE (%s) Put a 'scale_factor'.\n",__func__);
      if (rsprod_attr_createWithCopyValues(&newScaleFactor,"scale_factor",fromType,1,scale)) {
         fprintf(stderr,"ERROR (%s) cannot create the new scale_factor parameter.\n",__func__);
         return 1;
      }
      if (rsprod_attributes_replaceAttr(list,newScaleFactor)) {
         fprintf(stderr,"WARNING (%s) cannot insert the new 'scale_factor' in the list of attributes.\n",__func__);
         return 1;
      }
   }
   if (offset) {
      rsprod_attr *newAddOffset;
      rsprod_echo(stderr,"VERBOSE (%s) Put a 'add_offset'.\n",__func__);
      if (rsprod_attr_createWithCopyValues(&newAddOffset,"add_offset",fromType,1,offset)) {
          fprintf(stderr,"ERROR (%s) cannot create the new add_offset parameter.\n",__func__);
          return 1;
      }
      if (rsprod_attributes_replaceAttr(list,newAddOffset)) {
          fprintf(stderr,"WARNING (%s) cannot insert the new 'add_offset' in the list of attributes.\n",__func__);
          return 1;
      }
   }
   /* update the valid_min, valid_max and valid_range attributes. 
    * According CF conventions, those should have the same type than
    * and reflect the range of the packed data */
   rsprod_attr *ValidityAttr;
   char *validity_range_attributes[] = {"valid_min","valid_max","valid_range"};
   for (size_t i_attr = 0 ; i_attr < 3 ; i_attr++) {
      if (rsprod_attributes_getAttr(list,&ValidityAttr,validity_range_attributes[i_attr])) {
         rsprod_echo(stderr,"VERBOSE (%s) did not find the '%s' for field %s.\n",__func__,validity_range_attributes[i_attr],this->name);
      } else {
         /* check type of attribute */
         if (rsprod_attr_getType(ValidityAttr) != fromType) {
            fprintf(stderr,"ERROR (%s) the '%s' attribute does not have the same type as the original data object (%s).\n",
                  __func__,rsprod_attr_getName(ValidityAttr),TypeName[fromType]);
            return 1;
         }
         rsprod_echo(stderr,"VERBOSE (%s) Ready to transform %s from %s into %s.\n",__func__,
               rsprod_attr_getName(ValidityAttr),TypeName[fromType],TypeName[toType]);
         /* apply the packing */
         if (rsprod_data_castToType(ValidityAttr->content.notype,toType,calibration_method,fillval1,fillval2,scale,offset)) {
            fprintf(stderr,"ERROR (%s) while transforming %s from %f into %s.\n",__func__,
                  rsprod_attr_getName(ValidityAttr),TypeName[fromType],TypeName[toType]);
            return 1;
         }
         /* redirect the methods to those of the new type */
         if (rsprod_attr_assignMethods(ValidityAttr,toType)) {
            fprintf(stderr,"ERROR (%s) Failed to redefine the methods of the '%s' attribute after packing.\n",__func__,
                  rsprod_attr_getName(ValidityAttr));
            return 1;
         }
      }
   }

   if (librsprod_echo_mode == LIBRSPROD_VERBOSE) {
      fprintf(stderr,"Field at exit:\n");
      rsprod_field_printInfo(this);
   }
   rsprod_echo(stderr,"Exiting %s\n",__func__);

   return 0;
}

/* 
 * NAME : rsprod_field_createCopy
 *
 * PURPOSE:
 *    get a new field object, as copy of another.
 *
 * NOTE:
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    TL, met.no, 04.09.2009   :   rename to rsprod_field_createCopy (was rsprod_field_copy)
 *
 */ 
rsprod_field *rsprod_field_createCopy(rsprod_field *orig) {

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
   rsprod_attributes *attr;
   ret += rsprod_attributes_copy(orig->attr,&attr);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_copy) cannot generate copies of the Attributes object.\n");
      return NULL;
   }
   rsprod_data *data;
   if (orig->data) {
      data = rsprod_data_copy(orig->data);
      if (data == NULL) {
         fprintf(stderr,"ERROR (rsprod_field_copy) cannot generate copies of the Data object.\n");
         return NULL;
      }
   } else {
      data = NULL;
   }

   rsprod_field *dest;
   ret = rsprod_field_create (&dest,orig->name,dims,attr,data);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_copy) cannot create a new Field object.\n");
      return NULL;
   }

   return dest;
}
int rsprod_field_copy(rsprod_field *to, rsprod_field *from) {
   rsprod_field *tmp = rsprod_field_createCopy(from);
   to->name = tmp->name;
   to->dims = tmp->dims;
   to->data = tmp->data;
   to->attr = tmp->attr;
   free(tmp); /* only frees the data structure, not its content */
   return 0;
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
 *    max       [opt]  -> Attribute 'valid_max'. Not put if NULL.
 *    range     [opt]  -> Attribute 'valid_range', values taken from 'min' and 'max'. 
 *                           0 -> not use, 
 *                           1 -> use only 'range'
 *                           2 -> use valid_min, valid_max and valid_range
 *    coords    [opt]  -> Attribute 'coordinates'. Not put if NULL.
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
 *    o The data is not copied, but _assigned_ to the rsprod_field object. This means
 *      it is not adviced to release or modify the pointer from the calling software 
 *      after call to this routine.
 *
 * AUTHOR :
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED :
 *    TL, met.no, 02.04.2008      : Added 'mapInfo' and 'addCoords' to conform to CF-1.x (projection information).
 *                                  Most of the attributes are now 'optional' (nothing happens if given NULL).
 *    TL, met.no, 17.02.2009      : modify the 'addCoords' flag to a 'coords' (optional) character string.
 *    TL, met.no, 25.01.2010      : Add the valid_range attribute.
 *
 *
 */
int rsprod_field_createStandard(rsprod_field **this, char *name, rsprod_type type, size_t nbvalues,
      rsprod_dimensions *dims, char *longName, char *stdName, char *units, void *fillv, void *min, void *max, /*int range,*/
      char *coords, char *mapInfo, void *values) {

   int range = 0;

   int ret;

   TestValidType(type);

   /* dimensions */
   rsprod_dimensions *cdims = rsprod_dims_copy(dims);
   if (cdims == NULL) {
      fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot duplicate Dimension object.\n");
      return 1;
   }

   /* it is not allowed to give a non-zero value for 'range' if one of 'min' or 'max' are missing */
   if ( (!min || !max) && range) {
	  fprintf(stderr,"WARNING (%s) cannot ask for 'valid_range' attribute if either 'valid_min' or 'valid_max' is missing."
          "Will remove 'valid_range'.\n",__func__);
      range = 0;
   }

   /* attributes */
   rsprod_attributes *list;
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
   if (min && (range != 1)) {
      if ( rsprod_attr_createWithCopyValues(&attr,"valid_min",type,1,min) ) {
         fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create attribute valid_min.\n");
         ret++;
      }
      ret += rsprod_attributes_addCopyAttr(list,attr);rsprod_attr_delete(attr);
   }
   /** valid_max [opt] **/
   if (max && (range != 1)) {
      if ( rsprod_attr_createWithCopyValues(&attr,"valid_max",type,1,max) ) {
	 fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create attribute valid_max.\n");
	 ret++;
      }
      ret += rsprod_attributes_addCopyAttr(list,attr);rsprod_attr_delete(attr);
   }
   /** valid_range [opt] **/
   if (range) {
      /* create a attribute object for valid_range */
      if (rsprod_attr_create(&attr,"valid_range",type,2)) {
         fprintf(stderr,"ERROR (%s) cannot create an object for valid_range attribute.\n",__func__);
         ret++;
      } else {
         rsprod_notype *rangeObject = attr->content.notype;
         /* copy the min */
         void *firstCell  = rangeObject->content.values;
         memcpy(firstCell,min,SizeOf[type]*1);
         /* copy the max */
         void *secondCell = firstCell + SizeOf[type];
         memcpy(secondCell,max,SizeOf[type]*1);
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

   /** coordinates [opt] **/
   if (coords) {
      if (rsprod_attr_createWithCopyValues(&attr,"coordinates",RSPROD_CHAR,strlen(coords),coords)) {
	 fprintf(stderr,"ERROR (rsprod_field_createStandard) cannot create attribute coordinates.\n");
	 ret++;
      }
      ret += rsprod_attributes_addCopyAttr(list,attr);rsprod_attr_delete(attr);
   }
   /* final error message for list of all attributes */
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

char rsprod_field_name_matches_string_param[100];
int rsprod_field_name_matches_string(rsprod_field *this /*, char *rsprod_field_name_matches_string_param */) {
   return (!strcmp(this->name,rsprod_field_name_matches_string_param));
}


int rsprod_field_has_nonNULLdim(rsprod_field *this) {
   return (this->dims == NULL);
}

rsprod_dimensions *rsprod_field_join_dims_glob_dest;
int rsprod_field_join_dims_glob(rsprod_field *this) {
   rsprod_field_join_dims_glob_dest = rsprod_dims_join(rsprod_field_join_dims_glob_dest,this->dims);
   return (rsprod_field_join_dims_glob_dest == NULL);
}

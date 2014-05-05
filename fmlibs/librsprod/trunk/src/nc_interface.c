/* 
 * NAME : nc_interface.c
 *
 * PURPOSE:
 *    Implement all the interface between the rsprod and the nectcdf library.
 *    All the reading and writing to/from this format should be contained here.
 *
 * AUTHOR:    
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    Thomas Lavergne, met.no, 29.04.2008     :     add the nc_get_mode() routine.
 *    Thomas Lavergne, met.no, 03.03.2011     :     move some routines under the rsprod_ namespace
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <netcdf.h>
#include "rsprod_intern.h"
#include "runtime_config.h"
#include "rsprod_report_utils.h"
#include "rsprod_memory_utils.h"
#include "rsprod_string_utils.h"
#include "query_string.h"
#include "nc_interface.h"

/* 
 * NAME: get_netCDF_version
 *
 * PURPOSE:
 *    Obtain a string containing the version number of the netCDF library
 *       
 */
char *get_netCDF_version(void) {
   char ncversion[64];
   sprintf(ncversion,"%s",nc_inq_libvers());
   /* extract the version number. It comes first in the nc_inq_libvers() string but 
    * we should take care of double-quotes
    * for library versions higher than 3.6.0 */
   char *firstSpace = strchr(ncversion,' ');
   if (firstSpace)
      *firstSpace='\0';
   char *firstDQ = strchr(ncversion,'"');
   if (firstDQ) {
      firstDQ+=1;
      char *secDQ = strchr(firstDQ,'"');
      if (secDQ) *secDQ='\0';
   } else {
      firstDQ = ncversion;
   }
   char *tmp = rsprodMalloc(strlen(firstDQ)+1);
   sprintf(tmp,"%s",firstDQ);

   return tmp;
}

/* 
 * NAME: convertType_rsprod2nc,
 *       convertType_nc2rsprod
 *
 * PURPOSE:
 *    Convert a rsprod_type to/from nc_type.
 *       
 */
nc_type convertType_rsprod2nc(rsprod_type type) {

   nc_type ret;
   switch(type) {
      case RSPROD_DOUBLE: 
         ret = NC_DOUBLE;
         break;
      case RSPROD_FLOAT: 
         ret = NC_FLOAT;
         break;
      case RSPROD_SHORT: 
         ret = NC_SHORT;
         break;
      case RSPROD_INT: 
         ret = NC_INT;
         break;
      case RSPROD_CHAR: 
         ret = NC_CHAR;
         break;
      case RSPROD_BYTE:
         ret = NC_BYTE;
         break;
      default:
         ret = NC_NAT;
     fprintf(stderr,"ERROR (%s) Unknown rsprod type (%u).\n",__func__,type);
     break;
   }
   return (ret);

}
rsprod_type convertType_nc2rsprod(nc_type type) {

   rsprod_type ret;
   switch(type) {
      case NC_DOUBLE: 
         ret = RSPROD_DOUBLE;
         break;
      case NC_FLOAT: 
         ret = RSPROD_FLOAT;
         break;
      case NC_SHORT: 
         ret = RSPROD_SHORT;
         break;
      case NC_INT: 
         ret = RSPROD_INT;
         break;
      case NC_CHAR: 
         ret = RSPROD_CHAR;
         break;
      case NC_BYTE: 
         ret = RSPROD_BYTE;
         break;
      default:
         ret = RSPROD_NAT;
     fprintf(stderr,"ERROR (%s) Unknown netcdf type (%d).\n",__func__,type);
     break;
   }
   return (ret);
   
}

/* 
 * In netcdf, string datasets must use a 'dummy' additiontal dimension to store
 * the length of the charatcer strings. This 'dummy' dimention should have
 * a valid name which we test for in this routine. 
 */
int isValidStringLengthDimensionName(char *dname) {
   int isValid = 0;
   /*
   if (!strncmp(dname,NC_NUMCHARDIM_BASE,strlen(NC_NUMCHARDIM_BASE))) {
      isValid = 1;
   }
   */
   isValid = 1;
   return (isValid);
}

/* For an open netCDF with id ncid and mode either NC_DAT_MODE or NC_DEF_MODE. 
 *    make sure ncid is in mode $mode when leaving this routine. 
 */
int rsprod_nc_set_mode(const int ncid,const int mode) {
   int status = nc_redef(ncid);   /* try to change to DEFINE mode */
   int entrymode;
   if ( status == NC_NOERR ) {
      entrymode = NC_DAT_MODE;    /* ncid is now in DEFINE mode but was in DATA mode when entering nc_set_mode */
      if (entrymode == mode) {
     (void) nc_enddef(ncid);  /* we want DATA mode at exit so step back */
     return NC_NOERR;
      } else {
         return NC_NOERR;         /* we want DEFINE mode at exit so keep the change */
      }
   } else if (status == NC_EINDEFINE) {
      entrymode = NC_DEF_MODE;    /* ncid was (and is still) in DEFINE mode */
      if (entrymode == mode) {
     return NC_NOERR;         /* we want DEFINE mode at exit so leave */
      } else {
     (void) nc_enddef(ncid);  /* we want DATA mode at exit so switch to it */
         return NC_NOERR;
      }
   } else if (status == NC_EPERM) { /* ncid is read only */
      /* the entrymode has to be NC_DAT_MODE, since there is no way to change to DEFINE mode. */
      entrymode = NC_DAT_MODE;
      if (entrymode == mode) {
     return NC_NOERR;         /* we want DATA mode at exit so leave */
      } else {
     return NC_EPERM;      /* we want DEFINE mode which is illegal */
      }
   }
   /* else */             
   return status;
}
/* 
 * For an open netCDF with id ncid find out its mode (either NC_DAT_MODE or NC_DEF_MODE). 
 */
int rsprod_nc_get_mode(const int ncid,int *mode) {
   int status = nc_redef(ncid);   /* try to change to DEFINE mode */

   if ( status == NC_NOERR ) {
      *mode = NC_DAT_MODE;        /* ncid is now in DEFINE mode but was in DATA mode when entering nc_set_mode */
      (void) nc_enddef(ncid);     /* undo what we just did. */
      return NC_NOERR;
   } else if (status == NC_EINDEFINE) {
      *mode = NC_DEF_MODE;        /* ncid was (and is still) in DEFINE mode */
      return NC_NOERR;         /* we want DEFINE mode at exit so leave */
   } else if (status == NC_EPERM) { /* ncid is read only */
      /* the mode has to be NC_DAT_MODE, since there is no way to change to DEFINE mode. */
      *mode = NC_DAT_MODE;
      return NC_NOERR;
   }
   /* else */             
   return status;
}

/* Error handling for IO with netcdf files */
int rsprod_nc_handle_status(int status) {
   if (status != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) IO problem to netcdf interface <%s>.\n",__func__,nc_strerror(status));
      return 1;
   }
   return 0;
}

/* 
 * NAME : rsprod_nc_get_var_strings
 *
 * PURPOSE:
 *    Special case when reading a character strings dataset to 
 *    a netcdf file: we need to use several calls to 
 *    'nc_get_vara_text' to format the rsprod_field's data
 *    as an array of strings.
 *
 * NOTE:
 *    o Refer to: 
 *      http://www.unidata.ucar.edu/software/netcdf/docs/netcdf-c.html#Strings
 *    o This does not work with the current data model in rsprod. We are
 *      left with loading the char dataset as a long array of characters and
 *      post-process it to form an array of strings.
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no/FoU, 14.05.2008
 *
 * MODIFIED:
 *
 */
int rsprod_nc_get_var_strings(rsprod_data *data, rsprod_dimensions *dims, int ncid, int varid) {
   
   int ret;
   size_t ndims = dims->nbdims;

   /* check that we have a final 'nchars' dimension in our dimension object */
   char *lastDimName = dims->name[ndims-1];
   if (!isValidStringLengthDimensionName(lastDimName)) {
      fprintf(stderr,"ERROR (%s) Invalid name for the 'number of character' dimension in nc_char dataset: <%s>\n",
        __func__,lastDimName);
      return 1;
   }
   size_t maximum_string_length = dims->length[ndims-1];
   
   /* prepare the 'start' and 'count' arrays as we will make use of nc_get_vara */
   size_t *start = rsprodMalloc(ndims*sizeof(size_t));
   size_t *count = rsprodMalloc(ndims*sizeof(size_t));
      
   for (size_t d = 0 ; d < ndims-1 ; d++) {
      count[d] = 1;
   }
   count[ndims-1] = maximum_string_length;

   long *mindex = rsprodMalloc(ndims*sizeof(long));
   for ( size_t e = 0 ; e < dims->totelems ; e+=maximum_string_length ) {

      if( matIndex(dims,e,mindex) ) continue;

      for (size_t d = 0 ; d < ndims ; d++) {
         start[d] = mindex[d];
      }
    
      size_t index_of_string = e/maximum_string_length;
      char *stringValue = (char *)rsprod_data_getValues(data) + e;

      /*
      printf("VERBOSE (%s) e:%u. index_of_string:%u. start is [",__func__,e,index_of_string);
      for (size_t d = 0 ; d < ndims ; d++) {printf("%u ",start[d]);};
      printf("\b] and count is [");
      for (size_t d = 0 ; d < ndims ; d++) {printf("%u ",count[d]);};
      printf("\b]\n");
      */

      /*
      char stringValue[100];
      memset(stringValue,RSPROD_FILLVAL_CHAR,100);
      stringValue[100-1]='\0';
      */

      ret = rsprod_nc_handle_status(nc_get_vara_text(ncid,varid,start,count,stringValue));
      if (ret) {
     fprintf(stderr,"ERROR (%s) Could not write string at position [",__func__);
         for (size_t d = 0 ; d < ndims ; d++) {printf("%u ",start[d]);};
     fprintf(stderr,"\b] in the netcdf dataset.\n");
     return 1;
      }
      //printf("VERBOSE (%s): e:%u stringValue is\n\t<%s>\n",__func__,e,stringValue);
   }


   free(start);
   free(count);
   free(mindex);

   return 0;

}

/* 
 * NAME : rsprod_nc_get_var
 *
 * PURPOSE:
 *   Type independent version of the nc_get_var_type family of 
 *   netcdf functions.
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    Thomas Lavergne, met.no/FoU, 14.05.2008  :  change interface to use a rsprod_dimensions 
 *       (instead of just a rsprod_data) because we need acces to the dimensions for some cases
 *       like for string datasets.
 *    Thomas Lavergne, met.no/FoU, 14.05.2008  :  implement a special behaviour for strings 
 *       datasets.
 *
 */
int rsprod_nc_get_var(rsprod_data *data, rsprod_dimensions *dims,int ncid, int varid) {

   int ret;

   rsprod_type type = rsprod_data_getType(data);
   void *values = rsprod_data_getValues(data);

   nc_type ncType;
   if (isValidType(type)) {
      ncType = convertType_rsprod2nc(type);
   } else {
      return 1;
   }
   switch(ncType) {
      case NC_DOUBLE:
         ret = rsprod_nc_handle_status(nc_get_var_double(ncid,varid,values));
         break;
      case NC_FLOAT:
         ret = rsprod_nc_handle_status(nc_get_var_float(ncid,varid,values));
         break;
      case NC_SHORT:
         ret = rsprod_nc_handle_status(nc_get_var_short(ncid,varid,values));
         break;
      case NC_INT:
         ret = rsprod_nc_handle_status(nc_get_var_int(ncid,varid,values));
         break;
      case NC_CHAR:
         ret = rsprod_nc_get_var_strings(data,dims,ncid,varid);
         break;
      case NC_BYTE:
         ret = rsprod_nc_handle_status(nc_get_var_uchar(ncid,varid,values));
         break;
      default:
         ret = 1;
         fprintf(stderr,"ERROR (%s) unknown netCDF type.\n",__func__);
         break;
   }
   
   if (ret) {
      fprintf(stderr,"ERROR (%s) problem with reading a variable from netCDF file.\n",__func__);
      return 1;
   }
   return 0;
}

/* 
 * NAME : rsprod_nc_put_var_strings
 *
 * PURPOSE:
 *    Special case when writing a character strings dataset to 
 *    a netcdf file: we need to use several calls to 
 *    'nc_put_vara_text' as the rsprod_field's data can contain 
 *    many '\0' which we do not always want to write.
 *
 * NOTE:
 *    o Works because we pre-process the string datasets via
 *      rsprod_field_preprocess_for_netcdf() (see below).
 *    o Refer to: 
 *      http://www.unidata.ucar.edu/software/netcdf/docs/netcdf-c.html#Strings
 *      
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    Thomas Lavergne, met.no, 26.10.2010   :   Adapt to the new run-time configuration 'librsprod_write_nullchar'
 *
 */
int rsprod_nc_put_var_strings(rsprod_field *this, int ncid, int varid) {

   int ret;
   size_t ndims = this->dims->nbdims;

   /* check that we have a final 'nchars' dimension in our dimension object */
   char *lastDimName = this->dims->name[ndims-1];
   if (!isValidStringLengthDimensionName(lastDimName)) {
      fprintf(stderr,"ERROR (%s) Invalid name for the 'number of character' dimension in nc_char dataset: <%s>\n",
        __func__,lastDimName);
      return 1;
   }
   size_t maximum_string_length = this->dims->length[ndims-1];
   
   /* prepare the 'start' and 'count' arrays as we will make use of nc_put_vara */
   size_t *start = rsprodMalloc(ndims*sizeof(size_t));
   size_t *count = rsprodMalloc(ndims*sizeof(size_t));
      
   for (size_t d = 0 ; d < ndims-1 ; d++) {
      count[d] = 1;
   }

   long *mindex = rsprodMalloc(ndims*sizeof(long));
   for ( size_t e = 0 ; e < this->dims->totelems ; e+=maximum_string_length ) {

      if( matIndex(this->dims,e,mindex) ) continue;

      for (size_t d = 0 ; d < ndims ; d++) {
         start[d] = mindex[d];
      }
      
      char *stringValue = ((char **)rsprod_data_getValues(this->data))[e/maximum_string_length];
      count[ndims-1] = strlen(stringValue);
      if (librsprod_write_nullchar) 
         count[ndims-1] += 1; /* We want to write one trailing '\0' */

      ret = rsprod_nc_handle_status(nc_put_vara_text(ncid,varid,start,count,stringValue));
      if (ret) {
     fprintf(stderr,"ERROR (%s) Could not write string at position [",__func__);
         for (size_t d = 0 ; d < ndims ; d++) {printf("%u ",start[d]);};
     fprintf(stderr,"\b] in the netcdf dataset.\n");
     return 1;
      }
   }


   free(start);
   free(count);
   free(mindex);

   return 0;
}

/* 
 * NAME : rsprod_nc_put_var
 *
 * PURPOSE:
 *   Type independent version of the nc_put_var_type family of 
 *   netcdf functions.
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    TL, met.no, 01.04.2011    :   Re-implement all the nc write statements
 *                                    to use nc_put_vara_type. This should help
 *                                    for handling UNLIMITED dimensions.
 *
 */
int rsprod_nc_put_var(rsprod_field *this,int ncid, int varid) {

   int ret,mode;

   /* check MODE of netcdf interface (DEFINE vs DATA) */
   if (rsprod_nc_handle_status(rsprod_nc_get_mode(ncid,&mode))) {
      fprintf(stderr,"WARNING (%s) Could not access the mode for nectdf id %d\n",__func__,ncid);
   } else {
      if (mode != NC_DAT_MODE) {
         fprintf(stderr,"WARNING (%s) Call while ncid %d is in DEFINE mode.\n",__func__,ncid);
      }
   }

   /* check type of the rsprod_field */
   rsprod_type type = rsprod_data_getType(this->data);
   nc_type ncType;
   if (isValidType(type)) {
      ncType = convertType_rsprod2nc(type);
   } else {
      return 1;
   }

   /* Prepare the start[] and count[] arrays for call to nc_put_vara_
    * This instructs nc_put_vara_ to write all the elements, just as 
    * nc_put_var_ would do. But it works with UNLIMITED dimensions. */
   size_t ndims = this->dims->nbdims;
   size_t *start = rsprodMalloc(ndims*sizeof(size_t));
   size_t *count = rsprodMalloc(ndims*sizeof(size_t));
   for (size_t d = 0 ; d < ndims ; d++) {
      start[d] = 0;
      count[d] = this->dims->length[d];
   }

   /* access to the data values */
   void *values = rsprod_data_getValues(this->data);
   switch(ncType) {
      case NC_DOUBLE:
         ret = rsprod_nc_handle_status(nc_put_vara_double(ncid,varid,start,count,values));
         break;
      case NC_FLOAT:
         ret = rsprod_nc_handle_status(nc_put_vara_float(ncid,varid,start,count,values));
         break;
      case NC_SHORT:
         ret = rsprod_nc_handle_status(nc_put_vara_short(ncid,varid,start,count,values));
         break;
      case NC_INT:
         ret = rsprod_nc_handle_status(nc_put_vara_int(ncid,varid,start,count,values));
         break;
      case NC_CHAR:
         /* special case here as we want to write a variable with character strings */
         ret = rsprod_nc_put_var_strings(this,ncid,varid); 
         break;
      case NC_BYTE:
         ret = rsprod_nc_handle_status(nc_put_vara_uchar(ncid,varid,start,count,values));
         break;
      default:
         ret = 1;
         fprintf(stderr,"ERROR (%s) unknown netCDF type.\n",__func__);
         break;
   }
   free(start); free(count);
   
   if (ret) {
      fprintf(stderr,"ERROR (%s) problem with reading a variable from netCDF file.\n",__func__);
      return 1;
   }
   return 0;
}

/* 
 * NAME : rsprod_nc_get_attr
 *
 * PURPOSE:
 *   Type independent version of the nc_get_attr_type family of 
 *   netcdf functions.
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */
int rsprod_nc_get_attr(rsprod_attr *attr,int ncid, int varid) {

   int ret;
   rsprod_type type = rsprod_attr_getType(attr);
   char *name       = rsprod_attr_getName(attr);
   void *values     = rsprod_attr_getValues(attr);
   
   nc_type ncType;
   if (isValidType(type)) {
      ncType = convertType_rsprod2nc(type);
   } else {
      return 1;
   }
   switch(ncType) {
      case NC_DOUBLE:
         ret = rsprod_nc_handle_status(nc_get_att_double(ncid,varid,name,values));
         break;
      case NC_FLOAT:
         ret = rsprod_nc_handle_status(nc_get_att_float(ncid,varid,name,values));
         break;
      case NC_SHORT:
         ret = rsprod_nc_handle_status(nc_get_att_short(ncid,varid,name,values));
         break;
      case NC_INT:
         ret = rsprod_nc_handle_status(nc_get_att_int(ncid,varid,name,values));
         break;
      case NC_CHAR:
         ret = rsprod_nc_handle_status(nc_get_att_text(ncid,varid,name,values));
         break;
      case NC_BYTE:
         ret = rsprod_nc_handle_status(nc_get_att_uchar(ncid,varid,name,values));
         break;
      default:
         ret = 1;
         fprintf(stderr,"ERROR (%s) unknown netCDF type.\n",__func__);
         break;
   }
   
   if (ret) {
      fprintf(stderr,"ERROR (%s) problem with reading an attribute from netCDF file.\n",__func__);
      return 1;
   }
   return 0;
}


/* 
 * NAME : rsprod_nc_put_attr
 *
 * PURPOSE:
 *   Type independent version of the nc_put_attr_type family of 
 *   netcdf functions.
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */
int rsprod_nc_put_attr(rsprod_attr *attr,int ncid, int varid) {

   int ret,mode;

   if (rsprod_nc_handle_status(rsprod_nc_get_mode(ncid,&mode))) {
      fprintf(stderr,"WARNING (%s) Could not access the mode for netcdf id %d.\n",__func__,ncid);
   } else {
      if (mode != NC_DEF_MODE) {
     fprintf(stderr,"WARNING (%s) Call while ncid %d is in DATA mode.\n",__func__,ncid);
      }
   }
   
   rsprod_type type = rsprod_attr_getType(attr);
   char *name       = rsprod_attr_getName(attr);
   void *values     = rsprod_attr_getValues(attr);
   size_t length    = rsprod_attr_getNbvalues(attr);
   nc_type ncType;
   if (isValidType(type)) {
      ncType = convertType_rsprod2nc(type);
   } else {
      fprintf(stderr,"ERROR (%s) Invalid type for attribute <%s>\n",__func__,name);
      return 1;
   }
   switch(ncType) {
      case NC_DOUBLE:
         ret = rsprod_nc_handle_status(nc_put_att_double(ncid,varid,name,ncType,length,values));
         break;
      case NC_FLOAT:
         ret = rsprod_nc_handle_status(nc_put_att_float(ncid,varid,name,ncType,length,values));
         break;
      case NC_SHORT:
         ret = rsprod_nc_handle_status(nc_put_att_short(ncid,varid,name,ncType,length,values));
         break;
      case NC_INT:
         ret = rsprod_nc_handle_status(nc_put_att_int(ncid,varid,name,ncType,length,values));
         break;
      case NC_CHAR:
         ret = rsprod_nc_handle_status(nc_put_att_text(ncid,varid,name,length,values));
         break;
      case NC_BYTE:
         ret = rsprod_nc_handle_status(nc_put_att_uchar(ncid,varid,name,ncType,length,values));
         break;
      default:
         ret = 1;
         fprintf(stderr,"ERROR (%s) unknown netCDF type.\n",__func__);
         break;
   }
   
   if (ret) {
      fprintf(stderr,"ERROR (%s) problem with writing an attribute in  netCDF file.\n",__func__);
      return 1;
   }
   return 0;
}

/* global variables so that the _glob routine can be called by the list routines */
int rsprod_nc_put_attr_glob_ncid;  
int rsprod_nc_put_attr_glob_fieldid;  
static int rsprod_nc_put_attr_glob(rsprod_attr *this) {
   return rsprod_nc_put_attr(this,rsprod_nc_put_attr_glob_ncid,rsprod_nc_put_attr_glob_fieldid);
}


/* 
 * NAME : rsprod_data_loadFromNetCDF
 *
 * PURPOSE : 
 *    Read a netcdf 'datafield' and store it into a rsprod_data 
 *
 * NOTE :
 *    Needs a pre-opened 'ncid' to the current file.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    Thomas Lavergne, met.no/FoU, 14.05.2008 : Change the interface to
 *       access the full dimension object (is needed for string datasets).
 *
 */
int rsprod_data_loadFromNetCDF(rsprod_data **this,rsprod_dimensions *dims,int ncid,int fieldid) {

   unsigned int elems = dims->totelems;

   int ret;
   /* Get type info (netcdf convention) */
   nc_type ncType;
   ret = rsprod_nc_handle_status(nc_inq_vartype  (ncid, fieldid,&ncType));
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not load type info for variable.\n",__func__);
      return 1;
   }

   /* tranform to our internal type convention */
   rsprod_type myType = convertType_nc2rsprod(ncType);

   /* allocate the data object */
   ret = rsprod_data_create(this, myType, elems);
   if (ret) {
      fprintf(stderr,"ERROR (%s) problem in creating the data object.\n",__func__);
      return 1;
   }

   /* read values from the netcdf file and store them in our object. We do that only if the size is not 0 */
   if (elems) {
      ret = rsprod_nc_get_var(*this,dims,ncid,fieldid);
      if (ret) {
     fprintf(stderr,"ERROR (%s) problem in using rsprod_nc_get_var().\n",__func__);
     return 1;
      } 
   }

   return 0;

}

/* 
 * NAME : rsprod_dimensions_loadFromNetCDF
 *
 * PURPOSE : 
 *    Read the dimensions for a given dataset in the netCDF file
 *    and format them into a rsprod_dimensions object.
 *
 * NOTE :
 *    Needs a pre-opened 'ncid' to the current file and a
 *    pre-opened 'fieldid' to the netcdf dataset.
 *
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    TL, 31.03.2011    :    Detect un-limited dimensions
 *
 */
int rsprod_dimensions_loadFromNetCDF(rsprod_dimensions **this,const int ncid, const int fieldid) {

   int ret;

   /* Read info on the dimensions */
   int *dimids,ndims;
   ret = rsprod_nc_handle_status(nc_inq_varndims(ncid, fieldid, &ndims));
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot find number of dimensions.\n",__func__);
      return 1;
   }
   dimids = rsprodMalloc(ndims*sizeof(int));
   ret = rsprod_nc_handle_status(nc_inq_vardimid(ncid, fieldid, dimids));
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot find IDs for %d dimensions.\n",__func__,ndims);
      return 1;
   }
   int unlimdim;
   ret = rsprod_nc_handle_status(nc_inq_unlimdim(ncid,&unlimdim));
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot find IDs for unlimited dimensions.\n",__func__);
      return 1;
   }

   /* load the dimensions info (name and length) from the netcdf file */
   char **ncNames          = rsprodMalloc(ndims*sizeof(char *));
   unsigned int *ncLengths = rsprodMalloc(ndims*sizeof(unsigned int)); 
   short *ncUnlims = rsprodMalloc(ndims*sizeof(short)); 
   for (unsigned int i=0; i < ndims; i++) {
      char string[NC_MAX_NAME+1];size_t length;
      /* read in file */
      ret = rsprod_nc_handle_status(nc_inq_dim(ncid, dimids[i], string, &length));
      if (ret) {
         fprintf(stderr,"ERROR (%s) cannot load info about dim %d.\n",__func__,dimids[i]);
         return 1;
      }
      /* process, trim and store the name */
      if (rsprod_string_trim_allNullsExceptLast(string,&(ncNames[i]))) {
     fprintf(stderr,"ERROR (%s) Problem dealing with null characters in name <%s>\n",__func__,string);
     return 1;
      }
      /* store the length */
      ncLengths[i] = (unsigned int)length;

      /* record if the dimension is 'unlimited' */
      ncUnlims[i] = 0;
      if ( dimids[i] == unlimdim)
         ncUnlims[i] = 1;
   }

   /* create the dimension object */
   ret = rsprod_dims_create(this,(const unsigned short)ndims,ncNames,ncLengths,ncUnlims);
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot create an rsprod_dimensions object.\n",__func__);
      return 1;
   }


   for (unsigned int i=0; i < ndims; i++) 
      free(ncNames[i]);
   free(dimids);
   free(ncNames);
   free(ncLengths);
   free(ncUnlims);

   return 0;
}

/* 
 * NAME : rsprod_attributes_loadFromNetCDF
 *
 * PURPOSE : 
 *    Read the attributes for a given dataset in the netCDF file
 *    and format them into a rsprod_attributes object (list of rsprod_attr obojects).
 *
 * NOTE :
 *    o Needs a pre-opened 'ncid' to the current file and a
 *      pre-opened 'fieldid' to the netcdf dataset.
 *    o Can be used with fieldid = NC_GLOBAL to read all the global
 *      parameters.
 *
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */
int rsprod_attributes_loadFromNetCDF(rsprod_attributes **this, const int ncid, const int fieldid) {

   int ret;

   /* get the number of attributes for this variable */
   int nbattr;
   ret = rsprod_nc_handle_status(nc_inq_varnatts (ncid,fieldid,&nbattr));
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot find the number of attributes for current variable.\n",__func__);
      return 1;
   }

   dbl_list *L = dbl_list_init(sizeof(rsprod_attr *));

   /* load each attribute */
   for (int i = 0; i < nbattr; i++) {

      /* get the attribute's name */
      char name[NC_MAX_NAME+1];
      memset(name,0,NC_MAX_NAME+1);
      ret = rsprod_nc_handle_status(nc_inq_attname(ncid,fieldid,i,name));
      if (ret) {
         fprintf(stderr,"ERROR (%s) no name associated to attribute %d.\n",__func__,i);
         return 1;
      }

      /* get additional information on the attribute */
      nc_type ncType;
      size_t len;
      ret = rsprod_nc_handle_status(nc_inq_att(ncid,fieldid,name,&ncType,&len));
      if (ret) {
     fprintf(stderr,"ERROR (%s) could not load additional info for attribute %d.\n",__func__,i);
     return 1;
      }

      /* tranform to our internal type convention */
      rsprod_type myType = convertType_nc2rsprod(ncType);

      /* create the new attribute object */
      rsprod_attr *attr;
      //printf("VERBOSE (rsprod_attributes_loadFromNetCDF) name for attribute is <%s>\n",name);
      ret = rsprod_attr_create(&attr,name,myType,len);
      if (ret) {
         fprintf(stderr,"ERROR (%s) could not create an attribute object with name %s.\n",__func__,name);
         return 1;
      }
      
      /* read the value from the netcdf file */
      ret = rsprod_nc_get_attr(attr,ncid,fieldid);

      /* insert it in the list of attributes */
      if (dbl_list_addNode(L,dbl_node_createAssignContent(attr),LIST_POSITION_LAST)) {
         fprintf(stderr,"ERROR (%s) could not add the new attribute %s into the list of attributes.\n",__func__,name);
         return 1;
      }
      
   }

   *this = L;

   return 0;

}

/* 
 * NAME : rsprod_field_loadFromNetCDF
 *
 * PURPOSE : 
 *    Read a whole field/dataset from the netcdf file (data+dimensions+attributes)
 *    and format it into a rsprod_field object.
 *
 * NOTE :
 *    o Needs a pre-opened 'ncid' to the current file. The field is 
 *      searched by means of its name.
 *
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */
int rsprod_field_loadFromNetCDF(rsprod_field **this,char *fieldname,const int ncid) {
   int fieldid;
   int ret;
   
   /* Turn ncid to data mode (if necessary). */
   ret = rsprod_nc_handle_status(rsprod_nc_set_mode(ncid,NC_DAT_MODE));
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not change to data mode.\n",__func__);
      return 1;
   }

   /* check if the variable is defined in this file */
   ret = rsprod_nc_handle_status(nc_inq_varid(ncid,fieldname,&fieldid));
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot find field %s in netCDF file.\n",__func__,fieldname);
      return 1;
   }

   /* load and create the dimensions */
   rsprod_dimensions *dims;
   ret = rsprod_dimensions_loadFromNetCDF(&dims,ncid,fieldid);
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot find and load dimensions for %s in netCDF file.\n",__func__,fieldname);
      return 1;
   }

   /* load and create the attributes */
   rsprod_attributes *attr;
   ret = rsprod_attributes_loadFromNetCDF(&attr,ncid,fieldid);
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot find and load attributes for %s in netCDF file.\n",__func__,fieldname);
      return 1;
   }

   /* create and load the data from file*/
   rsprod_data *data;
   ret = rsprod_data_loadFromNetCDF(&data,dims,ncid,fieldid);
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot find and load data for %s in netCDF file.\n",__func__,fieldname);
      return 1;
   }

   /* create the field object */
   ret = rsprod_field_create(this,fieldname,dims,attr,data);
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot create Field object %s from netCDF file.\n",__func__,fieldname);
      return 1;
   }

   return 0;
}
   
/* 
 * NAME : rsprod_field_preprocess_for_netcdf
 *
 * PURPOSE : 
 *    Transform the rsprod_field object to prepare the writing to
 *    a netCDF file. Could be used to check and modify the field
 *    so that it abides by some convention.
 *
 * NOTE :
 *    o Altough this routine does not use _per se_ the netcdf interface library,
 *      we keep it here because it is clearly a netcdf only routine.
 *    o Currently (April 2008) the only case is for preparing string datasets for writing.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    Thomas Lavergne, met.no/FoU, 09.05.2008     :    Adapt behaviour when 2 character string fields are
 *                                                       created in the same file: they cannot share the same
 *                                                       name for the 'number-of-character' dimension. This
 *                                                       demands knowing the currently defined dimensions thus
 *                                                       the need for 'ncid'.
 *    Thomas Lavergne, met.no/FoU, 26.10.2010     :    Use the new run time config 'librsprod_write_nullchar' and
 *                                                       'librsprod_padd_strings'.
 *
 */
int rsprod_field_preprocess_for_netcdf(int ncid, rsprod_field *this) {

   /* currently only for character string fields */
   if (rsprod_data_getType(this->data) == RSPROD_CHAR) {

      /* parse the character strings and find out which is the longest */
      size_t maxchars = 0;
      char **strarr = rsprod_data_getValues(this->data);
      for (size_t e = 0 ; e < rsprod_data_getNbvalues(this->data) ; e ++) {
         size_t len = strlen(strarr[e]);
         maxchars = (len>maxchars?len:maxchars);
      }
      /* go once again over each string and pad it (right padding) with appropriate character */
      if (librsprod_pad_strings_with != '\0') {
         for (size_t e = 0 ; e < rsprod_data_getNbvalues(this->data) ; e ++) {
            char *string = strarr[e];
            size_t len = strlen(string);
            for (size_t c = len ; c < maxchars ; c++) {
               string[c] = librsprod_pad_strings_with;
            }
            string[maxchars] = '\0'; /* make sure all the strings are null-terminated */
         }
      }
      if (librsprod_write_nullchar)
         maxchars++;
      /* Note: maxchars *does* include the terminating '\0' character.
       * This is because we want to write this '\0' into the netcdf character string _dataset_. 
       * EDIT TL Oct 2010: Writing a final '\0' character is now controlled by run-time configuration
       *    variable 'librsprod_write_nullchar' */

      /* choose a name (based on NC_NUMCHARDIM_BASE) for this 'dummy' dimension. We must check in 'ncid' to make sure
       * that we do not define the dimension twice. 
       */
      char   ndimname[NC_MAX_NAME+1];
      unsigned short dimnumber = 0;
      short found_nchar_dim = 1;
      while (found_nchar_dim) {
     int dimid;
     snprintf(ndimname,NC_MAX_NAME,"%s_%d",NC_NUMCHARDIM_BASE,dimnumber);
     int ret = nc_inq_dimid(ncid,ndimname,&dimid);
     //printf("Try %u, name <%s>\n",dimnumber,ndimname);
     if (ret != NC_NOERR) {
        if (ret != NC_EBADDIM) {
           /* undefined error occured */
           ret = rsprod_nc_handle_status(ret);
           fprintf(stderr,"ERROR (%s) problem in loading dimensions from netCDF file.\n",__func__);
           return 1;
        } else {
           /* error occurred because the dimension does not exist yet. 
        * we can thus use its name. */
           found_nchar_dim = 0;
        }
     }
     dimnumber++;
      }

      /* add the (fastest varying) dimension for the maximum number of characters in strings */
      rsprod_dims_addOne(this->dims,-1,ndimname,maxchars,1);

   }

   return 0;

}

/* 
 * NAME : rsprod_attributes_writeToNetCDF
 *
 * PURPOSE : 
 *    Write the rsprod_attributes object to the netcdf file/dataset.
 *
 * NOTE :
 *    o The netcdf file is already opened (ncid) as well as the dataset (fieldid);
 *    o Using fieldid == NC_GLOBAL allows to write global datasets.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    TL, met.no, 31.03.2011    :    Cowardly not writing anything when getting NULL
 *
 */
int rsprod_attributes_writeToNetCDF(rsprod_attributes *this,const int ncid, const int fieldid) {

   int ret;
   int mode;

   if (this == NULL) {
      fprintf(stderr,"WARNING (%s) Cowardly not writing a NULL attribute.\n",__func__);
      return 0;
   }
   
   /* Turn ncid to define mode (if necessary). */
   ret = rsprod_nc_handle_status(rsprod_nc_set_mode(ncid,NC_DEF_MODE));
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not change to define mode.\n",__func__);
      return 1;
   }

   /* go through all the attributes and write them to netCDf file */
   /* global variable for use of rsprod_nc_put_attr_glob() routine */
   rsprod_nc_put_attr_glob_ncid    = ncid;
   rsprod_nc_put_attr_glob_fieldid = fieldid;
   ret = dbl_list_browseAndCheck(this,&rsprod_nc_put_attr_glob,LIST_CHECK_KEEPMAX);
   if (ret) {
      fprintf(stderr,"ERROR (%s) Did not manage to write all the attributes to file.\n",__func__);
      return 1;
   }
   
   return 0;
}

/* 
 * NAME : rsprod_field_writeToNetCDF()
 *
 * PURPOSE : 
 *    Write the rsprod_field to the netcdf file.
 *
 * NOTE :
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *    TL, 31.03.2011   :   Handle the case where the dimention is unlimited
 *    TL, 01.04.2011   :   Cleanup the has_data if-blocks
 *
 */
int rsprod_field_writeToNetCDF(rsprod_field *this,const int ncid) {

   int ret;
   
   /* Turn ncid to define mode (if necessary). */
   ret = rsprod_nc_handle_status(rsprod_nc_set_mode(ncid,NC_DEF_MODE));
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not change to define mode.\n",__func__);
      return 1;
   }

   /* check if the dataset is actually holding some data or is it only a dummy/place-holder one, e.g.
    * for describing the the projection in CF-1 compliant files */
   if ( (this->dims && !this->data) || (!this->dims && this->data) ) {
      fprintf(stderr,"ERROR (%s) Cannot handle datasets which have data but no dimension (or vice-versa).\n",
            __func__);
      return 1;
   }
   short field_has_data = 1;
   if (!this->data) {
      rsprod_echo(stderr,"VERBOSE (%s) Dataset %s has no data to be written.\n",__func__,this->name);
      field_has_data = 0;
   }

   size_t nc_ndims;
   int *ncdimids;
   if (!field_has_data) {
      ncdimids = NULL;
      nc_ndims = 0;
   } else {
      nc_ndims = this->dims->nbdims;
      /* Pre-process the field to make it compatible with the netcdf formatting requirements. */
      /* This currently concerns only the NC_CHAR datasets which require an extra dimension 
       * (max_nb_char) in order to be stored and later accessed. */
      ret = rsprod_field_preprocess_for_netcdf(ncid,this);
      if (ret) {
         fprintf(stderr,"ERROR (%s) could not prepare the field object for netcdf formatting.\n",__func__);
         return 1;
      }
      ncdimids = rsprodMalloc(this->dims->nbdims * sizeof(int));

      /* DATASETS */
      /* check if the dataset already exists in the file. If yes, we are really
       * trying to overwrite some data, or to append new data to an UNLIMITED
       * dimension (if any). We should thus check that the dimension matches */
      int dataset_already_exists = 0;
      /* check how many variables there are in the file */
      int nvars;
      ret = rsprod_nc_handle_status(nc_inq_nvars(ncid,&nvars));
      if (ret) {
         fprintf(stderr,"ERROR (%s) could not get information on the number of datasets.\n",__func__);
         return 1;
      }
      rsprod_echo(stdout,"VERBOSE (%s) There are %d datasets in this file\n",__func__,nvars);
      if (nvars) {
         /* check if one a variable has the same name */
         for (int v = 0 ; v < nvars ; v++) {
            char name[NC_MAX_NAME+1];
            ret = rsprod_nc_handle_status(nc_inq_varname(ncid,v,name));
            if (ret) {
               fprintf(stderr,"WARNING (%s) Cannot access name of variable #%d.\n",__func__,v);
               continue;
            }
            if (!strcmp(name,this->name)) {
               dataset_already_exists = 1;
               break;
            }
         }
      }

      if (dataset_already_exists) {
         /* TODO: instead of barking: check if the dimension match. Special case for UNLIMITED dimensions */
         fprintf(stderr,"ERROR (%s) Dataset <%s> already exists in the file, but we do not support overwriting data in or appending to an existing dataset.\n",__func__,this->name);
         return 1;
      }

      /* DIMENSIONS */
      /* check if the dimensions of the current field are already defined in the
       * netcdf interface. If one is not, write it. */
      for (unsigned int d = 0; d < this->dims->nbdims; d++) {
         /* load dimensions from the file */
         int did;
         int rankd = d;
         ret = nc_inq_dimid(ncid, this->dims->name[rankd], &did);
         if (ret != NC_NOERR) {
            if (ret != NC_EBADDIM) {
               /* undefined error occured */
               ret = rsprod_nc_handle_status(ret);
               fprintf(stderr,"ERROR (%s) problem in loading exisiting dimensions from netCDF file.\n",__func__);
               return 1;
            } else {
               /* error occurred because the dimension does not exist yet. 
                * we should thus create it. */
               size_t dim_length = this->dims->length[rankd];
               /* swith length to NC_UNLIMITED if the dimension is unlimited */
               if (this->dims->unlimited[rankd]) {
                  dim_length = NC_UNLIMITED;
                  rsprod_echo(stderr,"%s:%d: Use nc_def_dim %s with NC_UNLIMITED (%u)\n",
                        __func__,__LINE__,this->dims->name[rankd],dim_length);
               } else {
                  rsprod_echo(stderr,"%s:%d: Use nc_def_dim %s with %u\n",
                        __func__,__LINE__,this->dims->name[rankd],dim_length);
               }
               ret = rsprod_nc_handle_status(nc_def_dim(ncid,this->dims->name[rankd],dim_length,&did));
               if (ret) {
                  fprintf(stderr,"ERROR (%s) problem in writing dimension %s.\n",__func__,this->dims->name[rankd]);
                  return 1;
               }
               rsprod_echo(stderr,"%s:%d: Associated dim_id is %u\n",
                     __func__,__LINE__,did);
            }
         } else {
            /* the dimension is already defined. Check if it has the same length. */
            size_t dsize;
            ret = rsprod_nc_handle_status(nc_inq_dimlen(ncid,did,&dsize));
            if (ret) {
               fprintf(stderr,"ERROR (%s) Cannot get info on length of existing dimensions %s.\n",__func__,
                     this->dims->name[rankd]);
               return 1;
            }
            if (dsize != this->dims->length[rankd]) {
               fprintf(stderr,"ERROR (%s) Incompatible length for dimension %s in netcdf file.\n",__func__,
                     this->dims->name[rankd]);
               fprintf(stderr,"\t(cont.)  Length is %u in netcdf file and %u in current field object.\n",
                     (unsigned int)dsize,this->dims->length[rankd]);
               return(1);
            }
         }
         ncdimids[rankd] = did;
      }

      /* re-arrange the dimension ids: in NetCDF (v3), the 
       * UNLIMITED dim must have id 0 */
      short  unlimited_dim = 0;
      for (unsigned int d = 0; d < this->dims->nbdims; d++) {
         if (this->dims->unlimited[d]) {
            /* catch an error */
            if (unlimited_dim) {
               fprintf(stderr,"ERROR (%s) NetCDF (v3) does not allow more than one UNLIMITED dimension.\n",__func__);
               return 1;
            }
            unlimited_dim = d;
         }
      }
      if (unlimited_dim) {
         fprintf(stderr,"ERROR (%s) The UNLIMITED dimension should be at index 0.\n",__func__);
         return 1;
      } /*else {//Either no unlimited dimension or it is already at position 0}*/
   }

   /* DEFINE THE VARIABLE */
   int fieldid;
   int ncType = convertType_rsprod2nc(rsprod_data_getType(this->data));
   ret = rsprod_nc_handle_status(nc_def_var(ncid, this->name,ncType,nc_ndims,ncdimids,&fieldid));
   if (ret) {
      fprintf(stderr,"ERROR (%s) Cannot define new variable <%s> in netcdf file.\n",__func__,this->name);
      return 1;
   }

   /* ATTRIBUTES */
   ret = rsprod_attributes_writeToNetCDF(this->attr,ncid,fieldid);
   if (ret) {
      fprintf(stderr,"ERROR (%s) Cannot write attributes for variable %s.\n",__func__,this->name);
      return 1;
   }

   /* Leave the define mode */
   ret = rsprod_nc_handle_status(nc_enddef(ncid));
   if (ret) {
      fprintf(stderr,"ERROR (%s) Cannot turn to netcdf data mode.\n",__func__);
      return 1;
   }

   if (field_has_data) {
      /* WRITE THE DATA */
      ret = rsprod_nc_put_var(this,ncid,fieldid);
      if (ret) {
         fprintf(stderr,"ERROR (%s) Cannot write values for %s in netcdf file.\n",__func__,this->name);
         return 1;
      }
   }

   if (field_has_data)
      free(ncdimids);
   return 0;
}
int rsprod_field_writeToNetCDF_glob_ncid;  /* global variable so that the _glob routine can be 
                                              called by the list routines (see rsprod_file_writeToNetCDF below)*/
static int rsprod_field_writeToNetCDF_glob(rsprod_field *this) {
   return rsprod_field_writeToNetCDF(this,rsprod_field_writeToNetCDF_glob_ncid);
}

/* 
 * NAME : rsprod_file_loadFromNetCDF
 *
 * PURPOSE : 
 *    Read a whole file from the netcdf file (all datasets + global attributes)
 *    and format it into a rsprod_file object.
 *
 * NOTE :
 *    o Needs a pre-opened 'ncid' to the current file. 
 *    o The routine will return the number of datasets that were effectively read.
 *    o Special value RSPROD_READALL will read all the fields (parameter fieldNames can then be NULL)
 *
 * TODO :
 *    o When the list of names is short wrt to the number of datasets in a file, it would maybe be more 
 *      efficient to search for the names directly instead of the current solution (try all the names and
 *      check if they match)
 *
 * RETURN CODE :
 *    -1    Error in memory, IO, etc...
 *     0->  Number of fields loaded.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 04.09.2009
 *
 * MODIFIED:
 *    Thomas Lavergne, met.no, 10.12.2009    :   add possibility to select some datasets from the file
 *    Thomas Lavergne, met.no, 17.12.2009    :   detect if the object was already allocated, in which case we append to it.
 *    Thomas Lavergne, met.no, 18.12.2009    :   change the return codes.
 *
 */
int rsprod_file_loadFromNetCDF(rsprod_file **this,int nbFieldsSearched, char *fieldNames[/*nbFieldsSearched*/],const int ncid) {

   int errorcode = -1;
   int number_of_fields_read = 0;

   int fieldid;
   int ret;


   /* check if the rsprod_file object is new of not */
   short new_rsprod_file = 1;
   if (*this) {
      new_rsprod_file = 0;
      rsprod_echo(stderr,"VERBOSE (%s) rsprod_file object is not empty, we shall append to it.\n",__func__);
   } else {
      rsprod_echo(stderr,"VERBOSE (%s) rsprod_file object was never used, we shall create it.\n",__func__);
   }
   
   /* Turn ncid to data mode (if necessary). */
   ret = rsprod_nc_handle_status(rsprod_nc_set_mode(ncid,NC_DAT_MODE));
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not change to data mode.\n",__func__);
      return (errorcode);
   }

   /* check how many variables there are in the file */
   int nvars;
   ret = rsprod_nc_handle_status(nc_inq_nvars(ncid,&nvars));
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not get information on the number of datasets.\n",__func__);
      return (errorcode);
   }
   rsprod_echo(stdout,"VERBOSE (%s) There are %d datasets in this file\n",__func__,nvars);
   if (!nvars) {
      fprintf(stderr,"ERROR (%s) No dataset in the netCDF file. Abort\n",__func__);
      return (errorcode);
   }
   dbl_list *datasets; 
   if (new_rsprod_file) {
      datasets = dbl_list_init(sizeof(rsprod_field));
   } else {
      datasets = (*this)->datasets;
   }
   size_t nbFieldsFound = 0;
   short  *fieldsFound = NULL; /* must be initialized to NULL! */
   if (nbFieldsSearched != RSPROD_READALL) {
      fieldsFound = rsprodMalloc(nbFieldsSearched*sizeof(fieldsFound[0]));
      memset(fieldsFound,0,nbFieldsSearched*sizeof(fieldsFound[0]));
   }
   /* go through the list of datasets in the file and load them */
   for (int v = 0 ; v < nvars ; v++) {
      /* stop the search if we found all the names */
      if (nbFieldsFound == nbFieldsSearched) 
         break;

      char name[NC_MAX_NAME+1];
      ret = rsprod_nc_handle_status(nc_inq_varname(ncid,v,name));
      if (ret) {
         fprintf(stderr,"WARNING (%s) Cannot access name of variable #%d.\n",__func__,v);
         continue;
      }
      /* Check if this name already exists in the rsprod_file object (we do not want to load it twice) */
      sprintf(rsprod_field_name_matches_string_param,"%s",name);
      dbl_node *node = dbl_list_findIf(datasets,NULL,&rsprod_field_name_matches_string);
      if (node) {
         rsprod_echo(stderr,"VERBOSE (%s) Skip dataset <%s> as it was already loaded in the rsprod_file object.\n",__func__,name);
         continue;
      }

      /* Select the fields by name */
      rsprod_echo(stderr,"VERBOSE (%s) Check if dataset <%s> is of interest to the user.\n",__func__,name);
      int read_this_field = 0;
      if (nbFieldsSearched == RSPROD_READALL)
         read_this_field = 1;
      else {
         for (size_t n = 0 ; n < nbFieldsSearched ; n++) { 
            if (!fieldsFound[n]) {
               if (!strcmp(name,fieldNames[n])) {
                  read_this_field = 1;
                  fieldsFound[n] = 1;
               }
            }
         }
      }
      if (!read_this_field) {
         rsprod_echo(stderr,"VERBOSE (%s) No, skip dataset <%s>.\n",__func__,name);
         continue;
      }
      
      /* If we arrive here, it means we want to load the dataset from the netCDF file */
      rsprod_echo(stderr,"VERBOSE (%s) Yes, load dataset <%s>.\n",__func__,name);
      rsprod_field *f;
      ret = rsprod_field_loadFromNetCDF(&f,name,ncid);
      if (ret) {
         fprintf(stderr,"ERROR (%s) Cannot load dataset #%d (%s).\n",__func__,v,name);
         return (errorcode);
      }
      /* unpack the field (default action, but can be parametrized at run time) */
      if (librsprod_unpack_datasets) {
         /* 
          * The unpacking is done in two steps:
          * The first one applies the scale and offset to the data, if the dataset is packed.
          * The second one casts the data to type librsprod_unpack_to, if not RSPROD_NAT
          */
         for (short step = 1 ; step <= 2 ; step++ ) {
            ret = rsprod_field_unpack(f);
            if (ret) {
               fprintf(stderr,"ERROR (%s) Cannot unpack dataset %s (step #%d).\n",__func__,name,step);
               return (errorcode);
            }
         }
      }
      dbl_node *dataset = dbl_node_createAssignContent(f);
      if (!dataset) {
         fprintf(stderr,"ERROR (%s) Cannot create a node for the list.\n",__func__);
         return (errorcode);
      }
      ret = dbl_list_addNode(datasets,dataset,LIST_POSITION_LAST);
      if (ret) {
         fprintf(stderr,"ERROR (%s) Cannot add dataset %s to the list of all datasets.\n",__func__,name);
         return (errorcode);
      }
      number_of_fields_read++;
   }

   free(fieldsFound); /* safe as long as the initialization to NULL is kept */

   /* load the global attributes */
   rsprod_attributes *glob_attr;
   if (new_rsprod_file) {
      ret = rsprod_attributes_loadFromNetCDF(&glob_attr,ncid,NC_GLOBAL);
      if (ret) {
         fprintf(stderr,"ERROR (%s) Cannot load the global attributes.\n",__func__);
         return (errorcode);
      }
   }

   /* put the datasets and the attributes in the file object */
   if (new_rsprod_file) {
      rsprod_file *tmp;
      ret = rsprod_file_create(&tmp,datasets,glob_attr);
      if (ret) {
         fprintf(stderr,"ERROR (%s) Cannot create the rsprod_file object.\n",__func__);
         return (errorcode);
      }
      *this = tmp;
   }

   return (number_of_fields_read);
}

/* 
 * NAME : rsprod_file_writeToNetCDF
 *
 * PURPOSE : 
 *    Write a whole file object to the netcdf file (all datasets + global attributes).
 *
 * NOTE :
 *    o Needs a pre-opened 'ncid' to the new file. 
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 04.09.2009
 *
 * MODIFIED:
 *
 */
int rsprod_file_writeToNetCDF(rsprod_file *this,const int ncid) {

   int ret;

   /* Turn ncid to define mode (if necessary). */
   ret = rsprod_nc_handle_status(rsprod_nc_set_mode(ncid,NC_DEF_MODE));
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not change to define mode.\n",__func__);
      return 1;
   }
   
   /* write the global attributes */
   ret = rsprod_attributes_writeToNetCDF(this->glob_attr,ncid,NC_GLOBAL);
   if (ret) {
      fprintf(stderr,"ERROR (%s) Did not manage to write the global attributes.\n",__func__);
      return 1;
   }

   /* go through all the datasets and write them to netCDf file */
   rsprod_field_writeToNetCDF_glob_ncid = ncid; /* global variable for use of rsprod_field_writeToNetCDF_glob() routine */
   ret = dbl_list_browseAndCheck(this->datasets,&rsprod_field_writeToNetCDF_glob,LIST_CHECK_KEEPMAX);
   if (ret) {
      fprintf(stderr,"ERROR (%s) Did not manage to write all the datasets to file.\n",__func__);
      return 1;
   }


   return 0;
}

int nc_file_read(char *fname, char *qstring, ...) {

   int  v_rsprod_file_accessContent(rsprod_file *this, char *qstring,va_list);

   int ret;

   /* read through the query string, and get the list of datasets that must be loaded */
   size_t nb_fieldnames;
   char **fieldnames;
   if (!get_field_names_from_query_string(qstring,&fieldnames,&nb_fieldnames)) {
      fprintf(stderr,"ERROR (%s) Cannot get the list of datasets to be read from the given query string:\n\t%s\n",
            __func__,qstring);
      return 0;
   }
   if (librsprod_echo_mode == LIBRSPROD_VERBOSE) {
      fprintf(stderr,"VERBOSE (%s): the list of %d datasets to be loaded is:\n",__func__,nb_fieldnames);
      for (size_t f = 0 ; f < nb_fieldnames ; f++) {
         fprintf(stderr,"\t%s\n",fieldnames[f]);
      }
   }
   /* open the netCDF file for reading */
   int ncid;
   ret = nc_open(fname,NC_NOCLOBBER,&ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) Could not open netCDF file for read access:\n\t%s\n",__func__,fname);
      return 0;
   }
   /* read the needed datasets from the netCDF file: */
   int nb_fieldnames_read;
   rsprod_file *file = NULL;
   nb_fieldnames_read = rsprod_file_loadFromNetCDF(&file,nb_fieldnames,fieldnames,ncid); 
   if (nb_fieldnames_read != nb_fieldnames) {
      fprintf(stderr,"ERROR (%s) Only managed to load %d datasets (out of %d) from netCDF file.\n",
            __func__,nb_fieldnames_read,nb_fieldnames);
      return 0;
   } 
   /* close the netCDF file */
   ret = nc_close(ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) Could not close netCDF file after read access:\n\t%s\n",__func__,fname);
      return 0;
   }

   /* map the datasets into memory slots for returning from the function */
   va_list ap;
   va_start( ap, qstring );
   int error = v_rsprod_file_accessContent(file,qstring,ap);
   if (error) {
      fprintf(stderr,"ERROR (%s) Cannot use query string for accessing the content of the netCDF file:\n\t%s\n",
            __func__,qstring);
      return 0;
   }
   va_end(ap);

   /* the memory is mapped and data is copied to the returned memory slots: we can now delete the rsprod_file object */
   rsprod_file_delete(file);

   return 1;
}

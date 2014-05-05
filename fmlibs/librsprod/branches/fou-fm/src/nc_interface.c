
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
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "rsprod_intern.h"
#include "rsprod_memory_utils.h"
#include "rsprod_string_utils.h"

#define NC_DEF_MODE 0
#define NC_DAT_MODE 1

/* base string for the 'number of characters' dimension. Usefull when dealing with character strings datasets */
/* This variable should be 'configurable' byt the user. */
#define NC_NUMCHARDIM_BASE "nch"

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
      default:
         ret = NC_NAT;
	 fprintf(stderr,"ERROR (convertType_rsprod2nc) Unknown rsprod type (%u).\n",type);
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
      default:
         ret = RSPROD_NAT;
	 fprintf(stderr,"ERROR (convertType_nc2rsprod) Unknown netcdf type (%d).\n",type);
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
   if (!strncmp(dname,NC_NUMCHARDIM_BASE,strlen(NC_NUMCHARDIM_BASE))) {
      isValid = 1;
   }
   return (isValid);
}

/* For an open netCDF with id ncid and mode either NC_DAT_MODE or NC_DEF_MODE. 
 *    make sure ncid is in mode $mode when leaving this routine. 
 */
int nc_set_mode(const int ncid,const int mode) {
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

/* Error handling for IO with netcdf files */
int nc_handle_status(int status) {
   if (status != NC_NOERR) {
      fprintf(stderr,"ERROR (nc_handle_status) IO problem to netcdf interface <%s>.\n",nc_strerror(status));
      return 1;
   }
   return 0;
}

/* 
 * NAME : nc_get_var
 *
 * PURPOSE:
 *   Type independent version of the nc_get_var_type family of 
 *   netcdf functions.
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */
int nc_get_var(rsprod_data *data,int ncid, int varid) {

   int ret;
   nc_type ncType;
   if (isValidType(rsprod_data_getType(data))) {
      ncType = convertType_rsprod2nc(rsprod_data_getType(data));
   } else {
      return 1;
   }
   void *values = rsprod_data_getValues(data);
   switch(ncType) {
      case NC_DOUBLE:
	 ret = nc_handle_status(nc_get_var_double(ncid,varid,values));
	 break;
      case NC_FLOAT:
	 ret = nc_handle_status(nc_get_var_float(ncid,varid,values));
	 break;
      case NC_SHORT:
	 ret = nc_handle_status(nc_get_var_short(ncid,varid,values));
	 break;
      case NC_INT:
	 ret = nc_handle_status(nc_get_var_int(ncid,varid,values));
	 break;
      default:
	 ret = 1;
	 fprintf(stderr,"ERROR (nc_get_var) unknown netCDF type.\n");
	 break;
   }
   
   if (ret) {
      fprintf(stderr,"ERROR (nc_get_var) problem with reading a variable from netCDF file.\n");
      return 1;
   }
   return 0;
}

/* 
 * NAME : nc_put_var_strings
 *
 * PURPOSE:
 *    Special case when writing a character strings dataset to 
 *    a netcdf file: we need to use several calls to 
 *    'nc_put_vara_text' as the rsprod_field's data can contain 
 *    many '\0' which we do not want to write.
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
 *
 */
int nc_put_var_strings(rsprod_field *this, int ncid, int varid) {

   int ret;
   size_t ndims = this->dims->nbdims;

   /* check that we have a final 'nchars' dimension in our dimension object */
   char *lastDimName = this->dims->name[ndims-1];
   if (!isValidStringLengthDimensionName(lastDimName)) {
      fprintf(stderr,"ERROR (nc_put_var_strings) Invalid name for the 'number of character' dimension in nc_char dataset: <%s>\n",
	    lastDimName);
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
      count[ndims-1] = strlen(stringValue)+1; /* We want to write one trailing '\0' */

      ret = nc_handle_status(nc_put_vara_text(ncid,varid,start,count,stringValue));
      if (ret) {
	 fprintf(stderr,"ERROR (nc_put_var_strings) Could not write string at position [");
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
 * NAME : nc_put_var
 *
 * PURPOSE:
 *   Type independent version of the nc_put_var_type family of 
 *   netcdf functions.
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */
int nc_put_var(rsprod_field *this,int ncid, int varid) {

   int ret;

   rsprod_type type = rsprod_data_getType(this->data);
   void *values = rsprod_data_getValues(this->data);

   nc_type ncType;
   if (isValidType(type)) {
      ncType = convertType_rsprod2nc(type);
   } else {
      return 1;
   }
   switch(ncType) {
      case NC_DOUBLE:
	 ret = nc_handle_status(nc_put_var_double(ncid,varid,values));
	 break;
      case NC_FLOAT:
	 ret = nc_handle_status(nc_put_var_float(ncid,varid,values));
	 break;
      case NC_SHORT:
	 ret = nc_handle_status(nc_put_var_short(ncid,varid,values));
	 break;
      case NC_INT:
	 ret = nc_handle_status(nc_put_var_int(ncid,varid,values));
	 break;
      case NC_CHAR:
	 /* special case here as we want to write a variable with character strings */
	 ret = nc_put_var_strings(this,ncid,varid); 
	 break;
      default:
	 ret = 1;
	 fprintf(stderr,"ERROR (nc_put_var) unknown netCDF type.\n");
	 break;
   }
   
   if (ret) {
      fprintf(stderr,"ERROR (nc_put_var) problem with reading a variable from netCDF file.\n");
      return 1;
   }
   return 0;
}

/* 
 * NAME : nc_get_attr
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
int nc_get_attr(rsprod_attr *attr,int ncid, int varid) {

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
	 ret = nc_handle_status(nc_get_att_double(ncid,varid,name,values));
	 break;
      case NC_FLOAT:
	 ret = nc_handle_status(nc_get_att_float(ncid,varid,name,values));
	 break;
      case NC_SHORT:
	 ret = nc_handle_status(nc_get_att_short(ncid,varid,name,values));
	 break;
      case NC_INT:
	 ret = nc_handle_status(nc_get_att_int(ncid,varid,name,values));
	 break;
      case NC_CHAR:
	 ret = nc_handle_status(nc_get_att_text(ncid,varid,name,values));
	 break;
      default:
	 ret = 1;
	 fprintf(stderr,"ERROR (nc_get_att) unknown netCDF type.\n");
	 break;
   }
   
   if (ret) {
      fprintf(stderr,"ERROR (nc_get_att) problem with reading an attribute from netCDF file.\n");
      return 1;
   }
   return 0;
}

/* 
 * NAME : nc_put_attr
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
int nc_put_attr(rsprod_attr *attr,int ncid, int varid) {

   int ret;
   rsprod_type type = rsprod_attr_getType(attr);
   char *name       = rsprod_attr_getName(attr);
   void *values     = rsprod_attr_getValues(attr);
   size_t length    = rsprod_attr_getNbvalues(attr);
   nc_type ncType;
   if (isValidType(type)) {
      ncType = convertType_rsprod2nc(type);
   } else {
      return 1;
   }
   switch(ncType) {
      case NC_DOUBLE:
	 ret = nc_handle_status(nc_put_att_double(ncid,varid,name,ncType,length,values));
	 break;
      case NC_FLOAT:
	 ret = nc_handle_status(nc_put_att_float(ncid,varid,name,ncType,length,values));
	 break;
      case NC_SHORT:
	 ret = nc_handle_status(nc_put_att_short(ncid,varid,name,ncType,length,values));
	 break;
      case NC_INT:
	 ret = nc_handle_status(nc_put_att_int(ncid,varid,name,ncType,length,values));
	 break;
      case NC_CHAR:
	 ret = nc_handle_status(nc_put_att_text(ncid,varid,name,length,values));
	 break;
      default:
	 ret = 1;
	 fprintf(stderr,"ERROR (nc_put_attr) unknown netCDF type.\n");
	 break;
   }
   
   if (ret) {
      fprintf(stderr,"ERROR (nc_put_attr) problem with writing an attribute in  netCDF file.\n");
      return 1;
   }
   return 0;
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
 *
 */
int rsprod_data_loadFromNetCDF(rsprod_data **this,unsigned long elems,int ncid,int fieldid) {

   int ret;
   /* Get type info (netcdf convention) */
   nc_type ncType;
   ret = nc_handle_status(nc_inq_vartype  (ncid, fieldid,&ncType));
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_data_loadFromNetCDF) could not load type info for variable.\n");
      return 1;
   }

   /* tranform to our internal type convention */
   rsprod_type myType = convertType_nc2rsprod(ncType);

   /* allocate the data object */
   ret = rsprod_data_create(this, myType,elems);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_data_loadFromNetCDF) problem in creating the data object.\n");
      return 1;
   }

   /* read values from the netcdf file and store them in our object. We do that only if the size is not 0 */
   if (elems) {
      ret = nc_get_var(*this,ncid,fieldid);
      if (ret) {
	 fprintf(stderr,"ERROR (rsprod_data_loadFromNetCDF) problem in using nc_get_var().\n");
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
 *
 */
int rsprod_dimensions_loadFromNetCDF(rsprod_dimensions **this,const int ncid, const int fieldid) {

   int ret;

   /* Read info on the dimensions */
   int *dimids,ndims;
   ret = nc_handle_status(nc_inq_varndims(ncid, fieldid, &ndims));
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_dimensions_loadFromNetCDF) cannot find number of dimensions.\n");
      return 1;
   }
   dimids = rsprodMalloc(ndims*sizeof(int));
   ret = nc_handle_status(nc_inq_vardimid(ncid, fieldid, dimids));
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_dimensions_loadFromNetCDF) cannot find IDs for %d dimensions.\n",ndims);
      return 1;
   }

   /* load the dimensions info (name and length) from the netcdf file */
   char **ncNames          = rsprodMalloc(ndims*sizeof(char *));
   unsigned int *ncLengths = rsprodMalloc(ndims*sizeof(unsigned int)); 
   short *ncOrders = rsprodMalloc(ndims*sizeof(short)); 
   for (unsigned int i=0; i < ndims; i++) {
      char string[NC_MAX_NAME+1];size_t length;
      /* read in file */
      ret = nc_handle_status(nc_inq_dim(ncid, dimids[i], string, &length));
      if (ret) {
         fprintf(stderr,"ERROR (rsprod_dimensions_loadFromNetCDF) cannot load info about dim %d.\n",dimids[i]);
         return 1;
      }
      /* process, trim and store the name */
      if (rsprod_string_trim_allNullsExceptLast(string,&(ncNames[i]))) {
	 fprintf(stderr,"ERROR (rsprod_dimensions_loadFromNetCDF) Problem dealing with null characters in name <%s>\n",string);
	 return 1;
      }
      /* store the length */
      ncLengths[i] = (unsigned int)length;

      /* by default order is +1 */
      ncOrders[i] = 1;
   }

   /* create the dimension object */
   ret = rsprod_dims_create(this,(const unsigned short)ndims,ncNames,ncLengths,ncOrders);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_dimensions_loadFromNetCDF) cannot create an rsprod_dimensions object.\n");
      return 1;
   }


   for (unsigned int i=0; i < ndims; i++) 
      free(ncNames[i]);
   free(dimids);
   free(ncNames);
   free(ncLengths);
   free(ncOrders);

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
int rsprod_attributes_loadFromNetCDF(List *this, const int ncid, const int fieldid) {

   int ret;

   /* get the number of attributes for this variable */
   int nbattr;
   ret = nc_handle_status(nc_inq_varnatts (ncid,fieldid,&nbattr));
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_attributes_loadFromNetCDF) cannot find the number of attributes for current variable.\n");
      return 1;
   }

   Position P;
   List L = MakeEmpty(NULL); 

   P = Header(L);
   /* load each attribute */
   for (int i = 0; i < nbattr; i++) {

      /* get the attribute's name */
      char name[NC_MAX_NAME+1];
      memset(name,0,NC_MAX_NAME+1);
      ret = nc_handle_status(nc_inq_attname(ncid,fieldid,i,name));
      if (ret) {
	 fprintf(stderr,"ERROR (rsprod_attributes_loadFromNetCDF) no name associated to attribute %d.\n",i);
	 return 1;
      }

      /* get additional information on the attribute */
      nc_type ncType;
      size_t len;
      ret = nc_handle_status(nc_inq_att(ncid,fieldid,name,&ncType,&len));
      if (ret) {
	 fprintf(stderr,"ERROR (rsprod_attributes_loadFromNetCDF) could not load additional info for attribute %d.\n",i);
	 return 1;
      }

      /* tranform to our internal type convention */
      rsprod_type myType = convertType_nc2rsprod(ncType);

      /* create the new attribute object */
      rsprod_attr *attr;
      //printf("VERBOSE (rsprod_attributes_loadFromNetCDF) name for attribute is <%s>\n",name);
      ret = rsprod_attr_create(&attr,name,myType,len);
      if (ret) {
	 fprintf(stderr,"ERROR (rsprod_attributes_loadFromNetCDF) could not create an attribute object with name %s.\n",name);
	 return 1;
      }
      
      /* read the value from the netcdf file */
      ret = nc_get_attr(attr,ncid,fieldid);

      /* insert it in the list of attributes */
      Insert(attr, L, P);
      P = Advance(P);
      
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
   ret = nc_handle_status(nc_set_mode(ncid,NC_DAT_MODE));
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_loadFromNetCDF) could not change to data mode.\n");
      return 1;
   }

   /* check if the variable is defined in this file */
   ret = nc_handle_status(nc_inq_varid(ncid,fieldname,&fieldid));
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_loadFromNetCDF) cannot find field %s in netCDF file.\n",fieldname);
      return 1;
   }

   /* load and create the dimensions */
   rsprod_dimensions *dims;
   ret = rsprod_dimensions_loadFromNetCDF(&dims,ncid,fieldid);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_loadFromNetCDF) cannot find and load dimensions for %s in netCDF file.\n",fieldname);
      return 1;
   }

   /* load and create the attributes */
   rsprod_attributes attr;
   ret = rsprod_attributes_loadFromNetCDF(&attr,ncid,fieldid);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_loadFromNetCDF) cannot find and load attributes for %s in netCDF file.\n",fieldname);
      return 1;
   }

   /* create and load the data from file*/
   rsprod_data *data;
   ret = rsprod_data_loadFromNetCDF(&data,dims->totelems,ncid,fieldid);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_loadFromNetCDF) cannot find and load data for %s in netCDF file.\n",fieldname);
      return 1;
   }

   /* create the field object */
   ret = rsprod_field_create(this,fieldname,dims,attr,data);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_loadFromNetCDF) cannot create Field object %s from netCDF file.\n",fieldname);
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
 *
 */
int rsprod_field_preprocess_for_netcdf(rsprod_field *this) {

   /* currently only for character string fields */
   if (rsprod_data_getType(this->data) == RSPROD_CHAR) {

      /* parse the character strings and find out which is the longest */
      size_t maxchars = 0;
      char **strarr = rsprod_data_getValues(this->data);
      for (size_t e = 0 ; e < rsprod_data_getNbvalues(this->data) ; e ++) {
	 size_t len = strlen(strarr[e]);
	 maxchars = (len>maxchars?len:maxchars);
      }
      maxchars++;
      /* Note: maxchars *does* include the terminating '\0' character.
       * This is because we want to write this '\0' into the netcdf character string _dataset_. */

      /* add the (fastest varying) dimension for the maximum number of characters in strings */
      rsprod_dims_addOne(this->dims,-1,NC_NUMCHARDIM_BASE,maxchars,1);

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
 *
 */
int rsprod_attributes_writeToNetCDF(rsprod_attributes this,const int ncid, const int fieldid) {

   int ret;
   Position P = Header(this);
   rsprod_attr *attr;
   
   /* Turn ncid to define mode (if necessary). */
   ret = nc_handle_status(nc_set_mode(ncid,NC_DEF_MODE));
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_writeToNetCDF) could not change to define mode.\n");
      return 1;
   }
   
   if ( !IsEmpty(this) ) {
      do {
	 P = Advance(P);

	 attr = Retrieve(P);
	 ret = nc_put_attr(attr,ncid,fieldid);
	 if (ret) {
	    fprintf(stderr,"ERROR (rsprod_attributes_writeToNetCDF) problem in writing attribute %s in netcdf file.\n",
		  rsprod_attr_getName(attr));
	    return 1;
	 }
      } while (!IsLast(P, this));
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
 *
 */
int rsprod_field_writeToNetCDF(rsprod_field *this,const int ncid) {

   int ret;
   
   /* Turn ncid to define mode (if necessary). */
   ret = nc_handle_status(nc_set_mode(ncid,NC_DEF_MODE));
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_writeToNetCDF) could not change to define mode.\n");
      return 1;
   }

   /* Pre-process the field to make it compatible with the netcdf formatting requirements. */
   /* This currently concerns only the NC_CHAR datasets which require an extra dimension 
    * (max_nb_char) in order to be stored and later accessed. */
   ret = rsprod_field_preprocess_for_netcdf(this);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_field_writeToNetCDF) could not prepare the field object for netcdf formatting.\n");
      return 1;
   }

   /* DIMENSIONS */
   /* check if the dimensions of the current field are already defined in the
    * netcdf interface. If one is not, write it. */
   int *ncdimids = rsprodMalloc(this->dims->nbdims * sizeof(int));
   for (unsigned int d = 0; d < this->dims->nbdims; d++) {
      /* load dimensions from the file */
      int did;
      int rankd = d;
      ret = nc_inq_dimid(ncid, this->dims->name[rankd], &did);
      if (ret != NC_NOERR) {
	 if (ret != NC_EBADDIM) {
	    /* undefined error occured */
	    ret = nc_handle_status(ret);
	    fprintf(stderr,"ERROR (rsprod_field_writeToNetCDF) problem in loading exisiting dimensions from netCDF file.\n");
	    return 1;
	 } else {
	    /* error occurred because the dimension does not exist yet. 
	     * we should thus create it. */
	    ret = nc_handle_status(nc_def_dim(ncid,this->dims->name[rankd],this->dims->length[rankd],&did));
	    if (ret) {
	       fprintf(stderr,"ERROR (rprod_field_writeToNetCDF) problem in writing dimension %s.\n",this->dims->name[rankd]);
	       return 1;
	    }
	 }
      } else {
	 /* the dimension is already defined. Check if it has the same length. */
	 size_t dsize;
	 ret = nc_handle_status(nc_inq_dimlen(ncid,did,&dsize));
	 if (ret) {
	    fprintf(stderr,"ERROR (rprod_field_writeToNetCDF) Cannot get info on length of existing dimensions %s.\n",
		  this->dims->name[rankd]);
	    return 1;
	 }
	 if (dsize != this->dims->length[rankd]) {
	    fprintf(stderr,"ERROR (rprod_field_writeToNetCDF) Incompatible length for dimension %s in netcdf file.\n",
		  this->dims->name[rankd]);
	    fprintf(stderr,"\t(cont.)  Length is %u in netcdf file and %u in current field object.\n",
		  (unsigned int)dsize,this->dims->length[rankd]);
	    return(1);
	 }
      }
      ncdimids[rankd] = did;
   }

   /* DEFINE THE VARIABLE */
   int fieldid;
   int ncType = convertType_rsprod2nc(rsprod_data_getType(this->data));
   ret = nc_handle_status(nc_def_var(ncid, this->name,ncType,this->dims->nbdims,ncdimids,&fieldid));
   if (ret) {
      fprintf(stderr,"ERROR (rprod_field_writeToNetCDF) Cannot define new variable <%s> in netcdf file.\n",this->name);
      return 1;
   }
   
   /* ATTRIBUTES */
   ret = rsprod_attributes_writeToNetCDF(this->attr,ncid,fieldid);
   if (ret) {
      fprintf(stderr,"ERROR (rprod_field_writeToNetCDF) Cannot write attributes for variable %s.\n",this->name);
      return 1;
   }

   /* Leave the define mode */
   ret = nc_handle_status(nc_enddef(ncid));
   if (ret) {
      fprintf(stderr,"ERROR (rprod_field_writeToNetCDF) Cannot turn to netcdf data mode.\n");
      return 1;
   }

   /* WRITE THE DATA */
   ret = nc_put_var(this,ncid,fieldid);
   if (ret) {
      fprintf(stderr,"ERROR (rprod_field_writeToNetCDF) Cannot write values for %s in netcdf file.\n",this->name);
      return 1;
   }

   free(ncdimids);
   return 0;
}


/* 
 * NAME : hdf4_interface.c
 *
 * PURPOSE:
 *    Implement all the interface between the rsprod and the HDF4 (SDS) library.
 *    All the reading and writing to/from this format should be contained here.
 *
 * AUTHOR:    
 *    Thomas Lavergne, met.no, 17.08.2009
 *
 * MODIFIED:
 *
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mfhdf.h>
#include "rsprod_intern.h"
#include "runtime_config.h"
#include "rsprod_report_utils.h"
#include "rsprod_memory_utils.h"
#include "rsprod_string_utils.h"
#include "hdf4_interface.h"

#define HDF4_DIMNAME_LENGTH 64
#define HDF4_ATTNAME_LENGTH 64

/* 
 * NAME: convertType_rsprod2hdf4,
 *       convertType_hdf42rsprod
 *
 * PURPOSE:
 *    Convert a rsprod_type to/from hdf4_type.
 *       
 */
rsprod_type convertType_hdf42rsprod(nc_type type) {

   rsprod_type ret;
   switch(type) {
      case DFNT_FLOAT64: 
         ret = RSPROD_DOUBLE;
         break;
      case DFNT_FLOAT32: 
         ret = RSPROD_FLOAT;
         break;
      case DFNT_INT16: 
         ret = RSPROD_SHORT;
         break;
      case DFNT_INT32: 
         ret = RSPROD_INT;
         break;
      case DFNT_CHAR: 
         ret = RSPROD_CHAR;
         break;
      default:
         ret = RSPROD_NAT;
         fprintf(stderr,"ERROR (%s) Unknown HDF4 type (%d).\n",__func__,type);
         break;
   }
   return (ret);
}

/* Error handling for IO with HDF4 files */
/*
int nc_handle_status(intn status) {
   if (status != NC_NOERR) {
      fprintf(stderr,"ERROR (nc_handle_status) IO problem to netcdf interface <%s>.\n",nc_strerror(status));
      return 1;
   }
   return 0;
}
*/

/* 
 * NAME : hdf4_get_attr
 *
 * PURPOSE:
 *    Wrapper around SDreadattr()
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 17.08.2009
 *
 * MODIFIED:
 *
 */
int hdf4_get_attr(rsprod_attr *attr, int32 sds_id, int32 attr_index) {
   
   int ret;
   intn status;

   rsprod_type type = rsprod_attr_getType(attr);
   char *name       = rsprod_attr_getName(attr);
   void *values     = rsprod_attr_getValues(attr);
   
   nc_type ncType;
   if (!isValidType(type)) {
      return 1;
   }

   status = SDreadattr(sds_id,attr_index,values);
   if (status == FAIL) {
      fprintf (stderr,"ERROR (%s) Cannot use SDreadattr() for dataset %d\n",__func__,attr_index); 
      return 1;
   }


   return 0;
}

/* 
 * NAME : hdf4_get_var
 *
 * PURPOSE:
 *    Wrapper around SDreaddata()
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 17.08.2009
 *
 * MODIFIED:
 *
 */
int hdf4_get_var(rsprod_data *data, rsprod_dimensions *dims, int32 sds_id) {
   
   int ret;
   intn status;

   rsprod_type type = rsprod_data_getType(data);
   void *values     = rsprod_data_getValues(data);
   
   nc_type ncType;
   if (!isValidType(type)) {
      return 1;
   }

   /* create the start, stride and edges arrays for calling SDreaddata() */
   unsigned short nbdims = dims->nbdims;
   int32 *start  = rsprodMalloc(sizeof(start[0]*nbdims));
   int32 *stride = NULL; //rsprodMalloc(sizeof(stride[0]*nbdims));
   int32 *edge   = rsprodMalloc(sizeof(edge[0]*nbdims));
   for (unsigned short d = 0 ; d < nbdims ; d++) {
      start[d]   = 0;
      //stride[d]  = 1;
      edge[d]    = dims->length[d];
   }

   /* call to SDreaddata */
   status = SDreaddata (sds_id, start, stride, edge, values);
   if (status == FAIL) {
      fprintf (stderr,"ERROR (%s) Cannot use SDreaddata() for dataset %d\n",__func__,sds_id); 
      return 1;
   }

   return 0;
}


/* 
 * NAME : rsprod_dimensions_loadFromHDF4
 *
 * PURPOSE : 
 *    Read the dimensions for a given dataset in the hdf4 file
 *    and format them into a rsprod_dimensions object.
 *
 * NOTE :
 *    Needs a pre-opened 'sd_id' to the current file and a
 *    pre-opened 'sds_id' to the dataset.
 *
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 17.08.2009
 *
 * MODIFIED:
 *
 */
int rsprod_dimensions_loadFromHDF4(rsprod_dimensions **this,const int32 sd_id, const int32 sds_id) {

   int  ret;
   intn status;
   
   /* Access information on the dataset */
   int32 rank;
   int32 dimsizes[H4_MAX_VAR_DIMS];
   int32 data_type;
   int32 num_attrs;
   status = SDgetinfo(sds_id,NULL,&rank,dimsizes,&data_type,&num_attrs);
   if (status == FAIL) { 
      fprintf (stderr,"ERROR (%s) Failed to access information for the current dataset.\n",__func__); 
      return 1;
   }

   if (librsprod_echo_mode == LIBRSPROD_VERBOSE) {
      fprintf(stdout,"In %s: Dataset has %d dimensions. Dimensions have length:\n\t",__func__,rank);
      for (int d = 0 ; d < rank ; d++) {
         printf("%d ",dimsizes[d]);
      }
      printf("\n");
   }

   /* access information on the dimensions and create the internal object */
   char         **dimNames  = rsprodMalloc(rank*sizeof(char *));
   unsigned int *dimLengths = rsprodMalloc(rank*sizeof(unsigned int)); 
   short        *dimUnlims  = rsprodMalloc(rank*sizeof(short));

   short dim_read_ok = 1;
   for (intn dim_index = 0 ; dim_index < rank ; dim_index++) {
      int32 dim_id = SDgetdimid(sds_id,dim_index);
      if (dim_id == FAIL) {
         fprintf (stderr,"ERROR (%s) Cannot get ID for dimension %d (%d/%d)\n",__func__,dim_index,dim_index+1,rank); 
         dim_read_ok = 0;
         continue;
      }
      char string[HDF4_DIMNAME_LENGTH+1];
      int32 dim_size, dim_data_type, dim_num_attrs;
      status = SDdiminfo(dim_id,string,&dim_size,&dim_data_type,&dim_num_attrs);
      if (status == FAIL) {
         fprintf (stderr,"ERROR (%s) Failed to access information.\n",__func__); 
         dim_read_ok = 0;
         continue;
      }
      if (dim_data_type != 0) {
         fprintf (stderr,"WARNING (%s) Dimension <%s> is not a coordinate dimension (%d)!\n",__func__,string,dim_data_type); 
         dim_read_ok = 0;
         continue;
      }
      if (rsprod_string_trim_allNullsExceptLast(string,&(dimNames[dim_index]))) {
         fprintf (stderr,"ERROR (%s) Problem dealing with null characters in name <%s>\n",__func__,string); 
         dim_read_ok = 0;
         continue;
      }
      dimLengths[dim_index] = dim_size;
      dimUnlims[dim_index]  = 0; /* do NOT support unlimited dimensions in HDF4 */
      rsprod_echo(stdout,"In %s: Dimension %d/%d has name <%s> and size %d. Type is %d.\n",__func__,
            dim_index+1,rank,
            dimNames[dim_index],dimLengths[dim_index],dim_data_type);
   }
   if (!dim_read_ok) {
      fprintf (stderr,"ERROR (%s) Failed to read dimensions for the current dataset.\n",__func__); 
      return 1;
   }

   /* create the dimension object */
   ret = rsprod_dims_create(this,(const unsigned short)rank,dimNames,dimLengths,dimUnlims);
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot create an rsprod_dimensions object.\n",__func__);
      return 1;
   }

   for (unsigned int i=0; i < rank; i++) 
      free(dimNames[i]);
   free(dimNames);
   free(dimLengths);
   free(dimUnlims);

   return 0;
}

/* 
 * NAME : rsprod_attributes_loadFromHDF4
 *
 * PURPOSE : 
 *    Read the attributes for a given dataset in the HDF4 file
 *    and format them into a rsprod_attributes object (list of rsprod_attr objects).
 *
 * NOTE :
 *    o Needs a pre-opened 'sd_id' to the current file and a
 *      pre-opened 'sds_id' to the dataset.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 17.08.2009
 *
 * MODIFIED:
 *
 */
int rsprod_attributes_loadFromHDF4(rsprod_attributes **this, const int sd_id, const int sds_id) {

   int ret;
   intn status;

   int32 num_attrs;
   /* check if we want to access attributes to the file (global) or to the dataset */
   if (sd_id == sds_id) {
      /* GLOBAL (file) attributes */
      rsprod_echo(stdout,"VERBOSE (%s) Access GLOBAL attributes.\n",__func__);

      int32 nvars;
      status = SDfileinfo(sd_id,&nvars,&num_attrs);
      if (status == FAIL) {
         fprintf(stderr,"ERROR (%s) could not get information on the number of global attributes.\n",__func__);
         return (1);
      }
      rsprod_echo(stdout,"VERBOSE (%s) There are %d global attributes in this file\n",__func__,num_attrs);
   } else {
      /* attributes for a dataset */
      int32 rank;
      int32 dimsizes[H4_MAX_VAR_DIMS];
      int32 data_type;
      status = SDgetinfo(sds_id,NULL,&rank,dimsizes,&data_type,&num_attrs);
      if (status == FAIL) { 
         fprintf (stderr,"ERROR (%s) Failed to access information for the current dataset.\n",__func__); 
         return 1;
      }
      rsprod_echo(stdout,"In %s: Dataset has %d attributes.\n",__func__,num_attrs);
   }
   
   dbl_list *L = dbl_list_init(sizeof(rsprod_attr *));

   /* load each attribute */
   short att_read_ok = 1;
   for (int32 attr_index = 0; attr_index < num_attrs; attr_index++) {

      /* get the attribute's name and other info */
      char   name[HDF4_ATTNAME_LENGTH+1];
      int32 hdf4Type, count;
      status = SDattrinfo(sds_id,attr_index,name,&hdf4Type,&count);
      if (status == FAIL) {
         fprintf (stderr,"ERROR (%s) Failed to access information attribute %d.\n",__func__,attr_index); 
         return 1;
      }
      rsprod_echo(stdout,"In %s: Attribute %d has name <%s>, type %d and count %d.\n",__func__,attr_index,name,hdf4Type,count);

      /* tranform to our internal type convention */
      rsprod_type myType = convertType_hdf42rsprod(hdf4Type);
      
      /* create the new attribute object */
      rsprod_attr *attr;
      ret = rsprod_attr_create(&attr,name,myType,count);
      if (ret) {
         fprintf(stderr,"ERROR (%s) could not create an attribute object with name %s.\n",__func__,name);
         return 1;
      }
      /* load the value from the file */
      ret = hdf4_get_attr(attr,sds_id,attr_index);
      if (ret) {
         fprintf(stderr,"ERROR (%s) cannot read values for attribute %s.\n",__func__,name);
         return 1;
      }
      
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
 * NAME : rsprod_data_loadFromHDF4
 *
 * PURPOSE : 
 *    Read a hdf4 'datafield' (SDS) and store it into a rsprod_data 
 *
 * NOTE :
 *    Needs a pre-opened 'sds_id' to the current file.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 18.08.2009
 *
 * MODIFIED:
 *
 */
int rsprod_data_loadFromHDF4(rsprod_data **this,rsprod_dimensions *dims,int32 sd_id,int32 sds_id) {

   int ret;
   intn status;

   unsigned int elems = dims->totelems;

   /* Access information on the dataset */
   int32 rank;
   int32 dimsizes[H4_MAX_VAR_DIMS];
   int32 hdf4Type;
   int32 num_attrs;
   status = SDgetinfo(sds_id,NULL,&rank,dimsizes,&hdf4Type,&num_attrs);
   if (status == FAIL) { 
      fprintf (stderr,"ERROR (%s) Failed to access information for the current dataset.\n",__func__); 
      return 1;
   }
   /* type convertion */
   rsprod_type myType = convertType_hdf42rsprod(hdf4Type);
   if (myType == RSPROD_NAT) {
      /* error message is provided by convertType_hdf42rsprod() */
      return 1;
   }
   rsprod_echo(stdout,"In %s: Dataset has type %d (orig: %d).\n",__func__,myType,hdf4Type);

   /* allocate the data object */
   ret = rsprod_data_create(this, myType, elems);
   if (ret) {
      fprintf(stderr,"ERROR (%s) problem in creating the data object.\n",__func__);
      return 1;
   }

   /* read values from the hdf4 file and store them in our object. We do that only if the size is not 0 */
   if (elems) {
      ret = hdf4_get_var(*this,dims,sds_id);
      if (ret) {
         fprintf(stderr,"ERROR (%s) problem in using hdf4_get_var().\n",__func__);
         return 1;
      } 
   }

   return 0;

}

/* 
 * NAME : rsprod_field_loadFromHDF4
 *
 * PURPOSE : 
 *    Read a whole field/dataset from the HDF4 file (data+dimensions+attributes)
 *    and format it into a rsprod_field object.
 *
 * NOTE :
 *    o Needs a pre-opened 'fileid' to the current file. The field is 
 *      searched by means of its name.
 *
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 17.08.2009
 *
 * MODIFIED:
 *
 */
int rsprod_field_loadFromHDF4(rsprod_field **this,char *fieldname,const int32 sd_id) {
   int  ret;
   intn status;

   /* search and select the SDS dataset by name */
   int32 sds_index = SDnametoindex (sd_id, fieldname);
   if (sds_index == FAIL) { 
      fprintf (stderr,"ERROR (%s) Data set with the name <%s> does not exist\n",__func__,fieldname); 
      return 1;
   }
   int32 sds_id = SDselect (sd_id, sds_index);
   if (sds_id == FAIL) { 
      fprintf (stderr,"ERROR (%s) Failed to access ID for dataset with index %d (%s)\n",__func__,fieldname,sds_index,fieldname); 
      return 1;
   }

   /* load and create the dimensions */
   rsprod_dimensions *dims;
   ret = rsprod_dimensions_loadFromHDF4(&dims,sd_id,sds_id);
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot find and load dimensions for %s in HDF4 file.\n",__func__,fieldname);
      return 1;
   }

   /* load and create the attributes */
   rsprod_attributes *attr;
   ret = rsprod_attributes_loadFromHDF4(&attr,sd_id,sds_id);
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot find and load attributes for %s in HDF4 file.\n",__func__,fieldname);
      return 1;
   }

   /* load and create the data */
   rsprod_data *data;
   ret = rsprod_data_loadFromHDF4(&data,dims,sd_id,sds_id);
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot find and load data for %s in HDF4 file.\n",__func__,fieldname);
      return 1;
   }

   /* create the field object */
   ret = rsprod_field_create(this,fieldname,dims,attr,data);
   if (ret) {
      fprintf(stderr,"ERROR (%s) cannot create Field object %s from netCDF file.\n",__func__,fieldname);
      return 1;
   }



   /* close access to the dataset */
   status = SDendaccess (sds_id);

   return 0;
}

/* 
 * NAME : rsprod_file_loadFromHDF4
 *
 * PURPOSE : 
 *    Read a whole file from the hdf4 file (all SDS datasets + global attributes)
 *    and format it into a rsprod_file object.
 *
 * NOTE :
 *    o Needs a pre-opened 'sd_id' to the current file. 
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
 *    Thomas Lavergne, met.no, 16.02.2010    :   start version from the corresponding NetCDF routine
 *
 * MODIFIED:
 *
 */
int rsprod_file_loadFromHDF4(rsprod_file **this,int nbFieldsSearched, char *fieldNames[/*nbFieldsSearched*/],const int32 sd_id) {

   int errorcode = -1;
   int number_of_fields_read = 0;

   int fieldid;
   int ret;
   intn status;


   /* check if the rsprod_file object is new of not */
   short new_rsprod_file = 1;
   if (*this) {
      new_rsprod_file = 0;
      rsprod_echo(stderr,"VERBOSE (%s) rsprod_file object is not empty, we shall append to it.\n",__func__);
   } else {
      rsprod_echo(stderr,"VERBOSE (%s) rsprod_file object was never used, we shall create it.\n",__func__);
   }
   
   /* check how many variables there are in the file */
   int32 nvars, n_file_attr;
   status = SDfileinfo(sd_id,&nvars,&n_file_attr);
   if (status == FAIL) {
      fprintf(stderr,"ERROR (%s) could not get information on the number of datasets.\n",__func__);
      return (errorcode);
   }
   rsprod_echo(stdout,"VERBOSE (%s) There are %d datasets in this file\n",__func__,nvars);
   if (!nvars) {
      fprintf(stderr,"ERROR (%s) No dataset in the HDF4 file. Abort\n",__func__);
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

      int32 sds_id = SDselect(sd_id,v);
      int32 rank, attributes, num_type;
      int32 dim_sizes[H4_MAX_VAR_DIMS];
      char name[H4_MAX_NC_NAME+1];
      status = SDgetinfo(sds_id, name, &rank, dim_sizes,&num_type, &attributes);
      if (status == FAIL) {
         fprintf(stderr,"WARNING (%s) Cannot access info of variable #%d.\n",__func__,v);
         continue;
      }
      status = SDendaccess(sds_id);

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
      ret = rsprod_field_loadFromHDF4(&f,name,sd_id);
      if (ret) {
         fprintf(stderr,"ERROR (%s) Cannot load dataset #%d (%s).\n",__func__,v,name);
         return (errorcode);
      }
      if (librsprod_echo_mode == LIBRSPROD_VERBOSE) {
         rsprod_field_printInfo(f);
      }
      /* unpack the field (default action, but can be parametrized at run time) */
      //if (librsprod_unpack_datasets) {
      if (0) {
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
      ret = rsprod_attributes_loadFromHDF4(&glob_attr,sd_id,sd_id);
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

   if (librsprod_echo_mode == LIBRSPROD_VERBOSE) {
      fprintf(stdout,"VERBOSE (%s) File was read as:\n",__func__);
      rsprod_file_printInfo(*this);
      fprintf(stdout,"VERBOSE (%s) END file ***\n",__func__);
   }

   return (number_of_fields_read);
}

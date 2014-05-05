/*
 * NAME: file.c
 * 
 * PURPOSE: 
 *    Implement the functionalities of a file object. 
 *
 * DESCRIPTION:
 *    In rsprod, a file is composed of a list of fields and a list of global attributes.
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 04.09.2009
 *
 * MODIFIED:
 *
 */ 
#include <stdio.h> 
#include <stdlib.h> 
#include <stdarg.h>
#include "rsprod_intern.h"
#include "runtime_config.h"
#include "rsprod_report_utils.h"
#include "rsprod_memory_utils.h"
#include "rsprod_string_utils.h"
#include "query_string.h"

/* 
 * NAME : rsprod_file_create
 *
 * PURPOSE:
 *    Allocate and initialize a rsprod_file object with the given datasets and attributes.
 *
 * NOTE:
 *    The various parameters are 'pointed to' so they must not be de-allocated by the calling routine.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 04.09.2009
 *
 * MODIFIED:
 *
 */ 
int rsprod_file_create(rsprod_file **this,dbl_list *datasets,rsprod_attributes *glob_attr) {
   
   rsprod_file *tmp = rsprodMalloc(sizeof(rsprod_file));
   tmp->datasets    = datasets;
   tmp->glob_attr   = glob_attr;
   
   *this = tmp;
   return 0;
}

/* 
 * NAME : rsprod_file_delete
 *
 * PURPOSE:
 *    De-allocate the memory space of the field (and of its elements).
 *
 * NOTE:
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 04.09.2009
 *
 * MODIFIED:
 *
 */ 
void rsprod_file_delete(rsprod_file *this) {
   dbl_list_delete(this->datasets,&rsprod_field_delete);
   rsprod_attributes_delete(this->glob_attr);
}

/* 
 * NAME : rsprod_file_printInfo
 *
 * PURPOSE:
 *    print some info on the file
 *
 * NOTE:
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 04.09.2009
 *
 * MODIFIED:
 *
 */ 
void rsprod_file_printInfo(rsprod_file *this) {
   fprintf(stderr,"DATASETS:\n");
   if (this->datasets) 
      dbl_list_browse(this->datasets,rsprod_field_printInfo);
   else
      fprintf(stderr,"NONE\n");

   fprintf(stderr,"\nGLOBAL ATTRIBUTES:\n");
   if (this->glob_attr) 
      rsprod_attributes_printInfo(this->glob_attr);
   else
      fprintf(stderr,"NONE\n");
}

void rsprod_file_printNcdump(rsprod_file *this) {

   int rsprod_file_collectAllDims(rsprod_file *this, rsprod_dimensions **dims);
   rsprod_dimensions *allDims;
   int ret = rsprod_file_collectAllDims(this,&allDims);

   fprintf(stderr,"file-name {\n");
   fprintf(stderr,"dimensions:\n");
   if (allDims) {
      rsprod_dims_printNcdump(allDims);
      rsprod_dims_delete(allDims);
   } else {
      fprintf(stderr,"\t<no dims>\n");
   }

   fprintf(stderr,"variables:\n");
   if (this->datasets) 
      dbl_list_browse(this->datasets,rsprod_field_printNcdump);
   else
      fprintf(stderr,"\tno datasets\n");

   fprintf(stderr,"\n// global attributes:\n");
   if (this->glob_attr) 
      rsprod_attributes_printNcdump(this->glob_attr);
   else
      fprintf(stderr,"\tno global attributes\n");

   fprintf(stderr,"}\n");
}

/* 
 * NAME : rsprod_file_collectAllDims
 *
 * PURPOSE:
 *    go through a file object and collect all the (unique) dimensions
 *    into one dimension object
 *
 * NOTE:
 *    o Dimensions are not really attached to a file, but to individual datasets.
 *      It can, however, be handy to be able to collect all the dimensions from
 *      a file, for example for consistency checks or for printNcdump().
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 10.11.2010
 *
 * MODIFIED:
 *
 */
int rsprod_file_collectAllDims(rsprod_file *this, rsprod_dimensions **dims) {

   /* check if the file object has some datasets */
   if (!this->datasets) {
      fprintf(stderr,"WARNING (%s) File object does not have any dataset\n",__func__);
      return 0;
   }

   /* go though all the remaining datasets, and add the unique dimensions to the new dimension object */
   rsprod_field_join_dims_glob_dest = NULL;
   int ret = dbl_list_browseAndCheck(this->datasets,&rsprod_field_join_dims_glob,LIST_CHECK_KEEPMAX);
   if (ret) {
      fprintf(stderr,"ERROR (%s) Cannot join the dimension objects for all datasets\n",__func__);
      return 1;
   }

   /* return the allDims object as a copy */
   *dims = rsprod_dims_copy(rsprod_field_join_dims_glob_dest);

   /* de-allocate the memory under global variable rsprod_field_join_dims_glob_dest */
   rsprod_dims_delete(rsprod_field_join_dims_glob_dest);

   return 0;
}

/* 
 * NAME : rsprod_file_addDataset
 *
 * PURPOSE:
 *    Add a dataset to the list of datasets
 *
 * NOTE:
 *    o The dataset is not copied but assigned to the rsprod_file
 *      object. It should thus not be modified or released from
 *      the calling software after call to this routine.
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 07.09.2009
 *
 * MODIFIED:
 *
 */ 
int rsprod_file_addDataset(rsprod_file *this, rsprod_field *f) {

   if (!(this->datasets)) {
      rsprod_echo(stderr,"VERBOSE (%s) Initialize the list of datasets since it is empty.\n",__func__);
      this->datasets = dbl_list_init(sizeof(*f));
   }
   /*
   if (librsprod_echo_mode == LIBRSPROD_VERBOSE) {
      fprintf(stderr,"Entering %s, file's datasets are:\n",__func__);
      dbl_list_browse(this->datasets,rsprod_field_printInfo);
      fprintf(stderr,"Dataset to be added is:\n");
      rsprod_field_printInfo(f);
   }
   */

   dbl_node *dataset = dbl_node_createAssignContent(f);
   if (!dataset) {
      fprintf(stderr,"ERROR (%s) Cannot create a node for the list.\n",__func__);
      return 1;
   }
   int ret = dbl_list_addNode(this->datasets,dataset,LIST_POSITION_LAST);
   if (ret) {
      fprintf(stderr,"WARNING (%s) Cannot add dataset %s to the list of all datasets.\n",__func__,f->name);
      return 1;
   }

   /*
   if (librsprod_echo_mode == LIBRSPROD_VERBOSE) {
      fprintf(stderr,"Exiting %s, file's datasets are:\n",__func__);
      dbl_list_browse(this->datasets,rsprod_field_printInfo);
   }
   */
   return 0;
}

/* 
 * NAME : rsprod_file_removeDataset
 *
 * PURPOSE:
 *    remove a dataset from the list of datasets
 *
 * NOTE:
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 18.12.2009
 *
 * MODIFIED:
 *
 */ 
int rsprod_file_removeDataset(rsprod_file *this, char fieldname[]) {

   sprintf(rsprod_field_name_matches_string_param,"%s",fieldname);
   dbl_node *node = dbl_list_findIf(this->datasets,NULL,&rsprod_field_name_matches_string);
   if (!node) {
      fprintf(stderr,"ERROR (%s) Cannot find field with name <%s> in current file.\n",__func__,fieldname);
      return 1;
   }

   if (dbl_list_removeNode(this->datasets,node,&rsprod_field_delete)) {
      fprintf(stderr,"ERROR (%s) Unable to remove field with name <%s> from the list of datasets.\n",__func__,
            ((rsprod_field *)(node->c))->name);
      return 1;
   }

   return 0;
}

/* 
 * NAME : rsprod_file_getDataset
 *
 * PURPOSE:
 *    Search a dataset by its name and set a pointer to it
 *
 * NOTE:
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 11.12.2009
 *
 * MODIFIED:
 *
 */ 
int rsprod_file_getDataset(rsprod_file *this, rsprod_field **field_p, char *fieldname) { 

   sprintf(rsprod_field_name_matches_string_param,"%s",fieldname);
   dbl_node *node = dbl_list_findIf(this->datasets,NULL,&rsprod_field_name_matches_string);
   if (!node) {
      fprintf(stderr,"ERROR (%s) Cannot find field with name <%s> in current file.\n",__func__,fieldname);
      return 1;
   }
   *field_p = node->c;

   return 0;
}

/* 
 * NAME : rsprod_file_accessContent
 *
 * PURPOSE:
 *    give access to file content by using query strings.
 *
 * NOTE:
 *    make use of VA_args
 *
 * AUTHOR:
 *    Thomas Lavergne, met.no, 11.12.2009
 *
 * MODIFIED:
 *    TL, met.no, 06.10.2010   :   Add code for the ::*<type>* query
 *    TL, met.no, 07.10.2010   :   Split in a wrapper around a va_list for
 *                                    use from nc_file_read().
 *                                    see http://www.c-faq.com/varargs/handoff.html
 *
 */ 
int rsprod_file_accessContent(rsprod_file *this, char *qstring,...) {
   /* start the VA engine and start interpreting the tokens */
   va_list ap;
   va_start(ap,qstring);
   /* call the v_ subroutine */
   int  v_rsprod_file_accessContent(rsprod_file *this, char *qstring,va_list);
   int ret = v_rsprod_file_accessContent(this,qstring,ap);
   /* close/release the VA engine */
   va_end(ap);
   return ret;
}
int v_rsprod_file_accessContent(rsprod_file *this, char *qstring, va_list ap) {

   int retcode = 0; /* return value if everythink ok */

   /* tokenize the query string */
   char   **qTokens;
   size_t nbQtokens;
   tokenize_query_string(qstring,&qTokens,&nbQtokens);

   for ( size_t tok = 0 ; tok < nbQtokens ; tok++ ) {
      /* translate the token */
      int  isGbAttr,isData, isDims;
      char *attrName,*fieldName;
      int  query;
      rsprod_type type;
      long int elem;
      if (!decode_qtoken(qTokens[tok],&isGbAttr,&attrName,&fieldName,&isData,&isDims,&query,&type,&elem)) {
         fprintf(stderr,"ERROR (%s) cannot decode token <%s>.\n",__func__,qTokens[tok]);
         retcode = 1;
         continue; 
      }
      /* search for the data we want to access to */
      if (!isData) {
         /* search for an attribute, but in which list? */
         rsprod_attributes *listattr;
         if (isGbAttr) {
            /* global attribute */
            listattr = this->glob_attr;
            rsprod_echo(stderr,"VERBOSE (%s) Search for %s in the list of Global Attributes\n",__func__,attrName);
         } else {
            rsprod_field *field;
            /* attribute of a field */
            if (rsprod_file_getDataset(this,&field,fieldName)) {
               fprintf(stderr,"ERROR (%s) <%s> no such field (token is <%s>).\n",__func__,fieldName,qTokens[tok]);
               retcode = 1;
               continue;
            }
            listattr = field->attr;
            rsprod_echo(stderr,"VERBOSE (%s) Search for %s in the attributes of %s\n",__func__,attrName,field->name);
         }
         rsprod_attr *attr;
         /* access the attribute in the list of global attributes */
         if (rsprod_attributes_getAttr(listattr,&attr,attrName)) {
            fprintf(stderr,"ERROR (%s) <%s> no such attribute (token is <%s>).\n",__func__,attrName,qTokens[tok]);
            retcode = 1;
            continue;
         }
         rsprod_echo(stderr,"VERBOSE (%s) name is %s and query code is %d\n",__func__,attrName,query);
         switch (query) {
            case RSPROD_QUERY_VAL:
                {
                   rsprod_echo(stderr,"VERBOSE (%s:%d:) RSPROD_QUERY_VAL on attribute %s.\n",__func__,__LINE__,attrName);
                   /* Query of the value. Depends on data type */
                   rsprod_type dtype = rsprod_attr_getType(attr);
                   if (dtype != type) {
                      fprintf(stderr,"ERROR (%s) <%s> type mismatch between file (%s) and query <%s>.\n",
                            __func__,attrName,TypeName[dtype],TypeName[type]);
                      retcode = 1;
                      continue;
                   }
                   rsprod_echo(stderr,"VERBOSE (%s:%d:) Type OK (%s).\n",__func__,__LINE__,TypeName[type]);
                   /* access the data. Different case whether we access a single element or copy all of them (*<type>* query) */
                   if (elem == RSPROD_QUERY_DEFAULTELEMS) { /* the 'elem' information was not provided in the qtoken */
                      /* default value: if the attribute is a scalar (1 value), then default to '0' (access first value).
                       * Otherwise, default to '*' (copy over all values).*/
                      size_t n_elems_in_attr = rsprod_attr_getNbvalues(attr);
                      if (n_elems_in_attr > 1) 
                         elem = RSPROD_QUERY_COPYALLELEMS;
                      else
                         elem = 0;
                   }
                   if (elem != RSPROD_QUERY_COPYALLELEMS) {
                      int aret;
                      switch (dtype) {
                         //case RSPROD_INT: {Int *vap = va_arg(ap,Int *);rsprod_attr_accessValue_Int(attr,0,vap);break;}
#define SwitchCase(t,T,c) case RSPROD_ ## T : {t *vap = va_arg(ap,t *); aret=rsprod_attr_accessValue_ ## t (attr,c,vap); break;}
                            SwitchCase(Double,DOUBLE,elem)
                            SwitchCase(Float,FLOAT,elem)
                            SwitchCase(Short,SHORT,elem)
                            SwitchCase(Int,INT,elem)
                            SwitchCase(Char,CHAR,elem)
                            SwitchCase(Byte,BYTE,elem)
#undef  SwitchCase
                         default:
                            fprintf(stderr,"ERROR (%s) Non valid type for attribute %s!\n",__func__,attrName);
                            retcode = 1;
                            continue;
                      }
                      if (aret) {
                         fprintf(stderr,"ERROR (%s) Cannot access value of %s at index %ld!\n",__func__,attrName,elem);
                         retcode = 1;
                         continue;
                      }
                   } else {
                      /* we want to: 
                       *    1) allocate new memory, 
                       *    2) copy over the data, 
                       *    3) make the va_arg pointer to the new memory 
                       * point 1) and 2) above are already implemented in rsprod_attr_getVoidStar(data,&p)
                       */
                      int aret;
                      switch (dtype) {
#define SwitchCase(t,T) case RSPROD_ ## T : {t **vap = va_arg(ap,t **); aret=rsprod_attr_getVoidStar(attr,(void **)vap); break;}
                            SwitchCase(Double,DOUBLE)
                            SwitchCase(Float,FLOAT)
                            SwitchCase(Short,SHORT)
                            SwitchCase(Int,INT)
                            SwitchCase(Char,CHAR)
                            SwitchCase(Byte,BYTE)
#undef  SwitchCase
                         default:
                            fprintf(stderr,"ERROR (%s) Non valid type for attribute %s!\n",__func__,attrName);
                            retcode = 1;
                            continue;
                      }
                      if (aret) {
                         fprintf(stderr,"ERROR (%s) Cannot copy all values of %s!\n",__func__,attrName);
                         retcode = 1;
                         continue;
                      }
                   }
                   break;
                }
            case RSPROD_QUERY_NUM:
                {
                   /* Query of the number of elements */
                   size_t *vap = va_arg(ap,size_t *);
                   *vap = rsprod_attr_getNbvalues(attr);
                }
                break;
            case RSPROD_QUERY_TYP:
                {
                   /* Query of the data type */
                   rsprod_type *vap = va_arg(ap,rsprod_type *);
                   *vap = rsprod_attr_getType(attr);
                }
                break;
            case RSPROD_QUERY_ADD:
                {
                   /* Query of the address of the data (pointer to start of data). Depends on data type */
                   rsprod_type dtype = rsprod_attr_getType(attr);
                   if (dtype != type) {
                      fprintf(stderr,"ERROR (%s) <%s> type mismatch between data and query <%s>.\n",__func__,attrName,qTokens[tok]);
                      retcode = 1;
                      continue;
                   }
                   switch (dtype) {
                      //case RSPROD_INT: {Int **vap = va_arg(ap,Int **);rsprod_attr_accessValues_Int(attr,vap);break;}
#define SwitchCase(t,T) case RSPROD_ ## T : {t **vap = va_arg(ap,t **); rsprod_attr_accessValues_ ## t (attr,vap); break;}
                      SwitchCase(Double,DOUBLE)
                      SwitchCase(Float,FLOAT)
                      SwitchCase(Short,SHORT)
                      SwitchCase(Int,INT)
                      SwitchCase(Char,CHAR)
                      SwitchCase(Byte,BYTE)
#undef  SwitchCase
                      default:
                          fprintf(stderr,"ERROR (%s) Non valid type for attribute %s!\n",__func__,attrName);
                          retcode = 1;
                          continue;
                   }
                   break;
                }
            case RSPROD_QUERY_OBJ:
                {
                   /* Query of the internal object */
                   rsprod_attr **vap = va_arg(ap,rsprod_attr **);
                   *vap = attr;
                }
                break;
            default:
                fprintf(stderr,"ERROR (%s) Only '*','&','#',T' and 'O' queries are implemented for attributes.\n",__func__);
                retcode = 1;
                continue;
         }
      } else {
         rsprod_field *field;
         /* access the field in the list of datasets */
         if (rsprod_file_getDataset(this,&field,fieldName)) {
            fprintf(stderr,"ERROR (%s) <%s> no such field (token is <%s>).\n",__func__,fieldName,qTokens[tok]);
            retcode = 1;
            continue;
         }
         rsprod_echo(stderr,"VERBOSE (%s) name is %s and query code is %d\n",__func__,field->name,query);
         if (!isDims) { /* query is on the dataset itself. (nor its attributes, nor its dimensions) */
            rsprod_echo(stderr,"VERBOSE (%s) Perform the query for the dataset itself (not attr, not dims).\n",__func__);
            switch (query) {
               case RSPROD_QUERY_VAL:
                   {
                      rsprod_echo(stderr,"VERBOSE (%s:%d:) RSPROD_QUERY_VAL on dataset %s.\n",__func__,__LINE__,field->name);
                      /* Query of the value. Depends on data type */
                      rsprod_type dtype = rsprod_data_getType(field->data);
                      if (dtype != type) {
                         fprintf(stderr,"ERROR (%s) <%s> type mismatch between data in file (%s) and query (%s).\n",
                               __func__,field->name,TypeName[dtype],TypeName[type]);
                         retcode = 1;
                         continue;
                      }
                      rsprod_echo(stderr,"VERBOSE (%s:%d:) TYPE OK (%s).\n",__func__,__LINE__,TypeName[dtype]);
                      if (elem == RSPROD_QUERY_DEFAULTELEMS) { /* the 'elem' information was not provided in the qtoken */
                         /* default value: if the field is a scalar (1 value), then default to '0' (access first value).
                          * Otherwise, default to '*' (copy over all values).*/
                         size_t n_elems_in_data = rsprod_data_getNbvalues(field->data);
                         if (n_elems_in_data > 1) 
                            elem = RSPROD_QUERY_COPYALLELEMS;
                         else
                            elem = 0;
                      }
                      /* access the data. Different case whether we access a single element or copy all of them (*<type>* query) */
                      if (elem != RSPROD_QUERY_COPYALLELEMS) {
                         int aret;
                         switch (dtype) {
#define SwitchCase(t,T,c) case RSPROD_ ## T : {t *vap = va_arg(ap,t *); aret=(*field->data->methods->accessValue)(field->data,c,vap); break;}
                               SwitchCase(Double,DOUBLE,elem)
                               SwitchCase(Float,FLOAT,elem)
                               SwitchCase(Short,SHORT,elem)
                               SwitchCase(Int,INT,elem)
                               SwitchCase(Char,CHAR,elem)
                               SwitchCase(Byte,BYTE,elem)
#undef  SwitchCase
                            default:
                               fprintf(stderr,"ERROR (%s) Non valid type for dataset %s!\n",__func__,fieldName);
                               retcode = 1;
                               continue;
                         }
                         if (aret) {
                            fprintf(stderr,"ERROR (%s) Cannot access value of %s at index %ld!\n",__func__,fieldName,elem);
                            retcode = 1;
                            continue;
                         }
                      } else {
                         /* we want to: 
                          *    1) allocate new memory, 
                          *    2) copy over the data, 
                          *    3) make the va_arg pointer to the new memory 
                          * point 1) and 2) above are already implemented in rsprod_data_getVoidStar(data,&p)
                          */
                         int aret;
                         switch (dtype) {
#define SwitchCase(t,T) case RSPROD_ ## T : {t **vap = va_arg(ap,t **); aret=rsprod_data_getVoidStar(field->data,(void **)vap); break;}
                               SwitchCase(Double,DOUBLE)
                               SwitchCase(Float,FLOAT)
                               SwitchCase(Short,SHORT)
                               SwitchCase(Int,INT)
                               SwitchCase(Char,CHAR)
                               SwitchCase(Byte,BYTE)
#undef  SwitchCase
                            default:
                               fprintf(stderr,"ERROR (%s) Non valid type for for dataset %s!\n",__func__,fieldName);
                               retcode = 1;
                               continue;
                         }
                         if (aret) {
                            fprintf(stderr,"ERROR (%s) Cannot copy all values of %s!\n",__func__,fieldName);
                            retcode = 1;
                            continue;
                         }
                      }
                      break;
                   }
               case RSPROD_QUERY_NUM:
                   {
                      /* Query of the number of elements */
                      size_t *vap = va_arg(ap,size_t *);
                      *vap = rsprod_data_getNbvalues(field->data);
                   }
                   break;
               case RSPROD_QUERY_TYP:
                   {
                      /* Query of the data type */
                      rsprod_type *vap = va_arg(ap,rsprod_type *);
                      *vap = rsprod_data_getType(field->data);
                   }
                   break;
               case RSPROD_QUERY_ADD:
                   {
                      /* Query of the address of the data (pointer to start of data). Depends on data type */
                      rsprod_type dtype = rsprod_data_getType(field->data);
                      if (dtype != type) {
                         fprintf(stderr,"ERROR (%s) <%s> type mismatch between data and query <%s>.\n",__func__,attrName,qTokens[tok]);
                         retcode = 1;
                         continue;
                      }
                      switch (dtype) {
#define SwitchCase(t,T) case RSPROD_ ## T : {t **vap = va_arg(ap,t **); (*field->data->methods->accessValues)(field->data,vap); break;}
                         SwitchCase(Double,DOUBLE)
                         SwitchCase(Float,FLOAT)
                         SwitchCase(Short,SHORT)
                         SwitchCase(Int,INT)
                         SwitchCase(Char,CHAR)
                         SwitchCase(Byte,BYTE)
#undef  SwitchCase
                         default:
                             fprintf(stderr,"ERROR (%s) Non valid type for dataset %s!\n",__func__,fieldName);
                             retcode = 1;
                             continue;
                      }
                      break;
                   }
               case RSPROD_QUERY_OBJ:
                   {
                      /* Query of the internal object */
                      rsprod_data **vap = va_arg(ap,rsprod_data **);
                      *vap = field->data;
                   }
                   break;
               default:
                   fprintf(stderr,"ERROR (%s) Only '*','&','#' and 'T' queries are implemented for datasets.\n",__func__);
                   retcode = 1;
                   continue;
            }
         } else { /* query is on the dimensions of a dataset */
            rsprod_echo(stderr,"VERBOSE (%s) Perform the query for the dimensions of %s\n",__func__,fieldName);
            switch (query) {
               case RSPROD_QUERY_VAL:
                   {
                      /* Query of the value. Returns dims->length[elem] (always an unsigned int) */
                      if (elem >= (long int)field->dims->nbdims) {
                         fprintf(stderr,"ERROR (%s) Field %s have only %u dimensions!\n",__func__,fieldName,field->dims->nbdims);
                         retcode = 1;
                         continue;
                      }
                      unsigned int *vap = va_arg(ap,unsigned int *);
                      *vap = field->dims->length[elem];
                      break;
                   }
               case RSPROD_QUERY_NUM:
                   {
                      /* Query of the number of elements (total number of dimensions) */
                      unsigned short *vap = va_arg(ap,unsigned short *);
                      *vap = field->dims->nbdims;
                   }
                   break;
               case RSPROD_QUERY_ADD:
                   {
                      /* Query of the address to the 'dims->length[]' array. Is unsigned int. */
                      unsigned int **vap = va_arg(ap,unsigned int **);
                      *vap = field->dims->length;
                      break;
                   }
               case RSPROD_QUERY_OBJ:
                   {
                      /* Query of the internal object */
                      rsprod_dimensions **vap = va_arg(ap,rsprod_dimensions **);
                      *vap = field->dims;
                   }
                   break;
               case RSPROD_QUERY_TYP: /* does not make any sense for dimensions */
               default:
                   fprintf(stderr,"ERROR (%s) Only '*','&','#' and 'O' queries are implemented for datasets.\n",__func__);
                   retcode = 1;
                   continue;
            }
         }
      }
   }

   
   return (retcode);
}

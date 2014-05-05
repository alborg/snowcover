/*
 * NAME: query_string.c
 * 
 * PURPOSE: 
 *    Parse the query strings and transform into requests to the rsprod objects 
 *
 * DESCRIPTION:
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 11.12.2009
 *
 * MODIFIED:
 *    TL, met.no, 16.12.2009   :   Add the query for dimensions
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rsprod_types.h"
#include "rsprod_intern.h"
#include "rsprod_memory_utils.h"
#include "rsprod_string_utils.h"
#include "rsprod_report_utils.h"
#include "query_string.h"


void tokenize_query_string(char *qstring,char ***qtokens,size_t *nbQtokens) {
   rsprod_string_split(qstring,RSPROD_QSTRNG_SEP,qtokens,nbQtokens);
}

static int string_is_underscore (char *str) {
   return !strcmp(str,"_");
}

int get_field_names_from_query_string(char *qstring, char ***fieldnames, size_t *nbFieldnames) {
   int ret;

   /* get an arry of all the query tokens */
   char **all_fieldnames;
   size_t nbAllFieldnames;
   tokenize_query_string(qstring,&all_fieldnames,&nbAllFieldnames);
   /* go through each token, keep only the field names and store the strings in a list */
   dbl_list *list_fieldnames = dbl_list_init(sizeof(dbl_list_string));
   for (size_t t = 0 ; t < nbAllFieldnames ; t++) {
      char *qtoken = all_fieldnames[t];
      char   **toks;
      size_t nbToks;
      rsprod_string_split(qtoken,RSPROD_QTOKEN_SEP,&toks,&nbToks);
      if (nbToks != 3) {
         fprintf(stderr,"ERROR (%s) Cannot find two mandatory '%c' delimiters in token <%s>\n",__func__,RSPROD_QTOKEN_SEP,qtoken);
         return 0;
      }
      if (strlen(toks[0]) == 0) {
         qtoken[0] = '_';qtoken[1]='\0'; /* use a special character for global attibutes */
      } else {
         qtoken[strlen(toks[0])]='\0';
      }
      rsprod_echo(stderr,"In %s: QTOK[%d] is now <%s>\n",__func__,t,qtoken);
      dbl_list_string oneFieldName;
      ret = setString(&oneFieldName,qtoken);
      ret = dbl_list_addNode(list_fieldnames,dbl_node_createCopyContent(list_fieldnames,&oneFieldName,copyString),LIST_POSITION_LAST);
      deleteString(&oneFieldName);
   }
   for (size_t e = 0 ; e < nbAllFieldnames ; e++)
      free(all_fieldnames[e]);
   free(all_fieldnames);
   /* uniq the list */
   if (dbl_list_uniq(list_fieldnames,&Strings_areSame,NULL) < 0) {
      fprintf(stderr,"ERROR (%s) Problem uniq'ing the list of field names.\n",__func__);
      return 0;
   }
   /* remove the '_' elements */
   sprintf(dbl_list_stringis_param,"_");
   int nb_del = dbl_list_removeAllThose(list_fieldnames,NULL,&StringIs);
   /* transform the list back into an array for return from function, release memory */
   size_t array_fieldnames_nbelems;
   char **array_fieldnames = dbl_list_list2array(list_fieldnames,NULL,&array_fieldnames_nbelems);
   if (!array_fieldnames) {
      fprintf(stderr,"ERROR (%s) Problem in obtaining the final array of field names.\n",__func__);
      return 0;
   }
   dbl_list_delete(list_fieldnames,NULL);
   /* map return parameters */
   *fieldnames   = rsprodMalloc(array_fieldnames_nbelems*sizeof(char *));
   for (size_t f = 0 ; f < array_fieldnames_nbelems ; f++) {
      (*fieldnames)[f] = rsprodMalloc(strlen(array_fieldnames[f])+1);
      sprintf((*fieldnames)[f],"%s",array_fieldnames[f]);
   }
   *nbFieldnames = array_fieldnames_nbelems;

   return 1;
}

/* the format of individual token is: <field>:<attr>:<what>[<type>][<elem>], where:
 * <field> is the name of the dataset   
 *            If not given ('') user wants to access a global attribute.
 * <attr>  is the name of the attribute 
 *            If not given (''), user wants to access the data. 
 *            If '[]', user wants to access the dimensions)
 * <what>  can be '*' (star) or '&' (ampersand) or '#' (sharp) or 'T':
 *    '*' means we want to access the value; This is the default.
 *    '&' means we want to access the address;
 *    '#' means we want to access the number of elements;
 *    'T' means we want to access the data type;
 *    'O' means we want to access the 'rsprod internal Object' (for advanced users).
 * <type>  (mandatory for '*' and '&', ignored for others): expected type of the data.
 *         -> 'f' for float, 'd' for double, 'i' for int, 'c' for char, 's' for short and 'b' for byte
 * <elem>  (optional for '*') index of the element we want to access (** 0-indexed notation**).
 *            by default, '0' is used (first element). If '*' (star) is used (::*d*) then
 *            it instructs the library to allocate new memory and copy all elements.
 *
 * The 2 ':' are mandatory but can be configured (see below)
 *
 * Note : 
 *    o  All characters can be configured in the associated header file.
 *
 * TODO : fix memory leak with character strings
 * TODO : secure the atol() with strtol()
 */
int decode_qtoken(char *qtoken, int *isGlobalAttribute, char **attrName, char **fieldName, int *isData, int *isDims,
      int *what, rsprod_type *type,long int *elem) {
   
   rsprod_echo(stderr,"VERBOSE (%s) with token <%s>\n",__func__,qtoken);
   char   **toks;
   size_t nbToks;
   rsprod_string_split(qtoken,RSPROD_QTOKEN_SEP,&toks,&nbToks);
   if (nbToks != 3) {
      fprintf(stderr,"ERROR (%s) Cannot find two mandatory '%c' delimiters in token <%s>\n",__func__,RSPROD_QTOKEN_SEP,qtoken);
      return 0;
   }
   if (strlen(toks[0]) == 0) {
      *isGlobalAttribute = 1; 
      rsprod_echo(stderr,"VERBOSE (%s) Token <%s> is for a Global Attribute.\n",__func__,qtoken);
   } else {
      *isGlobalAttribute = 0;
      *fieldName = toks[0];
   }
   if ( (*isGlobalAttribute) && (strlen(toks[1]) == 0) ) {
      fprintf(stderr,"ERROR (%s) Since token <%s> corresponds to an attribute, its name must be given after the first '%c' character.\n",
            __func__,qtoken,RSPROD_QTOKEN_SEP);
      return 0;
   } else if ( strlen(toks[1]) == 0 ) {
      *isData = 1;
      *isDims = 0;
   } else if ( !strcmp(toks[1],RSPROD_QTOKEN_DIMS) ) {
      *isData = 1;
      *isDims = 1;
   } else {
      *isData   = 0;
      *isDims   = 0;
      *attrName = toks[1];
   }
   if (strlen(toks[2]) == 0) {
      fprintf(stderr,"ERROR (%s) Token <%s> misses a <query> information, following the second '%c' character.\n",
            __func__,qtoken,RSPROD_QTOKEN_SEP);
      return 0;
   }
   char cquery = (toks[2][0]); /* first char */
   rsprod_echo(stderr,"VERBOSE (%s) First character is '%c'\n",__func__,cquery);
   int have_query_info = 1;
   switch (cquery) {
      case RSPROD_QTOKEN_QVAL: 
         *what = RSPROD_QUERY_VAL;break;
      case RSPROD_QTOKEN_QADD:
         *what = RSPROD_QUERY_ADD;break;
      case RSPROD_QTOKEN_QNUM:
         *what = RSPROD_QUERY_NUM;break;
      case RSPROD_QTOKEN_QTYP:
         *what = RSPROD_QUERY_TYP;break;
      default:
         have_query_info = 0;
         rsprod_echo(stderr,"VERBOSE (%s) Did not find a query character, use '%c' as default.\n",__func__,RSPROD_QTOKEN_QVAL);
         *what = RSPROD_QUERY_VAL;break;
   }
   rsprod_echo(stderr,"VERBOSE (%s) query code is %d\n",__func__,*what);

   /* for the '*' and '&' queries, the second character is coding the expected type. (but not if its a dimension) */
   short have_type_info;
   if ( !(*isDims) && ((*what == RSPROD_QUERY_VAL) || (*what == RSPROD_QUERY_ADD))) {
      have_type_info = 1;
      /*
      if (strlen(toks[2]) == 1) {
         fprintf(stderr,"ERROR (%s) Token <%s> misses a <type> information, following the '%c' character.\n",
            __func__,qtoken,cquery);
         return 0;
      }
      */
      char ctype = (toks[2][have_query_info]); /* second char (unless the query action was missing)*/
      *type = qTypeToken(ctype);
      if (*type == RSPROD_NAT) {
         fprintf(stderr,"ERROR (%s) Token <%s> has invalid type information (%c) following the '%c' character.\n",
               __func__,qtoken,ctype,cquery);
         return 0;
      }
      rsprod_echo(stderr,"VERBOSE (%s) Type for the request is %s\n",__func__,TypeName[*type]);
   } else {
      have_type_info = 0;
      *type = RSPROD_NAT;
   }

   /* for the '*' query, have the last digits of the query string to hold the optional element we wish to access in the array */
   if (*what == RSPROD_QUERY_VAL) {
      if (strlen(toks[2]) > (have_query_info+have_type_info)) {
         char *sindex = toks[2]+have_query_info+have_type_info;
         if (!strcmp(sindex,"*")) {
            rsprod_echo(stderr,"VERBOSE (%s) Token <%s> specifies that all elements should be copied (%s)\n",
                  __func__,qtoken,sindex);
            *elem = RSPROD_QUERY_COPYALLELEMS;
         } else {
            rsprod_echo(stderr,"VERBOSE (%s) Token <%s> specifies optional element index (%s)\n",__func__,qtoken,sindex);
            *elem = atol(sindex);
         }
      } else {
         /* We are in a '*' query, but there is no final character to indicate which elem(s) to access. */
         rsprod_echo(stderr,"VERBOSE (%s) Token <%s> does NOT specify optional element index, use default.\n",__func__,qtoken);
         *elem = RSPROD_QUERY_DEFAULTELEMS;
      }
   } else {
      *elem = 0l; /* default value for other cases */
   }

   
   return 1;
}

   

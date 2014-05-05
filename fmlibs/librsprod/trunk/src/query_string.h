#ifndef QUERY_STRING_H
#define QUERY_STRING_H

#define RSPROD_QSTRNG_SEP ' '
#define RSPROD_QTOKEN_SEP ':'

#define RSPROD_QTOKEN_DIMS "[]" /* not part of the numbered 'queries' (*,&,#,T) */

#define RSPROD_QTOKEN_QVAL '*'
#define RSPROD_QTOKEN_QADD '&'
#define RSPROD_QTOKEN_QNUM '#'
#define RSPROD_QTOKEN_QTYP 'T'
#define RSPROD_QTOKEN_QOBJ 'O'

#define RSPROD_QUERY_VAL 0
#define RSPROD_QUERY_ADD 1
#define RSPROD_QUERY_NUM 2
#define RSPROD_QUERY_TYP 3
#define RSPROD_QUERY_OBJ 4

#define RSPROD_QUERY_COPYALLELEMS -1 
#define RSPROD_QUERY_DEFAULTELEMS -2 

void tokenize_query_string(char *qstring,char ***qtokens,size_t *nbQtokens);
int get_field_names_from_query_string(char *qstring, char ***fieldnames, size_t *nbFieldnames);
int decode_qtoken(char *qtoken, int *isGlobalAttribute, char **attrName, char **fieldName, int *isData, int *isDim,
      int *query, rsprod_type *type,long int *elem);


#endif /* QUERY_STRING_H */



#ifndef DBL_LIST_H 
#define DBL_LIST_H 

/* Control for the level of verbosity of the library */
#define LIBLIST_QUIET   0
#define LIBLIST_VERBOSE 1
extern int liblist_running_mode;
#define ifverbose if (liblist_running_mode == LIBLIST_VERBOSE)

/* control for where to add a new node in the list */
#define LIST_POSITION_LAST    0
#define LIST_POSITION_FIRST   1

/* control for what to do when aggregating several return codes : keep the MAX or MIN */
#define LIST_CHECK_KEEPMAX    0
#define LIST_CHECK_KEEPMIN    1
#define LIST_CHECK_ERRMAX     99999
#define LIST_CHECK_ERRMIN     0

typedef void (*doitFunc)     (/* elem */);
typedef int  (*chkitFunc)    (/* elem */);
typedef int  (*copyFunc)     (/* dest, src */);
typedef void (*printFunc)    (/* elem */);
typedef void (*deleteFunc)   (/* elem */);
typedef int  (*areEqualsFunc)(/* elem1, elem2 */);
typedef int  (*condFunc)     (/* elem */);
typedef int  (*rankingFunc)  (/* elem1, elem2 */);

typedef struct dbl_node {
   void   *c;
   struct dbl_node *n;
   struct dbl_node *p;
} dbl_node;

typedef struct dbl_list {
   size_t   nbnodes;
   size_t   sizeofc;
   dbl_node *head;
} dbl_list;

/* nodes */
dbl_node *dbl_node_create(void);
dbl_node *dbl_node_createAssignContent(void *content);
dbl_node *dbl_node_createCopyContent(dbl_list *l,void *content,copyFunc);

/* list */
dbl_list *dbl_list_init(size_t);
int dbl_list_addNode(dbl_list *l,dbl_node *n,short pos);
int dbl_list_removeNode(dbl_list *l,dbl_node *nd,deleteFunc);
dbl_node *dbl_list_findIf(dbl_list *l,dbl_node *start,condFunc condition);
dbl_node *dbl_list_find(dbl_list *l,dbl_node *start,areEqualsFunc,void *search);
int dbl_list_browse(dbl_list *l,doitFunc);
int dbl_list_print(dbl_list *l,doitFunc);
int dbl_list_browseAndCheck(dbl_list *l,chkitFunc,short action_on_returns);
int dbl_list_delete(dbl_list *l,deleteFunc);
int dbl_list_sort(dbl_list *l,rankingFunc,copyFunc,deleteFunc);
dbl_list *dbl_list_join(dbl_list *l1,dbl_list *l2);
dbl_list *dbl_list_copy(dbl_list *from,copyFunc);
int dbl_list_removeAllThose(dbl_list *l,deleteFunc,condFunc);
int dbl_list_uniq(dbl_list *l,areEqualsFunc, deleteFunc);
void *dbl_list_list2array(dbl_list *l,copyFunc copy,size_t *nbelems);
dbl_list *dbl_list_array2list(void *start_of_array,size_t nbelems, size_t sizeofc, copyFunc copy);

/* strings */
typedef struct dbl_list_string {
   char *str;
} dbl_list_string;
void printString(dbl_list_string *dbl_list_string);
int copyString(dbl_list_string *S1,dbl_list_string *S2);
int setString(dbl_list_string *S,char *str);
void deleteString(dbl_list_string *S);
int Strings_areSame(dbl_list_string *a, dbl_list_string *b);
int StringIs(dbl_list_string *str);

extern char dbl_list_stringis_param[];

#endif /* DBL_LIST_H */


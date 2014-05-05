

#ifndef DBL_LIST_H 
#define DBL_LIST_H 

#define LIBLIST_QUIET   0
#define LIBLIST_VERBOSE 1
extern int liblist_running_mode;
#define ifverbose if (liblist_running_mode == LIBLIST_VERBOSE)

#define LIST_POSITION_LAST    3

typedef void (*doitFunc)     (/* elem */);
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
int dbl_list_delete(dbl_list *l,deleteFunc);
int dbl_list_sort(dbl_list *l,rankingFunc,copyFunc,deleteFunc);
dbl_list *dbl_list_join(dbl_list *l1,dbl_list *l2);
dbl_list *dbl_list_copy(dbl_list *from,copyFunc);
int dbl_list_removeAllThose(dbl_list *l,deleteFunc,condFunc);
int dbl_list_uniq(dbl_list *l,areEqualsFunc, deleteFunc);

#endif /* DBL_LIST_H */


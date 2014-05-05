

#include <stdlib.h>
#include <stdio.h>
#include "rsprod_memory_utils.h"


void *rsprodMalloc(size_t size) {
   void *res;
   res = malloc(size);
   if (!res) {
      fprintf(stderr,"ERROR (rsprodMalloc) Memory allocation problem.\n");
      exit(1);
   }
   return(res);
}

void *rsprodRealloc(void *ptr, size_t size) {
   void *res;
   res = realloc(ptr,size);
   if (!res) {
      fprintf(stderr,"ERROR (rsprodRealloc) Memory allocation problem.\n");
      exit(1);
   }
   return(res);
}

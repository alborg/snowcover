
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rsprod_intern.h"
#include "rsprod_memory_utils.h"
#include "rsprod_string_utils.h"

int rsprod_dims_create(rsprod_dimensions **this, 
      const unsigned short nbdims, 
      char          **dimsName, 
      unsigned int   *dimsLength,
      short          *dimsOrder) {

   rsprod_dimensions *tmp = rsprodMalloc(sizeof(rsprod_dimensions));
   *this = tmp;

   tmp->name     = rsprodMalloc(nbdims*sizeof(char *));
   tmp->length   = rsprodMalloc(nbdims*sizeof(unsigned int));
   tmp->rank     = rsprodMalloc(nbdims*sizeof(unsigned short));
   tmp->order    = rsprodMalloc(nbdims*sizeof(short));
   tmp->nbdims   = nbdims;
   tmp->totelems = 0lu;
  
   /* by default, the dimensions are sorted by rank */
   for (unsigned short i=0; i < tmp->nbdims; i++) {
      tmp->rank[i] = i;
   }

   int ret = 0;
   ret += rsprod_dims_setNames  (tmp,nbdims,dimsName);
   ret += rsprod_dims_setLengths(tmp,nbdims,dimsLength);
   ret += rsprod_dims_setOrders (tmp,nbdims,dimsOrder);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_dims_create) cannot set values for dimensions.\n");
      return 1;
   }

   return 0;
}

void rsprod_dims_delete(rsprod_dimensions *this) {

   if (!this) return;

   for (unsigned short i=0; i < this->nbdims; i++) {
      free(this->name[i]);
   }
   free(this->name);
   free(this->length);
   free(this->order);
   free(this->rank);

}

int rsprod_dims_addOne(rsprod_dimensions *this, 
      short           position,
      char           *dimName, 
      unsigned int    dimLength,
      short           dimOrder) {
   
   if (!this) {
      fprintf(stderr,"ERROR (rsprod_dims_addOne) Cannot add a dimension to a NULL dimension object.\n");
      return 1;
   }

   size_t nbdims  = this->nbdims;
  
   /* Test that the position we want to insert our dimension is valid. */
   /* -1 is a short-cut to 'last element' (append) */
   if (position == -1)
      position = nbdims;
   if ( (position < 0) || (position > nbdims) ) {
      fprintf(stderr,"ERROR (rsprod_dims_addOne) Wrong index to insert a dimension. Valid values are -1 and [0.,%d]\n",nbdims);
      return 1;
   }

   /* realloc the memory */
   this->name     = rsprodRealloc(this->name,(nbdims+1)*sizeof(char *));
   this->length   = rsprodRealloc(this->length,(nbdims+1)*sizeof(unsigned int));
   this->rank     = rsprodRealloc(this->rank,(nbdims+1)*sizeof(unsigned short));
   this->order    = rsprodRealloc(this->order,(nbdims+1)*sizeof(short));
   this->nbdims  += 1;

   for (size_t f = this->nbdims-1 ; f > position ; f--) {
      this->name[f]   = this->name[f-1];
      this->length[f] = this->length[f-1];
      this->order[f]  = this->order[f-1];
   }

   /* store the dimension at appropriate position */
   size_t e = position ;  

   /* new name */
   char *trname;
   if (rsprod_string_trim_allNullsExceptLast(dimName,&trname)) {
      fprintf(stderr,"ERROR (rsprod_dims_addOne) Problem dealing with null characters in name <%s>\n",dimName);
      return 1;
   }
   this->name[e] = trname;

   /* new length */
   if ( dimLength > 0 ) {
      this->length[e] = dimLength;
      this->totelems *= dimLength;
   } else {
      fprintf(stderr,"ERROR (rsprod_dims_addOne) dimensions cannot have 0 lengths.\n");
      return 1;
   }

   /* new order */
   if ( (dimOrder  == -1) || (dimOrder == +1) ) {
      this->order[e] = dimOrder;
   } else {
      fprintf(stderr,"ERROR (rsprod_dims_addOne) orders can only be -1 or +1.\n");
      return 1;
   }

   /* move the dimensions to create the necessary hole for storing the 
    * new dimension at position */
   for ( size_t f = 0 ; f < this->nbdims ; f++ ) {
      this->rank[f] = f;
   }

   return 0;
}

int rsprod_dims_setNames(rsprod_dimensions *this,const unsigned short nbNames, char *names[]) {

   if (this->nbdims != nbNames) {
      fprintf(stderr,"ERROR (rsprod_dims_setNames) number of strings and number of dimensions do not match (%u,%u).\n",
	    this->nbdims,nbNames);
      return 1;
   }
   for (unsigned short i = 0; i < this->nbdims; i++) {
      char *name;
      if (rsprod_string_trim_allNullsExceptLast(names[i],&name)) {
	 fprintf(stderr,"ERROR (rsprod_dims_setNames) Problem dealing with null characters in name <%s>\n",names[i]);
	 return 1;
      }
      this->name[this->rank[i]] = name;
   }

   return 0;
}

int rsprod_dims_getListNames(rsprod_dimensions *this, int startInd, char **list, char delim) {

   if ( (startInd < 0) || (startInd >= this->nbdims) ) {
      fprintf(stderr,"ERROR (rsprod_dims_getListNames) The startInd parameter (%d) should be in [%d,%d[\n",
	    startInd,0,this->nbdims);
      return 1;
   }

   size_t nbchars = 0;
   char   *clist;
   for (short pass = 0 ; pass <= 1 ; pass ++) {
      if (pass) {
	 nbchars += this->nbdims - 1 - startInd;
	 *list = rsprodMalloc(nbchars + 1);
	 memset(*list,delim,nbchars);(*list)[nbchars] = '\0';
	 clist = *list;
      }
      for (size_t d = startInd ; d < this->nbdims ; d++) {
	 if (pass) {
	    memcpy(clist,this->name[d],strlen(this->name[d]));
	    clist += strlen(this->name[d])+1;
	 }
	 nbchars += strlen(this->name[d]);
      }
   }

   return 0;

}

int rsprod_dims_setLengths(rsprod_dimensions *this,const unsigned short nbLengths, unsigned int *lengths) {

   if (this->nbdims != nbLengths) {
      fprintf(stderr,"ERROR (rsprod_dims_setLengths) number of provided lengths and number of dimensions do not match (%u,%u).\n",
	    this->nbdims,nbLengths);
      return 1;
   }
   for (unsigned short i = 0; i < this->nbdims; i++) {
      if ( lengths[i] > 0 ) {
	 this->length[this->rank[i]] = lengths[i];
         this->totelems = (this->totelems==0? lengths[i] : this->totelems * lengths[i]);
      } else {
	 fprintf(stderr,"ERROR (rsprod_dims_setLengths) dimensions cannot have 0 lengths.\n");
	 return 1;
      }
   }
   return 0;

}

int rsprod_dims_setOrders(rsprod_dimensions *this,const unsigned short nbOrders, short *orders) {

   if (this->nbdims != nbOrders) {
      fprintf(stderr,"ERROR (rsprod_dims_setOrders) number of provided orders and number of dimensions do not match (%u,%u).\n",
	    this->nbdims,nbOrders);
      return 1;
   }
   for (unsigned short i = 0; i < this->nbdims; i++) {
      if ( (orders[i]  == -1) || (orders[i] == +1) ) {
	 this->order[this->rank[i]] = orders[i];
      } else {
	 fprintf(stderr,"ERROR (rsprod_dims_setOrders) orders can only be -1 or +1.\n");
	 return 1;
      }
   }
   return 0;

}

rsprod_dimensions *rsprod_dims_copy(rsprod_dimensions *orig) {

   int ret;
   rsprod_dimensions *dest;

   if (!orig) return NULL;
   
   char **Names          = rsprodMalloc(orig->nbdims*sizeof(char *));
   unsigned int *Lengths = rsprodMalloc(orig->nbdims*sizeof(unsigned int)); 
   short *Orders = rsprodMalloc(orig->nbdims*sizeof(short)); 
   for (unsigned int i=0; i < orig->nbdims; i++) {
      /* process, trim and store the name */
      if (rsprod_string_trim_allNullsExceptLast((orig->name)[i],&(Names[i]))) {
	 fprintf(stderr,"ERROR (rsprod_dims_copy) Problem dealing with null characters in name <%s>\n",(orig->name)[i]);
	 return NULL;
      }
      /* store the length */
      Lengths[i] = (orig->length)[i];
      Orders[i] = (orig->order)[i];
   }

   /* now fill the rsprod_dimension object with this info */
   ret = rsprod_dims_create(&dest,orig->nbdims,Names,Lengths,Orders);
   if (ret) {
      fprintf(stderr,"ERROR (rsprod_dims_copy) Could not transfer the info.\n");
      return NULL;
   }

   for (unsigned int i=0; i < orig->nbdims; i++) 
      free(Names[i]);
   free(Names);
   free(Lengths);
   free(Orders);

   return dest;
}

/*
int rsprod_dims_sort(rsprod_dimensions *this,const unsigned short nbRanks, unsigned short *newranks) {

   if (this->nbdims != nbRanks) {
      fprintf(stderr,"ERROR (rsprod_dims_sort) number of provided ranks and number of dimensions do not match (%u,%u).\n",
	    this->nbdims,nbRanks);
      return 1;
   }
   unsigned short *usedims = rsprodMalloc(this->nbdims*sizeof(unsigned short));
   for (unsigned short i = 0; i < this->nbdims; i++) usedims[i]=0;

   for (unsigned short i = 0; i < this->nbdims; i++) {

      if ( newranks[i] >= this->nbdims ) {
	 fprintf(stderr,"ERROR (sortDimensions) rank %u is too large: only %u dimensions.\n",newranks[i],this->nbdims);
	 return 1;
      }
      if ( usedims[newranks[i]] > 0 ) {
	 fprintf(stderr,"ERROR (sortDimensions) rank %u was given twice.\n",newranks[i]);
	 return 1;
      }
      this->rank[i] = newranks[i];
      usedims[this->rank[i]]++;
   }
   free(usedims);
   return 0;

}
*/

void rsprod_dims_printInfo(rsprod_dimensions *this) {
   
   if (!this) {
      fprintf(stdout,"Dimension NULL\n");
   } else {
      fprintf(stdout,"Dimensions (%u)\n",this->nbdims);
      for (unsigned short i = 0; i < this->nbdims; i++) {
	 fprintf(stdout,"\t%02u (%02u) -> [%s, %u, %+1d]\n",i,this->rank[i],
	       this->name[this->rank[i]],this->length[this->rank[i]],this->order[this->rank[i]]);
      }
      fprintf(stdout,"\tTot elems is %lu\n",this->totelems);
   }
}

int rsprod_dims_compare(rsprod_dimensions *one,rsprod_dimensions *two) {
   int areLike = 1;
   if (!one && !two)
      return areLike;
   if (one && !two)
      return 0;
   if (!one && two)
      return 0;
   if (one->nbdims != two->nbdims) 
      return 0;
   if (one->totelems != two->totelems) 
      return 0;
   for (size_t d = 0; d < one->nbdims ; d++) {
      if (one->length[d] != two->length[d])
	 return 0;
      if (strcmp(one->name[d],two->name[d]))
	 return 0;
      if (one->rank[d] != two->rank[d])
	 return 0;
      if (one->order[d] != two->order[d])
	 return 0;
   }

   return areLike;
}

unsigned long vecIndex(rsprod_dimensions *this, unsigned short nbMatIndex, long *MatIndex) {

   unsigned long vecIndex = 0;

   unsigned long base = 1;
   for (unsigned short dim2 = (this->nbdims)-1; dim2 < this->nbdims ; dim2--) {
      unsigned short rank   = this->rank[dim2];
      unsigned long  length = this->length[rank];
      if ((MatIndex[rank] < 0) || (MatIndex[rank] >= length)) {
         //fprintf(stderr,"ERROR (vecIndex) Index %ld cannot be in dimension %u which has length %lu\n.",
	 //      MatIndex[rank],rank,length);
	 return (this->totelems);
      }
      vecIndex += MatIndex[rank] * base; 
      //fprintf(stdout,"\tvecIndex... dim2 is %u. MatIndex is %lu. Base is %lu. vecIndex is %lu\n",
	//    rank,MatIndex[rank],base,vecIndex);
      base *= length;
   }

   return vecIndex;

}

int matIndex(rsprod_dimensions *this, unsigned long VecIndex,long *MatIndex) {
  
   if (VecIndex >= this->totelems) {
      //fprintf(stderr,"ERROR (matIndex) Index %lu is larger than totelems (%lu).\n",VecIndex,this->totelems);
      return 1;
   }

   unsigned long base = this->totelems;
   for (unsigned short dim=0; dim < this->nbdims; dim++) {
   //   fprintf(stderr,"\tmatIndex.... dim is %d (l=%u).",dim,this->length[this->rank[dim]]);
      base /= this->length[this->rank[dim]];
     // fprintf(stderr," base is now %lu.",base);
      MatIndex[this->rank[dim]] = VecIndex / base;
      //fprintf(stderr,"\t%lu / %lu is %lu.\n", VecIndex, base, MatIndex[this->rank[dim]]);
      VecIndex -= MatIndex[this->rank[dim]]*base;
   }

   return 0;
}

unsigned long vecIndexRelative(rsprod_dimensions *this, unsigned long current, unsigned short nbMatIndex, long *MatIndex) {

   unsigned long vecInd;
   long *matIndexCurrent = rsprodMalloc(sizeof(long)*nbMatIndex);
   int mat = matIndex(this,current,matIndexCurrent);
   if (!mat) {
      for (int j=0; j < nbMatIndex; j++) {
	 matIndexCurrent[j]+=MatIndex[j];
      }
      vecInd = vecIndex(this,nbMatIndex,matIndexCurrent);
   } else {
      vecInd = this->totelems;
   }

   free(matIndexCurrent);
   return vecInd;
}


/*
int browseAllElems_mat(rsprod_dimensions *this) {
   
   unsigned long  *MatIndex = rsprodMalloc(this->nbdims*sizeof(unsigned long));

   unsigned long nbElems = 0;
   while ( nbElems < this->totelems ) {

      fprintf(stdout,"GET ADDRESS FOR ELEMENT: (%lu).\n",nbElems);
      int ret=matIndex(this,nbElems,MatIndex);
      if (ret) {
	 fprintf(stderr,"ERROR.\n");
	 return 1;
      }

      fprintf(stdout,"\tVERBOSE: (%lu) matIndex is [ ",nbElems);
      for (unsigned short dim=0; dim<this->nbdims; dim++) fprintf(stdout,"%lu ",MatIndex[this->rank[dim]]);
      fprintf(stdout," ]\n");

      unsigned long vI;
      vI = vecIndex(this, this->nbdims, MatIndex);
      if (vI == this->totelems) {
	 fprintf(stderr,"ERROR\n");
	 break;
      }
      fprintf(stdout,"\t\t==> (%lu);\n",vI);
       
      
      nbElems++;
      
   }
   fprintf(stdout,"\nVERBOSE: READ THROUGH %lu elements (%lu)\n",nbElems+1,this->totelems);

   free(MatIndex);
   return 0;
}
*/

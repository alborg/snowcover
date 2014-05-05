
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rsprod_report_utils.h"
#include "rsprod_memory_utils.h"
#include "rsprod_intern.h"
#include "runtime_config.h"

int         librsprod_echo_mode         = LIBRSPROD_QUIET;
int         librsprod_unpack_datasets   = 1; /* yes (0 = no) */ 
rsprod_type librsprod_unpack_to         = RSPROD_NAT;
int         librsprod_write_nullchar    = 1; /* 1 = yes, 0 = no */ 
char        librsprod_pad_strings_with  = '\0'; /* '\0' = no, any other char = pad string datasets with char */

void librsprod_dump_config(void) {

   fprintf(stderr,"\n");
   fprintf(stderr,"LIBRSPROD run time configuration:\n");
   fprintf(stderr,"VERBOSE  : %s\n",(librsprod_echo_mode == LIBRSPROD_QUIET?"no":"yes"));
   fprintf(stderr,"UNPACK DATASETS: %s\n",(librsprod_unpack_datasets?"no":"yes"));
   fprintf(stderr,"UNPACK TO: %s\n",(librsprod_unpack_to != RSPROD_NAT?TypeName[librsprod_unpack_to]:"N/A"));
   char *nclib_version_number = get_netCDF_version();
   fprintf(stderr,"NetCDF lib version: %s\n",nclib_version_number);
   free(nclib_version_number);
   fprintf(stderr,"\n");

}

void librsprod_init_fillvalue(rsprod_type t, void *fv) {
   if (!FillValues[t])
      FillValues[t] = rsprodMalloc(1*SizeOf[t]);
   memcpy(FillValues[t],fv,SizeOf[t]);
}

void librsprod_init_fillvalues(void) {
   rsprod_echo(stderr,"Entering %s\n",__func__);
#define init_fill_value(t,T) {t fv = RSPROD_FILLVAL_ ## T;librsprod_init_fillvalue(RSPROD_ ## T,&fv);}
   init_fill_value(Float,FLOAT);
   init_fill_value(Short,SHORT);
   init_fill_value(Char,CHAR);
   init_fill_value(Double,DOUBLE);
   init_fill_value(Int,INT);
   init_fill_value(Byte,BYTE);
#undef init_fill_value
   rsprod_echo(stderr,"Exiting %s\n",__func__);
}

void librsprod_init_unityvalues(void) {
   rsprod_echo(stderr,"Entering %s\n",__func__);
   for (short t = 0 ; t < RSPROD_NBTYPES ; t++) {
      UnityValues[t] = rsprodMalloc(1*SizeOf[t]);
   }
#define init_unity_value(t,T) {t fv = RSPROD_UNITY_ ## T;memcpy(UnityValues[RSPROD_ ## T],&fv,SizeOf[RSPROD_ ## T]);}
   init_unity_value(Float,FLOAT);
   init_unity_value(Short,SHORT);
   init_unity_value(Char,CHAR);
   init_unity_value(Double,DOUBLE);
   init_unity_value(Int,INT);
   init_unity_value(Byte,BYTE);
#undef init_unity_value
   rsprod_echo(stderr,"Exiting %s\n",__func__);
}

void librsprod_init_zerovalues(void) {
   rsprod_echo(stderr,"Entering %s\n",__func__);
   for (short t = 0 ; t < RSPROD_NBTYPES ; t++) {
      ZeroValues[t] = rsprodMalloc(1*SizeOf[t]);
   }
#define init_zero_value(t,T) {t fv = RSPROD_ZERO_ ## T;memcpy(ZeroValues[RSPROD_ ## T],&fv,SizeOf[RSPROD_ ## T]);}
   init_zero_value(Float,FLOAT);
   init_zero_value(Short,SHORT);
   init_zero_value(Char,CHAR);
   init_zero_value(Double,DOUBLE);
   init_zero_value(Int,INT);
   init_zero_value(Byte,BYTE);
#undef init_zero_value
   rsprod_echo(stderr,"Exiting %s\n",__func__);
}

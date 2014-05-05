
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netcdf-3/netcdf.h>
#include <rsprod.h>

char progname[] = "test_rsprodcharfields";

const size_t maxchars = 24;

int main(void) {

   int ret;

   size_t nx = 3;
   size_t ny = 11;
   size_t ne = nx*ny;

   /* create a dimension object */
   char *dimnames[2];
   dimnames[0] = malloc(strlen("ny")+1); strcpy(dimnames[0],"ny");
   dimnames[1] = malloc(strlen("nx")+1); strcpy(dimnames[1],"nx");
   size_t dimlengths[2];
   dimlengths[0]  = ny;
   dimlengths[1]  = nx;
   short  dimunlims[2];
   dimunlims[0] = dimunlims[1] = 0;
   rsprod_dimensions *outdims;
   ret = rsprod_dims_create(&outdims,2,dimnames,dimlengths,dimunlims);
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not create the Dimension object for output grid.\n",progname);
      exit(1);
   }

   float fillvalf = -99.;
   float *dataset_float = malloc(ne * sizeof(float));
   if (!dataset_float) {printf("PANIC with memory error.\n");exit(3);}
   for (size_t e = 0 ; e < ne ; e++) {
      dataset_float[e]  = e;
   }
   rsprod_field *floats_out;
   if (rsprod_field_createStandard(
	    &floats_out,"flt",RSPROD_FLOAT,ne,outdims,"floating point dataset","1",&fillvalf,NULL,NULL,dataset_float)) {
      fprintf(stderr,"ERROR (%s) could not create Field object <flt>.\n",progname);
      exit(1);
   }
   
   char fillvalc = '\0';

   char *dataset_char = malloc( (ne * maxchars + 1) * sizeof(char));
   if (!dataset_char) {printf("PANIC with memory error.\n");exit(3);}
   memset(dataset_char,(int)'.',ne*maxchars*sizeof(char));
   dataset_char[(ne * maxchars)] = '\0';
   for (size_t xi = 0 ; xi < nx ; xi++) {
      for (size_t yi = 0 ; yi < ny ; yi++) {
	 size_t e    = xi*ny + yi;
	 size_t i    = maxchars * e;
	 char *startchar = dataset_char + i;

         snprintf(startchar,maxchars,"f1:%01d:%01d=%02d",xi,yi,e);
	 startchar[strlen(startchar)] = ',';
      }
   }
   printf("Info: The char dataset 1 is:\n%s\n",dataset_char);

   rsprod_field *chars1_out;
   if (rsprod_field_createStandard(
	    &chars1_out,"chr1",RSPROD_CHAR,ne,outdims,"character strings dataset","-",&fillvalc,NULL,NULL,dataset_char)) {
      fprintf(stderr,"ERROR (%s) could not create Field object <chr1>.\n",progname);
      exit(1);
   }

   char **dataset_char2 = malloc( ne * sizeof(char *));
   if (!dataset_char2) {printf("PANIC with memory error.\n");exit(3);}
   for ( size_t e = 0 ; e < ne ; e ++ ) {
      dataset_char2[e] = malloc( maxchars * sizeof(char) );
      if (!dataset_char2[e]) {printf("PANIC with memory error.\n");exit(3);}
      memset(dataset_char2[e],(int)'\0',maxchars*sizeof(char));
   }
   for (size_t xi = 0 ; xi < nx ; xi++) {
      for (size_t yi = 0 ; yi < ny ; yi++) {
	 size_t e    = xi*ny + yi;

         snprintf(dataset_char2[e],maxchars,"f2:%01d:%01d=%d",xi,yi,e);
      }
   }
   printf("Info: The char dataset 2 is:<\n");
   for (size_t e = 0; e < ne ; e++) {
      printf("\t%s (%p)\n",dataset_char2[e],dataset_char2[e]);
   }
   printf(">\n");

   rsprod_field *chars2_out;
   if (rsprod_field_createStandard(
	    &chars2_out,"chr2",RSPROD_CHAR,ne,outdims,"character strings dataset","-",&fillvalc,NULL,NULL,dataset_char2)) {
      fprintf(stderr,"ERROR (%s) could not create Field object <char2>.\n",progname);
      exit(1);
   }

   int ncid;
   char outfile[] = "/tmp/testRSPRODCHARdatas.nc";
   ret = nc_create(outfile,NC_WRITE,&ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) Cannot open file <%s>\n",progname,outfile);
      exit(1);
   }
   printf("Info: output file is %s\n",outfile);

   ret = 0;
   ret += rsprod_field_writeToNetCDF(floats_out,ncid);
   ret += rsprod_field_writeToNetCDF(chars2_out,ncid);
   if (ret) {
      fprintf(stderr,"ERROR (%s) Cannot write Fields to netcdf file.\n",progname);
   }

   ret = nc_close(ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) Cannot close file <%s>\n",progname,outfile);
      exit(1);
   }

   exit(EXIT_SUCCESS);

}

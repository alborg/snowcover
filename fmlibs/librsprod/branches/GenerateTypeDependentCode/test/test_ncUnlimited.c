/*
 * NAME: test_ncUnlimited.c
 * 
 * PURPOSE: 
 *    Test program to check the use of UNLIMITED dimension in netcdf files.
 *
 * DESCRIPTION:
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 31.03.2011
 *
 * MODIFIED:
 *
 */ 


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "../src/rsprod.h"

char srcFile[] = "test_ncUnlimited.c";


int main(int argc, char *argv[]) {

   int ret;

   char *progname = malloc(strlen(argv[0])+1);
   if (!progname) {
      fprintf(stderr,"ERROR (%s) memory allocation problem.\n",srcFile);
      exit(EXIT_FAILURE);
   }
   sprintf(progname,"%s",argv[0]);;

   printf("<DESCRIPTION>\n");
   printf("Test program [%s]\n",progname);
   printf("Compiled from source file [%s]\n",srcFile);
   printf("SYNOPSIS:\n");
   printf("\tCreate a dataset with an unlimited dimension and write it\n");
   printf("\tto a netCDF file. Then append new data to the file.\n");
   printf("<START RUNNING>\n");

   char ncName[1028];
   sprintf(ncName,"/tmp/rsprod-tmp.nc");

   size_t n_lat = 6;
   size_t n_lon = 12;
   size_t n_rec = 3;
   size_t n_lev = 2;
   size_t ne = n_lat*n_lon*n_rec*n_lev;

   /* allocate the data field (arrays) and fill them with dummy values */
   double fillvalue_d  = -999.;

   double *fielddata = malloc(sizeof(*fielddata)*ne);
   if (!fielddata) {
      fprintf(stderr,"ERROR (%s) memory allocation problem.\n",progname);
      exit(EXIT_FAILURE);
   }
   for (size_t e = 0 ; e < ne ; e++) {
      if (e == 4 || e == 12) {
        fielddata[e] = fillvalue_d;
      } else {
        fielddata[e] = (e + 11.) ;
      }
   }


   /* 
    * Prepare the dimension object for this field object. If we have
    * several fields sharing the same dimensions (as is usually the case)
    * we still need to define this object only once.
    */
   int ndims = 4;
   char *dimnames[ndims];
   int d=0;
   dimnames[d] = malloc(strlen("time")+1); strcpy(dimnames[d],"time");d++;
   dimnames[d] = malloc(strlen("lev")+1); strcpy(dimnames[d],"lev");d++;
   dimnames[d] = malloc(strlen("lat")+1); strcpy(dimnames[d],"lat");d++;
   dimnames[d] = malloc(strlen("lon")+1); strcpy(dimnames[d],"lon");d++;
   size_t dimlengths[ndims];
   d=0;
   dimlengths[d]  = n_rec;d++;
   dimlengths[d]  = n_lev;d++;
   dimlengths[d]  = n_lat;d++;
   dimlengths[d]  = n_lon;d++;
   short  dimunlims[ndims];
   d=0;
   dimunlims[d] = 1;d++;
   dimunlims[d] = 0;d++;
   dimunlims[d] = 0;d++;
   dimunlims[d] = 0;d++;  
   rsprod_dimensions *outdims;
   ret = rsprod_dims_create(&outdims,ndims,dimnames,dimlengths,dimunlims);
   if (ret) {
      fprintf(stderr,"ERROR (%s) could not create the Dimension object for output grid.\n",progname);
      exit(EXIT_FAILURE);
   }
   
   /* 
    * Prepare the dataset/field. We describe the data with a name,
    * a type, its dimensions and some attributes.
    *
    */
   rsprod_field *f;
   ret = rsprod_field_createStandard(&f,
	 "field",                 /* 'code' name of the field */
	 RSPROD_DOUBLE,            /* type */
	 ne,outdims,              /* information on the dimensions */
	 "LongName for field",    /* long_name */
	 "StdName for field",     /* no standard name */
	 "m/s",                   /* units */
	 &fillvalue_d,            /* fill value */
	 NULL,                    /* valid_min */
	 NULL,                    /* valid_max */
	 0,NULL,                  /* no geographical coordinates */
	 fielddata               /* the array of data */
	 );
   if (ret) {
      fprintf(stderr,"ERROR (%s) Unable to create the field <%s>\n",progname,"field");
      exit(EXIT_FAILURE);
   }

   /* create a file objet with one dataset */
   rsprod_file *file;
   if (rsprod_file_create(&file,NULL,NULL)) {
      fprintf(stderr,"ERROR (%s) Did not manage to create the file object.\n",progname);
      exit(EXIT_FAILURE);
   }
   ret  = 0;
   ret += rsprod_file_addDataset(file,f) ;
   if (ret) {
      fprintf(stderr,"ERROR (%s) Did not manage to add the dataset.\n",progname);
      exit(EXIT_FAILURE);
   }

   rsprod_file_printNcdump(file);

   /* Write the file object to netcdf file */
   printf("Open netcdf file <%s> for writing\n",ncName);
   int ncid;
   ret = nc_create(ncName,NC_CLOBBER,&ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While opening netcdf file <%s>\n\t%s\n",progname,ncName,nc_strerror(ret));
      exit(EXIT_FAILURE);
   }
  
   ret = rsprod_file_writeToNetCDF(file,ncid);
   if (ret) {
      fprintf(stderr,"ERROR (%s) Cannot write file object to netCDF file\n\t%s\n",progname,ncName);
      exit(EXIT_FAILURE);
   }
   ret = nc_close(ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While closing the netcdf file.\n",progname);
      exit(EXIT_FAILURE);
   }

   rsprod_file_delete(file);

   exit(1);

   /* now read-in this file with the rsprod_file object */
   file = NULL;
   printf("Open netcdf file <%s> for reading\n",ncName);
   ret = nc_open(ncName,NC_NOCLOBBER,&ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While opening netcdf file <%s> for reading\n\t%s\n",progname,ncName,nc_strerror(ret));
      exit(EXIT_FAILURE);
   }
   char *names[] = {"field"};
   int nbNames = sizeof(names) / sizeof(names[0]);

   ret = rsprod_file_loadFromNetCDF(&file,nbNames,names,ncid);
   if (ret != nbNames) {
      fprintf(stderr,"ERROR (%s) Cannot load the file from netCDF.\n",progname);
      exit(EXIT_FAILURE);
   } 
   librsprod_echo_mode = LIBRSPROD_QUIET;
   ret = nc_close(ncid);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While closing the netcdf file.\n",progname);
      exit(EXIT_FAILURE);
   }
   fprintf(stderr,"Content of <%s> is:\n",ncName);
   rsprod_file_printInfo(file);

   /* test using the 'query string' */
   int myage;
   float *ffvalues;
   size_t myage_nb,ffvalue_nb;
   rsprod_type myage_type;
   char  *fname,*funit;
   short *svalues,*svalues_p,svaluesFV;
   size_t svalues_nb;
   unsigned short nbdims;
   unsigned int  *dims;
   unsigned int   dimv;

   char qstring[] = "field::# field::*s* field::&s field:_FillValue:*s field:units:&c :age_of_the_captain:*i :some_floats:f* :some_floats:# :age_of_the_captain:T :filename:&c field:[]:# field:[]:& field:[]:*0";
   fprintf(stderr,"VERBOSE (%s) Qstring is <%s>\n",progname,qstring);
   librsprod_echo_mode = LIBRSPROD_VERBOSE;
   ret = rsprod_file_accessContent(file,qstring,&svalues_nb,&svalues,&svalues_p,&svaluesFV,&funit,&myage,&ffvalues,&ffvalue_nb,&myage_type,&fname,&nbdims,&dims,&dimv);
   librsprod_echo_mode = LIBRSPROD_QUIET;
   if (ret) {
      fprintf(stderr,"ERROR (%s) Cannot use query string (%s) for accessing the content of rsprod_file.\n",progname,qstring);
      exit(EXIT_FAILURE);
   }
   printf("Age of the captain is %d (%u)\n",myage,myage_type);
   printf("The %d float values are:\n\t",ffvalue_nb);
   for (size_t i = 0 ; i < ffvalue_nb ; i++) printf("%.2f ",ffvalues[i]);
   printf("\n");
   printf("fname from global attributes is: <%s>\n",fname);
   printf("Number of values in the dataset: %u\n",svalues_nb);
   for (size_t n = 0 ; n < svalues_nb ; n++) {
      if (svalues[n] == svaluesFV) {
         printf("\t%2d -> %5s\n",n,"_");
      } else {
         printf("\t%2d -> %5d %s (%5d)\n",n,svalues[n],funit,svalues_p[n]);
      }
   }
   printf("Number of dimensions for the dataset: %u (%u)\n",nbdims,dimv);
   for (size_t n = 0 ; n < nbdims ; n++) {
      printf("\t%2u\n",dims[n]);
   }

   svalues[0]   = 0;

   char **field_names;
   size_t nb_field_names;
   if (!get_field_names_from_query_string(qstring,&field_names,&nb_field_names)) {
      fprintf(stderr,"ERROR (%s) Cannot extract interesting field names from query string:\n\t%s\n",__func__,qstring);
      exit(EXIT_FAILURE);
   }
   for (size_t c = 0 ; c < nb_field_names ; c++) {
      printf("FIELD NAME %02d is <%s>\n",c,field_names[c]);
   }
   

   char ncName2[1028];
   sprintf(ncName2,"/tmp/rsprod-tmp-copy.nc");

   /* now write the file object back to another netcdf file */
   printf("Open netcdf file <%s> for writing\n",ncName2);
   int ncid2;
   ret = nc_create(ncName2,NC_CLOBBER,&ncid2);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While opening netcdf file <%s> for writing\n\t%s\n",progname,ncName2,nc_strerror(ret));
      exit(EXIT_FAILURE);
   }
   librsprod_echo_mode = LIBRSPROD_VERBOSE;
   ret = rsprod_file_writeToNetCDF(file,ncid2);
   if (ret) {
      fprintf(stderr,"ERROR (%s) Cannot write the file to netCDF.\n",progname);
      exit(EXIT_FAILURE);
   } 
   librsprod_echo_mode = LIBRSPROD_QUIET;
   ret = nc_close(ncid2);
   if (ret != NC_NOERR) {
      fprintf(stderr,"ERROR (%s) While closing the netcdf file.\n",progname);
      exit(EXIT_FAILURE);
   }

   printf("<END %s>\n",progname);
   return(EXIT_SUCCESS);

}

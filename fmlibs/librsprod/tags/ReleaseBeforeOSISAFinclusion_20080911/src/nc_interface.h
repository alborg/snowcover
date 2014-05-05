
/* 
 * NAME : nc_interface.h
 *
 * PURPOSE:
 *    Implement all the interface between the rsprod and the nectcdf library.
 *    All the reading and writing to/from this format should be contained here.
 *
 * AUTHOR:    
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 * */

#ifndef NC_INTERFACE_H
#define NC_INTERFACE_H

#include <netcdf.h>

#define NC_DEF_MODE 0
#define NC_DAT_MODE 1

/* base string for the 'number of characters' dimension. Usefull when dealing with character strings datasets */
/* This variable should be 'configurable' by the user. */
#define NC_NUMCHARDIM_BASE "nch"

/* =================================================================== 
 *         NETCDF    INTERFACE 
 * =================================================================== */
nc_type convertType_rsprod2nc(rsprod_type type);
rsprod_type convertType_nc2rsprod(nc_type type);
int  rsprod_field_loadFromNetCDF(rsprod_field **this,char *fieldname,const int ncid);
int  rsprod_field_writeToNetCDF(rsprod_field *this,const int ncid);
int  rsprod_attributes_loadFromNetCDF(List *this, const int ncid, const int fieldid);
int  rsprod_attributes_writeToNetCDF(rsprod_attributes this,const int ncid, const int fieldid);


#endif /* NC_INTERFACE_H */

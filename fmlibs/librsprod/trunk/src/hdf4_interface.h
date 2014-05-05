/* 
 * NAME : hdf4_interface.h
 *
 * PURPOSE:
 *    Implement all the interface between the rsprod and the HDF4 (SDS) library.
 *    All the reading and writing to/from this format should be contained here.
 *
 * AUTHOR:    
 *    Thomas Lavergne, met.no, 17.08.2009
 *
 * MODIFIED:
 *
 * */

#ifndef HDF4_INTERFACE_H
#define HDF4_INTERFACE_H

#include <mfhdf.h>

#include "x_interface.h"

int rsprod_dimensions_loadFromHDF4(rsprod_dimensions **this,const int32 sd_id, const int32 sds_id);
int rsprod_attributes_loadFromHDF4(rsprod_attributes **this, const int ncid, const int fieldid);
int rsprod_data_loadFromHDF4(rsprod_data **this,rsprod_dimensions *dims,int32 sd_id,int32 sds_id);
int rsprod_field_loadFromHDF4(rsprod_field **this,char *fieldname,const int32 fileid);
int rsprod_file_loadFromHDF4(rsprod_file **this,int nbFieldsSearched, char *fieldNames[/*nbFieldsSearched*/],const int32 sd_id);

#endif /* HDF4_INTERFACE_H */


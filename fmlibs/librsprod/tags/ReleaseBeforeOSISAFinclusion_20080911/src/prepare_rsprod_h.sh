#!/bin/bash

date=`date`

outname="rsprod.h"
srcnames="list.h rsprod_types.h rsprod_intern.h calibration.h nc_interface.h"

rm -f $outname

cat > $outname <<PREAMB
/* 
 * File : $outname
 * Generated automatically on $date
 * By prepare_rsprod_h.sh
 * From: $PWD/
PREAMB

for src in $srcnames; do
   echo -e "\t\t$src" >> $outname
done
echo '*/' >> $outname
echo '' >> $outname

echo '#ifndef RSPROD_H'      >> $outname
echo '#define RSPROD_H'      >> $outname
echo ''                      >> $outname

for src in $srcnames; do
   cat $src | grep -v -e "^#include"    >> $outname
done

echo '#endif /* RSPROD_H */' >> $outname

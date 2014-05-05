#!/bin/bash

date=`date`

outname=$1
shift
srcnames=$@

#echo "OUTNAME: <$outname>"
#echo "SRCNAME: <$srcnames>"

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
   #cat $src  >> $outname
   cat $src | grep -v -e "^#include \""    >> $outname
done

echo '#endif /* RSPROD_H */' >> $outname

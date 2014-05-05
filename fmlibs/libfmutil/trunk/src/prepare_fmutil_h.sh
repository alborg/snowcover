#!/bin/bash

date=`date`

outname="fmutil.h"

srcnames="fmutil_config.h fmutil_types.h fmcoord.h fmtime.h fmsolar.h fmangles.h fmangleconversion.h fmstrings.h"
srcnames="$srcnames fmstorage.h fmimage.h fmbyteswap.h fmcolormaps.h fmerrmsg.h fm_ch3b_reflectance.h fmtouch.h"
srcnames="$srcnames fmfeltfile.h fmfilesystem.h"

rm -f $outname

cat > $outname <<PREAMB
/* 
 * File : $outname
 * Generated automatically on $date
 * By $0
 * From: $PWD/
PREAMB

for src in $srcnames; do
   echo -e "\t\t$src" >> $outname
done
echo '*/' >> $outname
echo '' >> $outname

echo '#ifndef FMUTIL_H'      >> $outname
echo '#define FMUTIL_H'      >> $outname
echo ''                      >> $outname

cat >> $outname <<PREAMB2
#ifdef __cplusplus
extern "C" {
#endif
PREAMB2

echo '#include <time.h>'      >> $outname

for src in $srcnames; do
   cat $src | grep -v -e "^#include \""    >> $outname
done

cat >> $outname <<PREAMB2
#ifdef __cplusplus
}
#endif
PREAMB2

echo '#endif /* FMUTIL_H */' >> $outname

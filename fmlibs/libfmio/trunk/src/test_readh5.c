/*
 * test_readh5.c
 *
 *  Created on: 16. apr. 2014
 *      Author: anettelb
 */

//  gcc -c test_readh5.c -I/home/anettelb/prog/satellite/snoflak/fmlibs/libfmio/trunk/src -I/user/include -I/home/anettelb/cpp/include
//  gcc -c fm_readdataMETSATswath.c -I/home/anettelb/prog/satellite/snoflak/fmlibs/libfmio/trunk/src -I/user/include -I/home/anettelb/cpp/include
//  gcc test_readh5.o fm_readdataMETSATswath.o -o test_h5 -lhdf5 -L/home/anettelb/cpp/lib -lfmio -lfmutil



#include <fmio.h>
//#include <fmutil.h>

int main(int argc, char** argv[])
{

//	char *test_tz;
//	test_tz = getenv("TZ");
//	printf("TZ: %s \n",test_tz);

	//char *filename = "/disk1/snoflak/import/PPS_data/remapped/S_NWC_viirs_npp_09364_20130818T1055446Z_20130818T1109575Z.h5"; //Nye NPP-filer, data
	//char *filename = "/disk1/snoflak/noaa16_20140415_0844_69924_satproj_00000_04476_avhrr.h5"; //Nye noaa-filer, datafil

	//char *filename = "/disk1/snoflak/MariAnneEx/noaa14_20000101_1144_99999_satproj_00000_12221_cloudtype.h5"; //GAC-fil, gammel, skytype
	char *filename = "/disk1/snoflak/export/S_NWC_CT_npp_09364_20130818T1055446Z_20130818T1109575Z.h5"; //Nye NPP-filer, skytype
	//char *filename = "/disk1/snoflak/import/AUX_data/remapped/S_NWC_physiography_npp_09364_20130818T1055446Z_20130818T1109575Z.h5"; //Nye NPP-filer, physio
	//char *filename = "/disk1/snoflak/import/ANC_data/remapped/S_NWC_sunsatangles_npp_09364_20130818T1055446Z_20130818T1109575Z.h5"; //Nye NPP-filer, sun angles
	//char *filename = "S_NWC_CT_noaa16_08252_20020429T1128434Z_20020429T1139276Z.nc"; //Nye noaa-filer, skytype, swath

	fmio_img *h1;
	fmdataset *h2;

	int test = fm_readMETSATdata_swath(filename, h1);
	fmbool header_only = FMFALSE;
	//int test = fm_readH5data(filename, h2, header_only);

	return 0;

}

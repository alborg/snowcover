/*
 * NAME: calibration.c/h
 * 
 * PURPOSE: 
 *    Implement the some 'calibration' and 'cast' routines. The main goal is
 *    to allow the smooth transfer from one type to another, using calibration
 *    parameters like scale and offset.
 *
 * DESCRIPTION:
 *    We have three 'calibration' types between type <t1> and <t2>.
 *    o noeffect : simply a cast from <t1> to <t2>.
 *    o type1    : uses 2 coefficients of type <t1> : <t2> = <t1> * a1 + a2;
 *    o type2    : uses 2 coefficients of type <t2> : <t2> = <t1> * a1 + a2;
 *    
 *
 * NOTE:
 *    
 * DEPENDENCIES:
 *
 * AUTHOR: 
 *    Thomas Lavergne, met.no, 07.01.2008
 *
 * MODIFIED:
 *
 */ 

#include <stdio.h>
#include <stdlib.h>
#include "rsprod_types.h"
#include "rsprod_intern.h"

#define DefineNoCalibrationFunction(f,t) \
   void calibration_noeffectFrom_##f##_to_##t (t *r, f *v,void *p1,void *p2) { *r = *v; }
DefineNoCalibrationFunction(float,float)
DefineNoCalibrationFunction(float,short)
DefineNoCalibrationFunction(float,char)
DefineNoCalibrationFunction(float,double)
DefineNoCalibrationFunction(float,int)
DefineNoCalibrationFunction(short,float)
DefineNoCalibrationFunction(short,short)
DefineNoCalibrationFunction(short,char)
DefineNoCalibrationFunction(short,double)
DefineNoCalibrationFunction(short,int)
DefineNoCalibrationFunction(char,float)
DefineNoCalibrationFunction(char,short)
DefineNoCalibrationFunction(char,char)
DefineNoCalibrationFunction(char,double)
DefineNoCalibrationFunction(char,int)
DefineNoCalibrationFunction(double,float)
DefineNoCalibrationFunction(double,short)
DefineNoCalibrationFunction(double,char)
DefineNoCalibrationFunction(double,double)
DefineNoCalibrationFunction(double,int)
DefineNoCalibrationFunction(int,float)
DefineNoCalibrationFunction(int,short)
DefineNoCalibrationFunction(int,char)
DefineNoCalibrationFunction(int,double)
DefineNoCalibrationFunction(int,int)
#undef DefineNoCalibrationFunction

#define DefineCalibrationType1Function(f,t) \
   void calibration_type1From_##f##_to_##t (t *r, f *v, f *p1, f *p2) { *r = (*v) * (*p1) + *p2; }
DefineCalibrationType1Function(float,float)
DefineCalibrationType1Function(float,short)
DefineCalibrationType1Function(float,char)
DefineCalibrationType1Function(float,double)
DefineCalibrationType1Function(float,int)
DefineCalibrationType1Function(short,float)
DefineCalibrationType1Function(short,short)
DefineCalibrationType1Function(short,char)
DefineCalibrationType1Function(short,double)
DefineCalibrationType1Function(short,int)
DefineCalibrationType1Function(char,float)
DefineCalibrationType1Function(char,short)
DefineCalibrationType1Function(char,char)
DefineCalibrationType1Function(char,double)
DefineCalibrationType1Function(char,int)
DefineCalibrationType1Function(double,float)
DefineCalibrationType1Function(double,short)
DefineCalibrationType1Function(double,char)
DefineCalibrationType1Function(double,double)
DefineCalibrationType1Function(double,int)
DefineCalibrationType1Function(int,float)
DefineCalibrationType1Function(int,short)
DefineCalibrationType1Function(int,char)
DefineCalibrationType1Function(int,double)
DefineCalibrationType1Function(int,int)
#undef DefineCalibrationType1Function

#define DefineCalibrationType2Function(f,t) \
   void calibration_type2From_##f##_to_##t (t *r, f *v, t *p1, t *p2) { *r = (t)(*v) * (*p1) + *p2; }
DefineCalibrationType2Function(float,float)
DefineCalibrationType2Function(float,short)
DefineCalibrationType2Function(float,char)
DefineCalibrationType2Function(float,double)
DefineCalibrationType2Function(float,int)
DefineCalibrationType2Function(short,float)
DefineCalibrationType2Function(short,short)
DefineCalibrationType2Function(short,char)
DefineCalibrationType2Function(short,double)
DefineCalibrationType2Function(short,int)
DefineCalibrationType2Function(char,float)
DefineCalibrationType2Function(char,short)
DefineCalibrationType2Function(char,char)
DefineCalibrationType2Function(char,double)
DefineCalibrationType2Function(char,int)
DefineCalibrationType2Function(double,float)
DefineCalibrationType2Function(double,short)
DefineCalibrationType2Function(double,char)
DefineCalibrationType2Function(double,double)
DefineCalibrationType2Function(double,int)
DefineCalibrationType2Function(int,float)
DefineCalibrationType2Function(int,short)
DefineCalibrationType2Function(int,char)
DefineCalibrationType2Function(int,double)
DefineCalibrationType2Function(int,int)
#undef DefineCalibrationType2Function


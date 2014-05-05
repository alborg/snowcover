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
 *    o type1    : uses 2 coefficients of type <t1> : <t2> = <t1> * a1 + a2; e.g. <short> = <float>*
 *    o type2    : uses 2 coefficients of type <t2> : <t2> = <t1> * a1 + a2; e.g. <float> = <short>*0.01 + 0.1
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
DefineNoCalibrationFunction(Float,Float)
DefineNoCalibrationFunction(Float,Short)
DefineNoCalibrationFunction(Float,Char)
DefineNoCalibrationFunction(Float,Double)
DefineNoCalibrationFunction(Float,Int)
DefineNoCalibrationFunction(Float,Byte)
DefineNoCalibrationFunction(Short,Float)
DefineNoCalibrationFunction(Short,Short)
DefineNoCalibrationFunction(Short,Char)
DefineNoCalibrationFunction(Short,Double)
DefineNoCalibrationFunction(Short,Int)
DefineNoCalibrationFunction(Short,Byte)
DefineNoCalibrationFunction(Char,Float)
DefineNoCalibrationFunction(Char,Short)
DefineNoCalibrationFunction(Char,Char)
DefineNoCalibrationFunction(Char,Double)
DefineNoCalibrationFunction(Char,Int)
DefineNoCalibrationFunction(Char,Byte)
DefineNoCalibrationFunction(Double,Float)
DefineNoCalibrationFunction(Double,Short)
DefineNoCalibrationFunction(Double,Char)
DefineNoCalibrationFunction(Double,Double)
DefineNoCalibrationFunction(Double,Int)
DefineNoCalibrationFunction(Double,Byte)
DefineNoCalibrationFunction(Int,Float)
DefineNoCalibrationFunction(Int,Short)
DefineNoCalibrationFunction(Int,Char)
DefineNoCalibrationFunction(Int,Double)
DefineNoCalibrationFunction(Int,Int)
DefineNoCalibrationFunction(Int,Byte)
DefineNoCalibrationFunction(Byte,Float)
DefineNoCalibrationFunction(Byte,Short)
DefineNoCalibrationFunction(Byte,Char)
DefineNoCalibrationFunction(Byte,Double)
DefineNoCalibrationFunction(Byte,Int)
DefineNoCalibrationFunction(Byte,Byte)
#undef DefineNoCalibrationFunction

#define DefineCalibrationType1Function(f,t) \
   void calibration_type1From_##f##_to_##t (t *r, f *v, f *p1, f *p2) { *r = ( ( (*v) + *p2 ) / *p1 ); }
DefineCalibrationType1Function(Float,Float)
DefineCalibrationType1Function(Float,Short)
DefineCalibrationType1Function(Float,Char)
DefineCalibrationType1Function(Float,Double)
DefineCalibrationType1Function(Float,Int)
DefineCalibrationType1Function(Float,Byte)
DefineCalibrationType1Function(Short,Float)
DefineCalibrationType1Function(Short,Short)
DefineCalibrationType1Function(Short,Char)
DefineCalibrationType1Function(Short,Double)
DefineCalibrationType1Function(Short,Int)
DefineCalibrationType1Function(Short,Byte)
DefineCalibrationType1Function(Char,Float)
DefineCalibrationType1Function(Char,Short)
DefineCalibrationType1Function(Char,Char)
DefineCalibrationType1Function(Char,Double)
DefineCalibrationType1Function(Char,Int)
DefineCalibrationType1Function(Char,Byte)
DefineCalibrationType1Function(Double,Float)
DefineCalibrationType1Function(Double,Short)
DefineCalibrationType1Function(Double,Char)
DefineCalibrationType1Function(Double,Double)
DefineCalibrationType1Function(Double,Int)
DefineCalibrationType1Function(Double,Byte)
DefineCalibrationType1Function(Int,Float)
DefineCalibrationType1Function(Int,Short)
DefineCalibrationType1Function(Int,Char)
DefineCalibrationType1Function(Int,Double)
DefineCalibrationType1Function(Int,Int)
DefineCalibrationType1Function(Int,Byte)
DefineCalibrationType1Function(Byte,Float)
DefineCalibrationType1Function(Byte,Short)
DefineCalibrationType1Function(Byte,Char)
DefineCalibrationType1Function(Byte,Double)
DefineCalibrationType1Function(Byte,Int)
DefineCalibrationType1Function(Byte,Byte)
#undef DefineCalibrationType1Function

#define DefineCalibrationType2Function(f,t) \
   void calibration_type2From_##f##_to_##t (t *r, f *v, t *p1, t *p2) { *r = (t)(*v) * (*p1) + *p2; }

DefineCalibrationType2Function(Float,Float)
DefineCalibrationType2Function(Float,Short)
DefineCalibrationType2Function(Float,Char)
DefineCalibrationType2Function(Float,Double)
DefineCalibrationType2Function(Float,Int)
DefineCalibrationType2Function(Float,Byte)
DefineCalibrationType2Function(Short,Float)
DefineCalibrationType2Function(Short,Short)
DefineCalibrationType2Function(Short,Char)
DefineCalibrationType2Function(Short,Double)
DefineCalibrationType2Function(Short,Int)
DefineCalibrationType2Function(Short,Byte)
DefineCalibrationType2Function(Char,Float)
DefineCalibrationType2Function(Char,Short)
DefineCalibrationType2Function(Char,Char)
DefineCalibrationType2Function(Char,Double)
DefineCalibrationType2Function(Char,Int)
DefineCalibrationType2Function(Char,Byte)
DefineCalibrationType2Function(Double,Float)
DefineCalibrationType2Function(Double,Short)
DefineCalibrationType2Function(Double,Char)
DefineCalibrationType2Function(Double,Double)
DefineCalibrationType2Function(Double,Int)
DefineCalibrationType2Function(Double,Byte)
DefineCalibrationType2Function(Int,Float)
DefineCalibrationType2Function(Int,Short)
DefineCalibrationType2Function(Int,Char)
DefineCalibrationType2Function(Int,Double)
DefineCalibrationType2Function(Int,Int)
DefineCalibrationType2Function(Int,Byte)
DefineCalibrationType2Function(Byte,Float)
DefineCalibrationType2Function(Byte,Short)
DefineCalibrationType2Function(Byte,Char)
DefineCalibrationType2Function(Byte,Double)
DefineCalibrationType2Function(Byte,Int)
DefineCalibrationType2Function(Byte,Byte)
#undef DefineCalibrationType2Function


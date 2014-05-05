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

#ifndef CALIBRATION_H
#define CALIBRATION_H

#include "rsprod_types.h"

/* define the calibration 'types' */
#define RSPROD_CALIBRATION_UNKNOWN    -1
#define RSPROD_CALIBRATION_ONLYCAST    0
#define RSPROD_CALIBRATION_TYPE1       1
#define RSPROD_CALIBRATION_TYPE2       2

/* =============================================================
 *     CALIBRATION FUNCTIONS
 * ============================================================= */
#define DefineNoCalibrationFunctionPrototype(f,t) \
   void calibration_noeffectFrom_##f##_to_##t (t *r, f *v,void *p1,void *p2)
DefineNoCalibrationFunctionPrototype(Float,Float);
DefineNoCalibrationFunctionPrototype(Float,Short);
DefineNoCalibrationFunctionPrototype(Float,Char);
DefineNoCalibrationFunctionPrototype(Float,Double);
DefineNoCalibrationFunctionPrototype(Float,Int);
DefineNoCalibrationFunctionPrototype(Float,Byte);
DefineNoCalibrationFunctionPrototype(Short,Float);
DefineNoCalibrationFunctionPrototype(Short,Short);
DefineNoCalibrationFunctionPrototype(Short,Char);
DefineNoCalibrationFunctionPrototype(Short,Double);
DefineNoCalibrationFunctionPrototype(Short,Int);
DefineNoCalibrationFunctionPrototype(Short,Byte);
DefineNoCalibrationFunctionPrototype(Char,Float);
DefineNoCalibrationFunctionPrototype(Char,Short);
DefineNoCalibrationFunctionPrototype(Char,Char);
DefineNoCalibrationFunctionPrototype(Char,Double);
DefineNoCalibrationFunctionPrototype(Char,Int);
DefineNoCalibrationFunctionPrototype(Char,Byte);
DefineNoCalibrationFunctionPrototype(Double,Float);
DefineNoCalibrationFunctionPrototype(Double,Short);
DefineNoCalibrationFunctionPrototype(Double,Char);
DefineNoCalibrationFunctionPrototype(Double,Double);
DefineNoCalibrationFunctionPrototype(Double,Int);
DefineNoCalibrationFunctionPrototype(Double,Byte);
DefineNoCalibrationFunctionPrototype(Int,Float);
DefineNoCalibrationFunctionPrototype(Int,Short);
DefineNoCalibrationFunctionPrototype(Int,Char);
DefineNoCalibrationFunctionPrototype(Int,Double);
DefineNoCalibrationFunctionPrototype(Int,Int);
DefineNoCalibrationFunctionPrototype(Int,Byte);
DefineNoCalibrationFunctionPrototype(Byte,Float);
DefineNoCalibrationFunctionPrototype(Byte,Short);
DefineNoCalibrationFunctionPrototype(Byte,Char);
DefineNoCalibrationFunctionPrototype(Byte,Double);
DefineNoCalibrationFunctionPrototype(Byte,Int);
DefineNoCalibrationFunctionPrototype(Byte,Byte);
#undef DefineNoCalibrationFunctionPrototype

#define DefineCalibrationType1FunctionPrototype(f,t) \
   void calibration_type1From_##f##_to_##t (t *r, f *v, f *p1, f *p2)
DefineCalibrationType1FunctionPrototype(Float,Float);
DefineCalibrationType1FunctionPrototype(Float,Short);
DefineCalibrationType1FunctionPrototype(Float,Char);
DefineCalibrationType1FunctionPrototype(Float,Double);
DefineCalibrationType1FunctionPrototype(Float,Int);
DefineCalibrationType1FunctionPrototype(Float,Byte);
DefineCalibrationType1FunctionPrototype(Short,Float);
DefineCalibrationType1FunctionPrototype(Short,Short);
DefineCalibrationType1FunctionPrototype(Short,Char);
DefineCalibrationType1FunctionPrototype(Short,Double);
DefineCalibrationType1FunctionPrototype(Short,Int);
DefineCalibrationType1FunctionPrototype(Short,Byte);
DefineCalibrationType1FunctionPrototype(Char,Float);
DefineCalibrationType1FunctionPrototype(Char,Short);
DefineCalibrationType1FunctionPrototype(Char,Char);
DefineCalibrationType1FunctionPrototype(Char,Double);
DefineCalibrationType1FunctionPrototype(Char,Int);
DefineCalibrationType1FunctionPrototype(Char,Byte);
DefineCalibrationType1FunctionPrototype(Double,Float);
DefineCalibrationType1FunctionPrototype(Double,Short);
DefineCalibrationType1FunctionPrototype(Double,Char);
DefineCalibrationType1FunctionPrototype(Double,Double);
DefineCalibrationType1FunctionPrototype(Double,Int);
DefineCalibrationType1FunctionPrototype(Double,Byte);
DefineCalibrationType1FunctionPrototype(Int,Float);
DefineCalibrationType1FunctionPrototype(Int,Short);
DefineCalibrationType1FunctionPrototype(Int,Char);
DefineCalibrationType1FunctionPrototype(Int,Double);
DefineCalibrationType1FunctionPrototype(Int,Int);
DefineCalibrationType1FunctionPrototype(Int,Byte);
DefineCalibrationType1FunctionPrototype(Byte,Float);
DefineCalibrationType1FunctionPrototype(Byte,Short);
DefineCalibrationType1FunctionPrototype(Byte,Char);
DefineCalibrationType1FunctionPrototype(Byte,Double);
DefineCalibrationType1FunctionPrototype(Byte,Int);
DefineCalibrationType1FunctionPrototype(Byte,Byte);
#undef DefineCalibrationType1FunctionPrototype

#define DefineCalibrationType2FunctionPrototype(f,t) \
   void calibration_type2From_##f##_to_##t (t *r, f *v, t *p1, t *p2)
DefineCalibrationType2FunctionPrototype(Float,Float);
DefineCalibrationType2FunctionPrototype(Float,Short);
DefineCalibrationType2FunctionPrototype(Float,Char);
DefineCalibrationType2FunctionPrototype(Float,Double);
DefineCalibrationType2FunctionPrototype(Float,Int);
DefineCalibrationType2FunctionPrototype(Float,Byte);
DefineCalibrationType2FunctionPrototype(Short,Float);
DefineCalibrationType2FunctionPrototype(Short,Short);
DefineCalibrationType2FunctionPrototype(Short,Char);
DefineCalibrationType2FunctionPrototype(Short,Double);
DefineCalibrationType2FunctionPrototype(Short,Int);
DefineCalibrationType2FunctionPrototype(Short,Byte);
DefineCalibrationType2FunctionPrototype(Char,Float);
DefineCalibrationType2FunctionPrototype(Char,Short);
DefineCalibrationType2FunctionPrototype(Char,Char);
DefineCalibrationType2FunctionPrototype(Char,Double);
DefineCalibrationType2FunctionPrototype(Char,Int);
DefineCalibrationType2FunctionPrototype(Char,Byte);
DefineCalibrationType2FunctionPrototype(Double,Float);
DefineCalibrationType2FunctionPrototype(Double,Short);
DefineCalibrationType2FunctionPrototype(Double,Char);
DefineCalibrationType2FunctionPrototype(Double,Double);
DefineCalibrationType2FunctionPrototype(Double,Int);
DefineCalibrationType2FunctionPrototype(Double,Byte);
DefineCalibrationType2FunctionPrototype(Int,Float);
DefineCalibrationType2FunctionPrototype(Int,Short);
DefineCalibrationType2FunctionPrototype(Int,Char);
DefineCalibrationType2FunctionPrototype(Int,Double);
DefineCalibrationType2FunctionPrototype(Int,Int);
DefineCalibrationType2FunctionPrototype(Int,Byte);
DefineCalibrationType2FunctionPrototype(Byte,Float);
DefineCalibrationType2FunctionPrototype(Byte,Short);
DefineCalibrationType2FunctionPrototype(Byte,Char);
DefineCalibrationType2FunctionPrototype(Byte,Double);
DefineCalibrationType2FunctionPrototype(Byte,Int);
DefineCalibrationType2FunctionPrototype(Byte,Byte);
#undef DefineCalibrationType2FunctionPrototype


#endif /* CALIBRATION_H */


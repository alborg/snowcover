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

/* define the calibration 'types' */
#define RSPROD_CALIBRATION_NOEFFECT    0
#define RSPROD_CALIBRATION_TYPE1       1
#define RSPROD_CALIBRATION_TYPE2       2

/* =============================================================
 *     CALIBRATION FUNCTIONS
 * ============================================================= */
#define DefineNoCalibrationFunctionPrototype(f,t) \
   void calibration_noeffectFrom_##f##_to_##t (t *r, f *v,void *p1,void *p2)
DefineNoCalibrationFunctionPrototype(float,float);
DefineNoCalibrationFunctionPrototype(float,short);
DefineNoCalibrationFunctionPrototype(float,char);
DefineNoCalibrationFunctionPrototype(float,double);
DefineNoCalibrationFunctionPrototype(float,int);
DefineNoCalibrationFunctionPrototype(short,float);
DefineNoCalibrationFunctionPrototype(short,short);
DefineNoCalibrationFunctionPrototype(short,char);
DefineNoCalibrationFunctionPrototype(short,double);
DefineNoCalibrationFunctionPrototype(short,int);
DefineNoCalibrationFunctionPrototype(char,float);
DefineNoCalibrationFunctionPrototype(char,short);
DefineNoCalibrationFunctionPrototype(char,char);
DefineNoCalibrationFunctionPrototype(char,double);
DefineNoCalibrationFunctionPrototype(char,int);
DefineNoCalibrationFunctionPrototype(double,float);
DefineNoCalibrationFunctionPrototype(double,short);
DefineNoCalibrationFunctionPrototype(double,char);
DefineNoCalibrationFunctionPrototype(double,double);
DefineNoCalibrationFunctionPrototype(double,int);
DefineNoCalibrationFunctionPrototype(int,float);
DefineNoCalibrationFunctionPrototype(int,short);
DefineNoCalibrationFunctionPrototype(int,char);
DefineNoCalibrationFunctionPrototype(int,double);
DefineNoCalibrationFunctionPrototype(int,int);
#undef DefineNoCalibrationFunctionPrototype

#define DefineCalibrationType1FunctionPrototype(f,t) \
   void calibration_type1From_##f##_to_##t (t *r, f *v, f *p1, f *p2)
DefineCalibrationType1FunctionPrototype(float,float);
DefineCalibrationType1FunctionPrototype(float,short);
DefineCalibrationType1FunctionPrototype(float,char);
DefineCalibrationType1FunctionPrototype(float,double);
DefineCalibrationType1FunctionPrototype(float,int);
DefineCalibrationType1FunctionPrototype(short,float);
DefineCalibrationType1FunctionPrototype(short,short);
DefineCalibrationType1FunctionPrototype(short,char);
DefineCalibrationType1FunctionPrototype(short,double);
DefineCalibrationType1FunctionPrototype(short,int);
DefineCalibrationType1FunctionPrototype(char,float);
DefineCalibrationType1FunctionPrototype(char,short);
DefineCalibrationType1FunctionPrototype(char,char);
DefineCalibrationType1FunctionPrototype(char,double);
DefineCalibrationType1FunctionPrototype(char,int);
DefineCalibrationType1FunctionPrototype(double,float);
DefineCalibrationType1FunctionPrototype(double,short);
DefineCalibrationType1FunctionPrototype(double,char);
DefineCalibrationType1FunctionPrototype(double,double);
DefineCalibrationType1FunctionPrototype(double,int);
DefineCalibrationType1FunctionPrototype(int,float);
DefineCalibrationType1FunctionPrototype(int,short);
DefineCalibrationType1FunctionPrototype(int,char);
DefineCalibrationType1FunctionPrototype(int,double);
DefineCalibrationType1FunctionPrototype(int,int);
#undef DefineCalibrationType1FunctionPrototype

#define DefineCalibrationType2FunctionPrototype(f,t) \
   void calibration_type2From_##f##_to_##t (t *r, f *v, t *p1, t *p2)
DefineCalibrationType2FunctionPrototype(float,float);
DefineCalibrationType2FunctionPrototype(float,short);
DefineCalibrationType2FunctionPrototype(float,char);
DefineCalibrationType2FunctionPrototype(float,double);
DefineCalibrationType2FunctionPrototype(float,int);
DefineCalibrationType2FunctionPrototype(short,float);
DefineCalibrationType2FunctionPrototype(short,short);
DefineCalibrationType2FunctionPrototype(short,char);
DefineCalibrationType2FunctionPrototype(short,double);
DefineCalibrationType2FunctionPrototype(short,int);
DefineCalibrationType2FunctionPrototype(char,float);
DefineCalibrationType2FunctionPrototype(char,short);
DefineCalibrationType2FunctionPrototype(char,char);
DefineCalibrationType2FunctionPrototype(char,double);
DefineCalibrationType2FunctionPrototype(char,int);
DefineCalibrationType2FunctionPrototype(double,float);
DefineCalibrationType2FunctionPrototype(double,short);
DefineCalibrationType2FunctionPrototype(double,char);
DefineCalibrationType2FunctionPrototype(double,double);
DefineCalibrationType2FunctionPrototype(double,int);
DefineCalibrationType2FunctionPrototype(int,float);
DefineCalibrationType2FunctionPrototype(int,short);
DefineCalibrationType2FunctionPrototype(int,char);
DefineCalibrationType2FunctionPrototype(int,double);
DefineCalibrationType2FunctionPrototype(int,int);
#undef DefineCalibrationType2FunctionPrototype


#endif /* CALIBRATION_H */


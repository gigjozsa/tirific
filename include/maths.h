/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file maths.h
   @brief  Mathematical functions that are not contained in the 
   standard library

   maths.c defines all mathematical functions that are not contained
   in math.h and binds the math.h functionality without interfacing
   with internal structures. To bind all generic mathematical
   functionality include this module.
   
   $Source: /Volumes/DATA_J_II/data/CVS/tirific/include/maths.h,v $
   $Date: 2011/05/25 22:25:25 $
   $Revision: 1.13 $
   $Author: jozsa $
   $Log: maths.h,v $
   Revision 1.13  2011/05/25 22:25:25  jozsa
   Left work

   Revision 1.12  2010/04/12 23:15:50  jozsa
   Left work

   Revision 1.11  2010/04/01 09:24:19  jozsa
   included and hopefully debugged: radial/vertical movement/gradients of those in z/azimuthal harmonics in velocity and surface brightness. To do 1) subclouds 2) Gaussian variations in azimuth 3) portions of a disk 4) 4 disks

   Revision 1.10  2009/05/27 15:07:37  jozsa
   Left work

   Revision 1.9  2009/05/26 07:56:39  jozsa
   Left work

   Revision 1.8  2007/08/22 15:58:35  gjozsa
   Left work

   Revision 1.7  2005/08/17 14:31:52  gjozsa
   Left work

   Revision 1.6  2005/05/24 15:58:32  gjozsa
   Added rotation

   Revision 1.5  2005/04/14 10:32:12  gjozsa
   Left work

   Revision 1.4  2005/03/08 17:55:04  gjozsa
   Left work

   Revision 1.3  2004/12/09 16:17:24  gjozsa
   Changed some floating point operations from double to float accuracy

   Revision 1.2  2004/11/09 15:58:38  gjozsa
   fixed minor bug

   Revision 1.1.1.1  2004/10/29 11:13:05  gjozsa
   Added to CVS control


*/
/* ------------------------------------------------------------ */



/* Include guard */
#ifndef MATHS_H
#define MATHS_H


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <math.h>



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def PI_HERE
   @brief pi
   
   There is no standard ANSI symbolic constant that is pi
*/
/* ------------------------------------------------------------ */
#define PI_HERE 3.141592653589793115997963468544185161590576171875



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def INTERPTYPE_XXX
   @brief Integer identifyer for interpolation types

   PGP_I_LINEAR: linear interpolation
   PGP_I_CPLINE: natural cubic spline
   PGP_I_AKIMA: natural Akima interpolation

   See gsl documentation.

*/
/* ------------------------------------------------------------ */
#define MATHS_I_LINEAR 0
#define MATHS_I_CSPLINE 1
#define MATHS_I_AKIMA 2


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* MACROS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* TYPEDEFS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* STRUCTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct maths_rstrf 
   @brief A struct for the use in the implemented random number
   generator maths_rndmf()

   Struct for the control of the implemented random generator in this
   module. This is the float version. Has to be allocated and then to
   be initialised with maths_rndmf_init(). After that, if passed to
   maths_rndmf() it serves to deliver quasirandom numbers.
*/
/* ------------------------------------------------------------ */

typedef struct maths_rstrf 
{
  int i97;
  int j97;
  float c;
  float cd;
  float cm;
  float u[97];
} maths_rstrf;

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct maths_rstr
   @brief A struct for the use in the implemented random number
   generator maths_rndm()

   Struct for the control of the implemented random generator in this
   module. This is the float version. Has to be allocated and then to
   be initialised with maths_rndmf_init(). After that, if passed to
   maths_rndmf() it serves to deliver quasirandom numbers.
*/
/* ------------------------------------------------------------ */

typedef struct maths_rstr
{
  int i97;
  int j97;
  double c;
  double cd;
  double cm;
  double u[97];
} maths_rstr;

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int roundnormal(float number)
    @brief Rounds number.
    
    @param number (float) The number to be rounded
    
    @return int roundnormal: The rounded number, no error handling
*/
/* ------------------------------------------------------------ */
int roundnormal(float number);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int roundup(float number)
    @brief Rounds number up.
    
    @param number (float) The number to be rounded
    
    @return int roundup: The rounded number, no error handling
*/
/* ------------------------------------------------------------ */
int roundup(float number);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int roundown(float number)
    @brief Rounds number down.
    
    @param number (float) The number to be rounded
    
    @return The rounded number, no error handling
*/
/* ------------------------------------------------------------ */
int roundown(float number);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn float modfupsq(float number)
    @brief Returns the difference to the next higher integer squared

    Very common problem that is needed everywhere. No-one knows why it
    is not included in the standard libraries. Well, nevertheless it
    is a stand-alone mathematical function and thus belongs here.

    @param number (float) The number

    @return float modfupsq: The difference to the next higher int
    squared (no error handling)
*/
/* ------------------------------------------------------------ */
float modfupsq(float number);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn float modfdownsq(float number)
    @brief Returns the difference to the next lower integer squared

    Very common problem that is needed everywhere. No-one knows why it
    is not included in the standard libraries. Well, nevertheless it
    is a stand-alone mathematical function and thus belongs here.

    @param number (float) The number

    @return float modfdownsq: The difference to the next higher int
    squared (no error handling)
*/
/* ------------------------------------------------------------ */
float modfdownsq(float number);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn float degreetoradian(float degrees)
  @brief Converts from degrees to radians

  Changes a number from deg to rad

  @param degrees (float) An angle in degrees

  @return float degreetoradian: Input angle in radians
*/
/* ------------------------------------------------------------ */
float degreetoradian(float degrees);




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn *sincosofangle(float angle)
  @brief Returns the sin and the cosine of an angle in an 
  allocated array

  Returns an allocated array containing the cos and the sin of an
  angle in degrees. The array has to be freed.

  @param angle (float) Angle in degrees

  @return (success) float *sincosofangle: An array with the sin and 
  the cos of the angle\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
float *sincosofangle(float angle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int *imax(float *borders)
  @brief Return an array of ints instead of floats, which are rounded up

  @param borders (float *) An array of 3 floating point numbers

  @return int *imax: The output array with ints
*/
/* ------------------------------------------------------------ */
int *imax(float *borders);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* 
   @fn void maths_rndm_init(int *seed, maths_rstr *out);
   @brief Initialises a random number object

   @param seed (int *)      Two component seed
   @param out  (maths_rstr *) An allocated maths_rstr object

   @return (success) maths_rstr *maths_rndm_init: Initialised random struct
           (error) NULL
*/
/* ------------------------------------------------------------ */
void maths_rndm_init(int *seed, maths_rstr *out);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* 
   @fn void maths_rndmf_init(int *seed, maths_rstrf *out);
   @brief Initialises a random number object

   NOTE: The seed variables can have all values, but
   internally only seed[0] = seed[0]%31328 and seed[0]%30081
   is used.

   @param seed (int *)      Two component seed
   @param out  (maths_rstr *) An allocated maths_rstr object

   @return (success) maths_rstr *maths_rndm_init: Initialised random struct
           (error) NULL
*/
/* ------------------------------------------------------------ */
void maths_rndmf_init(int *seed, maths_rstrf *out);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* 
   @fn double maths_rndm(maths_rstr *rnob);
   @brief Returns a random number in the range [0,1)

   Returns a random number and changes the rnob for the return of the
   next random number. The rnob has to be initialised with the
   function maths_rndm_init()

   @param rnob (maths_rstr *) A maths_rstr struct

   @return double random: A quasirandom number
*/
/* ------------------------------------------------------------ */
double maths_rndm(maths_rstr *rnob);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* 
   @fn float maths_rndmf(maths_rstrf *rnob);
   @brief Returns a random number in the range [0,1)

   Returns a random number and changes the rnob for the return of the
   next random number. The rnob has to be initialised with the
   function maths_rndm_init()

   @param rnob (maths_rstrf *) A maths_rstrf struct

   @return float random: A quasirandom number
*/
/* ------------------------------------------------------------ */
float maths_rndmf(maths_rstrf *rnob);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int maths_checkeq(double a, double b, double accuracy)
  @brief Checks whether the values are equal to a given accuracy

  @param a        (double) First number
  @param b        (double) Second number
  @param accuracy (double) Accuracy as double, e.g. 0.000001 for float accuracy

  @return int maths_checkeq: 0 if they are in each others range, 1 if they are not equal
*/
/* ------------------------------------------------------------ */
int maths_checkeq(double a, double b, double accuracy);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int maths_checkinbetw(double max, double min, double value)
  @brief Checks whether values is in-between max and min

  Min can be larger than max.

  @param a     (double) First number
  @param b     (double) Second number
  @param value (double) The value to check

  @return int maths_checkeq: 0 if it is in-between, 1 if not 
*/
/* ------------------------------------------------------------ */
int maths_checkinbetw(double max, double min, double value);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int maths_checkinbetwf(float max, float min, float value)
  @brief Checks whether values is in-between max and min

  Min can be larger than max.

  @param a     (float) First number
  @param b     (float) Second number
  @param value (float) The value to check

  @return int maths_checkeq: 1 if it is in-between, 0 if not 
*/
/* ------------------------------------------------------------ */
int maths_checkinbetwf(float max, float min, float value);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void maths_rotaz(double cosang, double sinang, double *xin, double *xout)
  @brief 3d active rotation about z-axis

  Calculates an active rotation about the z- axis:
  xout[0] = cosang*xin[0]-sinang*xin[1]+0
  xout[1] = sinang*xin[0]+cosang*xin[1]+0
  xout[2] = 0            +            0+z

  @param cosang (double)   cosine of the rotation angle
  @param sinang (double)   sine of the rotation angle
  @param xin    (double *) Allocated 3d vector to be rotated
  @param xout   (double *) Allocated 3d vector to put the result

  @return void
*/
/* ------------------------------------------------------------ */
void maths_rotaz(double cosang, double sinang, double *xin, double *xout);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void maths_rotax(double cosang, double sinang, double *xin, double *xout)
  @brief 3d active rotation about x-axis

  Calculates an active rotation about the x- axis:
  xout[0] = xin[0]+0            +0
  xout[1] = 0     +cosang*xin[1]-sinang*xin[2]
  xout[2] = 0     +sinang*xin[1]+cosang*xin[2]

  @param cosang (double)   cosine of the rotation angle
  @param sinang (double)   sine of the rotation angle
  @param xin    (double *) Allocated 3d vector to be rotated
  @param xout   (double *) Allocated 3d vector to put the result

  @return void
*/
/* ------------------------------------------------------------ */
void maths_rotax(double cosang, double sinang, double *xin, double *xout);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void maths_rotay(double cosang, double sinang, double *xin, double *xout)
  @brief 3d active rotation about y-axis

  Calculates an active rotation about the x- axis:
  xout[0] = cosang*xin[0]+0     -sinang*xin[2]
  xout[1] = 0            +xin[1]+0
  xout[2] = sinang*xin[0]+0     +cosang*xin[2]

  @param cosang (double)   cosine of the rotation angle
  @param sinang (double)   sine of the rotation angle
  @param xin    (double *) Allocated 3d vector to be rotated
  @param xout   (double *) Allocated 3d vector to put the result

  @return void
*/
/* ------------------------------------------------------------ */
void maths_rotay(double cosang, double sinang, double *xin, double *xout);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn double maths_cosn(int n, double cosang, double sinang)
  @brief cosine of n times an angle, for which cos and sin is given

  @param n      (int)      positive integer
  @param cosang (double)   cosine of the rotation angle
  @param sinang (double)   sine of the rotation angle

  @return double maths_cosn: cos(n*ang)
*/
/* ------------------------------------------------------------ */
double maths_cosn(int n, double cosang, double sinang);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn float maths_cosnf(int n, float cosang, float sinang)
  @brief cosine of n times an angle, for which cos and sin is given

  @param n      (int)      positive integer
  @param cosang (float)   cosine of the rotation angle
  @param sinang (float)   sine of the rotation angle

  @return float maths_cosnf: cos(n*ang)
*/
/* ------------------------------------------------------------ */
float maths_cosnf(int n, float cosang, float sinang);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn double maths_sinn(int n, double cosang, double sinang)
  @brief sine of n times an angle, for which cos and sin is given

  @param n      (int)      positive integer
  @param cosang (double)   cosine of the rotation angle
  @param sinang (double)   sine of the rotation angle

  @return double maths_sinn: cos(n*ang)
*/
/* ------------------------------------------------------------ */
double maths_sinn(int n, double cosang, double sinang);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn float maths_sinnf(int n, float cosang, float sinang)
  @brief sine of n times an angle, for which cos and sin is given

  @param n      (int)      positive integer
  @param cosang (float)   cosine of the rotation angle
  @param sinang (float)   sine of the rotation angle

  @return float maths_sinnf: cos(n*ang)
*/
/* ------------------------------------------------------------ */
float maths_sinnf(int n, float cosang, float sinang);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int maths_getint(const char *input, int *output)
  @brief Get integer from the input, using strtol

  @param input  (const char *) string to read
  @param output (int *)        output

  @return int maths_getint: (success) 0
                            (error) -1
*/
/* ------------------------------------------------------------ */
int maths_getint(const char *input, int *output);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn double maths_sin5(double cosang, double sinang)
  @brief sine of 5 times an angle, for which cos and sin is given

  @param cosang (double)   cosine of the angle
  @param sinang (double)   sine of the angle

  @return double maths_sinn: cos(5*angle)
*/
/* ------------------------------------------------------------ */
double maths_sin2(double cosang, double sinang);
double maths_sin3(double cosang, double sinang);
double maths_sin4(double cosang, double sinang);
double maths_sin5(double cosang, double sinang);
double maths_cos2(double cosang, double sinang);
double maths_cos3(double cosang, double sinang);
double maths_cos4(double cosang, double sinang);
double maths_cos5(double cosang, double sinang);
float maths_sin2f(float cosang, float sinang);
float maths_sin3f(float cosang, float sinang);
float maths_sin4f(float cosang, float sinang);
float maths_sin5f(float cosang, float sinang);
float maths_cos2f(float cosang, float sinang);
float maths_cos3f(float cosang, float sinang);
float maths_cos4f(float cosang, float sinang);
float maths_cos5f(float cosang, float sinang);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn float maths_intabsfou4(float a0, float a1, float p1, float a2, float p2,float a3, float p3, float a4, float p4)
  @brief Calculates the integral of the absolute value of a Fourier decomposition to 4th order

  solves the integral of
  |a0+a1*cos(1*(x-p1))+a2*cos(2*(x-p2))+a1*cos(3*(x-p3))+a1*cos(4*(x-p4))|
  with x ranging from 0 to 2 pi by using a Romberg sum. Error is 0.25,
  absolute value (this is a very specialised thing).

  @param float a0  0ht order Fourier coefficient
  @param float a1  1st order Fourier coefficient, amplitude
  @param float p1  1st order Fourier coefficient, phase    
  @param float a2  2nd order Fourier coefficient, amplitude
  @param float p2  2nd order Fourier coefficient, phase    
  @param float a3  3rd order Fourier coefficient, amplitude
  @param float p3  3rd order Fourier coefficient, phase    
  @param float a4  4th order Fourier coefficient, amplitude
  @param float p4  4th order Fourier coefficient, phase    

  @return float maths_intabsfou: the integral
*/
/* ------------------------------------------------------------ */
float maths_intabsfou4(float a0, float a1, float p1, float a2, float p2,float a3, float p3, float a4, float p4);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn double maths_smooth_step(double xstar, double xleng, double ystar, double yleng, double x)
  @brief Makes a smooth step

  This returns a value for x performing a smooth step (differentiable at all points). At values x < xstar the return value is equal to ystar. At values x > xstar+xleng the return value is equal to ystar+yleng. In-between, the function is a scaled version of -3/2*(1/3*x^3-x), the minimal polynomial, which can fulfil the criterion of being smooth (i.e., can have two derivatives, which can be 0 at the edges of the step range).

  @param xstar (double) start of step
  @param xleng (double) width of step
  @param ystar (double) start value at xstar
  @param yleng (double) amplitude of step
  @param x     (double) variable

  @return double maths_smooth_step()
*/
/* ------------------------------------------------------------ */
double maths_smooth_step(double xstar, double xleng, double ystar, double yleng, double x);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn float maths_smooth_stepf(float xstar, float xleng, float ystar, float yleng, float x)
  @brief Makes a smooth step

  This returns a value for x performing a smooth step (differentiable at all points). At values x < xstar the return value is equal to ystar. At values x > xstar+xleng the return value is equal to ystar+yleng. In-between, the function is a scaled version of -3/2*(1/3*x^3-x), the minimal polynomial, which can fulfil the criterion of being smooth (i.e., can have two derivatives, which can be 0 at the edges of the step range).

  @param xstar (float) start of step
  @param xleng (float) width of step
  @param ystar (float) start value at xstar
  @param yleng (float) amplitude of step
  @param x     (float) variable

  @return float maths_smooth_step()
*/
/* ------------------------------------------------------------ */
float maths_smooth_stepf(float xstar, float xleng, float ystar, float yleng, float x);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn double maths_hard_step(double xstar, double xleng, double ystar, double yleng, double x)
  @brief Makes a hard step

  This returns a value for x performing a hard step. At values x < xstar the return value is equal to ystar. At values x > xstar+xleng the return value is equal to ystar+yleng. In-between, the function is linearly interpolating between the two values.

  @param xstar (double) start of step
  @param xleng (double) width of step
  @param ystar (double) start value at xstar
  @param yleng (double) amplitude of step
  @param x     (double) variable

  @return double maths_hard_step()
*/
/* ------------------------------------------------------------ */
double maths_hard_step(double xstar, double xleng, double ystar, double yleng, double x);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn float maths_hard_stepf(float xstar, float xleng, float ystar, float yleng, float x)
  @brief Makes a hard step

  This returns a value for x performing a hard step. At values x < xstar the return value is equal to ystar. At values x > xstar+xleng the return value is equal to ystar+yleng. In-between, the function is linearly interpolating between the two values.

  @param xstar (float) start of step
  @param xleng (float) width of step
  @param ystar (float) start value at xstar
  @param yleng (float) amplitude of step
  @param x     (float) variable

  @return float maths_hard_step()
*/
/* ------------------------------------------------------------ */
float maths_hard_stepf(float xstar, float xleng, float ystar, float yleng, float x);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static void maths_bubble(double *array, long length)
  @brief Inefficient sorting of the input array

  @param array  (double *) Input array
  @param length (long)     Length of input array
  @return void
*/
/* ------------------------------------------------------------ */
void maths_bubble(double *array, long length);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static void maths_bubblei(int *array, long length)
  @brief Inefficient sorting of the input array

  @param array  (int *) Input array
  @param length (long)     Length of input array
  @return void
*/
/* ------------------------------------------------------------ */
void maths_bubblei(int *array, long length);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void testmaths(void)
  @brief testing

  @return void
*/
/* ------------------------------------------------------------ */
void testmaths(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: maths.h,v $
   Revision 1.13  2011/05/25 22:25:25  jozsa
   Left work

   Revision 1.12  2010/04/12 23:15:50  jozsa
   Left work

   Revision 1.11  2010/04/01 09:24:19  jozsa
   included and hopefully debugged: radial/vertical movement/gradients of those in z/azimuthal harmonics in velocity and surface brightness. To do 1) subclouds 2) Gaussian variations in azimuth 3) portions of a disk 4) 4 disks

   Revision 1.10  2009/05/27 15:07:37  jozsa
   Left work

   Revision 1.9  2009/05/26 07:56:39  jozsa
   Left work

   Revision 1.8  2007/08/22 15:58:35  gjozsa
   Left work

   Revision 1.7  2005/08/17 14:31:52  gjozsa
   Left work

   Revision 1.6  2005/05/24 15:58:32  gjozsa
   Added rotation

   Revision 1.5  2005/04/14 10:32:12  gjozsa
   Left work

   Revision 1.4  2005/03/08 17:55:04  gjozsa
   Left work

   Revision 1.3  2004/12/09 16:17:24  gjozsa
   Changed some floating point operations from double to float accuracy

   Revision 1.2  2004/11/09 15:58:38  gjozsa
   fixed minor bug

   Revision 1.1.1.1  2004/10/29 11:13:05  gjozsa
   Added to CVS control


   ------------------------------------------------------------ */

/* Include guard */
#endif

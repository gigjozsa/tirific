/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file maths.c
   @brief Mathelmatical functions that are not contained in the 
   standard library

   maths.c defines all mathematical functions that are not contained
   in math.h and binds the math.h functionality without interfacing
   with internal structures. To bind all generic mathematical
   functionality include this module.

   $Source: /Volumes/DATA_J_II/data/CVS/tirific/src/maths.c,v $
   $Date: 2011/05/25 22:25:26 $
   $Revision: 1.17 $
   $Author: jozsa $
   $Log: maths.c,v $
   Revision 1.17  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.16  2011/05/10 00:30:16  jozsa
   Left work

   Revision 1.15  2011/05/04 01:09:21  jozsa
   test

   Revision 1.14  2010/04/12 23:15:52  jozsa
   Left work

   Revision 1.13  2010/04/01 09:24:19  jozsa
   included and hopefully debugged: radial/vertical movement/gradients of those in z/azimuthal harmonics in velocity and surface brightness. To do 1) subclouds 2) Gaussian variations in azimuth 3) portions of a disk 4) 4 disks

   Revision 1.12  2009/05/27 15:07:38  jozsa
   Left work

   Revision 1.11  2008/07/30 16:31:55  jozsa
   A change in maths_checkeq() again

   Revision 1.10  2007/08/22 15:58:42  gjozsa
   Left work

   Revision 1.9  2006/12/11 12:41:39  gjozsa
   BUGFIX: Removed big error in maths_checkeq

   Revision 1.8  2006/11/03 10:52:55  gjozsa
   Corrected maths_checkeq

   Revision 1.7  2006/11/03 10:49:20  gjozsa
   Introduced logarithmic scaling, hms dms

   Revision 1.6  2005/08/17 14:31:55  gjozsa
   Left work

   Revision 1.5  2005/05/24 15:58:31  gjozsa
   Added rotation

   Revision 1.4  2005/04/14 10:32:15  gjozsa
   Left work

   Revision 1.3  2005/03/08 17:55:07  gjozsa
   Left work

   Revision 1.2  2004/12/09 16:17:14  gjozsa
   Changed some floating point operations from double to float accuracy

   Revision 1.1.1.1  2004/10/29 11:13:20  gjozsa
   Added to CVS control


*/
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <maths.h>
#include <stdio.h>
#include <stdlib.h>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def _MEMORY_HERE_ON
   @brief Controls the use of the memory_here module

   If you don't want to use the memory_here facility comment this
   define, otherways it will be included.

*/
/* ------------------------------------------------------------ */
/* #define _MEMORY_HERE_ON */
/* #include <memory_here.h> */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */
#ifndef TWOPI_HERE
#define TWOPI_HERE 6.283185307179586231995926937088370323181152343750
#endif

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE MACROS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* (PRIVATE) GLOBAL VARIABLES */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE TYPEDEFS */
/* ------------------------------------------------------------ */




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE STRUCTS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int factori(int n)
  @brief factorial

  @param n (int)   integer > 0

  @return factorial
*/
/* ------------------------------------------------------------ */
static int factori(int n);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int aoverb(int n, int m)
  @brief binomial coefficient

  @param n (int)   integer > 0
  @param m (int)   integer > 0

  @return int aoverb n
                     m
*/
/* ------------------------------------------------------------ */
static int aoverb(int n, int m);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static double powint(double a, int n)
  @brief integer power

  @param a (double)   basis
  @param n (int)      exponent

  @return double powint a^n
*/
/* ------------------------------------------------------------ */
static double powint(double a, int n);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static float powintf(float a, int n)
  @brief integer power

  @param a (float)    basis
  @param n (int)      exponent

  @return float powint a^n
*/
/* ------------------------------------------------------------ */
static float powintf(float a, int n);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn float absfou4(float x, float a0, float a1, float p1, float a2, float p2,float a3, float p3, float a4, float p4)
  @brief Calculates the absolute value of a Fourier decomposition to 4th order

  returns |a0+a1*cos(1*(x-p1))+a2*cos(2*(x-p2))+a1*cos(3*(x-p3))+a1*cos(4*(x-p4))|

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
static float absfou4(float x, float a0, float a1, float p1, float a2, float p2,float a3, float p3, float a4, float p4);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION CODE */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Round a number */

int roundnormal(float number) 
{
  float ip;
  if (number > 0){
    if ((modff(number, &ip)) >= 0.5)
      return (int) ip+1;
    else
      return (int) ip;
  }
  else { 
    if ((modff(number, &ip)) <= -0.5)
      return (int) ip-1;
    else
      return (int) ip;
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Round a number up */

int roundup(float number)
{
  float ip;

  if (modff(number, &ip) == 0.0)
    return (int) ip;
  else if (ip < 0.0)
    return (int) ip;
  else
    return (int) ip+1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Round a number down */

int roundown(float number)
{
  float ip;
  if (modff(number, &ip) == 0.0)
    return (int) ip;
  else if (ip < 0.0)
    return (int) ip-1;
  else 
    return ip;
}

/* ------------------------------------------------------------ */

/* Return the difference to the next higher integer squared */

float modfupsq(float number)
{
  float dummy;
  if (number < 0.0) 

    /* hoping that the author of pow is not an idiot */
    return powf(modff(number, &dummy), 2);
  else {
    if ((dummy = modff(number, &dummy)) == 0.0)
      return (dummy);
    else 
      return powf(1.0-dummy, 2);
  }
}



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Return the difference to the next lower integer squared */

float modfdownsq(float number)
{
  float dummy;
  if (number >= 0.0) 

    /* hoping that the author of pow is not an idiot */
    return powf(modff(number, &dummy), 2);
  else {
    if ((dummy = modff(number, &dummy)) == 0.0)
      return dummy;
    else 
      return powf(1.0+dummy, 2);
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Translate from degree to radians */

float degreetoradian(float degrees)
{
  return PI_HERE*degrees/180.0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */



/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns the sin and the cosine of an angle in an allocated array */

float *sincosofangle(float angle)
{
  float *cossinofangle;
    if ((cossinofangle = (float *) malloc(2*sizeof(float)))) {
      *cossinofangle = sinf(degreetoradian(angle));
      *(cossinofangle+1) = cosf(degreetoradian(angle));
    }
  return cossinofangle;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Return an array of ints instead of floats, which are rounded up */

int *imax(float *borders)
{
  int *imax;
    if ((imax = (int *) malloc(3*sizeof(int)))) {
  
      /* The maximum extent to evaluate the gaussian */
      imax[0] = roundup(borders[0]);
      imax[1] = roundup(borders[1]);
      imax[2] = roundup(borders[2]);
    }
  return imax;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Checks whether the values are equal to a given accuracy */
int maths_checkeq(double a, double b, double accuracy) 
{
  double sc;

  sc = fabs(a)>fabs(b)?fabs(a):fabs(b);

  if (sc <= accuracy) {
    if (fabs(a-b) > accuracy)
      return 1;
    else
      return 0;
  }
  else {
    if (fabs((a-b)/sc) > accuracy)
      return 1;
    else
      return 0;
  }

}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Checks whether values is in-between max and min */
int maths_checkinbetw(double max, double min, double value)
{
  double max_here;

  if (min > max) {
    max_here = min;
    min = max;
  }
  else
    max_here = max;

  if (value <= max_here && value >= min)
    return 0;

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Checks whether values is in-between max and min */
int maths_checkinbetwf(float max, float min, float value)
{
  float max_here;

  if (min > max) {
    max_here = min;
    min = max;
  }
  else
    max_here = max;

  if (value <= max_here && value >= min)
    return 1;

  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Initialises a random number object */
void maths_rndm_init(int *seed, maths_rstr *out)
{
  double s,t;
  int ii,i,j,k,l,jj,m;

  i = (seed[0]%31328)/177%177+2;
  j = (seed[0]%31328)%177+2;
  k = (seed[1]%30081)/169%178+1;
  l = (seed[1]%30081)%169;

  for (ii = 0; ii < 97; ++ii) {
    s = 0;
    t = 0.5;
    for(jj = 0; jj <= 23; ++jj) {
      m = (i*j%179*k)%179;
      i = j;
      j = k;
      k = m;
      l = (53*l+1)%169;
      if (l*m%64 >= 32) 
 s = s+t;
      t = t*0.5;
        }
    out -> u[ii] = s;
  }

  out -> c = 362436;
  out -> cd = 7654321;
  out -> cm = 16777213;
  out -> c = out -> c/16777216;
  out -> cd = out -> cd/16777216;
  out -> cm = out -> cm/16777216;
  out -> i97 = 97;
  out -> j97 = 33;

  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Initialises a random number object */
void maths_rndmf_init(int *seed, maths_rstrf *out)
{
  float s,t;
  int ii,i,j,k,l,jj,m;

  i = (seed[0]%31328)/177%177+2;
  j = (seed[0]%31328)%177+2;
  k = (seed[1]%30081)/169%178+1;
  l = (seed[1]%30081)%169;

  for (ii = 0; ii < 97; ++ii) {
    s = 0;
    t = 0.5;
    for(jj = 0; jj <= 23; ++jj) {
      m = (i*j%179*k)%179;
      i = j;
      j = k;
      k = m;
      l = (53*l+1)%169;
      if (l*m%64 >= 32) 
 s = s+t;
      t = t*0.5;
        }
    out -> u[ii] = s;
  }

  out -> c = 362436;
  out -> cd = 7654321;
  out -> cm = 16777213;
  out -> c = out -> c/16777216;
  out -> cd = out -> cd/16777216;
  out -> cm = out -> cm/16777216;
  out -> i97 = 97;
  out -> j97 = 33;

  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns a random number in the range [0,1) */
double maths_rndm(maths_rstr *rnob)
{
  double random;
  random = rnob -> u[rnob -> i97-1] - rnob -> u[rnob -> j97-1];
  if (random <= 0) 
    random = random + 1.0;

  rnob -> u[rnob -> i97-1] = random;
  rnob -> i97 = rnob -> i97-1;
  if (!(rnob -> i97))
    rnob -> i97 = 97;
  
  rnob -> j97 = rnob -> j97-1;
  if (!(rnob -> j97))
    rnob -> j97 = 97;

  rnob -> c = rnob -> c-rnob -> cd;
  if (rnob -> c < 0)
    rnob -> c = rnob -> c+rnob -> cm;

  random = random-rnob -> c;
  if (random < 0) 
    random = random+1;

  return random;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns a random number in the range [0,1) */
float maths_rndmf(maths_rstrf *rnob)
{
  float random;
  random = rnob -> u[rnob -> i97-1] - rnob -> u[rnob -> j97-1];
  if (random <= 0) 
    random = random + 1.0;

  rnob -> u[rnob -> i97-1] = random;
  rnob -> i97 = rnob -> i97-1;
  if (!(rnob -> i97))
    rnob -> i97 = 97;
  
  rnob -> j97 = rnob -> j97-1;
  if (!(rnob -> j97))
    rnob -> j97 = 97;

  rnob -> c = rnob -> c-rnob -> cd;
  if (rnob -> c < 0)
    rnob -> c = rnob -> c+rnob -> cm;

  random = random-rnob -> c;
  if (random < 0) 
    random = random+1;

  return random;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* 3d active rotation about x-axis */
void maths_rotax(double cosang, double sinang, double *xin, double *xout)
{
  xout[0] = xin[0];
  xout[1] = cosang*xin[1]-sinang*xin[2];
  xout[2] = sinang*xin[1]+cosang*xin[2];

  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* 3d active rotation about x-axis */
void maths_rotay(double cosang, double sinang, double *xin, double *xout)
{
  xout[1] = xin[1];
  xout[0] = cosang*xin[0]-sinang*xin[2];
  xout[2] = sinang*xin[0]+cosang*xin[2];

  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* 3d active rotation about z-axis */
void maths_rotaz(double cosang, double sinang, double *xin, double *xout)
{
  xout[2] = xin[2];
  xout[0] = cosang*xin[0]-sinang*xin[1];
  xout[1] = sinang*xin[0]+cosang*xin[1];

  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* factorial */
static int factori(int n)
{
  int i = 1;
  while(n) {
    i=i*n;
    --n;
  }
  return i;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* binomial coefficient */
static int aoverb(int n, int m)
{
  return factori(n)/(factori(m)*factori(n-m));
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* cosine of n times an angle, for which cos and sin is given */
double maths_cosn(int n, double cosang, double sinang)
{
  int i,m,k=-1;
  double maths_cosn = 0.0;

  m = n/2+1;

  for (i=0; i < m; ++i) 
    maths_cosn = maths_cosn+(k=-k)*aoverb(n,2*i)*powint(sinang,2*i)*powint(cosang, n-2*i);
  
  return maths_cosn;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* cosine of n times an angle, for which cos and sin is given */
float maths_cosnf(int n, float cosang, float sinang)
{
  int i,m,k=-1;
  float maths_cosnf = 0.0;

  m = n/2+1;

  for (i=0; i < m; ++i) 
    maths_cosnf = maths_cosnf+(k=-k)*aoverb(n,2*i)*powintf(sinang,2*i)*powintf(cosang, n-2*i);
  
  return maths_cosnf;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* sine of n times an angle, for which cos and sin is given */
float maths_sinnf(int n, float cosang, float sinang)
{
  int i, m, k = -1;
  float maths_sinnf = 0.0;

  m = (n+1)/2+1;

  for (i = 1; i < m; ++i) 
    maths_sinnf = maths_sinnf+(k=-k)*aoverb(n, 2*i-1)*powintf(sinang, 2*i-1)*powintf(cosang, n-2*i+1);
  
  return maths_sinnf;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* sine of n times an angle, for which cos and sin is given */
double maths_sinn(int n, double cosang, double sinang)
{
  int i, m, k = -1;
  double maths_sinn = 0.0;

  m = (n+1)/2+1;

  for (i = 1; i < m; ++i) 
    maths_sinn = maths_sinn+ (k = -k)*aoverb(n, 2*i-1)*powint(sinang, 2*i-1)*powint(cosang, n-2*i+1);
  
  return maths_sinn;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* sine of 2 times an angle, for which cos and sin is given */
double maths_sin2(double cosang, double sinang)
{
  return 2.0*sinang*cosang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* sine of 3 times an angle, for which cos and sin is given */
double maths_sin3(double cosang, double sinang)
{
  return 3.0*sinang-4.0*sinang*sinang*sinang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* sine of 4 times an angle, for which cos and sin is given */
double maths_sin4(double cosang, double sinang)
{
  return 8.0*sinang*cosang*cosang*cosang-4.0*sinang*cosang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* sine of 5 times an angle, for which cos and sin is given */
double maths_sin5(double cosang, double sinang)

{
  return 5.0*sinang-20.0*sinang*sinang*sinang+16.0*sinang*sinang*sinang*sinang*sinang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* cosine of 2 times an angle, for which cos and sin is given */
double maths_cos2(double cosang, double sinang)
{
  return 1.0-2.0*sinang*sinang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* cosine of 3 times an angle, for which cos and sin is given */
double maths_cos3(double cosang, double sinang)
{
  return 4.0*cosang*cosang*cosang-3.0*cosang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* cosine of 4 times an angle, for which cos and sin is given */
double maths_cos4(double cosang, double sinang)
{
  return 8.0*cosang*cosang*cosang*cosang-8.0*cosang*cosang+1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* cosine of 5 times an angle, for which cos and sin is given */
double maths_cos5(double cosang, double sinang)
{
  return 16.0*cosang*cosang*cosang*cosang*cosang-20.0*cosang*cosang*cosang+5.0*cosang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* sine of 2 times an angle, for which cos and sin is given */
float maths_sin2f(float cosang, float sinang)
{
  return 2.0*sinang*cosang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* sine of 3 times an angle, for which cos and sin is given */
float maths_sin3f(float cosang, float sinang)
{
  return 3.0*sinang-4.0*sinang*sinang*sinang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* sine of 4 times an angle, for which cos and sin is given */
float maths_sin4f(float cosang, float sinang)
{
  return 8.0*sinang*cosang*cosang*cosang-4.0*sinang*cosang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* sine of 5 times an angle, for which cos and sin is given */
float maths_sin5f(float cosang, float sinang)

{
  return 5.0*sinang-20.0*sinang*sinang*sinang+16.0*sinang*sinang*sinang*sinang*sinang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* cosine of 2 times an angle, for which cos and sin is given */
float maths_cos2f(float cosang, float sinang)
{
return 1.0-2.0*sinang*sinang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* cosine of 3 times an angle, for which cos and sin is given */
float maths_cos3f(float cosang, float sinang)
{
  return 4.0*cosang*cosang*cosang-3.0*cosang;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* cosine of 4 times an angle, for which cos and sin is given */
float maths_cos4f(float cosang, float sinang)
{
  return 8.0*cosang*cosang*cosang*cosang-8.0*cosang*cosang+1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* cosine of 5 times an angle, for which cos and sin is given */
float maths_cos5f(float cosang, float sinang)
{
  return 16.0*cosang*cosang*cosang*cosang*cosang-20.0*cosang*cosang*cosang+5.0*cosang;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* integer power */
static double powint(double a, int n)
{
  double powint=1.0;
  while (n) {
    powint = powint*a;
    --n;
  }
  return powint;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* integer power */
static float powintf(float a, int n)
{
  float powintf = 1.0;
  while (n) {
    powintf = powintf*a;
    --n;
  }
  return powintf;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* integer power */
float maths_intabsfou4(float a0, float a1, float p1, float a2, float p2,float a3, float p3, float a4, float p4)
{
    int i, j, denom;
  float t[21][21], width, est, power, sum;
  /* First do an "integral" using trapezoidal rule */
  t[1][1] = TWOPI_HERE*(absfou4(TWOPI_HERE, a0, a1, p1, a2, p2, a3, p3, a4, p4));

  width = PI_HERE;

  t[2][1] = 0.5*t[1][1]+width*absfou4(width, a0, a1, p1, a2, p2, a3, p3, a4, p4);
  t[2][2] = (4.0*t[2][1]-t[1][1])/3.0;
  est = fabsf (t[2][2]-t[1][1])/ 2.0;
     
  denom = 2;
  i = 2;

  while ( (est > 0.1 && i < 22) || i < 4) {
    ++i;

    width = 0.5*width;
    sum = 0.0;

    for ( j = 1; j < 2*denom; j = j+2)
      sum = sum + absfou4(j*width,a0,a1,p1,a2,p2,a3,p3,a4,p4);
    t[i][1] = 0.5*t[i-1][1]+width*sum;
    
    power = 1.0;
    for ( j = 2; j <= i; j++ ) {
      power = 4.0*power;
      t[i][j] = (power*t[i][j-1]-t[i-1][j-1])/(power-1.0);
    }
    denom = 2*denom;
    est = fabs(t[i][i]-t[i-1][i-1])/((double) denom);
  }

  return t[i][i];

}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* integer power */
static float absfou4(float x, float a0, float a1, float p1, float a2, float p2,float a3, float p3, float a4, float p4)
{
  return fabsf(a0+a1*cosf(x-p1)+a2*cosf(2.0*(x-p2))+a3*cosf(3.0*(x-p3))+a4*cosf(4.0*(x-p4)));
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Makes a smooth step */
double maths_smooth_step(double xstar, double xleng, double ystar, double yleng, double x)
{
  double dxn, xn, dyn;

  if (x <= xstar) {
    return ystar;
  }
  else if (x >= (xstar+xleng)) {
    return ystar+yleng;
  }
  else {

  dxn = xleng/2.0;
  dyn = yleng/2.0;

  xn = (x-dxn-xstar)/dxn;

/*   return -1.5*(xn*xn*xn/3.0-xn)*dyn+ystar+dyn; */
  return -0.5*(xn*xn*xn-3.0*xn)*dyn+ystar+dyn;
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Makes a smooth step */
float maths_smooth_stepf(float xstar, float xleng, float ystar, float yleng, float x)
{
  float dxn, xn, dyn;

  if (x <= xstar) {
    return ystar;
  }
  else if (x >= (xstar+xleng)) {
    return ystar+yleng;
  }
  else {

  dxn = xleng/2.0;
  dyn = yleng/2.0;

  xn = (x-dxn-xstar)/dxn;

/*   return -1.5*(xn*xn*xn/3.0-xn)*dyn+ystar+dyn; */
  return -0.5*(xn*xn*xn-3.0*xn)*dyn+ystar+dyn;
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Makes a hard step */
double maths_hard_step(double xstar, double xleng, double ystar, double yleng, double x)
{
  if (x < xstar) {
    return ystar;
  }
  else if (x >= (xstar+xleng)) {
    return ystar+yleng;
  }
  else {
    return ystar+yleng*(x-xstar)/xleng;
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Makes a smooth step */
float maths_hard_stepf(float xstar, float xleng, float ystar, float yleng, float x)
{
  if (x <= xstar) {
    return ystar;
  }
  else if (x >= (xstar+xleng)) {
    return ystar+yleng;
  }
  else {
    return ystar+yleng*(x-xstar)/xleng;
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* testing */
void testmaths(void)
{
  double alpha,sinalpha,cosalpha;
  float  alphaf,sinalphaf,cosalphaf;
  int i;

  printf("fact: %i\n",aoverb(7,7));

  alpha=3.14*57.0/180.0;
  alphaf=3.14*57.0/180.0;

  sinalpha = sin(alpha);
  cosalpha = cos(alpha);
  sinalphaf = sinf(alphaf);
  cosalphaf = cosf(alphaf);

  printf("sin(2 alpha): %f %f\n",sin(2*alpha),maths_sin2(cosalpha,sinalpha));
  printf("sin(3 alpha): %f %f\n",sin(3*alpha),maths_sin3(cosalpha,sinalpha));
  printf("sin(4 alpha): %f %f\n",sin(4*alpha),maths_sin4(cosalpha,sinalpha));
  printf("sin(5 alpha): %f %f\n",sin(5*alpha),maths_sin5(cosalpha,sinalpha));
  printf("cos(2 alpha): %f %f\n",cos(2*alpha),maths_cos2(cosalpha,sinalpha));
  printf("cos(3 alpha): %f %f\n",cos(3*alpha),maths_cos3(cosalpha,sinalpha));
  printf("cos(4 alpha): %f %f\n",cos(4*alpha),maths_cos4(cosalpha,sinalpha));
  printf("cos(5 alpha): %f %f\n",cos(5*alpha),maths_cos5(cosalpha,sinalpha));

  printf("sinf(2 alpha): %f %f\n",sinf(2*alpha),maths_sin2f(cosalphaf,sinalphaf));
  printf("sinf(3 alpha): %f %f\n",sinf(3*alpha),maths_sin3f(cosalphaf,sinalphaf));
  printf("sinf(4 alpha): %f %f\n",sinf(4*alpha),maths_sin4f(cosalphaf,sinalphaf));
  printf("sinf(5 alpha): %f %f\n",sinf(5*alpha),maths_sin5f(cosalphaf,sinalphaf));
  printf("cosf(2 alpha): %f %f\n",cosf(2*alpha),maths_cos2f(cosalphaf,sinalphaf));
  printf("cosf(3 alpha): %f %f\n",cosf(3*alpha),maths_cos3f(cosalphaf,sinalphaf));
  printf("cosf(4 alpha): %f %f\n",cosf(4*alpha),maths_cos4f(cosalphaf,sinalphaf));
  printf("cosf(5 alpha): %f %f\n",cosf(5*alpha),maths_cos5f(cosalphaf,sinalphaf));

  i=0;
  fprintf(stderr,"normal\n");
  while (i<100000000) {
    /* dummy = cos(alpha+0.1*i); */
    ++i;
  }
  fprintf(stderr,"alternative 1\n");
  i=0;
  while (i<1000000000) {
    /* dummy = 1.0-2.0*sinalpha*sinalpha; */
/* maths_cos2(cosalpha, sinalpha); */
    ++i;
  }
  fprintf(stderr,"alternative 2\n");
  i=0;
  while (i<1000000000) {
    /* dummy = maths_cos2(cosalpha, sinalpha); */
    ++i;
  }
  fprintf(stderr,"alternative 3\n");
  i=0;
  while (i<1000000000) {
    /* dummy = maths_cosn(2,cosalpha, sinalpha); */
    ++i;
  }
  printf("end\n");


  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Inefficiently sorts the input arry */
void maths_bubble(double *array, long length)
{
  char satisfied = 1;
  double holder;
  long i;
  while (satisfied) {
    satisfied = 0;
    for (i = 0; i < length - 1; ++i) {
      if (array[i] > array[i+1]) {
 holder = array[i+1];
 array[i+1] = array[i];
 array[i] = holder;
 satisfied = 1;
      }
    }
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Inefficiently sorts the input arry */
void maths_bubblei(int *array, long length)
{
  char satisfied = 1;
  int holder;
  long i;
  while (satisfied) {
    satisfied = 0;
    for (i = 0; i < length - 1; ++i) {
      if (array[i] > array[i+1]) {
 holder = array[i+1];
 array[i+1] = array[i];
 array[i] = holder;
 satisfied = 1;
      }
    }
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: maths.c,v $
   Revision 1.17  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.16  2011/05/10 00:30:16  jozsa
   Left work

   Revision 1.15  2011/05/04 01:09:21  jozsa
   test

   Revision 1.14  2010/04/12 23:15:52  jozsa
   Left work

   Revision 1.13  2010/04/01 09:24:19  jozsa
   included and hopefully debugged: radial/vertical movement/gradients of those in z/azimuthal harmonics in velocity and surface brightness. To do 1) subclouds 2) Gaussian variations in azimuth 3) portions of a disk 4) 4 disks

   Revision 1.12  2009/05/27 15:07:38  jozsa
   Left work

   Revision 1.11  2008/07/30 16:31:55  jozsa
   A change in maths_checkeq() again

   Revision 1.10  2007/08/22 15:58:42  gjozsa
   Left work

   Revision 1.9  2006/12/11 12:41:39  gjozsa
   BUGFIX: Removed big error in maths_checkeq

   Revision 1.8  2006/11/03 10:52:55  gjozsa
   Corrected maths_checkeq

   Revision 1.7  2006/11/03 10:49:20  gjozsa
   Introduced logarithmic scaling, hms dms

   Revision 1.6  2005/08/17 14:31:55  gjozsa
   Left work

   Revision 1.5  2005/05/24 15:58:31  gjozsa
   Added rotation

   Revision 1.4  2005/04/14 10:32:15  gjozsa
   Left work

   Revision 1.3  2005/03/08 17:55:07  gjozsa
   Left work

   Revision 1.2  2004/12/09 16:17:14  gjozsa
   Changed some doubleing point operations from double to float accuracy

   Revision 1.1.1.1  2004/10/29 11:13:20  gjozsa
   Added to CVS control


   ------------------------------------------------------------ */

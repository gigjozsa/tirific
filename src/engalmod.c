/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file engalmod.c 
   @brief c-routines for function calls from Fortran in an enhanced
   galmod environment
   
   This module contains two functions that provide a very specialised
   chisquare evaluation when comparing an observed cube with a cube
   that is passed by galmod of the GIPSY environment. The c functions
   are meant for a hack into galmod and therefore have as such no
   use. There is one initialiser routine that initialises the
   functionality, i.e., allocates memory, when needed and safes
   variables that don't change if a lot of models are passed to the
   second routine the chisquare evaluation routine. This does nothing
   but give back a chisquare when comparing the original cube and the
   model. Both are passed as float arrays only. The module will be
   compiled as a library, so that only the library libengalmod.a will
   be needed except this include file.
   This is the first stable version.

   @todo A lot of optimisation: As the array size doesn't change
   calculate the fftw_plans ONCE excessively and safe them to global
   variables. Make an array of the parts of the gaussian beam that are
   redundant (in the hope that it makes things faster). It seems that
   a division is a long process: think about it. Test whether
   powf(x,2) is faster than x*x (should be...).
   
   $Source: /Volumes/DATA_J_II/data/CVS/tirific/src/engalmod.c,v $
   $Date: 2011/05/25 22:25:26 $
   $Revision: 1.44 $
   $Author: jozsa $
   $Log: engalmod.c,v $
   Revision 1.44  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.43  2011/05/11 13:37:12  jozsa
   Left work

   Revision 1.42  2011/05/10 00:30:15  jozsa
   Left work

   Revision 1.41  2009/05/26 07:56:40  jozsa
   Left work

   Revision 1.40  2007/08/22 15:58:40  gjozsa
   Left work

   Revision 1.39  2006/04/07 11:13:32  gjozsa
   simple BUGFIX

   Revision 1.38  2006/04/06 10:39:09  gjozsa
   Included function engalmod_chflgs

   Revision 1.37  2006/04/03 11:47:46  gjozsa
   included masking, fixed a mask to be present if pixval < -1024

   Revision 1.36  2005/04/20 13:26:24  gjozsa
   Left work

   Revision 1.35  2005/04/12 15:52:14  gjozsa
   Left work

   Revision 1.34  2005/04/07 12:45:47  gjozsa
   Bugfix

   Revision 1.33  2005/04/06 05:58:24  gjozsa
   Bugfix: init now corrects the noiseweight to 1 in case of mode%2

   Revision 1.32  2005/04/04 08:42:19  gjozsa
   Left work

   Revision 1.31  2005/04/01 12:37:11  gjozsa
   Large improvements, repeated calls with same velocity dispersion are much faster

   Revision 1.29  2005/03/11 17:45:54  gjozsa
   Left work

   Revision 1.28  2005/03/04 18:13:53  gjozsa
   Left work

   Revision 1.27  2005/03/02 17:56:09  gjozsa
   Left work

   Revision 1.26  2005/01/17 12:13:34  gjozsa
   Left work

   Revision 1.25  2005/01/06 10:44:10  gjozsa
   Left work

   Revision 1.24  2005/01/05 15:33:02  gjozsa
   Left work

   Revision 1.23  2004/12/30 13:36:05  gjozsa
   Added probability evaluation and out-of-place fft

   Revision 1.22  2004/12/27 12:54:40  gjozsa
   Last updatde before commenting, no changes anymore allowed

   Revision 1.21  2004/12/23 20:20:50  gjozsa
   some minor changes, leaves the implementation of arbitrary arrays

   Revision 1.18  2004/12/22 17:33:57  gjozsa
   Left work

   Revision 1.14  2004/12/21 18:42:12  gjozsa
   Left work

   Revision 1.10  2004/12/21 17:50:21  gjozsa
   some changes

   Revision 1.7  2004/12/20 14:55:58  gjozsa
   Left work

   Revision 1.5  2004/12/20 10:44:12  gjozsa
   added

   Revision 1.4  2004/12/17 14:13:40  gjozsa
   First debugged running version

   Revision 1.3  2004/12/16 13:19:51  gjozsa
   Left work

   Revision 1.1  2004/12/11 17:44:51  gjozsa
   Added to CVS control

   
*/
/* ------------------------------------------------------------ */
/* void fftw_execute_dft_r2c(
     const fftw_plan p,
     double *in, fftw_complex *out);
void fftw_execute_dft_c2r(
     const fftw_plan p,
     fftw_complex *in, double *out);
check whether FFTW_UNALIGNED is necessary as a planner flag
or use fftw_malloc and fftw_free in this module for the arrays betweeen which the fft takes place.
*/

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <stdio.h>
#include <stdlib.h>
#include <math.h> 
#include <fftw3.h>

#ifndef OPENMPTIR
  #undef OPENMPFFT
#endif
#ifdef OPENMPFFT
  #undef OPENMPTIR
#endif
#ifdef OPENMPTIR
  #define OPENMPFFT
  #include <omp.h>
#endif
/* #include <gsl/gsl_randist.h> */
/* #include <gsl/gsl_cdf.h> */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <engalmod.h>

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
/* (PRIVATE) SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */
#define PI_HERE 3.141592653589793115997963468544185161590576171875
#define SQRTPI 1.772453850905516
#define SQRTOF2 0.70710678118655

/* This is -1024 */
#define HOT_VALUE -1024


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* STRUCTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* (PRIVATE) GLOBAL VARIABLES */
/* ------------------------------------------------------------ */
static float *expofacsfft_;
static float *expofacsfft_noise_;
static float *veloarray_;
static float *veloarray_noise_;

static float sigma_maj_;
static float sigma_min_;
static float sigma_maj_noise_;
static float sigma_min_noise_;

static Cube original_;
static Cube model_;
static Cube noise_;
static Cube expcube_model_;
static Cube expcube_noise_;

static double *chisquare_;

static fftwf_complex *transformed_cube_model_;
static fftwf_complex *transformed_cube_noise_;

static fftwf_plan plan_noise_, plin_noise_;
static fftwf_plan plan_model_, plin_model_;

static int cubesizexhalf_;
static int cubesizeyhalf_;
static int newsize_;
static int dummy_;

static Cube *(*conmodel_)(void);
static Cube *(*connoise_)(void);
static double (*fetchchisquare_)(void);

static float noiseconstant_1_;
static float noiseconstant_2_;
static float modelconstant_1_;

static int realorigsizex_;
static int realorigsizey_;
static int realmodelsizex_;
static int realmodelsizey_;

static float oldsigma_;

#ifdef OPENMPTIR
#include <omp.h>
#endif
static int threads_;
static double *vector_;

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static double fetchchisquare_unflagged(void)
  @brief Get the chisquare without taking care of flags

  Returns the chisquare without taking care of flags. This function
  will be assigned to the pointer of fetchchisquare if no blanked
  pixels are found in the cube.

  @return double fetchchisquare_unflagged the chisquared
*/
/* ------------------------------------------------------------ */
static double fetchchisquare_unflagged(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static double fetchchisquare_flagged(void)
  @brief Get the chisquare taking care of flags

  Returns the chisquare taking care of flags. This function
  will be assigned to the pointer of fetchchisquare if any blanked
  pixel is found in the cube.

  @return double fetchchisquare_unflagged the chisquared
*/
/* ------------------------------------------------------------ */
static double fetchchisquare_flagged(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static float fftgaussian (int nx, int ny, int nv, float *expofacs)
  @brief Calculate a gaussian

  Returns the value of a gaussian in dependence of expofacs. 
  gaussian  =  exp((expofacs[0]*nx*nx+expofacs[1]*nx*ny+expofacs[2]*
  ny*ny+expofacs[3]*nv*nv+expofacs[4])).
  To be used by convolgaussfft. No modulation with respect of the
  signum of the coordinates will be done. For a number in the exponent
  lesser than MINEXPONENT the return value is 0.

  @todo Implement the last thing in the description

  @param nx       (int)     Relative pixelposition in x
  @param ny       (int)     Relative pixelposition in x
  @param nv       (int)     Relative pixelposition in x
  @param expofacs (float *) Factors in the gaussian, calculated by 
  expofacsfft and normalised with respect to the sizes of the array

  @return float fftgaussian The gaussian at the desired position
*/
/* ------------------------------------------------------------ */
static float fftgaussian (int nx, int ny, int nv, float *expofacs, float *veloarray);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static float fftgaussian (int nx, int ny, int nv, float *expofacs, float *veloarray)
  @brief Calculate a gaussian

  Returns the value of a gaussian in dependence of expofacs. 
  gaussian  =  exp((expofacs[0]*nx*nx+expofacs[1]*nx*ny+expofacs[2]*
  ny*ny+expofacs[3]*nv*nv+expofacs[4])).
  To be used by convolgaussfft. No modulation with respect of the
  signum of the coordinates will be done. For a number in the exponent
  lesser than MINEXPONENT the return value is 0.

  @todo Implement the last thing in the description

  @param nx       (int)     Relative pixelposition in x
  @param ny       (int)     Relative pixelposition in x
  @param nv       (int)     Relative pixelposition in x
  @param expofacs (float *) Factors in the gaussian, calculated by 
  expofacsfft and normalised with respect to the sizes of the array

  @return float fftgaussian The gaussian at the desired position
*/
/* ------------------------------------------------------------ */
static float fftgaussian (int nx, int ny, int nv, float *expofacs, float *veloarray);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static float fftgaussian (int nx, int ny, int nv, float *expofacs)
  @brief Calculate a gaussian

  Returns the value of a gaussian in dependence of expofacs. 
  gaussian  =  exp((expofacs[0]*nx*nx+expofacs[1]*nx*ny+expofacs[2]*
  ny*ny+expofacs[3]*nv*nv+expofacs[4])).
  To be used by convolgaussfft. No modulation with respect of the
  signum of the coordinates will be done. For a number in the exponent
  lesser than MINEXPONENT the return value is 0.

  @todo Implement the last thing in the description

  @param nx       (int)     Relative pixelposition in x
  @param ny       (int)     Relative pixelposition in x
  @param nv       (int)     Relative pixelposition in x
  @param expofacs (float *) Factors in the gaussian, calculated by 
  expofacsfft and normalised with respect to the sizes of the array

  @return float fftgaussian The gaussian at the desired position
*/
/* ------------------------------------------------------------ */
static float fftgaussian_array (int nx, int ny, int nv, float *expofacs, float *array, float *veloarray);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static float fftgaussian2d (int nx, int ny, float *expofacs)
  @brief Calculate a gaussian

  Returns the value of a gaussian in dependence of expofacs with 
  nu_v  = 0. gaussian = exp((expofacs[0]*nx*nx+expofacs[1]*nx*ny+
  expofacs[2]*ny*ny+expofacs[4])).
  To be used by convolgaussfft. No modulation with respect of the
  signum of the coordinates will be done. For a number in the exponent
  lesser than MINEXPONENT the return value is 0 (see fftgaussian).

  @todo The last item in the description to be implemented

  @param nx       (int)     Relative pixelposition in x
  @param ny       (int)     Relative pixelposition in y
  @param expofacs (float *) Factors in the gaussian, calculated by 
  expofacsfft and normalised with respect to the sizes of the array

  @return float fftgaussian2d: The gaussian at the desired position,
  no error handling.
*/
/* ------------------------------------------------------------ */
static float fftgaussian2d(int nx, int ny, float *expofacs);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static float fftgaussian2d (int nx, int ny, float *expofacs)
  @brief Calculate a gaussian

  Returns the value of a gaussian in dependence of expofacs with 
  nu_v  = 0. gaussian = exp((expofacs[0]*nx*nx+expofacs[1]*nx*ny+
  expofacs[2]*ny*ny+expofacs[4])).
  To be used by convolgaussfft. No modulation with respect of the
  signum of the coordinates will be done. For a number in the exponent
  lesser than MINEXPONENT the return value is 0 (see fftgaussian).

  @todo The last item in the description to be implemented

  @param nx       (int)     Relative pixelposition in x
  @param ny       (int)     Relative pixelposition in y
  @param expofacs (float *) Factors in the gaussian, calculated by 
  expofacsfft and normalised with respect to the sizes of the array

  @return float fftgaussian2d: The gaussian at the desired position,
  no error handling.
*/
/* ------------------------------------------------------------ */
static float fftgaussian2d_array(int nx, int ny, float *expofacs, float *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static Cube *convolgaussfft_here(void)
  @brief Convolve a cube with a gaussian via fft

  In-place convolution of a cube Cube with a gaussian via fft. The
  convolution is not normalised in the xy-plane but in v. No
  convolution takes place in v-direction in case of only one
  plane. See function expofacsfft_here for definition of expofacsfft_
  array.

  @param cube        (Cube *)  The cube

  @return (success) Cube *convolgaussfft_here: The convolved cube\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static Cube *convolgaussfft_here(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static Cube *convolgaussfft_here_single(void)
  @brief Convolve a cube with a gaussian via fft

  In-place convolution of a cube Cube with a gaussian via fft. The
  convolution is not normalised in the xy-plane but in v. No
  convolution takes place in v-direction in case of only one
  plane. See function expofacsfft_here for definition of expofacsfft_
  array.

  @param cube        (Cube *)  The cube

  @return (success) Cube *convolgaussfft_here: The convolved cube\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static Cube *convolgaussfft_here_single(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static Cube *convolgaussfft_noise(Cube *cube)
  @brief Calculation of a weights map from the cube

  cube is convolved with a beam of sqrt(1/2) times the sigma of the
  convolving beam and normalized with a factor
  2*sqrt(pi)*sigma_v*fluxpoint, where fluxpoint is the flux of one
  pointsource in galmod. This is not an in-place convolution, but it
  is safed to noise_.points. Then the noise of the original cube
  squared is added to noise_.points (more accurately this is done in
  Fourier-space before backtransformation.) The resulting map is used
  as a weights map for calculation of the chisquare. See function
  expofacsfft_noise for definition of expofacsfft_noise_ array.

  @param cube (Cube *)  The (pointsource) cube

  @return (success) Cube *convolgaussfft_here: The convolved cube\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static Cube *convolgaussfft_noise(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static Cube *convolgaussfft_noise_single(Cube *cube)
  @brief Calculation of a weights map from the cube

  cube is convolved with a beam of sqrt(1/2) times the sigma of the
  convolving beam and normalized with a factor
  2*sqrt(pi)*sigma_v*fluxpoint, where fluxpoint is the flux of one
  pointsource in galmod. This is not an in-place convolution, but it
  is safed to noise_.points. Then the noise of the original cube
  squared is added to noise_.points (more accurately this is done in
  Fourier-space before backtransformation.) The resulting map is used
  as a weights map for calculation of the chisquare. See function
  expofacsfft_noise for definition of expofacsfft_noise_ array.

  @param cube (Cube *)  The (pointsource) cube

  @return (success) Cube *convolgaussfft_here: The convolved cube\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static Cube *convolgaussfft_noise_single(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static void makemodelarray(float *array)
  @brief Fill the allocated array *array with precalculated summands for exp evaluation of the model_ cube

  @param cube (Cube *)  The (pointsource) cube

  @return (success) Cube *convolgaussfft_here: The convolved cube\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static void makemodelarray(float *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static void makenoisearray(float *array)
  @brief Fill the allocated array *array with precalculated summands for exp evaluation of the model_ cube

  @param cube (Cube *)  The (pointsource) cube

  @return (success) Cube *convolgaussfft_here: The convolved cube\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static void makenoisearray(float *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static float findpixelrealrel(Cube cube, int x, int y, int v) 

  @brief Find relative pixel values in a padded Cube

  The zero coordinate is array[0]. This function is not safe at all!

  @param array     (float *) The input cube
  @param x         (int)     relative x coordinate
  @param y         (int)     relative y coordinate
  @param v         (int)     relative v coordinate

  @return (success) float findpixelrel: Pixel value
*/
/* ------------------------------------------------------------ */
static float findpixelrealrel(Cube cube, int x, int y, int v);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static float findpixelrealrel(Cube cube, int x, int y, int v) 

  @brief Find relative pixel values in a padded Cube

  The zero coordinate is array[0]. This function is not safe at all!

  @param array     (float *) The input cube
  @param x         (int)     relative x coordinate
  @param y         (int)     relative y coordinate
  @param v         (int)     relative v coordinate

  @return (success) float findpixelrel: Pixel value
*/
/* ------------------------------------------------------------ */
static float findpixelrealrelmod(Cube cube, int x, int y, int v);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static float *expofacsfft_here(float sigma_maj, float sigma_min, float *sincosofangle)
  @brief Calculate static factors needed by convolgaussfft

  Returns an allocated array containing factors needed by
  convolgaussfft to convolve an array with a gaussian with sigma at
  the major axis sigma_major, minor axis sigma_minor. These factors
  are calculated from the measures of the convolution kernel and won't
  change during the whole program. There are, however members of the
  array that will change and will be added by calling the
  changeexpofacsfft and changeexpofacsfft_noise routines.

  @param sigma_maj     (float)   The sigma in direction of the major axis
  @param sigma_min     (float)   The sigma in direction of the minor axis
  @param sincosofangle (float *) An array containing the sin and the cos 
  of the position angle

  @return (success) float *expofacsfft: The factors wanted\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static float *expofacsfft_here(float sigma_maj, float sigma_min, float *sincosofangle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static Cube *convolgaussfft_here_array(void)
  @brief Convolve a cube with a gaussian via fft using a predefined array

  In-place convolution of a cube Cube with a gaussian via fft. The
  convolution is not normalised in the xy-plane but in v. No
  convolution takes place in v-direction in case of only one
  plane. See function expofacsfft_here for definition of expofacsfft_
  array.

  @param cube        (Cube *)  The cube

  @return (success) Cube *convolgaussfft_here: The convolved cube\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static Cube *convolgaussfft_here_array(void);
/* static void convolgaussfft_here_array_help1(void); */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static Cube *convolgaussfft_here_single_array(void)
  @brief Convolve a cube with a gaussian via fft using a predefined array

  In-place convolution of a cube Cube with a gaussian via fft. The
  convolution is not normalised in the xy-plane but in v. No
  convolution takes place in v-direction in case of only one
  plane. See function expofacsfft_here for definition of expofacsfft_
  array.

  @param cube        (Cube *)  The cube

  @return (success) Cube *convolgaussfft_here: The convolved cube\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static Cube *convolgaussfft_here_single_array(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static Cube *convolgaussfft_noise_array(Cube *cube)
  @brief Calculation of a weights map from the cube using a predefined array

  cube is convolved with a beam of sqrt(1/2) times the sigma of the
  convolving beam and normalized with a factor
  2*sqrt(pi)*sigma_v*fluxpoint, where fluxpoint is the flux of one
  pointsource in galmod. This is not an in-place convolution, but it
  is safed to noise_.points. Then the noise of the original cube
  squared is added to noise_.points (more accurately this is done in
  Fourier-space before backtransformation.) The resulting map is used
  as a weights map for calculation of the chisquare. See function
  expofacsfft_noise for definition of expofacsfft_noise_ array.

  @param cube (Cube *)  The (pointsource) cube

  @return (success) Cube *convolgaussfft_here: The convolved cube\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static Cube *convolgaussfft_noise_array(void);
/* static void convolgaussfft_noise_array_help1(void); */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static Cube *convolgaussfft_noise_single_array(Cube *cube)
  @brief Calculation of a weights map from the cube using a predefined array

  cube is convolved with a beam of sqrt(1/2) times the sigma of the
  convolving beam and normalized with a factor
  2*sqrt(pi)*sigma_v*fluxpoint, where fluxpoint is the flux of one
  pointsource in galmod. This is not an in-place convolution, but it
  is safed to noise_.points. Then the noise of the original cube
  squared is added to noise_.points (more accurately this is done in
  Fourier-space before backtransformation.) The resulting map is used
  as a weights map for calculation of the chisquare. See function
  expofacsfft_noise for definition of expofacsfft_noise_ array.

  @param cube (Cube *)  The (pointsource) cube

  @return (success) Cube *convolgaussfft_here: The convolved cube\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static Cube *convolgaussfft_noise_single_array(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static void changeexpofacsfft(float sigma_v)
  @brief Calculate factors needed by convolgaussfft
  
  Changes the expofacsfft_ array containing factors needed by
  convolgaussfft to convolve an array with a gaussian with sigma at
  the major axis sigma_major, minor axis sigma_minor, and v-axis
  sigma_v.

  @param sigma_v (float) The (original) sigma in v-direction

  @return (success) void
*/
/* ------------------------------------------------------------ */
static void changeexpofacsfft(float sigma_v);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static void changeexpofacsfft_noise(float sigma_v)
  @brief Calculate factors needed by convolgaussfft_noise

  Changes the expofacsfft_noise_ array containing factors needed by
  convolgaussfft to convolve an array with a gaussian with sigma at
  the major axis sigma_major, minor axis sigma_minor, and v-axis
  sigma_v/sqrt(2). Also, a normalisation is applied, such that the
  output is scaled by scale*2*sqrt(pi)*sigma_v*fluxpoint.

  @param sigma_maj     (float)   The sigma in direction of the major axis
  @param sigma_min     (float)   The sigma in direction of the minor axis
  @param sigma_v       (float)   The sigma in v-direction
  @param sincosofangle (float *) An array containing the sin and the cos 
  of the position angle

  @return (success) float *expofacsfft: The factors wanted\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static void changeexpofacsfft_noise(float sigma_v);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static float *sincosofangle(float angle)
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
static float *sincosofangle(float angle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static float degreetoradian(float degrees)
  @brief Converts from degrees to radians

  Changes a number from deg to rad

  @param degrees (float) An angle in degrees

  @return float degreetoradian: Input angle in radians
*/
/* ------------------------------------------------------------ */
static float degreetoradian(float degrees);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
   @fn static int initchisquare((float *arrayorig, float *arraymodel, int
  *x, int *y, int *v, float *hpbwmaj, float *hpbwmin, float *pa, float
  *scale, float *flux, float *sigma, int *mode, int *arrayvsize,
  double *chisquare, float *noiseweight, int *inimode, int *threads)

  @brief Initializes the chisquare derivation control, internal function

  This function will be called internally by initchisquare_, while as
  many parameters as possible are made local copies to this module.

  Initialising routine. Reads in the arrays corresponding to the
  original cube and the model and links them into global tirific Cubes
  that are private to the module.  The user gives the logical size of
  the cubes *x, *y, *v, where any pixel is adressed by
  pixelvalue(x,y,v) = x+(physical_size_in_x)(y+physical_size_in_y*v),
  where x,y,v is the integer pixelvalues starting with 0. The physical
  size of the array in x has to be 2*(logical_size_x/2+1), where the
  division is an integer division (rounds down to the next
  integer). The physical size of the array in y has to be the same as
  the logical, while there is no restriction of the physical size in v
  else than it has to be larger than the logical size in v. Hence the
  physical size is passed in arrayvsize.

  hpbwmaj, hpbwmin are the major axis and minor axis HPBW of the
  convolving gaussian beam, pa its position angle. These quantities
  don't change during several calculations of the chisquare. Scale is
  the scale the passed model has to be multiplied by to match the
  units of the original (If the model is in W.U., the original in Jy,
  this is 0.005). Flux is the flux of one pointsource in galmod. Sigma
  is the rms noise in the original. 

  At initialisation the mode of the chisquare calculation can be
  set. Bitwise the following adjustments are made, while the value to be passed with mode is bit0+2*bit1:

  Bit 0: A uniform error that equals the noise in the original is used
  to weight the chisquare if unset (bit0 = 0), if set (bit0 = 1), a
  weightmap is calculated from the model, that is then used to weight
  the chisquare.

  Bit 1: If set (bit1 = 1), memory will be allocated for one or two
  arrays within which precalculated factors are stored that otherways
  at time of chisquare calculation will be repeatedly
  recalculated. Saves some time. If unset (bit1 = 0) this optimisation
  will not take place, saving memory.

  Bit 2: If set (bit2 = 0), memory will be allocated for out-of-place
  ffts instead of the in-place ffts used for the convolution. fftw can
  calculate more efficient ways to perform the fft. This costs a lot
  of memory (see list below), such that for computers with small
  memory and big cubes, this option should be unset.

  initchisquare will check upon the feasibility of the operation and
  return 0 if the memory allocations cannot be made. The additionally
  required memory (not counting the passed arrays) is a bit more than:

  mode = 0: sizeof(float)* [ 0 ]
  mode = 1: sizeof(float)* [ (*x/2)*2+2)**y**v ]
  mode = 2: sizeof(float)* [ (*x/2+1)**y ]
  mode = 3: sizeof(float)* [ (*x/2)*2+2)**y**v + 2 * (*x/2+1)**y ]
  mode = 4: sizeof(float)* [ (*x/2)*2+2)**y**v ]
  mode = 5: sizeof(float)* [ 3 * (*x/2)*2+2)**y**v ]
  mode = 6: sizeof(float)* [ (*x/2)*2+2)**y**v + (*x/2+1)**y ]
  mode = 7: sizeof(float)* [ 3 * (*x/2)*2+2)**y**v + 2 * (*x/2+1)**y ]
  
  The chisquare evaluation goes as follows (logically, internal
  calculation goes a slightly different path):

  1) The pointsource model is convolved with a gaussian beam of HPBW
  size hpbwmaj, hpbwmin, hpbwv (given when calling the getchisquare
  function), The beam is rotated from N to E with the position angle
  pa, but not in the third dimension. All values are pixel values,
  except for the pa, which is in deg. The result of the convolution is
  m.

  2) The pointsource model is convolved with a gaussian beam of
  1/sqrt(2) times the size of the original convolving beam and the
  same position angle and then multiplied with the flux of one
  pointsource pointflux resulting in the map r

  3) An inverse weightmap w is computed with w(x,y,z) =
  ((sigma*noiseweight)^2+r(x,y,z))*noiseweight^(-2)

  The noiseweight parameter serves two functions. It determines how
  much weight is laid on the quantisation noise imposed by the
  pointsource quantisation in comparison to the natural noise sigma of
  the original datacube. It hence also serves as a downweighting
  function of regions with high surface density. Bit 0 of model set to 0 is
  equivalent with noiseweight = infity, while noiseweight towards 0 (0
  is actually an error) will impose an additional weight on regions of
  low surface density.

  4) The chisquare is calculated from original o, convolved model m,
  and inverse noisemap n by chisquare = sum_x_y_v
  (o(x,y,v)-m(x,y,v))^2/n(x,y,v)

  A value of mode of 2 or 3 means that the module needs a bit more
  memory, but will be slightly faster, as some factors for the
  evaluation of the chisquare are precalculated and stored in the
  memory.

  The chisquare parameter passes the pointer to the variable that
  contains the chisquare.

  As this module is meant for the use in interfacing fortran and c,
  all parameters are pointers. It is recommended to use them only for
  passing them into the initialisation routine, as some of them are
  used internally. Mark that when calling the initchisquare_ routine,
  the input arrays will be overwritten. They should be initialised
  after calling initchisquare_.

  The parameter inimode ranges from 0 to 3 and determines the time
  that is given to the fft initialising routines to calculate the
  shortest way to perform an fft. 0 means that almost no time is spent
  on optimising the routines, which then will take longer, 3 means to
  spend a long time optimising (once, for the whole process) to really
  get the shortest fft, which maybe pays if a long time is spend
  calculating again and again the chisquare.
  
  @param arrayorig  (*float)    Array corresponding to the original cube
  @param arraymodel (*float)    Array corresponding to the model (pointsource) cube
  @param x          (int *)     Size of logical array in x (that is regarded in calculation)
  @param y          (int *)     Size fo logical array in y
  @param v          (int *)     Size fo logical array in v
  @param hpbwmaj    (float *)   HPBW of the gaussian beam, major axis
  @param hpbwmin    (float *)   HPBW of the gaussian beam, minor axis
  @param pa         (float *)   Position angle of the gaussian beam
  @param scale      (float *)   Scale factor to scale model by to match original
  @param flux       (float *)   The flux of one pointsource in galmod
  @param sigma      (float *)   Sigma rms in the original
  @param mode       (int *)     Calculation of the chisquare depends also on quantisation noise (1 or 3) or on the sigma rms in the original alone (0 or 2) If set to 2 or 3 a bit more memory is needed but the routine runs a bit faster.
  @param arrayxsize  (int *)    Physical size of reserved arrays of model and original in x, a dummy, is always 2*((int) *x/2+1)
  @param chisquare   (double *) Pointer to the variable containing the chisquare used throughout the code
  @param noiseweight (float *)  Parameter used for weighting quantisation noise
  @param inimode     (int *)    Mode for the determination of the best fft.
  @param threads     (int *)    Number of threads.

  @return (success) int initchisquare_: 1
          (error) 0
*/
/* ------------------------------------------------------------ */
static int initchisquare(float *arrayorig, float *arraymodel, int *x, int *y, int *v, float *hpbwmaj, float *hpbwmin, float *pa, float *scale, float *flux, float *sigma, int *mode, int *arrayvsize, double *chisquare, float *noiseweight, int *inimode, int *threads);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION CODE */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Initialisation from external, the sense of this function is to make the module robust to changes from external, i.e., the function expects pointers, because that is what you get when you call c from fortran. Inernally these should be protected, i.e. local static variables are created that are pointed to */
int initchisquare_(float *arrayorig, float *arraymodel, int *x, int *y, int *v, float *hpbwmaj, float *hpbwmin, float *pa, float *scale, float *flux, float *sigma, int *mode, int *arrayvsize, double *chisquare, float *noiseweight, int *inimode, int *threads)
{
  static int xm, ym, vm; 
  static float hpbwmajm, hpbwminm, pam, scalem, fluxm, sigmam; 
  static int modem, arrayvsizem; 
  static float noiseweightm; 
  static int inimodem;
  static int threadsm;

  xm = *x;
  ym = *y;
  vm = *v;

  hpbwmajm = *hpbwmaj;
  hpbwminm = *hpbwmin;
  pam = *pa;
  scalem = *scale;
  fluxm = *flux;
  sigmam = *sigma;

  modem =  *mode;
  arrayvsizem = *arrayvsize;
  noiseweightm = *noiseweight; 
  inimodem = *inimode;
  threadsm = *threads;

  return initchisquare(arrayorig, arraymodel, &xm, &ym, &vm, &hpbwmajm, &hpbwminm, &pam, &scalem, &fluxm, &sigmam, &modem, &arrayvsizem, chisquare, &noiseweightm, &inimodem, &threadsm);
}


/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Initialisation from external, the sense of this function is to make the module robust to changes from external, i.e., the function expects pointers, because that is what you get when you call c from fortran. Inernally these should be protected, i.e. local static variables are created that are pointed to */
int initchisquare_c(float *arrayorig, float *arraymodel, int x, int y, int v, float hpbwmaj, float hpbwmin, float pa, float scale, float flux, float sigma, int mode, int arrayvsize, double *chisquare, float noiseweight, int inimode, int threads)
{
  static int xm, ym, vm; 
  static float hpbwmajm, hpbwminm, pam, scalem, fluxm, sigmam; 
  static int modem, arrayvsizem; 
  static float noiseweightm; 
  static int inimodem;
  static int threadsm;

  xm = x;
  ym = y;
  vm = v;

  hpbwmajm = hpbwmaj;
  hpbwminm = hpbwmin;
  pam = pa;
  scalem = scale;
  fluxm = flux;
  sigmam = sigma;

  modem =  mode;
  arrayvsizem = arrayvsize;
  noiseweightm = noiseweight; 
  inimodem = inimode;
  threadsm = threads;

  return initchisquare(arrayorig, arraymodel, &xm, &ym, &vm, &hpbwmajm, &hpbwminm, &pam, &scalem, &fluxm, &sigmam, &modem, &arrayvsizem, chisquare, &noiseweightm, &inimodem, &threadsm);
}


/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Initialisation */

int initchisquare(float *arrayorig, float *arraymodel, int *x, int *y, int *v, float *hpbwmaj, float *hpbwmin, float *pa, float *scale, float *flux, float *sigma, int *mode, int *arrayvsize, double *chisquare, float *noiseweight, int *inimode, int *threads)
{
  float *sincosofangle_;
  int physical[3];
  int physical2[3];
  int logical[3];
  int physicaln[3];
  int inimodel;
  static char usedonce = 0;

  /* hyper */
#ifdef OPENMPFFT
  fftwf_init_threads();
#endif

  if ((usedonce)) {
    if ((noise_.points))
      fftwf_free(noise_.points);
    if ((transformed_cube_noise_))
      fftwf_free(transformed_cube_noise_);
    if ((transformed_cube_model_))
      fftwf_free(transformed_cube_model_);
    if ((expcube_model_.points))
      fftwf_free(expcube_model_.points);
    if ((expcube_noise_.points))
      fftwf_free(expcube_noise_.points);
    if ((expofacsfft_))
      free(expofacsfft_);
    if ((expofacsfft_noise_))
      free(expofacsfft_noise_);
    if ((veloarray_))
      fftwf_free(veloarray_);
     if ((veloarray_noise_))
      fftwf_free(veloarray_noise_);
 }

  noise_.points = NULL;
  transformed_cube_noise_ = NULL;
  transformed_cube_model_ = NULL;
  expcube_model_.points = NULL;
  expcube_noise_.points = NULL;
  expofacsfft_ = NULL;
  expofacsfft_noise_ = NULL;
  veloarray_ = NULL;
  veloarray_noise_ = NULL;

  oldsigma_ = -1;

  threads_ = *threads;
  vector_ = (double *) malloc(threads_*sizeof(double));

  /* set number of threads */
#ifdef OPENMPTIR
  omp_set_num_threads(threads_);
#endif

  /* put the chisquare in its place */
  chisquare_ = chisquare;

/* Get the array of the original */
  original_.points = arrayorig;
/* Get the array of the model */
  model_.points = arraymodel;

  realorigsizex_ = 2*(*x/2+1);
/* 2*(*x/2+1); */
realorigsizey_ = *y;
realmodelsizex_ = 2*(*x/2+1);
realmodelsizey_ = *y;

  /* Allocate memory for the noisecube if the noise per pixel is required in future */
  if ((*mode & 1)) {
    if (!((noise_.points) = (float *) fftwf_malloc(((*x/2)*2+2)**y**v*sizeof(float))))
      goto error;

    /* There might be a chance that things work faster with an out-of-place trafo on the expense of double the memory usage */
    if (*mode & 4) {
      if (!(transformed_cube_noise_ = (fftwf_complex *) fftwf_malloc((*x/2+1)**y**v*sizeof(fftwf_complex)))) {
	fftwf_free(noise_.points);
	goto error;
      }
    }
  }
  else 
    noise_.points = NULL;

    /* There might be a chance that things work faster with an out-of-place trafo on the expense of double the memory usage */
  if (*mode & 4) {
    if (!(transformed_cube_model_ = (fftwf_complex *) fftwf_malloc((*x/2+1)**y**v*sizeof(fftwf_complex)))) {
      if (*mode & 1) {
	fftwf_free(noise_.points);
	fftwf_free(transformed_cube_noise_);
	goto error;
      }
    }
  }

    /* Allocate memory for the expcubes if they are required in future */
  if ((*mode & 2)) {
    if (!((expcube_model_.points) = (float *) fftwf_malloc((*x/2+1)**y*sizeof(float)))) {
      if ((*mode & 1)) 
	fftwf_free(noise_.points);
      if ((*mode & 4)) {
	if ((*mode & 1))
	fftwf_free(transformed_cube_noise_);
	fftwf_free(transformed_cube_model_);
      }
      goto error;
    }
    expcube_model_.size_x = *x/2+1;
    expcube_model_.size_y = *y;
    expcube_model_.size_v = 1;
    expcube_model_.padding = 0;
    if ((*mode & 1)) {
      if (!((expcube_noise_.points) = (float *) fftwf_malloc((*x/2+1)**y*sizeof(float)))) {
	if ((*mode & 1))
	  fftwf_free(noise_.points);
	fftwf_free(expcube_model_.points);
      if ((*mode & 4)) {
	if ((*mode & 1))
	fftwf_free(transformed_cube_noise_);
	fftwf_free(transformed_cube_model_);
      }
	goto error;
      }
    }
    else
      expcube_noise_.points = NULL;
    /* This info is warranted */
      expcube_noise_.size_x = *x/2+1;
      expcube_noise_.size_y = *y;
      expcube_noise_.size_v = 1;
      expcube_noise_.padding = 0;
    expcube_noise_.refpix_x = expcube_noise_.refpix_y = expcube_noise_.refpix_v = expcube_model_.refpix_x = expcube_model_.refpix_y = expcube_model_.refpix_v = 0;
  }
  else 
    expcube_model_.points = expcube_noise_.points = NULL;

  /* Now get the sizes right */
  original_.size_x = model_.size_x = noise_.size_x = *x;
  original_.size_y = model_.size_y = noise_.size_y = *y;
  original_.size_v = model_.size_v = noise_.size_v = *v;

  original_.refpix_x = model_.refpix_x  = noise_.refpix_x  = 0;
  original_.refpix_y = model_.refpix_y  = noise_.refpix_y  = 0;
  original_.refpix_v = model_.refpix_v  = noise_.refpix_v  = 0;

  /* We don't need the reference pixel, but the padding */
  original_.padding = model_.padding = noise_.padding = (*x/2)*2+2-*x;

  /* The scale */
  original_.scale = *scale;
  model_.scale = *flux;
  if (!(*mode & 1))
    *noiseweight = 1;
  noise_.scale = *sigma**sigma**noiseweight**noiseweight;
  expcube_model_.scale = *noiseweight**noiseweight;

  /* Now initialize the expofacsfft array */

  /* We have only the HPBWs, so calculate the gaussian widths */
  if (!(sincosofangle_ = sincosofangle(*pa))) {
    if ((*mode & 1)) {
      fftwf_free(noise_.points);
    if ((*mode & 2))
      fftwf_free(expcube_noise_.points);
    }
    if ((*mode & 2))
      free(expcube_model_.points);
      if ((*mode & 4)) {
	if ((*mode & 1))
	fftwf_free(transformed_cube_noise_);
	fftwf_free(transformed_cube_model_);
      }
    goto error;
  }

  if (!(expofacsfft_ = expofacsfft_here(sigma_maj_ = 0.42466090014401**hpbwmaj, sigma_min_ = 0.42466090014401**hpbwmin, sincosofangle_))) {
    if ((*mode & 1)) {
      fftwf_free(noise_.points);
    if ((*mode & 2))
      fftwf_free(expcube_noise_.points);
    }
    if ((*mode & 2))
      fftwf_free(expcube_model_.points);
    free(sincosofangle_);
      if ((*mode & 4)) {
	if ((*mode & 1))
	fftwf_free(transformed_cube_noise_);
	fftwf_free(transformed_cube_model_);
      }
    goto error;
  }

  if (!(expofacsfft_noise_ = expofacsfft_here(sigma_maj_noise_ = sigma_maj_*SQRTOF2, sigma_min_noise_ = sigma_min_*SQRTOF2, sincosofangle_))) {
    if ((*mode & 1)) {
      fftwf_free(noise_.points);
    if ((*mode & 2))
      fftwf_free(expcube_noise_.points);
    }
    if ((*mode & 2))
      fftwf_free(expcube_model_.points);
    free(sincosofangle_);
    free(expofacsfft_);
      if ((*mode & 4)) {
	if ((*mode & 1))
	fftwf_free(transformed_cube_noise_);
	fftwf_free(transformed_cube_model_);
      }
    goto error;
  }

    /* Now the veloarray */
  if (!(veloarray_ = (float *) fftwf_malloc((model_.size_v/2+1)*sizeof(float)))) {
    if ((*mode & 1)) {
      fftwf_free(noise_.points);
      noise_.points = NULL;
      if ((*mode & 2)) {
      fftwf_free(expcube_noise_.points);
      expcube_noise_.points = NULL;
      }
    }
    if ((*mode & 2)) {
      fftwf_free(expcube_model_.points);
      expcube_model_.points = NULL;
    }
    free(sincosofangle_);
    sincosofangle_ = NULL;
    free(expofacsfft_);
    expofacsfft_ = NULL;
      if ((*mode & 4)) {
	if ((*mode & 1)) {
	  fftwf_free(transformed_cube_noise_);
	  transformed_cube_noise_ = NULL;
	}
	fftwf_free(transformed_cube_model_);
	transformed_cube_model_ = NULL;
      }
    goto error;
  }

    /* Now the veloarray */
  if (!(veloarray_noise_ = (float *) fftwf_malloc((model_.size_v/2+1)*sizeof(float)))) {
    if ((*mode & 1)) {
      fftwf_free(noise_.points);
      noise_.points = NULL;
      if ((*mode & 2)) {
	fftwf_free(expcube_noise_.points);
	expcube_noise_.points = NULL;
      }
    }
    if ((*mode & 2)) {
      fftwf_free(expcube_model_.points);
      expcube_model_.points = NULL;
    }
    free(sincosofangle_);
    sincosofangle_ = NULL;
    free(expofacsfft_);
    expofacsfft_ = NULL;
    if ((*mode & 4)) {
      if ((*mode & 1)) {
	fftwf_free(transformed_cube_noise_);
	transformed_cube_noise_ = NULL;
      }
      fftwf_free(transformed_cube_model_);
      transformed_cube_model_ = NULL;
    }
    fftwf_free(veloarray_);
    goto error;
  }

/* Fill the arrays that describe the transformation */

  if (model_.size_v != 1) {
    logical[0] = model_.size_v;
    logical[1] = model_.size_y;
    logical[2] = model_.size_x;
    
    physical[0] = *arrayvsize;
    physical[1] = model_.size_y;
    physical[2] = 2*(model_.size_x/2)+2;

    physicaln[0] = model_.size_v;
    physicaln[1] = model_.size_y;
    physicaln[2] = 2*(model_.size_x/2)+2;

    physical2[0] = model_.size_v;
    physical2[1] = model_.size_y;
    physical2[2] = (model_.size_x/2)+1;

        if (*mode & 2) {
    connoise_ = convolgaussfft_noise_array;
    conmodel_ = convolgaussfft_here_array;
    }
    else {
    connoise_ = convolgaussfft_noise;
    conmodel_ = convolgaussfft_here;
    }

  }
  else {
    logical[0] = model_.size_y;
    logical[1] = model_.size_x;
    
    physical[0] = model_.size_y;
    physical[1] = 2*(model_.size_x/2)+2;

    physicaln[0] = model_.size_y;
    physicaln[1] = 2*(model_.size_x/2)+2;

    physical2[0] = model_.size_y;
    physical2[1] = (model_.size_x/2)+1;

        if (*mode & 2) {
    connoise_ = convolgaussfft_noise_single;
    conmodel_ = convolgaussfft_here_single;
    }
    else {
    connoise_ = convolgaussfft_noise_single_array;
    conmodel_ = convolgaussfft_here_single_array;
    }
  }

  /* Take the input from inimodel to decide upon the way to
     initialize fftw */
  switch (*inimode) {
  case 1:
    inimodel = FFTW_MEASURE;
    break;
  case 2:
    inimodel = FFTW_PATIENT;
    break;
  case 3:
    inimodel = FFTW_EXHAUSTIVE;
    break;
  default:
    inimodel = FFTW_ESTIMATE;
    break;
  }
  
  /* Now make the plans for the fftw */

#ifdef OPENMPFFT
  fftwf_plan_with_nthreads(threads_);
#endif

  if (*mode & 1) {

    /* point the trasnsformed cube to the cube itself for an in-place
	   transformation */
    if (*mode & 4)
      ;
      else
    transformed_cube_noise_ = (fftwf_complex *) noise_.points;
    
    /* fill plan_noise_ and plin_noise_ with the necessary information. Take care with the order of the axes, reversed for fftw */


      if (model_.size_v != 1) {
	plan_noise_ = fftwf_plan_many_dft_r2c(3, logical, 1, model_.points, physical, 1, 0, transformed_cube_noise_, physical2, 1, 0, inimodel | FFTW_PRESERVE_INPUT);
	plin_noise_ = fftwf_plan_many_dft_c2r(3, logical, 1, transformed_cube_noise_, physical2, 1, 0, noise_.points, physicaln, 1, 0, inimodel);
/* (*x/2)*2+2)**y**v */
/* fftwf_plan_dft_c2r_3d(model_.size_v, model_.size_y, model_.size_x, transformed_cube_noise_, noise_.points, inimodel); */
      }
      else {
	plan_noise_ = fftwf_plan_many_dft_r2c(2,logical , 1, model_.points, physical, 1, 0, transformed_cube_noise_, physical2, 1, 0, inimodel | FFTW_PRESERVE_INPUT);
      plin_noise_ = fftwf_plan_dft_c2r_2d(model_.size_y, model_.size_x, transformed_cube_noise_, noise_.points, inimodel);    
      
      }
  }

  /* Fill the global variables that affect the noise estimation and
     the convolution */
  cubesizexhalf_ = model_.size_x/2;
  cubesizeyhalf_ = model_.size_y/2;
  newsize_ = cubesizexhalf_+1; /* The physical size of the cube in x */
  dummy_ = model_.size_v/2;

  /* point the trasnsformed cube to the cube itself for an in-place transformation */
  if (*mode & 4)
    ;
  else
    transformed_cube_model_ = (fftwf_complex *) (model_).points;

  if (model_.size_v != 1) {
    logical[0] = model_.size_v;
    logical[1] = model_.size_y;
    logical[2] = model_.size_x;

    physical[0] = *arrayvsize;
    /* formerly: model_.size_v */
    physical[1] = model_.size_y;
    physical[2] = 2*(model_.size_x/2)+2;

    physical2[0] = *arrayvsize;
    /* formerly: model_.size_v */
    physical2[1] = model_.size_y;
    physical2[2] = (model_.size_x/2)+1;
  }
  else {
    logical[0] = model_.size_y;
    logical[1] = model_.size_x;
    
    physical[0] = model_.size_y;
    physical[1] = 2*(model_.size_x/2)+2;

    physical2[0] = model_.size_y;
    physical2[1] = (model_.size_x/2)+1;
  }

   /* fill plan and plin with the necessary information. Take care with the order of the axes, reversed for fftw */
  if (model_.size_v != 1) {
    plan_model_ = fftwf_plan_many_dft_r2c(3, logical, 1, model_.points, physical, 1, 0, transformed_cube_model_, physical2, 1, 0, inimodel);
    plin_model_ = fftwf_plan_many_dft_c2r(3, logical, 1, transformed_cube_model_, physical2, 1, 0, model_.points, physical, 1, 0, inimodel);
  }
  else {
    plan_model_ = fftwf_plan_dft_r2c_2d((model_).size_y, (model_).size_x, (model_).points, transformed_cube_model_, inimodel);
    plin_model_ = fftwf_plan_dft_c2r_2d((model_).size_y, (model_).size_x, transformed_cube_model_, (model_).points, inimodel);    
  } 

  /* Now do some silly hacking */
  if (*mode & 1) {
    noiseconstant_1_ = (-2*SQRTOF2*PI_HERE*SQRTOF2*PI_HERE)/(original_.size_v*original_.size_v);
    noiseconstant_2_ = original_.scale*model_.scale*2*PI_HERE*sigma_min_noise_*sigma_maj_noise_/(original_.size_v*original_.size_y*original_.size_x*2*SQRTPI);
  }

  modelconstant_1_ = -2*(PI_HERE*PI_HERE)/(original_.size_v*original_.size_v);

  /* Fill the arrays for the exponential acceleration if required */
  if (*mode & 2) {
    /* In any case that is for the model */
    makemodelarray(expcube_model_.points);
    
    /* Could be that it is also the noisemap */
    if (*mode & 1) {
      makenoisearray(expcube_noise_.points);
    }
  }

  /* Now check for the function that is needed to calculate the chisquare */
  engalmod_chflgs();

  /* nearly finished */
  free(sincosofangle_);
  return 1;
  
 error:
  noise_.points = NULL;
  transformed_cube_noise_ = NULL;
  transformed_cube_model_ = NULL;
  expcube_model_.points = NULL;
  expcube_noise_.points = NULL;
  expofacsfft_ = NULL;
  expofacsfft_noise_ = NULL;
  veloarray_ = NULL;
  veloarray_noise_ = NULL;
  return 0;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* (Re-)Initialisation of the chisquare finding routine */
void engalmod_chflgs(void)
{
  int i,j,k;

  fetchchisquare_ = &fetchchisquare_unflagged; 

  for(k = 0; k < original_.size_v; ++k){
    for(j = 0; j < original_.size_y; ++j) {
      for(i = 0; i < original_.size_x; ++i) {
	  /* A nan compared with itself is false */
	if (findpixelrealrel(original_, i, j, k) != findpixelrealrel(original_, i, j, k)) {
	/* if ((double) ((findpixelrealrel(original_, i, j, k))) < HOT_VALUE) { */
	  fetchchisquare_ = &fetchchisquare_flagged;
	  break;
	}
      }
    }
  }
  
  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

static float findpixelrealrel(Cube cube, int x, int y, int v)
{
  return (cube.points)[x+realorigsizex_*(y+realorigsizey_*v)];
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

static double fetchchisquare_flagged(void)
{
  int i,j,k;
  double chisquare = 0;
  int nthreadz = 0;

  for (i =0 ; i < threads_; ++i)
    vector_[i] = 0.0;

  /* Now calculate the chisquare */
  if ((noise_.points)) {
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
    for(k = 0; k < original_.size_v; ++k){
#ifdef OPENMPTIR
      if (nthreadz == 0) {
	nthreadz = omp_get_num_threads();
      }
#else
      nthreadz = 1;
#endif
      for(j = 0; j < original_.size_y; ++j) {
	for(i = 0; i < original_.size_x; ++i) {
#ifdef OPENMPTIR
	  /* A nan compared with itself is false */
	if (findpixelrealrel(original_, i, j, k) == findpixelrealrel(original_, i, j, k)) {
	  /* if (findpixelrealrel(original_, i, j, k) > HOT_VALUE) { */
	    vector_[omp_get_thread_num()] += (double) ((findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))*(findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))/findpixelrealrelmod(noise_, i, j, k));
	  }
#else
	  /* A nan compared with itself is false */
	if (findpixelrealrel(original_, i, j, k) == findpixelrealrel(original_, i, j, k)) {
	  /* if (findpixelrealrel(original_, i, j, k) > HOT_VALUE) { */
	    vector_[0] += (double) ((findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))*(findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))/findpixelrealrelmod(noise_, i, j, k));
	  }
#endif
	  
	}
      }
    }
  
    for (i = 0; i < nthreadz; ++i) 
      chisquare += vector_[i]*(double) expcube_model_.scale;
  }
  else {
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
    for(k = 0; k < original_.size_v; ++k){
#ifdef OPENMPTIR
      if (nthreadz == 0) {
	nthreadz = omp_get_num_threads();
      }
#else
      nthreadz = 1;
#endif
      for(j = 0; j < original_.size_y; ++j) {
	for(i = 0; i < original_.size_x; ++i) {
#ifdef OPENMPTIR
	  /* A nan compared with itself is false */
	if (findpixelrealrel(original_, i, j, k) == findpixelrealrel(original_, i, j, k)) {
	  /* if (findpixelrealrel(original_, i, j, k) > HOT_VALUE) { */
	    vector_[omp_get_thread_num()] += (double) ((findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))*(findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k)));
	  }
#else
	  /* A nan compared with itself is false */
	if (findpixelrealrel(original_, i, j, k) == findpixelrealrel(original_, i, j, k)) {
	  /* if (findpixelrealrel(original_, i, j, k) > HOT_VALUE) { */
	    vector_[0] += (double) ((findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))*(findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k)));
	  }
#endif
	}
      }
    }

    for (i = 0; i < nthreadz; ++i) 
      chisquare += vector_[i]/noise_.scale;
  }
    return chisquare;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

static double fetchchisquare_unflagged(void)
{
  int i,j,k;
  double chisquare = 0;
  int nthreadz = 0;

  for (i =0 ; i < threads_; ++i)
    vector_[i] = 0;

  /* Now calculate the chisquare */
  if ((noise_.points)) {
#ifdef OPENMPTIR
# pragma omp parallel for
#endif
    for(k = 0; k < original_.size_v; ++k){
#ifdef OPENMPTIR
      if (nthreadz == 0) {
	nthreadz = omp_get_num_threads();
      }
#else
      nthreadz = 1;
#endif
      for(j = 0; j < original_.size_y; ++j) {
	for(i = 0; i < original_.size_x; ++i) {
#ifdef OPENMPTIR
	  vector_[omp_get_thread_num()] += (double) ((findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))*(findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))/findpixelrealrelmod(noise_, i, j, k));
#else
	  vector_[0] += (double) ((findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))*(findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))/findpixelrealrelmod(noise_, i, j, k));
#endif
	}
      }
    }


    for (i = 0; i < nthreadz; ++i) 
	  chisquare += vector_[i]*(double) expcube_model_.scale;
  }
  else {
#ifdef OPENMPTIR
# pragma omp parallel for
#endif

    for(k = 0; k < original_.size_v; ++k){
#ifdef OPENMPTIR
      if (nthreadz == 0)
	nthreadz = omp_get_num_threads();
#else
      nthreadz = 1;
#endif
     for(j = 0; j < original_.size_y; ++j) {
	for(i = 0; i < original_.size_x; ++i) {
#ifdef OPENMPTIR
	  vector_[omp_get_thread_num()] += (double) ((findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))*(findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k)));
#else
	  vector_[0] += (double) ((findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k))*(findpixelrealrel(original_, i, j, k)-findpixelrealrelmod(model_, i, j, k)));
#endif
	}
      }
    }



    for (i = 0; i < nthreadz; ++i) 
      chisquare += vector_[i]/noise_.scale;
  }

  return chisquare;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

static float findpixelrealrelmod(Cube cube, int x, int y, int v)
{
  return (cube.points)[x+realmodelsizex_*(y+realmodelsizey_*v)];
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

double getchisquare_(float *sigma_v)
{
  /* formerly times 0.42466090014401, but now it's the real sigma */
  return getchisquare_c(*sigma_v);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

double getchisquare_c (float sigma_v)
/* If ever the flux of one pointsource changes during one run, activate this */
/* static double getchisquare (float *array, float HPBW_v, float pointflux) */
{
  /* Set the chisquare to 0 */
  double chisquare = 0;

  /* If a weight map should be calculated */
  if ((noise_.points)) {
    if (sigma_v != oldsigma_) {
      changeexpofacsfft_noise(sigma_v);
    }
    /* If ever the flux of one pointsource changes during one run, activate this */
    /*     model_.scale = pointflux; */
    
    (*connoise_)();
  }
 
  /* In any case we need the convolved cube */
  if (sigma_v != oldsigma_) {
    changeexpofacsfft(sigma_v);
  }

  (*conmodel_)();
  oldsigma_ = sigma_v;
  
  /* Now calculate the chisquare */
  chisquare = (*fetchchisquare_)();

  *chisquare_ = chisquare;
  return chisquare;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Convolve a cube with a gaussian via fft */

static Cube *convolgaussfft_here(void)
{
  int i, j, k;
  float expresult;                 /* A dummy */
  
  /* Convolution in all dimensions or in xy only */
  
  
  /* Now do the transform */
  fftwf_execute(plan_model_);
  
  /* multiply with the gaussian, first for nu_v = 0 */
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
  for (i = 0; i < newsize_; ++i) {
    for (j = 0; j < (model_).size_y; ++j) {
      
      /* The exponential will be evaluated from 0, ... , N/2 and -1, ..., -N/2 or -N/2 - 1 */
      expresult = fftgaussian2d((i <= cubesizexhalf_) ? i : (i-(model_).size_x), (j <= cubesizeyhalf_) ? j : (j-(model_).size_y), expofacsfft_);
      transformed_cube_model_[i+newsize_*j][0] = expresult*transformed_cube_model_[i+newsize_*j][0];
      transformed_cube_model_[i+newsize_*j][1] = expresult*transformed_cube_model_[i+newsize_*j][1];
    }
  }
 
  /* Check for an extra-axis in v, i.e. if the dimension in v is even, we have to calculate one v-plane separately */
  if (!((model_).size_v % 2)) {
    /* multiply with the gaussian, first for nu_v = N_v/2 */
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
    for (i = 0; i < newsize_; ++i) {
      for (j = 0; j < (model_).size_y; ++j)
	{
	  /* The exponential will be evaluated from 0, ... , N/2 and -1, ..., -N/2 or -N/2 - 1 */
	  expresult = fftgaussian((i <= cubesizexhalf_) ? i : (i-(model_).size_x), (j <= cubesizeyhalf_) ? j : (j-(model_).size_y), dummy_, expofacsfft_, veloarray_);
	  transformed_cube_model_[i+newsize_*(j+(model_).size_y*dummy_)][0] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*dummy_)][0];
	  transformed_cube_model_[i+newsize_*(j+(model_).size_y*dummy_)][1] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*dummy_)][1];
	}
    }
  }
  
  /* Now the rest has to be done, v ranges from 1, ..., N_v-1/2, and using the symmetrics of the gaussian we fill the rest */
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
  for (i = 0; i < newsize_; ++i) {
    for (j = 0; j < (model_).size_y; ++j) {
      for (k = 1; k <= ((model_).size_v-1)/2; ++k) {
	expresult = fftgaussian((i <= cubesizexhalf_) ? i : (i-(model_).size_x), (j <= cubesizeyhalf_) ? j : (j-(model_).size_y), k, expofacsfft_, veloarray_);
	transformed_cube_model_[i+newsize_*(j+(model_).size_y*k)][0] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*k)][0];
	transformed_cube_model_[i+newsize_*(j+(model_).size_y*k)][1] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*k)][1];
	
	/* Because of the symmetry, f(v) = f(-v), we can safe quite some calculations */
	transformed_cube_model_[i+newsize_*(j+(model_).size_y*((model_).size_v-k))][0] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*((model_).size_v-k))][0];
	transformed_cube_model_[i+newsize_*(j+(model_).size_y*((model_).size_v-k))][1] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*((model_).size_v-k))][1];
      }
    }
  }
  
      /* Now do the backtransformation */
      fftwf_execute(plin_model_);    
    
  return &model_; 
  
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Convolve a cube with a gaussian via fft */

static Cube *convolgaussfft_here_single(void)
{
  int i, j;
  float expresult;                 /* A dummy */

      /* Now do the transform */
      fftwf_execute(plan_model_);
      
      /* multiply with the gaussian, first axis y, second x */
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
      for (i = 0; i < newsize_; ++i) {
	for (j = 0; j < (model_).size_y; ++j) {
	  expresult = fftgaussian2d((i <= cubesizexhalf_) ? i : (i-(model_).size_x), (j <= cubesizeyhalf_) ? j : (j-(model_).size_y), expofacsfft_);
	  transformed_cube_model_[i+newsize_*j][0] = expresult*transformed_cube_model_[i+newsize_*j][0];
	  transformed_cube_model_[i+newsize_*j][1] = expresult*transformed_cube_model_[i+newsize_*j][1];
	  
	}
      }
      
      /* Now do the backtransformation */
      fftwf_execute(plin_model_);    
  return &model_; 
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Convolve the input cube with a gaussian via fft to the weightmap, adding a constant offset */

static Cube *convolgaussfft_noise(void)
{
  int i, j, k;
  float expresult;                 /* A dummy */
     
      /* Now do the transform */
      fftwf_execute(plan_noise_);
/*       fftwf_execute(plin_noise_); */
/*       return NULL; */
      /* multiply with the gaussian, first for nu_v = 0 */

#ifdef OPENMPTIR
#pragma omp parallel for
#endif
      for (i = 0; i < newsize_; ++i) {
	for (j = 0; j < model_.size_y; ++j) {
	  
	  /* The exponential will be evaluated from 0, ... , N/2 and -1, ..., -N/2 or -N/2 - 1 */
	  expresult = fftgaussian2d((i <= cubesizexhalf_) ? i : (i-model_.size_x), (j <= cubesizeyhalf_) ? j : (j-model_.size_y), expofacsfft_noise_);
	  transformed_cube_noise_[i+newsize_*j][0] = expresult*transformed_cube_noise_[i+newsize_*j][0];
	  transformed_cube_noise_[i+newsize_*j][1] = expresult*transformed_cube_noise_[i+newsize_*j][1];
	}
      }
      
      /* Check for an extra-axis in v, i.e. if the dimension in v is even, we have to calculate one v-plane separately */
      if (!(model_.size_v % 2)) {
	/* multiply with the gaussian, first for nu_v = N_v/2 */
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
	for (i = 0; i < newsize_; ++i) {
	  for (j = 0; j < model_.size_y; ++j)
	    {
	      /* The exponential will be evaluated from 0, ... , N/2 and -1, ..., -N/2 or -N/2 - 1 */
	      expresult = fftgaussian((i <= cubesizexhalf_) ? i : (i-model_.size_x), (j <= cubesizeyhalf_) ? j : (j-model_.size_y), dummy_, expofacsfft_noise_, veloarray_noise_);
	      transformed_cube_noise_[i+newsize_*(j+model_.size_y*dummy_)][0] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*dummy_)][0];
	      transformed_cube_noise_[i+newsize_*(j+model_.size_y*dummy_)][1] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*dummy_)][1];
	    }
	}
      }
      
      /* Now the rest has to be done, v ranges from 1, ..., N_v-1/2, and using the symmetrics of the gaussian we fill the rest */
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
      for (i = 0; i < newsize_; ++i) {
	for (j = 0; j < model_.size_y; ++j) {
	  for (k = 1; k <= (model_.size_v-1)/2; ++k) {
	    expresult = fftgaussian((i <= cubesizexhalf_) ? i : (i-model_.size_x), (j <= cubesizeyhalf_) ? j : (j-model_.size_y), k, expofacsfft_noise_, veloarray_noise_);
	    transformed_cube_noise_[i+newsize_*(j+model_.size_y*k)][0] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*k)][0];
	    transformed_cube_noise_[i+newsize_*(j+model_.size_y*k)][1] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*k)][1];
	    
	    /* Because of the symmetry, f(v) = f(-v), we can safe quite some calculations */
	    transformed_cube_noise_[i+newsize_*(j+model_.size_y*(model_.size_v-k))][0] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*(model_.size_v-k))][0];
	    transformed_cube_noise_[i+newsize_*(j+model_.size_y*(model_.size_v-k))][1] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*(model_.size_v-k))][1];
	  }
	}
      }
      /* Now add the constant square of the noise */
      transformed_cube_noise_[0][0] = transformed_cube_noise_[0][0] + noise_.scale;
      
      /* Now do the backtransformation */
      fftwf_execute(plin_noise_);    
    
    
    
    return &noise_; 

}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Convolve the input cube with a gaussian via fft to the weightmap, adding a constant offset */

static Cube *convolgaussfft_noise_single(void)
{
  int i, j;
  float expresult;                 /* A dummy */
    

      /* Now do the transform */
      fftwf_execute(plan_noise_);
      
      /* multiply with the gaussian, first axis y, second x */
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
      for (i = 0; i < newsize_; ++i) {
	for (j = 0; j < model_.size_y; ++j) {
	  expresult = fftgaussian2d((i <= cubesizexhalf_) ? i : (i-model_.size_x), (j <= cubesizeyhalf_) ? j : (j-model_.size_y), expofacsfft_noise_);
	  transformed_cube_noise_[i+newsize_*j][0] = expresult*transformed_cube_noise_[i+newsize_*j][0];
	  transformed_cube_noise_[i+newsize_*j][1] = expresult*transformed_cube_noise_[i+newsize_*j][1];
	  
	}
      }
      
      /* Now add the constant square of the noise */
      transformed_cube_noise_[0][0] = transformed_cube_noise_[0][0] + noise_.scale;
      
      /* Now do the backtransformation */
      fftwf_execute(plin_noise_);    
    
    
    return &noise_; 

}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Calculate factors needed by convolgaussfft */

static float *expofacsfft_here(float sigma_maj, float sigma_min, float *sincosofangle)
{
  float *expofacs;

  if ((sincosofangle && (expofacs = (float *) malloc(5*sizeof(float))))) {

  /* First content is the factor to put before (n_x/N_x)^2 */
  expofacs[0] = -2*PI_HERE*PI_HERE*(sigma_min*sigma_min*sincosofangle[1]*sincosofangle[1]+sigma_maj*sigma_maj*sincosofangle[0]*sincosofangle[0])/(original_.size_x*original_.size_x);

  /* Second content is the factor to put before (n_x/N_x)(n_y/N_y) */
  expofacs[1] = -4*PI_HERE*PI_HERE*sincosofangle[0]*sincosofangle[1]*(sigma_min*sigma_min-sigma_maj*sigma_maj)/(original_.size_x*original_.size_y);

  /* Third content is the factor to put before (n_y/N_y)^2 */
  expofacs[2] = -2*PI_HERE*PI_HERE*(sigma_min*sigma_min*sincosofangle[0]*sincosofangle[0]+sigma_maj*sigma_maj*sincosofangle[1]*sincosofangle[1])/(original_.size_y*original_.size_y);

  /* Fifth component is the normalisation factor due to the width of the gaussians. This is not a factor to put in the exponent though. Here we have to care if only one direction conovolution is desired */
    if (sigma_maj == 0) 
      sigma_maj = 1.0/sqrtf(2*PI_HERE);
    
    if (sigma_min == 0)
      sigma_min = 1.0/sqrtf(2*PI_HERE);

    expofacs[4] = original_.scale*2*PI_HERE*sigma_min*sigma_maj/(original_.size_v*original_.size_y*original_.size_x);
  }
  else
    expofacs = NULL;

  return expofacs;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Calculate factors needed by convolgaussfft */

static void changeexpofacsfft_noise(float sigma_v)
{
  int i;
  /* Fourth content is the factor to put before (n_v/N_v)^2 */
  expofacsfft_noise_[3] = sigma_v*sigma_v*noiseconstant_1_;
  if ((sigma_v)) {
    expofacsfft_noise_[4] = noiseconstant_2_/sigma_v;
    /* Now fill the veloarray */
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
    for (i = 0; i < model_.size_v/2+1; ++i) {
      veloarray_noise_[i] = expf(expofacsfft_noise_[3]*i*i)*expofacsfft_noise_[4];
    }
  }
  else {
    expofacsfft_noise_[4] = 2*SQRTPI*noiseconstant_2_;
   /* Now fill the veloarray */
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
    for (i = 0; i < model_.size_v/2+1; ++i) {
      veloarray_noise_[i] = expf(expofacsfft_noise_[3]*i*i)*expofacsfft_noise_[4];
    }
  }
  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Calculate factors needed by convolgaussfft */

static void changeexpofacsfft(float sigma_v)
{
  int i;
/* Fourth content is the factor to put before (n_v/N_v)^2 */
  expofacsfft_[3] = modelconstant_1_*sigma_v*sigma_v;

  /* Now fill the veloarray */
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
    for (i = 0; i < model_.size_v/2+1; ++i) {
      veloarray_[i] = expf(expofacsfft_[3]*i*i)*expofacsfft_[4];
    }
  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Calculate a gaussian */

static float fftgaussian (int nx, int ny, int nv, float *expofacs, float *veloarray)
{ 
  /* As the trial to safe some time as seen below failed for some reason, we postpone it */

    return expf(expofacs[0]*nx*nx+expofacs[1]*nx*ny+expofacs[2]*ny*ny)*veloarray[nv];
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Calculate a gaussian */

static float fftgaussian_array (int nx, int ny, int nv, float *expofacs, float *array, float *veloarray)
{ 
  /* As the trial to safe some time as seen below failed for some reason, we postpone it */

/*     return array[nx+expcube_noise_.size_x*ny]*expf(expofacs[3]*nv*nv) * expofacs[4]; */
    return array[nx+expcube_noise_.size_x*ny]*veloarray[nv];
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Calculate a gaussian */

static float fftgaussian2d (int nx, int ny, float *expofacs)
{
/*   double number; */

  /* If the floating point is low enough, we can simply return 0 */
/*   if ((number = expf(expofacs[0]*nx*nx+expofacs[1]*nx*ny+expofacs[2]*ny*ny+expofacs[4])) <= MINEXPONENT) */
/*     return (float) exp(number); */
/*   else */
    return expf(expofacs[0]*nx*nx+expofacs[1]*nx*ny+expofacs[2]*ny*ny)*expofacs[4];
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Calculate a gaussian */

static float fftgaussian2d_array (int nx, int ny, float *expofacs, float *array)
{
  return array[nx+expcube_noise_.size_x*ny]*expofacs[4];
}

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

/* Translate from degree to radians */

static float degreetoradian(float degrees)
{
  return PI_HERE*degrees/180.0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Interface to get a cube out of engalmod */

Cube *getoriginal_galmod_(void)
{
  return &original_;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Interface to get a cube out of engalmod */

Cube *getmodel_galmod_(void)
{
  return &model_;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Interface to get a cube out of engalmod */

Cube *getnoise_galmod_(void)
{
  return &noise_;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

static void makemodelarray(float *array)
{
  int i,j;
  int nx, ny;
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
  for (i = 0; i < expcube_model_.size_x; ++i) {
    for (j = 0; j < expcube_model_.size_y; ++j) {
      nx = (i <= cubesizexhalf_) ? i : (i-(model_).size_x);
      ny = (j <= cubesizeyhalf_) ? j : (j-(model_).size_y);
      expcube_model_.points[i+expcube_model_.size_x*j] = expofacsfft_[0]*nx*nx+expofacsfft_[1]*nx*ny+expofacsfft_[2]*ny*ny;
    }
  }
  
  for (i = 0; i < expcube_model_.size_x; ++i) {
    for (j = 0; j < expcube_model_.size_y; ++j) {
      expcube_model_.points[i+expcube_model_.size_x*j] = expf(array[i+expcube_model_.size_x*j]);
    }
  }

  return;
}

/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Convolve a cube with a gaussian via fft */

static Cube *convolgaussfft_here_array(void)
{
  int i, j, k;
  float expresult;                 /* A dummy */

  /* Convolution in all dimensions or in xy only */

 
  /* Now do the transform */
  fftwf_execute(plan_model_);
  
  /* multiply with the gaussian, first for nu_v = 0 */
/*   #ifdef OPENMPTIR */
/*   #pragma omp parallel for */
/*   #endif */
  for (i = 0; i < newsize_; ++i) {
    for (j = 0; j < (model_).size_y; ++j) {
      
      /* The exponential will be evaluated from 0, ... , N/2 and -1, ..., -N/2 or -N/2 - 1 */
      expresult = fftgaussian2d_array(i, j, expofacsfft_, expcube_model_.points);
      transformed_cube_model_[i+newsize_*j][0] = expresult*transformed_cube_model_[i+newsize_*j][0];
      transformed_cube_model_[i+newsize_*j][1] = expresult*transformed_cube_model_[i+newsize_*j][1];
    }
  }
  
/*   convolgaussfft_here_array_help1(); */
   /* Check for an extra-axis in v, i.e. if the dimension in v is even, we have to calculate one v-plane separately */
  if (!((model_).size_v % 2)) {
    /* multiply with the gaussian, first for nu_v = N_v/2 */
    #ifdef OPENMPTIR
    /* pragma omp parallel for */
    #endif
    for (i = 0; i < newsize_; ++i) {
      for (j = 0; j < (model_).size_y; ++j) {
	/* The exponential will be evaluated from 0, ... , N/2 and -1, ..., -N/2 or -N/2 - 1 */
	expresult = fftgaussian_array(i, j, dummy_, expofacsfft_, expcube_model_.points, veloarray_);
	transformed_cube_model_[i+newsize_*(j+(model_).size_y*dummy_)][0] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*dummy_)][0];
	transformed_cube_model_[i+newsize_*(j+(model_).size_y*dummy_)][1] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*dummy_)][1];
      }
    }
  }

  /* Now the rest has to be done, v ranges from 1, ..., N_v-1/2, and using the symmetrics of the gaussian we fill the rest */
/*   #ifdef OPENMPTIR */
/* !!! pragma omp parallel for */
/*   #endif */
  for (i = 0; i < newsize_; ++i) {
    for (j = 0; j < (model_).size_y; ++j) {
      for (k = 1; k <= ((model_).size_v-1)/2; ++k) {
	expresult = fftgaussian_array(i, j, k, expofacsfft_, expcube_model_.points, veloarray_);
	transformed_cube_model_[i+newsize_*(j+(model_).size_y*k)][0] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*k)][0];
	transformed_cube_model_[i+newsize_*(j+(model_).size_y*k)][1] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*k)][1];
	transformed_cube_model_[i+newsize_*(j+(model_).size_y*((model_).size_v-k))][0] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*((model_).size_v-k))][0];
	transformed_cube_model_[i+newsize_*(j+(model_).size_y*((model_).size_v-k))][1] = expresult*transformed_cube_model_[i+newsize_*(j+(model_).size_y*((model_).size_v-k))][1];
      }
    }
  }

  /* Now do the backtransformation */
  fftwf_execute(plin_model_);    
    
  return &model_; 
  
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Convolve a cube with a gaussian via fft */

static Cube *convolgaussfft_here_single_array(void)
{
  int i, j;
  float expresult;                 /* A dummy */

      /* Now do the transform */
      fftwf_execute(plan_model_);
      
      /* multiply with the gaussian, first axis y, second x */
/* #ifdef OPENMPTIR */
/* !!! pragma omp parallel for */
/* #endif */
      for (i = 0; i < newsize_; ++i) {
	for (j = 0; j < (model_).size_y; ++j) {
	  expresult = fftgaussian2d_array(i, j, expofacsfft_, expcube_model_.points);
	  transformed_cube_model_[i+newsize_*j][0] = expresult*transformed_cube_model_[i+newsize_*j][0];
	  transformed_cube_model_[i+newsize_*j][1] = expresult*transformed_cube_model_[i+newsize_*j][1];
	  
	}
      }
      
      /* Now do the backtransformation */
      fftwf_execute(plin_model_);    
  return &model_; 
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Convolve the input cube with a gaussian via fft to the weightmap, adding a constant offset */

static Cube *convolgaussfft_noise_array(void)
{
  int i, j, k;
  float expresult;                 /* A dummy */
     
      /* Now do the transform */
      fftwf_execute(plan_noise_);
/*       fftwf_execute(plin_noise_); */
/*       return NULL; */
      /* multiply with the gaussian, first for nu_v = 0 */

#ifdef OPENMPTIR
/* pragma omp parallel for */
#endif
      for (i = 0; i < newsize_; ++i) {
	for (j = 0; j < model_.size_y; ++j) {
	  
	  /* The exponential will be evaluated from 0, ... , N/2 and -1, ..., -N/2 or -N/2 - 1 */
	  expresult = fftgaussian2d_array(i, j, expofacsfft_noise_, expcube_noise_.points);
	  transformed_cube_noise_[i+newsize_*j][0] = expresult*transformed_cube_noise_[i+newsize_*j][0];
	  transformed_cube_noise_[i+newsize_*j][1] = expresult*transformed_cube_noise_[i+newsize_*j][1];
	}
      }
      
    if (!(model_.size_v % 2)) {
    /* multiply with the gaussian, first for nu_v = N_v/2 */
#ifdef OPENMPTIR
/* pragma omp parallel for */
#endif
    for (i = 0; i < newsize_; ++i) {
      for (j = 0; j < model_.size_y; ++j)
	{
	  /* The exponential will be evaluated from 0, ... , N/2 and -1, ..., -N/2 or -N/2 - 1 */
	  expresult = fftgaussian_array(i,j, dummy_, expofacsfft_noise_, expcube_noise_.points, veloarray_noise_);
	      transformed_cube_noise_[i+newsize_*(j+model_.size_y*dummy_)][0] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*dummy_)][0];
	      transformed_cube_noise_[i+newsize_*(j+model_.size_y*dummy_)][1] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*dummy_)][1];
	}
    }
  }

      /* Now the rest has to be done, v ranges from 1, ..., N_v-1/2, and using the symmetrics of the gaussian we fill the rest */
/* #ifdef OPENMPTIR */
/* !!! pragma omp parallel for */
/* #endif */
      for (i = 0; i < newsize_; ++i) {
	for (j = 0; j < model_.size_y; ++j) {
	  for (k = 1; k <= (model_.size_v-1)/2; ++k) {
	    expresult = fftgaussian_array(i,j, k, expofacsfft_noise_,expcube_noise_.points, veloarray_noise_);
	    transformed_cube_noise_[i+newsize_*(j+model_.size_y*k)][0] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*k)][0];
	    transformed_cube_noise_[i+newsize_*(j+model_.size_y*k)][1] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*k)][1];
	    
	    /* Because of the symmetry, f(v) = f(-v), we can safe quite some calculations */
	    transformed_cube_noise_[i+newsize_*(j+model_.size_y*(model_.size_v-k))][0] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*(model_.size_v-k))][0];
	    transformed_cube_noise_[i+newsize_*(j+model_.size_y*(model_.size_v-k))][1] = expresult*transformed_cube_noise_[i+newsize_*(j+model_.size_y*(model_.size_v-k))][1];
	  }
	}
      }
      /* Now add the constant square of the noise */
      transformed_cube_noise_[0][0] = transformed_cube_noise_[0][0] + noise_.scale;
      
      /* Now do the backtransformation */
      fftwf_execute(plin_noise_);    
    
    
    
    return &noise_; 

}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Convolve the input cube with a gaussian via fft to the weightmap, adding a constant offset */

static Cube *convolgaussfft_noise_single_array(void)
{
  int i, j;
  float expresult;                 /* A dummy */
    

      /* Now do the transform */
      fftwf_execute(plan_noise_);
      
      /* multiply with the gaussian, first axis y, second x */
#ifdef OPENMPTIR
/* pragma omp parallel for */
#endif
      for (i = 0; i < newsize_; ++i) {
	for (j = 0; j < model_.size_y; ++j) {
	  expresult = fftgaussian2d_array(i,j, expofacsfft_noise_, expcube_noise_.points);
	  transformed_cube_noise_[i+newsize_*j][0] = expresult*transformed_cube_noise_[i+newsize_*j][0];
	  transformed_cube_noise_[i+newsize_*j][1] = expresult*transformed_cube_noise_[i+newsize_*j][1];
	  
	}
      }
      
      /* Now add the constant square of the noise */
      transformed_cube_noise_[0][0] = transformed_cube_noise_[0][0] + noise_.scale;
      
      /* Now do the backtransformation */
      fftwf_execute(plin_noise_);    
    
    
    return &noise_; 

}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

static void makenoisearray(float *array)
{
  int i,j;
  int nx, ny;
#ifdef OPENMPTIR
#pragma omp parallel for
#endif
  for (i = 0; i < expcube_noise_.size_x; ++i) {
    for (j = 0; j < expcube_noise_.size_y; ++j) {
      nx = (i <= cubesizexhalf_) ? i : (i-(noise_).size_x);
      ny = (j <= cubesizeyhalf_) ? j : (j-(noise_).size_y);
      expcube_noise_.points[i+expcube_noise_.size_x*j] = expofacsfft_noise_[0]*nx*nx+expofacsfft_noise_[1]*nx*ny+expofacsfft_noise_[2]*ny*ny;
    }
  }

  /* This is maybe not elegant but may be safe */
/* #ifdef OPENMPTIR */
/* !!! pragma omp parallel for */
/* #endif */
  for (i = 0; i < expcube_noise_.size_x; ++i) {
    for (j = 0; j < expcube_noise_.size_y; ++j) {
      expcube_noise_.points[i+expcube_noise_.size_x*j] = expf(array[i+expcube_noise_.size_x*j]);
    }
  }
  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Synonyme of fftwf_malloc */
void *malloc_engalmod(size_t size)
{
  return fftwf_malloc(size);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Synonyme of fftwf_free */
void free_engalmod(void *array)
{
  fftwf_free(array);
  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: engalmod.c,v $
   Revision 1.44  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.43  2011/05/11 13:37:12  jozsa
   Left work

   Revision 1.42  2011/05/10 00:30:15  jozsa
   Left work

   Revision 1.41  2009/05/26 07:56:40  jozsa
   Left work

   Revision 1.40  2007/08/22 15:58:40  gjozsa
   Left work

   Revision 1.39  2006/04/07 11:13:32  gjozsa
   simple BUGFIX

   Revision 1.38  2006/04/06 10:39:09  gjozsa
   Included function engalmod_chflgs

   Revision 1.37  2006/04/03 11:47:46  gjozsa
   included masking, fixed a mask to be present if pixval < -1024

   Revision 1.36  2005/04/20 13:26:24  gjozsa
   Left work

   Revision 1.35  2005/04/12 15:52:14  gjozsa
   Left work

   Revision 1.34  2005/04/07 12:45:47  gjozsa
   Bugfix

   Revision 1.33  2005/04/06 05:58:24  gjozsa
   Bugfix: init now corrects the noiseweight to 1 in case of mode%2

   Revision 1.32  2005/04/04 08:42:19  gjozsa
   Left work

   Revision 1.31  2005/04/01 12:37:11  gjozsa
   Large improvements, repeated calls with same velocity dispersion are much faster

   Revision 1.29  2005/03/11 17:45:54  gjozsa
   Left work

   Revision 1.28  2005/03/04 18:13:53  gjozsa
   Left work

   Revision 1.27  2005/03/02 17:56:09  gjozsa
   Left work

   Revision 1.26  2005/01/17 12:13:34  gjozsa
   Left work

   Revision 1.25  2005/01/06 10:44:10  gjozsa
   Left work

   Revision 1.24  2005/01/05 15:33:02  gjozsa
   Left work

   Revision 1.23  2004/12/30 13:36:05  gjozsa
   Added probability evaluation and out-of-place fft

   Revision 1.22  2004/12/27 12:54:40  gjozsa
   Last updatde before commenting, no changes anymore allowed

   Revision 1.21  2004/12/23 20:20:50  gjozsa
   some minor changes, leaves the implementation of arbitrary arrays

   Revision 1.18  2004/12/22 17:33:57  gjozsa
   Left work

   Revision 1.14  2004/12/21 18:42:12  gjozsa
   Left work

   Revision 1.10  2004/12/21 17:50:21  gjozsa
   some changes

   Revision 1.7  2004/12/20 14:55:58  gjozsa
   Left work

   Revision 1.5  2004/12/20 10:44:12  gjozsa
   added

   Revision 1.4  2004/12/17 14:13:40  gjozsa
   First debugged running version

   Revision 1.3  2004/12/16 13:19:51  gjozsa
   Left work

   Revision 1.1  2004/12/11 17:44:51  gjozsa
   Added to CVS control


   ------------------------------------------------------------ */

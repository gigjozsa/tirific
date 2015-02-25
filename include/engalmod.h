/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file engalmod.h 

   @brief c-routines for function calls from Fortran in an enhanced
   galmod environment

   This module contains three functions that provide a very
   specialised chisquare evaluation when comparing an observed cube
   with a cube that is passed by galmod of the GIPSY environment. The
   c functions are meant for a hack into galmod and therefore have as
   such no use. There is one initialiser routine that initialises the
   functionality, i.e., allocates memory, when needed and safes
   variables that don't change if a lot of models are passed to the
   second routine the chisquare evaluation routine. This does nothing
   but give back a chisquare when comparing the original cube and the
   model. Both are passed as float arrays only.

   This is the final stable version, all tests are passed. The source
   code is a mess with respect to documentation, I fear, but I spend a
   lot in optimising all the routines, such that even a very silly
   compiler produces a good binary. No need to touch it again,
   hopefully.

   Important remark: All functions are robust against the change of
   the parameters passed in, except the adresses of model and
   original, which was formerly not the case. They now use local
   copies of the variables the adress of which was passed to the
   module. The strange form of the functions is due to their purpose
   of being called in a fortran code.

   Compiling and linking:
   Given (No need for the bracketed lines)
   fftw3 include files reside in $(FFTW3INCLUDE)
   fftw3f library resides in $(FFTW3LIB)
   (   gsl include files reside in $(GSLINCLUDE)/gsl   )
   (   gsl library resides in $(GSLLIB)   )

   Compile with the flags

   -I$(FFTW3INCLUDE)   (   -I$(GSLINCLUDE)   )

   Link with the flags

   -L$(FFTW3LIB) -lfftw3f   (   -lgsl -lgslcblas   )

   (   Instead of -lgslcblas you can try -lcblas   )


   $Source: /Volumes/DATA_J_II/data/CVS/tirific/include/engalmod.h,v $
   $Date: 2011/05/11 13:37:11 $
   $Revision: 1.22 $
   $Author: jozsa $
   $Log: engalmod.h,v $
   Revision 1.22  2011/05/11 13:37:11  jozsa
   Left work

   Revision 1.21  2009/05/26 07:56:39  jozsa
   Left work

   Revision 1.20  2007/08/22 15:58:33  gjozsa
   Left work

   Revision 1.19  2006/04/06 10:39:09  gjozsa
   Included function engalmod_chflgs

   Revision 1.18  2006/04/03 11:47:46  gjozsa
   included masking, fixed a mask to be present if pixval < -1024

   Revision 1.17  2005/04/14 10:32:11  gjozsa
   Left work

   Revision 1.16  2005/04/12 15:52:13  gjozsa
   Left work

   Revision 1.15  2005/03/04 18:13:52  gjozsa
   Left work

   Revision 1.14  2005/03/02 17:56:09  gjozsa
   Left work

   Revision 1.13  2005/01/17 12:13:34  gjozsa
   Left work

   Revision 1.12  2005/01/06 15:37:14  gjozsa
   Left work

   Revision 1.10  2005/01/05 15:33:00  gjozsa
   Left work

   Revision 1.9  2004/12/30 13:36:06  gjozsa
   Added probability evaluation and out-of-place fft

   Revision 1.7  2004/12/22 14:53:11  gjozsa
   Left work

   Revision 1.6  2004/12/21 17:23:28  gjozsa
   Added noiseweight and made convolution slightly faster

   Revision 1.5  2004/12/20 14:55:58  gjozsa
   Left work

   Revision 1.3  2004/12/16 13:19:49  gjozsa
   Left work

   
*/
/* ------------------------------------------------------------ */


/* Include guard */
#ifndef ENGALMOD_H
#define ENGALMOD_H



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <stdlib.h>


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <cubarithm.h>


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* MACROS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* TYPEDEFS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* STRUCTS */
/* ------------------------------------------------------------ */


#define CUBE_DEFINED
#ifndef CUBE_DEFINED
#define CUBE_DEFINED

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct Cube
   @brief A datacube with absolute position information

   A datacube. According to the internal coordinate structure its
   position is determined by the reference pixel (the coordinates of
   the first pixel points[0] in the adopted coordinate system. The
   left lowest pixel of the reference cube is the origin) and the
   size. The array with the data is ordered not exactly according to
   fits rules. x is axis1, y axis2, and v axis 3, i.e. pixel (x,y,z)
   is points[x+cube.size_x*(y+cube.size_y*z)].  Will be defined only
   if not already defined. After testing the structure can be removed
   and put into the .c file.

   Layout: 
   @li @c int @c refpix_x         x-coordinate of the reference pixel
   @li @c int @c refpix_y         y-coordinate of the reference pixel
   @li @c int @c refpix_v         v-coordinate of the reference pixel
	    
   @li @c int @c size_x           size in pixels in x direction
   @li @c int @c size_y           size in pixels in y direction
   @li @c int @c size_v           size in pixels in v direction

   @li @c float @c scale          a scale factor
   @li @c int @c padding          number of pixels the cube is padded with
   @li @c float @c *points        array of floats containing the information
*/
/* ------------------------------------------------------------ */
typedef struct Cube
{
  /** @brief x-coordinate of the reference pixel */
  int refpix_x;
  /** @brief y-coordinate of the reference pixel */
  int refpix_y;
  /** @brief v-coordinate of the reference pixel */
  int refpix_v;

  /** @brief size in pixels in x direction */
  int size_x;
  /** @brief size in pixels in y direction */
  int size_y;
  /** @brief size in pixels in v direction */
  int size_v;

  /** @brief a scale factor */
  float scale;
  /** @brief number of pixels the cube is padded with */
  int padding;

  /** @brief array of floats containing the information */
  float *points;
} Cube;

#endif

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int initchisquare_((float *arrayorig, float *arraymodel, int
  *x, int *y, int *v, float *hpbwmaj, float *hpbwmin, float *pa, float
  *scale, float *flux, float *sigma, int *mode, int *arrayvsize,
  double *chisquare, float *noiseweight, int *inimode, int *threads)

  @brief Initializes the chisquare derivation control

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

  Masks have been introduced in a simple way. If a pixel in the
  original cube is assigned a value < 1024, then it will not count in
  the chisquare evaluation.

  Note: If the input array is changed after initialisation and flags
  are present in the new cube, run engalmod_chflgs() in order to
  initialise the proper chisquare evaluation.
  
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
int initchisquare_(float *arrayorig, float *arraymodel,int *x, int *y, int *v, float *hpbwmaj, float *hpbwmin, float *pa, float *scale, float *flux, float *sigma, int *mode, int *arrayvsize, double *chisquare, float *noiseweight, int *inimode, int *threads);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int initchisquare_c(float *arrayorig, float *arraymodel, int x, int y, int v, float hpbwmaj, float hpbwmin, float pa, float scale, float flux, float sigma, int mode, int arrayvsize, double *chisquare, float noiseweight, int inimode, int threads)

   @brief Same as initchisquare, only for c

  @param arrayorig  (*float)    Array corresponding to the original cube
  @param arraymodel (*float)    Array corresponding to the model (pointsource) cube
  @param x          (int)     Size of logical array in x (that is regarded in calculation)
  @param y          (int)     Size fo logical array in y
  @param v          (int)     Size fo logical array in v
  @param hpbwmaj    (float)   HPBW of the gaussian beam, major axis
  @param hpbwmin    (float)   HPBW of the gaussian beam, minor axis
  @param pa         (float)   Position angle of the gaussian beam
  @param scale      (float)   Scale factor to scale model by to match original
  @param flux       (float)   The flux of one pointsource in galmod
  @param sigma      (float)   Sigma rms in the original
  @param mode       (int)     Calculation of the chisquare depends also on quantisation noise (1 or 3) or on the sigma rms in the original alone (0 or 2) If set to 2 or 3 a bit more memory is needed but the routine runs a bit faster.
  @param arrayxsize  (int)    Physical size of reserved arrays of model and original in x, a dummy, is always 2*((int) *x/2+1)
  @param chisquare   (double *) Pointer to the variable containing the chisquare used throughout the code
  @param noiseweight (float)  Parameter used for weighting quantisation noise
  @param inimode     (int)    Mode for the determination of the best fft.
  @param threads     (int)    Number of threads.
*/
/* ------------------------------------------------------------ */
int initchisquare_c(float *arrayorig, float *arraymodel, int x, int y, int v, float hpbwmaj, float hpbwmin, float pa, float scale, float flux, float sigma, int mode, int arrayvsize, double *chisquare, float noiseweight, int inimode, int threads);




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void engalmod_chflgs(void)

  @brief (Re-)Initialisation of the chisquare finding routine

  Two chisquare evaluations are made, one slightly slower if flags are
  present, one faster, if they are not. If the input array gets
  changed after initialisation with initchisquare, this routine has to
  be run.

  @return void
*/
/* ------------------------------------------------------------ */
void engalmod_chflgs(void);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn double getchisquare_(float *array, float *HPBW_v)

  @brief Chisquare calculation from a model

  Links the array to the model_.points member, and calculates the
  chisquare by comparing it to the original_.points array. For this
  the array expofacsfft_ and (optionally, if mode == 1) the array
  expofacsfft_noise_ is initialised. Then, if mode == 1, the weight
  map is calculated. The model is convolved, and the Chisquare is
  calculated (See initchisquare2_.

  @param sigma_v     (float *) The input array

  @return (success) double getchisquare: The chisquare
*/
/* ------------------------------------------------------------ */
double getchisquare_ (float *sigma_v);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn double getchisquare_c(float *HPBW_v)

  @brief Chisquare calculation from a model

  Links the array to the model_.points member, and calculates the
  chisquare by comparing it to the original_.points array. For this
  the array expofacsfft_ and (optionally, if mode == 1) the array
  expofacsfft_noise_ is initialised. Then, if mode == 1, the weight
  map is calculated. The model is convolved, and the Chisquare is
  calculated (See initchisquare2_.

  @param sigma_v     (float) The velocity dispersion

  @return (success) double getchisquare: The chisquare
*/
/* ------------------------------------------------------------ */
double getchisquare_c (float sigma_v);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn double getproba_(double *chisquare, int *degrees_of_freedom)

  @brief Calculates the chisquare probability density from the
  chisquare and the degrees of freedom

  (Currently not implemented, this is just to preserve the function name)

  @param chisquare          (double *) The chisquare
  @param degrees_of_freedom (int *)    

  @return (success) double getproba_: The probability of the chisquare
*/
/* ------------------------------------------------------------ */
double getproba_(double *chisquare, double *degrees_of_freedom);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Interface functions for testing routines */

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn Cube *getmodel_galmod_(void)

  @brief Interface to get a cube out of engalmod

  Returns a pointer to the private global Cube model_ in engalmod

  @return (success) Cube *getmodel_galmod_: The model
*/
/* ------------------------------------------------------------ */
Cube *getmodel_galmod_(void);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn Cube *getoriginal_galmod_(void)

  @brief Interface to get a cube out of engalmod

  Returns a pointer to the private global Cube original_ in engalmod

  @return (success) Cube *getmodel_galmod_: The model
*/
/* ------------------------------------------------------------ */
Cube *getoriginal_galmod_(void);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn Cube *getnoise_galmod_(void)

  @brief Interface to get a cube out of engalmod

  Returns a pointer to the private global Cube noise_ in engalmod

  @return (success) Cube *getmodel_galmod_: The model
*/
/* ------------------------------------------------------------ */
Cube *getnoise_galmod_(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void *malloc_engalmod(size_t size)

  @brief Synonyme of fftwf_malloc

  This function can be called without needing to have the fftw header
  files included

  @param size (size_t) Size of array to be allocated

  @return (success) void *malloc_engalmod: Allocated array
          (error) NULL
*/
/* ------------------------------------------------------------ */
void *malloc_engalmod(size_t size);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void free_engalmod(void *array)

  @brief Synonyme of fftwf_free

  This function can be called without needing to have the fftw header
  files included

  @param array (void *) Array to be freed

  @return void
*/
/* ------------------------------------------------------------ */
void free_engalmod(void *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: engalmod.h,v $
   Revision 1.22  2011/05/11 13:37:11  jozsa
   Left work

   Revision 1.21  2009/05/26 07:56:39  jozsa
   Left work

   Revision 1.20  2007/08/22 15:58:33  gjozsa
   Left work

   Revision 1.19  2006/04/06 10:39:09  gjozsa
   Included function engalmod_chflgs

   Revision 1.18  2006/04/03 11:47:46  gjozsa
   included masking, fixed a mask to be present if pixval < -1024

   Revision 1.17  2005/04/14 10:32:11  gjozsa
   Left work

   Revision 1.16  2005/04/12 15:52:13  gjozsa
   Left work

   Revision 1.15  2005/03/04 18:13:52  gjozsa
   Left work

   Revision 1.14  2005/03/02 17:56:09  gjozsa
   Left work

   Revision 1.13  2005/01/17 12:13:34  gjozsa
   Left work

   Revision 1.12  2005/01/06 15:37:14  gjozsa
   Left work

   Revision 1.10  2005/01/05 15:33:00  gjozsa
   Left work

   Revision 1.9  2004/12/30 13:36:06  gjozsa
   Added probability evaluation and out-of-place fft

   Revision 1.7  2004/12/22 14:53:11  gjozsa
   Left work

   Revision 1.6  2004/12/21 17:23:28  gjozsa
   Added noiseweight and made convolution slightly faster

   Revision 1.5  2004/12/20 14:55:58  gjozsa
   Left work

   Revision 1.3  2004/12/16 13:19:49  gjozsa
   Left work


   ------------------------------------------------------------ */

/* Include guard */
#endif


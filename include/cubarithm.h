/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file cubarithm.h
   @brief Simple arithmetics with Cubes

   This module contains all mathematical operations done on
   cubes as a stand-alone.
   
   $Source: /Volumes/DATA_J_II/data/CVS/tirific/include/cubarithm.h,v $
   $Date: 2009/05/26 07:56:39 $
   $Revision: 1.3 $
   $Author: jozsa $
   $Log: cubarithm.h,v $
   Revision 1.3  2009/05/26 07:56:39  jozsa
   Left work

   Revision 1.2  2007/08/22 15:58:33  gjozsa
   Left work

   Revision 1.1.1.1  2004/10/29 11:13:05  gjozsa
   Added to CVS control


*/
/* ------------------------------------------------------------ */



/* Include guard */
#ifndef CUBARITHM_H
#define CUBARITHM_H


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <qfits.h>
/* #include <wcs.h> */
/* #include <wcshdr.h> */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def _MEMORY_HERE_ON
   @brief Controls the use of the memory_here module

   If you don't want to use the memory_here facility comment this
   define, otherways it will be included.

*/
/* ------------------------------------------------------------ */
#define _MEMORY_HERE_ON
/* #include <memory_here.h> */
/* #include <maths.h> */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */

#define CUBARITHM_CUBE_ERROR_NONE        0
#define CUBARITHM_CUBE_ERROR_READ        1
#define CUBARITHM_CUBE_ERROR_MEM         2
#define CUBARITHM_CUBE_ERROR_NONAXIS     3
#define CUBARITHM_CUBE_ERROR_WRONGNAXIS  4
#define CUBARITHM_CUBE_ERROR_NONAXISI    5
#define CUBARITHM_CUBE_ERROR_WRONGNAXISI 6
#define CUBARITHM_CUBE_ERROR_NOCRPIXI    7
#define CUBARITHM_CUBE_ERROR_NOCRVALI    8
#define CUBARITHM_CUBE_ERROR_NOCDELTI    9
#define CUBARITHM_CUBE_ERROR_NOCTYPEI    10
#define CUBARITHM_CUBE_ERROR_WRONGCTYPEI 11
#define CUBARITHM_CUBE_ERROR_NOEPOCH     12
#define CUBARITHM_CUBE_ERROR_WRONGEPOCH  13
#define CUBARITHM_CUBE_ERROR_WRONGBUNIT  14
#define CUBARITHM_CUBE_ERROR_WRONGBITPIX 15
#define CUBARITHM_CUBE_ERROR_WCS         16

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
   @struct Cube
   @brief A datacube with absolute position information

   A datacube. This data cube has always three axes. First is always RA, second always Dec, third always frequency type (velocity).

   According to the internal coordinate structure its
   position is determined by the reference pixel (the coordinates of
   the first pixel points[0] in the adopted coordinate system. The
   left lowest pixel of the reference cube is the origin) and the
   size. The array with the data is ordered not exactly according to
   fits rules. x is axis1, y axis2, and v axis 3, i.e. pixel (x,y,z)
   is points[x+cube.size_x*(y+cube.size_y*z)]

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
  /** @brief size in pixels in x direction, same as naxis1 in fits */
  int size_x;
  /** @brief size in pixels in y direction, same as naxis2 in fits */
  int size_y;
  /** @brief size in pixels in v direction, same as naxis3 in fits */
  int size_v;

  /** @brief x-coordinate of the reference pixel, same as crpix1 in fits */
  double refpix_x;
  /** @brief y-coordinate of the reference pixel, same as crpix2 in fits */
  double refpix_y;
  /** @brief v-coordinate of the reference pixel, same as crpix3 in fits */
  double refpix_v;

  /** @brief x-coordinate of the reference pixel, same as crval1 in fits (degrees) */
  double refval_x;
  /** @brief y-coordinate of the reference pixel, same as crval2 in fits  (degrees)*/
  double refval_y;
  /** @brief v-coordinate of the reference pixel, same as crval3 in fits  (m/s, Hz)*/
  double refval_v;

  /** @brief x-coordinate of the reference pixel, same as cdelt1 in fits  (degrees)*/
  double delt_x;
  /** @brief y-coordinate of the reference pixel, same as cdelt2 in fits  (degrees)*/
  double delt_y;
  /** @brief v-coordinate of the reference pixel, same as cdelt3 in fits  (m/s, Hz)*/
  double delt_v;

  /** @brief x-coordinate of the reference pixel, same as ctype1 in fits */
  char *type_x;
  /** @brief y-coordinate of the reference pixel, same as ctype2 in fits */
  char *type_y;
  /** @brief v-coordinate of the reference pixel, same as ctype3 in fits */
  char *type_v;

  /** @brief epoch in fits */
  char *epoch;
  /** @brief freq0 in Hz */
  double restfreq;
  /** @brief vobs in m/s */
  double vobs;
  /** @brief cellscal 1: 1/F, 0: CONST, might become useful in future */
  int cellscal;

  /** @brief major axis of beam, degrees */
  double bmaj;
  /** @brief minor axis of beam, degrees */
  double bmin;
  /** @brief beam position angle, degrees */
  double bpa;

  /** @brief a scale factor (bscale) */
  float scale;
  /** @brief bzero */
  float zero;
  /** @brief btype */
  char *type;
  /** @brief bunit */
  char *unit;

  /** @brief number of pixels the cube is padded with */
  int padding;

  /** @brief complete qfits header */
  qfits_header *header;

  /** @brief number of bytes required for this text header */
  size_t headerbytes;
  /** @brief ascii version of qfits header */
  char *asciiheader;

  /** @brief number of coordinate conversion structs in wcs */
  int nwcs;

  /** @brief wcs conversion struct array, void is equivalent to struct wcsprm */
  void *wcs;

  /** @brief total number of points in data cube (size_x*size_y*size_v) */
  int sumpoints;
  /** @brief array of floats containing the information */
  float *points;

} Cube;

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn Cube *cubarithm_cube_create(void)
    @brief Create a cube with all the pointers set to 0, and some allocations
    
    @return (success) Cube *cubarithm_cube_create allocated cube
            (error) NULL
*/
/* ------------------------------------------------------------ */
Cube *cubarithm_cube_create(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn void cubarithm_cube_destroy(Cube *cube)
    @brief Deallocate cube and all dependent structs
    
    @param cube (Cube *) The input cube
    
    @return void
*/
/* ------------------------------------------------------------ */
void cubarithm_cube_destroy(Cube *cube);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn void *cuberase(Cube *cube)
    @brief Setting all pixels in a Cube cube to 0.
    
    @param cube (Cube *) The input cube
    
    @return void
*/
/* ------------------------------------------------------------ */
void cuberase(Cube *cube);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn float *findpixel(Cube *cube, int x, int y, int v)
  @brief Finds the member of the array in cube corresponding to
  the absolute coordinates x, y, v.

  @param cube (Cube *) The input cube
  @param x    (int)    x coordinate
  @param y    (int)    y coordinate
  @param v    (int)    v coordinate

  @return (success) float *findpixel: Pointer to the pixel\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
float *findpixel(Cube *cube, double x, double y, double v);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn float *findpixelrel(Cube *cube, int x, int y, int v, int padding)
  @brief Find the relative pixel in a Cube

  Finds the member of the array in cube corresponding to the relative
  coordinates x, y, z with an explicite padding.

  @param cube    (Cube *) The input cube
  @param x       (int)    relative x coordinate
  @param y       (int)    relative y coordinate
  @param v       (int)    relative v coordinate
  @param padding (int)    Padding

  @return (success) float *findpixelrel: Pointer to the pixel\n
          (error) NULL

*/
/* ------------------------------------------------------------ */
float *findpixelrel(Cube *cube, double x, double y, double v, int padding);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int *calcbordergauss(float sigma_maj, float sigma_min, float sigma_v, float *cossinpa, float n)
  @brief Returns the measure of a cube containing an ellipsoid

  Returns an allocated array of the measure of a cube just large
  enough to contain an ellipsoid decribed by the parameters. The
  ellipsoid has the axes sigma_min/2 in x-direction, sigma_max/2 in y
  direction, sigma_v/2 in v-direction and is then rotated
  counterclockwise about the v-axis with the angle pa, and finally
  inflated by a factor n. The output has to be deallocated

  @param sigma_maj (float)   Diameter of ellipsoid in y-direction 
  before rotation
  @param sigma_min (float)   Diameter of ellipsoid in x-direction 
  before rotation
  @param sigma_v   (float)   Diameter of ellipsoid in v-direction
  @param cossinpa  (float *) Array with sin and cos of the rotation 
  angle of the ellipsoid (couterclockwise) in the xy-plane
  @param n         (float)   Factor to inflate ellipsoid

  @return int *calcbordergauss Cube diameter in x, y, and v direction
*/
/* ------------------------------------------------------------ */
int *calcbordergauss(float sigma_maj, float sigma_min, float sigma_v, float *cossinpa, float n);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn Cube *padcubex(Cube *cube)
  @brief Change the padding in a cube
  
  Change the padding of a cube. Some routines will need a padded cube
  for a calculation. padcubex will automatically change the padding to
  the right value, either back or forth.

  @param cube (Cube *) The cube

  @return (success) Cube *padcubex: The same cube with different 
  padding\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
Cube *padcubex(Cube *cube);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn Cube *padcubex_norealloc(Cube *cube)
  @brief Change the padding in a cube, assuming that the memory is already allocated
  
  Change the padding of a cube. Some routines will need a padded cube
  for a calculation. padcubex_norealloc will automatically change the
  padding to the right value, either back or forth. Other than
  padcubex, padcubex_norealloc assumes that a large enough memory has
  been allocated for an in-place change of the padding (resorting the
  pixels).

  @param cube (Cube *) The cube

  @return (success) Cube *padcubex_norealloc: The same cube with different 
  padding\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
Cube *padcubex_norealloc(Cube *cube);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn Cube *addcubes(Cube *cube1, Cube *cube2)
  @brief Adds cubes in the overlapping region

  Adds cube2 to cube1 in the overlapping region. cube1 will be
  replaced with the result.

  @param cube1 (Cube *) Cube on which the other cube will be added
  @param cube2 (Cube *) Cube added to cube1 in the overlapping region

  @return (success) Cube  *addcubes: The same pointer to the cube1 passed into the function\n
          (error) NULL

*/
/* ------------------------------------------------------------ */
Cube *addcubes(Cube *cube1, Cube *cube2);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn qfits_header *qfits_header_update(qfits_header *old, qfits_header *new)
  @brief Update an old header with a new one

  Updates a qfits header old object with the header object new
  following the rules:
  @li Every card of new that exists in old will be overwritten. If
  there are multiple cards except COMMENT, the last card with the same
  keyword in old will be written into old instead of the card with the
  same keyword in old.
  @li every COMMENT HISTORY or blank card will be appended to old.
  @li cards that don't exist in new will stay the same

  @param old (qfits_header *) The header object to be updated
  @param new (qfits_header *) The header object with which the old
  header will be updated.

  @return (success) qfits_header *qfits_header_update: Pointer to the
  updated old header \n
          (error) NULL
*/
/* ------------------------------------------------------------ */
qfits_header *qfits_header_update(qfits_header *old, qfits_header *new);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftsout_putcard(qfits_header *old, char *key, char *value)
   @brief Add a card to a header

   Adds a card with the keyword *key (8 characters) to a header with
   the value contained in value. Except for a blank HISTORY or COMMENT
   key, the function will update header items if they are already
   present. For a blank, HISTORY, or COMMENT key, the values are
   appended. In case of a blank, HISTORY, or COMMENT key, the value
   may contain 70 characters, in all other cases, it may contain 20
   characters. For a string it should contain 18 cahracters. For this
   the user has to care. If no header struct is passed (NULL as first
   argument), ftsout_putcard will create a standard primary header.

   @param old (qfits_header *) A qfits header item or NULL
   @param key    (char *)         A keyword
   @param value  (char *)         The value of the keyword

   @return (success) qfits_header *ftsout_putcard: The input or an allocated object \n
           (error) NULL
 */
/* ------------------------------------------------------------ */
qfits_header *ftsout_putcard(qfits_header *old, char *key, char *value);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn void ftsout_header_destroy(qfits_header *header)
   @brief Synonyme of qfits_header_destroy

   Destroys the header item header.

   @param header (qfits_header *) A properly created qfits_header

   @return void
*/
/* ------------------------------------------------------------ */
void ftsout_header_destroy(qfits_header *header);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int *ftsout_writecube(char *filename, Cube *cubename, qfits_header *header)
  @brief Write a cube using the reference frame in header

  Writes the cube cubename into the fitsfile filename, using the
  information about the coordinate system in the qfits_header
  structure as reference coordinate system.

  @param filename (char *) FITS file to be written
  @param cubename (Cube *) Cube structure to be written.
  @param header   (qfits_header *) qfits_header structure with 
  reference frame information

  @return (success) int writecube: 1\n
          (error) 0
*/
/* ------------------------------------------------------------ */
int ftsout_writecube(char *filename, Cube *cubename, qfits_header *header); 



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int *ftsout_writeimage(char *filename, float *array, qfits_header *header, int nx, int ny)
  @brief Writes an image to a file filename

  Writes the fits image filename with BITPIX= -32.

  @param filename (char *)         FITS file to be written
  @param array    (float *)        Array with the points to be written
  @param header   (qfits_header *) qfits_header structure with complete information
  @param nx       (int)            size of image in x-direction
  @param ny       (int)            size of image in y-direction
  @return (success) int writecube: 1\n
          (error) 0
*/
/* ------------------------------------------------------------ */
int ftsout_writeimage(char *filename, float *array, qfits_header *header, int nx, int ny); 


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn float cubarithm_fftgaussian (int nx, int ny, int nv, float *expofacs)
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
float cubarithm_fftgaussian (int nx, int ny, int nv, float *expofacs);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn float cubarithm_fftgaussian2d (int nx, int ny, float *expofacs)
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
float cubarithm_fftgaussian2d(int nx, int ny, float *expofacs);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn Cube *convolgaussfft(Cube *cube, float *expofacsfft)
  @brief Convolve a cube with a gaussian via fft

  In-place convolution of a cube Cube with a gaussian via fft. The
  convolution is not normalised in the xy-plane but in v. No
  convolution takes place in v-direction in case of only one
  plane. See function expofacsfft for definition of expofacsfft
  parameter.

  @param cube        (Cube *)  The cube
  @param expofacsfft (float *) Factors in the exponent of the gaussian

  @return (success) Cube *convolgaussfft: The convolved cube\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
Cube *convolgaussfft(Cube *cube, float *expofacsfft);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static float *expofacsfft(float sigma_maj, float sigma_min, float sigma_v, float *sincosofangle)
  @brief Calculate factors needed by convolgaussfft

  Returns an allocated array containing factors needed by
  convolgaussfft to convolve an array with a gaussian with sigma at
  the major axis sigma_major, minor axis sigma_minor, v axis sigma_v
  axis. These factors are calculated from the measures of the
  convolution kernel and won't change during the whole program.

  @param sigma_maj     (float)   The sigma in direction of the major axis
  @param sigma_min     (float)   The sigma in direction of the minor axis
  @param sigma_v       (float)   The sigma in v-direction
  @param sincosofangle (float *) An array containing the sin and the cos 
  of the position angle

  @return (success) float *expofacsfft: The factors wanted\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
float *expofacsfft(float sigma_maj, float sigma_min, float sigma_v, float *sincosofangle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int cubarithm_readcube(char *filename, Cube **cubename, char *errorstr)
  @brief Read in a cube if possible

  Reads the fitsfile filename into the Cube cubename, allocating
  memory and replacing the pointer (so always pass either NULL or an invalid pointer). This is rather basic as this function expects the three
  accepted coordinates to be in that order: RA,DEC, and VELO. There
  are only three axes accepted to have more than 1 pixel length (i.e. the first axis is RA and has length > 0, the second axis is Dec and has length > 0, the fourth axis is VELO, FELO, or FREQ and has length > 0, the fourth axis can eb whatever, but has length = 1, the fifth axis can be whatever, but has length = 1 and so on. There are further restrictions listed in the error values. The read cube has always three axes, not more, so the information about further axes gets lost. wcs information is generated and stored, such that world pixel values can be read with the functions cubarithm_w2p cubarithm_p2w.  

  @param filename (char *) FITS file name to be read in
  @param cubename (Cube **) Cube structure to be filled in
  @param errstr   (char *)  Output string to be filled in on error. Should be allocated and larger than 120 characters or NULL.

  @return (success) int readcube: 0
	  (error) CUBARITHM_CUBE_ERROR_READ        : Problems reading cube (either not existent or wrong format)														
                  CUBARITHM_CUBE_ERROR_MEM         : Memory problems reading cube.						
                  CUBARITHM_CUBE_ERROR_NONAXIS     : Keyword NAXIS not found							
                  CUBARITHM_CUBE_ERROR_WRONGNAXIS  : NAXIS has the wrong value							
                  CUBARITHM_CUBE_ERROR_NONAXISI    : Keyword NAXISi not found							
                  CUBARITHM_CUBE_ERROR_WRONGNAXISI : NAXISi has the wrong value ( > 1 for i > 3)				
                  CUBARITHM_CUBE_ERROR_NOCRPIXI    : Keyword CRPIXi not found							
                  CUBARITHM_CUBE_ERROR_NOCRVALI    : Keyword CRVALi not found							
                  CUBARITHM_CUBE_ERROR_NOCDELTI    : Keyword CDELTi not found							
                  CUBARITHM_CUBE_ERROR_NOCTYPEI    : Keyword CTYPEi not found							
                  CUBARITHM_CUBE_ERROR_WRONGCTYPEI : CTYPEi has the wrong value							
                  CUBARITHM_CUBE_ERROR_NOEPOCH     : Keyword EPOCH not found							
                  CUBARITHM_CUBE_ERROR_WRONGBUNIT  : Keyword EPOCH not found							
                  CUBARITHM_CUBE_ERROR_WRONGBITPIX : BITPIX has wrong value							
                  CUBARITHM_CUBE_ERROR_WCS         : Problems activating WCS.							
                                                                                                                                
@todo: Protect the function from causing memory leakage. In principle, one should include the allocation of a cube from scratch.
*/
/* ------------------------------------------------------------ */
int cubarithm_readcube(const char *filename, Cube **cubename, char *errorstr);




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int cubarithm_qf_copycard(qfits_header *origin, qfits_header *target, const char *key)
  @brief Copy first occurance of key from origin to target

  Copy first occurance of key from origin to target.

  @param origin (qfits_header *) header to search for key
  @param target (qfits_header *) target to copy key to
  @param key (const char *) key

  @return (success) int cubarithm_qf_copycard: 0
 	  (error) 1 if key cannot be found in origin

@todo: Protect the function from causing memory leakage. In principle, one should include the allocation of a cube from scratch.
*/
/* ------------------------------------------------------------ */
int cubarithm_qf_copycard(qfits_header *origin, qfits_header *target, const char *key);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int cubarithm_qf_addrcard(qfits_header *target, const char *key, const char *value, const char *comment)
  @brief Copy triplet key, value, comment, overwriting existent occurence

  @param target (qfits_header *) target to copy key to
  @param key (const char *) key
  @param value (const char *) value
  @param comment (const char *) comment

  @return (success) int cubarithm_qf_addrcard: 0
 	  (error) 1 if for some reason impossible

*/
/* ------------------------------------------------------------ */
int cubarithm_qf_addrcard(qfits_header *target, const char *key, const char *value, const char *comment);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int cubarithm_p2w(void *thewcsthingy, double *coordsin, double *coordsout)
  @brief Convert the coordsin triplet (pixel values, starting at 0) to world coordinates triplet coordsout

  Input starts at 0, output is in degrees, degrees, m/s or Hz, the triplet is RA, Dec, Spec/V. Again, the lowest pixel is (0,0,0).

  @param thewcsthingy (void *) A wcsprm struct (the argument has to be struct wcsprm *, casted to void *), properly initiated with wcspih, as done for every Cube where wcs is not NULL
  @param coordsin (double *) Input triplet (any length > 3 allowed)
  @param coordsout (double *) Output triplet  (any length > 3 allowed)

  @return (success) int cubarithm_p2w: 0
 	  (error) 1

*/
/* ------------------------------------------------------------ */
int cubarithm_p2w(void *thewcsthingy, double *coordsin, double *coordsout);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int cubarithm_w2p(struct wcsprm *thewcsthingy, double *coordsin, double *coordsout)
  @brief Convert the coordsin triplet (world coordinates) to pixel coordinates triplet (starting at 0)

  Input is in degrees, degrees, m/s or Hz, output starts at 0 (lowest pixel that exists has coordinates 0, 0, 0).

  @param thewcsthingy (struct wcsprm *) A wcsprm struct, properly initiated with wcspih, as done for every Cube where wcs is not NULL
  @param coordsin (double *) Input triplet (any length > 3 allowed)
  @param coordsout (double *) Output triplet  (any length > 3 allowed)

  @return (success) int cubarithm_w2p: 0
 	  (error) 1

*/
/* ------------------------------------------------------------ */
int cubarithm_w2p(void *thewcsthingy, double *coordsin, double *coordsout);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn Cube *cubarithm_copycube(Cube *incubus)
  @brief Make a copy of the input cube incubus

  @param incubus (Cube *) Properly allocated Cube

  @return (success) int cubarithm_copycube: pointer to copied cube
 	  (error) NULL

*/
/* ------------------------------------------------------------ */
Cube *cubarithm_copycube(Cube *incubus);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int cubarithm_writecube(Cube *incubus, char *filename, float *replaceflarray)
  @brief Put the cube onto disk as a fits file

  @param incubus (Cube *) Properly allocated Cube
  @param filename (conat char *) Name of output file
  @param replaceflarray (float *) Array of pixel values to be used instead of the array in the Cube (in that case, the header is identical, but the array passed is used. Pass NULL if you don't want that)


  @return (success) int cubarithm_writecube: 0
 	  (error) 1

*/
/* ------------------------------------------------------------ */
int cubarithm_writecube(Cube *incubus, char *filename, float *replaceflarray);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn qfits_header *qfits_header_update(qfits_header *old,
  qfits_header *new)
  @brief Update an old header with a new one

  Updates a qfits header old object with the header object new
  following the rules:
  @li Every card of new that exists in old will be overwritten. If
  there are multiple cards except COMMENT, the last card with the same
  keyword in old will be written into old instead of the card with the
  same keyword in old.
  @li every COMMENT HISTORY or blank card will be appended to old.
  @li cards that don't exist in new will stay the same

  @param old (qfits_header *) The header object to be updated
  @param new (qfits_header *) The header object with which the old
  header will be updated.

  @return (success) qfits_header *qfits_header_update: Pointer to the
  updated old header \n
          (error) NULL
*/
/* ------------------------------------------------------------ */
qfits_header *qfits_header_update(qfits_header *old, qfits_header *new);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int *writecube(char *filename, Cube *cubename, qfits_header *header)
  @brief Write a cube using the reference frame in header

  Writes the cube cubename into the fitsfile filename, using the
  information about the coordinate system in the qfits_header
  structure as reference coordinate system.

  @param filename (char *) FITS file to be written
  @param cubename (Cube *) Cube structure to be written.
  @param header   (qfits_header *) qfits_header structure with 
  reference frame information

  @return (success) int writecube: 1\n
          (error) 0
*/
/* ------------------------------------------------------------ */
int writecube(char *filename, Cube *cubename, qfits_header *header); 



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: cubarithm.h,v $
   Revision 1.3  2009/05/26 07:56:39  jozsa
   Left work

   Revision 1.2  2007/08/22 15:58:33  gjozsa
   Left work

   Revision 1.1.1.1  2004/10/29 11:13:05  gjozsa
   Added to CVS control


   ------------------------------------------------------------ */

/* Include guard */
#endif

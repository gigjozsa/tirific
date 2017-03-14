/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file fourat.h
   @brief Arithmetics on fourier coefficients

   This module contains arithmetics on fourier coefficients. Main
   purpose is to filter higher order modes in curves.

   $Source: /Volumes/DATA_J_II/data/CVS/tirific/include/fourat.h,v $
   $Date: 2011/05/25 22:24:11 $
   $Revision: 1.1 $
   $Author: jozsa $
   $Log: fourat.h,v $
   Revision 1.1  2011/05/25 22:24:11  jozsa
   added to CVS control


*/
/* ------------------------------------------------------------ */

/* Include guard */
#ifndef FOURAT_H
#define FOURAT_H

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <fftw3.h>
#include <float.h>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define FOURAT_ERR
   @brief Error codes

*/
/* ------------------------------------------------------------ */
#define FOURAT_ERR_NONE     0
#define FOURAT_ERR_MEMORY   1
#define FOURAT_ERR_NARRAY   2
#define FOURAT_ERR_NNUM     4
#define FOURAT_ERR_NDEN     8
#define FOURAT_ERR_NACT    16
#define FOURAT_ERR_OUTACT  32
#define FOURAT_ERR_OUTNUM  64
#define FOURAT_ERR_OUTDEN 128
#define FOURAT_ERR_NULL   256


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define FOURAT_RAT_RATIO
   @brief Idenitfyer for mode in fourat_rat()

*/
/* ------------------------------------------------------------ */
#define FOURAT_RAT_RATIO     0
#define FOURAT_RAT_SUM       1



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* MACROS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* GLOBAL VARIABLES */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* TYPEDEFS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* STRUCTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct fourat_container
   @brief Generic struct for the use in fourat_ratio

   Struct to be filled in for the use of fourat_ratio

*/
/* ------------------------------------------------------------ */
typedef struct fourat_container
{
  /** @brief Dimension of input double array */
  int narray;

  /** @brief (input) double array */
  double *array;

  /** @brief (input) number of active elements */
  int nact;

  /** @brief (input) list of numbers of active elements (starting at 0) */
  int *act;

  /** @brief (input) number of numerator harmonic elements */
  int nnum;

  /** @brief (input) list of numbers of numerator harmonic elements */
  int *num;

  /** @brief (input) number of denomiator harmonic elements */
  int nden;

  /** @brief (input) denominator harmonic elements */
  int *den;

  /** @brief (input) return value for division by zero */
  double huge_dbl;

  /** @brief (output) fftw_complex array of dimension narray/2+1 */
  fftw_complex *harmarray;

  /** @brief (output) array to store interpolated data of length narray */
  double *avarray;

  /** @brief (private) fftw plan for the fft; will be calculated in the function fourat_gen_init */
  fftw_plan plan;

  /** @brief number of the next higher active dependant; will be calculated in the function fourat_gen_init */
  int *dephi;

  /** @brief number of the next lower active dependant; will be calculated in the function fourat_gen_init */
  int *deplo;
}
  fourat_container;


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn fourat_container *fourat_container_const()
    @brief Constructor of a fourat_container struct
    
    All ints are set to 0, all arrays to NULL.

    @param void
    
    @return (success) fourat_container *fourat_container_const allocated fourat_container struct
            (error) NULL
*/
/* ------------------------------------------------------------ */
fourat_container *fourat_container_const();



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn void fourat_container_destr(fourat_container *fourat_containerv)
    @brief Destructor of a fourat_container struct
    
    Deallocates the struct and all arrays contained in the struct.

    @param void
    
    @return (success) fourat_container *fourat_container_const allocated fourat_container struct
            (error) NULL
*/
/* ------------------------------------------------------------ */
void fourat_container_destr(fourat_container *fourat_containerv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int fourat_put_length(fourat_container *fourat_containerv, int narray, int nact, int nnum, int nden, double huge_dbl)
    @brief Entering array lengths
    
    Function to enter the sizes of the input arrays in the
    fourat_container struct. These numbers are only updated, if the
    single array is not already allocated. This function should be
    called before calling fourat_init(). Errors are binary ored.

    @param fourat_containerv (fourat_containerv *)    Allocated fourat_container struct.
    @param narray            (int)                    Length of input array.
    @param nact              (int)                    Length of array with indices of "active" members of array.
    @param nnum              (int)                    Number of harmonic amplitudes summed in the numerator
    @param nden              (int)                    Number of harmonic amplitudes summed in the denominator
    @param huge_dbl          (double)                 Return value in case of denominator = 0; a negative number will be converted to a very large double (DBL_MAX)

    @return (success) int fourat_put_length 0
            (error)   FOURAT_ERR_NARRAY: narray is <= 0
                      FOURAT_ERR_NNUM: nnum is < 0
                      FOURAT_ERR_NDEN: nden is < 0
                      FOURAT_ERR_NACT: nact is < 0
                      FOURAT_ERR_NULL: NULL passed
*/
/* ------------------------------------------------------------ */
int fourat_put_length(fourat_container *fourat_containerv, int narray, int nact, int nnum, int nden, double huge_dbl);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int fourat_put_vectors(fourat_container *fourat_containerv, double *array, int *act, int *num, int *den)
    @brief Entering 
    
    Function to copy (copy, not hook up) arrays into the
    fourat_container struct. The arrays inside the control struct
    should already have been allocated. This can be done by calling
    the function fourat_init() after using the function
    fourat_put_lenght. The function should be called after calling
    fourat_init() as part of the initialising sequence, but not if the
    array member should be changed for several subsequent calls of
    fourat_rat. For this purpose, the function fourat_put_array() is
    used. Note that the arrays passed must at least have the lengths
    specified in the fourat_container struct, best entered by using
    the function fourat_put_length. Any NULL passed results in no
    action for that array, an error is returned if NULL is passed for
    the control struct. An error for a specific array leaves the array
    in the control struct unchanged.

    @param fourat_containerv (fourat_containerv *)    Allocated fourat_container struct.
    @param array             (double *)               input array.
    @param act               (int *)                  array with indices of "active" members of array.
    @param num               (int *)                  Harmonic amplitudes summed in the numerator
    @param den               (int *)                  Harmonic amplitudes summed in the denominator

    @return (success) int fourat_put_vectors 0 (FOURAT_ERR_NONE)
            (error) FOURAT_ERR_OUTACT: act contains a number that does not point to an element of array
                    FOURAT_ERR_OUTNUM: num contains a number that does not correspond to a possible harmonic number (must be <= narray/2, where narray/2 is the integer division).
		    FOURAT_ERR_OUTDEN: den contains a number that does not correspond to a possible harmonic number.
		    FOURAT_ERR_NULL: NULL passed
*/
/* ------------------------------------------------------------ */
int fourat_put_vectors(fourat_container *fourat_containerv, double *array, int *act, int *num, int *den);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int fourat_meminit(fourat_container *fourat_containerv)
    @brief Initialisation of fourat_container struct, alias
    
    Alias of fourat_init for orientation purposes. A correct sequence to use this module is:

    fourat_container_const()          to get the container
    fourat_put_length()               to specify the lengths of the arrays
    fourat_init() or fourat_meminit() to allocate memory inside the control struct
    fourat_put_vectors()              to specify the arrays inside the control struct
    fourat_init()                     to calculate intrinsic structures.
    fourat_rat()                      calculate a ratio, do an fft.
    fourat_put_array()                change the input array
    fourat_rat()                      calculate a ratio, do an fft.
    fourat_put_array()                change the input array
    ...
    fourat_container_destr()          deallocate the container.

    To distinguish between the call of the initialising function before and after the vectors are inserted, this alias has been created.

    @param fourat_containerv (fourat_containerv *)    Allocated fourat_container struct.

    @return (success) int fourat_init 0 (FOURAT_ERR_NONE)
            (error) FOURAT_ERR_MEMORY: memory problems
                    FOURAT_ERR_NARRAY: narray is <= 0
                    FOURAT_ERR_NNUM: nnum is <= 0 and the array is already allocated.
                    FOURAT_ERR_NDEN: nden is <= 0 and the array is already allocated
                    FOURAT_ERR_NACT: nact is < 0
                    FOURAT_ERR_OUTACT: act contains a number that does not point to an element of array
                    FOURAT_ERR_OUTNUM: num contains a number that does not correspond to a possible harmonic number (must be <= narray/2, where narray/2 is the integer division).
		    FOURAT_ERR_OUTDEN: den contains a number that does not correspond to a possible harmonic number.
		    FOURAT_ERR_NULL: NULL passed
*/
/* ------------------------------------------------------------ */
int fourat_meminit(fourat_container *fourat_containerv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int fourat_init(fourat_container *fourat_containerv)
    @brief Initialisation of fourat_container struct
    
    Assumes that allocated arrays have the correct length. Refreshing
    the struct should work by first destroying and the creating a
    fourat_container struct.  This function does the following: if the
    arrays are unallocated (NULL), they get allocated and linked in
    the input struct, according to the array lengths specified by
    narray, nact, nnum, nden (which have been changed by the user or
    using fourat_put_length). Using the act array, it calculates the
    dependencies of the data points and stores them in dep. Then, the
    fftw is initialised and the fftw_plan stored in the member
    plan. After this function has been called, narray, nact, nnum,
    nden should only be changed, if also *array, *act, *num, *den are
    changed accordingly (and properly de-allocated). Better start the
    whole process again. Nevertheless, the function should be called
    if any member of the control struct gets changed, except for the
    case that the content of array is changed. The function returns an
    error if a subsequent call of fourat_rat cannot be successful or
    does not make sense.  Errors are binary ored.

    @param fourat_containerv (fourat_containerv *)    Allocated fourat_container struct.

    @return (success) int fourat_init 0 (FOURAT_ERR_NONE)
            (error) FOURAT_ERR_MEMORY: memory problems
                    FOURAT_ERR_NARRAY: narray is <= 0
                    FOURAT_ERR_NNUM: nnum is <= 0 and the array is already allocated.
                    FOURAT_ERR_NDEN: nden is <= 0 and the array is already allocated
                    FOURAT_ERR_NACT: nact is < 0
                    FOURAT_ERR_OUTACT: act contains a number that does not point to an element of array
                    FOURAT_ERR_OUTNUM: num contains a number that does not correspond to a possible harmonic number (must be <= narray/2, where narray/2 is the integer division).
		    FOURAT_ERR_OUTDEN: den contains a number that does not correspond to a possible harmonic number.
		    FOURAT_ERR_NULL: NULL passed
*/
/* ------------------------------------------------------------ */
int fourat_init(fourat_container *fourat_containerv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn void fourat_put_array(fourat_container *fourat_containerv, double *array)
    @brief Entering the array to be Fourier-transformed
    
    Function to copy the array to the array member of the fourat_container struct if the array member should be changed for several subsequent calls of fourat_rat. The arrays inside the control struct should already have been allocated. Note that the array passed must at least have the length specified in the fourat_container struct (member narray), best entered by using the function fourat_put_length.

    @param fourat_containerv (fourat_containerv *)    Allocated fourat_container struct.
    @param array             (double *)               Length of input array.

    @return void
*/
/* ------------------------------------------------------------ */
void fourat_put_array(fourat_container *fourat_containerv, double *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
@fn int fourat_rat(fourat_container *fourat_containerv, double *out, int mode)
    @brief Calculate ratio of modes
    
    Interpolates array and stores the result in interarray. Makes a discrete Fourier transformation of interarray into outarray. 
If mode = FOURAT_RAT_RATIO:
Calculates sum of the amplitudes of the harmonics stored in num and divides that by the sum of the amplitudes of the harmonics stored in den. If den = 0 or nden = 0, DBL_MAX is returned. 
If mode = FOURAT_RAT_SUM:
Returns sum of the amplitudes of the harmonics stored in num
Errors are binary ored.


    @param fourat_containerv (fourat_containerv *)    Allocated fourat_container struct.
    @param out               (double *)               Output: either ratio or sum of modes stored in num
    @param mode              (int *)                  Output mode

    @return (success) int fourat_rat 0
            (error) FOURAT_ERR_MEMORY: memory problems
                    FOURAT_ERR_NARRAY: narray is <= 0
                    FOURAT_ERR_NNUM: nnum is <= 0 and the array is already allocated.
                    FOURAT_ERR_NDEN: nden is <= 0 and the array is already allocated
                    FOURAT_ERR_NACT: nact is < 0
                    FOURAT_ERR_OUTACT: act contains a number that does not point to an element of array
                    FOURAT_ERR_OUTNUM: num contains a number that does not correspond to a possible harmonic number (must be <= narray/2, where narray/2 is the integer division).
		    FOURAT_ERR_OUTDEN: den contains a number that does not correspond to a possible harmonic number.
		    FOURAT_ERR_NULL: NULL passed
*/
/* ------------------------------------------------------------ */
int fourat_rat(fourat_container *fourat_containerv, double *out, int mode);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: fourat.h,v $
   Revision 1.1  2011/05/25 22:24:11  jozsa
   added to CVS control


   ------------------------------------------------------------ */

/* Include guard */
#endif

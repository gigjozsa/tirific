/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file fourat.c
   @brief Arithmetics on fourier coefficients

   This module contains arithmetics on fourier coefficients. Main
   purpose is to filter higher order modes in curves.

   $Source: /Volumes/DATA_J_II/data/CVS/tirific/src/fourat.c,v $
   $Date: 2011/05/25 22:23:53 $
   $Revision: 1.1 $
   $Author: jozsa $
   $Log: fourat.c,v $
   Revision 1.1  2011/05/25 22:23:53  jozsa
   added to CVS control


*/
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <stdlib.h>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <fourat.h>
#include <maths.h>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE MACROS */
/* ------------------------------------------------------------ */
#define FREE_COND(x) if ((x)) free(x)

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
    @fn static int averarray(fourat_container *fourat_containerv)
    @brief Average array, put the results in interarray
    
    @param fourat_containerv (fourat_containerv *)    Allocated fourat_container struct.

    @return (success) int averarray 0 (FOURAT_ERR_NONE)
            (error) FOURAT_ERR_NARRAY: narray is <= 0
		    FOURAT_ERR_NULL: NULL passed

            
*/
/* ------------------------------------------------------------ */
static int averarray(fourat_container *fourat_containerv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION CODE */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Constructor of a fourat_container struct */
fourat_container *fourat_container_const(void)
{
  fourat_container *fc;

  if(!(fc = (fourat_container *) malloc(sizeof(fourat_container))))
    return NULL;

  fc -> narray = 0;
  fc -> array = NULL;
  fc -> nact = 0;
  fc -> act = NULL;
  fc -> nnum = 0;
  fc -> num = NULL;
  fc -> nden = 0;
  fc -> den = NULL;
  fc -> huge_dbl = DBL_MAX;
  fc -> harmarray = NULL;
  fc -> avarray = NULL;
  fc -> plan = NULL;
  fc -> dephi = NULL; 
  fc -> deplo = NULL; 

  return fc;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destructor of a fourat_container struct */
void fourat_container_destr(fourat_container *fc)
{
  if (!fc)
    return;

  FREE_COND(fc -> array);
  FREE_COND(fc -> act);
  FREE_COND(fc -> num);
  FREE_COND(fc -> den);
  if (fc -> harmarray)
    fftw_free(fc -> harmarray);
  if (fc -> avarray)
    fftw_free(fc -> avarray);
  FREE_COND(fc -> dephi);
  FREE_COND(fc -> deplo);

  if ((fc -> plan))
    fftw_destroy_plan(fc -> plan);
  
  free(fc);

  return;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Entering array lengths */
int fourat_put_length(fourat_container *fc, int narray, int nact, int nnum, int nden, double huge_dbl)
{
  int errcode = FOURAT_ERR_NONE;

  if (!(fc)) {
    return FOURAT_ERR_NULL;
  }

  /* Check */
  if (narray <= 0) {
    errcode |= FOURAT_ERR_NARRAY;
  }
  else {
    if (!fc -> array)
      fc -> narray = narray;
  }

  if (nact < 0) {
    errcode |= FOURAT_ERR_NACT;
  }
  else {
    if (!fc -> act)
      fc -> nact = nact;
  }

  if (nden < 0) {
    errcode |= FOURAT_ERR_NDEN;
  }
  else {
    if (!fc -> den)
      fc -> nden = nden;
  }

  if (nnum < 0) {
    errcode |= FOURAT_ERR_NNUM;
  }
  else {
    if (!fc -> num)
      fc -> nnum = nnum;
  }

  if (huge_dbl < 0.0) {
    fc -> huge_dbl = DBL_MAX;
  }
  else {
    fc -> huge_dbl = huge_dbl;
  }

  return errcode;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Entering vectors */
int fourat_put_vectors(fourat_container *fc, double *array, int *act, int *num, int *den)
{
  int errm = FOURAT_ERR_NONE;
  int i;

  if (!(fc)) {
    return FOURAT_ERR_NULL;
  }

  /* input array cannot cause an error */
  for (i = 0; i < fc -> narray; ++i)
    fc -> array[i] = array[i];

  /* Now check whether the arrays contain proper numbers */
  for (i = 0; i < fc -> nact; ++i) {
    if (act[i] < 0)
      errm |= FOURAT_ERR_OUTACT;
    if (fc -> nact > 0){
      if (act[i] >= fc -> narray)
	errm |= FOURAT_ERR_OUTACT;
    }
  }
  
  for (i = 0; i < fc -> nden; ++i) {
    if (den[i] < 0)
      errm |= FOURAT_ERR_OUTDEN;
    if (fc -> nden > 0){
      if (den[i] > (fc -> narray/2))
	errm |= FOURAT_ERR_OUTDEN;
    }
  }

  for (i = 0; i < fc -> nnum; ++i) {
    if (num[i] < 0)
      errm |= FOURAT_ERR_OUTDEN;
    if (fc -> nnum > 0){
      if (num[i] > (fc -> narray/2))
	errm |= FOURAT_ERR_OUTNUM;
    }
  }

  /* Then copy them into the allocated arrays */

  if (!(errm & FOURAT_ERR_OUTACT)) {
    for (i = 0; i < fc -> nact; ++i)
      fc -> act[i] = act[i];
  }
  if (!(errm & FOURAT_ERR_OUTDEN)) {
    for (i = 0; i < fc -> nden; ++i)
      fc -> den[i] = den[i];
  }
  if (!(errm & FOURAT_ERR_OUTNUM)) {
    for (i = 0; i < fc -> nnum; ++i)
      fc -> num[i] = num[i];
  }
  return errm;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Initialisation of fourat_container struct, alias */
int fourat_meminit(fourat_container *fc)
{
  return fourat_init(fc);
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Initialisation of fourat_container struct */
int fourat_init(fourat_container *fc)
{
  int errm = 0;
  int i, j, loval, hival, curnext;
  
  /* First look if everything is ok */
  
  if (!(fc)) {
    errm |= FOURAT_ERR_NULL;
    goto error;
  }
  
  if (fc -> narray <= 0) {
    errm |= FOURAT_ERR_NARRAY;
  }
  else {
    
    /* Allocate */
    if (!(fc -> array)) {
      if (!(fc -> array = (double *) malloc(fc -> narray*sizeof(double)))) {
	errm |= FOURAT_ERR_MEMORY;
	goto error;
      }
    }
    if (!(fc -> harmarray)) {
      if (!(fc -> harmarray = (fftw_complex *) fftw_malloc((fc -> narray/2+1)*sizeof(fftw_complex)))) {
	errm |= FOURAT_ERR_MEMORY;
	goto error;
      }
    }
    if (!(fc -> avarray)) {
      if (!(fc -> avarray = (double *) fftw_malloc(fc -> narray*sizeof(double)))) {
	errm |= FOURAT_ERR_MEMORY;
	goto error;
      }
    }
    
    /* Make a plan */
    fc -> plan = fftw_plan_dft_r2c_1d(fc -> narray, fc -> avarray, fc -> harmarray, FFTW_ESTIMATE | FFTW_PRESERVE_INPUT);
  }
  
  if  (fc -> nact < 0) {
    errm |= FOURAT_ERR_NACT;
  }
  else {
    if (!(fc -> act)) {
      if (fc -> nact > 0) {
	if (!(fc -> act = (int *) malloc(fc -> nact*sizeof(int)))) {
	  errm |= FOURAT_ERR_MEMORY;
	  goto error;
	}
      }
      /* Set all the numbers to 0 */
      for (i = 0; i < fc -> nact; ++i) {
	fc -> act[i] = 0;
      }
    }
  }
  
  if  (fc -> nnum < 0) {
    errm |= FOURAT_ERR_NNUM;
  }
  else {
    if (!(fc -> num)) {
      if (fc -> nnum > 0) {
	if (!(fc -> num = (int *) malloc(fc -> nnum*sizeof(int)))) {
	  errm |= FOURAT_ERR_MEMORY;
	  goto error;
	}
      }
      
      /* Set all the numbers to 0 */
      for (i = 0; i < fc -> nnum; ++i) {
	fc -> num[i] = 0;
      }
    }
  }
  
  if  (fc -> nden < 0) {
    errm |= FOURAT_ERR_NDEN;
  }
  else {
    if (!(fc -> den)) {
      if (fc -> nden > 0) {
	if (!(fc -> den = (int *) malloc(fc -> nden*sizeof(int)))) {
	  errm |= FOURAT_ERR_MEMORY;
	  goto error;
	}
      }

      /* Set all the numbers to 0 */
      for (i = 0; i < fc -> nden; ++i) {
	fc -> den[i] = 0;
      }
    }
  }
  

  /* Now check whether the arrays contain proper numbers */
  for (i = 0; i < fc -> nact; ++i) {
    if (fc -> act[i] < 0)
      errm |= FOURAT_ERR_OUTACT;
    if (fc -> nact > 0){
      if (fc -> act[i] >= fc -> narray)
	errm |= FOURAT_ERR_OUTACT;
    }
  }
  
  for (i = 0; i < fc -> nden; ++i) {
    if (fc -> den[i] < 0)
      errm |= FOURAT_ERR_OUTDEN;
    if (fc -> nden > 0){
      if (fc -> den[i] > (fc -> narray/2))
	errm |= FOURAT_ERR_OUTDEN;
    }
  }

  for (i = 0; i < fc -> nnum; ++i) {
    if (fc -> num[i] < 0)
      errm |= FOURAT_ERR_OUTDEN;
    if (fc -> nnum > 0){
      if (fc -> num[i] > (fc -> narray/2))
	errm |= FOURAT_ERR_OUTNUM;
    }
  }
  
  /* Now allocate and fill the dependant array */
  /* a[k] = a[deplo[k]]+(k-deplo[k])*(a[dephi[k]]-a[deplo[k]])/(dephi[k]-deplo[k]) */
  
  
  if ((fc -> narray)) {
    if (!(fc -> dephi)) {
      if (!(fc -> dephi = (int *) malloc(fc -> narray*sizeof(int)))) {
	errm |= FOURAT_ERR_MEMORY;
	goto error;
      }
    }
    if (!(fc -> deplo)) {
      if (!(fc -> deplo = (int *) malloc(fc -> narray*sizeof(int)))) {
	errm |= FOURAT_ERR_MEMORY;
	goto error;
      }
    }
  }
  

  /* Fill that array */
  if (fc -> nact > 0) {

    /* find the lowest value */
    loval = fc -> narray;
    for (i = 0; i < fc -> nact; ++i) {
      if (fc -> act[i] < loval)
	loval = fc -> act[i];
    }
    

    /* find the highest value */
    hival = -1;
    for (i = 0; i < fc -> nact; ++i) {
      if (fc -> act[i] > hival)
	hival = fc -> act[i];
    }
    
    /* Everything below loval points to loval */
    for (i = 0; i < loval; ++i) {
      fc -> deplo[i] = fc -> dephi[i] = loval;
    }
    
    /* loval points to loval */
    fc -> deplo[i] = fc -> dephi[i] = loval;
    ++i;
    

    /* Find the next higher active */
    while (i < hival) {
      curnext = fc -> narray;
      for (j = 0; j < fc -> nact; ++j) {
	if ((fc -> act[j] >= i) && (fc -> act[j] < curnext)) {
	  curnext = fc -> act[j];
	}
      }
      
      if (curnext == i)
	loval = i;
      
      fc -> deplo[i] = loval;
      fc -> dephi[i] = curnext;
      ++i;
    }
    
    while (i < fc -> narray) {
      fc -> deplo[i] = fc -> dephi[i] = hival;
      ++i;
    }
  }
  else {
    
    /* This basically means that we filter all data points */
    for (i = 0; i < fc -> narray; ++i) {
      fc -> deplo[i] = fc -> dephi[i] = 0;
    }
  }

  return errm;

 error:
  return errm;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Average array, put the results in avarray */
static int averarray(fourat_container *fc)
{
  int i;

  if (fc -> narray <= 0)
    return FOURAT_ERR_NARRAY;

  if (!(fc -> array))
    return FOURAT_ERR_NULL;

  for (i = 0; i < fc -> narray; ++i) {
    if (fc -> deplo[i] == fc -> dephi[i]) {
      fc -> avarray[i] = fc -> array[fc -> deplo[i]];
    }
    else {
      fc -> avarray[i] = fc -> array[fc -> deplo[i]]+(((double) i)-((double) fc -> deplo[i]))*(fc -> array[fc -> dephi[i]]-fc -> array[fc -> deplo[i]])/(fc -> dephi[i]-fc -> deplo[i]);
    }
  }

  return FOURAT_ERR_NONE;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Average array, put the results in avarray */
int fourat_rat(fourat_container *fc, double *ratio, int mode)
{
  int errcode = 0, i;
  double sumden = 0.0, sumnum = 0.0;

  if (!fc) {
    errcode |=  FOURAT_ERR_NULL;
    goto error;
  }

  if (!ratio) {
    errcode |=  FOURAT_ERR_NULL;
    goto error;
  }

  /* Averaging */
  if ((errcode = averarray(fc)))
    goto error;

  /* Fourier transform */
  fftw_execute(fc -> plan);

  for (i = 0; i < fc -> nnum; ++i) {
    sumnum += sqrt(fc -> harmarray[fc -> num[i]][0]*fc -> harmarray[fc -> num[i]][0]+fc -> harmarray[fc -> num[i]][1]*fc -> harmarray[fc -> num[i]][1]);
  }

  if (mode == FOURAT_RAT_SUM) {
    *ratio = sumnum;
    return errcode;
  }

  /* Calculate the sum of the denominator elements */
  for (i = 0; i < fc -> nden; ++i) {
    sumden += sqrt(fc -> harmarray[fc -> den[i]][0]*fc -> harmarray[fc -> den[i]][0]+fc -> harmarray[fc -> den[i]][1]*fc -> harmarray[fc -> den[i]][1]);
  }

  if (sumnum == 0.0) {
    *ratio = 0;
  }
  else {
    if (sumden == 0.0) {
      *ratio = fc -> huge_dbl;
    }
    else {
      *ratio = sumnum/sumden;
    }
  }

  return errcode;
  
 error:
  return errcode;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Entering the array to be Fourier-transformed */
void fourat_put_array(fourat_container *fc, double *array)
{
  int i;

  /* input array cannot cause an error */
  for (i = 0; i < fc -> narray; ++i)
    fc -> array[i] = array[i];

  return;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: fourat.c,v $
   Revision 1.1  2011/05/25 22:23:53  jozsa
   added to CVS control


   ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file golden.c
   @brief golden section minimisation

   This module implements a minimisation method based on the golden section nested function algorithm

   $Source: /Volumes/DATA_J_II/data/CVS/gft/golden.c,v $
   $Date: 2011/05/04 01:14:29 $
   $Revision: 1.1 $
   $Author: jozsa $
   $Log: golden.c,v $
   Revision 1.1  2011/05/04 01:14:29  jozsa
   added

  
*/
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <stdlib.h>
#include <float.h>
#include <math.h>
/* #include <stdio.h> */
#include "golden.h"


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define AFAC
   @brief A constant needed for the golden section

   omega = (3-sqrt(5))/2 0.3819660112501052
   AFAC = (1-omega)/omega
   BFAC = 1-omega = omega/(1-omega)
*/
/* ------------------------------------------------------------ */
#define AFAC 1.618033988749894
#define BFAC 0.6180339887498948

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def LARGE_INTEGER
   @brief a large integer
*/
/* ------------------------------------------------------------ */
#define LARGE_INTEGER 100000000UL



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def NACC_MAX
   @brief maximum number of accelerations
*/
/* ------------------------------------------------------------ */
#define NACC_MAX 10



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE MACROS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def FREE_COND
   @brief free but check before if pointer is NULL
   
   Note: It has to be checked if this is compliant with memory_here
*/
/* ------------------------------------------------------------ */
#define FREE_COND(x) if ((x)) free(x)



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def FLUSH_COND
   @brief flush but check before if pointer is NULL
   
   Note: It has to be checked if this is compliant with memory_here
*/
/* ------------------------------------------------------------ */
#define FLUSH_COND(x) if ((x)) free(x);x = 0



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
  @fn static int init_iter(golden_container *gc) 
  @brief Initialises the gc for the next iteration

  Initialises the running variables in gc for the next iteration

  @param golden_containerv (* golden_container) The container to be updated

  @return void
*/
/* ------------------------------------------------------------ */
static int init_iter(golden_container *golden_containerv);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION CODE */
/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get golden section container */
golden_container *golden_container_const(void)
{
  golden_container *golden_containerv;
  
  if (!(golden_containerv = (golden_container *) malloc(sizeof(golden_container))))
    return NULL;

  /* input */
  golden_containerv -> npar = 0;
  golden_containerv -> nospar = NULL;
  golden_containerv -> nodpar = NULL;
  golden_containerv -> gchsq = NULL;
  golden_containerv -> adar = NULL;
  golden_containerv -> ncalls_st = (size_t) -1;
  golden_containerv -> minstep = 0.0;

  /* output */
  golden_containerv -> npar_cur = -1;

  golden_containerv -> actchisq = DBL_MAX;
  golden_containerv -> nopar = NULL;

  golden_containerv -> solchisq = DBL_MAX;
  golden_containerv -> solpar = NULL;
  golden_containerv -> solsize = DBL_MAX;

  golden_containerv -> calls = 0;
  golden_containerv -> iters = 0;
  golden_containerv -> loop = 0;

  golden_containerv -> calls_st = 0;
  golden_containerv -> iterstat = 1;

  golden_containerv -> nastep = 0.0;

  /* intrinsic */
  golden_containerv -> dummypar = NULL;
  golden_containerv -> nacc = 0;

  return golden_containerv;
}

/* ------------------------------------------------------------ */




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destructor of the container */
void golden_container_destr(golden_container *golden_containerv)
{
  if (!golden_containerv)
    return;

  FREE_COND(golden_containerv -> solpar  );
  FREE_COND(golden_containerv -> nopar     );
  FREE_COND(golden_containerv -> dummypar     );
  FREE_COND(golden_containerv -> nospar  );
  FREE_COND(golden_containerv -> nodpar  );

  free(golden_containerv);

  return;
}

/* ------------------------------------------------------------ */




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Initialise the fitting process */
int golden_refresh(golden_container *gc, size_t npar)
{
  double *solpar = NULL, *nopar = NULL, *nospar = NULL, *nodpar = NULL, *dummypar = NULL;

  /**************/
  /**************/
/*      fprintf(stderr, "got here: golden_refresh\n"); */
  /**************/


  /* Number of parameters */
  gc -> npar = npar;

  /* (De-) allocations */
  if (!(solpar = (double *) malloc(npar*sizeof(double)))) goto error;
  if (!(nopar  = (double *) malloc(npar*sizeof(double)))) goto error;
  if (!(dummypar  = (double *) malloc(npar*sizeof(double)))) goto error;
  if (!(nospar = (double *) malloc(npar*sizeof(double)))) goto error;
  if (!(nodpar = (double *) malloc(npar*sizeof(double)))) goto error;
  
  FREE_COND(gc -> solpar  );
  FREE_COND(gc -> nopar   );
  FREE_COND(gc -> dummypar);
  FREE_COND(gc -> nospar  );
  FREE_COND(gc -> nodpar  );

  gc -> solpar = solpar;
  gc -> nopar  = nopar ;
  gc -> dummypar  = dummypar ;
  gc -> nospar = nospar;
  gc -> nodpar = nodpar;

  gc -> loop = 0; /* Number of loops */
  gc -> iters = 0; /* Number of iterations */
  gc -> calls = 0; /* Function calls */
  gc -> calls_st = 0; /* calls in iteration */
  gc -> npar_cur = 0; /* parameter number */

  /* This is not exactly correct, but we have a flag */
  gc -> solsize = DBL_MAX;
  gc -> nacc = 0;

  return 0;

 error:
  FREE_COND(gc -> solpar  );
  FREE_COND(gc -> nopar   );
  FREE_COND(gc -> dummypar   );
  FREE_COND(gc -> nospar  );
  FREE_COND(gc -> nodpar  );
  return 1;
}

/* ------------------------------------------------------------ */




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Initialise the fitting process */
int golden_init(golden_container *gc)
{
  int i = 0; /* control variable */

  /**************/
  /**************/
/*       fprintf(stderr, "got here: golden_init\n"); */
  /**************/

  if (!(gc))
    goto error;
  if (!(gc -> npar > 0))
    goto error;
  if (!(gc -> solpar))
    goto error;
  if (!(gc -> nopar))
    goto error;
  if (!(gc -> dummypar))
    goto error;
  if (!(gc -> nospar))
    goto error;
/* This is not really necessary, but we do it here */
  if (!(gc -> nodpar))
    goto error;
  if (!(gc -> gchsq))
    goto error;

  /* Copy start parameters to actual parameters */
  for (i = 0; i < gc -> npar; ++i) {
    gc -> solpar[i] = gc -> nopar[i] = gc -> nospar[i];
  }  


  for (i = 0; i < gc -> npar; ++i) {
    gc -> dummypar[i] = gc -> nopar[i];
  }  
  gc -> solchisq = gc -> actchisq = (gc -> gchsq)(gc -> dummypar, gc -> adar);

  gc -> ncurstep = 0.0;

  gc -> npar_cur = 0;

  init_iter(gc);

  return 0;

 error:
  return 1;
}

/* ------------------------------------------------------------ */




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Initialise a single iteration */
static int init_iter(golden_container *gc)
{
  gc -> iterstat = 1; /* Searching (1)/found (0) minimum */
  gc -> nastep = gc -> nodpar[gc -> npar_cur]; /* step width */
  gc -> calls_st = 0; /* calls per step */
  gc -> nacc = 0;

  return 0;
}

/* ------------------------------------------------------------ */




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/*  */
int golden_iterate(golden_container *gc)
{
  int i; /* Control variable */
  double curstep, dummy; /* dummies */
  double befchisq, befpar;

  curstep = fabs(gc -> nastep);
  gc -> ncurstep = gc -> nastep;

  befchisq = gc -> actchisq;
  befpar = gc -> nopar[gc -> npar_cur];
  gc -> nopar[gc -> npar_cur] = gc -> nopar[gc -> npar_cur]+gc -> nastep;

  for (i = 0; i < gc -> npar; ++i) {
    gc -> dummypar[i] = gc -> nopar[i];
  }

  gc -> actchisq = (gc -> gchsq)(gc -> dummypar, gc -> adar);

  /* Check whether we have found the minimum or not */
  if ((gc -> iterstat)) {
    
    /* We are searching for the minimum, so we only have to react if we find a better chisq */    
    if (gc -> actchisq >= befchisq) {
      gc -> nopar[gc -> npar_cur] = befpar;
      gc -> nastep = -gc -> nastep;
      gc -> actchisq = befchisq;
      
      /* If we find a higher chisquare, we have found the minimum except for at the very start */
      if ((gc -> calls_st)) {
	gc -> iterstat = 0;
	gc -> nacc = 0;
	gc -> nastep = gc -> nastep*BFAC;
      }
    }
    else {
      if (gc -> nacc < NACC_MAX) {
	gc -> nastep = gc -> nastep*AFAC;
	++gc -> nacc;
      }
    }
  }
  else {
    gc -> nastep = gc -> nastep*BFAC;
    if (gc -> actchisq >= befchisq) {
      gc -> nopar[gc -> npar_cur] = befpar;
      gc -> nastep = -gc -> nastep;
      gc -> actchisq = befchisq;
    }
  }

  /* Now do the bookkeeping */

  /* number of calls in an iteration or minimal step width*/
  ++gc -> calls_st;
  ++gc -> calls;

  if ((gc -> calls_st == gc -> ncalls_st) || (curstep < gc -> minstep)) {

    /* We change the npar_cur */
    ++gc -> npar_cur;
    ++gc -> iters;

    /* If we have reached the end of a loop, we change the current solution, and the length of the current solution */
    if (gc -> npar_cur == gc -> npar) {
      gc -> solchisq = gc -> actchisq;
      gc -> solsize = 0;
      for (i = 0; i < gc -> npar; ++i) {
	if (gc -> solsize < (dummy = fabs(gc -> solpar[i] - gc -> nopar[i]))) {
	  gc -> solsize = dummy;
	}
	gc -> solpar[i] = gc -> nopar[i];
      }
      ++gc -> loop;
      gc -> npar_cur = 0;
    }

    init_iter(gc);
  }

  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* i/o functions */
int golden_o_npar(size_t *npar, golden_container *golden_containerv)                {*npar = golden_containerv -> npar; return 0;}
int golden_i_nospar(double *nospar, golden_container *golden_containerv)                      {size_t i; for (i = 0; i < golden_containerv -> npar; ++i) {golden_containerv -> nospar[i] = nospar[i];} return 0;}
int golden_i_nodpar(double *nodpar, golden_container *golden_containerv)                      {size_t i; for (i = 0; i < golden_containerv -> npar; ++i) {golden_containerv -> nodpar[i] = nodpar[i];} return 0;}
int golden_i_gchsq(double (*gchsq)(double *, void *), golden_container *golden_containerv)    {golden_containerv -> gchsq = gchsq; return 0;}
int golden_i_adar(void *adar, golden_container *golden_containerv)                            {golden_containerv -> adar = adar; return 0;}
int golden_i_ncalls_st(size_t ncalls_st, golden_container *golden_containerv)                 {golden_containerv -> ncalls_st = ncalls_st; return 0;}
int golden_i_minstep(double minstep, golden_container *golden_containerv)                     {golden_containerv -> minstep = minstep; return 0;}
int golden_i_nastep(double nastep, golden_container *golden_containerv)                      {golden_containerv -> nastep = nastep; return 0;}

int golden_o_nospar(double *nospar, golden_container *golden_containerv)                      {size_t i; for (i = 0; i < golden_containerv -> npar; ++i) {nospar[i] = golden_containerv -> nospar[i];} return 0;}
int golden_o_nodpar(double *nodpar, golden_container *golden_containerv)                      {size_t i; for (i = 0; i < golden_containerv -> npar; ++i) {nodpar[i] = golden_containerv -> nodpar[i];} return 0;}
int golden_o_ncalls_st(size_t *ncalls_st, golden_container *golden_containerv)                {*ncalls_st = golden_containerv -> ncalls_st; return 0;}
int golden_o_minstep(double *minstep, golden_container *golden_containerv)                    {*minstep = golden_containerv -> minstep; return 0;}
int golden_o_npar_cur(int *npar_cur, golden_container *golden_containerv)                  {*npar_cur = golden_containerv -> npar_cur; return 0;}
int golden_o_actchisq(double *actchisq, golden_container *golden_containerv)                  {*actchisq  = golden_containerv -> actchisq; return 0;}
int golden_o_nopar(double *nopar, golden_container *golden_containerv)                        {size_t i; for (i = 0; i < golden_containerv -> npar; ++i) {nopar[i] = golden_containerv -> nopar[i];} return 0;}
int golden_o_solchisq(double *solchisq, golden_container *golden_containerv)                  {*solchisq  = golden_containerv -> solchisq; return 0;}
int golden_o_solpar(double *solpar, golden_container *golden_containerv)                      {size_t i; for (i = 0; i < golden_containerv -> npar; ++i) {solpar[i] = golden_containerv -> solpar[i];} return 0;}
int golden_o_solsize(double *solsize, golden_container *golden_containerv)                    {*solsize  = golden_containerv -> solsize; return 0;}
int golden_o_calls(size_t *calls, golden_container *golden_containerv)                        {*calls  = golden_containerv -> calls; return 0;}
int golden_o_iters(size_t *iters, golden_container *golden_containerv)                        {*iters  = golden_containerv -> iters; return 0;}
int golden_o_loop(size_t *loop, golden_container *golden_containerv)                          {*loop  = golden_containerv -> loop; return 0;}
int golden_o_calls_st(size_t *calls_st, golden_container *golden_containerv)                  {*calls_st  = golden_containerv -> calls_st; return 0;}
int golden_o_nastep(double *nastep, golden_container *golden_containerv)                      {*nastep  = golden_containerv -> nastep; return 0;}
int golden_o_ncurstep(double *ncurstep, golden_container *golden_containerv)                  {*ncurstep  = golden_containerv -> ncurstep; return 0;}
int golden_o_iterstat(int *iterstat, golden_container *golden_containerv)                     {*iterstat  = golden_containerv -> iterstat; return 0;}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: golden.c,v $
   Revision 1.1  2011/05/04 01:14:29  jozsa
   added

   Revision 1.4  2007/12/12 17:19:21  gjozsa
   checked something

   Revision 1.3  2007/08/14 17:10:51  gjozsa
   Added some counters

   Revision 1.2  2007/07/02 12:55:12  gjozsa
   Made ANSI compliant

   Revision 1.1  2007/06/22 12:43:28  gjozsa
   Added to CVS control


   ------------------------------------------------------------ */

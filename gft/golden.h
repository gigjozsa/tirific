/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file golden.c
   @brief golden section minimisation

   This module implements a minimisation method based on the golden section nested function algorithm

   $Source: /Volumes/DATA_J_II/data/CVS/gft/golden.h,v $
   $Date: 2011/05/04 01:14:32 $
   $Revision: 1.1 $
   $Author: jozsa $
   $Log: golden.h,v $
   Revision 1.1  2011/05/04 01:14:32  jozsa
   added

   
*/
/* ------------------------------------------------------------ */

/* This is the most primitive minimum-finder one can imagine:

in one loop fit the first parameter using a golden-section nested
intervals method. Then fit the second, then the next, and so on, until
the last one is reached. Then determine the characteristic length as
the maximum of the deviations of the fitted parameters with respect to
the parameters at the beginning of the loop. Each iteration process of
a single parameter is called an iteration, each step in an iteration
is called a call. Each iteration is stopped when either the maximum
numbers of calls per iteration have been issued, or when the step
width between calls becomes lower than a minimal step width.

The module provides a container struct golden_container for the io
that has to be filled and to be read out by hand. A further
description of the parameters can be found in the definition of that
struct below. It is not expected that more than one person will use
this minimiser, such that not much energy is put into encapsulating
this module.

The four functions that can be called are a con- and destructor of the container, an initialisation, where
all running parameters are re-set (and a call of the function is made
to estimate the initial function value), and a function that
progresses the fitting process by one step, changing the running
parameters in the container (including the loop number, and the
characteristic length).

After constructing the container (providing the number npar of parameters), the following parameters have to be specified (in golden_container; do copy vectors, do not re-allocate them):

double   (*gchsq)(double *par, void *adar) function to be minimised, with npar fit parameters that are passed to the function in a double array (first argument).
void     *adar                             some additional parameters to the function, encapsulated.
double   *nospar                           start parameters
double   *nodpar                           start stepwidths
double   stopsize                          final stepwidths
size_t   ncalls_st                         maximum number of calls per iteration
double   minstep                           minimal step width

They will stay constant for the rest of the time. Then, the function golden_init will set initial parameters for the fitting process, and will calculate the actual function value, calling the function to be minimised once.

The fitting process is conducted by calling the function golden_iterate() repeatedly. There is a number of output values that can be read out to control the fitting process (of course, the input parameters can be read out as well, but they don't change).

size_t npar_cur            current parameter (0 to npar-1)
double actchisq            current function value, always the best chisquared. Not identical with the chisquare of the last call.
double *nopar              current parameters, always the best fit. Not identical with the parameters of the last call.
double solchisq           function value of the solution. This is actchisq at the end of the last loop.
double *solpar             parameters of the solution (par at the end of the last loop).
double solsize             characteristic size of solpar (see below).
size_t calls               total number of function calls in the fitting process (not counting initialisation).
size_t iters               total number of iterations in the fitting process.
size_t loop                number of finished loops.
size_t calls_st            number of function calls within iteration.
double nastep              step width, next call.
double ncurstep            step width of the last call.
int iterstat               indicator if in current iteration minimum is found (0) or not (1).

*/

/* Include guard */
#ifndef GOLDEN_H
#define GOLDEN_H


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */



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

typedef struct golden_container
{
  /** @brief Number of parameters (input) */
  size_t npar;

  /** @brief start parameters (input) */
  double *nospar;

  /** @brief start step widths (input) */
  double *nodpar;

  /** @brief the external function (input) */
  double (*gchsq)(double *par, void *adar);

  /** @brief the additional arguments to chisquare function (input) */
  void *adar;

  /** @brief maximal number of function calls within an iteration (input) */
  size_t ncalls_st;

  /** @brief minimal step width (input) */
  double minstep;

  /** @brief Current parameter number, starting with 0 (output) */
  int npar_cur;

  /** @brief Chisquare of actual parameters (output) */
  double actchisq;

  /** @brief actual parameters (output) */
  double *nopar;

  /** @brief dummy vector (intrinsic) */
  double *dummypar;

  /** @brief Chisquare of solution (output) */
  double solchisq;

  /** @brief the actual solution (output) */
  double *solpar;

  /** @brief characteristic vector size of current solution (output) */
  double solsize;

  /** @brief Number of calls of chisquare function since start of minimising (output) */
  size_t calls;

/** @brief Number of iteration steps since start of minimising (output) */
  size_t iters;

  /** @brief actual loop number (output) */
  size_t loop;

  /** @brief actual function calls within current iteration (output) */
  size_t calls_st;

  /** @brief step width of the next call (output) */
  double nastep;

  /** @brief step width of the current call (output) */
  double ncurstep;

  /** @brief status in minimisation, 1: searchig minimum, 0: found mimimum (output) */
  int iterstat;

  /** @brief internal, accelleration count */
  size_t nacc;

} golden_container;


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn golden_container *golden_container_const(void)
  @brief Get golden section container

  The function allocates the container. Then, default parameters
  are set. All arrays are NULL, npar is 0.

  @param void

  @return (success)    golden_container *golden_container_const
          (error)      NULL
*/
/* ------------------------------------------------------------ */
golden_container *golden_container_const(void);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void golden_container_destr(golden_container *golden_containerv)
  @brief Destructor of the container

  The function deallocates the container and all arrays in the
  container.

  @param golden_containerv (* golden_container) The container to be destroyed

  @return void
*/
/* ------------------------------------------------------------ */
void golden_container_destr(golden_container *golden_containerv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void golden_refresh(golden_container *golden_containerv, size_t npar)
  @brief Refresh the fitting process

  Allocate memory and reset parameters accordingly.

  @param golden_containerv (* golden_container) The container to be updated
  @param npar (size_t) number of parameters

  @return void
*/
/* ------------------------------------------------------------ */
int golden_refresh(golden_container *golden_containerv, size_t npar);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void golden_init(golden_container *golden_containerv)
  @brief Refresh fitting process

  An initial evaluation of the fitting function is made and the variables in the golden container are changed accordingly.

  @param golden_containerv (* golden_container) The container to be updated

  @return void
*/
/* ------------------------------------------------------------ */
int golden_init(golden_container *golden_containerv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void golden_i_nospar(double *nospar, golden_container *golden_containerv)
  @brief Input start parameters

  The input struct can be accessed directly, but it's safer to use the
  _i_ and _o_ functions for i/o. Input makes a copy of the passed
  arrays/single values to the "intrinsic" struct. The length of the
  arrays must be npar, which is passed in the function golden_refresh.

  @param nospar (double *)                      Array to pass to the container
  @param golden_containerv (* golden_container) The container to be updated

  @return void
*/
/* ------------------------------------------------------------ */
/** @brief input start parameters */
int golden_i_nospar(double *nospar, golden_container *golden_containerv);
/** @brief input start stepwidths */
int golden_i_nodpar(double *nodpar, golden_container *golden_containerv);
/** @brief input function */
int golden_i_gchsq(double (*gchsq)(double *, void *), golden_container *golden_containerv);
/** @brief input additional arguments (void to be casted from the function) */
int golden_i_adar(void *adar, golden_container *golden_containerv);
/** @brief input maximum calls per iteration */
int golden_i_ncalls_st(size_t ncalls_st, golden_container *golden_containerv);
/** @brief input minimum step width iteration */
int golden_i_minstep(double minstep, golden_container *golden_containerv);
/** @brief input next step width */
int golden_i_nastep(double nastep, golden_container *golden_containerv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void golden_o_nospar(double *nospar, golden_container *golden_containerv)
  @brief Input start parameters

  The input struct can be accessed directly, but it's safer to use the
  _i_ and _o_ functions for i/o. Output makes a copy of the "intrinsic"
  arrays/single values to the passed parameter. The length of the
  arrays must be npar, which is passed in the function golden_refresh.

  @param nospar (double *)                      Array to be filled from the container
  @param golden_containerv (* golden_container) The container to be updated

  @return void
*/
/* ------------------------------------------------------------ */
/** @brief output start parameters, array */
int golden_o_npar(size_t *npar, golden_container *golden_containerv);
int golden_o_nospar(double *nospar, golden_container *golden_containerv);
/** @brief output start stepwidths, array */
int golden_o_nodpar(double *nodpar, golden_container *golden_containerv);
/** @brief output maximum calls per iteration, single value */
int golden_o_ncalls_st(size_t *ncalls_st, golden_container *golden_containerv);
/** @brief output minimum step width iteration, single value */
int golden_o_minstep(double *minstep, golden_container *golden_containerv);
/** @brief output Current parameter number, starting with 0, single value */
int golden_o_npar_cur(int *npar_cur, golden_container *golden_containerv);
/** @brief output Chisquare of actual parameters, single value */
int golden_o_actchisq(double *actchisq, golden_container *golden_containerv);
/** @brief output actual parameters */
int golden_o_nopar(double *nopar, golden_container *golden_containerv);
/** @brief output Chisquare of solution, single value */
int golden_o_solchisq(double *solchisq, golden_container *golden_containerv);
/** @brief output the actual solution */
int golden_o_solpar(double *solpar, golden_container *golden_containerv);
/** @brief output characteristic vector size of current solution, single value */
int golden_o_solsize(double *solsize, golden_container *golden_containerv);
/** @brief output Number of calls of chisquare function since start of minimising, single value */
int golden_o_calls(size_t *calls, golden_container *golden_containerv);
/** @brief output Number of iteration steps since start of minimising, single value */
int golden_o_iters(size_t *iters, golden_container *golden_containerv);
/** @brief output actual loop number, single value */
int golden_o_loop(size_t *loop, golden_container *golden_containerv);
/** @brief output actual function calls within current iteration, single value */
int golden_o_calls_st(size_t *calls_st, golden_container *golden_containerv);
/** @brief output step width of the next call, single value */
int golden_o_nastep(double *nastep, golden_container *golden_containerv);
/** @brief output step width of the current call, single value */
int golden_o_ncurstep(double *ncurstep, golden_container *golden_containerv);
/** @brief output status in minimisation, 1: searchig minimum, 0: found mimimum, single value */
int golden_o_iterstat(int *iterstat, golden_container *golden_containerv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void golden_iterate(golden_container *golden_containerv)
  @brief Perform one iteration in the fitting process

  One iteration is done and the variables in the golden container changed accordingly.

  @param golden_containerv (* golden_container) The container to be updated

  @return void
*/
/* ------------------------------------------------------------ */
int golden_iterate(golden_container *golden_containerv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: golden.h,v $
   Revision 1.1  2011/05/04 01:14:32  jozsa
   added

  
   ------------------------------------------------------------ */

/* Include guard */
#endif

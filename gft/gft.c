/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file gft.c
   @brief Generic frontend for chisquare minimisation

   This is planned to comprise all external fitting functions for the
   use wherever

   $Source: /Volumes/DATA_J_II/data/CVS/gft/gft.c,v $
   $Date: 2011/05/04 01:13:59 $
   $Revision: 1.5 $
   $Author: jozsa $  
   $Log: gft.c,v $
   Revision 1.5  2011/05/04 01:13:59  jozsa
   did a lot

   Revision 1.4  2007/12/12 17:19:21  gjozsa
   checked something

   Revision 1.3  2007/08/14 17:10:51  gjozsa
   Added some counters

   Revision 1.2  2007/07/02 12:55:12  gjozsa
   Made ANSI compliant

   Revision 1.1  2007/06/22 12:43:28  gjozsa
   Added to CVS control

  
*/
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
/* #include <stdio.h> */
#include <errno.h>
#include <stdlib.h>
#include <float.h>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <gft.h>
#include <golden.h>
#include <pswarm.h>


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def LARGE_INTEGER
   @brief a large integer
*/
/* ------------------------------------------------------------ */
#define LARGE_INTEGER 100000000UL



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_INPUT_MAX
   @brief Maximum input identifyer number
   
*/
/* ------------------------------------------------------------ */
#define GFT_INPUT_MAX 20



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_OUTPUT_MAX
   @brief Maximum output identifyer number
   
*/
/* ------------------------------------------------------------ */
#define GFT_OUTPUT_MAX 51



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_STOP_CHAR_DO
   @brief Character of stop flag

   GFT_STOP_CHAR_ID:   Stop is not set
   GFT_STOP_CHAR_DO:   stop set but do not leave loop
   GFT_STOP_CHAR_STOP: leave loop
   
*/
/* ------------------------------------------------------------ */
#define GFT_STOP_CHAR_DO   1
#define GFT_STOP_CHAR_STOP 2
#define GFT_STOP_CHAR_ID   0



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_BUSY_CHAR_YES
   @brief Character of busy flag

   GFT_BUSY_CHAR_NO:   Not busy
   GFT_BUSY_CHAR_YES:  busy
   
*/
/* ------------------------------------------------------------ */
#define GFT_BUSY_CHAR_NO   0
#define GFT_BUSY_CHAR_YES  1

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_ACT_MAX
   @brief Maximum output identifyer number
   
*/
/* ------------------------------------------------------------ */
#define GFT_ACT_MAX 5



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def MET_NONE
   @brief no algorithm present alias
   
   Alias for undefined fitting algorithm
*/
/* ------------------------------------------------------------ */
#define MET_NONE 0 



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def MET_GOLDEN
   @brief golden section algorithm
   
   Alias for golden section fitting algorithm
*/
/* ------------------------------------------------------------ */
#define MET_GOLDEN 1


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def MET_SIMPLEX
   @brief simplex algorithm
   
   Alias for simplex fitting algorithm
*/
/* ------------------------------------------------------------ */
#ifdef GFT_GSL_PRESENT
/* gsl is linked like: -lgsl -lgslcblas */
  #include <gsl/gsl_multimin.h>
  #define MET_SIMPLEX 2 
#else
  #define MET_SIMPLEX -2
#endif


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def MET_PSWARM
   @brief pswarm algorithm
   
   Alias for pswarm fitting algorithm
*/
/* ------------------------------------------------------------ */
#define MET_PSWARM 3


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def MET_SIMPLEX_MAXEQ
   @brief A specific stopping condition for simplex
   
   If the chisquare requsted is (number of
   parameters+MET_SIMPLEX_MAXEQ) times the same number in a row, the
   algorithm is stopped.

*/
/* ------------------------------------------------------------ */
#define MET_SIMPLEX_MAXEQ 2 


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
/**
   @def NORM_HUGE
   @brief This will turn a huge value, which is not a real double into a double
   
*/
/* ------------------------------------------------------------ */
#define NORM_HUGE(x) ((x) == HUGE_VAL?DBL_MAX:((x) == -HUGE_VAL?(-DBL_MAX):()))



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* (PRIVATE) GLOBAL VARIABLES */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE TYPEDEFS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @typedef mst_spe
   @brief generic special struct with fitting info
*/
/* ------------------------------------------------------------ */
typedef void mst_spe;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE STRUCTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct mst_gen
   @brief generic part of every mstr.in struct

   The stop and idle coding goes as follows:

   busy 0      0          1         1
   stop 0      1          0         1
        idle   process    process   process
               has been   is        is
               broken     running   stopped

*/
/* ------------------------------------------------------------ */
typedef struct mst_gen
{
  /** brief indicates if a fitting can be started */
  int misinf;

  /** brief indicates a running fit process, forbidding some actions */
  int busy;

  /** brief indicates a stop of the running process */
  int stopped;

  /** brief indicates a kill of the running process */
  int broken;

  /** brief indicates an overflow or underflow error */
  int error;

  /** brief Number of calls of chisquare function since the function is there (reset if function or parameter number is changed) */
  size_t allcalls;

  /** brief Total number of iterations (reset if function or parameter number is changed) */
  size_t alliter;

  /** brief Total number of iterations (reset if function or parameter number is changed) */
  size_t alloops;

  /** brief Number of finished minimisation runs */
  size_t minruns;

  /** @brief Number of parameters */
  size_t npar;

  /** @brief Currently addressed parameter */
  int npar_cur;

  /** @brief Number of independent data points of problem */
  double indpoints;

  /** @brief Chisquare of actual parameters */
  double actchisq;

  /** @brief reduced chisquare of actual parameters */
  double actchisqred;

  /** @brief Minimal chisquare reached */
  double bestchisq;

  /** @brief Reduced minimal chisquare reached */
  double bestchisqred;

  /** @brief actual parameters */
  double *par;

  /** @brief dummy needed by gchsq_n and gchsq_ndummy */
  double *dummypar;

  /** @brief another dummy needed by gchsq_n and gchsq_ndummy */
  double *dummypar2;

  /** @brief best parameters reached */
  double *bestpar;

  /** @brief the actual solution */
  double *solpar;

  /** @brief errors to the solution */
  double *solerr;

  /** @brief Chisquare of solution */
  double solchsq;

  /** @brief reduced chisquare of solution */
  double solchsqred;

  /** @brief start parameters */
  double *spar;

  /** @brief upper bounds */
  double *ubounds;

  /** @brief lower bounds */
  double *lbounds;

  /** @brief Normalisation, origin */
  double *opar;

  /** @brief starting step widths */
  double *dpar;

  /** @brief normalisation, grid */
  double *ndpar;

  /** @brief the external function */
  double (*gchsq)(double *par, void *adar);

  /** @brief the additional arguments to chisquare function */
  void *adar;

  /** @brief maximal number of total function calls */
  size_t ncalls;

  /** @brief Number of calls of chisquare function since start of minimising */
  size_t calls;

/** @brief maximal number of iteration steps */
  size_t niters;

/** @brief Number of iteration steps */
  size_t iters;

  /** @brief maximal number of function calls within a step */
  size_t ncalls_st;

  /** @brief number of function calls within the actual step */
  size_t calls_st;

  /** @brief characteristic vector size, stopping condition, start */
  double stopsize;

  /** @brief characteristic vector size, stopping condition, actual */
  double stopsize_act;

  /** @brief actual characteristic vector size */
  double size;

  /** @brief golden section only: size of the normalised current step width in an iteration process for a single parameter; if not golden section, same as size */
  double dsize;

  /** @brief repetitional loops */
  size_t loops;

  /** @brief actual loop number */
  size_t loop;

  /** @brief factor to multiply ncalls_st from loop to loop */
  double ncalls_st_fac;

  /** @brief factor to multiply starting step
      widths by from loop to loop */
  double dpar_fac;

  /** @brief factor to multiply stopsize from loop to loop */
  double stopsize_fac;

  /** @brief rng seed */
  int seed;

  /** @brief Number of particles for any minimum finder that works with separate group solutions (psw) */
  int psnpart;

  /** @brief pswarm cognitional parameter */
  double pscogni;

  /** @brief pswarm social parameter */
  double pssocia;

  /** @brief pswarm maximum velocity factor */
  double psmaxvf;

  /** @brief pswarm number of iterations to final weight */
  int psnitfi;

  /** @brief pswarm initial weight */
  double psiniin;

  /** @brief pswarm final weight */
  double psfinin;

  /** @brief pswarm increase mesh delta by this factor */
  double psincde;

  /** @brief pswarm decrease mesh delta by this factor */
  double psdecde;
  
  /** @brief the normalised external function */
  double (*gchsq_n)(double *npar, struct mst_gen *mst_genv);

  /** @brief vector containing normalised parameter vector */
  double *nopar;

  /** @brief vector containing normalised start parameter vector */
  double *nospar;

  /** @brief vector containing normalised upper bounds */
  double *noubounds;

  /** @brief vector containing normalised lower bounds */
  double *nolbounds;

  /** @brief vector containing normalised step sizes */
  double *nodpar;

} mst_gen;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct mst
   @brief top layer splitting in generic and minimiser psecific struct

   Contains arrays, variables, functions for the simplex algorithm
   Note: all allocation is done here. pointers in objects will be
   deallocated in this struct.

*/
/* ------------------------------------------------------------ */
typedef struct mst
{
  /** brief fit method as defined by GFT_MET_* */
  int method;

  /** @brief generic content of all minimisers */
  mst_gen *gen;
 
  /** @brief content for running the specific minimiser */
  void *spe;
} mst;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct mst_sim
   @brief internal control struct for the simplex algorithm

   Contains arrays, variables, functions for the simplex algorithm
   Note: all allocation is done here. pointers in objects will be
   deallocated in this struct.

*/
/* ------------------------------------------------------------ */

/* Block this if the GSL is not available */
#if MET_SIMPLEX == -2
  typedef void mst_sim;
#else
typedef struct mst_sim
{
  /** @brief Probably a pointer to a function */
  const gsl_multimin_fminimizer_type \
  *multimin_fminimizer_type_gsl;

  /** @brief struct containing the info needed by the gsl minimiser */
  gsl_multimin_fminimizer *multimin_fminimizer_gsl;
  
  /** @brief gsl vector containing the step size */
  gsl_vector *stp_gsl_vec;
  
  /** @brief block inside the vector (nevermind) */
/*   gsl_block stp_gsl_block; */

  /** @brief gsl vector containing the variables */
  gsl_vector *var_gsl_vec;
  
  /** @brief block inside the vector (nevermind) */
/*   gsl_block var_gsl_block; */

  /** @brief the vector itself, a local copy of nopar in mst_gen */
/*   double *var_vec; */

  /** @brief the vector itself, a local copy of nopar in mst_gen */
/*   double *stp_vec; */

  /** @brief counts the number of equal chisquares */
  size_t eqchisq;

  /** @brief counts the number of equal chisquares */
  size_t eqchisq2;

  /** @brief counts the number of equal chisquares */
  double chisqbef;

  /** @brief counts the number of alternating equal chisquares */
  double chisqbef2;

  /** @brief normalisation of length */
  double vlnorm;

  /** @brief struct containing the function to minimise and stuff */
  gsl_multimin_function *multimin_function_gsl;
} mst_sim;
#endif


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct mst_gol
   @brief internal control struct for the golden section algorithm

   Contains arrays, variables, functions for the golden section
   algorithm Note: all allocation is done here. pointers in objects
   will be deallocated in this struct.

*/
/* ------------------------------------------------------------ */

/* Block this if golden section is not available */
typedef struct mst_gol
{

  golden_container *gc;

} mst_gol;


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct mst_psw
   @brief internal control struct for the golden section algorithm

   Contains arrays, variables, functions for the golden section
   algorithm Note: all allocation is done here. pointers in objects
   will be deallocated in this struct.

*/
/* ------------------------------------------------------------ */

/* Block this if golden section is not available */
typedef struct mst_psw
{

  /** @brief struct containing the function to minimise and stuff */
  pswarm_options *optv;

  /** @brief output and control struct */
  pswarm_swarm   *swav;

  /** @brief current normalised start parameters */
  double *curnospar;

} mst_psw;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static mst *mst_const()
   @brief Consts a mst struct
   
   The function consts a mst-struct and
   initialises the generic part. Pointers are set to NULL.
   
   @param void
   
   @return (success) mst *mst_const(): pointer to struct
   (error)                             NULL
*/
/* ------------------------------------------------------------ */
static mst *mst_const();



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_destr(mst *mstv)
   @brief Destroys mst struct
   
   Will deallocate mstv. Will deallocate everything connected to
   mstv if possible. Will report on untidy deallocation (memory
   leak).

  @param mstv (mst *) Pointer to struct to deallocate

  @return (success) int mst_destr:               GFT_ERROR_NONE
          (error)                                standard
*/
/* ------------------------------------------------------------ */
static int mst_destr(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_flush(mst *mstv)
   @brief Flush of mst struct
   
   Will destroy allocated structs in an mst structs

  @param mstv (mst *) Pointer to struct to deallocate

  @return (success) int mst_flush:               GFT_ERROR_NONE
          (error)                                standard
*/
/* ------------------------------------------------------------ */
static int mst_flush(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int mst_act(mst *mstv, int spec)
  @brief Action!

  Internal alias, see gft_mst_act

  @param gft_mstv (gft_mst *)  Pointer to main struct
  @param spec     (int)        specifyer of the type of input

  @return (success) int mst_act:    GFT_ERROR_NONE
          (error)                   standard
*/
/* ------------------------------------------------------------ */
static int mst_act(mst *mstv, int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_stop(mst *mst_genv)
   @brief Stop the fitting process, keeping it active

   Some fitting methods will have the possibility to stop the fitting,
   and to continue with a further call of mst_start. Stopping is done
   with this function, starting with mst_start. Basically, this
   function sets the stop parameter in the gen struct to 1, which
   should cause the fitting to stop.
   
   @param mst_stop (mst_gen *) Pointer to global fit struct

  @return (success) int mst_stop: GFT_ERROR_NONE    successful in this part
          (error)                        standard
*/
/* ------------------------------------------------------------ */
static int mst_stop(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_start(mst *mstv)
   @brief (Re-)start the fitting process

   Start or restart (in case it has been stopped) the fitting.
   
   @param mstv (mst *) Pointer to global fit struct

  @return (success) int mst_start: GFT_ERROR_NONE
          (error)                        standard
*/
/* ------------------------------------------------------------ */
static int mst_start(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_break(mst *mstv)
   @brief Abort the fitting process

   After calling this function, the next call of mst_start will start
   a new fitting process.
   
   @param mstv (mst *) Pointer to global fit struct

  @return (success) int mst_start: GFT_ERROR_NONE
          (error)                        standard
*/
/* ------------------------------------------------------------ */
static int mst_break(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int mst_get(mst *mstv, void *output, int spec)
  @brief Generically get information from a minimiser struct, intern

  Internal alias, see gft_mst_put

  @param gft_mstv (gft_mst *)  Pointer to main struct
  @param output   (void *)     array
  @param spec     (int)        specifyer of the type of input

  @return (success) int mst_put:    GFT_ERROR_NONE
          (error)                   standard
*/
/* ------------------------------------------------------------ */
static int mst_get(mst *mstv, void *output, int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int mst_put(mst *mstv, void *input, int spec)
  @brief Generically supply information to a minimiser struct, intern

  constant expression   input type   description
  GFT_INPUT_FIT_MET     int *        Specify fit method as defined by GFT_MET_*

  @param gft_mstv (gft_mst *)  Pointer to main struct
  @param input    (void *)     array containing info
  @param spec     (int)        specifyer of the type of input

  @return (success) int mst_put:    GFT_ERROR_NONE
          (error)                   standard
*/
/* ------------------------------------------------------------ */
static int mst_put(mst *mstv, void *input, int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int mst_putf(mst *mstv, double (*input)(double *, void *), int spec)
  @brief Generically supply functions to a minimiser struct, intern

  @param gft_mstv (gft_mst *)  Pointer to main struct
  @param input    (double (*input)(double *, void *)) pointer to function
  @param spec     (int)        specifyer of the type of input

  @return (success) int mst_putf:    GFT_ERROR_NONE
          (error)                   standard
*/
/* ------------------------------------------------------------ */
static int mst_putf(mst *mstv, double (*input)(double *, void *), int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_ckme(int method)
   @brief Check if a minimisation method is valid and existent
   
   @param method (int) method suggested

   @return (success) int mst_ckme:                GFT_ERROR_NONE
           (error)                                standard
*/
/* ------------------------------------------------------------ */
static int mst_ckme(int method);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_initspe(mst *mstv)
   @brief Do initialisations of specific minimiser that needs 
          a call of the minimising function
   
   This function is called from mst_start, and a check whether the
   minimiser is existent has been made before. If no minimiser is
   present, the misinf status is set != 0. No check of the proper
   allocation status of the minimiser struct is made.

   @param mstv (mst *) minimiser struct

   @return (success) int mst_initspe:             GFT_ERROR_NONE
           (error)                                standard
*/
/* ------------------------------------------------------------ */
static int mst_initspe(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_refresh(mst *mstv)
   @brief Refresh of links and allocations in an mst struct
   
   Refreshes all links and allocations in mstv, sets the misinf status
   accordingly. Returns GFT_ERROR_NONE if misinf is GFT_ERROR_NONE, an error otherways

  @param mstv (mst *) Pointer to struct

  @return (success) int mst_refresh:  GFT_ERROR_NONE  a fit can start
          (error)                     standard
*/
/* ------------------------------------------------------------ */
static int mst_refresh(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_refreshspe(mst *mstv)
   @brief Refresh specific part of fit control struct
   
   The function will also change the generic part of the control struct

   @param mst (mst *) Pointer to fit control struct

   @return (success) int mst_gen_refresh: GFT_ERROR_NONE        successful
           (error)                        standard
*/
/* ------------------------------------------------------------ */
static int mst_refreshspe(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_refreshsim(mst *mstv)
   @brief Refresh part of control struct connected to simplex
   
   The function will also change the generic part of the control struct

   @param mstv (mst *) Pointer to fit control struct

   @return (success) int mst_gen_refresh: GFT_ERROR_NONE        successful
           (error)                        standard
*/
/* ------------------------------------------------------------ */
static int mst_refreshsim(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_refreshgol(mst *mstv)
   @brief Refresh part of control struct connected to golden section
   
   The function will also change the generic part of the control struct

   @param mstv (mst *) Pointer to fit control struct

   @return (success) int mst_refreshgol: GFT_ERROR_NONE        successful
           (error)                        standard
*/
/* ------------------------------------------------------------ */
static int mst_refreshgol(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_refreshpsw(mst *mstv)
   @brief Refresh part of control struct connected to golden section
   
   The function will also change the generic part of the control struct

   @param mstv (mst *) Pointer to fit control struct

   @return (success) int mst_refreshpswarm: GFT_ERROR_NONE        successful
           (error)                          standard
*/
/* ------------------------------------------------------------ */
static int mst_refreshpsw(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_ckop(mst *mstv, int spec)
   @brief Check if output request makes sense
   
   Helper to mst_get.  The function will return an appropriate error,
   if at the current stage the request for a quantity makes or better,
   made no sense.

   @param mstv (mst *) Pointer to fit control struct
   @param spec (int)   Specification of the quantity requested

   @return (success) int mst_ckop: GFT_ERROR_NONE        successful
           (error)                 standard
*/
/* ------------------------------------------------------------ */
static int mst_ckop(mst *mstv, int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_iterspe(mst *mstv)
   @brief Make an iteration with the specific minimiser
   
   This is a switch to invoke the specific minimiser functions that do
   one iteration. This function contians no check of proper
   initialisation of all necessary structs.

   @param mstv (mst *) Pointer to fit control struct

   @return (success) int mst_iterspe: GFT_ERROR_NONE        successful
           (error)                    standard
*/
/* ------------------------------------------------------------ */
static int mst_iterspe(mst *mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static mst_gen *mst_gen_const()
   @brief Consts a mst_gen struct
   
   The function consts the generic part of the mst-struct and
   initialises everything. Pointers are set to NULL. Concerning the rest:

   misinf = GFT_ERROR_STANDARD;
   busy = GFT_ERROR_NONE;
   stopped = GFT_ERROR_NONE;
   error = GFT_ERROR_NONE;
allcalls = 0;
alliter = 0;
alloops = 0;
minruns = 0;
   method = MET_NONE;
   npar = 0;
   npar_cur = -1;
indpoints = 0.0
onecall = 0
actchisq = DBL_MAX
bestchisq = DBL_MAX
bestchisqred = DBL_MAX
solchsq = DBL_MAX
solchsqred = DBL_MAX
   ncalls = LARGE_INTEGER;
calls = 0
   niters = LARGE_INTEGER;
iters = 0
   ncalls_st = LARGE_INTEGER;
calls_st = 0
   stopsize = 0.0;
stopsize_act = 0.0;
   size = 1.0;
   dsize = 1.0;
   loops = 1;
loop = 0
   ncalls_st_fac = 1.0;
   dpar_fac = 1.0;
   stopsize_fac 1.0;
  
   @param void
   
   @return (success) mst_gen *mst_gen_const(): pointer to struct
   (error)                                     NULL
*/
/* ------------------------------------------------------------ */
static mst_gen *mst_gen_const();



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_gen_destr(mst_gen *mst_genv)
   @brief Destroys mst_genv
   
   Will deallocate mst_genv. Will deallocate everything connected to mst_genv.
   Will report on untidy deallocation (memory leak).

  @param mst_genv   (mst_gen *) Pointer to struct to deallocate

  @return (success) int mst_gen_destr:         GFT_ERROR_NONE
          (error)                              standard
*/
/* ------------------------------------------------------------ */
static int mst_gen_destr(mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_gen_flush(mst_gen *mst_genv)
   @brief Flushes mst_gen struct
   
   Will deallocate all allocations in mst_genv and set pointers to
   NULL.

  @param mst_genv   (mst_gen *) Pointer to struct to deallocate

  @return (success) int mst_gen_flush:         GFT_ERROR_NONE
          (error)                              standard
*/
/* ------------------------------------------------------------ */
static int mst_gen_flush(mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_gen_ckbu(mst_gen *mst_genv)
   @brief check busy status of mst_gen struct
   
   Checks if the member busy is set to GFT_ERROR_NONE, indicative for no
   minimisation being active.

   @param mst_genv (mst_gen *) Pointer to generic fit struct

  @return (success) int mst_gen_ckbu: GFT_ERROR_NONE  idle 
          (error)                     standard        active
*/
/* ------------------------------------------------------------ */
static int mst_gen_ckbu(mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_gen_ckmi(mst_gen *mst_genv)
   @brief check misinf status of mst_gen struct
   
   Checks if the member misinf is set to GFT_ERROR_NONE, meaning that a minimisation
   can be started.

   @param mst_genv (mst_gen *) Pointer to generic fit struct

  @return (success) int mst_gen_ckmi: GFT_ERROR_NONE if fit can start
          (error)                     standard
*/
/* ------------------------------------------------------------ */
/* static int mst_gen_ckmi(mst_gen *mst_genv); */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_gen_ckch(mst_gen *mst_genv, double *array, double chisq)
   @brief Check out (best) chisquares and parameter arrays
   
   Helper to gchsq_n or gchsq_ndummy. Takes the array, puts it into
   mst_genv -> nopar, copies mstv_genv -> dummypar in mstv_genv ->
   par. Then checks whether the chisq(uare) is better than the current
   minimal chisquare. If yes, the current best chisquare and the best
   fit parameters are changed accordingly.

   @param mst_genv (mst_gen *) Pointer to generic fit struct
   @param array (double *)     Array of the correct length
   @param chisq (double)       actual chisquare

  @return (success) int mst_gen_ckch: GFT_ERROR_NONE if fit can start
          (error)                     standard
*/
/* ------------------------------------------------------------ */
static int mst_gen_ckch(mst_gen *mst_genv, double *array, double chisq);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_gen_ckop(mst_gen *mst_genv, int spec)
   @brief Check if output request makes sense
   
   Helper to mst_ckop.  The function will return an appropriate error,
   if at the current stage the request for a quantity makes or better,
   made no sense.

   @param mstv_genv (mst_gen *) Pointer to fit control struct
   @param spec      (int)   Specification of the quantity requested

   @return (success) int mst_ckop: GFT_ERROR_NONE        successful
           (error)                 standard
*/
/* ------------------------------------------------------------ */
static int mst_gen_ckop(mst_gen *mst_genv, int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_gen_refresh(mst_gen *mst_genv)
   @brief Refresh generic part of fit control struct
   
   @param mst_genv (mst_gen *) Pointer to generic fit struct

  @return (success) int mst_gen_refresh: GFT_ERROR_NONE    successful in this part
          (error)                        standard
*/
/* ------------------------------------------------------------ */
static int mst_gen_refresh(mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_gen_init(mst_gen *mst_genv)
   @brief Calculate the chisquare ONCE

   If possible (if both the function to minimise and the actual
   parameters are present), calculate the chisquare and change the
   informations about the chisquares and best-fit parameters.
   
   @param mst_genv (mst_gen *) Pointer to generic fit struct

  @return (success) int mst_gen_init: GFT_ERROR_NONE    successful in this part
          (error)                        standard
*/
/* ------------------------------------------------------------ */
static int mst_gen_init(mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static mst_spe *mst_spe_const(int method);
   @brief Generic constructor of the mst_spe struct
   
   Calls the specific constructors of the specific fit structs
   
   @param method (int)     fit method as defined by GFT_MET_*
   
   @return (success) mst_spe *mst_spe_const pointer to struct
   (error)           NULL                   memory allocation problems
*/
/* ------------------------------------------------------------ */
static mst_spe *mst_spe_const(int method);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_spe_destr(mst_spe *spev, int method)
   @brief Generic destructor of the specific minimiser struct
   
   Will deallocate spev. Will deallocate everything connected to spe.
   Will report on untidy deallocation (memory leak).

  @param spe    (mst_sim *)  pointer to the struct to deallocate
  @param method (int)        fit method

  @return (success) int mst_spe_destr: GFT_ERROR_NONE
          (error)                      standard
*/
/* ------------------------------------------------------------ */
static int mst_spe_destr(mst_spe *spev, int method);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_spe_ckop(mst_spe *mst_spev, int spec, int method)
   @brief Check if output request makes sense
   
   Helper to mst_ckop.  The function will return an appropriate error,
   if the request for a quantity makes or better, made no sense for a
   specific fitting method. A non-valid fit method is allowed as
   input.

   @param mstv   (mst *) Pointer to fit control struct
   @param spec   (int)   Specification of the quantity requested
   @param method (int)   Specification of fit method

   @return (success) int mst_ckop: GFT_ERROR_NONE        successful
           (error)                 standard
*/
/* ------------------------------------------------------------ */
static int mst_spe_ckop(mst_spe *mst_spev, int spec, int method);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int ckmetinp(int method, int spec)
   @brief Check if a value passed makes sense
   
   The function checks if an input identifyer makes sense in the
   context of the specified fitting algorithm. This case must be checked
   independently.

   @param method   (int)       fit method
   @param spec     (int)       quantity specifyer to check for

   @return (success) int mst_ckmeaning:           GFT_ERROR_NONE
           (error)                                standard
*/
/* ------------------------------------------------------------ */
static int ckmetinp(int method, int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int ckmetinp_undef(int spec)
   @brief Helper to ckmetinp
   
   The function checks if for an undefined method the input specifyer
   makes sense.

   @param spec     (int)       quantity specifyer to check for

   @return (success) int mst_ckmetinp_undef:      GFT_ERROR_NONE
           (error)                                standard
*/
/* ------------------------------------------------------------ */
static int ckmetinp_undef(int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static mst_sim *mst_sim_const();
   @brief Consts 
   
   The function constructs the simplex part of the in-struct and
   initialises everything. Pointers are set to NULL.
   
   @param void
   
   @return (success) mst_sim *mst_sim_const pointer to struct
           (error)   NULL                   memory allocation problems
*/
/* ------------------------------------------------------------ */
static mst_sim *mst_sim_const();



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static mst_sim *mst_psw_const();
   @brief Consts 
   
   The function constructs the simplex part of the in-struct and
   initialises everything. Pointers are set to NULL.
   
   @param void
   
   @return (success) mst_psw *mst_psw_const pointer to struct
           (error)   NULL                   memory allocation problems
*/
/* ------------------------------------------------------------ */
static mst_psw *mst_psw_const();



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_sim_destr(mst_sim *spe)
   @brief Destrs a specific minimiser struct: simplex
   
   Will deallocate spe. Will deallocate everything connected to spe.
   Will report on untidy deallocation (memory leak).

  @param spe   (mst_sim *)  pointer to the struct to deallocate

  @return (success) int mst_sim_destr: GFT_ERROR_NONE
          (error)                      standard
*/
/* ------------------------------------------------------------ */
static int mst_sim_destr(mst_sim *spe);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_psw_destr(mst_psw *spe)
   @brief Destrs a specific minimiser struct: simplex
   
   Will deallocate spe. Will deallocate everything connected to spe.
   Will report on untidy deallocation (memory leak).

  @param spe   (mst_psw *)  pointer to the struct to deallocate

  @return (success) int mst_psw_destr: GFT_ERROR_NONE
          (error)                      standard
*/
/* ------------------------------------------------------------ */
static int mst_psw_destr(mst_psw *spe);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_sim_init(mst_sim *mst_simv, mst_gen *mst_genv)
   @brief Do initialisations of the minimiser: simplex
   
  Does initialisations of the minimiser that need calls of the
  minimising function.

  @param mst_simv (mst_sim *)  pointer to the simplex specific struct
  @param mst_genv (mst_gen *)  pointer to the generic struct

  @return (success) int mst_sim_init: GFT_ERROR_NONE
          (error)                     standard
*/
/* ------------------------------------------------------------ */
static int mst_sim_init(mst_sim *mst_simv, mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_psw_init(mst_psw *mst_pswv, mst_gen *mst_genv)
   @brief Do initialisations of the minimiser: pswarm
   
  Does initialisations of the minimiser that need calls of the
  minimising function.

  @param mst_pswv (mst_psw *)  pointer to the pswarm specific struct
  @param mst_genv (mst_gen *)  pointer to the generic struct

  @return (success) int mst_psw_init: GFT_ERROR_NONE
          (error)                     standard
*/
/* ------------------------------------------------------------ */
static int mst_psw_init(mst_psw *mst_pswv, mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_sim_iter(mst_sim *mst_simv, mst_gen *mst_genv)
   @brief Do one iteration step: simplex
   
   Does one iteration step and refreshes, if possible, best-fit
   parameters and solution.

  @param mst_simv (mst_sim *)  pointer to the simplex specific struct
  @param mst_genv (mst_gen *)  pointer to the generic struct

  @return (success) int mst_sim_iter: GFT_ERROR_NONE
          (error)                     standard
*/
/* ------------------------------------------------------------ */
static int mst_sim_iter(mst_sim *mst_simv, mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_psw_iter(mst_psw *mst_pswv, mst_gen *mst_genv)
   @brief Do one iteration step: pswarm
   
   Does one iteration step and refreshes, if possible, best-fit
   parameters and solution.

  @param mst_simv (mst_psw *)  pointer to the simplex specific struct
  @param mst_genv (mst_gen *)  pointer to the generic struct

  @return (success) int mst_psw_iter: GFT_ERROR_NONE
          (error)                     standard
*/
/* ------------------------------------------------------------ */
static int mst_psw_iter(mst_psw *mst_pswv, mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int cksiminp(int spec)
   @brief Check if a value passed makes sense
   
   The function checks if an input identifyer makes sense in the
   context of the specified fitting algorithm. The identifyer must be
   valid, otherways GFT_ERROR_NONE is returned.

   @param spec     (int)       quantity specifyer to check for

   @return (success) int mst_ckmeaning:           GFT_ERROR_NONE
           (error)                                standard
*/
/* ------------------------------------------------------------ */
static int cksiminp(int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int ckpswinp(int spec)
   @brief Check if a value passed makes sense
   
   The function checks if an input identifyer makes sense in the
   context of the specified fitting algorithm. The identifyer must be
   valid, otherways GFT_ERROR_NONE is returned.

   @param spec     (int)       quantity specifyer to check for

   @return (success) int mst_ckmeaning:           GFT_ERROR_NONE
           (error)                                standard
*/
/* ------------------------------------------------------------ */
static int ckpswinp(int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int cksimop(int spec)
   @brief Check if a value requested makes sense
   
   The function checks if an input identifyer makes sense in the
   context of the specified fitting algorithm. The identifyer must be
   valid, otherways GFT_ERROR_NONE is returned.

   @param spec     (int)       quantity specifyer to check for

   @return (success) int mst_ckmeaning:           GFT_ERROR_NONE
           (error)                                standard
*/
/* ------------------------------------------------------------ */
/* static int cksimop(int spec); */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static gsl_vector *empty_gsl_dbl_vector(void);
  @brief Allocate and initialise an empty gsl vector with double elements

  The vector is allocated and initialised as such:
  size = 0    
  stride = sizeof(double)
  data = NULL   
  block = NULL		    
  owner = 0              

  @param void  
  @return (success) gsl_vector *empty_gsl_dbl_vector   the vector
          (error)   NULL
*/
/* ------------------------------------------------------------ */
#ifdef GFT_GSL_PRESENT
/* static gsl_vector *empty_gsl_dbl_vector(void); */
#endif


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int fill_gsl_dbl_vector(gsl_vector **gsl_vectorv, double *array, size_t length)
  @brief Fill (and allocate) gsl vector

  If gsl_vectorv = NULL, nothing can be done. If *gsl_vectorv = NULL,
  a gsl double vector of length is allocated (by gsl), otherways it is
  assumed that the gsl vector passed is of size length. Then, the
  vector is filled with the length first elements of array, which
  should of course be large enough. If array is empty,
  GFT_ERROR_NULL_PASSED is returned. If length is 0,
  GFT_ERROR_WRONG_PARAM is returned.

  @param empty_gsl_vector (gsl_vector **)  pointer to gsl vector
  @param array            (double *)       array of length length
  @param length           (size_t)         length of array (vector)

  @return (success) int fill_gsl_dbl_vector: GFT_ERROR_NONE
          (error)                            standard
*/
/* ------------------------------------------------------------ */
#ifdef GFT_GSL_PRESENT
static int fill_gsl_dbl_vector(gsl_vector **gsl_vectorv, double *array, size_t length);
#endif


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static mst_gol *mst_gol_const();
   @brief Consts 
   
   The function constructs the golplex part of the in-struct and
   initialises everything. Pointers are set to NULL.
   
   @param void
   
   @return (success) mst_gol *mst_gol_const pointer to struct
           (error)   NULL                   memory allocation problems
*/
/* ------------------------------------------------------------ */
static mst_gol *mst_gol_const();



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_gol_destr(mst_gol *spe)
   @brief Destrs a specific minimiser struct: golplex
   
   Will deallocate spe. Will deallocate everything connected to spe.
   Will report on untidy deallocation (memory leak).

  @param spe   (mst_gol *)  pointer to the struct to deallocate

  @return (success) int mst_gol_destr: GFT_ERROR_NONE
          (error)                      standard
*/
/* ------------------------------------------------------------ */
static int mst_gol_destr(mst_gol *spe);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_gol_init(mst_gol *mst_golv, mst_gen *mst_genv)
   @brief Do initialisations of the minimiser: golplex
   
  Does initialisations of the minimiser that need calls of the
  minimising function.

  @param mst_golv (mst_gol *)  pointer to the golplex specific struct
  @param mst_genv (mst_gen *)  pointer to the generic struct

  @return (success) int mst_gol_init: GFT_ERROR_NONE
          (error)                     standard
*/
/* ------------------------------------------------------------ */
static int mst_gol_init(mst_gol *mst_golv, mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int mst_gol_iter(mst_gol *mst_golv, mst_gen *mst_genv)
   @brief Do one iteration step: golden section
   
   Does one iteration step and refreshes, if possible, best-fit
   parameters and solution.

  @param mst_simv (mst_gol *)  pointer to the golden section specific struct
  @param mst_genv (mst_gen *)  pointer to the generic struct

  @return (success) int mst_gol_iter: GFT_ERROR_NONE
          (error)                     standard
*/
/* ------------------------------------------------------------ */
static int mst_gol_iter(mst_gol *mst_golv, mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int ckgolinp(int spec)
   @brief Check if a value passed makes sense
   
   The function checks if an input identifyer makes sense in the
   context of the specified fitting algorithm. The identifyer must be
   valid, otherways GFT_ERROR_NONE is returned. For the golden section
   algorithm, this function is a dummy.

   @param spec     (int)       quantity specifyer to check for

   @return (success) int mst_ckmeaning:           GFT_ERROR_NONE
           (error)                                standard
*/
/* ------------------------------------------------------------ */
static int ckgolinp(int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static double gchsq_gol(gsl_vector *par, void *npa)
  @brief Function for gsl_minimiser

  This is the function passed to the golden section minimiser. npa is interpreted as a struct that is
  passed as it is to gchsq_n, passing as parameter array the data
  array in the gsl vector.

  @param par         (double *) An array
  @param void *npa   A mst_gen_nad struct

  @return double gchsq_gol: gchsq_n(par -> data, npa)

*/
/* ------------------------------------------------------------ */
static double gchsq_gol(double *nopar, void *npa);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static double gchsq_psw(gsl_vector *par, void *npa)
  @brief Function for gsl_minimiser

  This is the function passed to the pswarm minimiser. npa is interpreted as a struct that is
  passed as it is to gchsq_n.

  @param par         (double *) An array
  @param void *npa   A mst_gen_nad struct

  @return double gchsq_psw

*/
/* ------------------------------------------------------------ */
static double gchsq_psw(double *nopar, void *npa);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static double gchsq_n(double *nopar, mst_gen *mst_genv)
  @brief Calls the external chisquare function contained in npa
  renormalising the arguments before

  Will set mst_gen_nadv -> par to the renormalised input nopar.
  par[i] = nopar[i]*ndpar[i]+opar[i]

  @param nopar    (double *)  parameters to minimise, normalised
  @param mst_genv (mst_gen *) struct containing additional external 
                              arguments and original fitting function 
                              and normalisation

  @return double gchsq_n: function value of external function after 
                          renormalising the nopar arry
*/
/* ------------------------------------------------------------ */
static double gchsq_n(double *nopar, mst_gen *mst_genv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static double gchsq_ndummy(double *nopar, mst_gen *mst_genv)
  @brief Calls the external chisquare function contained in npa without
  renormalising the arguments before

  If normalisation is not desired this takes the place of the
  normalised function.  Will set mst_genv -> par to the input nopar. For later use, commented out in this version.

  @param nopar    (double *)    parameters to minimise, not normalised
  @param mst_genv (mst_gen_nad) struct containing additional external 
                                arguments and original fitting function 
                                and normalisation

  @return double gchsq_ndummy: function value of external function after 
                               not renormalising the nopar arry
*/
/* ------------------------------------------------------------ */
/* static double gchsq_ndummy(double *nopar, mst_gen *mst_genv); */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static double gchsq_sim(gsl_vector *par, void *npa)
  @brief Function for gsl_minimiser

  This is the function passed to any gsl minimiser, which at the
  current state is only one. npa is interpreted as a struct that is
  passed as it is to gchsq_n, passing as parameter array the data
  array in the gsl vector.

  @param par         (gsl_vector *) A gsl vector
  @param void *npa   A mst_gen_nad struct

  @return double gchsq_sim: gchsq_n(par -> data, npa)

*/
/* ------------------------------------------------------------ */
#ifdef GFT_GSL_PRESENT
static double gchsq_sim(const gsl_vector *nopar, void *npa);
#endif


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int copyvec(void *from, void *to, size_t size, size_t length)
  @brief Copy an array

  Copies the content of from to to. size_t is the size of a member
  (can be accessed by the sizeof operator), length is the number of
  elements. If from is NULL, the error returned is GFT_ERROR_OBSOLETE_OP,
  if to is NULL, the error is GFT_ERROR_NULL_PASSED.

  @param from   (void *)   array to copy
  @param to     (void *)   goal of copy process
  @param size   (size_t)   size of array element
  @param length (size_t)   length of array (part) to copy

  @return (success) int copyvec: GFT_ERROR_NONE
          (error)                standard
*/
/* ------------------------------------------------------------ */
static int copyvec(void *from, void *to, size_t size, size_t length);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int copydblvec(double *from, double **to, size_t length)
  @brief Copy an array

  If the content of to is NULL, this function allocates a double array
  of size length and sets to to this array. In any case it copies the
  length first elements of from to *to. If from is NULL, or length is
  < 1, *to is deallocated (if it exists) and set to NULL. So, *to
  should be dynamically allocated. Action takes place even if an error
  is returned.

  @param from   (double *)    array to copy
  @param to     (double **)   goal of copy process
  @param length (size_t)      length of array (part) to copy

  @return (success) int copydblvec: GFT_ERROR_NONE
          (error)                   standard
*/
/* ------------------------------------------------------------ */
static int copydblvec(double *from, double **to, size_t length);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int cklimits(size_t length, double *array)
  @brief Check if the array is within the numberical limits

  Will check every number in the array of length n if the numerical
  limits are reached. This will return GFT_ERROR_OVERFLOW if the
  absolute number of one of the numbers is greater than DBL_MAX.

  @param length (size_t)    length of array
  @param array  (double *)  array to check

  @return (success) int cklimits: GFT_ERROR_NONE
          (error)                 standard
*/
/* ------------------------------------------------------------ */
static int cklimits(size_t length, double *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static double makenormalnumber(double number)
  @brief Check for overflow, in case of overflow, replace by largest double

  This is the most primitive way to do that and I hope it's standard.

  @param number  (double)  the number

  @return double makenormalnumber

*/
/* ------------------------------------------------------------ */
static double makenormalnumber(double number);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int gft_pswarm_standardprint(pswarm_swarm *pswarm_swarmv)
  @brief The output function for pswarm

  This suppresses any success messages to be printed to stdout, but reports only on errors.

  @param pswarm_swarmv (pswarm_swarm)  Properly allocated pswarm_swarm struct (see pswarm description).

  @return int gft_pswarm_standardprint() success: 0
                                         error:   1

*/
/* ------------------------------------------------------------ */
static int gft_pswarm_standardprint(pswarm_swarm *pswarm_swarmv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static double gft_psw_startsize(int n, double *lb, double *ub, double tol, double fdelta)
  @brief Calculates the initial size of the problem without using the pswarm library

  @param n      (int)      Dimension
  @param lb     (double *) Lower bounds (array size is dimension)
  @param ub     (double *) Upper bounds (array size is dimension)
  @param tol    (double)   Tolerance (the same as stopsize)
  @param fdelta (double)   Some factor, see pswarm library, use 5.0 as a default.


  @return double gft_psw_startsize() size of the problem, DBL_MAX on error.

*/
/* ------------------------------------------------------------ */
static double gft_psw_startsize(int n, double *lb, double *ub, double tol, double fdelta);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int outfcn(pswarm_options *opt, pswarm_swarm *pop)
  @brief Output for testing pswarm implementation

  Prints some information.

  @param opt    (pswarm_options *) Properly allocated pswarm_options struct
  @param pop    (pswarm_swarm *)   Properly allocated pswarm_swarm struct

  @return int outfcn() 0: success, != 0 on error

*/
/* ------------------------------------------------------------ */
/* static int outfcn(pswarm_options *opt, pswarm_swarm *pop); */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION CODE */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get top layer to fitting control object */

gft_mst *gft_mst_const() 
{
  return (gft_mst *) mst_const();
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* destroys gft_mst * struct */

int gft_mst_destr(gft_mst *gft_mstv) 
{
  return mst_destr((mst *) gft_mstv);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Input of info */

int gft_mst_put (gft_mst *gft_mstv, void *input, int spec)
{
  return mst_put((mst *) gft_mstv, input, spec);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Input of info */

int gft_mst_putf (gft_mst *gft_mstv, double (*input)(double *, void *), int spec)
{
  return mst_putf((mst *) gft_mstv, input, spec);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Output of info */

int gft_mst_get(gft_mst *gft_mstv, void *output, int spec)
{
  return mst_get((mst *) gft_mstv, output, spec);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Action! */

int gft_mst_act(gft_mst *gft_mstv, int spec)
{
  return mst_act((mst *) gft_mstv, spec);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Constructs a mst struct */

static mst *mst_const()
{
  mst *mst_const = NULL;

  if (!(mst_const = (mst *) malloc(sizeof(mst))))
    return NULL;

  mst_const -> method = MET_NONE;
  mst_const -> gen = NULL;
  mst_const -> spe = NULL;

  if (!(mst_const -> gen = mst_gen_const())) {
    mst_destr(mst_const);
    return NULL;
  }

  return mst_const;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destroys mst * struct */

static int mst_destr(mst *mstv)
{
  int mst_destr = GFT_ERROR_NONE;

  if (!mstv)
    return GFT_ERROR_NULL_PASSED;

  if ((mstv -> gen))
    mst_destr = mst_gen_destr(mstv -> gen) | GFT_ERROR_MEMORY_LEAK;

  if ((mstv -> spe))
    mst_destr = mst_spe_destr(mstv -> spe, mstv -> method) | GFT_ERROR_MEMORY_LEAK;

  free(mstv);
  return mst_destr;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Flush mst * struct */

static int mst_flush(mst *mstv)
{
  int mst_flush = GFT_ERROR_NONE;

  if (!mstv)
    return GFT_ERROR_NULL_PASSED;

  if ((mstv -> gen))
    mst_flush = mst_gen_destr(mstv -> gen) | GFT_ERROR_MEMORY_LEAK;

  if ((mstv -> spe))
    mst_flush = mst_spe_destr(mstv -> spe, mstv -> method) | GFT_ERROR_MEMORY_LEAK;

  mstv -> gen = NULL;
  mstv -> spe = NULL;

  return mst_flush;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Stop fitting */

static int mst_stop(mst *mstv)
{
  if (!mstv)
    return GFT_ERROR_NULL_PASSED;

  if (!(mstv -> gen -> busy || mstv -> gen -> stopped))
    return GFT_ERROR_NONE;

  if (!mst_gen_ckbu(mstv -> gen))
    mstv -> gen -> busy = GFT_BUSY_CHAR_YES;
    mstv -> gen -> stopped = GFT_STOP_CHAR_STOP;

  return GFT_ERROR_NONE;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Action!!! Somewhere here a completeness check, please */

static int mst_start(mst *mstv)
{
  int mst_start_ret = GFT_ERROR_NONE;

  /* This is an endless loop replacing a recursive definition of this
     function that caused a segfault */
  while (1) {
    /* Check if action is allowed */
    if (!mstv || !mstv -> gen)
      return GFT_ERROR_NULL_PASSED;
    
    /* Check if this can be started at all */
    if (mstv -> gen -> misinf)
      return GFT_ERROR_MISSING_INFO;
    
    /* Check if we do anything, if there's an error or the process has
       been stopped within execution of one loop, we do this */
    if ((mstv -> gen -> error) || (mstv -> gen -> stopped & GFT_STOP_CHAR_STOP)) {
      mstv -> gen -> stopped = GFT_STOP_CHAR_DO;
      mstv -> gen -> busy = GFT_BUSY_CHAR_YES;
      return mstv -> gen -> error;
    }
    
    /* A break */
    if (mstv -> gen -> stopped == GFT_STOP_CHAR_DO && mstv -> gen -> busy == GFT_BUSY_CHAR_NO) {
      mstv -> gen -> stopped = GFT_STOP_CHAR_ID;
      return mstv -> gen -> error;
    }
    
    /* A continued stop */
    if (mstv -> gen -> stopped & GFT_STOP_CHAR_DO && mstv -> gen -> busy)
      mstv -> gen -> stopped = GFT_STOP_CHAR_ID;
    
    /* This indicates a new start, one has to initialise */
    if (!mstv -> gen -> busy) {

      /* If there are no loops or similar, we just initiate and return */
      if ( !mstv -> gen -> ncalls			\
	   || !mstv -> gen -> niters			\
	   || !mstv -> gen -> loops) {
	mstv -> gen -> busy = GFT_BUSY_CHAR_NO;
	mstv -> gen -> error = mst_gen_init(mstv -> gen);
	mstv -> gen -> stopped = GFT_STOP_CHAR_DO;
	return mstv -> gen -> error;
      }

      if ((mstv -> gen -> error = (mst_initspe(mstv))))
	return mstv -> gen -> error;
      
      /* We're at it, we ignore the initialisation */
      mstv -> gen -> busy = GFT_BUSY_CHAR_YES;
      mstv -> gen -> calls = 0;
      mstv -> gen -> iters = 0;
      mstv -> gen -> calls_st = 0;
      mstv -> gen -> loop = 0;
    }

    /* Do an iteration */
    mst_start_ret = mst_iterspe(mstv);
    
    /* Now check if we're finished already because of parameters */
    if (mstv -> gen -> calls >= mstv -> gen -> ncalls		\
	|| mstv -> gen -> iters >= mstv -> gen -> niters	\
	|| mstv -> gen -> loop >= mstv -> gen -> loops) {
      mstv -> gen -> stopped = GFT_STOP_CHAR_DO;
      mstv -> gen -> busy = GFT_BUSY_CHAR_NO;
      ++mstv -> gen -> minruns;
    }
  }
  
  /* DEBUG: Before the endless loop above was reached with the
     following recursive call, which resulted in a
     segfault. Obviously, the stack filled up for no obvious
     reason. Therefore, this was changed into an endless loop */
  /*   mst_start(mstv); */
  
  /* Dummy */
  return mst_start_ret;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Break fitting */

static int mst_iterspe(mst *mstv)
{
  int mst_iterspe = GFT_ERROR_NONE;
  /**********/
  /**********/
/*   fprintf(stderr,"mst_iterspe mst_iterspe_ret/error: %i %i\n", mst_iterspe, mstv -> gen -> error); */
  /**********/

  switch (mstv -> method) {
  case GFT_MET_SIMPLEX:
    mst_iterspe = mst_sim_iter((mst_sim *) mstv -> spe, mstv -> gen);
    break;
  case GFT_MET_PSWARM:
    mst_iterspe = mst_psw_iter((mst_psw *) mstv -> spe, mstv -> gen);
    break;
  case GFT_MET_GOLDEN:
    mst_iterspe = mst_gol_iter((mst_gol *) mstv -> spe, mstv -> gen);
    break;
  }

  /* This should work without checking of anything, all checks are
     done in the calling functions */
  return mst_iterspe;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Break fitting */

static int mst_break(mst *mstv)
{
  if (!mstv)
    return GFT_ERROR_NULL_PASSED;

  /* This sets the break status to 1, the fitting loop will be broken
     instantly */
  if ((mstv -> gen -> busy)) {
    if ((mstv -> gen -> stopped)) {
      mstv -> gen -> stopped = mstv -> gen -> busy = 0;
    }
    else {
      mstv -> gen -> busy = 0;
      mstv -> gen -> stopped = 1;
    }
  }

  return GFT_ERROR_NONE;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* specify method for fitting */

static int mst_put(mst *mstv, void *input, int spec)
{
  int mst_put = GFT_ERROR_NONE;
  double *input_dbl;
  int    *input_int;
  size_t *input_size_t;
  size_t i;

  if (!(mstv && mstv -> gen))
    return GFT_ERROR_NULL_PASSED;

  if (mstv -> gen -> error)
    return GFT_ERROR_ERROR_PRESENT;
  
  if (!input)
    mst_put |= GFT_ERROR_NULL_PASSED;

  /* These are allowed during fitting */
  switch (spec) {
    
    /* external arguments */
  case GFT_INPUT_ADAR:
    mstv -> gen -> adar = input;
    break;

    /* These are allowed only when idle */
  default:
    if (mst_gen_ckbu(mstv -> gen)) {
/*       fprintf(stderr,"heyho "); */
      return GFT_ERROR_BUSY;
    }

    switch (spec) {

      /* fit method */
    case GFT_INPUT_METHOD:
      
      /* This is an int */
      input_int = (int *) input;
      
      if (mst_ckme(*input_int)) {
	mst_put |= GFT_ERROR_WRONG_IDENT;
	break;
      }
      
      /* Now destroy the current spe struct */
      mst_put |= mst_spe_destr(mstv -> spe, mstv -> method);
      
      /* Reset the spe struct */
      mstv -> spe = NULL;
      
      /* These are not valid anymore */
      FLUSH_COND(mstv -> gen -> solpar);
      FLUSH_COND(mstv -> gen -> solerr);

      /* specify method */
      mstv -> method = *input_int;
      break;
      
      /* Number of parameters, heaviest thing to do */
    case GFT_INPUT_NPAR:
      
      input_size_t = (size_t *) input;
      
      if (!input_size_t) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      
      /* less than 0 counts as 0 */
      mstv -> gen -> npar = *input_size_t;
      
      /* Now flush the generic part and destroy the specific part */
      mst_put |= mst_gen_flush(mstv -> gen);
      mst_put |= mst_spe_destr(mstv -> spe, mstv -> method);
      mstv -> spe = NULL;
      break;
      
      /* Actual parameters */
    case GFT_INPUT_PAR:
      input_dbl = (double *) input;
      mst_put |= copydblvec(input_dbl, &mstv -> gen -> par, mstv -> gen -> npar);
      break;
      
      /* Start parameters */
    case GFT_INPUT_SPAR:
      input_dbl = (double *) input;
      mst_put |= copydblvec(input_dbl, &mstv -> gen -> spar, mstv -> gen -> npar);
      break;
      
      /* Upper bounds */
    case GFT_INPUT_UBOUNDS:
      input_dbl = (double *) input;
      mst_put |= copydblvec(input_dbl, &mstv -> gen -> ubounds, mstv -> gen -> npar);
      break;
      
      /* Start parameters */
    case GFT_INPUT_LBOUNDS:
      input_dbl = (double *) input;
      mst_put |= copydblvec(input_dbl, &mstv -> gen -> lbounds, mstv -> gen -> npar);
      break;
      
      /* Grid origin */
    case GFT_INPUT_OPAR:
      input_dbl = (double *) input;
      mst_put |= copydblvec(input_dbl, &mstv -> gen -> opar, mstv -> gen -> npar);
      break;
      
      /* Start step width */
    case GFT_INPUT_DPAR:
      input_dbl = (double *) input;
      mst_put |= copydblvec(input_dbl, &mstv -> gen -> dpar, mstv -> gen -> npar);
      break;
      
      /* Grid normalisation */
    case GFT_INPUT_NDPAR:
      input_dbl = (double *) input;
      if (mstv -> gen -> npar && input_dbl)
	for (i = 0; i < mstv -> gen -> npar; ++i) {
	  if (!input_dbl[i])
	    mst_put |= GFT_ERROR_WRONG_PARAM;
	}
      if (mst_put & GFT_ERROR_WRONG_PARAM)
	break;
      mst_put |= copydblvec(input_dbl, &mstv -> gen -> ndpar, mstv -> gen -> npar);
      break;
     
      /* Total allowed function calls */
    case GFT_INPUT_NCALLS:
      input_size_t = (size_t *) input;
      if (!input_size_t) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> ncalls = *input_size_t;
      break;
      
      /* Maximum number of iteration steps */
    case GFT_INPUT_NITERS:
      input_size_t = (size_t *) input;
      
      if (!input_size_t) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> niters = *input_size_t;
      break;
      
      /* Function calls within a step, must be positive */
    case GFT_INPUT_NCALLS_ST:
      input_size_t = (size_t *) input;
      
      if (!input_size_t) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> ncalls_st = *input_size_t;
      break;
      
      /* Stopping condition, size of vector, must be positive */
    case GFT_INPUT_STOPSIZE:
     input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      
      if (*input_dbl < 0)
	mstv -> gen -> stopsize = 0.0;
      else
	mstv -> gen -> stopsize = *input_dbl;
      break;
      
      /* loops */
    case GFT_INPUT_LOOPS:
      input_size_t = (size_t *) input;
      
      if (!input_size_t) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> loops = *input_size_t;
      break;
      
      /* factor to multiply ncalls_st from loop to loop, must be positive */
    case GFT_INPUT_NCALLS_ST_FAC:
      input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      
      if (*input_dbl < 0)
	mstv -> gen -> ncalls_st_fac = 0.0;
      else
	mstv -> gen -> ncalls_st_fac = *input_dbl;
      break;
      
      /* factor to multiply step widths with from loop to loop, everything but zero */
    case GFT_INPUT_DPAR_FAC:
      input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      
      if (*input_dbl == 0)
	mstv -> gen -> dpar_fac = 1.0;
      else
	mstv -> gen -> dpar_fac = *input_dbl;
      break;
      
      /* factor to multiply stopping condition with from loop to loop */
    case  GFT_INPUT_STOPSIZE_FAC:
      input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      
      if (*input_dbl < 0)
	mstv -> gen -> stopsize_fac = 0.0;
      else
	mstv -> gen -> stopsize_fac = *input_dbl;
      break;

    case  GFT_INPUT_INDPOINTS:
      input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      
      mstv -> gen -> indpoints = *input_dbl;

      /* Report here if the parameter was bad */
    if (mstv -> gen -> indpoints - (double) mstv -> gen -> npar < 1.0)
      mst_put |= GFT_ERROR_WRONG_PARAM;

      break;
      
    case GFT_INPUT_SEED:
      input_int = (int *) input;
      
      if (!input_int) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> seed = *input_int;
      break;

    case GFT_INPUT_PSNPART:
      input_int = (int *) input;
      
      if (!input_int) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> psnpart = *input_int;
      break;

      /* pswarm cognitional parameter */
    case GFT_INPUT_PSCOGNI:
      input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> pscogni = *input_dbl;
      break;
      
      /* pswarm cognitional parameter */
    case GFT_INPUT_PSSOCIA:
      input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> pssocia = *input_dbl;
      break;

    case GFT_INPUT_PSMAXVF:
      input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> psmaxvf = *input_dbl;
      break;
      
    case GFT_INPUT_PSNITFI:
      input_int = (int *) input;
      
      if (!input_int) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> psnitfi = *input_int;
      break;
      
    case GFT_INPUT_PSINIIN:
      input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> psiniin = *input_dbl;
      break;
      
    case GFT_INPUT_PSFININ:
      input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> psfinin = *input_dbl;
      break;

    case GFT_INPUT_PSINCDE:
      input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> psincde = *input_dbl;
      break;

    case GFT_INPUT_PSDECDE:
      input_dbl = (double *) input;
      
      if (!input_dbl) {
	mst_put |= GFT_ERROR_NULL_PASSED;
	break;
      }
      mstv -> gen -> psdecde = *input_dbl;
      break;
      
    default:
      return GFT_ERROR_WRONG_IDENT;
    }
  }

  return mst_refresh(mstv) | mst_put | ckmetinp(mstv -> method, spec);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* specify fitting function */

static int mst_putf(mst *mstv, double (*input)(double *, void *), int spec)
{
  int mst_putf = GFT_ERROR_NONE;

  if (!(mstv && mstv -> gen))
    return GFT_ERROR_NULL_PASSED;

  if (mstv -> gen -> error)
    return GFT_ERROR_ERROR_PRESENT;
  
  if (!input)
    mst_putf |= GFT_ERROR_NULL_PASSED;

  /* These are allowed during fitting */
  switch (spec) {
    
    /* external function without flushing best fit parameters */
  case GFT_INPUT_GCHSQ_REP:
    mstv -> gen -> gchsq = input;
	 return mst_putf;
    break;
    
    /* These are allowed only when idle */
  default:
    if (mst_gen_ckbu(mstv -> gen))
      return GFT_ERROR_BUSY;
    
    switch (spec) {

      /* external function */
    case GFT_INPUT_GCHSQ:
      
      if (!input)
	mst_putf |= GFT_ERROR_NULL_PASSED;
      
      /* the chisquares are not valid anymore */
      FLUSH_COND(mstv -> gen -> bestpar);
      FLUSH_COND(mstv -> gen -> solpar);
      FLUSH_COND(mstv -> gen -> solerr);
      
      /* This is a new function that has never been called */
      mstv -> gen -> allcalls = 0;
      mstv -> gen -> alliter = 0;
      mstv -> gen -> alloops = 0;
      
      mstv -> gen -> gchsq = input;
      break;
      
    default:
      return GFT_ERROR_WRONG_IDENT;
    }
  }

  return mst_refresh(mstv) | mst_putf;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Generically get information from a minimiser struct, intern */

static int mst_get(mst *mstv, void *output, int spec)
{
  int mst_get = GFT_ERROR_NONE;
  if (!mstv)
    return GFT_ERROR_NULL_PASSED;

  /* Note that if the mst struct is there, the gen component will be
     allocated, too. This is silly, why a gen struct at all? This is
     an error of history that is hard to remove, and I'm too lazy */

  switch (spec) {
  case GFT_OUTPUT_MISINF:
    mst_get |= copyvec(&mstv -> gen -> misinf, output, sizeof(int), 1);
    break;
  case GFT_OUTPUT_BUSY:
    mst_get |= copyvec(&mstv -> gen -> busy, output, sizeof(int), 1);
    break;
  case GFT_OUTPUT_STOPPED:
    mst_get |= copyvec(&mstv -> gen -> stopped, output, sizeof(int), 1);
    break;
  case GFT_OUTPUT_ERROR:
    mst_get |= copyvec(&mstv -> gen -> error, output, sizeof(int), 1);
    break;
  case GFT_OUTPUT_ALLCALLS:
    mst_get |= copyvec(&mstv -> gen -> allcalls, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_ALLITER:
    mst_get |= copyvec(&mstv -> gen -> alliter, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_ALLOOPS:
    mst_get |= copyvec(&mstv -> gen -> alloops, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_MINRUNS:
    mst_get |= copyvec(&mstv -> gen -> minruns, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_METHOD:
    mst_get |= copyvec(&mstv -> method, output, sizeof(int), 1);
    break;
  case GFT_OUTPUT_NPAR:
    mst_get |= copyvec(&mstv -> gen -> npar, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_NPAR_CUR:
    mst_get |= copyvec(&mstv -> gen -> npar_cur, output, sizeof(int), 1);
    break;
  case GFT_OUTPUT_INDPOINTS:
    mst_get |= copyvec(&mstv -> gen -> indpoints, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_ACTCHISQ:
    mst_get |= copyvec(&mstv -> gen -> actchisq, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_ACTCHISQRED:
    mst_get |= copyvec(&mstv -> gen -> actchisqred, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_BESTCHISQ:
    mst_get |= copyvec(&mstv -> gen -> bestchisq, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_BESTCHISQRED:
    mst_get |= copyvec(&mstv -> gen -> bestchisqred, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_PAR:
    mst_get |= copyvec(mstv -> gen -> par, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_BESTPAR:
    mst_get |= copyvec(mstv -> gen -> bestpar, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_SOLPAR:
    mst_get |= copyvec(mstv -> gen -> solpar, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_SOLERR:
    mst_get |= copyvec(mstv -> gen -> solerr, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_SOLCHSQ:
    mst_get |= copyvec(&mstv -> gen -> solchsq, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_SOLCHSQRED:
    mst_get |= copyvec(&mstv -> gen -> solchsqred, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_SPAR:
    mst_get |= copyvec(mstv -> gen -> spar, output, sizeof(double),  mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_OPAR:
    mst_get |= copyvec(mstv -> gen -> opar, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_UBOUNDS:
    mst_get |= copyvec(mstv -> gen -> ubounds, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_LBOUNDS:
    mst_get |= copyvec(mstv -> gen -> lbounds, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_DPAR:
    mst_get |= copyvec(mstv -> gen -> dpar, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_NDPAR:
    mst_get |= copyvec(mstv -> gen -> ndpar, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_ADAR:
    output = mstv -> gen -> adar;
    break;
  case GFT_OUTPUT_NCALLS:
    mst_get |= copyvec(&mstv -> gen -> ncalls, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_CALLS:
    mst_get |= copyvec(&mstv -> gen -> calls, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_NITERS:
    mst_get |= copyvec(&mstv -> gen -> niters, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_ITERS:
    mst_get |= copyvec(&mstv -> gen -> iters, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_NCALLS_ST:
    mst_get |= copyvec(&mstv -> gen -> ncalls_st, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_CALLS_ST:
    mst_get |= copyvec(&mstv -> gen -> calls_st, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_STOPSIZE:
    mst_get |= copyvec(&mstv -> gen -> stopsize, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_STOPSIZE_ACT:
    mst_get |= copyvec(&mstv -> gen -> stopsize_act, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_SIZE:
    mst_get |= copyvec(&mstv -> gen -> size, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_DSIZE:
    mst_get |= copyvec(&mstv -> gen -> dsize, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_LOOPS:
    mst_get |= copyvec(&mstv -> gen -> loops, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_LOOP:
    mst_get |= copyvec(&mstv -> gen -> loop, output, sizeof(size_t), 1);
    break;
  case GFT_OUTPUT_NCALLS_ST_FAC:
    mst_get |= copyvec(&mstv -> gen -> ncalls_st_fac, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_DPAR_FAC:
    mst_get |= copyvec(&mstv -> gen -> dpar_fac, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_STOPSIZE_FAC:
    mst_get |= copyvec(&mstv -> gen -> stopsize_fac, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_NOPAR:
    mst_get |= copyvec(mstv -> gen -> nopar, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_NOUBOUNDS:
    mst_get |= copyvec(mstv -> gen -> noubounds, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_NOLBOUNDS:
    mst_get |= copyvec(mstv -> gen -> nolbounds, output, sizeof(double), mstv -> gen -> npar);
    break;
  case GFT_OUTPUT_SEED:
    mst_get |= copyvec(&mstv -> gen -> seed, output, sizeof(int), 1);
    break;
  case GFT_OUTPUT_PSNPART:
    mst_get |= copyvec(&mstv -> gen -> psnpart, output, sizeof(int), 1);
    break;
  case GFT_OUTPUT_PSCOGNI:
    mst_get |= copyvec(&mstv -> gen -> pscogni, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_PSSOCIA:
    mst_get |= copyvec(&mstv -> gen -> pssocia, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_PSMAXVF:
    mst_get |= copyvec(&mstv -> gen -> psmaxvf, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_PSNITFI:
    mst_get |= copyvec(&mstv -> gen -> psnitfi, output, sizeof(int), 1);
    break;
  case GFT_OUTPUT_PSINIIN:
    mst_get |= copyvec(&mstv -> gen -> psiniin, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_PSFININ:
    mst_get |= copyvec(&mstv -> gen -> psfinin, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_PSINCDE:
    mst_get |= copyvec(&mstv -> gen -> psincde, output, sizeof(double), 1);
    break;
  case GFT_OUTPUT_PSDECDE:
    mst_get |= copyvec(&mstv -> gen -> psdecde, output, sizeof(double), 1);
    break;
  default:
    mst_get |= GFT_ERROR_WRONG_PARAM;
  }

  return mst_get | mst_ckop(mstv, spec);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* check if an output makes sense */

static int mst_ckop(mst *mstv, int spec)
{
  int mst_ckop = GFT_ERROR_NONE;

  /***********/
  /***********/
/*   fprintf(stderr,"mst_ckop %i\n", mst_ckop); */
  /***********/

  mst_ckop |= mst_gen_ckop(mstv -> gen, spec);

  mst_ckop |= mst_spe_ckop(mstv -> spe, spec, mstv -> method);

  return mst_ckop;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Action! */

static int mst_act(mst *mstv, int spec)
{
  int mst_act = GFT_ERROR_NONE;
  /* The called functions will check a NULL pointer passed */

  switch (spec) {
  case GFT_ACT_FLUSH:
    mst_act |= mst_flush(mstv);
    if ((mstv))
      mst_act |= (mstv -> gen = mst_gen_const())?0:GFT_ERROR_MEMORY_ALLOC;
    break;
  case GFT_ACT_INIT:
    return mst_gen_init(mstv -> gen);
    break;
  case GFT_ACT_STOP:
    return mst_stop(mstv);
    break;
  case GFT_ACT_START:
    return mst_start(mstv);
    break;
  case GFT_ACT_BREAK:
    return mst_break(mstv);
    break;
  default:
    mst_act |= GFT_ERROR_WRONG_IDENT;
  }

  return mst_act;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* check existence of fit method */

static int mst_ckme(int method)
{
  switch (method) {
  case GFT_MET_SIMPLEX:
    return MET_SIMPLEX>0?GFT_ERROR_NONE:GFT_ERROR_WRONG_IDENT;
  case GFT_MET_GOLDEN:
    return GFT_ERROR_NONE;
  case GFT_MET_PSWARM:
    return GFT_ERROR_NONE;
  default:
    ;
  }

  return GFT_ERROR_WRONG_IDENT;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Do initialisations of specific minimiser that needs 
          a call of the minimising function */

static int mst_initspe(mst *mstv)
{
  int mst_initspe = GFT_ERROR_NONE;

  switch (mstv -> method) {
  case GFT_MET_SIMPLEX:
    mst_initspe = mst_sim_init((mst_sim *) mstv -> spe, mstv -> gen);
    break;
  case GFT_MET_GOLDEN:
    mst_initspe = mst_gol_init((mst_gol *) mstv -> spe, mstv -> gen);
    break;
  case GFT_MET_PSWARM:
    mst_initspe = mst_psw_init((mst_psw *) mstv -> spe, mstv -> gen);
    break;
  }

  /* This should work without checking of anything, all checks are
     done in the calling functions */
  return mst_initspe;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* static int mst_refresh(mst *mstv) */

static int mst_refresh(mst *mstv)
{
  int mst_refresh = GFT_ERROR_NONE;

  mstv -> gen -> misinf = GFT_ERROR_NONE;

  /* set misinf refreshing the generic part */
  mst_refresh |= mst_gen_refresh(mstv -> gen);
  mstv -> gen -> misinf |= mst_refresh;

  /* refresh the specific part if there are no memory problems */
  if (!(mstv -> gen -> misinf & GFT_ERROR_MEMORY_ALLOC)) {
    mst_refresh |= mst_refreshspe(mstv);
    mstv -> gen -> misinf |= mst_refresh;
  }

  /* We got here, so we were successful */
  return mst_refresh;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Refresh specific part of fit control struct */

static int mst_refreshspe(mst *mstv)
{
  int mst_refreshspe = GFT_ERROR_NONE;

  if (!mstv)
    return 1;

  /* Check existence of fit method */
  if (mst_ckme(mstv -> method)) {
    mstv -> gen -> misinf |= GFT_ERROR_WRONG_IDENT;
    return GFT_ERROR_NONE;
  }

  /* If the spe part is not allocated, do so */
  if (!(mstv -> spe)) {
    if (!(mstv -> spe = mst_spe_const(mstv -> method)))
      return GFT_ERROR_MEMORY_ALLOC;
  }

  /* refresh the specific part */
  switch(mstv -> method) {
  case GFT_MET_SIMPLEX:
    mst_refreshspe = mst_refreshsim(mstv);
    break;
  case GFT_MET_GOLDEN:
    mst_refreshspe = mst_refreshgol(mstv);
    break;
  case GFT_MET_PSWARM:
    mst_refreshspe = mst_refreshpsw(mstv);
    break;
  }

  /* finis */
  return mst_refreshspe;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Refresh specific part of fit control struct */

static int mst_refreshsim(mst *mstv)
#if MET_SIMPLEX == -2
{
  return GFT_ERROR_WRONG_IDENT;
}
#else
{
  size_t i;
  double cl, srt;
  int mst_refreshsim = GFT_ERROR_NONE;
  mst_sim *mst_simv;

  mst_simv = (mst_sim *) mstv -> spe;

  /* The specific function is in any case this one */
  mst_simv -> multimin_function_gsl -> f =  &gchsq_sim;

  /* The additional parameters are allocated */
  mst_simv -> multimin_function_gsl -> params = (void *) mstv -> gen;

  /* Also we know that we will use the normalised function */
  mstv -> gen -> gchsq_n = &gchsq_n;

  /* Check if the number of parameters is clear and basic allocations are made */
  if (mstv -> gen -> npar && mstv -> gen -> spar &&  mstv -> gen -> dpar) {

    /* Slot in the start grid vectors, they exist if this is called */
    for (i = 0; i < mstv -> gen -> npar; ++i) {
      mstv -> gen -> nospar[i] = (mstv -> gen -> spar[i]-mstv -> gen -> opar[i])/mstv -> gen -> ndpar[i];
      mstv -> gen -> nodpar[i] = mstv -> gen -> dpar[i]/mstv -> gen -> ndpar[i];
      mstv -> gen -> nopar[i]  = (mstv -> gen -> par[i]-mstv -> gen -> opar[i])/mstv -> gen -> ndpar[i];
    }

    /* The normalisation for the characteristic length is given by */
    mst_simv -> vlnorm = 1.0/(((double) mstv -> gen -> npar*sqrt((double) mstv -> gen -> npar*(double) mstv -> gen -> npar+(double) mstv -> gen -> npar-1)+sqrt((double) mstv -> gen -> npar))/(((double) mstv -> gen -> npar+1)*((double) mstv -> gen -> npar+1)));

    /* calculate the inverse of the characteristic length given by the gsl without initialising */
    cl = 0.0;
    srt = 0.0;
    for (i = 0; i < mstv -> gen -> npar; ++i) {
      srt += mstv -> gen -> nodpar[i]*mstv -> gen -> nodpar[i];
    }
    for (i = 0; i < mstv -> gen -> npar; ++i) {
      cl += sqrt(((double) mstv -> gen -> npar* (double) mstv -> gen -> npar-1)*mstv -> gen -> nodpar[i]*mstv -> gen -> nodpar[i]+srt);
    }
    cl += sqrt(srt);

    /* Now this is the characteristic length that we can give */
    mstv -> gen -> size = mst_simv -> vlnorm*cl/(((double) mstv -> gen -> npar+1)*((double) mstv -> gen -> npar+1));

    /* If already done, this causes no memory leak, since the
       minimiser won't touch nodpar, we do not make a local copy */
/*     if (!(mst_refreshsim = copydblvec(mstv -> gen -> nodpar, &mst_simv -> stp_vec, mstv -> gen -> npar))) { */

/*       fill_gsl_dbl_vector(mst_simv -> stp_gsl_vec, mst_simv -> stp_vec, &mst_simv -> stp_gsl_block, mstv -> gen -> npar); */
/*     } */

     mstv -> gen -> misinf |= mst_refreshsim |= fill_gsl_dbl_vector(&mst_simv -> stp_gsl_vec, mstv -> gen -> nodpar, mstv -> gen -> npar);
     mstv -> gen -> misinf |= mst_refreshsim |= fill_gsl_dbl_vector(&mst_simv -> var_gsl_vec, mstv -> gen -> nospar, mstv -> gen -> npar);

    /*************/
    /*************/
/*     fprintf(stderr,"gsl step vector: "); */
/*     for (i = 0; i < mstv -> gen -> npar; ++i) { */
/*       fprintf(stderr,"%.1e ", gsl_vector_get (mst_simv -> stp_gsl_vec, i)); */
/*     } */
/*     fprintf(stderr,"\n"); */
/*     fprintf(stderr,"input tried:     "); */
/*     for (i = 0; i < mstv -> gen -> npar; ++i) { */
/*       fprintf(stderr,"%.1e ", mstv -> gen -> nodpar[i]); */
/*     } */
/*     fprintf(stderr,"\n"); */
    /*************/

    /* Copy the content of nopar to the local copy */
    /* ERROR SOURCE??? Here, we copy the start parameters, not the actual parameters */
/*     if (!(mst_refreshsim = copydblvec(mstv -> gen -> nospar, &mst_simv -> var_vec, mstv -> gen -> npar))) */
/*       fill_gsl_dbl_vector(mst_simv -> var_gsl_vec, mst_simv -> var_vec, &mst_simv -> var_gsl_block, mstv -> gen -> npar); */

    /* We can tell more about the function */
    mst_simv -> multimin_function_gsl -> n =  mstv -> gen -> npar;

    /* Now we can allocate the minimiser */
    if (!mst_simv -> multimin_fminimizer_gsl) {
     if (!(mst_simv -> multimin_fminimizer_gsl = gsl_multimin_fminimizer_alloc (mst_simv -> multimin_fminimizer_type_gsl, mstv -> gen -> npar)))
	mst_refreshsim |= GFT_ERROR_MEMORY_ALLOC;
    }

    /* What is missing is the initialisation of the minimiser calling
       gsl_multimin_fminimizer_set (mst_simv -> multimin_fminimizer_gsl,
       mst_simv -> multimin_function_gsl, mst_simv -> var_gsl_vec, mst_simv
       -> stp_gsl_vec); . Since this will call the chisquare function, we do
       this when starting the minimising process */

  }
  else 
    mstv -> gen -> misinf |= GFT_ERROR_MISSING_INFO;

  mst_simv -> eqchisq = 0;
  mst_simv -> eqchisq2 = 0;
  mst_simv -> chisqbef = -1.0;
  mst_simv -> chisqbef2 = -1.0;


  /* finis */
  return mst_refreshsim;
}
#endif

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Refresh specific part of fit control struct */

static int mst_refreshpsw(mst *mstv)
{
  size_t i;
  int mst_refreshpsw = GFT_ERROR_NONE;
  mst_psw *mst_pswv;

  mst_pswv = (mst_psw *) mstv -> spe;

  /* The specific function is in any case this one */
  /* mst_pswv -> multimin_function_gsl -> f =  &gchsq_psw; */

  /* The additional parameters are allocated */
  /* mst_simv -> multimin_function_gsl -> params = (void *) mstv -> gen; */

  /* Also we know that we will use the normalised function */
  mstv -> gen -> gchsq_n = &gchsq_n;

  /* Check if the number of parameters is clear and basic allocations are made */
  if (mstv -> gen -> npar && mstv -> gen -> spar &&  mstv -> gen -> dpar && mstv -> gen -> ubounds && mstv -> gen -> lbounds) {

    /* Slot in the start grid vectors, they exist if this is called */
    for (i = 0; i < mstv -> gen -> npar; ++i) {
      mstv -> gen -> nospar[i] = (mstv -> gen -> spar[i]-mstv -> gen -> opar[i])/mstv -> gen -> ndpar[i];
      mstv -> gen -> noubounds[i] = (mstv -> gen -> ubounds[i]-mstv -> gen -> opar[i])/mstv -> gen -> ndpar[i];
      mstv -> gen -> nolbounds[i] = (mstv -> gen -> lbounds[i]-mstv -> gen -> opar[i])/mstv -> gen -> ndpar[i];
      mstv -> gen -> nodpar[i] = mstv -> gen -> dpar[i]/mstv -> gen -> ndpar[i];
      mstv -> gen -> nopar[i]  = (mstv -> gen -> par[i]-mstv -> gen -> opar[i])/mstv -> gen -> ndpar[i];
    }

    /* If the vector does not yet exist, it will be created and be filled with the defaults */
    /* if (!(mst_pswv -> curnospar)) */
      copydblvec(mstv -> gen -> nospar, &mst_pswv -> curnospar, mstv -> gen -> npar);

    /* calculate the size without calling the function */
    if (mstv -> gen -> stopsize > 0.0)
      mstv -> gen -> size = gft_psw_startsize(mstv -> gen -> npar, mstv -> gen -> nolbounds, mstv -> gen -> noubounds, mstv -> gen -> stopsize, 5.0);
    else
      mstv -> gen -> size = DBL_MAX;

    /* Transfer nospar and nodpar ? No. */
    /* mstv -> gen -> misinf |= mst_refreshpsw |= fill_gsl_dbl_vector(&mst_pswv -> stp_gsl_vec, mstv -> gen -> nodpar, mstv -> gen -> npar); */
    /* mstv -> gen -> misinf |= mst_refreshpsw |= fill_gsl_dbl_vector(&mst_pswv -> var_gsl_vec, mstv -> gen -> nospar, mstv -> gen -> npar); */

    /* We can tell more about the function: mstv -> gen -> npar is known */
    /* mst_pswv -> multimin_function_gsl -> n =  mstv -> gen -> npar; */

    /* Now we can allocate the minimiser */
    /* if (!mst_simv -> multimin_fminimizer_gsl) { */
    /*  if (!(mst_simv -> multimin_fminimizer_gsl = gsl_multimin_fminimizer_alloc (mst_simv -> multimin_fminimizer_type_gsl, mstv -> gen -> npar))) */
    /* 	mst_refreshsim |= GFT_ERROR_MEMORY_ALLOC; */
    /* } */

    /* What is missing is the initialisation of the minimiser calling
       gsl_multimin_fminimizer_set (mst_simv -> multimin_fminimizer_gsl,
       mst_simv -> multimin_function_gsl, mst_simv -> var_gsl_vec, mst_simv
       -> stp_gsl_vec); . Since this will call the chisquare function, we do
       this when starting the minimising process */

  }
  else 
    mstv -> gen -> misinf |= GFT_ERROR_MISSING_INFO;

  /* finis */
  return mst_refreshpsw;
}


/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Refresh specific part of fit control struct */

static int mst_refreshgol(mst *mstv)
{
  size_t i;
  int mst_refreshgol = GFT_ERROR_NONE;
  mst_gol *mst_golv;


  mst_golv = (mst_gol *) mstv -> spe;

  /* The specific function is in any case this one */
/*   mst_golv -> gc -> gchsq =  &gchsq_gol; */
  golden_i_gchsq(&gchsq_gol, mst_golv -> gc);

  /* The additional parameters are allocated */
/*   mst_golv -> gc -> adar = (void *) mstv -> gen; */
  golden_i_adar((void *) mstv -> gen, mst_golv -> gc);

  /* Also we know that we will use the normalised function */
  mstv -> gen -> gchsq_n = &gchsq_n;

  /* Check if the number of parameters is clear and basic allocations are made */
  if (mstv -> gen -> npar && mstv -> gen -> spar &&  mstv -> gen -> dpar && mstv -> gen -> ncalls_st) {
    
    /* This (de-) allocates all arrays and resets parameters to a generic value */
    golden_refresh(mst_golv -> gc, mstv -> gen -> npar);
    
    /* Slot in the start grid vectors, they exist if this is called */
    for (i = 0; i < mstv -> gen -> npar; ++i) {
      mstv -> gen -> nospar[i] = (mstv -> gen -> spar[i]-mstv -> gen -> opar[i])/mstv -> gen -> ndpar[i];
      mstv -> gen -> nodpar[i] = mstv -> gen -> dpar[i]/mstv -> gen -> ndpar[i];
      mstv -> gen -> nopar[i]  = (mstv -> gen -> spar[i]-mstv -> gen -> opar[i])/mstv -> gen -> ndpar[i];
    }
    /* The normalisation for the characteristic length is given by */
    /* calculate the inverse of the characteristic length given by the gsl without initialising */
    /* Now this is the characteristic length that we can give */
    /*     mstv -> gen -> size = mst_golv -> gc -> solsize; */
    golden_o_solsize(&(mstv -> gen -> size), mst_golv -> gc);
    
    /* Indirect use of gc, hopefully working */
    /*     if (!(ndpar_here = (double *) malloc(mstv -> gen -> npar*sizeof(double)))) { */
    /*       mst_refreshgol |= GFT_ERROR_MEMORY_ALLOC; */
    /*       goto error; */
    /*     } */
    
    /* This is not initialised at this stage */
/*     golden_o_ncurstep(&ncurstep_here, mst_golv -> gc); */
    /*     golden_o_ndpar(ndpar_here, mst_golv -> gc); */
    
    /* This is not initialised at this stage */
/*     golden_o_npar_cur(&npar_cur_here, mst_golv -> gc); */
    /* the current npar after a refresh is -1 */
    mstv -> gen -> npar_cur = -1;    

    /* This is not initialised at this stage */
    mstv -> gen -> dsize =  mstv -> gen -> dpar[0];
        
    /* We can tell more about the function */
    /*     mst_golv -> gc -> npar =  mstv -> gen -> npar; */
    
    /* and fill in the vectors */
    /*      mstv -> gen -> misinf |= mst_refreshgol |= copydblvec(mstv -> gen -> nodpar, &mst_golv -> gc -> nodpar, mstv -> gen -> npar); */
    /*      mstv -> gen -> misinf |= mst_refreshgol |= copydblvec(mstv -> gen -> nospar, &mst_golv -> gc -> nospar, mstv -> gen -> npar); */
    /*      mst_golv -> gc -> ncalls_st = mstv -> gen -> ncalls_st; */
    /*      mst_golv -> gc -> minstep = 1.0; */
    mstv -> gen -> misinf |= mst_refreshgol |= golden_i_nodpar(mstv -> gen -> nodpar, mst_golv -> gc);
    mstv -> gen -> misinf |= mst_refreshgol |= golden_i_nospar(mstv -> gen -> nospar, mst_golv -> gc);

    /* Check if ncalls_st and minstep are initialised in gen */
    golden_i_ncalls_st(mstv -> gen -> ncalls_st, mst_golv -> gc);
    golden_i_minstep(1.0, mst_golv -> gc);
    
/*     for (i = 0; i < mst_golv -> gc -> npar; ++i) */
/*       fprintf(stderr,"%.3f ", mstv -> gen -> spar[i]); */


    /* What is missing is the initialisation of the minimiser calling
       gsl_multimin_fminimizer_set (mst_simv -> multimin_fminimizer_gsl,
       mst_simv -> multimin_function_gsl, mst_simv -> var_gsl_vec, mst_simv
       -> stp_gsl_vec); . Since this will call the chisquare function, we do
       this when starting the minimising process */

  }
  else 
    mstv -> gen -> misinf |= GFT_ERROR_MISSING_INFO;

  /* finis */
  return mst_refreshgol;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* allocate and initialise internal generic struct to mstr_in */

static mst_gen *mst_gen_const()
{
  mst_gen *mst_gen_const;

  if (!(mst_gen_const = (mst_gen *) malloc (sizeof(mst_gen))))
    return NULL;

/*   if (!(mst_gen_const -> nad = mst_gen_nad_const())) */
/*     return NULL; */

  /* Now initialise */
  mst_gen_const -> misinf = GFT_ERROR_STANDARD;
  mst_gen_const -> busy = GFT_ERROR_NONE;
  mst_gen_const -> error = GFT_ERROR_NONE;
  mst_gen_const -> stopped = GFT_ERROR_NONE;
  mst_gen_const -> broken = GFT_ERROR_NONE;
  mst_gen_const -> allcalls = 0;
  mst_gen_const -> alliter = 0;
  mst_gen_const -> alloops = 0;
  mst_gen_const -> minruns = 0;
  mst_gen_const -> npar = 0;
  mst_gen_const -> npar_cur = -1;
  mst_gen_const -> indpoints = 0.0;
  mst_gen_const -> actchisq = DBL_MAX;
  mst_gen_const -> actchisqred = DBL_MAX;
  mst_gen_const -> bestchisq = DBL_MAX;
  mst_gen_const -> bestchisqred = DBL_MAX;
  mst_gen_const -> solchsq = DBL_MAX;
  mst_gen_const -> solchsqred = DBL_MAX;
  mst_gen_const -> par = NULL;
  mst_gen_const -> dummypar = NULL;
  mst_gen_const -> dummypar2 = NULL;
  mst_gen_const -> bestpar = NULL;
  mst_gen_const -> solpar = NULL;
  mst_gen_const -> solerr = NULL;
  mst_gen_const -> spar = NULL;
  mst_gen_const -> ubounds = NULL;
  mst_gen_const -> lbounds = NULL;
  mst_gen_const -> noubounds = NULL;
  mst_gen_const -> nolbounds = NULL;
  mst_gen_const -> opar = NULL;
  mst_gen_const -> dpar = NULL;
  mst_gen_const -> ndpar = NULL;
  mst_gen_const -> gchsq = NULL;
  mst_gen_const -> adar = NULL;
  mst_gen_const -> ncalls = LARGE_INTEGER;
  mst_gen_const -> calls = 0;
  mst_gen_const -> niters = LARGE_INTEGER;
  mst_gen_const -> iters = 0;
  mst_gen_const -> ncalls_st = LARGE_INTEGER;
  mst_gen_const -> calls_st = 0;
  mst_gen_const -> stopsize = 0.0;
  mst_gen_const -> stopsize_act = 0.0;
  mst_gen_const -> size = 1.0;
  mst_gen_const -> dsize = 0.0;
  mst_gen_const -> loops = 1;
  mst_gen_const -> loop = 0;
  mst_gen_const -> ncalls_st_fac = 1.0;
  mst_gen_const -> dpar_fac = 1.0;
  mst_gen_const -> stopsize_fac = 1.0;
  mst_gen_const -> gchsq_n = NULL;
  mst_gen_const -> nopar = NULL;
  mst_gen_const -> nospar = NULL;
  mst_gen_const -> nodpar = NULL;
  /* Just copy-paste from gft defaults */
  mst_gen_const -> psnpart = 42;      /* Of course 42 */
  mst_gen_const -> seed    = 42;         /* Why not? */
  mst_gen_const -> pscogni = 0.5;     /* cognitial parameter */
  mst_gen_const -> pssocia = 0.5;     /* social parameter */
  mst_gen_const -> psmaxvf = 0.5;     /* maximum velocity factor */
  mst_gen_const -> psnitfi = 8000;    /* iterations until final weight is reached */
  mst_gen_const -> psiniin = 0.9;     /* initial weight (initial inertia) */
  mst_gen_const -> psfinin = 0.4;     /* final weight (final inertia) */
  mst_gen_const -> psincde = 2;       /* increase delta by a factor of */
  mst_gen_const -> psdecde = 0.5;     /* decrease delta by a factor of */

  return mst_gen_const;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destroys mst_genv */
static int mst_gen_destr(mst_gen *mst_genv)
{
  if (!mst_genv)
    return GFT_ERROR_NULL_PASSED;

  /* All allocations for sub-objects in this struct have pointers in
     this struct, therefore we can simply de-allocate */
  FREE_COND(mst_genv -> par);
  FREE_COND(mst_genv -> dummypar);
  FREE_COND(mst_genv -> dummypar2);
  FREE_COND(mst_genv -> spar);
  FREE_COND(mst_genv -> opar);
  FREE_COND(mst_genv -> dpar);
  FREE_COND(mst_genv -> ndpar);
  FREE_COND(mst_genv -> bestpar);
  FREE_COND(mst_genv -> solpar);
  FREE_COND(mst_genv -> solerr);
/*   mst_gen_nad_destr(mst_genv -> nad); */
  FREE_COND(mst_genv -> nopar);
  FREE_COND(mst_genv -> nospar);
  FREE_COND(mst_genv -> nodpar);

  free(mst_genv);

  return GFT_ERROR_NONE;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Flushes mst_genv */
static int mst_gen_flush(mst_gen *mst_genv)
{
  if (!mst_genv)
    return GFT_ERROR_NULL_PASSED;

  /* All allocations for sub-objects in this struct have pointers in
     this struct, therefore we can simply de-allocate */
  FLUSH_COND(mst_genv -> par);
  FLUSH_COND(mst_genv -> dummypar);
  FLUSH_COND(mst_genv -> dummypar2);
  FLUSH_COND(mst_genv -> spar);
  FLUSH_COND(mst_genv -> opar);
  FLUSH_COND(mst_genv -> dpar);
  FLUSH_COND(mst_genv -> ndpar);
  FLUSH_COND(mst_genv -> nopar);
  FLUSH_COND(mst_genv -> nospar);
  FLUSH_COND(mst_genv -> nodpar);
  FLUSH_COND(mst_genv -> bestpar);
  FLUSH_COND(mst_genv -> solpar);
  FLUSH_COND(mst_genv -> solerr);

  /* We change the error statuses to none */
  mst_genv -> misinf = GFT_ERROR_MISSING_INFO;
  mst_genv -> busy = GFT_ERROR_NONE;
  mst_genv -> stopped = GFT_ERROR_NONE;
  mst_genv -> error = GFT_ERROR_NONE;
  mst_genv -> broken = GFT_ERROR_NONE;

  return GFT_ERROR_NONE;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* check busy status of mst_gen struct */

static int mst_gen_ckbu(mst_gen *mst_genv)
{
    return mst_genv -> busy;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* check misinf status of mst_gen struct */

/* static int mst_gen_ckmi(mst_gen *mst_genv) */
/* { */
/*   return mst_genv -> misinf; */
/* } */

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Execute the fitting function once */

static int mst_gen_init(mst_gen *mst_genv)
{
  double chisquare;
  size_t i;
  int mst_gen_init = GFT_ERROR_NONE;

  if (!mst_genv)
    return GFT_ERROR_NULL_PASSED;

  /* This can only be done if the process is idle and an error is not present */

  if ((mst_genv  -> busy || mst_genv -> stopped))
    return GFT_ERROR_BUSY;

  if ((mst_genv -> error))
    return GFT_ERROR_ERROR_PRESENT;

  /* We need: a valid number of parameters, a present minimiser
     function, a vector to calculate the chisquare with. If I remember
     well, the last element of the following test is obsolete, but... */
  if ((mst_genv -> par) && (mst_genv -> gchsq) && (mst_genv -> npar)) {

    /* Check for overflow */
    if (mst_genv -> error |= cklimits(mst_genv -> npar, mst_genv -> par))
      goto error;

    /* Copy to dummypar */
    mst_gen_init |= copydblvec(mst_genv -> par, &mst_genv -> dummypar, mst_genv -> npar);for (i = 0; i < mst_genv -> npar; ++i);
    mst_gen_init |= copydblvec(mst_genv -> par, &mst_genv -> dummypar2, mst_genv -> npar);for (i = 0; i < mst_genv -> npar; ++i);

    /* at initialisation, in this part the parameter is not determined */
    mst_genv -> npar_cur = -1;

    /* get chisquare */
    chisquare = makenormalnumber((mst_genv -> gchsq)(mst_genv -> par, mst_genv -> adar));

    /* If this is the first call ever of the chisquare function, do this */
    if (!mst_genv -> allcalls) {
      mst_genv -> actchisq = chisquare;
      mst_genv -> actchisqred = mst_genv -> actchisq/(mst_genv -> indpoints - (double) mst_genv -> npar);
      mst_genv -> bestchisq = mst_genv -> actchisq;
      mst_genv -> bestchisqred = mst_genv -> actchisq/(mst_genv -> indpoints - (double) mst_genv -> npar);
      mst_gen_init |= copydblvec(mst_genv -> par, &mst_genv -> bestpar, mst_genv -> npar);
      mst_genv -> solchsq = mst_genv -> actchisq;
      mst_genv -> solchsqred = mst_genv -> actchisq/(mst_genv -> indpoints - (double) mst_genv -> npar);
      mst_gen_init |= copydblvec(mst_genv -> par, &mst_genv -> solpar, mst_genv -> npar);
      ++mst_genv -> allcalls;
    }

    /* Check for the existence of opar and ndpar, if they exist this has to go all the way */
    if ((mst_genv -> opar) && (mst_genv -> ndpar)) {
      
      /* if they exist, ndpar elements may not be 0, otherways this counts as wrong parameter */
      for (i = 0; i < mst_genv -> npar; ++i)
	mst_gen_init |= mst_genv -> ndpar[i] == 0?GFT_ERROR_WRONG_PARAM:GFT_ERROR_NONE;
      
      if (mst_gen_init & GFT_ERROR_WRONG_PARAM)
	;
      else {
	
	/* nopar has to be filled and thus to be created, this is a lazy variant of checking an allocation */
	if (!mst_genv -> nopar)
	  mst_gen_init |= copydblvec(mst_genv -> par, &mst_genv -> nopar, mst_genv -> npar);

	/* We got this far, so we can fill it */
	if ((mst_genv -> nopar)) {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> nopar[i] = (mst_genv -> par[i]-mst_genv -> opar[i])/mst_genv -> ndpar[i];
	}
      }
    }

    /* Now do the other things */
    mst_genv -> actchisq = chisquare;
    mst_genv -> actchisqred = mst_genv -> actchisq/(mst_genv -> indpoints - (double) mst_genv -> npar);

    if (mst_genv -> actchisq < mst_genv -> bestchisq) {
      mst_genv -> bestchisq = mst_genv -> actchisq;
      mst_genv -> bestchisqred = mst_genv -> actchisq/(mst_genv -> indpoints - (double) mst_genv -> npar);
      mst_gen_init |= copydblvec(mst_genv -> par, &mst_genv -> bestpar, mst_genv -> npar);
      if (mst_genv -> allcalls > 1)
	++mst_genv -> allcalls;
    }
  }
  else
    mst_gen_init |= GFT_ERROR_MISSING_INFO;

  return mst_gen_init;

 error:
  return GFT_ERROR_OVERFLOW;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* check whether output makes sense */

static int mst_gen_ckop(mst_gen *mst_genv, int spec)
{
  int mst_gen_ckop = GFT_ERROR_NONE;
  /***********/
  /***********/
/*   fprintf(stderr,"mst_gen_ckop %i %i\n", mst_gen_ckop, mst_genv -> error); */
  /***********/

  /* I think this is not necessary, but it might become */
  if (!mst_genv)
    return GFT_ERROR_NULL_PASSED;

  /* Check validity */
  if (spec < 0 || spec > GFT_OUTPUT_MAX)
    mst_gen_ckop |= GFT_ERROR_WRONG_IDENT;

  /* if there has been no iteration, this is not defined */
  if (!mst_genv -> allcalls) {
    switch (spec) {
    case GFT_OUTPUT_ACTCHISQ:
    case GFT_OUTPUT_ACTCHISQRED:
    case GFT_OUTPUT_BESTCHISQ:
    case GFT_OUTPUT_BESTCHISQRED:
    case GFT_OUTPUT_SOLPAR:
    case GFT_OUTPUT_SOLERR:
    case GFT_OUTPUT_SOLCHSQ:
    case GFT_OUTPUT_SOLCHSQRED:
      mst_gen_ckop |= GFT_ERROR_OBSOLETE_OP;
    default:
      ;
    }
  }

  /* if there has been one call, this is not defined */
  if (mst_genv -> allcalls == 1) {
    switch (spec) {
    case GFT_OUTPUT_SOLPAR:
    case GFT_OUTPUT_SOLERR:
    case GFT_OUTPUT_SOLCHSQ:
    case GFT_OUTPUT_SOLCHSQRED:
      mst_gen_ckop |= GFT_ERROR_OBSOLETE_OP;
    default:
      ;
    }
  }



  /* if there has been no finished minimisation run, this is not defined */
  if (!mst_genv -> minruns) {
    switch (spec) {
    case GFT_OUTPUT_SOLPAR:
    case GFT_OUTPUT_SOLERR:
    case GFT_OUTPUT_SOLCHSQ:
    case GFT_OUTPUT_SOLCHSQRED:
      mst_gen_ckop |= GFT_ERROR_OBSOLETE_OP;
    default:
      ;
    }
  }

  return mst_gen_ckop;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Refresh generic part of fit control struct */

static int mst_gen_refresh(mst_gen *mst_genv)
{
  size_t i;
  int mst_gen_refresh = GFT_ERROR_NONE;

  if (!mst_genv)
    return GFT_ERROR_NULL_PASSED;

  /* Check whether the number of parameters is > 0 */
  if (mst_genv -> npar > 0) {

    /* Check the number of independent data points and change */
    if (mst_genv -> indpoints - (double) mst_genv -> npar < 1.0)
      mst_genv -> indpoints = (double) mst_genv -> npar + 1.0;

    /* Create dummypar */
    if (!mst_genv -> dummypar) {
      if (!(mst_genv -> dummypar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
    }

    /* Create dummypar2 */
    if (!mst_genv -> dummypar2) {
      if (!(mst_genv -> dummypar2 = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
    }

    /* Mutually check upon the existence of first guess, working
       vector, and normalisations, create and fill */

    /* spar, opar, and par */
    if ((mst_genv -> spar)) {
      if (!mst_genv -> opar) {
	if (!(mst_genv -> opar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	  mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
	}
	else {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> opar[i] = mst_genv -> spar[i];
	}
      }
      if (!mst_genv -> par) {
	if (!(mst_genv -> par = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	  mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
	}
	else {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> par[i] = mst_genv -> spar[i];
	}
      }
    }
    if ((mst_genv -> opar)) {
      if (!mst_genv -> spar) {
	if (!(mst_genv -> spar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	  mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
	}
	else {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> spar[i] = mst_genv -> opar[i];
	}
      }
      if (!mst_genv -> par) {
	if (!(mst_genv -> par = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	  mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
	}
	else {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> par[i] = mst_genv -> opar[i];
	}
      }
    }
    if ((mst_genv -> par)) {
      if (!mst_genv -> spar) {
	if (!(mst_genv -> spar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	  mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
	}
	else {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> spar[i] = mst_genv -> par[i];
	}
      }
      if (!mst_genv -> opar) {
	if (!(mst_genv -> opar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	  mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
	}
	else {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> opar[i] = mst_genv -> par[i];
	}
      }
    }

    /* Now check the existence of one of these, they should all be there, or none */
    if (!mst_genv -> opar) {
      mst_genv -> misinf |= GFT_ERROR_MISSING_INFO;
    }

    /* dpar and ndpar */
    if ((mst_genv -> dpar)) {
      if (!mst_genv -> ndpar) {
	if (!(mst_genv -> ndpar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	  mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
	}
	else {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> ndpar[i] = mst_genv -> dpar[i];
	}
      }
    }

    if ((mst_genv -> ndpar)) {
      if (!mst_genv -> dpar) {
	if (!(mst_genv -> dpar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	  mst_gen_refresh |= GFT_ERROR_MEMORY_ALLOC;
	}
	else {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> dpar[i] = mst_genv -> ndpar[i];
	}
      }
    }


        /* Now check the existence of one of these, they should all be there, or none */
    if (!mst_genv -> dpar) {
      mst_gen_refresh |= mst_gen_refresh;
    }
    else {

      /* Check also if none of these is 0 */
      for (i = 0; i < mst_genv -> npar; ++i) {
	if (!mst_genv -> ndpar[i])
	  mst_genv -> misinf |= GFT_ERROR_WRONG_PARAM;
      }
    }

    /* At the beginning, allocate bestpar */
    if (!(mst_genv -> bestpar)) {
      if (!(mst_genv -> bestpar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
    }

    /* If the chisquare has been calculated once, we don't do this */
    if (!mst_genv -> allcalls) {
      if ((mst_genv -> spar)) {
	if (mst_genv -> bestpar) {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> bestpar[i] =  mst_genv -> spar[i];
	}
      }
    }

    /* At the beginning, allocate solpar */
    if (!(mst_genv -> solpar)) {
      if (!(mst_genv -> solpar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
    }

    /* If a solution has been found once, we don't do this */
    if (!mst_genv -> minruns) {
      if ((mst_genv -> spar)) {
	if (mst_genv -> solpar) {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> solpar[i] =  mst_genv -> spar[i];
	}
      }
    }

    /* At the beginning, allocate solerr */
    if (!(mst_genv -> solerr)) {
      if (!(mst_genv -> solerr = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
    }

    /* If a solution has been found once, we don't do this */
    if (!mst_genv -> minruns) {
      if ((mst_genv -> spar)) {
	if (mst_genv -> solerr) {
	  for (i = 0; i < mst_genv -> npar; ++i)
	    mst_genv -> solerr[i] =  0.0;
	}
      }
    }

    /* If not allocated, the normalised vector is 0 */
    if (!mst_genv -> nospar) {
      if (!(mst_genv -> nospar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
      else {
	for (i = 0; i < mst_genv -> npar; ++i)
	  mst_genv -> nospar[i] = 0.;
      }
    }

    /* If not allocated, allocate the normalised vector */
    if (!mst_genv -> nopar) {
      if (!(mst_genv -> nopar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
      else {
	for (i = 0; i < mst_genv -> npar; ++i)
	  mst_genv -> nopar[i] = mst_genv -> nospar[i];
      }
    }

    /* If not allocated, the normalised step width is 1 */
    if (!mst_genv -> nodpar) {
      if (!(mst_genv -> nodpar = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
      else {
	/* This will in any case be redone by the specific refresher */
	for (i = 0; i < mst_genv -> npar; ++i)
	  mst_genv -> nodpar[i] = 1.0;
      }
    }

    /* Upper and lower bounds are easy if not yet fixed */

    /* Upper bounds to be allocated */
    if (!mst_genv -> ubounds) {
      if (!(mst_genv -> ubounds = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
      else {
	for (i = 0; i < mst_genv -> npar; ++i)
	  mst_genv -> ubounds[i] = DBL_MAX;
      }
    }

    /* Lower bounds to be allocated */
    if (!mst_genv -> lbounds) {
      if (!(mst_genv -> lbounds = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
      else {
	for (i = 0; i < mst_genv -> npar; ++i)
	  mst_genv -> lbounds[i] = -DBL_MAX;
      }
    }

    /* Normalised upper bounds to be allocated */
    if (!mst_genv -> noubounds) {
      if (!(mst_genv -> noubounds = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
      else {
	/* This will in any case be redone by the specific refresher (or not) */
	for (i = 0; i < mst_genv -> npar; ++i)
	  mst_genv -> noubounds[i] = DBL_MAX;
      }
    }

    /* Normalised lower bounds to be allocated */
    if (!mst_genv -> nolbounds) {
      if (!(mst_genv -> nolbounds = (double *) malloc(mst_genv -> npar*sizeof(double)))) {
	mst_gen_refresh = mst_gen_refresh | GFT_ERROR_MEMORY_ALLOC;
      }
      else {
	/* This will in any case be redone by the specific refresher (or not) */
	for (i = 0; i < mst_genv -> npar; ++i)
	  mst_genv -> noubounds[i] = -DBL_MAX;
      }
    }
  }
  else
    mst_genv -> misinf |= GFT_ERROR_MISSING_INFO;

  /* Check the chisquare function */
  if (!mst_genv -> gchsq) {
    mst_genv -> misinf |= GFT_ERROR_MISSING_INFO;
  }

  return mst_gen_refresh;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check out (best) chisquares and parameter arrays */

static int mst_gen_ckch(mst_gen *mst_genv, double *array, double chisq)
{
  int mst_gen_ckch = GFT_ERROR_NONE;
  /************/
  /************/
/*   fprintf(stderr," mst_gen_ckch/error: %i %i\n", mst_gen_ckch, mst_genv -> error); */
  /************/

  /* Copy vectors */
  mst_gen_ckch |= copydblvec(array, &mst_genv -> nopar, mst_genv -> npar);
  mst_gen_ckch |= copydblvec(mst_genv -> dummypar, &mst_genv -> par, mst_genv -> npar);
  
  /* Copy chisquare */
  mst_genv -> actchisq = chisq;
  mst_genv -> actchisqred = mst_genv -> actchisq/(mst_genv -> indpoints - (double) mst_genv -> npar);

  /* compare with best chisquare */
  if (mst_genv -> actchisq < mst_genv -> bestchisq) {

    /* was better */
    mst_genv -> bestchisq = mst_genv -> actchisq;
    mst_genv -> bestchisqred = mst_genv -> actchisq/(mst_genv -> indpoints - (double) mst_genv -> npar);
    
    /* Change best chisquare parameters */
    mst_gen_ckch |= copydblvec(mst_genv -> par, &mst_genv -> bestpar, mst_genv -> npar);
  }
  
  /* This was an icrease in the fitting routines */
  ++mst_genv -> allcalls;
  ++mst_genv -> calls;
  ++mst_genv -> calls_st;

/* That's it */
  return mst_gen_ckch;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Generic constructor of the mst_spe struct */

static mst_spe *mst_spe_const(int method)
{
  mst_spe *mst_spe_const;

  switch (method) {
  case MET_NONE:
    mst_spe_const = NULL;
    break;
  case MET_SIMPLEX:
    mst_spe_const = (mst_spe *) mst_sim_const();
    break;
  case MET_GOLDEN:
    mst_spe_const = (mst_spe *) mst_gol_const();
    break;
  case MET_PSWARM:
    mst_spe_const = (mst_spe *) mst_psw_const();
    break;
  default:
    mst_spe_const = NULL;
  }

  return mst_spe_const;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* destroys a mst_spe * struct */
static int mst_spe_destr(mst_spe *spev, int method)
{
  switch(method) {
  case MET_NONE:
    if ((spev))
      return GFT_ERROR_MEMORY_LEAK;
    else
      break;
  case MET_SIMPLEX:
    return mst_sim_destr((mst_sim *) spev);
  case MET_GOLDEN:
    return mst_gol_destr((mst_gol *) spev);
  case MET_PSWARM:
    return mst_psw_destr((mst_psw *) spev);
  default:
    if ((spev))
      return GFT_ERROR_MEMORY_LEAK;
  }

  /* obsolete but for the compiler */
  return GFT_ERROR_NONE;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* check whether output makes sense */

static int mst_spe_ckop(mst_spe *mst_spev, int spec, int method)
{
  int mst_spe_ckop = GFT_ERROR_NONE;

  /* I think this is not necessary, but it might become */
  if ((method)) {
    if (!mst_spev) {
      return GFT_ERROR_NULL_PASSED;
    }
  }

  /* Check validity */
  if (spec < 0 || spec > GFT_OUTPUT_MAX)
    mst_spe_ckop |= GFT_ERROR_WRONG_IDENT;

  switch (method) {
  case GFT_MET_SIMPLEX:
    switch(spec){
      /* Only needed for GOLDEN */
    case GFT_OUTPUT_NCALLS_ST:
    case GFT_OUTPUT_NCALLS_ST_FAC:
      /* Only needed for PSWARM */
    case GFT_OUTPUT_UBOUNDS:
    case GFT_OUTPUT_LBOUNDS:
    case GFT_OUTPUT_SEED:
    case GFT_OUTPUT_PSNPART:
    case GFT_OUTPUT_PSCOGNI:
    case GFT_OUTPUT_PSSOCIA:
    case GFT_OUTPUT_PSMAXVF:
    case GFT_OUTPUT_PSNITFI:
    case GFT_OUTPUT_PSINIIN:
    case GFT_OUTPUT_PSFININ:
    case GFT_OUTPUT_PSINCDE:
    case GFT_OUTPUT_PSDECDE:
      mst_spe_ckop |= GFT_ERROR_NO_MEANING;
    default:
      ;
    }
    break;
  case GFT_MET_PSWARM:
    switch(spec){
      /* Only needed for GOLDEN */
    case GFT_OUTPUT_NCALLS_ST:
    case GFT_OUTPUT_NCALLS_ST_FAC:
      mst_spe_ckop |= GFT_ERROR_NO_MEANING;
    default:
      ;
    }
    break;
  case GFT_MET_GOLDEN:
    switch(spec){
      /* Only needed for PSWARM */
    case GFT_OUTPUT_UBOUNDS:
    case GFT_OUTPUT_LBOUNDS:
    case GFT_OUTPUT_SEED:
    case GFT_OUTPUT_PSNPART:
    case GFT_OUTPUT_PSCOGNI:
    case GFT_OUTPUT_PSSOCIA:
    case GFT_OUTPUT_PSMAXVF:
    case GFT_OUTPUT_PSNITFI:
    case GFT_OUTPUT_PSINIIN:
    case GFT_OUTPUT_PSFININ:
    case GFT_OUTPUT_PSINCDE:
    case GFT_OUTPUT_PSDECDE:
      mst_spe_ckop |= GFT_ERROR_NO_MEANING;
    default:
      ;
    }
    break;
  default:
    
    /* If the fit method is unknown, these are not clear */
    switch (spec) {
    case GFT_OUTPUT_NCALLS_ST:
    case GFT_OUTPUT_NITERS:
    case GFT_OUTPUT_NOPAR:
    case GFT_OUTPUT_NOSPAR:
    case GFT_OUTPUT_NODPAR:
    case GFT_OUTPUT_LOOPS:
      mst_spe_ckop |= GFT_ERROR_UNDEF_MEANING;
    default:
      ;
    }
  }

  return mst_spe_ckop;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check if a value passed makes sense */
static int ckmetinp(int method, int spec)
{
  int ckmetinp = GFT_ERROR_NONE;

  /* Check validity */
  if (spec < 0 || spec > GFT_INPUT_MAX)
    ckmetinp |= GFT_ERROR_WRONG_IDENT;
  
  if (mst_ckme(method))
    ckmetinp |= ckmetinp_undef(spec);
  
  switch(method) {
  case MET_SIMPLEX:
    ckmetinp |= cksiminp(spec);
    break;
  case MET_PSWARM:
    ckmetinp |= ckpswinp(spec);
    break;
  case MET_GOLDEN:
    ckmetinp |= ckgolinp(spec);
    break;
  default:
    ckmetinp |= ckmetinp_undef(spec);
  }

  return ckmetinp;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check if a value passed makes sense */
static int ckmetinp_undef(int spec)
{
  int ckmetinp_undef = GFT_ERROR_NONE;

  /* Check validity, probably redundant */
  if (spec < 0 || spec > GFT_INPUT_MAX)
    ckmetinp_undef |= GFT_ERROR_WRONG_IDENT;
  

  switch(spec) {
  case GFT_INPUT_METHOD:
  case GFT_INPUT_NPAR:
  case GFT_INPUT_SPAR:
  case GFT_INPUT_PAR:
  case GFT_INPUT_ADAR:
  case GFT_INPUT_NCALLS:
  case GFT_INPUT_NITERS:
  case GFT_INPUT_LOOPS:
  case GFT_INPUT_INDPOINTS:
    break;
  default:
    ckmetinp_undef |= GFT_ERROR_UNDEF_MEANING;
  }
  
  return ckmetinp_undef;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* allocate and initialise internal specific struct to mstr_in: simplex */

static mst_sim *mst_sim_const()

/* if the GSL is not there, this is a dummy function */
#if MET_SIMPLEX == -2
{
  return NULL;
}

#else
{
  mst_sim *mst_sim_const = NULL;

  if (!(mst_sim_const = (mst_sim *) malloc (sizeof(mst_sim))))
    goto error;

  /* Now initialise */
  mst_sim_const -> multimin_fminimizer_type_gsl = gsl_multimin_fminimizer_nmsimplex;
  mst_sim_const -> multimin_fminimizer_gsl = NULL;
  mst_sim_const -> stp_gsl_vec = NULL;
  mst_sim_const -> var_gsl_vec = NULL;
  mst_sim_const -> multimin_function_gsl = NULL;
/*   mst_sim_const -> var_vec = NULL; */
/*   mst_sim_const -> stp_vec = NULL; */

  /* Make empty gsl vectors */
/*   if (!(mst_sim_const -> stp_gsl_vec = empty_gsl_dbl_vector())) */
/*     goto error; */
/*   if (!(mst_sim_const -> var_gsl_vec = empty_gsl_dbl_vector())) */
/*     goto error; */

  /* Make empty function */
  if (!(mst_sim_const -> multimin_function_gsl = (gsl_multimin_function *) malloc(sizeof(gsl_multimin_function))))
    goto error;

  /* This is only used once, so no explicit function */
  mst_sim_const -> multimin_function_gsl -> f = NULL;
  mst_sim_const -> multimin_function_gsl -> n = 0;
  mst_sim_const -> multimin_function_gsl -> params = NULL;

  return mst_sim_const;

 error:
  mst_sim_destr(mst_sim_const);
  return NULL;
}
#endif

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* allocate and initialise internal specific struct to mstr_in: simplex */

static mst_psw *mst_psw_const()
{
  mst_psw *mst_psw_const = NULL;

  if (!(mst_psw_const = (mst_psw *) malloc (sizeof(mst_psw))))
    goto error;

  /* Now initialise */
  mst_psw_const -> optv = NULL;
  mst_psw_const -> swav = NULL;
  mst_psw_const -> curnospar = NULL;

  if (!(mst_psw_const -> optv = pswarm_options_const()))
    goto error;
  if (!(mst_psw_const -> swav = pswarm_swarm_const()))
    goto error;

  return mst_psw_const;

 error:
  mst_psw_destr(mst_psw_const);
  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* destroy a mst_sim * struct */

/* if the GSL is not there, this is a dummy function */
#if MET_SIMPLEX == -2
static int mst_sim_destr(mst_sim *mst_simv)
{
  return GFT_ERROR_NONE;
}

#else
static int mst_sim_destr(mst_sim *mst_simv)
{
  /* Check pointer */
  if (!(mst_simv))
    return GFT_ERROR_NULL_PASSED;

  /* Destroy allocations */
  if ((mst_simv -> stp_gsl_vec))
    gsl_vector_free (mst_simv -> stp_gsl_vec);
  if ((mst_simv -> var_gsl_vec))
    gsl_vector_free (mst_simv -> var_gsl_vec);
  FREE_COND(mst_simv -> multimin_function_gsl);
/*   FREE_COND(mst_simv -> var_vec); */
/*   FREE_COND(mst_simv -> stp_vec); */

  if ((mst_simv -> multimin_fminimizer_gsl))
    gsl_multimin_fminimizer_free(mst_simv -> multimin_fminimizer_gsl);

  /* Destroy the struct */
  free(mst_simv);

  return GFT_ERROR_NONE;
}
#endif

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* destroy a mst_psw * struct */
static int mst_psw_destr(mst_psw *mst_pswv)
{
  /* Check pointer */
  if (!(mst_pswv))
    return GFT_ERROR_NULL_PASSED;

  /* Destroy allocations */
  if ((mst_pswv -> optv))
    pswarm_options_destr(mst_pswv -> optv);
  if ((mst_pswv -> swav))
    pswarm_swarm_destr(mst_pswv -> swav);
  FREE_COND(mst_pswv -> curnospar);

  /* Destroy the struct */
  free(mst_pswv);

  return GFT_ERROR_NONE;
}


/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Do initialisations of specific minimiser that needs a call of the
   minimising function: simplex */
static int mst_sim_init(mst_sim *mst_simv, mst_gen *mst_genv)

/* if the method is not there, this is a dummy function */
#if MET_SIMPLEX == -2
{
  return GFT_ERROR_UNDEF_MEANING;
}

/* method is there */
#else
{
  int mst_sim_init = GFT_ERROR_NONE;
  if (gsl_multimin_fminimizer_set(mst_simv -> multimin_fminimizer_gsl, mst_simv -> multimin_function_gsl, mst_simv -> var_gsl_vec, mst_simv  -> stp_gsl_vec)) {
    mst_sim_init |= GFT_ERROR_INTRINSIC;
    mst_genv -> error |= GFT_ERROR_INTRINSIC;
  }

  mst_genv -> size = gsl_multimin_fminimizer_size(mst_simv -> multimin_fminimizer_gsl)*mst_simv -> vlnorm;

  mst_simv -> eqchisq = 0;
  mst_simv -> eqchisq2 = 0;
  mst_simv -> chisqbef = -1.0;
  mst_simv -> chisqbef2 = -1.0;

  /**********/
  /**********/
/*   fprintf(stderr,"size intern at init: %.2e\n", mst_genv -> size); */
  /**********/
  return mst_sim_init;
}
#endif

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Do initialisations of specific minimiser that does not need a call of the
   minimising function: pswarm (maybe call the function once) */
static int mst_psw_init(mst_psw *mst_pswv, mst_gen *mst_genv)
{

  /* int i; */
  int mst_psw_init = GFT_ERROR_NONE;
  
  mst_genv -> stopsize_act = mst_genv -> stopsize * pow(mst_genv -> stopsize_fac,mst_genv -> loop);
  
  /* mst_genv -> stopsize_act is tol */
  if (pswarm_options_init(mst_pswv -> optv, mst_genv -> npar, &gchsq_psw, (void *) mst_genv, mst_genv -> nolbounds, mst_genv -> noubounds, mst_pswv -> curnospar, mst_genv -> stopsize_act)) {

    /* npar is, I think > 0, if this function is called, so the only possibility is that there is a memory problem */
    mst_psw_init |= GFT_ERROR_MEMORY_ALLOC;
    mst_genv -> error |= GFT_ERROR_MEMORY_ALLOC;
  }
  pswarm_i_printfun(mst_pswv -> optv, &gft_pswarm_standardprint);

  /* Change the input to options */
  mst_pswv -> optv -> inputseed = mst_genv -> seed;
  mst_pswv -> optv -> s = mst_genv -> psnpart;
  mst_pswv -> optv -> mu = mst_genv -> pscogni;
  mst_pswv -> optv -> nu = mst_genv -> pssocia;
  mst_pswv -> optv -> maxvfactor = mst_genv -> psmaxvf;
  mst_pswv -> optv -> iterfweight = mst_genv -> psnitfi;
  mst_pswv -> optv -> iweight = mst_genv -> psiniin;
  mst_pswv -> optv -> fweight = mst_genv -> psfinin;
  mst_pswv -> optv -> idelta = mst_genv -> psincde;
  mst_pswv -> optv -> ddelta = mst_genv -> psdecde;

  if (pswarm_swarm_init(mst_pswv -> optv, mst_pswv -> swav)) {

    /* npar is, I think > 0, if this function is called, so the only possibility is that there is a memory problem */
    mst_psw_init |= GFT_ERROR_MEMORY_ALLOC;
    mst_genv -> error |= GFT_ERROR_MEMORY_ALLOC;
 }

  if (pswarm_init(mst_pswv -> optv, mst_pswv -> swav)) {

    /* npar is, I think > 0, if this function is called, so the only possibility is that there is a memory problem */
    mst_psw_init |= GFT_ERROR_MEMORY_ALLOC;
    mst_genv -> error |= GFT_ERROR_MEMORY_ALLOC;
  }

  /* some normalisation here? No. */
  mst_genv -> size = mst_pswv -> swav -> delta;

  /* What have we done? */
  /* printf("dim: %i\n", mst_pswv -> optv -> n); */
  /* printf("tol: %.1E\n", mst_pswv -> optv -> tol); */
  /* printf("ub: "); */
  /* for (i = 0; i < 6; ++i) */
  /*   printf("%.1f ", mst_pswv -> optv -> ub[i]); */
  /*   printf("\n"); */
  /*   printf("lb: "); */
  /* for (i = 0; i < 6; ++i) */
  /*   printf("%.1f ", mst_pswv -> optv -> lb[i]); */
  /*   printf("\n"); */
  /*   printf("fg: "); */
  /* for (i = 0; i < 6; ++i) */
  /*   printf("%.1f ", mst_pswv -> optv -> fg[i]); */
  /*   printf("\n"); */



  return mst_psw_init;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Do iteration step: simplex */
static int mst_sim_iter(mst_sim *mst_simv, mst_gen *mst_genv)

/* if the method is not there, this is a dummy function */
#if MET_SIMPLEX == -2
{
  return GFT_ERROR_UNDEF_MEANING;
}

/* method is there */
#else
{
  int mst_sim_iter = GFT_ERROR_NONE;
  int status;
  gsl_vector *agsl_vector;
  size_t i;

  /* Call the minimiser */
  status = gsl_multimin_fminimizer_iterate(mst_simv -> multimin_fminimizer_gsl);

  if (mst_simv -> chisqbef == mst_genv -> actchisq)
    ++mst_simv -> eqchisq;
  else
    mst_simv -> eqchisq = 0;

  if (mst_simv -> chisqbef2 == mst_genv -> actchisq)
    ++mst_simv -> eqchisq2;
  else
    mst_simv -> eqchisq2 = 0;

  mst_simv -> chisqbef2 = mst_simv -> chisqbef;
  mst_simv -> chisqbef = mst_genv -> actchisq;

  if ((status)) {
    /* Is it a consequence of a domain error that occured before? */
    if ((mst_sim_iter |= mst_genv -> error) & GFT_ERROR_OVERFLOW)
      errno = 0;
    else 
      mst_sim_iter |= mst_genv -> error |= GFT_ERROR_INTRINSIC;
  }

  /* If that occurred, we return, otherways we update and check */
  else {
    ++mst_genv -> iters;
    ++mst_genv -> alliter;
    mst_genv -> calls_st = 0;

    /* Get current solution and chisquare */

    /* This is a pointer to an intrinsic vector, not a copy, but we check ... */
    if (!(agsl_vector = gsl_multimin_fminimizer_x (mst_simv -> multimin_fminimizer_gsl))) {
      mst_genv -> error |= mst_sim_iter |= GFT_ERROR_MEMORY_ALLOC | GFT_ERROR_INTRINSIC;
    }
    else {
      for (i = 0; i < mst_genv -> npar; ++i) {
	mst_genv -> solpar[i] = gsl_vector_get(agsl_vector, i);
	mst_genv -> solpar[i] = mst_genv -> solpar[i] *mst_genv -> ndpar[i]+mst_genv -> opar[i]; 
/* before: ((double *) (agsl_vector -> data))[i]*mst_genv -> ndpar[i]+mst_genv -> opar[i]; */
      }

      /* Do not deallocate!!! */
/*       gsl_vector_free(agsl_vector); */

      /* Get chisquare and reduced chisquare */
      mst_genv -> solchsq = gsl_multimin_fminimizer_minimum(mst_simv -> multimin_fminimizer_gsl);
      mst_genv -> solchsqred = mst_genv -> solchsq/(mst_genv -> indpoints - (double) mst_genv -> npar);

      /* OK, now we check for the actual size and copy it, first calculate the actual stop size */
      mst_genv -> stopsize_act = mst_genv -> stopsize * pow(mst_genv -> stopsize_fac,mst_genv -> loop);
      mst_genv -> dsize = mst_genv -> size = gsl_multimin_fminimizer_size(mst_simv -> multimin_fminimizer_gsl)*mst_simv -> vlnorm;

  /**********/
  /**********/
/*       fprintf(stderr,"size intern: %.2e\n", mst_genv -> size); */
  /**********/

      /* Now check if we haven't reached the maximum number of iterations or calls */
      if (!(mst_genv -> iters >= mst_genv -> niters || mst_genv -> calls >= mst_genv -> ncalls)) {

	/* Then ceck size */
	if (mst_genv -> size  <= mst_genv -> stopsize_act) {
	  ++mst_genv -> loop;
	  ++mst_genv -> alloops;
	  mst_simv -> eqchisq = 0;
	  mst_simv -> eqchisq2 = 0;
	  
	  /* If we're finished we don't change a thing. If we're not finished yet, we actualise things */
	  if (mst_genv -> loop < mst_genv -> loops){
	    for (i = 0; i < mst_genv -> npar; ++i) {

	      
	      mst_simv ->  var_gsl_vec -> data[i] = (mst_genv -> solpar[i]-mst_genv -> opar[i])/mst_genv -> ndpar[i];
	      mst_simv ->  stp_gsl_vec -> data[i] =  pow(mst_genv -> dpar_fac,mst_genv -> loop)*mst_genv -> dpar[i]/mst_genv -> ndpar[i];
	    }

	    /* ERROR SOURCE??? This is a nasty thing, but it seems to be all we can do. I don't know if this is not a cause of memory leakage */
	    mst_sim_init(mst_simv, mst_genv);
	  }
	}

	/* Then check if we've run rigid */
	if (mst_simv -> eqchisq > (mst_genv -> npar+MET_SIMPLEX_MAXEQ) || mst_simv -> eqchisq2 > (mst_genv -> npar+MET_SIMPLEX_MAXEQ)) {
	  ++mst_genv -> loop;
	  ++mst_genv -> alloops;
	  mst_simv -> eqchisq = 0;
	  mst_simv -> eqchisq2 = 0;

	  /* If we're finished we don't change a thing. If we're not finished yet, we actualise things */
	  if (mst_genv -> loop < mst_genv -> loops){
	    for (i = 0; i < mst_genv -> npar; ++i) {

	      mst_simv ->  var_gsl_vec -> data[i] = (mst_genv -> solpar[i]-mst_genv -> opar[i])/mst_genv -> ndpar[i];
	      mst_simv ->  stp_gsl_vec -> data[i] =  pow(mst_genv -> dpar_fac,mst_genv -> loop)*mst_genv -> dpar[i]/mst_genv -> ndpar[i];
	    }

	    /* ERROR SOURCE??? This is a nasty thing, but it seems to be all we can do. I don't know if this is not a cause of memory leakage */
	    mst_sim_init(mst_simv, mst_genv);
	  }
	}
      }
    }
  }

  return mst_sim_iter;
}
#endif

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Do iteration step: simplex */
static int mst_psw_iter(mst_psw *mst_pswv, mst_gen *mst_genv)
{
  int mst_psw_iter = GFT_ERROR_NONE;
  int status;
  /* gsl_vector *agsl_vector; */
  size_t i;
  
  /* Some testing */
    /* if(!(mst_pswv -> swav -> iter % 10))  */
    /*   outfcn(mst_pswv -> optv, mst_pswv -> swav); */

  /* Call the minimiser */
  status = pswarm_iter(mst_pswv -> optv, mst_pswv -> swav);
  
  /* Check on error, this is assumed to be a real domain or range
     error that occurred */
  if ((status)) {
    /* Is it a consequence of a domain error that occured before? */
    if ((mst_psw_iter |= mst_genv -> error) & GFT_ERROR_OVERFLOW)
      errno = 0;
    else 
      mst_psw_iter |= mst_genv -> error |= GFT_ERROR_INTRINSIC;
  }
  
  /* If that occurred, we return, otherways we update and check */
  else {
    ++mst_genv -> iters; /* this is redundant with respect to  mst_pswv -> swav -> iter */
    ++mst_genv -> alliter;
    mst_genv -> calls_st = 0;
    
    /* Get current solution and chisquare */
    for (i = 0; i < mst_genv -> npar; ++i) {
      mst_genv -> solpar[i] = mst_pswv -> swav -> sol[i];
      mst_genv -> solpar[i] = mst_genv -> solpar[i] *mst_genv -> ndpar[i]+mst_genv -> opar[i]; 
    }
    
    /* Get chisquare and reduced chisquare */
    mst_genv -> solchsq = mst_pswv -> swav -> fy[mst_pswv -> swav -> gbest];
    mst_genv -> solchsqred = mst_genv -> solchsq/(mst_genv -> indpoints - (double) mst_genv -> npar);
    
    /* OK, now we check for the actual size and copy it, first calculate the actual stop size */
    mst_genv -> stopsize_act = mst_genv -> stopsize * pow(mst_genv -> stopsize_fac,mst_genv -> loop);
    mst_genv -> dsize = mst_genv -> size = mst_pswv -> swav -> delta;
    
    /**********/
    /**********/
    /*       fprintf(stderr,"size intern: %.2e\n", mst_genv -> size); */
    /**********/
    
    /* Now check if we haven't reached the maximum number of iterations or calls */
    if (!(mst_genv -> iters >= mst_genv -> niters || mst_genv -> calls >= mst_genv -> ncalls)) {

      /* Then ceck stopping conditions */
      if (pswarm_check_exit(mst_genv -> niters, mst_genv -> ncalls, mst_pswv -> optv, mst_pswv -> swav) != PSWARM_STATUS_OK) {

	/* Some more testing */
	/* printf("Some more testing blabla tolerance\n"); */
	/* outfcn(mst_pswv -> optv, mst_pswv -> swav); */

	++mst_genv -> loop;
	++mst_genv -> alloops;
	
	/* If we're finished we don't change a thing. If we're not finished yet, we actualise things */
	if (mst_genv -> loop < mst_genv -> loops){
	  for (i = 0; i < mst_genv -> npar; ++i) {
	    mst_pswv -> curnospar[i] = (mst_genv -> solpar[i]-mst_genv -> opar[i])/mst_genv -> ndpar[i];
	    /*   mst_simv ->  stp_gsl_vec -> data[i] =  pow(mst_genv -> dpar_fac,mst_genv -> loop)*mst_genv -> dpar[i]/mst_genv -> ndpar[i]; */
	  }
	  mst_psw_init(mst_pswv, mst_genv);
	}
      }
    }    
  }
    
  return mst_psw_iter;
}

  
  
  /* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check if a value passed makes sense */
/* if the method is not there, this is a dummy function */
#if MET_SIMPLEX == -2
static int cksiminp(int spec)
{
  return GFT_ERROR_UNDEF_MEANING;
}

#else
static int cksiminp(int spec)
{
  switch (spec) {
  case GFT_INPUT_NCALLS_ST:
    return GFT_ERROR_NO_MEANING;
  case GFT_INPUT_NCALLS_ST_FAC:
    return GFT_ERROR_NO_MEANING;
  default:
    ;
  }
  return GFT_ERROR_NONE;
}
#endif

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check if a value passed makes sense */
static int ckpswinp(int spec)
{
  switch (spec) {
  case GFT_INPUT_NCALLS_ST:
    return GFT_ERROR_NO_MEANING;
  case GFT_INPUT_NCALLS_ST_FAC:
    return GFT_ERROR_NO_MEANING;
  default:
    ;
  }
  return GFT_ERROR_NONE;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check if a value passed makes sense */
static int ckgolinp(int spec)
{
  switch (spec) {
  default:
    ;
  }
  return GFT_ERROR_NONE;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Allocate and initialise an empty gsl vector with double elements */

#ifdef GFT_GSL_PRESENT
/* static gsl_vector *empty_gsl_dbl_vector(void) */
/* { */
/*   gsl_vector *empty_gsl_dbl_vector; */

/*   if (!(empty_gsl_dbl_vector = (gsl_vector *) malloc(sizeof(gsl_vector)))) { */
/*     return NULL; */
/*   } */
/*   empty_gsl_dbl_vector -> size = 0; */
/*   empty_gsl_dbl_vector -> stride = sizeof(double); */
/*   empty_gsl_dbl_vector -> data = NULL; */
/*   empty_gsl_dbl_vector -> block = NULL; */
/*   empty_gsl_dbl_vector -> owner = 0; */

/*   return empty_gsl_dbl_vector;        */
/* } */
#endif

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Fill empty gsl vector */

#ifdef GFT_GSL_PRESENT
static int fill_gsl_dbl_vector(gsl_vector **gsl_vectorv, double *array, size_t length)
{
  int fill_gsl_dbl_vector = GFT_ERROR_NONE;
  
  if (!gsl_vectorv)
    return GFT_ERROR_NULL_PASSED;

  if (length == 0) {
    if ((array))
      return GFT_ERROR_WRONG_PARAM;
  }

  if (!array) {
    fill_gsl_dbl_vector |= GFT_ERROR_NULL_PASSED;
  }
  if (!*gsl_vectorv) {
    if (!(*gsl_vectorv = gsl_vector_alloc(length)))
      return GFT_ERROR_MEMORY_ALLOC;
  }

  while(length--)
    gsl_vector_set(*gsl_vectorv, length, array[length]);

  return fill_gsl_dbl_vector;
}
#endif

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Function for gsl_minimiser */

#ifdef GFT_GSL_PRESENT
static double gchsq_sim(const gsl_vector *nopar, void *mst_genv)
{
  size_t i;

  for (i = 0; i < ((mst_gen *) mst_genv) -> npar; ++i)
    ((mst_gen *) mst_genv) -> dummypar2[i] = gsl_vector_get(nopar, i);

  /* This is not dangerous, dummypar will be changed immediately after
     this call, but we avoid a direct access of the array from the
     gsl */

  return (((mst_gen *) mst_genv) -> gchsq_n)(((mst_gen *) mst_genv) -> dummypar2, (mst_gen *) mst_genv);
}
#endif

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Function for golden section minimiser */

static double gchsq_gol(double *nopar, void *mst_genv)
{
  size_t i;

  for (i = 0; i < ((mst_gen *) mst_genv) -> npar; ++i)
    ((mst_gen *) mst_genv) -> dummypar2[i] = nopar[i];

  /* This is not dangerous, dummypar will be changed immediately after
     this call, but we avoid a direct access of the array from the
     gsl */

/*   return (((mst_gen *) mst_genv) -> gchsq_n)(((mst_gen *) mst_genv) -> dummypar, (mst_gen *) mst_genv); */
  return (((mst_gen *) mst_genv) -> gchsq_n)(((mst_gen *) mst_genv) -> dummypar2, (mst_gen *) mst_genv);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Function for pswarm minimiser */

static double gchsq_psw(double *nopar, void *mst_genv)
{
  size_t i;

  for (i = 0; i < ((mst_gen *) mst_genv) -> npar; ++i)
    ((mst_gen *) mst_genv) -> dummypar2[i] = nopar[i];

  /* This is not dangerous, dummypar will be changed immediately after
     this call, but we avoid a direct access of the array from the
     gsl */

/*   return (((mst_gen *) mst_genv) -> gchsq_n)(((mst_gen *) mst_genv) -> dummypar, (mst_gen *) mst_genv); */
  return (((mst_gen *) mst_genv) -> gchsq_n)(((mst_gen *) mst_genv) -> dummypar2, (mst_gen *) mst_genv);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Normalised function */

static double gchsq_n(double *nopar, mst_gen *mst_genv)
{
  size_t i;
  double gchsq_n;
  /************/
  /************/
/*   fprintf(stderr,"gchsq_n value %.1e error: %i\n", gchsq_n, mst_genv -> error); */
  /************/

  /* check here the validity of all components and place a flag */
  if (mst_genv -> error |= cklimits(mst_genv -> npar, nopar))
    goto error;

  /************/
  /* First place the stuff in the dummy part */
  for (i = 0; i < mst_genv -> npar; ++i) {
  /************/
  /************/
/*      fprintf(stderr,"gchsq_n nopar %.1e \n", nopar[i]); */
  /************/

    /* this is copying dummypar to itself, hence the realmst_genv ->  nopar will be changed in ckch and not be identical to the input nopar */
    mst_genv -> dummypar[i] =  nopar[i]*mst_genv -> ndpar[i]+mst_genv -> opar[i];
  }


  /************/
  /************/
/*    fprintf(stderr,"\n"); */
  /************/

  /************/
  /************/
/*   for (i = 0; i < mst_genv -> npar; ++i) { */
/*     fprintf(stderr,"gchsq_n nopar0 par %i %.1e %.1e dummy %.1e\n", (int) i, nopar[i], mst_genv -> nopar[i], mst_genv -> dummypar[i]); */
/*   }  */
  /************/

/* check here the validity of all components and place a flag */
  if (mst_genv -> error |= cklimits(mst_genv -> npar, mst_genv -> dummypar))
    goto error;
  
  /************/
  /************/
/*   for (i = 0; i < mst_genv -> npar; ++i) { */
/*     fprintf(stderr,"gchsq_n nopar1 par %i %.1e %.1e dummy %.1e\n", (int) i, nopar[i], mst_genv -> nopar[i], mst_genv -> dummypar[i]); */
/*   }  */
  /************/

  /* Check out the chisquare */
  gchsq_n = makenormalnumber((*mst_genv -> gchsq)(mst_genv -> dummypar, mst_genv -> adar));

  /************/
  /************/
/*   for (i = 0; i < mst_genv -> npar; ++i) { */
/*     fprintf(stderr,"gchsq_n nopar2 par %i %.1e %.1e dummy %.1e\n", (int) i, nopar[i], mst_genv -> nopar[i], mst_genv -> dummypar[i]); */
/*   }  */
  /************/


  /* Fill the array and the struct */
  mst_gen_ckch(mst_genv, nopar, gchsq_n);
  /************/
  /************/
/*   for (i = 0; i < mst_genv -> npar; ++i) { */
/*     fprintf(stderr,"gchsq_n nopar3 %.1e %1e\n", nopar[i], mst_genv -> nopar[i]); */
/*   } */
  /************/

  return gchsq_n;
  
 error:
  errno = EDOM;
  return HUGE_VAL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* allocate and initialise internal specific struct to mstr_in: golden */

static mst_gol *mst_gol_const()
{
  mst_gol *mst_gol_const = NULL;

  if (!(mst_gol_const = (mst_gol *) malloc (sizeof(mst_gol))))
    goto error;
  mst_gol_const -> gc = NULL;
  if (!(mst_gol_const -> gc = golden_container_const()))
    goto error;

  return mst_gol_const;

 error:
  mst_gol_destr(mst_gol_const);
  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* destroy a mst_gol * struct */
static int mst_gol_destr(mst_gol *mst_golv)
{
  /* Check pointer */
  if (!(mst_golv))
    return GFT_ERROR_NULL_PASSED;

  golden_container_destr(mst_golv -> gc);

  /* Destroy the struct */
  free(mst_golv);

  return GFT_ERROR_NONE;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Do initialisations of specific minimiser that needs a call of the
   minimising function: golden */
static int mst_gol_init(mst_gol *mst_golv, mst_gen *mst_genv)
{
  int mst_gol_init = GFT_ERROR_NONE;
  double ncurstep_here;
  int npar_cur_here;


  if (!(mst_golv -> gc)) {
    mst_gol_init |= GFT_ERROR_INTRINSIC;
    goto error;
  }

/* Just to be sure, we do the following again */

  mst_gol_init |= golden_i_gchsq(&gchsq_gol, mst_golv -> gc);
  mst_gol_init |= golden_i_adar((void *) mst_genv, mst_golv -> gc);
  mst_gol_init |= golden_i_nodpar(mst_genv -> nodpar, mst_golv -> gc);
  mst_gol_init |= golden_i_nospar(mst_genv -> nospar, mst_golv -> gc);
  mst_gol_init |= golden_i_ncalls_st(mst_genv -> ncalls_st, mst_golv -> gc);
  mst_gol_init |= golden_i_minstep(1.0, mst_golv -> gc);

  /* At initialisation, the current parameter is -1 */
  mst_genv -> npar_cur = -1;

  if (golden_init(mst_golv -> gc)) {
    mst_gol_init |= GFT_ERROR_INTRINSIC;
    mst_genv -> error |= GFT_ERROR_INTRINSIC;
    goto error;
  }

  /* Size defined here, perhaps normalisation necessary */
   golden_o_solsize(&(mst_genv -> size),  mst_golv -> gc);
   golden_o_ncurstep(&ncurstep_here, mst_golv -> gc);
   golden_o_npar_cur(&npar_cur_here, mst_golv -> gc);
	/* mst_genv -> npar_cur = npar_cur_here; */

   /* npar_cur_here should be 0; ncurstep_here IS 0., so dsize becomes 0. It is a dummy that otherwise is kept synchronized with size */
   mst_genv -> dsize = ncurstep_here*mst_genv -> ndpar[npar_cur_here];
/*    mst_genv -> dsize = 0.0; */

  return mst_gol_init;

 error:
  return mst_gol_init;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Do iteration step: simplex */
static int mst_gol_iter(mst_gol *mst_golv, mst_gen *mst_genv)
{
  int mst_gol_iter = GFT_ERROR_NONE;
  int status;
  size_t i, iters_here, loop_here;
  int npar_cur_here;
  double nastep_here;
  
  golden_o_nastep(&nastep_here, mst_golv -> gc);
  golden_o_npar_cur(&npar_cur_here, mst_golv -> gc);

  mst_genv -> npar_cur = npar_cur_here;

  /* Get the step width */
  mst_genv -> dsize =  nastep_here*mst_genv -> ndpar[npar_cur_here];

  /* Call the minimiser */
  status = golden_iterate(mst_golv -> gc);
  
  /* Check out if the chisquare changed */
  
  /* Check on error, this is assumed to be a real domain or range
     error that occurred. Currently golden_iter does not return any
     information on that, so this is basically an empty peace of
     code */
  if ((status)) {
    /* Is it a consequence of a domain error that occured before? */
    if ((mst_gol_iter |= mst_genv -> error) & GFT_ERROR_OVERFLOW)
      errno = 0;
    else 
      mst_gol_iter |= mst_genv -> error |= GFT_ERROR_INTRINSIC;
  }
  
  /* If that occurred, we return, otherways we update and check */
  else {
    
    /* simply synchronise this */
    golden_o_calls_st(&(mst_genv -> calls_st), mst_golv -> gc);
    golden_o_iters(&iters_here, mst_golv -> gc);
    if (mst_genv -> iters != iters_here) {
      ++mst_genv -> alliter;
      ++mst_genv -> iters;
    }

    /* already done in gchsq_n:
       calls
       allcalls
       not yet done:
       npar_cur XXX
       gc -> actchisq (is identical with mst_genv -> bestchisq, but not with mst_genv -> actchisq
       gc -> par      (is identical with mst_genv -> bestpar-mst_genv -> opar[i])/mst_genv -> ndpar[i];, but not with mst_genv -> nopar
       gc -> nastep
       gc -> ncurstep XXX
       gc -> iterstat XXX
    */
    
    /* Get current solution and chisquare */
    
    golden_o_solpar(mst_genv -> solpar, mst_golv -> gc);
/*     copydblvec(mst_golv -> gc -> solpar, &mst_genv -> solpar, mst_genv -> npar); */

    for (i = 0; i < mst_genv -> npar; ++i) {
      mst_genv -> solpar[i] = mst_genv -> solpar[i] *mst_genv -> ndpar[i]+mst_genv -> opar[i]; 
    }
        
    /* Get chisquare and reduced chisquare */
    golden_o_solchisq(&(mst_genv -> solchsq), mst_golv -> gc);
    mst_genv -> solchsqred = mst_genv -> solchsq/(mst_genv -> indpoints - (double) mst_genv -> npar);
    
    /* OK, now we check for the actual size and copy it, first calculate the actual stop size */
    mst_genv -> stopsize_act = mst_genv -> stopsize * pow(mst_genv -> stopsize_fac,mst_genv -> loop);

    golden_o_solsize(&(mst_genv -> size), mst_golv -> gc);
    
    /**********/
    /**********/
    /*       fprintf(stderr,"size intern: %.2e\n", mst_genv -> size); */
    /**********/
    /* synchronise */
    golden_o_loop(&loop_here, mst_golv -> gc);
    golden_o_nastep(&nastep_here, mst_golv -> gc);

    if (mst_genv -> loop != loop_here) {
      ++mst_genv -> alloops;
      mst_genv -> loop = loop_here;
      if (mst_genv -> loop < mst_genv -> loops){

	for (i = 0; i < mst_genv -> npar; ++i) {
	  mst_genv -> dummypar2[i] =  pow(mst_genv -> dpar_fac,mst_genv -> loop)*mst_genv -> dpar[i]/mst_genv -> ndpar[i];
	}
	
	golden_i_nodpar(mst_genv -> dummypar2, mst_golv -> gc);
	golden_i_nastep(mst_genv -> dummypar2[0], mst_golv -> gc);
	/* the last command is equivalent with this */
/* 	mst_golv -> gc -> nastep = mst_golv -> gc -> nodpar[0]; */
      }
    }
    
    /* Now check if we haven't reached the maximum number of iterations or calls or loops */
    if (!(mst_genv -> iters >= mst_genv -> niters || mst_genv -> calls >= mst_genv -> ncalls || mst_genv -> loop >= mst_genv -> loops)) {
      
      /* Then check size */
      if (mst_genv -> size  <= mst_genv -> stopsize_act) {
	mst_genv -> loops = mst_genv -> loop;
      }
    }
  }
  

  return mst_gol_iter;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Normalised function, dummy, intended for later use */

/* static double gchsq_ndummy(double *nopar, mst_gen *mst_genv) */
/* { */
/*   size_t i; */
/*   double gchsq_ndummy; */

/*   /\* check here the validity of all components and place a flag *\/ */
/*   if (mst_genv -> error |= cklimits(mst_genv -> npar, nopar)) */
/*     goto error; */

/*   /\* First place the stuff in the dummy part *\/ */
/*   for (i = 0; i < mst_genv -> npar; ++i) */
/*     mst_genv -> dummypar[i] =  nopar[i]; */

/*   /\* Check out the chisquare *\/ */
/*   gchsq_ndummy = makenormalnumber((*mst_genv -> gchsq)(mst_genv -> dummypar, mst_genv -> adar)); */
  
/*   /\* Fill the array and the struct *\/ */
/*   mst_gen_ckch(mst_genv, nopar, gchsq_ndummy); */
  
/*   return gchsq_ndummy; */
  
/*  error: */
/*   errno = EDOM; */
/*   return HUGE_VAL; */
/* } */

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Copy a vector */

static int copydblvec(double *from, double **to, size_t length)
{
  int copydblvec;

  copydblvec = GFT_ERROR_NONE;

  if (!to)
    return GFT_ERROR_NULL_PASSED;

  if (!from) {
    copydblvec |= GFT_ERROR_NULL_PASSED;
    length = 0;
  }

  if ((*to)) {
    if (!length) {
      FLUSH_COND(*to);
    }
  }
  else {
    if ((length)){
      if (!(*to = (double *) malloc(length*sizeof(double))))
	return GFT_ERROR_MEMORY_ALLOC;
    }
  }


  while (length--) {
    (*to)[length] = from[length];
  }

  return copydblvec;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Copy a vector */

static int copyvec(void *from, void *to, size_t size, size_t length)
{
  char *internto;
  char *internfrom;

  if (!from)
    return GFT_ERROR_NO_MEANING;
  if (!to)
    return GFT_ERROR_NULL_PASSED;

  /* ERROR SOURCE??? Here, expressively a char is the smallest
     element, this is standard */
  length = length*size;

  internto = (char *) to;
  internfrom = (char *) from;

  while (length--)
    internto[length] = internfrom[length];

  return GFT_ERROR_NONE;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check numerical limits */

static int cklimits(size_t length, double *array)
{
  int cklimits = GFT_ERROR_NONE;

  if (length < 1)
    return GFT_ERROR_NONE;

  if (!array)
    return GFT_ERROR_NULL_PASSED;

  /* -DBL_MAX is defined at least in C99, if it's in C90, I couldn't find out */
  while (length--)
    cklimits |= array[length] < -DBL_MAX?GFT_ERROR_OVERFLOW:(array[length] > DBL_MAX?GFT_ERROR_OVERFLOW:GFT_ERROR_NONE);

  return cklimits;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check for overflow, in case of overflow, replace by largest double */

static double makenormalnumber(double number)
{
  /* If it is possible to somehow find out if it is a big number, we
     delete the error status */
  if (number < -DBL_MAX || number > DBL_MAX)
    errno = 0;

  return number < -DBL_MAX?-DBL_MAX:(number > DBL_MAX?DBL_MAX:number);
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* input to pswarm: function to print only if the status is an error, but then to stderr */
static int gft_pswarm_standardprint(pswarm_swarm *pswarm_swarmv)
{
  if (!(pswarm_swarmv))
    return 1;

  if ((pswarm_swarmv -> status)) {
    if (pswarm_swarmv -> status & PSWARM_STATUS_ERROR)
      fprintf(stderr, "%s\n", pswarm_swarmv -> statusm);
  }
  return 0;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* input to pswarm: function to print only if the status is an error, but then to stderr */
double gft_psw_startsize(int n, double *lb, double *ub, double tol, double fdelta) {
  int j;
  double delta, mindelta;

  if ((n <= 0) || !(lb) || !(ub) || tol <= 0.0 || fdelta <= 0.0)
    return DBL_MAX;
  
  mindelta = DBL_MAX;
  for(j = 0; j < n ; j++){
    if(lb[j] > -DBL_MAX && ub[j] < DBL_MAX) {
      if(mindelta > (ub[j]-lb[j]))
	mindelta = (ub[j]-lb[j]);
    } 
  }
  if(mindelta >= DBL_MAX || mindelta<2*sqrt(tol))
    delta=2*sqrt(sqrt(tol));
  else delta=mindelta/fdelta;
  return delta;
}

/* ------------------------------------------------------------ */


/* Output function, only for testing */
/* static int outfcn(pswarm_options *opt, pswarm_swarm *pop) */
/* { */
/*   if(pop -> iter==0){ */
/*     printf("\n  Iter     Leader     Objective  " */
/* 	    "\n  -------------------------------\n" */
/* 	    "    %4d   %4d   %4.6e   %4.6e\n", pop -> iter, pop -> gbest, pop -> fy[pop -> gbest], pop -> delta); */
/*   } */
/*   else */
/*     printf("    %4d   %4d   %4.6e   %4.6e\n", pop -> iter, pop -> gbest, pop -> fy[pop -> gbest], pop -> delta); */
  
/*   return 0; */
/* } */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: gft.c,v $
   Revision 1.5  2011/05/04 01:13:59  jozsa
   did a lot

   Revision 1.4  2007/12/12 17:19:21  gjozsa
   checked something

   Revision 1.3  2007/08/14 17:10:51  gjozsa
   Added some counters

   Revision 1.2  2007/07/02 12:55:12  gjozsa
   Made ANSI compliant

   Revision 1.1  2007/06/22 12:43:28  gjozsa
   Added to CVS control


   ------------------------------------------------------------ */

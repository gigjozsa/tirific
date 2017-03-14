/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file gft.h
   @brief Generic minimisation wrapper

   GENERAL AIM
   This is a wrapper for multidimensional minimisation algrorithms or
   packages of multidimensional minimisers. The intention is to
   provide something with which it is possible to change a
   minimisation strategy quickly. The main purpose at the time this
   was written was to construct a wrapper for a chisquared
   minimisation of a function, the arithmetical form of which is
   unknown. Therefore, the module contains only derivative-free
   algorithms.

   GENERIC STRUCTURE OF THE MODULE

   The structure of the module is as strightforward as possible while
   maintaining two principles, which are to provide the user with as
   much information as possible and to encapsulate the fitting process
   as effectively as possible.

   Memory allocations are hidden and objects should be addressed only
   with the external functions provided in this module. The functions
   provide an output error code accessible through the symbolic
   constants GFT_ERRROR_* . In the context of memory allocation, if
   problems occur, the functions gft* will return an error logically
   ored with GFT_ERROR_MEMORY_ALLOC (errors are stored also in the
   internal acquisition struct and can be accessed by the output
   function, see below).

   The module provides one hidded object, the acquisition or control
   object gft_mst, which internally is used to access all information
   needed. Again, internally, the hidden object will also address
   dynamially allocated structures. This is why one should never
   forget to deallocate the acquisition object with the appropriate
   function provided here: Dynamically allocated memory associated
   with the control object can be deallocated properly only with the
   local deallocation function gft_mst_destr().

   Externally, the acquisition object can (should) only be created by
   the function gft_mst_const(), which will return an aliased void
   pointer to the acquisition structure: To distinguish this void
   pointer from other void pointers, the alias gft_mst is provided. A
   complete deallocation of all allocated memory can be done calling
   gft_mst_destr(). Input and output of information is done via two
   functions, gft_mst_put() (input), and gft_mst_get() (output), which
   take the gft_mst pointer that contains the necessary information as
   a first input (see description). Actions, such as the start or
   interrupt of a minimising process, are done with the last function
   in this module, gft_mst_act(). Except for the case of the fitting
   function, and the container of additional information, it is only
   possible to get a copy of the information contained in the control
   struct, such that an external deallocation of objects that contain
   information that is output from the control object, is safe. Also,
   input to the control object will be copied locally (except for the
   minimising function and external additional parameters to that
   function). Therefore, an array that has been passed to the control
   structure via gft_mst_put(), can safely be changed afterwards.
   
   PRAGMATICALLY
   Usually the user will allocate the control struct
   with gft_mst_const(), provide the necessary information with
   gft_mst_put(), start a fitting process with gft_mst_act(), read out
   the information acquired (e.g. coordinates and value of the
   minimum), possibly also during the fitting process, with
   gft_mst_get(), in order to finally free allocated memory with
   gft_mst_destr().

   ALGORITHM 

   Basically, the user provides i) the dimension npar of the problem,
   ii) a function to minimise (minimising function), which is a
   function of a double array of length n and a generic container of
   additional information, iii) a first guess double array of size n,
   iv) a start stepwidth double array of length n. In order to define
   a stopping condition, the user can give a characteristic minimal
   size (see below, and single algorithm descriptions), which is
   positive number (default 0.0), a maximum number of iterations
   (many), a maximum number of function calls (many).

   In this basic mode, a start of the algorithm will run through a
   minimisation once, repeating minimising steps until the stopping
   condition is reached. The (current) solution can be accessed
   through the output function. The way an algorithm works is defined
   in the single descriptions of the modules included into this one
   (see references). Also, the definition of an iteration step, the
   basic unit of an iteration, depends on the algorithm used. Within
   one step, a number of function calls are possible. After one step,
   the function can be interrupted. Whether it is possible to
   interrupt the minimisation within an iteration depends on the
   algorithm used. The user will receive an appropriate error message
   if she/he provides or intends to read information that has no
   meaning in the context of the minimiser specified. Currently, the
   only parameter, with which the character of a single iteration can
   virtually be influenced, is the number of function calls per
   iteration, which is currently superfluous. If the maximum number of
   function calls is encountered within one iteration, the iteration
   will be finished, calling the function furtherly, and afterwards a
   normal exit is initiated.

   NORMALISATION
   
   Unless explicitely stated, the implemented algorithms will not
   operate on the function passed, but on a normalised function in the
   sense that the user defines the origin and the dimensioning of the
   passed function.
   
   The user can define the origin to be 
   o1, o2, ...

   and the grid size to be 
   d1, d2, ...  

   Then, a function call from the minimiser at the position 
   x1, x2, ...

   will call the original, passed function with
   d1*x1+o1, d2*x1+o2, ...

   The sense here is that for realistic problems, a minimum is defined
   with differing accuracy in the variables, while the size of a
   problem is usually calculated under the assumption that all
   variables have the same weight. In that case, the minimal size will
   be dominated by the parameter with the largest variance.  This
   behaviour is prevented with the normalisation. The default for the
   origin is the first guess, the default for the internal grid widths
   is the input step widths, such that this behaviour will be hidden
   for the uninterested user.

   The size of the problem is normalised such that it is 1.0 at the
   start of the fitting process, provided, the grid origin and the
   grid size is identical to the first guess and the start step
   widths, respectively (which is the sensible default).

   A hidden error is the provision of a grid normalisation with a zero
   length in one variable, which is not allowed. This can also happen
   if the user provides starting step widths that contain a zero
   (which doesn't make sense at all, but who knows) without providing
   a valid grid normalisation.

   LOOPS

   The default state is to run one loop of iterations, which should
   usually be finished when the size of the problem has reached the
   minimal size provided by the user. The user has the possibility to
   increase the number of loops. After finishing one loop, a second
   iteration process is started, taking the solution of the first
   iteration process as a first guess and redefining the start step
   widths for the current loop. From loop to loop, the start step
   widths can be scaled by a factor provided by the user. Also, the
   minimal size, and the number of function calls per iteration can be
   scaled alike. For each loop, these quantities will be multiplied
   with the given double value (and in case of the number of function
   calls, converted into an integer value).

   For the golden section algorithm, the size is re-evaluated at the
   end of a loop. For this algorithm it makes hence no sense to choose
   less than two loops. If the size meets the stopping condition, the
   loop number (parameter loops) is set to the current number of
   finished loops. This has to be taken into account when using this
   algorithm.


   SOLUTIONS AND CURRENT VALUES

   A solution will be provided and can be accessed via gft_mst_get(),
   once the algorithm has finished one loop, while the information
   about the best fit reached until now, can be accessed after the
   firt call of the function. A trial to read these values will
   produce an output, but also an appropriate error message. Moreover,
   at any point in the fitting process, the last vector (and its
   normalised value), that was put into the minimising function, and
   the value reached can be accessed. If one accesses these outputs
   during a minimisation process by asking them in a function call, it
   should be noticed that these are always the function values from
   the last, and not the current call.

   STOPPING AND RUNNING
   The minimisation can be stopped after an iteration. A second start
   will then result in a continued fitting process. If the user wants
   to run the minimisation step-by-step, she/he has to stop the
   iteration within every call of the minimising function, in order to
   start it again.

   SOLUTION FOUND

   When a solution has been found or an error occured, this fact can
   be accessed asking for the value of the busy switch. If 0, either
   an error has occured, or the fitting exited normally. An error is
   indicated by a non-zero error variable. The number of finished
   loops can be accessed asking for the loop variable. If busy =
   GFT_ERROR_NONE, error = GFT_ERROR_NONE, and loop < loops (number of
   loops provided by the user), the maximum number of iterations or
   the maximum number of function calls has been reached.

   ERRORS
   
   The module checks for numerical errors and memory allocation errors
   that might occur during the fitting process (it might be necessary
   to allocate memory during a fitting process, depending on the
   implemented modules, although it has been tried to avoid this). If
   such an error is encountered, subsequent function calls in an
   iteration will not change the content of the minimiser
   afterwards. The minimisation will be stopped and can be restarted
   only if the error flag is removed by the user.

   Overflows and domain errors: The module assumes the domain of
   arguments to the function to be minimised to be
   [-DBL_MAX,DBL_MAX]. It will be checked if the minimising method
   asks for something outside this domain. This will interrupt the
   fitting process. The basic assumption is that with the compiler
   version used DBL_MAX and -DBL_MAX are defined, that -DBL_MAX <
   DBL_MAX, and that linear operations that cause a domain error will
   preserve the < > relations, i.e., DBL_MAX*2.0 > DBL_MAX. The
   function value will be corrected to either DBL_MAX or -DBL_MAX
   under the assumption that a range error occuring in a call of the
   minimising function will produce a value that is either > DBL_MAX
   or < -DBL_MAX. For the gcc, it seems that e.g. HUGE_VAL > DBL_MAX
   and -HUGE_VAL < DBL_MAX. No check for domain errors in a call of
   the fitting function will be made.

   EXPLICITE USAGE AND DETAILS

   Detailled information is contained in the function descriptions,
   and example codes are provided.

   $Source: /Volumes/DATA_J_II/data/CVS/gft/gft.h,v $
   $Date: 2011/05/04 01:13:59 $
   $Revision: 1.5 $
   $Author: jozsa $
   $Log: gft.h,v $
   Revision 1.5  2011/05/04 01:13:59  jozsa
   did a lot

   Revision 1.4  2007/12/12 17:19:21  gjozsa
   checked something

   Revision 1.3  2007/08/14 17:10:51  gjozsa
   Added some counters

   Revision 1.2  2007/07/02 12:55:12  gjozsa
   Made ANSI compliant

   Revision 1.1  2007/06/22 12:43:29  gjozsa
   Added to CVS control

   
*/
/* ------------------------------------------------------------ */



/* Include guard */
#ifndef GFT_H
#define GFT_H


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
   @def GFT_MET_GOLDEN
   @brief golden section algorithm alias
   
   Alias for golden section algorithm for use in gft_init()
*/
/* ------------------------------------------------------------ */
#define GFT_MET_GOLDEN 1



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_MET_SIMPLEX
   @brief simplex algorithm alias
   
   Alias for simplex algorithm for use in gft_init()
*/
/* ------------------------------------------------------------ */
#define GFT_MET_SIMPLEX 2



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_MET_PSWARM
   @brief pswarm algorithm alias
   
   Alias for pswarm algorithm for use in gft_init()
*/
/* ------------------------------------------------------------ */
#define GFT_MET_PSWARM 3



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_ERROR_NULL_PASSED
   @brief Error codes in this module

   Binary incremental error list. More than one error can
   occur. Access through return_value & GFT_ERROR_CODE
   
   GFT_ERROR_NULL_PASSED:   Null was passed instead of valid pointer
   GFT_ERROR_OBSOLETE_OP:   The function call took no effect
   GFT_ERROR_MEMORY_ALLOC:  Memory allocation problems
   GFT_ERROR_MEMORY_LEAK:   Possible memory leak
   GFT_ERROR_BUSY:          Action forbidden because of interference 
                            with another process
   GFT_ERROR_WRONG_IDENT:   An unknown integer identifyer was passed
   GFT_ERROR_MISSING_INFO:  A check for a properly initialised struct failed
   GFT_ERROR_WRONG_PARAM:   A parameter passed was wrong
   GFT_ERROR_NO_MEANING:    A parameter passed or read has no meaning
   GFT_ERROR_UNDEF_MEANING: It is not clear whether a paramer passed 
                            or read has a meaning
*/
/* ------------------------------------------------------------ */
#define GFT_ERROR_NONE              0
#define GFT_ERROR_STANDARD          1
#define GFT_ERROR_NULL_PASSED       2
#define GFT_ERROR_OBSOLETE_OP       4
#define GFT_ERROR_MEMORY_ALLOC      8
#define GFT_ERROR_MEMORY_LEAK      16
#define GFT_ERROR_BUSY             32
#define GFT_ERROR_WRONG_IDENT      64
#define GFT_ERROR_MISSING_INFO    128
#define GFT_ERROR_WRONG_PARAM     256
#define GFT_ERROR_NO_MEANING      512
#define GFT_ERROR_UNDEF_MEANING  1024
#define GFT_ERROR_OVERFLOW       2048
#define GFT_ERROR_UNDERFLOW      4196
#define GFT_ERROR_ERROR_PRESENT  8392
#define GFT_ERROR_INTRINSIC     16784


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_INPUT_
   @brief Input types to supply the minimiser with information
   
   GFT_INPUT_FIT_MET:  Fitting method as specified by GFT_MET
   GFT_INPUT_N_PARAM:  Number of parameters
*/
/* ------------------------------------------------------------ */
#define GFT_INPUT_METHOD           0
#define GFT_INPUT_NPAR             1
#define GFT_INPUT_SPAR             2
#define GFT_INPUT_UBOUNDS          3
#define GFT_INPUT_LBOUNDS          4  
#define GFT_INPUT_PAR              5
#define GFT_INPUT_OPAR             6
#define GFT_INPUT_DPAR             7
#define GFT_INPUT_NDPAR            8
#define GFT_INPUT_ADAR             9
#define GFT_INPUT_NCALLS          10
#define GFT_INPUT_NITERS          11
#define GFT_INPUT_NCALLS_ST       12
#define GFT_INPUT_STOPSIZE        13
#define GFT_INPUT_LOOPS           14
#define GFT_INPUT_NCALLS_ST_FAC   15
#define GFT_INPUT_DPAR_FAC        16
#define GFT_INPUT_STOPSIZE_FAC    17
#define GFT_INPUT_INDPOINTS       18
#define GFT_INPUT_SEED            19

#define GFT_INPUT_PSNPART         20 /* pswarm number of particles */
#define GFT_INPUT_PSCOGNI         21 /* pswarm cognitional parameter */
#define GFT_INPUT_PSSOCIA         22 /* pswarm social parameter */
#define GFT_INPUT_PSMAXVF         23 /* pswarm maximum velocity factor */
#define GFT_INPUT_PSNITFI         24 /* pswarm number of iterations to final weight */
#define GFT_INPUT_PSINIIN         25 /* pswarm initial weight */
#define GFT_INPUT_PSFININ         26 /* pswarm final weight */
#define GFT_INPUT_PSINCDE         27 /* pswarm increase mesh delta by this factor */
#define GFT_INPUT_PSDECDE         28 /* pswarm decrease mesh delta by this factor */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_INPUTF_
   @brief Input types to supply the minimiser with functions
   
   See function gft_mst_putf
*/
/* ------------------------------------------------------------ */
#define GFT_INPUT_GCHSQ            1
#define GFT_INPUT_GCHSQ_REP        2

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_OUTPUT_METHOD
   @brief Output types to get information from the minimiser
   
   See function gft_mst_get
*/
/* ------------------------------------------------------------ */
#define GFT_OUTPUT_MISINF        1
#define GFT_OUTPUT_BUSY          2
#define GFT_OUTPUT_STOPPED       3
#define GFT_OUTPUT_ERROR         4
#define GFT_OUTPUT_ALLCALLS      5
#define GFT_OUTPUT_MINRUNS       6
#define GFT_OUTPUT_METHOD        7
#define GFT_OUTPUT_NPAR          8
#define GFT_OUTPUT_INDPOINTS     9
#define GFT_OUTPUT_ACTCHISQ      10
#define GFT_OUTPUT_ACTCHISQRED   11
#define GFT_OUTPUT_BESTCHISQ     12
#define GFT_OUTPUT_BESTCHISQRED  13
#define GFT_OUTPUT_PAR           14
#define GFT_OUTPUT_BESTPAR       15
#define GFT_OUTPUT_SOLPAR        16
#define GFT_OUTPUT_SOLERR        17
#define GFT_OUTPUT_SOLCHSQ       18
#define GFT_OUTPUT_SOLCHSQRED    19
#define GFT_OUTPUT_SPAR          20
#define GFT_OUTPUT_UBOUNDS       21
#define GFT_OUTPUT_LBOUNDS       22  
#define GFT_OUTPUT_OPAR          23
#define GFT_OUTPUT_DPAR          24
#define GFT_OUTPUT_NDPAR         25
#define GFT_OUTPUT_ADAR          26
#define GFT_OUTPUT_NCALLS        27
#define GFT_OUTPUT_CALLS         28
#define GFT_OUTPUT_NITERS        29
#define GFT_OUTPUT_ITERS         30
#define GFT_OUTPUT_NCALLS_ST     31
#define GFT_OUTPUT_CALLS_ST      32
#define GFT_OUTPUT_STOPSIZE      33
#define GFT_OUTPUT_STOPSIZE_ACT  34
#define GFT_OUTPUT_SIZE          35
#define GFT_OUTPUT_LOOPS         36
#define GFT_OUTPUT_LOOP          37
#define GFT_OUTPUT_NCALLS_ST_FAC 38
#define GFT_OUTPUT_DPAR_FAC      39
#define GFT_OUTPUT_STOPSIZE_FAC  40
#define GFT_OUTPUT_NOPAR         41
#define GFT_OUTPUT_NOSPAR        42
#define GFT_OUTPUT_NOUBOUNDS     43
#define GFT_OUTPUT_NOLBOUNDS     44
#define GFT_OUTPUT_NODPAR        45
#define GFT_OUTPUT_ALLOOPS       46
#define GFT_OUTPUT_ALLITER       47
#define GFT_OUTPUT_DSIZE         48
#define GFT_OUTPUT_NPAR_CUR      49
#define GFT_OUTPUT_SEED          50
#define GFT_OUTPUT_PSNPART        51 /* pswarm number of particles */
#define GFT_OUTPUT_PSCOGNI        52 /* pswarm cognitional parameter */
#define GFT_OUTPUT_PSSOCIA        53 /* pswarm social parameter */
#define GFT_OUTPUT_PSMAXVF        54 /* pswarm maximum velocity factor */
#define GFT_OUTPUT_PSNITFI        55 /* pswarm number of iterations to final weight */
#define GFT_OUTPUT_PSINIIN        56 /* pswarm initial weight */
#define GFT_OUTPUT_PSFININ        57 /* pswarm final weight */
#define GFT_OUTPUT_PSINCDE        58 /* pswarm increase mesh delta by this factor */
#define GFT_OUTPUT_PSDECDE        59 /* pswarm decrease mesh delta by this factor */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def GFT_ACT_START
   @brief Output types to get information from the minimiser
   
   See function gft_mst_act
*/
/* ------------------------------------------------------------ */
#define GFT_ACT_START 1
#define GFT_ACT_STOP  2
#define GFT_ACT_BREAK 3
#define GFT_ACT_INIT  4
#define GFT_ACT_FLUSH 5
#define GFT_ACT_CLERR 6


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* MACROS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* TYPEDEFS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @typedef gft_mst
   @brief Top layer to the internal structs

*/
/* ------------------------------------------------------------ */
typedef void gft_mst;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* STRUCTS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn gft_mst *gft_mst_const()
  @brief Get top layer to fitting control object

  The function initialises the top layer structure. This is the first
  call one has to make in order to initiate a fitting process.

  @param void

  @return (success) gft_mst *gft_mst_const   pointer to struct
          (error)                            standard
*/
/* ------------------------------------------------------------ */
gft_mst *gft_mst_const();



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int gft_mst_destr(gft_mst *gft_mstv)
  @brief destroys gft_mst * struct

  Will deallocate mstv. Will deallocate everything connected to mstv.
  Will report on untidy deallocation (memory leak).

  @param mstv   (gft_mst *)  Pointer to struct to deallocate

  @return (success) int gft_init: 0
          (error)                 standard
*/
/* ------------------------------------------------------------ */
int gft_mst_destr(gft_mst *gft_mstv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int gft_mst_put(gft_mst *gft_mstv, void *input, int spec)
  @brief Generically supply information to a minimiser struct

  Generic input interface to the minimiser struct. The type of input
  and its meaning is specified by spec. Arrays passed must have at
  least the size specified given by GFT_INPUT_NPARAM. If
  GFT_INPUT_NPARAM is 0 or unspecified, nothing will be read in. The
  program reads in local copies, except in the case of the additional
  arguments to this function.

  constant expression     input type                   description
  GFT_INPUT_METHOD        single int *                 Fit method as defined by GFT_MET_*
  GFT_INPUT_NPAR        single size_t *                 Number of parameters, 0 allowed. This action deletes all array information, which must be read in again. 
  GFT_INPUT_PAR           array double *               Actual parameters. Defaults to grid origin if not specified
  GFT_INPUT_OPAR          array double *               Grid origin. Defaults to start parameters if not specified.
  GFT_INPUT_DPAR          array double *               Start stepwidths, defaults to grid normalisation, if not specified. 0 as a member is allowed, but makes no sense. Essential for golden, sets the start simplex definition for sim (otherwise this defaults to the grid). Not interesting for psw.
  GFT_INPUT_NDPAR         array double *               Grid normalisation, defaults to start sptepwidth, if not specified. 0 as a member is not allowed. Is the relevant normalisation.
  GFT_INPUT_ADAR          void *                       Additional arguments to the function to be minimised
  GFT_INPUT_NCALLS        single size_t *              Maximum number of function calls. If in the fitting process, this number is reached, last necessary calls in the actual iteration step will be made, but then the fitting is stopped. 
  GFT_INPUT_NITERS        single size_t *              Number of iteration steps. Each iteration step might call the function to be minimised more than once or once, depending on the method.
  GFT_INPUT_NCALLS_ST     single size_t *              Maximum number of function calls within one iteration step.
  GFT_INPUT_STOPSIZE      single double *              Stop size of characteristic length
  GFT_INPUT_LOOPS         single size_t *              Number of loops, this has to be put into the function again after a golden section run.
  GFT_INPUT_NCALLS_ST_FAC single double *              After each loop the maximum number of fuction calls within one iteration step will be multiplied with this number. Positive.
  GFT_INPUT_DPAR_FAC      single double *              After each loop the start stepwidths will be multiplied with this number. Must be positive.
  GFT_INPUT_STOPSIZE_FAC  single double *              After each loop the stopping size of the characteristic length will be multiplied with this number. Must be positive.
  GFT_INPUT_INDPONTS      single double *              Number of independent datapoints. Must be greater than the number of parameters plus 1.

  GFT_INPUT_SPAR          array double *               Start parameters. Defaults to grid origin if not specified
  GFT_INPUT_UBOUNDS       array double *               Upper bounds, not used for golden or sim
  GFT_INPUT_LBOUNDS       array double *               Lower bounds, not used for golden or sim
  GFT_INPUT_SEED          single int *                 Input seed for any random number generator. 
  GFT_INPUT_PSNPART       single int *                 Number of particles for any minimum finder that works with separate group solutions (psw)
  GFT_INPUT_PSCOGNI       single double *              pswarm cognitional parameter 
  GFT_INPUT_PSSOCIA       single double *              pswarm social parameter 
  GFT_INPUT_PSMAXVF       single double *              pswarm maximum velocity factor 
  GFT_INPUT_PSNITFI       single int *                 pswarm number of iterations to final weight 
  GFT_INPUT_PSINIIN       single double *              pswarm initial weight 
  GFT_INPUT_PSFININ       single double *              pswarm final weight 
  GFT_INPUT_PSINCDE       single double *              pswarm increase mesh delta by this factor 
  GFT_INPUT_PSDECDE       single double *              pswarm decrease mesh delta by this factor 


  @param gft_mstv (gft_mst *)  Pointer to main struct
  @param input    (void *)     input structure, type defined by spec
  @param spec     (int)        specifyer of the type of input

  @return (success) int gft_put_method: 0
          (error)                       standard
*/
/* ------------------------------------------------------------ */
int gft_mst_put(gft_mst *gft_mstv, void *input, int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int gft_mst_putf(gft_mst *gft_mstv, double (*input)(double *, void *), int spec)
  @brief Generically supply information to a minimiser struct on function basis

  Generic input interface to the minimiser struct. The type of input
  and its meaning is specified by spec. This function is beeing
  separated from gft_mst_put in order to avoid a non-ISO cast from
  void to function pointer.

  constant expression     input type                   description
  GFT_INPUT_GCHSQ         double (*)(double *, void *) Function to be minimised, takes as input a double array (of the size specified with NPARAM), and a neutral struct with additional arguments, this causes the deletion of the best fitting parameters.
  GFT_INPUT_GCHSQ_REP     double (*)(double *, void *) Function to be minimised, takes as input a double array (of the size specified with NPARAM), and a neutral struct with additional arguments, this does not cause the deletion of the best fitting parameters, useful if one wants to replace the fitting function with the same fitting function after some initialisation process.

  @param gft_mstv (gft_mst *)                  Pointer to main struct
  @param input    double (*)(double *, void *) input structure, type defined by spec
  @param spec     (int)                        specifyer of the type of input

  @return (success) int gft_mst_putf: 0
          (error)                       standard
*/
/* ------------------------------------------------------------ */
int gft_mst_putf(gft_mst *gft_mstv, double (*input)(double *, void *), int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int gft_mst_get(gft_mst *gft_mstv, void *output, int spec)
  @brief Generically get information from a minimiser struct

  Generic output interface to the minimiser struct. The type of output
  and its meaning is specified by spec. Arrays passed must have at
  least the size specified given by GFT_OUTPUT_NPARAM. If information
  cannot be accessed, indicated by a proper error message, the array
  may be changed nevertheless. In case of the input function and the
  additional arguments, the user must provide a pointer to a pointer
  and the ORIGINALS ARE PASSED, NO COPIES.

  constant expression     output type                   description
  GFT_OUTPUT_MISINF         single int *                 Indicator if enough information is gathered to start a fit process GFT_ERROR_NONE: can start, otherways not ready
  GFT_OUTPUT_BUSY          single int *                 Idle status: GFT_ERROR_NONE: idle, a fitting can start GFT_ERROR_BUSY: fitting process is running
  GFT_OUTPUT_STOPPED          single int *              Stop status: GFT_ERROR_NONE: not stopped, a fitting can start and changes can be made GFT_ERROR_BUSY: fitting process is stopped and can be carried on, no changes to the fitting struct are allowed
  GFT_OUTPUT_ERROR         single int *                 Overflow of a single parameter or underflow of the caracteristic length indicated GFT_ERROR_NONE: everything's ok. GFT_ERROR_OVERFLOW: some parameter overflow, the fitting will be stopped soon. GFT_ERROR_UNDERFLOW: Characteristic length is practically zero, the fitting will be stopped soon.
  GFT_OUTPUT_ALLCALLS      single size_t *              Number of calls of the input function. This is set to 0 if a new input function is inserted or the parameter number is changed, except if this is expressively avoided (GFT_INPUT_GCHSQ_REP)
  GFT_OUTPUT_ALLITER       single size_t *              Total number of iterations since start. This is set to 0 if a new input function is inserted or the parameter number is changed, except if this is expressively avoided (GFT_INPUT_GCHSQ_REP)
  GFT_OUTPUT_ALLOOPS       single size_t *              Total number of loops since start. This is set to 0 if a new input function is inserted or the parameter number is changed, except if this is expressively avoided (GFT_INPUT_GCHSQ_REP)
  GFT_OUTPUT_MINRUNS       single size_t *              Complete number of minimisation runs
  GFT_OUTPUT_METHOD        single size_t *                 Fit method as defined by GFT_MET_*
  GFT_OUTPUT_NPAR          single size_t *                 Number of parameters
  GFT_OUTPUT_NPAR_CUR      single int *                 Number of currently addressed parameter, -1 if multiple, otherwise starting at 0.
  GFT_OUTPUT_INDPOINTS     single double *              Number of independent data points. This is used to calculate a reduced chisquare
  GFT_OUTPUT_ACTCHISQ      single double *              Input function value from the last time the function has been called.
  GFT_OUTPUT_ACTCHISQRED   single double *              Input function value from the last time the function has been called divided by independent datapoints minus free parameters.
  GFT_OUTPUT_BESTCHISQ     single double *              Minimal input function value encountered since the function has been defined.
  GFT_OUTPUT_BESTCHISQRED  single double *              Minimal input function value encountered since the function has been defined divided by independent datapoints minus free parameters.
  GFT_OUTPUT_PAR           array double *               Parameters passed to the input function the last time it has been called
  GFT_OUTPUT_BESTPAR       array double *               Parameters put into the input function with the lowest function value
  GFT_OUTPUT_SOLPAR        array double *               Parameters put into the input function as solution from the last minimisation run
  GFT_OUTPUT_SOLERR        array double *               Errors of the Parameters put into the input function as solution from the last minimisation run
  GFT_OUTPUT_SOLCHSQ      single double *              Chisquare of the minimum found in the last minimisation run
  GFT_OUTPUT_SOLCHSQRED   single double *              Chisquare of the minimum found in the last minimisation run divided by independent datapoints minus free parameters.
  GFT_OUTPUT_SPAR          array double *               Start parameters.
  GFT_OUTPUT_OPAR          array double *               Grid origin.
  GFT_OUTPUT_DPAR          array double *               Start stepwidths
  GFT_OUTPUT_NDPAR         array double *               Grid normalisation
  GFT_OUTPUT_GCHSQ         double (**)(double *, void *) Function to be minimised
  GFT_OUTPUT_ADAR          void **                      Additional arguments to the function to be minimised
  GFT_OUTPUT_NCALLS        single size_t *              Maximum number of function calls in minimisation run
  GFT_OUTPUT_CALLS         single size_t *              Function calls in minimisation run, will be reset upon start of minimisation
  GFT_OUTPUT_NITERS        single size_t *              Maximum number of iteration steps
  GFT_OUTPUT_ITERS         single size_t *              Number of iteration steps
  GFT_OUTPUT_NCALLS_ST     single size_t *              Maximum number of function calls within one iteration step.
  GFT_OUTPUT_CALLS_ST      single size_t *              Number of function calls within actual iteration step.
  GFT_OUTPUT_STOPSIZE      single double *              Starting stop size of characteristic length
  GFT_OUTPUT_STOPSIZE_ACT  single double *              Stop size of characteristic length in active loop
  GFT_OUTPUT_SIZE          single double *              Actual characteristic size
  GFT_OUTPUT_DSIZE         single double *              Actual length of iteration
  GFT_OUTPUT_LOOPS         single size_t *              Total number of loops
  GFT_OUTPUT_LOOP          single size_t *              Actual loop number
  GFT_OUTPUT_NCALLS_ST_FAC single double *              After each loop the maximum number of fuction calls within one iteration step will be multiplied with this number
  GFT_OUTPUT_DPAR_FAC      single double *              After each loop the start stepwidths will be multiplied with this number
  GFT_OUTPUT_STOPSIZE_FAC  single double *              After each loop the stopping size of the characteristic length will be multiplied with this number
  GFT_OUTPUT_NOPAR         array  double *              Normalised parameters for the last input function call
  GFT_OUTPUT_NOSPAR        array  double *              Normalised start parameters
  GFT_OUTPUT_NODPAR        array  double *              Normalised start step widths (dpar/ndpar)

  @param gft_mstv (gft_mst *)  Pointer to main struct
  @param output   (void *)     pointer to output structure, type defined by spec
  @param spec     (int)        specifyer of the type of output

  @return (success) int gft_get_method: 0
          (error)                       standard
*/
/* ------------------------------------------------------------ */
int gft_mst_get(gft_mst *gft_mstv, void *output, int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int gft_mst_act(gft_mst *gft_mstv, int spec)
  @brief Generically initiate actions

  Generically act on the minimiser struct. This includes the start of
  the fitting process.

  constant expression description
  GFT_ACT_START       start fit process
  GFT_ACT_STOP        interrupt fit process, with the possibility to go on
  GFT_ACT_BREAK       stop fit process, a new start will result in a complete new run
  GFT_ACT_INIT        Calculate fitting function once, on the actual parameter values
  GFT_ACT_FLUSH       Delete all information
  GFT_ACT_CLERR       Clear error status. If an error is encountered, put and act will be blocked until the error is cleared.

  @param gft_mstv (gft_mst *)  Pointer to main struct
  @param output   (void *)     pointer to output structure, type defined by spec
  @param spec     (int)        specifyer of the type of output

  @return (success) int gft_get_method: 0
          (error)                       standard
*/
/* ------------------------------------------------------------ */
int gft_mst_act(gft_mst *gft_mstv, int spec);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: gft.h,v $
   Revision 1.5  2011/05/04 01:13:59  jozsa
   did a lot

   Revision 1.4  2007/12/12 17:19:21  gjozsa
   checked something

   Revision 1.3  2007/08/14 17:10:51  gjozsa
   Added some counters

   Revision 1.2  2007/07/02 12:55:12  gjozsa
   Made ANSI compliant

   Revision 1.1  2007/06/22 12:43:29  gjozsa
   Added to CVS control

  
   ------------------------------------------------------------ */

/* Include guard */
#endif

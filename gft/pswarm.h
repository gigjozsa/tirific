/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file pswarm.h
   @brief Pswarm minimisation algorithm

   The original pswarm routine was published by A.I.F. Vaz and
   L.N.Vicente (to date:
   http://www.norg.uminho.pt/aivaz/pswarm/). This is a version of
   pswarm provided by G. I. G. Jozsa (reshuffled version, C only,
   encapsulating all global variables, non-parallel, and no linear
   constraints).

   PSwarm is freely available for educational, research and commercial
   use, but we expect that all publications describing work using this
   software quote this reference:

   A.I.F. Vaz and L.N.Vicente, A particle swarm pattern search method
   for bound constrained global optimization, Journal of Global
   Optimization, 39 (2007) 197-219.

   $Source: /Volumes/DATA_J_II/data/CVS/tirific/include/cubarithm.h,v $
   $Date: 2009/05/26 07:56:39 $
   $Revision: 1.3 $
   $Author: jozsa $
   $Log: cubarithm.h,v $

*/
/* ------------------------------------------------------------ */

/* Include guard */
#ifndef PSWARM
#define PSWARM

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* SYMBOLIC CONSTANTS  and enumeration constants */
/* ------------------------------------------------------------ */

enum { /* Status and exit codes */ 
  PSWARM_STATUS_OK = 0, /* OK */
  PSWARM_STATUS_ERROR = 1, /* Generic error */
  PSWARM_STATUS_MEM = 2, /* Memory Problems */
  PSWARM_STATUS_INITIAL = 4, /* Wrong initialisation */
  PSWARM_STATUS_MSG = 8, /* New status message passed to pswarm_swarm -> statusm */
  PSWARM_STATUS_ABORT = 16, /* Terminating error */
  PSWARM_STATUS_EXIT = 32 /* Exiting loop */
};

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
   @struct pswarm_options
   @brief Container of input to the algorithm

   All input is stored here. A standard initialisation is provided by
   the routine pswarm_options_init(). Better to access this struct only by calling pswarm_i_ and pswarm_o_ functions
op
*/
/* ------------------------------------------------------------ */
typedef struct pswarm_options 
{
  /** @brief swarm size */
  int s;                           				    
  /** @brief cognitial and social parameters */
  double mu, nu;                   				    
  /** @brief maximum velocity factor */
  double maxvfactor;               				    
  /** @brief iterations until final weight is reached */
  int iterfweight;                     				     
  /** @brief maximum of function evaluations */
  /* int maxf;                        				     */
  /** @brief initial weight */
  double iweight;                  				    
  /** @brief final weight */
  double fweight;                  				    
  /** @brief tolerance for gradient norm */
  double n2grd;                    				    
  /** @brief bound limit */
  double blim;                     				    
  /** @brief Tolerance for stopping */
    double tol;                    				    
  /** @brief initial delta */
  double delta;                    				    
  /** @brief factor for initial delta */
  double fdelta;                   				    
  /** @brief increase delta */
  double idelta;                   				    
  /** @brief decrease delta */
  double ddelta;                   				    
  /** @brief Poll order */
  int pollbasis;                   				    
  /** @brief Epsilon for active constraints */
  double EpsilonActive;            				    
  /** @brief print information each IPrint iterations */
  /* int IPrint;                      				     */
  /** @brief process pswarm_swarm members status and statusm, usually conditional print, by default pswarm_standardprint() */
  int (*printfun)();               				    
  /** @brief Switch: objective function called with a vector of points */
  int vectorized;                  				    
  /** @brief Input seed for quasi-rng */
  unsigned int inputseed;          				    
  /** @brief dimension */
  int n;                           				    
  /** @brief objective function, the real one */
  double (*fun)(double *, void *); 				    
  /** @brief additional arguments */
  void *adar;                      				    
  /** @brief lower bounds */
  double *lb;                      				    
  /** @brief upper bounds */
  double *ub;                      				    
  /** @brief first guess */
  double *fg;                                                       
} pswarm_options;


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct pswarm_swarm
   @brief Container of internal information and output of the algorithm

   Container for internal use and output. A standard initialisation is provided by
   the routine pswarm_swarm_init().

*/
/* ------------------------------------------------------------ */
typedef struct pswarm_swarm {
  /** @brief particles */
  double *x;       
  /** @brief velocities */
  double *v;       
  /** @brief best particle so far */
  double *y;       
  /** @brief particle is active */
  int *active;     
  /** @brief all particles, objective function values */
  double *fx;      
  /** @brief best particle so far objective function value */
  double *fy;      
  /** @brief solution, NULL until finished, then pswarm_swarm -> y[gbest*opt->n] */
  double *sol;     
  /** @brief poll step size */
  double delta;
  /** @brief maximum normalized velocity */
  double maxnormv;    
  /** @brief scale for active constraints */
  /* double scale;     */
  /** @brief position of best particle: best function value fy[gbest], best vector y+gbest*n */
  int gbest;       
  /** @brief number of active particles*/
  int actives;     
  /** @brief Number of iterations */
  int iter;        
  /** @brief Number of calls to the objective function */
  int objfunctions;
  /** @brief Number of poll steps */
  int pollsteps;   
  /** @brief Number of successful poll steps */
  int sucpollsteps;
  /** @brief The current status, see PSWARM_STATUS_XXX */
  int status;      
  /** @brief Any status message */
  char *statusm;   
  /** @brief Internal container */
  void *adar;      
} pswarm_swarm;

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn pswarm_options *pswarm_options_const(void)
    @brief Allocate pswarm_options struct, setting all suitable pointers to NULL
    
    @return (success) pswarm_options *pswarm_options_const allocated struct
            (error) NULL
*/
/* ------------------------------------------------------------ */
pswarm_options *pswarm_options_const(void);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int pswarm_options_init(pswarm_options *opt, int n, double (*fun)(double *, void *), void *adar, double *lb, double *ub, double *fg)
    @brief Fill pswarm_options struct with default options
    
    @param n     (int)            Number of parameters
    @param fun   (double (*)())   Real objective function, input to function is 
                                  always a vector of size n that will be optimised and an unspecified container with additional arguments
    @param adar  (void *)         Container for additional arguments to fun, will not be copied, only linked.
    @param lb    (double *)       Lower bounds to the argument of fun, will be copied, not linked.
    @param ub    (double *)       Upper bounds to the argument of fun, will be copied, not linked.
    @param fg    (double *)       First guess, can be NULL, will be copied.
    @param tol   (double)         Tolerance as a stopping condition.

    @return (success) int pswarm_options_init 0
*/
/* ------------------------------------------------------------ */
int pswarm_options_init(pswarm_options *opt, int n, double (*fun)(double *, void *), void *adar, double *lb, double *ub, double *fg, double tol);

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn void pswarm_options_destr(pswarm_options *opt)
    @brief Destroy pswarm_options struct

    Destroys the struct and all struct members recursively.

    @param opt (pswarm_options *) Options struct to be destroyed

    @return void
*/
/* ------------------------------------------------------------ */
void pswarm_options_destr(pswarm_options *opt);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn pswarm_swarm *pswarm_swarm_const(void)
    @brief Allocate pswarm_swarm struct, setting all suitable pointers to NULL
    
    @return (success) pswarm_swarm *pswarm_swarm_const allocated struct
            (error) NULL
*/
/* ------------------------------------------------------------ */
pswarm_swarm *pswarm_swarm_const(void);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int pswarm_swarm_init(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv)
    @brief Fill pswarm_swarm struct with default values, given a pswarm_options struct

    Allocate a swarm struct and initialise it properly. The
    pswarm_options struct optv must be properly allocated and
    initiated. Any member of the struct that has been allocated and
    linked in the struct will be destroyed and replaced by a newly
    allocated member.

    @param pswarm_options *pswarm_optionsv          Number of parameters
    @param pswarm_swarm   *pswarm_swarmv            pswarm_swarm struct, properly allocated (best with pswarm_swarm_const())

    @return (success) int pswarm_swarm_init 0
*/
/* ------------------------------------------------------------ */
int pswarm_swarm_init(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn void pswarm_swarm_destr(pswarm_swarm *pswarm_swarmv)
    @brief Destroy pswarm_swarm struct instance

    Destroys the struct instance and all struct members recursively.

    @param pswarm_swarmv (pswarm_swarm *) pswarm_swarm struct to be destroyed

    @return void
*/
/* ------------------------------------------------------------ */
void pswarm_swarm_destr(pswarm_swarm *pswarm_swarmv);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int pswarm_init(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv)
    @brief Synchronize before minimizing

    Synchronizes pswarm_swarm with pswarm_options struct, initialises internal variables. To be called before minimizing.

    @param pswarm_optionsv (pswarm_options *) Properly allocated (and perhaps modified) input struct
    @param pswarm_swarmv   (pswarm_swarm *)   Properly allocated pswarm_swarm struct

    @return void
*/
/* ------------------------------------------------------------ */
int pswarm_init(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv);

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int pswarm_iter(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv)
    @brief Perform one minimisation iteration

    Consecutive calls of this function will lead to a minimisation

    @param pswarm_optionsv (pswarm_options *) Properly allocated input struct
    @param pswarm_swarmv    (pswarm_swarm *)   Properly allocated pswarm_swarm struct

    @return int pswarm_iter(), success: 0
*/
/* ------------------------------------------------------------ */
int pswarm_iter(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv);

/* This is a standard function that prints any message stored in pswarm_swarmv -> statusm. As a standard, this function will be called any time as pswarm -> statusm changes. To change the function to one provided by the user, hook up the user-provided function in the pswarm_options struct member printfunc (pswarm_options -> printfunc = &userfunction, where userfunction is declared int userfunction(pswarm_swarm *)). */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int pswarm_standardprint(pswarm_swarm *pswarm_swarmv)
    @brief Standard printing routine

    If pswarm_swarmv -> status inidcates a new message, will print
    pswarm_swarmv to i) stderr, if error is indicated ii) to stdout if
    no error is indicated.

    @param pswarm_swarmv    (pswarm_swarm *)   Properly allocated pswarm_swarm struct

    @return int pswarm_standardprint, success: 0
*/
/* ------------------------------------------------------------ */
int pswarm_standardprint(pswarm_swarm *pswarm_swarmv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int pswarm_outroutine(int status, const char *message, pswarm_options *opt, pswarm_swarm *pop)
    @brief Standard output processing routine

    Copies status to pswarm_swarmv -> status, copies message to
    pswarm_swarmv -> statusm, calls pswarm_options -> printfun(pop),
    sets pswarm_swarmv -> status to PSWARM_STATUS_OK. Simple but useful.

    @param pswarm_swarmv    (pswarm_swarm *)   Properly allocated pswarm_swarm struct

    @return int pswarm_outroutine(), success: 0
*/
/* ------------------------------------------------------------ */
int pswarm_outroutine(int status, const char *message, pswarm_options *opt, pswarm_swarm *pop);

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int pswarm_check_exit(int maxiter, int maxf, pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv)
    @brief Check if stopping criteria are reached

    On error the function will return PSWARM_STATUS_ERROR.
    Will return PSWARM_STATUS_EXIT and send a comment to the output function printfun linked in pswarm_optionsv if
    i) the maximum number of iteration has been reached (pswarm_swarmv -> iter >= maxiter)
    ii) the maximum number of function calls has been reached (pswarm_swarmv -> objfunctions >= maxf)
    iii) the maximum velocity and the step width are both below tolerance pswarm_optionsv -> tol
    iv) only one particle is active and step width is below tolerance pswarm_optionsv -> tol
    Otherwise the function returns PSWARM_STATUS_OK.

    @param maxiter         (int)              maximum number of iterations
    @param maxf            (int)              maximum number of function calls
    @param pswarm_optionsv (pswarm_options *) properly allocated and processed pswarm_options struct
    @param pswarm_swarmv   (pswarm_swarm *)   properly allocated and processed pswarm_swarm struct

    @return int pswarm_check_exit(), success: PSWARM_STATUS_EXIT or PSWARM_STATUS_OK
*/
/* ------------------------------------------------------------ */
int pswarm_check_exit(int maxiter, int maxf, pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int pswarm_i_dimension(pswarm_options *pswarm_optionsv, int size)
    @brief Input of dimension.

    Input function for the swarm size. Call pswarm_options_init, then
    pswarm_swarm_init, then pswarm_init before fitting. In general,
    input functions have an options struct as an input and an
    additional argument, which is either an int, a double, a function
    (pointer), or a double pointer. In case of a double pointer, the
    dimension of the array is the dimension of the problem.

    @param pswarm_optionsv (pswarm_options *) Properly allocated pswarm_options struct
    @param size            (int)              Swarm size

    @return int pswarm_i_dimension(), success: PSWARM_STATUS_EXIT or PSWARM_STATUS_OK
*/
/* ------------------------------------------------------------ */
/** @brief problem dimension */                        int pswarm_i_dimension  (pswarm_options *pswarm_optionsv, int    n            );
/** @brief swarm size */                               int pswarm_i_swarmsize  (pswarm_options *pswarm_optionsv, int    s            );
/** @brief Tolerance for stopping */                   int pswarm_i_tolerance  (pswarm_options *pswarm_optionsv, double tol          );
/** @brief Input seed for quasi-rng */                 int pswarm_i_seed       (pswarm_options *pswarm_optionsv, int    inputseed    );
/** @brief lower bounds */                             int pswarm_i_lowbounds  (pswarm_options *pswarm_optionsv, double *lb          );
/** @brief upper bounds */                             int pswarm_i_upbounds   (pswarm_options *pswarm_optionsv, double *ub          );
/** @brief first guess */                              int pswarm_i_firstguess (pswarm_options *pswarm_optionsv, double *fg          );

/** @brief cognitial parameter */                      int pswarm_i_cognitial  (pswarm_options *pswarm_optionsv, double mu           );
/** @brief social parameter */                         int pswarm_i_social     (pswarm_options *pswarm_optionsv, double nu           );
/** @brief maximum velocity factor */                  int pswarm_i_maxvfac    (pswarm_options *pswarm_optionsv, double maxvfactor   );
/** @brief iterations until final weight is reached */ int pswarm_i_iterfweight(pswarm_options *pswarm_optionsv, int    iterfweight  );
/** @brief initial weight */                           int pswarm_i_initweight (pswarm_options *pswarm_optionsv, double iweight      );
/** @brief final weight */                             int pswarm_i_finiweight (pswarm_options *pswarm_optionsv, double fweight      );
/** @brief tolerance for gradient norm */              int pswarm_i_n2grd      (pswarm_options *pswarm_optionsv, double n2grd        );
/** @brief bound limit */                              int pswarm_i_boundlim   (pswarm_options *pswarm_optionsv, double blim         );
/** @brief initial delta (grid size) */                int pswarm_i_initdelta  (pswarm_options *pswarm_optionsv, double delta        );
/** @brief factor for initial delta */                 int pswarm_i_facdelta   (pswarm_options *pswarm_optionsv, double fdelta       );
/** @brief increase delta */                           int pswarm_i_incdelta   (pswarm_options *pswarm_optionsv, double idelta       );
/** @brief decrease delta */                           int pswarm_i_decdelta   (pswarm_options *pswarm_optionsv, double ddelta       );
/** @brief Poll order */                               int pswarm_i_pollorder  (pswarm_options *pswarm_optionsv, int    pollbasis    );
/** @brief Epsilon for active constraints */           int pswarm_i_epsilonact (pswarm_options *pswarm_optionsv, double EpsilonActive);
/** @brief additional arguments */                     int pswarm_i_adar       (pswarm_options *pswarm_optionsv, void   *adar        );
/** @brief additional arguments */                     int pswarm_i_printfun   (pswarm_options *pswarm_optionsv, int    (*printfun)(pswarm_swarm *));

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
    @fn int pswarm_o_particle(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *x)
    @brief Particle coordinates, array size n*s

    Output function for the particle coordinates. The output functions
    are either arrays or single numbers. The length is indicated in
    the brief description. n is the dimension of the problem, s the
    swarm size.

    @param pswarm_optionsv (pswarm_options *) Properly allocated pswarm_options struct
    @param pswarm_swarmv   (pswarm_swarm *)   Properly allocated pswarm_swarm struct
    @param x               (double *)         Array large enough to hold n*s doubles

    @return int pswarm_o_particle(), success: PSWARM_STATUS_EXIT or PSWARM_STATUS_OK
*/
/* ------------------------------------------------------------ */
/** @brief particles, array size n*s */                                     int pswarm_o_particles (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *x          );
/** @brief velocities, array size n*s */                                    int pswarm_o_velocities(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *v          );
/** @brief coordinates for best value array size n*s */                     int pswarm_o_bestcoords(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *y          );
/** @brief particle is active, array size s */                              int pswarm_o_actives   (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    *active     );
/** @brief all particles, objective function values, array size s */        int pswarm_o_values    (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *fx         );
/** @brief best particle so far objective function value, array size s */   int pswarm_o_bestvalues(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *fy         );
/** @brief solution, NULL until finished, array size n */                   int pswarm_o_solution  (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *sol        );
/** @brief poll step size */                                                int pswarm_o_stepsize  (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double delta       );
/** @brief maximum normalized velocity */                                   int pswarm_o_maxvel    (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double maxnormv    );
/** @brief position of best particle */                                     int pswarm_o_coordsbest(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    gbest       );
/** @brief number of active particles*/                                     int pswarm_o_nparticles(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    actives     );
/** @brief Number of iterations */                                          int pswarm_o_niters    (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    iter        );
/** @brief Number of calls to the objective function */                     int pswarm_o_calls     (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    objfunctions);
/** @brief Number of poll steps */                                          int pswarm_o_pollsteps (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    pollsteps   );
/** @brief Number of successful poll steps */                               int pswarm_o_spollsteps(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    sucpollsteps);
/** @brief The current status, see PSWARM_STATUS_XXX */                     int pswarm_o_status    (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    status      );
/** @brief Any status message, size 201 char */                             int pswarm_o_statusm   (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, char   *statusm    );

#endif

/* 
The original pswarm routine was published by A.I.F. Vaz and
L.N.Vicente (to date: http://www.norg.uminho.pt/aivaz/pswarm/). This
is a version of pswarm provided by G. I. G. Jozsa (reshuffled version,
C only, encapsulating all global variables, non-parallel, and no
linear constraints).

PSwarm is freely available for educational, research and commercial
use, but we expect that all publications describing work using this
software quote this reference:

A.I.F. Vaz and L.N.Vicente, A particle swarm pattern search method for
bound constrained global optimization, Journal of Global Optimization,
39 (2007) 197-219.
*/

/* ******* */
/* external includes */
/* ******* */

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<memory.h>
#include<float.h>

/* ******* */
/* internal includes */
/* ******* */
#include "pswarm.h"

/* ******* */
/* Symbolic- and enumeration constants */
/* ******* */

/* Defines to use (1) or not use (0) the standard rng. If 0, the rng is defined below */
#define SYS_RANDOM 0 

#define STATUSLEN 800 /* length of status line */

#if SYS_RANDOM!=1
  #define SHUFFLE 256     /* size of random array */
#endif

/* ******* */
/* Structs */
/* ******* */

/* pattern declarations, start */

typedef struct poll_vector {
	double *vector;    /* vector */
	struct poll_vector *next;
} poll_vector;

/* This replaces some global variables and is linked as adar in pswarm_swarm */
typedef struct poll_container {
  poll_vector *PVectors;           /* lninked list of poll vectors */
  poll_vector *D;                  /* linked list of poll vectors maximal basis */
  poll_vector *last_success;       /* unknown thing that needs to be passed */
  /* poll_vector *last_D;   */

  int iterunsuc;
  double *maxv; /* maximum velocity allowed is stored here */
  double *alphamax;

  /** @brief rng seed to be passed along */
  long rand_seed;  

} poll_container;

/* ******* */
/* Function declarations */
/* ******* */
static void pollstep(int pi, pswarm_options *opt, pswarm_swarm *pop);
static void init_D(int n, pswarm_options *opt, poll_container *poll_containerv);
static void init_pattern(int, pswarm_options *opt, poll_container *poll_containerv);
static void objfn(double (*)(double *, void *), void *, int, int, double *, double *, double *, double *);

/* Allocates a poll_container and sets all pointers to 0 */
static poll_container *poll_container_const();

/* Destroys poll_container including the linked lists */
static int poll_container_destr(poll_container *);

/* Destroys poll_vector linked list, pass first element, otherwise memleakage */
static int poll_vector_destr(poll_vector *poll_vectorv);

/* Allocates a poll_container and sets all pointers to 0 */
static poll_container *poll_container_const();

/* Destroys poll_container including the linked lists */
static int poll_container_destr(poll_container *);

/* Destroys poll_vector linked list, pass first element, otherwise memleakage */
static int poll_vector_destr(poll_vector *);

/* pattern declarations, end */

static int feasible_p(int n, double *x, double *lb, double *ub);
static int init_pop(struct pswarm_swarm *, pswarm_options *, int, double *);
static double projection(double, double, double);
static void *pswarm_malloc(size_t size);

static int popnswarm_sanity(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv);
#if SYS_RANDOM != 1
  static double randflt(long *rand_seed);
  static double resettable_randflt(long *rand_seed, int reset);
#endif

/* Copy arrays */
static void copydarray(double *in, double *out, int n);
int o_dbl_gen(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *in, double *out, int dim);
int o_int_gen(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int *in, int *out, int dim);


/* ******* */
/* Function definitions */
/* ******* */
/* Copy arrays */
static void copydarray(double *in, double *out, int n)
{
  int i;
  for (i = 0; i < n; ++i)
    out[i] = in[i];
  return;
}

/* Constructor opt struct */
pswarm_options *pswarm_options_const()
{
  pswarm_options *pswarm_opt_const;
  if (!(pswarm_opt_const = (pswarm_options *) malloc(sizeof(pswarm_options))))
    return NULL;

  pswarm_opt_const -> adar = NULL;
  pswarm_opt_const -> lb = NULL;
  pswarm_opt_const -> ub = NULL;
  pswarm_opt_const -> fg = NULL;

  return pswarm_opt_const;
}

/* Destructor opt struct */
void pswarm_options_destr(pswarm_options *pswarm_opt) {
  if (!pswarm_opt)
    return;

  /*  if ((pswarm_opt -> adar))
      free(pswarm_opt -> adar); */
  if ((pswarm_opt -> lb))
    free(pswarm_opt -> lb);
  if ((pswarm_opt -> ub))
    free(pswarm_opt -> ub);
  if ((pswarm_opt -> fg))
    free(pswarm_opt -> fg);

  free(pswarm_opt);
  return;
}

/* Input to pswarm */
int pswarm_options_init(pswarm_options *opt, int n, double (*fun)(double *, void *), void *adar, double *lb, double *ub, double *fg, double tol)
{
  int i;
  double *lbv = NULL;
  double *ubv = NULL;
  double *fgv = NULL;

  if (n < 1)
    return 1;

  if (!(lbv = malloc(n*sizeof(double)))) goto error; for (i = 0; i < n; ++i) lbv[i] = lb[i]; 
  if (!(ubv = malloc(n*sizeof(double)))) goto error; for (i = 0; i < n; ++i) ubv[i] = ub[i]; 
  if (!(fgv = malloc(n*sizeof(double)))) goto error; for (i = 0; i < n; ++i) fgv[i] = fg[i]; 

  opt -> s             = 42;      /* swarm size */ 
  opt -> mu            = 0.5;     /* cognitial parameter */
  opt -> nu            = 0.5;     /* social parameter */
  opt -> maxvfactor    = 0.5;     /* maximum velocity factor */
  opt -> iterfweight  = 8000;     /* iterations until final weight is reached */
  /* opt -> maxf          = 50000;    maximum of function evaluations */
  opt -> iweight       = 0.9;     /* initial weight (initial inertia) */
  opt -> fweight       = 0.4;     /* final weight (final inertia) */

  /*   opt -> n2grd         = 0.5; */     /* max norm 2 for gradient */
  opt -> tol           = tol;     /* tolerance for stopping */
  opt -> delta         = DBL_MAX;     /* initial delta -- is computed or user provided, computed if DBL_MAX */ 
  opt -> idelta        = 2;       /* increase delta by a factor of */
  opt -> ddelta        = 0.5;     /* decrease delta by a factor of */
  /*  opt -> EpsilonActive = 0.1;   */  /* active constraints epsilon */
  opt -> printfun      = &pswarm_standardprint;      /* funciton to process members status and statusm of pswarm_swarm struct (usually printing) */
  opt -> vectorized    = 1;       /* Switch: vectorized call to the objective function? n: 0 y: not 0 */
  opt -> inputseed     = 1;       /* Input seed for quasi-rng */
  opt -> n             = n;       /* dimension */

  opt -> fun           = fun;     /* objective function, the real one */
  opt -> adar          = adar;    /* additional arguments */
  opt -> lb            = lbv;      /* lower bounds */
  opt -> ub            = ubv;      /* upper bounds */
  opt -> fg            = fgv;      /* first guess */

  /* Obsoletes */
  opt -> pollbasis     = 0;       /* switch to select type of basis on pattern search, parameter is not used, this is always the trivial case */
  opt -> blim          = 10;      /* bound limit, not understood */
  opt -> fdelta        = 5.0;     /* factor for initial delta, don't change */

  return 0;

 error:
  if ((lbv)) free(lbv);
  if ((ubv)) free(ubv);
  if ((fgv)) free(fgv);
  return 1;
}

/* Constructor out struct */ 
pswarm_swarm *pswarm_swarm_const()
{
  pswarm_swarm *pswarm_swarm_const;
  if (!(pswarm_swarm_const = (pswarm_swarm *) malloc(sizeof(pswarm_swarm))))
    return NULL;

  pswarm_swarm_const -> x = NULL;
  pswarm_swarm_const -> v = NULL;
  pswarm_swarm_const -> y = NULL;
  pswarm_swarm_const -> active = NULL;
  pswarm_swarm_const -> fx = NULL;
  pswarm_swarm_const -> fy = NULL;
  pswarm_swarm_const -> sol = NULL;
  pswarm_swarm_const -> statusm = NULL;
  pswarm_swarm_const -> adar = NULL;

  if (!(pswarm_swarm_const -> statusm = (char *) malloc(81*sizeof(char))))
    goto error;

  strcpy(pswarm_swarm_const -> statusm, "OK");

  return pswarm_swarm_const;

 error:
  pswarm_swarm_destr(pswarm_swarm_const);
  return NULL;
}

/* Destructor swarm struct */
void pswarm_swarm_destr(pswarm_swarm *pswarm_swarmv) {
  if (!pswarm_swarmv)
    return;

  if ((pswarm_swarmv -> x)) free(pswarm_swarmv -> x);
  if ((pswarm_swarmv -> v)) free(pswarm_swarmv -> v);
  if ((pswarm_swarmv -> y)) free(pswarm_swarmv -> y);
  if ((pswarm_swarmv -> active)) free(pswarm_swarmv -> active);
  if ((pswarm_swarmv -> fx)) free(pswarm_swarmv -> fx);
  if ((pswarm_swarmv -> fy)) free(pswarm_swarmv -> fy);
  if ((pswarm_swarmv -> sol)) free(pswarm_swarmv -> sol);
  if ((pswarm_swarmv -> statusm)) free(pswarm_swarmv -> statusm);
  if ((pswarm_swarmv -> adar)) poll_container_destr((poll_container *) pswarm_swarmv -> adar);

  free(pswarm_swarmv);
  return;
}

/* Constructor out struct */ 
int pswarm_swarm_init(pswarm_options *optv, pswarm_swarm *pswarm_swarmv)
{
  double *x = NULL;      /* particles */
  double *v = NULL;      /* velocities */
  double *y = NULL;      /* best particle so far per swarm */
  int *active = NULL;    /* particle is active */
  double *fx = NULL;     /* Best particle so far */
  double *fy = NULL;     /* best particle so far objective function */
  double *sol = NULL;    /* current solution */
  char *statusm = NULL;  /* status message */
  poll_container *poll_containerv = NULL;

  /* Wrong dimension, the programmer should really notice by him/herself */
  if (optv -> n < 1)
    return 1;

  /* Wrong number of swarms, the programmer should really notice by him/herself */
  if (optv -> s < 1)
    return 2;

  /* No input struct */
  if (!pswarm_swarmv)
    return 3;

  /* Allocate memory first, this is paranoid, but why not */
  if (!(x       = malloc(optv -> s*optv -> n   *sizeof(double)))) goto error;
  if (!(v       = malloc(optv -> s*optv -> n   *sizeof(double)))) goto error;
  if (!(y       = malloc(optv -> s*optv -> n   *sizeof(double)))) goto error;
  if (!(fx      = malloc(optv -> s             *sizeof(double)))) goto error;
  if (!(fy      = malloc(optv -> s             *sizeof(double)))) goto error;
  if (!(sol     = malloc(optv -> n             *sizeof(double)))) goto error;
  if (!(active  = malloc(optv -> s             *sizeof(int))))    goto error;
  if (!(statusm = malloc((STATUSLEN+1) *sizeof(char))))   goto error;

  /* Allocate the poll containter */
  if (!(poll_containerv = poll_container_const())){
    goto error;
  }

  if (!(poll_containerv -> alphamax = (double *) malloc(optv -> n*sizeof(double)))) {
    goto error;
  }

  /* allocate memory for maximum velocity allowed */
  if (!(poll_containerv -> maxv = (double *) malloc(optv -> n*sizeof(double)))) {
    goto error;
  }

  /* Check if we have filled struct */
  if ((pswarm_swarmv -> x))       free(pswarm_swarmv -> x);       pswarm_swarmv -> x       = NULL;
  if ((pswarm_swarmv -> v))       free(pswarm_swarmv -> v);       pswarm_swarmv -> v       = NULL;
  if ((pswarm_swarmv -> y))       free(pswarm_swarmv -> y);       pswarm_swarmv -> y       = NULL;
  if ((pswarm_swarmv -> active))  free(pswarm_swarmv -> active);  pswarm_swarmv -> active  = NULL;
  if ((pswarm_swarmv -> fx))      free(pswarm_swarmv -> fx);      pswarm_swarmv -> fx      = NULL;
  if ((pswarm_swarmv -> fy))      free(pswarm_swarmv -> fy);      pswarm_swarmv -> fy      = NULL;
  if ((pswarm_swarmv -> sol))     free(pswarm_swarmv -> sol);     pswarm_swarmv -> sol     = NULL;
  if ((pswarm_swarmv -> statusm)) free(pswarm_swarmv -> statusm); pswarm_swarmv -> statusm = NULL;
  if ((pswarm_swarmv -> adar))    poll_container_destr((poll_container *) pswarm_swarmv -> adar); pswarm_swarmv -> adar = NULL;

  /* Now link */
  pswarm_swarmv -> x      = x;
  pswarm_swarmv -> v      = v;
  pswarm_swarmv -> y      = y;
  pswarm_swarmv -> fx     = fx;
  pswarm_swarmv -> fy     = fy;
  pswarm_swarmv -> sol    = sol;
  pswarm_swarmv -> active = active;
  pswarm_swarmv -> adar = (void *) poll_containerv;

#if SYS_RANDOM==1
  srand(optv -> inputseed);
#else
  poll_containerv  -> rand_seed = optv -> inputseed;
  resettable_randflt (&(poll_containerv  -> rand_seed), 1); /* initialize random number generator */
#endif

  pswarm_swarmv -> status = PSWARM_STATUS_MSG;
  pswarm_swarmv -> statusm = statusm;
  sprintf(pswarm_swarmv -> statusm, "Swarm initialised");
  optv -> printfun(pswarm_swarmv);
  pswarm_swarmv -> status = PSWARM_STATUS_OK;

  return 0;

 error:
  if ((x))       free(x);
  if ((v))       free(v);
  if ((y))       free(y);
  if ((active))  free(active);
  if ((fx))      free(fx);
  if ((fy))      free(fy);
  if ((sol))     free(sol);
  if ((statusm)) free(statusm);
  if ((poll_containerv)) poll_container_destr(poll_containerv);
  return PSWARM_STATUS_MEM;
}

/** @brief swarm dimension */                          int pswarm_i_dimension  (pswarm_options *pswarm_optionsv, int    n            ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> n            = n            ; return PSWARM_STATUS_OK; }
/** @brief swarm size */                               int pswarm_i_swarmsize  (pswarm_options *pswarm_optionsv, int    s            ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> s            = s            ; return PSWARM_STATUS_OK; }
/** @brief cognitial parameter */                      int pswarm_i_cognitial  (pswarm_options *pswarm_optionsv, double mu           ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> mu           = mu           ; return PSWARM_STATUS_OK; }
/** @brief social parameter */                         int pswarm_i_social     (pswarm_options *pswarm_optionsv, double nu           ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> nu           = nu           ; return PSWARM_STATUS_OK; }
/** @brief maximum velocity factor */                  int pswarm_i_maxvfac    (pswarm_options *pswarm_optionsv, double maxvfactor   ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> maxvfactor   = maxvfactor   ; return PSWARM_STATUS_OK; }
/** @brief iterations until final weight is reached */ int pswarm_i_iterfweight(pswarm_options *pswarm_optionsv, int    iterfweight  ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> iterfweight  = iterfweight  ; return PSWARM_STATUS_OK; }
/** @brief initial weight */                           int pswarm_i_initweight (pswarm_options *pswarm_optionsv, double iweight      ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> iweight      = iweight      ; return PSWARM_STATUS_OK; }
/** @brief final weight */                             int pswarm_i_finiweight (pswarm_options *pswarm_optionsv, double fweight      ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> fweight      = fweight      ; return PSWARM_STATUS_OK; }
/** @brief bound limit */                              int pswarm_i_boundlim   (pswarm_options *pswarm_optionsv, double blim         ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> blim         = blim         ; return PSWARM_STATUS_OK; }
/** @brief Tolerance for stopping */                   int pswarm_i_tolerance  (pswarm_options *pswarm_optionsv, double tol          ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> tol          = tol          ; return PSWARM_STATUS_OK; }
/** @brief initial delta (grid size) */                int pswarm_i_initdelta  (pswarm_options *pswarm_optionsv, double delta        ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> delta        = delta        ; return PSWARM_STATUS_OK; }
/** @brief factor for initial delta */                 int pswarm_i_facdelta   (pswarm_options *pswarm_optionsv, double fdelta       ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> fdelta       = fdelta       ; return PSWARM_STATUS_OK; }
/** @brief increase delta */                           int pswarm_i_incdelta   (pswarm_options *pswarm_optionsv, double idelta       ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> idelta       = idelta       ; return PSWARM_STATUS_OK; }
/** @brief decrease delta */                           int pswarm_i_decdelta   (pswarm_options *pswarm_optionsv, double ddelta       ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> ddelta       = ddelta       ; return PSWARM_STATUS_OK; }
/** @brief Poll order */                               int pswarm_i_pollorder  (pswarm_options *pswarm_optionsv, int    pollbasis    ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> pollbasis    = pollbasis    ; return PSWARM_STATUS_OK; }
/** @brief Input seed for quasi-rng */                 int pswarm_i_seed       (pswarm_options *pswarm_optionsv, int    inputseed    ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> inputseed    = inputseed    ; return PSWARM_STATUS_OK; }
/** @brief additional arguments */                     int pswarm_i_adar       (pswarm_options *pswarm_optionsv, void   *adar        ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> adar         = adar         ; return PSWARM_STATUS_OK; }                
/** @brief lower bounds */                             int pswarm_i_lowbounds  (pswarm_options *pswarm_optionsv, double *lb          ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; copydarray(lb, pswarm_optionsv -> lb, pswarm_optionsv -> n); return PSWARM_STATUS_OK;}
/** @brief upper bounds */                             int pswarm_i_upbounds   (pswarm_options *pswarm_optionsv, double *ub          ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; copydarray(ub, pswarm_optionsv -> ub, pswarm_optionsv -> n); return PSWARM_STATUS_OK;}
/** @brief first guess */                              int pswarm_i_firstguess (pswarm_options *pswarm_optionsv, double *fg          ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; copydarray(fg, pswarm_optionsv -> fg, pswarm_optionsv -> n); return PSWARM_STATUS_OK;}

/** @brief Print function */                           int pswarm_i_printfun   (pswarm_options *pswarm_optionsv, int    (*printfun)(pswarm_swarm *)){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> printfun = printfun; return PSWARM_STATUS_OK;}
/** @brief objective function */                       int pswarm_i_fun        (pswarm_options *pswarm_optionsv, double (*fun)(double *, void *)){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> fun = fun; return PSWARM_STATUS_OK;}

/** @brief tolerance for gradient norm */             /* int pswarm_i_n2grd      (pswarm_options *pswarm_optionsv, double n2grd        ){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> n2grd        = n2grd        ; return PSWARM_STATUS_OK; } */
/** @brief Epsilon for active constraints */          /* int pswarm_i_epsilonact (pswarm_options *pswarm_optionsv, double EpsilonActive){if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; pswarm_optionsv -> EpsilonActive= EpsilonActive; return PSWARM_STATUS_OK; } */



int o_dbl_gen(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *in, double *out, int dim)
{
  int i;
  if ((in)) {
    for (i = 0; i < dim; ++i) {
      out[i] = in[i];
    }
  }
  return PSWARM_STATUS_OK;
}
int o_int_gen(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int *in, int *out, int dim)
{
  int i;
  if ((in)) {
    for (i = 0; i < dim; ++i) {
      out[i] = in[i];
    }
  }
  return PSWARM_STATUS_OK;
}

/** @brief particles, array size n*s */                                    int pswarm_o_particles (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *x          ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_dbl_gen(pswarm_optionsv, pswarm_swarmv,  pswarm_swarmv -> x           ,  x           , pswarm_optionsv -> n*pswarm_optionsv -> s);}
/** @brief velocities, array size n*s */                                   int pswarm_o_velocities(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *v          ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_dbl_gen(pswarm_optionsv, pswarm_swarmv,  pswarm_swarmv -> v           ,  v           , pswarm_optionsv -> n*pswarm_optionsv -> s);}
/** @brief coordinates for best value array size n*s */                    int pswarm_o_bestcoords(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *y          ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_dbl_gen(pswarm_optionsv, pswarm_swarmv,  pswarm_swarmv -> y           ,  y           , pswarm_optionsv -> n*pswarm_optionsv -> s);}
/** @brief particle is active, array size s */                             int pswarm_o_actives   (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    *active     ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_int_gen(pswarm_optionsv, pswarm_swarmv,  pswarm_swarmv -> active      ,  active      ,                      pswarm_optionsv -> s);}
/** @brief all particles, objective function values, array size s */       int pswarm_o_values    (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *fx         ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_dbl_gen(pswarm_optionsv, pswarm_swarmv,  pswarm_swarmv -> fx          ,  fx          ,                      pswarm_optionsv -> s);}
/** @brief best particle so far objective function value, array size s */  int pswarm_o_bestvalues(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *fy         ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_dbl_gen(pswarm_optionsv, pswarm_swarmv,  pswarm_swarmv -> fy          ,  fy          ,                      pswarm_optionsv -> s);}
/** @brief solution, NULL until finished, array size n */                  int pswarm_o_solution  (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double *sol        ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_dbl_gen(pswarm_optionsv, pswarm_swarmv,  pswarm_swarmv -> sol         ,  sol         , pswarm_optionsv -> n                     );}
/** @brief poll step size */                                               int pswarm_o_stepsize  (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double delta       ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_dbl_gen(pswarm_optionsv, pswarm_swarmv, &pswarm_swarmv -> delta       , &delta       , 1                                        );}
/** @brief maximum normalized velocity */                                  int pswarm_o_maxvel    (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, double maxnormv    ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_dbl_gen(pswarm_optionsv, pswarm_swarmv, &pswarm_swarmv -> maxnormv    , &maxnormv    , 1                                        );}
/** @brief position of best particle */                                    int pswarm_o_coordsbest(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    gbest       ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_int_gen(pswarm_optionsv, pswarm_swarmv, &pswarm_swarmv -> gbest       , &gbest       , 1                                        );}
/** @brief number of active particles*/                                    int pswarm_o_nparticles(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    actives     ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_int_gen(pswarm_optionsv, pswarm_swarmv, &pswarm_swarmv -> actives     , &actives     , 1                                        );}
/** @brief Number of iterations */                                         int pswarm_o_niters    (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    iter        ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_int_gen(pswarm_optionsv, pswarm_swarmv, &pswarm_swarmv -> iter        , &iter        , 1                                        );}
/** @brief Number of calls to the objective function */                    int pswarm_o_calls     (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    objfunctions) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_int_gen(pswarm_optionsv, pswarm_swarmv, &pswarm_swarmv -> objfunctions, &objfunctions, 1                                        );}
/** @brief Number of poll steps */                                         int pswarm_o_pollsteps (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    pollsteps   ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_int_gen(pswarm_optionsv, pswarm_swarmv, &pswarm_swarmv -> pollsteps   , &pollsteps   , 1                                        );}
/** @brief Number of successful poll steps */                              int pswarm_o_spollsteps(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    sucpollsteps) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_int_gen(pswarm_optionsv, pswarm_swarmv, &pswarm_swarmv -> sucpollsteps, &sucpollsteps, 1                                        );}
/** @brief The current status, see PSWARM_STATUS_XXX */                    int pswarm_o_status    (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, int    status      ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; return o_int_gen(pswarm_optionsv, pswarm_swarmv, &pswarm_swarmv -> status      , &status      , 1                                        );}
/** @brief Any status message, size 201 char */                            int pswarm_o_statusm   (pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv, char   *statusm    ) {if (!pswarm_optionsv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; if (!pswarm_swarmv) return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL; strcpy(statusm, pswarm_swarmv -> statusm); return PSWARM_STATUS_OK;}  /** @brief Any status message, size 201 char */                             


/* The objective function */
static void objfn(double (*fun)(double *, void *), void *adar, int n, int m, double *x, double *lb, double *ub, double *fx)
{
  int j;

  for(j=0;j<m;j++){
    fx[j]=fun(x+j*n, adar);
  }
}

/* synchronize everything */
int pswarm_init(pswarm_options *opt, pswarm_swarm *pop)
{
  int j, errval;
  double mindelta;
  poll_container *poll_containerv;
  double scale;

  if ((errval = popnswarm_sanity(opt, pop)))
    return errval;

  poll_containerv = (poll_container *) pop -> adar;
  pop -> actives = opt -> s;
  poll_containerv -> iterunsuc = 0;
  pop -> maxnormv=DBL_MAX; /* don't stop in first iteration */
  pop -> iter=0;
  pop -> pollsteps=0;
  pop -> sucpollsteps=0;
  pop -> objfunctions=0;
  pop -> gbest=0; /* global best */
  scale = 1.0; /* no scale */

  /* initialize maximum velocity allowed and compute delta. Compute scale for active constraints */
  if(opt -> delta >= DBL_MAX){ /* User didn't provide delta */
    mindelta = DBL_MAX;
    for(j = 0; j < opt -> n ; j++){
      if(opt -> lb[j] > -DBL_MAX && opt -> ub[j] < DBL_MAX) {
	if(scale < opt -> ub[j]-opt -> lb[j])
	  scale = opt -> ub[j]-opt -> lb[j];
	if(mindelta > (opt -> ub[j]-opt -> lb[j]))
	  mindelta = (opt -> ub[j]-opt -> lb[j]);
	poll_containerv -> maxv[j] = (opt -> ub[j]-opt -> lb[j])*opt -> maxvfactor;
      } 
      else {
	poll_containerv -> maxv[j] = DBL_MAX;
      }
    }
    if(mindelta>= DBL_MAX || mindelta<2*sqrt(opt -> tol))
      opt -> delta=2*sqrt(sqrt(opt -> tol));
    else opt -> delta=mindelta/opt -> fdelta;
  }

  /* initialize population */
  if(opt -> fg){
    pswarm_outroutine(PSWARM_STATUS_MSG, "Initial guess provided, including in initial population", opt, pop);
    if(init_pop(pop, opt, 1, opt -> fg)){
      pswarm_outroutine(PSWARM_STATUS_ERROR | PSWARM_STATUS_MEM, "Unable to initialize population\n", opt, pop);
      return PSWARM_STATUS_ERROR | PSWARM_STATUS_MEM;
    }
  } else {
    if(init_pop(pop, opt, 0, NULL)){
      pswarm_outroutine(PSWARM_STATUS_ERROR | PSWARM_STATUS_MEM, "Unable to initialize population\n", opt, pop);
      return PSWARM_STATUS_ERROR | PSWARM_STATUS_MEM;
    }
  }

  init_pattern(opt -> n, opt, (poll_container *) pop -> adar); /* initialize pattern search */

  return 0;
}

/* Minimal check if pop and opt are correctly initialised */
static int popnswarm_sanity(pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv)
{

  /* Sanity checks */
  /* Minimal check if pop and opt are correctly initialised */
  if (!(pswarm_swarmv)) {
    /* No other chance than standard channel */
    fprintf(stderr, "Empty pswarm_swarm struct passed");
    return (PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL);
  }

  if (!(pswarm_swarmv -> x && pswarm_swarmv -> v && pswarm_swarmv -> y && pswarm_swarmv -> active && pswarm_swarmv -> fx && pswarm_swarmv -> fy && pswarm_swarmv -> statusm)) {
    /* No other chance than standard channel */
    fprintf(stderr, "pswarm_swarm struct passed not properly initialised");
    return (PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL);
  }

  if (!(pswarm_optionsv)) {
    pswarm_outroutine(PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL, "Empty pswarm_options struct passed", pswarm_optionsv, pswarm_swarmv);
    return (PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL);
  }

  if (!(pswarm_optionsv -> lb && pswarm_optionsv -> ub && pswarm_optionsv -> fg)) {
    pswarm_outroutine(PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL, "pswarm_options struct passed without bounds and first guess", pswarm_optionsv, pswarm_swarmv);
    return (PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL);
  }

  if(pswarm_optionsv -> n < 1){
    pswarm_outroutine(PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL, "Number of variables must be positive in pswarm_iter", pswarm_optionsv, pswarm_swarmv);
    return PSWARM_STATUS_ERROR | PSWARM_STATUS_INITIAL;
  }

  return 0;
}

/* Standard print function */
int pswarm_standardprint(pswarm_swarm *pswarm_swarmv)
{
  if (!(pswarm_swarmv))
    return 1;

  if ((pswarm_swarmv -> status)) {
    if (pswarm_swarmv -> status & PSWARM_STATUS_ERROR)
      fprintf(stderr, "%s\n", pswarm_swarmv -> statusm);
    else {
      fprintf(stdout, "%s\n", pswarm_swarmv -> statusm);
    }
  } 
  return 0;
}

/* Standard output routine */
int pswarm_outroutine(int status, const char *message, pswarm_options *opt, pswarm_swarm *pop)
{
  if (!(pop) || !(opt) || !(pop -> statusm) || !(opt -> printfun))
    return PSWARM_STATUS_ERROR;
  sprintf(pop -> statusm, "%s", message);
  pop -> status = status;
  opt -> printfun(pop);
  if (!(pop -> status & PSWARM_STATUS_ERROR))
    pop -> status = PSWARM_STATUS_OK;

  return PSWARM_STATUS_OK;
}

int pswarm_check_exit(int maxiter, int maxf, pswarm_options *pswarm_optionsv, pswarm_swarm *pswarm_swarmv)
{
  if (popnswarm_sanity(pswarm_optionsv, pswarm_swarmv))
    return PSWARM_STATUS_ERROR;
  
  /* Check if we are finished */
  /* maximum number of iterations reached */
  if (pswarm_swarmv -> iter > maxiter) {
    pswarm_outroutine(PSWARM_STATUS_MSG | PSWARM_STATUS_EXIT, "Maximum number of iterations reached", pswarm_optionsv, pswarm_swarmv);
    return PSWARM_STATUS_EXIT;
  }
  /* maximum number of function calls reached */
  if (pswarm_swarmv -> objfunctions > maxf) {
    pswarm_outroutine(PSWARM_STATUS_MSG | PSWARM_STATUS_EXIT, "Maximum number of function calls reached", pswarm_optionsv, pswarm_swarmv);
    return PSWARM_STATUS_EXIT;
  }
  /* velocity and step width below tolerance */
  if (pswarm_swarmv -> maxnormv < pswarm_optionsv -> tol && pswarm_swarmv -> delta < pswarm_optionsv -> tol) {
    pswarm_outroutine(PSWARM_STATUS_MSG | PSWARM_STATUS_EXIT, "Velocity and step width below tolerance", pswarm_optionsv, pswarm_swarmv);
    return PSWARM_STATUS_EXIT;
  }
  /* only one particle left and step width below tolerance */
  if (pswarm_swarmv -> actives <= 1 && pswarm_swarmv -> delta < pswarm_optionsv -> tol) {
    pswarm_outroutine(PSWARM_STATUS_MSG | PSWARM_STATUS_EXIT, "One particle left and step width below tolerance", pswarm_optionsv, pswarm_swarmv);
    return PSWARM_STATUS_EXIT;
  }

  return PSWARM_STATUS_OK;
} 



int pswarm_iter(pswarm_options *opt, pswarm_swarm *pop)
{
  int i,j, success; /* control variables */
  double *vectorx, *vectorfx, normtmp, normtmp2, weight; /* dummy containters */
  poll_container *poll_containerv;

  poll_containerv = (poll_container *) pop -> adar;

  /* while(pop -> iter < opt -> maxiter && pop -> objfunctions < opt -> maxf) { */

    /* if(poll_containerv -> maxnormv<opt -> tol && pop -> delta<opt -> tol){ */
    /*   if(opt -> IPrint>=0) */
    /* 	printf("\n\nStopping due to velocity and tolerance\n\n"); */
    /*   break; */
    /* } */

    /* if(poll_containerv -> actives<=1 && pop -> delta<opt -> tol){ */
    /*   if(opt -> IPrint>=0) */
    /* 	printf("\n\nStopping due to single particle and tolerance\n\n"); */
    /*   break; */
    /* } */

    pop -> iter++;

    /*		check_feasible_pop(n, opt -> s, lincons, &pop, opt -> lb, ub, A, b); */

    success=0; /* controls if pop -> gbest was updated with success */

    if(opt -> vectorized){ /* call objf once with all the points */

      vectorx=(double *)pswarm_malloc(opt -> s*opt -> n*sizeof(double)); /* we could avoid to wast memory on points not active */
      vectorfx=(double *)pswarm_malloc(opt -> s*sizeof(double));

      for(j=0,i=0;i<opt -> s;i++)
	if(pop -> active[i] && feasible_p(opt -> n, &(pop -> x[i*opt -> n]), opt -> lb, opt -> ub)){
	  memcpy(&vectorx[j*opt -> n],&(pop -> x[i*opt -> n]),opt -> n*sizeof(double));
	  j++;
	}

      objfn(opt -> fun, opt -> adar, opt -> n, j, vectorx, opt -> lb, opt -> ub, vectorfx);
      pop -> objfunctions+=j;

      for(j=0,i=0;i<opt -> s;i++) /* we could avoid a second cycle if we saved the indices */
	if(pop -> active[i] && feasible_p(opt -> n, &(pop -> x[i*opt -> n]), opt -> lb, opt -> ub)){
	  pop -> fx[i]=vectorfx[j];
	  j++;
	}

      free(vectorx);
      free(vectorfx);
    } else {

      for(i=0;i<opt -> s;i++){
	if(pop -> active[i]){

	  if(feasible_p(opt -> n, &(pop -> x[i*opt -> n]), opt -> lb, opt -> ub)){
	    objfn(opt -> fun, opt -> adar, opt -> n, 1, &(pop -> x[i*opt -> n]), opt -> lb, opt -> ub, &(pop -> fx[i]));
	    pop -> objfunctions++;
	  } else {
	    pop -> fx[i]=+DBL_MAX;
	  }
	}
      }
    }


    /* process=0; */
    for(i=0;i<opt -> s;i++){
      if(pop -> active[i]){
	if(pop -> fy[i]>pop -> fx[i]){      /* progress obtained */
	  pop -> fy[i]=pop -> fx[i];           /* Search step */
	  memcpy(&(pop -> y[i*opt -> n]), &(pop -> x[i*opt -> n]), opt -> n*sizeof(double));

	  /* check if a new leader is available or if a progress was
	     obtained on the leader */
	  if(pop -> fy[pop -> gbest]>pop -> fy[i] || pop -> gbest==i){
	    pop -> gbest=i; /* global best indice */
	    success=1; /* success for leader obtained */
	    poll_containerv -> last_success=NULL; /* reset successful direction on poll step */
	  }
	}
      }
    }



    if(!success){ /* no success for the pop -> gbest particle in one generation, so performe a poll step */
      if(pop -> delta >= opt -> tol){

	/* performe a poll step, update y and delta */
	/* pollstep(n, lincons, pop -> gbest, objf, fun , opt -> adar, opt, &pop, &stats, opt -> lb, ub, A, b, &last_success); */
	pollstep(pop -> gbest, opt, pop);

	pop -> pollsteps++;
	poll_containerv -> iterunsuc=0;
      } else {
	poll_containerv -> iterunsuc++;
	/* printf("Consecutive unsuccesseful iterations %d\n", iterunsuc); */
      }
    } else { /* success for the pop -> gbest particle */
      poll_containerv -> iterunsuc=0;
      /* printf("Success in Swarm iteration\n"); */
      /* increase delta */
      if(pop -> delta<opt -> delta){
	pop -> delta*=opt -> idelta;
	/*  printf("Increasing delta in search step\n"); */
      }
      /* allow at least one more iteration */
      if(pop -> delta<opt -> tol)
	pop -> delta=2*opt -> tol;
    }


    /* inertia factor is a linear interpolation from iweight to fweight */
    if (pop -> iter < opt -> iterfweight)
      weight = opt -> iweight - (opt -> iweight-opt -> fweight)*((double)(pop -> iter))/((double) opt -> iterfweight);
    else
      weight = opt -> fweight;

    /* printf("Before step computation\n"); */
    /* check_feasible_pop(n, opt -> s, lincons, &pop, lb, ub, A, b); */
    /* printf("Before step computation\n"); */

    for(i=0;i<opt -> s;i++){ /* for each particle */

      if(pop -> active[i]){ /* active particle */
	/* update velocity */
	for(j=0;j<opt -> n;j++){
	  pop -> v[i*opt -> n+j] =
#if SYS_RANDOM==1
	    projection(weight*pop -> v[i*opt -> n+j]+
		       opt -> mu*(rand()/(RAND_MAX+1.0))*(pop -> y[i*opt -> n+j]-pop -> x[i*opt -> n+j])
		       +opt -> nu*(rand()/(RAND_MAX+1.0))*(pop -> y[pop -> gbest*opt -> n+j]-pop -> x[i*opt -> n+j]),
		       -poll_containerv -> maxv[j],poll_containerv -> maxv[j]);
#else
	  projection(weight*pop -> v[i*opt -> n+j]+
		     opt -> mu*(randflt (&(poll_containerv -> rand_seed)))*(pop -> y[i*opt -> n+j]-pop -> x[i*opt -> n+j])
		     +opt -> nu*(randflt (&(poll_containerv -> rand_seed)))*(pop -> y[pop -> gbest*opt -> n+j]-pop -> x[i*opt -> n+j]),
		     -poll_containerv -> maxv[j],poll_containerv -> maxv[j]);
#endif

	  poll_containerv -> alphamax[j]=1.0; /* a step no longer than 1 */
	}
		

	for(j=0;j<opt -> n;j++){
	  if(pop -> v[i*opt -> n+j]<0.0){
	    if(poll_containerv -> alphamax[j]>(opt -> lb[j]-pop -> x[i*opt -> n+j])/pop -> v[i*opt -> n+j])
	      poll_containerv -> alphamax[j]=(opt -> lb[j]-pop -> x[i*opt -> n+j])/pop -> v[i*opt -> n+j];
	  }
	  if(pop -> v[i*opt -> n+j]>0.0){
	    if(poll_containerv -> alphamax[j]>(opt -> ub[j]-pop -> x[i*opt -> n+j])/pop -> v[i*opt -> n+j])
	      poll_containerv -> alphamax[j]=(opt -> ub[j]-pop -> x[i*opt -> n+j])/pop -> v[i*opt -> n+j];
	  }
	}

	for(j=0;j<opt -> n;j++)
	  if(poll_containerv -> alphamax[j]<0.0){
	    /* printf("Também não deveria de acontecer\n"); */
	    poll_containerv -> alphamax[j]=0.0;
	  }

	/* update particle and check bound limits */
	for(j=0;j<opt -> n;j++){
	  pop -> x[i*opt -> n+j]=projection(pop -> x[i*opt -> n+j]+poll_containerv -> alphamax[j]*pop -> v[i*opt -> n+j],opt -> lb[j],opt -> ub[j]);
	}
      }
    }

    /* printf("After step computation\n"); */
    /* check_feasible_pop(n, opt -> s, lincons, &pop, lb, ub, A, b); */
    /* printf("After step computation\n"); */


    /* check for all norm velocities to zero */

    /* first for pop -> gbest */
    normtmp=0.0;
    for(j=0;j<opt -> n;j++)
      normtmp+=pow(pop -> v[pop -> gbest*opt -> n+j],2.0);
    pop -> maxnormv=sqrt(normtmp);


    /* remove particle close to pop -> gbest and compute maximum velocity */
    pop -> actives=0;
    for(i=0;i<opt -> s;i++){ /* for each particle */
      if(pop -> active[i] && i!=pop -> gbest){ /* active particle and not the pop -> gbest */
	normtmp=0.0; normtmp2=0.0;
	for(j=0;j<opt -> n;j++){
	  normtmp+=pow(pop -> y[i*opt -> n+j]-pop -> y[pop -> gbest*opt -> n+j],2.0);
	  normtmp2+=pow(pop -> v[i*opt -> n+j],2.0);
	}
	normtmp=sqrt(normtmp); normtmp2=sqrt(normtmp2);
	if(normtmp<opt -> delta && normtmp2<opt -> delta){ /* (fabs((double)(iter-(opt -> maxiter)/100.0)))){ */
	  pop -> active[i]--; /* remove particle */
	  /* printf("Particle %d inactive iter=%d\n", i, iter); */
	} else { /* particle not removed, so account for pop -> maxnormv */
	  if(pop -> maxnormv<normtmp2)
	    pop -> maxnormv=normtmp2;
	}
      }
      if(pop -> active[i])
	pop -> actives++; /* account in poll_containerv -> actives */
    }

    /*    printf("Maximum velocity norm: %f\n", pop -> maxnormv); */

    /* printf("%d;%.20f\n",stats.objfunctions,pop -> fy[pop -> gbest]); */

  /* } */
    memcpy(pop -> sol, &(pop -> y[pop -> gbest*opt -> n]),opt -> n*sizeof(double));

  return PSWARM_STATUS_OK;
}

static int init_pop(pswarm_swarm *pop, pswarm_options *opt, int ninitials, double *initials)
{
  int i, j;
  double normtmp = 10.0; /* should never be used by default */
  int feas, accepted;
  poll_container *poll_containerv;

  poll_containerv = (poll_container *) pop -> adar;

  /*   The user can provide an initial guess to include in the initial swarm
       A reset in the population can also proposed a fixed number of point
       to be in the next pswarm_swarm */

  /* Do simple check in the simple bound limits */

  if(ninitials > opt -> s){
    printf("Populations should be increased to %d particles\n", ninitials);
    ninitials = opt -> s;
  }

  accepted = 0;

  if(ninitials && initials){
    for(i=0; i < ninitials; i++){
      /* printf("Init %d: %f\n", i, objfun(n,&initials[i*n])); */
      for(j = 0; j < opt -> n; j++)
	pop -> x[accepted*opt -> n+j]=projection(initials[i*opt -> n+j],opt -> lb[j],opt -> ub[j]);
      feas=1;

      if(feas){
	pop->fy[accepted]=+DBL_MAX*10; /* in first iteration, y will be set */
	pop->active[accepted]=1;   /* chances to be near the pop -> gbest */
	accepted++;
      }
    }
  }

  if(accepted){
    /* compute standard deviation of first particle */
    normtmp = 0.0;
    for(j=0;j < opt -> n;j++)
      normtmp+=pow(pop->x[j],2.0);
    if(normtmp < 10)
      normtmp = opt -> blim;
  }

  if(accepted<ninitials)
    printf("Only %d particles were linear feasible after projection\n", accepted);

  for(i=accepted; i < opt -> s;i++){ /* for all remaining particle   */

    for(j=0; j < opt -> n;j++){   /* for all dimensions */
      if(opt -> lb[j]>-DBL_MAX && opt -> ub[j]<DBL_MAX){
#if SYS_RANDOM==1
	pop->x[i*opt -> n+j]=(rand()/(RAND_MAX+1.0))*(opt -> ub[j]-opt -> lb[j])+opt -> lb[j];
#else
	pop->x[i*opt -> n+j]=(randflt (&(poll_containerv->rand_seed)))*(opt -> ub[j]-opt -> lb[j])+opt -> lb[j];
#endif
      } else {
	if(opt -> lb[j]<=-DBL_MAX && opt -> ub[j]>=DBL_MAX){ /* both limits infinite */
#if SYS_RANDOM==1
	  pop->x[i*opt -> n+j]=2*(rand()/(RAND_MAX+1.0)-0.5)*normtmp+initials[j];
#else
	  pop->x[i*opt -> n+j]=2*(randflt (&(poll_containerv -> rand_seed))-0.5)*normtmp+initials[j];
#endif
	} else {
	  if(ninitials<=0){
	    printf("No finite bounds on all variables and no initial guess provided\n");
	    printf("Unable to obtain initial population\n");
	    return 1;
	  }
	  if(opt -> lb[j]<=-DBL_MAX){ /* lower infinite and upper finite */
#if SYS_RANDOM==1
	    pop->x[i*opt -> n+j]=2*(rand()/(RAND_MAX+1.0)-0.5)*(opt -> ub[j]-initials[j])+initials[j];
#else
	    pop->x[i*opt -> n+j]=2*(randflt (&(poll_containerv -> rand_seed))-0.5)*(opt -> ub[j]-initials[j])+initials[j];
#endif
	  } else { /* upper infinite and lower finite */
#if SYS_RANDOM==1
	    pop->x[i*opt -> n+j]=2*(rand()/(RAND_MAX+1.0)-0.5)*(initials[j]-opt -> lb[j])+initials[j];
#else
	    pop->x[i*opt -> n+j]=2*(randflt (&(poll_containerv -> rand_seed))-0.5)*(initials[j]-opt -> lb[j])+initials[j];
#endif
	  }
	}

      }
    }


    pop->fy[i]=+DBL_MAX*10; /* in first iteration, y will be set */
    pop->active[i]=1;   /* chances to be near the pop -> gbest */
  }


  pop -> delta = opt -> delta;

  /* initialize  velocity */
  memset(pop->v, 0, opt -> s*opt -> n*sizeof(double));


  /* print_pop(n, 0, s, pop); */
  return 0;
}


static double projection(double xi, double lbi, double ubi)
{
  if(xi<lbi){
    return lbi;
  }
  if(xi>ubi){
    return ubi;
  }
  return xi;
}


/* print the best of each particle in a population */
/* static void print_pop(int n, int pop -> gbest, int s, struct pswarm_swarm *pop) */
/* { */
/*   int i, j, inactive; */

/*   printf("Printing the best so far for each particle\n"); */

/*   inactive=0; */
/*   for(i=0;i<s;i++){                            */ /* for each particle */	
/*     if(pop->active[i]){                        */ /* active particle */	
/*       printf("x(%d)=[", i); */		                               
/*       for(j=0;j<n-1;j++)                       */ /* for each coordinate */	
/* 	printf("%.4f,", pop->x[i*n+j]); */	                               
/*       printf("%.4f];\n", pop->x[i*n+n-1]); */                               
/*     } else { */				                               
/*       inactive++; */				                               
/*     } */					                               
/*   } */					                               
						                               
/*   printf("%d inactive particles\n", inactive); */ 				
						                               
/*   printf("\n The very best\n"); */		                               
/*   printf("p(%d)=[", pop -> gbest); */		                               
/*   for(j=0;j<n-1;j++)                           */  /* for each coordinate */
/*     printf("%.10f,", pop->y[pop -> gbest*n+j]); */
/*   printf("%.10f];\n", pop->y[pop -> gbest*n+n-1]); */
/*   printf("f(%d)=%.10f\n", pop -> gbest, pop->fy[pop -> gbest]); */


/* } */


/* write the best pswarm_swarm particle */
/* static void print_best(int n, int pop -> gbest, int s, struct pswarm_swarm *pop, int pop -> iter) */
/* { */
/* 	int i, nactive; */
/* 	FILE *file; */


/* 	if(pop -> iter==1)file=fopen("results.txt", "w"); */
/* 	else file=fopen("results.txt", "a"); */

/* 	nactive=0; */
/* 	for(i=0;i<s;i++) */
/* 		if(pop->active[i])nactive++; */

/* 	if(!file)return; */

/* 	fprintf(file,"x=["); */
/* 	for(i=0;i<n;i++){  */
/* 		fprintf(file,"%.8f,", pop->y[pop -> gbest*n+i]); */
/* 	} */
/* 	fprintf(file,"]  fx=%f\n", pop->fy[pop -> gbest]); */

/* 	fprintf(file,"Nobj=%d  Npoll=%d  Nsucpoll=%d Active=%d\n", pop -> objfunctions, pop -> pollsteps, pop -> sucpollsteps, nactive); */

/* 	fclose(file); */

/* } */


/* write array to stdout */
/* static void print_array(int n, double *x) */
/* { */
/* 	int i; */


/* 	printf("=["); */
/* 	for(i=0;i<n-1;i++){ */
/* 		printf("%.4f,", x[i]); */
/* 	} */
/* 	printf("%.4f]\n",x[i]); */
/* } */




#if SYS_RANDOM!=1


/* #define LONG_INT long */

/*
The next two functions, myrand and randflt, were copied from
user.c in ASA.
*/

#define MULT ((long) 25173)
#define MOD ((long) 65536)
#define INCR ((long) 13849)
#define FMOD ((double) 65536.0)

/***********************************************************************
* double myrand - returns random number between 0 and 1
*   This routine returns the random number generator between 0 and 1
***********************************************************************/

static double myrand (long *rand_seed)
{
#if TRUE            /* (change to FALSE for alternative RNG) */
	*rand_seed = (long) ((MULT * (*rand_seed) + INCR) % MOD);
	printf("ey\n");
	return ((double) (*rand_seed) / FMOD);
#else
	/* See "Random Number Generators: Good Ones Are Hard To Find,"
	Park & Miller, CACM 31 (10) (October 1988) pp. 1192-1201.
	***********************************************************
	THIS IMPLEMENTATION REQUIRES AT LEAST 32 BIT INTEGERS
	*********************************************************** */
#define _A_MULTIPLIER  16807L
#define _M_MODULUS     2147483647L  /* (2**31)-1 */
#define _Q_QUOTIENT    127773L  /* 2147483647 / 16807 */
#define _R_REMAINDER   2836L    /* 2147483647 % 16807 */
	long lo;
	long hi;
	long test;

	hi = *rand_seed / _Q_QUOTIENT;
	lo = *rand_seed % _Q_QUOTIENT;
	test = _A_MULTIPLIER * lo - _R_REMAINDER * hi;
	if (test > 0)
	{
		*rand_seed = test;
	}
	else
	{
		*rand_seed = test + _M_MODULUS;
	}
	return ((double) *rand_seed / _M_MODULUS);
#endif /* alternative RNG */
}

/***********************************************************************
* double randflt
***********************************************************************/

static double
randflt (long *rand_seed)
{
	return (resettable_randflt (rand_seed, 0));
}

/***********************************************************************
* double resettable_randflt
***********************************************************************/
static double
resettable_randflt (long * rand_seed, int reset)

/* shuffles random numbers in random_array[SHUFFLE] array */

{

	/* This RNG is a modified algorithm of that presented in
	* %A K. Binder
	* %A D. Stauffer
	* %T A simple introduction to Monte Carlo simulations and some
	*    specialized topics
	* %B Applications of the Monte Carlo Method in statistical physics
	* %E K. Binder
	* %I Springer-Verlag
	* %C Berlin
	* %D 1985
	* %P 1-36
	* where it is stated that such algorithms have been found to be
	* quite satisfactory in many statistical physics applications. */

	double rranf;
	unsigned kranf;
	int n;
	static int randflt_initial_flag = 0;
	long initial_seed;
	static double random_array[SHUFFLE];  /* random variables */

	if (*rand_seed < 0)
		*rand_seed = -*rand_seed;

	if ((randflt_initial_flag == 0) || reset)
	{
		initial_seed = *rand_seed;

		for (n = 0; n < SHUFFLE; ++n)
			random_array[n] = myrand (&initial_seed);

		randflt_initial_flag = 1;

		for (n = 0; n < 1000; ++n)    /* warm up random generator */
			rranf = randflt (&initial_seed);

		rranf = randflt (rand_seed);

		return (rranf);
	}

	kranf = (unsigned) (myrand (rand_seed) * SHUFFLE) % SHUFFLE;
	rranf = *(random_array + kranf);
	*(random_array + kranf) = myrand (rand_seed);

	return (rranf);
}
#endif

/* check is particle is feasible */
int feasible_p(int n, double *x, double *lb, double *ub)
{
	int j;

	/* check for bound feasibility */
	for(j=0;j<n;j++)
		if(x[j]>ub[j] || x[j]<lb[j])
			return 0;

	return 1;
}


void *pswarm_malloc(size_t size)
{
	void *pointer;
	pointer=malloc(size);
	return pointer;
}



/***************************************/

/***************************************/
/***************************************/
/* Fromerly pattern.c, body start */
/***************************************/

/* Destructor of poll_vector ll */
static int poll_vector_destr(poll_vector *poll_vectorv)
{
  poll_vector *current;

  /* Something to do */
  while (poll_vectorv) {
    current = poll_vectorv;
    poll_vectorv = poll_vectorv -> next;
    free(current);
  }

  return 0;
}



/* Constructor of poll_container */
static poll_container *poll_container_const()
{
  poll_container *poll_container_constv;

  if (!(poll_container_constv = (poll_container *) malloc(sizeof(poll_container))))
    return NULL;

  poll_container_constv -> PVectors = NULL;
  poll_container_constv -> D = NULL;
  poll_container_constv -> last_success = NULL;
  poll_container_constv -> maxv = NULL;
  poll_container_constv -> alphamax = NULL;

  return poll_container_constv;
}

/* Destructor of poll_container */
static int poll_container_destr(poll_container *poll_containerv)
{
  if ((poll_containerv)) {
    /* PVectors is never allocated */
    /* poll_vector_destr(poll_containerv -> PVectors); */
    if ((poll_containerv -> D))
      poll_vector_destr(poll_containerv -> D);
    /* poll_vector_destr(poll_containerv -> last_D); */
    if ((poll_containerv -> maxv))
      free(poll_containerv -> maxv);
    if ((poll_containerv -> alphamax))
      free(poll_containerv -> alphamax);

    free(poll_containerv);
  }

  return 0;
}



/* performe a poll step for y[pi] position */
static void pollstep(int pi, pswarm_options *opt, pswarm_swarm *pop)
{
  int i,j;
  double *poll_point, fx, minfx;
  struct poll_vector *tmp, *minvector;
  double *vectorx, *vectorfx;
  poll_container *poll_containerv;

  poll_containerv = (poll_container *) pop -> adar;

  poll_point=pswarm_malloc(opt -> n*sizeof(double));
  
  if(!poll_point){
    printf("Unable to alocate memory in pattern.c\n");
    fprintf(stderr,"this?");
    exit(1);
  }

  
  /* Use D */
    poll_containerv -> PVectors = poll_containerv -> D;
  
  if(opt -> vectorized){
    
    /* account for the number of trial points */
    tmp = poll_containerv -> PVectors;
    j=0;
    while(tmp){
      j++;
      tmp=tmp->next;
    }
    
    
    /* allocate memory, we may be allocating more memory than necessary */
    vectorx=(double *)pswarm_malloc(j*opt -> n*sizeof(double));
    vectorfx=(double *)pswarm_malloc(j*sizeof(double));
    
    /* compute trial points */
    tmp = poll_containerv -> PVectors;
    j=0;
    while(tmp){
      for(i=0;i<opt -> n;i++)
	vectorx[j*opt -> n+i]=pop -> y[pi*opt -> n+i]+pop -> delta*tmp->vector[i];
      
      /* is it linear feasible ? */
      if(feasible_p(opt -> n, &vectorx[j*opt -> n], opt -> lb, opt -> ub))
	j++; /* yes */
      
      tmp=tmp->next;
    }
    
    /* printf("And %d are feasible\n", j); */
    
    if(j>0){
      objfn(opt -> fun, opt -> adar, opt -> n, j, vectorx, opt -> lb, opt -> ub, vectorfx);
      pop -> objfunctions+=j;
    }
    
    minvector=NULL;
    minfx=DBL_MAX;
    j=0;
    tmp = poll_containerv -> PVectors;
    while(tmp){
      for(i=0;i<opt -> n;i++)
	poll_point[i]=pop -> y[pi*opt -> n+i]+pop -> delta*tmp->vector[i];
      
      if(feasible_p(opt -> n, poll_point, opt -> lb, opt -> ub)){
	if(minfx>vectorfx[j]){
	  minfx=vectorfx[j];
	  minvector=tmp; /* don't break. We are in a non oportunistic version */
	}
	j++;
      }
      tmp=tmp->next;
    }
    
    free(vectorx);
    free(vectorfx);
    
  } else {
    tmp = poll_containerv -> PVectors;
    minvector=NULL;
    minfx=DBL_MAX;
    while(tmp){
      for(i=0;i<opt -> n;i++)
	poll_point[i]=pop -> y[pi*opt -> n+i]+pop -> delta*tmp->vector[i];
      
      if(feasible_p(opt -> n, poll_point, opt -> lb, opt -> ub)){
	objfn(opt -> fun, opt -> adar, opt -> n, 1, poll_point, opt -> lb, opt -> ub, &fx);
	pop -> objfunctions++;
	if(minfx>fx){
	  minfx=fx;
	  minvector=tmp; /* if oportunistic then break if fx < leader fx */
	  if(pop -> fy[pi]>minfx)
	    break;
	}
      }
      tmp=tmp->next;
    }
  }
  
  
  if(pop -> fy[pi]>minfx){ /* a successful poll point */
    pop -> sucpollsteps++;
    for(i=0;i<opt -> n;i++)
      pop -> y[pi*opt -> n+i]=pop -> y[pi*opt -> n+i]+pop -> delta*minvector->vector[i];
    pop -> fy[pi]=minfx;
    if(poll_containerv -> last_success==minvector){ /* last success vector twice, increase delta */
      pop -> delta*=opt -> idelta;
      /* printf("Increasing delta in poll step %f\n", pop -> delta); */
    } else { /* last success vector is different */
      poll_containerv -> last_success=minvector;
    }
  } else {
    pop -> delta*=opt -> ddelta;
    /* printf("Decreasing delta\n"); */
    poll_containerv -> last_success=NULL;
  }
  
  
  /* free variables */
  /* GJ: This is a joke, free variables */
  free(poll_point);
  
  poll_containerv -> PVectors=NULL;
  
}


static void init_pattern(int n, pswarm_options *opt, poll_container *poll_containerv)
{
  
  init_D(n, opt, poll_containerv);
  /*    print_D(n); */

  if ((poll_containerv -> PVectors))
    free(poll_containerv -> PVectors);
  poll_containerv -> PVectors = NULL;
  
}


static void init_D(int n, pswarm_options *opt, poll_container *poll_containerv)
{
  int i;
  struct poll_vector *tmp1, *tmp2;
  
  
  if(poll_containerv -> D) /* D already initialized */
    return;
  
  switch(opt -> pollbasis){
  default:
    printf("\n Poll basis order not defined\n");
    printf("\n Using I -I order\n");
    /* case N2: */ /* I -I */
  case 0:
    poll_containerv -> D = pswarm_malloc(sizeof(struct poll_vector));
    poll_containerv -> D -> next = NULL;
    poll_containerv -> D -> vector = pswarm_malloc(n*sizeof(double));
    memset(poll_containerv -> D -> vector, 0, n*sizeof(double));
    poll_containerv -> D->vector[0]=+1.0;
    tmp2 = poll_containerv -> D;
    for(i=1;i<2*n;i++){
      tmp1=pswarm_malloc(sizeof(struct poll_vector));
      tmp2->next=tmp1;
      tmp1->vector=pswarm_malloc(n*sizeof(double));
      memset(tmp1->vector, 0, n*sizeof(double)); /* clear memory */
      if(i<n)
	tmp1->vector[i]=+1.0;
      else
	tmp1->vector[i-n]=-1.0;
      tmp1->next=NULL ;
      tmp2=tmp1;
    }
    /* poll_containerv -> last_D = tmp2; */
  }
}


/* static void print_D(int n, poll_container *poll_containerv) */
/* { */
/*   struct poll_vector *tmp; */
  
/*   tmp = poll_containerv -> D; */
/*   while(tmp){ */
/*     print_poll_vector(n, tmp->vector); */
/*     tmp=tmp->next; */
/*   } */
  
/* } */


/* static void print_poll_vector(int n, double *vector) */
/* { */
/*   int i; */
  
  
/*   if(!vector) */
/*     return; */
  
/*   printf("D=(%.2f, ", vector[0]); */
/*   for(i=1;i<n;i++) */
/*     printf("%.2f ", vector[i]); */
/*   printf(")\n"); */
  
/* } */

/***************************************/
/* Fromerly pattern.c, body end */
/***************************************/
/***************************************/

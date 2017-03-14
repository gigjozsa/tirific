/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file identifyers.h
   @brief Identifyers for TiRiFiC

   Symbolic constants that identify an entity (parameter group,
   variable type) in TiRiFiC. Parameter groups for now.
   
*/
/* ------------------------------------------------------------ */



/* Include guard */
#ifndef TIRIFIC_IDENTIFYERS_H
#define TIRIFIC_IDENTIFYERS_H


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
/**
   @define NPARAMS
   @brief The number of parameters for every ring in the first disk
*/
/* ------------------------------------------------------------ */
#define NPARAMS 79



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define NDPARAMS
   @brief The number of parameters for each ring for all disks
*/
/* ------------------------------------------------------------ */
/* ndisk construction */
#define NDPARAMS 78



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define NSPARAMS
   @brief The number of global parameters for all rings

*/
/* ------------------------------------------------------------ */
#define NSPARAMS 1



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define NPMNDPARAMS
   @brief NPARAMS - NDPARAMS
*/
/* ------------------------------------------------------------ */
#define NSSDPARAMS (NPARAMS-NDPARAMS)


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define PRPARAMS
   @brief NPARAMS - NDPARAMS
*/
/* ------------------------------------------------------------ */
#define PRPARAMS (NPARAMS-NDPARAMS-1)


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*

ndisks      : (constant) number of disks

NPARAMS     : (constant) number of radially dependent parameters of first
disk (including global, radially dependent parameters, at the moment
only the radii)

NDPARAMS    : (constant) number of parameters shared by all disks

NSSDPARAMS : (constant) NPARAMS - NDPARAMS

PRPARAMS    : (constant) NPARAMS - NDPARAMS - 1

XXX         : (constant) identifyer of parameter XXX, starting with 1

PXXX        : (constant) identifyer of parameter XXX, starting with 0

disk        : (variable) disk number (ranging from 0 to ndisks)

nur         : (variable) number of rings

ring        : (variable) ring number (starting with 0)

A parameter starting from 0 is addressed by (disk = 0 for RADI):
(NPARAMS - NDPARAMS + disk*NDPARAMS - 1 + PXXX)
(    NSSDPARAMS     + disk*NDPARAMS - 1 + PXXX)
(     PRPARAMS      + disk*NDPARAMS     + PXXX)

A parameter starting from 1 is addressed by (disk = 0 for RADI):
(NPARAMS - NDPARAMS + disk*NDPARAMS - 1 + XXX)
(    NSSDPARAMS     + disk*NDPARAMS - 1 + XXX)
(     PRPARAMS      + disk*NDPARAMS     + XXX)

PCONDISP (starting from 0) has the number:
NPARAMS + (ndisks - 1)*NDPARAMS

CONDISP (starting from 1) has the number:
NPARAMS + (ndisks - 1)*NDPARAMS + 1

A certain parameter with ring number ring (starting with 0) is addressed by (disk = 0 for RADI):

(NPARAMS - NDPARAMS + disk*NDPARAMS - 1 + PXXX)*nur + ring    =
(    NSSDPARAMS     + disk*NDPARAMS - 1 + PXXX)*nur + ring    =
(     PRPARAMS      + disk*NDPARAMS     + PXXX)*nur + ring

The total number of parameters  (assuming that the only singular parameter is CONDISP) 
excluding the singular parameter condisp that are in the rpm list is:
NPARAMS + (ndisks - 1)*NDPARAMS + NSPARAMS =
CONDISP

The total number of parameters (assuming that the only singular parameter is CONDISP) that are in the rpm list is:
nur*(NPARAMS + (ndisks - 1)*NDPARAMS) + NSPARAMS

To reduce a parameter identifyer to the appropriate (same) parameter of the first disk (starting with 1):
par = (par-NSSDPARAMS-1)%NDPARAMS + NSSDPARAMS + 1;

To reduce a parameter identifyer to the appropriate (same) parameter of the first disk (starting with 0):
par = (par-NSSDPARAMS)%NDPARAMS + NSSDPARAMS;

 */
/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define PRADI @brief A parameter for each ring, but not CONDISP is
   adressed by (ndisk*NDPARAMS+PXXXX)*rpm -> nur+ringnr, CONDISP is
   addressed by ((ndisks-1)*NDPARAMS+PCONDISP)*rpm -> nur
*/
/* ------------------------------------------------------------ */
#define PRADI    0
#define PVROT    1
#define PVRAD    2
#define PVVER	 3
#define PDVRO    4
#define PDVRA	 5
#define PDVVE	 6
#define PZDRO    7 
#define PZDRA	 8 
#define PZDVE	 9 
#define PZ0      10
#define PSBR     11
#define PSM1A    12
#define PSM1P    13
#define PSM2A    14
#define PSM2P    15
#define PSM3A    16
#define PSM3P    17
#define PSM4A    18
#define PSM4P    19
#define PGA1A    20 
#define PGA1P    21
#define PGA1D    22
#define PGA2A    23 
#define PGA2P    24
#define PGA2D    25
#define PGA3A    26 
#define PGA3P    27
#define PGA3D    28
#define PGA4A    29 
#define PGA4P    30
#define PGA4D	 31
#define PAZ1P	 32
#define PAZ1W	 33
#define PAZ2P	 34
#define PAZ2W    35
#define PINCL    36
#define PPA      37
#define PXPOS    38
#define PYPOS    39
#define PVSYS    40
#define PSDIS    41
#define PCLNR    42
#define PVM0A    43
#define PVM1A    44
#define PVM1P    45
#define PVM2A    46
#define PVM2P    47
#define PVM3A    48
#define PVM3P    49
#define PVM4A    50
#define PVM4P    51
#define PWM0A    52
#define PWM1A    53
#define PWM1P    54
#define PWM2A    55
#define PWM2P    56
#define PWM3A    57
#define PWM3P    58
#define PWM4A    59
#define PWM4P    60
#define PLS0     61
#define PLC0     62
#define PRO1A    63
#define PRO1P    64
#define PRO2A    65
#define PRO2P    66
#define PRO3A    67
#define PRO3P    68
#define PRO4A    69
#define PRO4P    70
#define PRA1A    71
#define PRA1P    72
#define PRA2A    73
#define PRA2P    74
#define PRA3A    75
#define PRA3P    76
#define PRA4A    77
#define PRA4P    78

/* #define PCONDISP (NPARAMS + (ndisks - 1)*NDPARAMS) */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define RADI
   @brief Identification
*/
/* ------------------------------------------------------------ */
#define RADI    1 
#define VROT    2 
#define VRAD    3 
#define VVER    4 
#define DVRO    5 
#define DVRA    6 
#define DVVE    7 
#define ZDRO    8 
#define ZDRA    9 
#define ZDVE    10
#define Z0      11
#define SBR     12
#define SM1A    13
#define SM1P    14
#define SM2A    15
#define SM2P    16
#define SM3A    17
#define SM3P    18
#define SM4A    19
#define SM4P    20
#define GA1A    21
#define GA1P    22
#define GA1D    23
#define GA2A    24
#define GA2P    25
#define GA2D    26
#define GA3A    27
#define GA3P    28
#define GA3D    29
#define GA4A    30
#define GA4P    31
#define GA4D    32
#define AZ1P    33
#define AZ1W    34
#define AZ2P    35
#define AZ2W    36
#define INCL    37
#define PA      38
#define XPOS    39
#define YPOS    40
#define VSYS    41
#define SDIS    42
#define CLNR    43
#define VM0A    44
#define VM1A    45
#define VM1P    46
#define VM2A    47
#define VM2P    48
#define VM3A    49   
#define VM3P    50   
#define VM4A    51   
#define VM4P    52   
#define WM0A    53   
#define WM1A    54   
#define WM1P    55   
#define WM2A    56   
#define WM2P    57 
#define WM3A    58 
#define WM3P    59 
#define WM4A    60 
#define WM4P    61
#define LS0     62
#define LC0     63
#define RO1A    64
#define RO1P    65
#define RO2A    66
#define RO2P    67
#define RO3A    68
#define RO3P    69
#define RO4A    70
#define RO4P    71
#define RA1A    72
#define RA1P    73
#define RA2A    74
#define RA2P    75
#define RA3A    76
#define RA3P    77
#define RA4A    78
#define RA4P    79
/* #define CONDISP (NPARAMS + (ndisks - 1)*NDPARAMS + 1) */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define VERSION_NUMBER
   @brief Version number of this module
*/
/* ------------------------------------------------------------ */
#define VERSION_NUMBER 2.3.3


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define GOLDEN_SECTION
   @brief Identifyer for the GOLDEN_SECTION method
*/
/* ------------------------------------------------------------ */
#define GOLDEN_SECTION 1
#define METROPOLIS 0
#define GENFIT 2
#define GOLDEN_SECTION_ALT 2
#define SIMPLEX 3
#define PSWARM 4

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define INTERP_NUMBER
   @brief Number of interpolation methods
*/
/* ------------------------------------------------------------ */
#define INTERP_NUMBER 3

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define INTERP_LINEAR
   @brief Identifyer for linear interpolation
*/
/* ------------------------------------------------------------ */
#define INTERP_LINEAR MATHS_I_LINEAR
#define INTERP_CSPLINE MATHS_I_CSPLINE /* cubic natural spline */
#define INTERP_AKIMA MATHS_I_AKIMA /* Akima */

#define TIRIDENT_PARMAX   0
#define TIRIDENT_PARMIN   1
#define TIRIDENT_MODERATE 2
#define TIRIDENT_DELSTART 3
#define TIRIDENT_DELEND   4
#define TIRIDENT_ITESTART 5
#define TIRIDENT_ITEEND   6
#define TIRIDENT_SATDELT  7
#define TIRIDENT_MINDELTA 8

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
/* FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
   ------------------------------------------------------------ */

/* Include guard */
#endif

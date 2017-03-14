/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file tirific_defaults.c
   @brief Input defaults for TiRiFiC

    On the way to a more automated tirific, this module provides
   default values, for now starting with defaults for the fitting
   parameters.  Howto: For each of the parameters controlling the
   fitting process, PARMAX, PARMIN, MODERATE, DELSTART, DELEND,
   ITESTART, ITEEND, SATDELT, MINDELTA, a default is given. This
   default is overridden, if a parameter-specific default
   PARNAME_PARMAX ... is given.

*/
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <tirific_identifyers.h>
#include <tirific_defaults.h>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE MACROS */
/* ------------------------------------------------------------ */



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
/* FUNCTION CODE */
/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get default value (world coordinates) from parameter number */
int tirific_defaults_fitdefromident(int parnum, int nur, int ndisk, int fitmode, int fpar, double *value)
{
  int par;

  /* Identify the parameter number starting with 0 PPARAMETERNAME */
  par = ((parnum/nur)-NSSDPARAMS)%NDPARAMS + NSSDPARAMS;

  switch (fpar) {
  case TIRIDENT_PARMAX:
    switch (fitmode) {
    case PSWARM:
      switch (par) {
      case PVROT: *value = TIRIFIC_DEF_PARMAX_PSWARM_VROT; break;
      case PVRAD: *value = TIRIFIC_DEF_PARMAX_PSWARM_VRAD; break;
      case PVVER: *value = TIRIFIC_DEF_PARMAX_PSWARM_VVER; break;
      case PDVRO: *value = TIRIFIC_DEF_PARMAX_PSWARM_DVRO; break;
      case PDVRA: *value = TIRIFIC_DEF_PARMAX_PSWARM_DVRA; break;
      case PDVVE: *value = TIRIFIC_DEF_PARMAX_PSWARM_DVVE; break;
      case PZDRO: *value = TIRIFIC_DEF_PARMAX_PSWARM_ZDRO; break;
      case PZDRA: *value = TIRIFIC_DEF_PARMAX_PSWARM_ZDRA; break;
      case PZDVE: *value = TIRIFIC_DEF_PARMAX_PSWARM_ZDVE; break;
      case PZ0  : *value = TIRIFIC_DEF_PARMAX_PSWARM_Z0  ; break;
      case PSBR : *value = TIRIFIC_DEF_PARMAX_PSWARM_SBR ; break;
      case PSM1A: *value = TIRIFIC_DEF_PARMAX_PSWARM_SM1A; break;
      case PSM1P: *value = TIRIFIC_DEF_PARMAX_PSWARM_SM1P; break;
      case PSM2A: *value = TIRIFIC_DEF_PARMAX_PSWARM_SM2A; break;
      case PSM2P: *value = TIRIFIC_DEF_PARMAX_PSWARM_SM2P; break;
      case PSM3A: *value = TIRIFIC_DEF_PARMAX_PSWARM_SM3A; break;
      case PSM3P: *value = TIRIFIC_DEF_PARMAX_PSWARM_SM3P; break;
      case PSM4A: *value = TIRIFIC_DEF_PARMAX_PSWARM_SM4A; break;
      case PSM4P: *value = TIRIFIC_DEF_PARMAX_PSWARM_SM4P; break;
      case PGA1A: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA1A; break;
      case PGA1P: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA1P; break;
      case PGA1D: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA1D; break;
      case PGA2A: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA2A; break;
      case PGA2P: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA2P; break;
      case PGA2D: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA2D; break;
      case PGA3A: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA3A; break;
      case PGA3P: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA3P; break;
      case PGA3D: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA3D; break;
      case PGA4A: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA4A; break;
      case PGA4P: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA4P; break;
      case PGA4D: *value = TIRIFIC_DEF_PARMAX_PSWARM_GA4D; break;
      case PAZ1P: *value = TIRIFIC_DEF_PARMAX_PSWARM_AZ1P; break;
      case PAZ1W: *value = TIRIFIC_DEF_PARMAX_PSWARM_AZ1W; break;
      case PAZ2P: *value = TIRIFIC_DEF_PARMAX_PSWARM_AZ2P; break;
      case PAZ2W: *value = TIRIFIC_DEF_PARMAX_PSWARM_AZ2W; break;
      case PINCL: *value = TIRIFIC_DEF_PARMAX_PSWARM_INCL; break;
      case PPA  : *value = TIRIFIC_DEF_PARMAX_PSWARM_PA  ; break;
      case PXPOS: *value = TIRIFIC_DEF_PARMAX_PSWARM_XPOS; break;
      case PYPOS: *value = TIRIFIC_DEF_PARMAX_PSWARM_YPOS; break;
      case PVSYS: *value = TIRIFIC_DEF_PARMAX_PSWARM_VSYS; break;
      case PSDIS: *value = TIRIFIC_DEF_PARMAX_PSWARM_SDIS; break;
      case PCLNR: *value = TIRIFIC_DEF_PARMAX_PSWARM_CLNR; break;
      case PVM0A: *value = TIRIFIC_DEF_PARMAX_PSWARM_VM0A; break;
      case PVM1A: *value = TIRIFIC_DEF_PARMAX_PSWARM_VM1A; break;
      case PVM1P: *value = TIRIFIC_DEF_PARMAX_PSWARM_VM1P; break;
      case PVM2A: *value = TIRIFIC_DEF_PARMAX_PSWARM_VM2A; break;
      case PVM2P: *value = TIRIFIC_DEF_PARMAX_PSWARM_VM2P; break;
      case PVM3A: *value = TIRIFIC_DEF_PARMAX_PSWARM_VM3A; break;
      case PVM3P: *value = TIRIFIC_DEF_PARMAX_PSWARM_VM3P; break;
      case PVM4A: *value = TIRIFIC_DEF_PARMAX_PSWARM_VM4A; break;
      case PVM4P: *value = TIRIFIC_DEF_PARMAX_PSWARM_VM4P; break;
      case PWM0A: *value = TIRIFIC_DEF_PARMAX_PSWARM_WM0A; break;
      case PWM1A: *value = TIRIFIC_DEF_PARMAX_PSWARM_WM1A; break;
      case PWM1P: *value = TIRIFIC_DEF_PARMAX_PSWARM_WM1P; break;
      case PWM2A: *value = TIRIFIC_DEF_PARMAX_PSWARM_WM2A; break;
      case PWM2P: *value = TIRIFIC_DEF_PARMAX_PSWARM_WM2P; break;
      case PWM3A: *value = TIRIFIC_DEF_PARMAX_PSWARM_WM3A; break;
      case PWM3P: *value = TIRIFIC_DEF_PARMAX_PSWARM_WM3P; break;
      case PWM4A: *value = TIRIFIC_DEF_PARMAX_PSWARM_WM4A; break;
      case PWM4P: *value = TIRIFIC_DEF_PARMAX_PSWARM_WM4P; break;
      case PLS0 : *value = TIRIFIC_DEF_PARMAX_PSWARM_LS0 ; break;
      case PLC0 : *value = TIRIFIC_DEF_PARMAX_PSWARM_LC0 ; break;
      case PRO1A: *value = TIRIFIC_DEF_PARMAX_PSWARM_RO1A; break;
      case PRO1P: *value = TIRIFIC_DEF_PARMAX_PSWARM_RO1P; break;
      case PRO2A: *value = TIRIFIC_DEF_PARMAX_PSWARM_RO2A; break;
      case PRO2P: *value = TIRIFIC_DEF_PARMAX_PSWARM_RO2P; break;
      case PRO3A: *value = TIRIFIC_DEF_PARMAX_PSWARM_RO3A; break;
      case PRO3P: *value = TIRIFIC_DEF_PARMAX_PSWARM_RO3P; break;
      case PRO4A: *value = TIRIFIC_DEF_PARMAX_PSWARM_RO4A; break;
      case PRO4P: *value = TIRIFIC_DEF_PARMAX_PSWARM_RO4P; break;
      case PRA1A: *value = TIRIFIC_DEF_PARMAX_PSWARM_RA1A; break;
      case PRA1P: *value = TIRIFIC_DEF_PARMAX_PSWARM_RA1P; break;
      case PRA2A: *value = TIRIFIC_DEF_PARMAX_PSWARM_RA2A; break;
      case PRA2P: *value = TIRIFIC_DEF_PARMAX_PSWARM_RA2P; break;
      case PRA3A: *value = TIRIFIC_DEF_PARMAX_PSWARM_RA3A; break;
      case PRA3P: *value = TIRIFIC_DEF_PARMAX_PSWARM_RA3P; break;
      case PRA4A: *value = TIRIFIC_DEF_PARMAX_PSWARM_RA4A; break;
      case PRA4P: *value = TIRIFIC_DEF_PARMAX_PSWARM_RA4P; break;
      default: return 1;
      }
      break;
    default:
      switch (par) {
      case PVROT: *value = TIRIFIC_DEF_PARMAX_VROT; break;
      case PVRAD: *value = TIRIFIC_DEF_PARMAX_VRAD; break;
      case PVVER: *value = TIRIFIC_DEF_PARMAX_VVER; break;
      case PDVRO: *value = TIRIFIC_DEF_PARMAX_DVRO; break;
      case PDVRA: *value = TIRIFIC_DEF_PARMAX_DVRA; break;
      case PDVVE: *value = TIRIFIC_DEF_PARMAX_DVVE; break;
      case PZDRO: *value = TIRIFIC_DEF_PARMAX_ZDRO; break;
      case PZDRA: *value = TIRIFIC_DEF_PARMAX_ZDRA; break;
      case PZDVE: *value = TIRIFIC_DEF_PARMAX_ZDVE; break;
      case PZ0  : *value = TIRIFIC_DEF_PARMAX_Z0  ; break;
      case PSBR : *value = TIRIFIC_DEF_PARMAX_SBR ; break;
      case PSM1A: *value = TIRIFIC_DEF_PARMAX_SM1A; break;
      case PSM1P: *value = TIRIFIC_DEF_PARMAX_SM1P; break;
      case PSM2A: *value = TIRIFIC_DEF_PARMAX_SM2A; break;
      case PSM2P: *value = TIRIFIC_DEF_PARMAX_SM2P; break;
      case PSM3A: *value = TIRIFIC_DEF_PARMAX_SM3A; break;
      case PSM3P: *value = TIRIFIC_DEF_PARMAX_SM3P; break;
      case PSM4A: *value = TIRIFIC_DEF_PARMAX_SM4A; break;
      case PSM4P: *value = TIRIFIC_DEF_PARMAX_SM4P; break;
      case PGA1A: *value = TIRIFIC_DEF_PARMAX_GA1A; break;
      case PGA1P: *value = TIRIFIC_DEF_PARMAX_GA1P; break;
      case PGA1D: *value = TIRIFIC_DEF_PARMAX_GA1D; break;
      case PGA2A: *value = TIRIFIC_DEF_PARMAX_GA2A; break;
      case PGA2P: *value = TIRIFIC_DEF_PARMAX_GA2P; break;
      case PGA2D: *value = TIRIFIC_DEF_PARMAX_GA2D; break;
      case PGA3A: *value = TIRIFIC_DEF_PARMAX_GA3A; break;
      case PGA3P: *value = TIRIFIC_DEF_PARMAX_GA3P; break;
      case PGA3D: *value = TIRIFIC_DEF_PARMAX_GA3D; break;
      case PGA4A: *value = TIRIFIC_DEF_PARMAX_GA4A; break;
      case PGA4P: *value = TIRIFIC_DEF_PARMAX_GA4P; break;
      case PGA4D: *value = TIRIFIC_DEF_PARMAX_GA4D; break;
      case PAZ1P: *value = TIRIFIC_DEF_PARMAX_AZ1P; break;
      case PAZ1W: *value = TIRIFIC_DEF_PARMAX_AZ1W; break;
      case PAZ2P: *value = TIRIFIC_DEF_PARMAX_AZ2P; break;
      case PAZ2W: *value = TIRIFIC_DEF_PARMAX_AZ2W; break;
      case PINCL: *value = TIRIFIC_DEF_PARMAX_INCL; break;
      case PPA  : *value = TIRIFIC_DEF_PARMAX_PA  ; break;
      case PXPOS: *value = TIRIFIC_DEF_PARMAX_XPOS; break;
      case PYPOS: *value = TIRIFIC_DEF_PARMAX_YPOS; break;
      case PVSYS: *value = TIRIFIC_DEF_PARMAX_VSYS; break;
      case PSDIS: *value = TIRIFIC_DEF_PARMAX_SDIS; break;
      case PCLNR: *value = TIRIFIC_DEF_PARMAX_CLNR; break;
      case PVM0A: *value = TIRIFIC_DEF_PARMAX_VM0A; break;
      case PVM1A: *value = TIRIFIC_DEF_PARMAX_VM1A; break;
      case PVM1P: *value = TIRIFIC_DEF_PARMAX_VM1P; break;
      case PVM2A: *value = TIRIFIC_DEF_PARMAX_VM2A; break;
      case PVM2P: *value = TIRIFIC_DEF_PARMAX_VM2P; break;
      case PVM3A: *value = TIRIFIC_DEF_PARMAX_VM3A; break;
      case PVM3P: *value = TIRIFIC_DEF_PARMAX_VM3P; break;
      case PVM4A: *value = TIRIFIC_DEF_PARMAX_VM4A; break;
      case PVM4P: *value = TIRIFIC_DEF_PARMAX_VM4P; break;
      case PWM0A: *value = TIRIFIC_DEF_PARMAX_WM0A; break;
      case PWM1A: *value = TIRIFIC_DEF_PARMAX_WM1A; break;
      case PWM1P: *value = TIRIFIC_DEF_PARMAX_WM1P; break;
      case PWM2A: *value = TIRIFIC_DEF_PARMAX_WM2A; break;
      case PWM2P: *value = TIRIFIC_DEF_PARMAX_WM2P; break;
      case PWM3A: *value = TIRIFIC_DEF_PARMAX_WM3A; break;
      case PWM3P: *value = TIRIFIC_DEF_PARMAX_WM3P; break;
      case PWM4A: *value = TIRIFIC_DEF_PARMAX_WM4A; break;
      case PWM4P: *value = TIRIFIC_DEF_PARMAX_WM4P; break;
      case PLS0 : *value = TIRIFIC_DEF_PARMAX_LS0 ; break;
      case PLC0 : *value = TIRIFIC_DEF_PARMAX_LC0 ; break;
      case PRO1A: *value = TIRIFIC_DEF_PARMAX_RO1A; break;
      case PRO1P: *value = TIRIFIC_DEF_PARMAX_RO1P; break;
      case PRO2A: *value = TIRIFIC_DEF_PARMAX_RO2A; break;
      case PRO2P: *value = TIRIFIC_DEF_PARMAX_RO2P; break;
      case PRO3A: *value = TIRIFIC_DEF_PARMAX_RO3A; break;
      case PRO3P: *value = TIRIFIC_DEF_PARMAX_RO3P; break;
      case PRO4A: *value = TIRIFIC_DEF_PARMAX_RO4A; break;
      case PRO4P: *value = TIRIFIC_DEF_PARMAX_RO4P; break;
      case PRA1A: *value = TIRIFIC_DEF_PARMAX_RA1A; break;
      case PRA1P: *value = TIRIFIC_DEF_PARMAX_RA1P; break;
      case PRA2A: *value = TIRIFIC_DEF_PARMAX_RA2A; break;
      case PRA2P: *value = TIRIFIC_DEF_PARMAX_RA2P; break;
      case PRA3A: *value = TIRIFIC_DEF_PARMAX_RA3A; break;
      case PRA3P: *value = TIRIFIC_DEF_PARMAX_RA3P; break;
      case PRA4A: *value = TIRIFIC_DEF_PARMAX_RA4A; break;
      case PRA4P: *value = TIRIFIC_DEF_PARMAX_RA4P; break;
      default: return 1;
      }
      break;
    }
    break;
  case TIRIDENT_PARMIN  :
    switch (fitmode) {
    case PSWARM:
      switch (par) {
      case PVROT: *value = TIRIFIC_DEF_PARMIN_PSWARM_VROT; break;
      case PVRAD: *value = TIRIFIC_DEF_PARMIN_PSWARM_VRAD; break;
      case PVVER: *value = TIRIFIC_DEF_PARMIN_PSWARM_VVER; break;
      case PDVRO: *value = TIRIFIC_DEF_PARMIN_PSWARM_DVRO; break;
      case PDVRA: *value = TIRIFIC_DEF_PARMIN_PSWARM_DVRA; break;
      case PDVVE: *value = TIRIFIC_DEF_PARMIN_PSWARM_DVVE; break;
      case PZDRO: *value = TIRIFIC_DEF_PARMIN_PSWARM_ZDRO; break;
      case PZDRA: *value = TIRIFIC_DEF_PARMIN_PSWARM_ZDRA; break;
      case PZDVE: *value = TIRIFIC_DEF_PARMIN_PSWARM_ZDVE; break;
      case PZ0  : *value = TIRIFIC_DEF_PARMIN_PSWARM_Z0  ; break;
      case PSBR : *value = TIRIFIC_DEF_PARMIN_PSWARM_SBR ; break;
      case PSM1A: *value = TIRIFIC_DEF_PARMIN_PSWARM_SM1A; break;
      case PSM1P: *value = TIRIFIC_DEF_PARMIN_PSWARM_SM1P; break;
      case PSM2A: *value = TIRIFIC_DEF_PARMIN_PSWARM_SM2A; break;
      case PSM2P: *value = TIRIFIC_DEF_PARMIN_PSWARM_SM2P; break;
      case PSM3A: *value = TIRIFIC_DEF_PARMIN_PSWARM_SM3A; break;
      case PSM3P: *value = TIRIFIC_DEF_PARMIN_PSWARM_SM3P; break;
      case PSM4A: *value = TIRIFIC_DEF_PARMIN_PSWARM_SM4A; break;
      case PSM4P: *value = TIRIFIC_DEF_PARMIN_PSWARM_SM4P; break;
      case PGA1A: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA1A; break;
      case PGA1P: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA1P; break;
      case PGA1D: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA1D; break;
      case PGA2A: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA2A; break;
      case PGA2P: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA2P; break;
      case PGA2D: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA2D; break;
      case PGA3A: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA3A; break;
      case PGA3P: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA3P; break;
      case PGA3D: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA3D; break;
      case PGA4A: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA4A; break;
      case PGA4P: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA4P; break;
      case PGA4D: *value = TIRIFIC_DEF_PARMIN_PSWARM_GA4D; break;
      case PAZ1P: *value = TIRIFIC_DEF_PARMIN_PSWARM_AZ1P; break;
      case PAZ1W: *value = TIRIFIC_DEF_PARMIN_PSWARM_AZ1W; break;
      case PAZ2P: *value = TIRIFIC_DEF_PARMIN_PSWARM_AZ2P; break;
      case PAZ2W: *value = TIRIFIC_DEF_PARMIN_PSWARM_AZ2W; break;
      case PINCL: *value = TIRIFIC_DEF_PARMIN_PSWARM_INCL; break;
      case PPA  : *value = TIRIFIC_DEF_PARMIN_PSWARM_PA  ; break;
      case PXPOS: *value = TIRIFIC_DEF_PARMIN_PSWARM_XPOS; break;
      case PYPOS: *value = TIRIFIC_DEF_PARMIN_PSWARM_YPOS; break;
      case PVSYS: *value = TIRIFIC_DEF_PARMIN_PSWARM_VSYS; break;
      case PSDIS: *value = TIRIFIC_DEF_PARMIN_PSWARM_SDIS; break;
      case PCLNR: *value = TIRIFIC_DEF_PARMIN_PSWARM_CLNR; break;
      case PVM0A: *value = TIRIFIC_DEF_PARMIN_PSWARM_VM0A; break;
      case PVM1A: *value = TIRIFIC_DEF_PARMIN_PSWARM_VM1A; break;
      case PVM1P: *value = TIRIFIC_DEF_PARMIN_PSWARM_VM1P; break;
      case PVM2A: *value = TIRIFIC_DEF_PARMIN_PSWARM_VM2A; break;
      case PVM2P: *value = TIRIFIC_DEF_PARMIN_PSWARM_VM2P; break;
      case PVM3A: *value = TIRIFIC_DEF_PARMIN_PSWARM_VM3A; break;
      case PVM3P: *value = TIRIFIC_DEF_PARMIN_PSWARM_VM3P; break;
      case PVM4A: *value = TIRIFIC_DEF_PARMIN_PSWARM_VM4A; break;
      case PVM4P: *value = TIRIFIC_DEF_PARMIN_PSWARM_VM4P; break;
      case PWM0A: *value = TIRIFIC_DEF_PARMIN_PSWARM_WM0A; break;
      case PWM1A: *value = TIRIFIC_DEF_PARMIN_PSWARM_WM1A; break;
      case PWM1P: *value = TIRIFIC_DEF_PARMIN_PSWARM_WM1P; break;
      case PWM2A: *value = TIRIFIC_DEF_PARMIN_PSWARM_WM2A; break;
      case PWM2P: *value = TIRIFIC_DEF_PARMIN_PSWARM_WM2P; break;
      case PWM3A: *value = TIRIFIC_DEF_PARMIN_PSWARM_WM3A; break;
      case PWM3P: *value = TIRIFIC_DEF_PARMIN_PSWARM_WM3P; break;
      case PWM4A: *value = TIRIFIC_DEF_PARMIN_PSWARM_WM4A; break;
      case PWM4P: *value = TIRIFIC_DEF_PARMIN_PSWARM_WM4P; break;
      case PLS0 : *value = TIRIFIC_DEF_PARMIN_PSWARM_LS0 ; break;
      case PLC0 : *value = TIRIFIC_DEF_PARMIN_PSWARM_LC0 ; break;
      case PRO1A: *value = TIRIFIC_DEF_PARMIN_PSWARM_RO1A; break;
      case PRO1P: *value = TIRIFIC_DEF_PARMIN_PSWARM_RO1P; break;
      case PRO2A: *value = TIRIFIC_DEF_PARMIN_PSWARM_RO2A; break;
      case PRO2P: *value = TIRIFIC_DEF_PARMIN_PSWARM_RO2P; break;
      case PRO3A: *value = TIRIFIC_DEF_PARMIN_PSWARM_RO3A; break;
      case PRO3P: *value = TIRIFIC_DEF_PARMIN_PSWARM_RO3P; break;
      case PRO4A: *value = TIRIFIC_DEF_PARMIN_PSWARM_RO4A; break;
      case PRO4P: *value = TIRIFIC_DEF_PARMIN_PSWARM_RO4P; break;
      case PRA1A: *value = TIRIFIC_DEF_PARMIN_PSWARM_RA1A; break;
      case PRA1P: *value = TIRIFIC_DEF_PARMIN_PSWARM_RA1P; break;
      case PRA2A: *value = TIRIFIC_DEF_PARMIN_PSWARM_RA2A; break;
      case PRA2P: *value = TIRIFIC_DEF_PARMIN_PSWARM_RA2P; break;
      case PRA3A: *value = TIRIFIC_DEF_PARMIN_PSWARM_RA3A; break;
      case PRA3P: *value = TIRIFIC_DEF_PARMIN_PSWARM_RA3P; break;
      case PRA4A: *value = TIRIFIC_DEF_PARMIN_PSWARM_RA4A; break;
      case PRA4P: *value = TIRIFIC_DEF_PARMIN_PSWARM_RA4P; break;
      default: return 1;
      }
      break;
    default:
      switch (par) {
      case PVROT: *value = TIRIFIC_DEF_PARMIN_VROT; break;
      case PVRAD: *value = TIRIFIC_DEF_PARMIN_VRAD; break;
      case PVVER: *value = TIRIFIC_DEF_PARMIN_VVER; break;
      case PDVRO: *value = TIRIFIC_DEF_PARMIN_DVRO; break;
      case PDVRA: *value = TIRIFIC_DEF_PARMIN_DVRA; break;
      case PDVVE: *value = TIRIFIC_DEF_PARMIN_DVVE; break;
      case PZDRO: *value = TIRIFIC_DEF_PARMIN_ZDRO; break;
      case PZDRA: *value = TIRIFIC_DEF_PARMIN_ZDRA; break;
      case PZDVE: *value = TIRIFIC_DEF_PARMIN_ZDVE; break;
      case PZ0  : *value = TIRIFIC_DEF_PARMIN_Z0  ; break;
      case PSBR : *value = TIRIFIC_DEF_PARMIN_SBR ; break;
      case PSM1A: *value = TIRIFIC_DEF_PARMIN_SM1A; break;
      case PSM1P: *value = TIRIFIC_DEF_PARMIN_SM1P; break;
      case PSM2A: *value = TIRIFIC_DEF_PARMIN_SM2A; break;
      case PSM2P: *value = TIRIFIC_DEF_PARMIN_SM2P; break;
      case PSM3A: *value = TIRIFIC_DEF_PARMIN_SM3A; break;
      case PSM3P: *value = TIRIFIC_DEF_PARMIN_SM3P; break;
      case PSM4A: *value = TIRIFIC_DEF_PARMIN_SM4A; break;
      case PSM4P: *value = TIRIFIC_DEF_PARMIN_SM4P; break;
      case PGA1A: *value = TIRIFIC_DEF_PARMIN_GA1A; break;
      case PGA1P: *value = TIRIFIC_DEF_PARMIN_GA1P; break;
      case PGA1D: *value = TIRIFIC_DEF_PARMIN_GA1D; break;
      case PGA2A: *value = TIRIFIC_DEF_PARMIN_GA2A; break;
      case PGA2P: *value = TIRIFIC_DEF_PARMIN_GA2P; break;
      case PGA2D: *value = TIRIFIC_DEF_PARMIN_GA2D; break;
      case PGA3A: *value = TIRIFIC_DEF_PARMIN_GA3A; break;
      case PGA3P: *value = TIRIFIC_DEF_PARMIN_GA3P; break;
      case PGA3D: *value = TIRIFIC_DEF_PARMIN_GA3D; break;
      case PGA4A: *value = TIRIFIC_DEF_PARMIN_GA4A; break;
      case PGA4P: *value = TIRIFIC_DEF_PARMIN_GA4P; break;
      case PGA4D: *value = TIRIFIC_DEF_PARMIN_GA4D; break;
      case PAZ1P: *value = TIRIFIC_DEF_PARMIN_AZ1P; break;
      case PAZ1W: *value = TIRIFIC_DEF_PARMIN_AZ1W; break;
      case PAZ2P: *value = TIRIFIC_DEF_PARMIN_AZ2P; break;
      case PAZ2W: *value = TIRIFIC_DEF_PARMIN_AZ2W; break;
      case PINCL: *value = TIRIFIC_DEF_PARMIN_INCL; break;
      case PPA  : *value = TIRIFIC_DEF_PARMIN_PA  ; break;
      case PXPOS: *value = TIRIFIC_DEF_PARMIN_XPOS; break;
      case PYPOS: *value = TIRIFIC_DEF_PARMIN_YPOS; break;
      case PVSYS: *value = TIRIFIC_DEF_PARMIN_VSYS; break;
      case PSDIS: *value = TIRIFIC_DEF_PARMIN_SDIS; break;
      case PCLNR: *value = TIRIFIC_DEF_PARMIN_CLNR; break;
      case PVM0A: *value = TIRIFIC_DEF_PARMIN_VM0A; break;
      case PVM1A: *value = TIRIFIC_DEF_PARMIN_VM1A; break;
      case PVM1P: *value = TIRIFIC_DEF_PARMIN_VM1P; break;
      case PVM2A: *value = TIRIFIC_DEF_PARMIN_VM2A; break;
      case PVM2P: *value = TIRIFIC_DEF_PARMIN_VM2P; break;
      case PVM3A: *value = TIRIFIC_DEF_PARMIN_VM3A; break;
      case PVM3P: *value = TIRIFIC_DEF_PARMIN_VM3P; break;
      case PVM4A: *value = TIRIFIC_DEF_PARMIN_VM4A; break;
      case PVM4P: *value = TIRIFIC_DEF_PARMIN_VM4P; break;
      case PWM0A: *value = TIRIFIC_DEF_PARMIN_WM0A; break;
      case PWM1A: *value = TIRIFIC_DEF_PARMIN_WM1A; break;
      case PWM1P: *value = TIRIFIC_DEF_PARMIN_WM1P; break;
      case PWM2A: *value = TIRIFIC_DEF_PARMIN_WM2A; break;
      case PWM2P: *value = TIRIFIC_DEF_PARMIN_WM2P; break;
      case PWM3A: *value = TIRIFIC_DEF_PARMIN_WM3A; break;
      case PWM3P: *value = TIRIFIC_DEF_PARMIN_WM3P; break;
      case PWM4A: *value = TIRIFIC_DEF_PARMIN_WM4A; break;
      case PWM4P: *value = TIRIFIC_DEF_PARMIN_WM4P; break;
      case PLS0 : *value = TIRIFIC_DEF_PARMIN_LS0 ; break;
      case PLC0 : *value = TIRIFIC_DEF_PARMIN_LC0 ; break;
      case PRO1A: *value = TIRIFIC_DEF_PARMIN_RO1A; break;
      case PRO1P: *value = TIRIFIC_DEF_PARMIN_RO1P; break;
      case PRO2A: *value = TIRIFIC_DEF_PARMIN_RO2A; break;
      case PRO2P: *value = TIRIFIC_DEF_PARMIN_RO2P; break;
      case PRO3A: *value = TIRIFIC_DEF_PARMIN_RO3A; break;
      case PRO3P: *value = TIRIFIC_DEF_PARMIN_RO3P; break;
      case PRO4A: *value = TIRIFIC_DEF_PARMIN_RO4A; break;
      case PRO4P: *value = TIRIFIC_DEF_PARMIN_RO4P; break;
      case PRA1A: *value = TIRIFIC_DEF_PARMIN_RA1A; break;
      case PRA1P: *value = TIRIFIC_DEF_PARMIN_RA1P; break;
      case PRA2A: *value = TIRIFIC_DEF_PARMIN_RA2A; break;
      case PRA2P: *value = TIRIFIC_DEF_PARMIN_RA2P; break;
      case PRA3A: *value = TIRIFIC_DEF_PARMIN_RA3A; break;
      case PRA3P: *value = TIRIFIC_DEF_PARMIN_RA3P; break;
      case PRA4A: *value = TIRIFIC_DEF_PARMIN_RA4A; break;
      case PRA4P: *value = TIRIFIC_DEF_PARMIN_RA4P; break;
      default: return 1;
      }
      break;
    }
    break;
    case TIRIDENT_MODERATE:
    switch (par) {
    case PVROT: *value = TIRIFIC_DEF_MODERATE_VROT; break;
    case PVRAD: *value = TIRIFIC_DEF_MODERATE_VRAD; break;
    case PVVER: *value = TIRIFIC_DEF_MODERATE_VVER; break;
    case PDVRO: *value = TIRIFIC_DEF_MODERATE_DVRO; break;
    case PDVRA: *value = TIRIFIC_DEF_MODERATE_DVRA; break;
    case PDVVE: *value = TIRIFIC_DEF_MODERATE_DVVE; break;
    case PZDRO: *value = TIRIFIC_DEF_MODERATE_ZDRO; break;
    case PZDRA: *value = TIRIFIC_DEF_MODERATE_ZDRA; break;
    case PZDVE: *value = TIRIFIC_DEF_MODERATE_ZDVE; break;
    case PZ0  : *value = TIRIFIC_DEF_MODERATE_Z0  ; break;
    case PSBR : *value = TIRIFIC_DEF_MODERATE_SBR ; break;
    case PSM1A: *value = TIRIFIC_DEF_MODERATE_SM1A; break;
    case PSM1P: *value = TIRIFIC_DEF_MODERATE_SM1P; break;
    case PSM2A: *value = TIRIFIC_DEF_MODERATE_SM2A; break;
    case PSM2P: *value = TIRIFIC_DEF_MODERATE_SM2P; break;
    case PSM3A: *value = TIRIFIC_DEF_MODERATE_SM3A; break;
    case PSM3P: *value = TIRIFIC_DEF_MODERATE_SM3P; break;
    case PSM4A: *value = TIRIFIC_DEF_MODERATE_SM4A; break;
    case PSM4P: *value = TIRIFIC_DEF_MODERATE_SM4P; break;
    case PGA1A: *value = TIRIFIC_DEF_MODERATE_GA1A; break;
    case PGA1P: *value = TIRIFIC_DEF_MODERATE_GA1P; break;
    case PGA1D: *value = TIRIFIC_DEF_MODERATE_GA1D; break;
    case PGA2A: *value = TIRIFIC_DEF_MODERATE_GA2A; break;
    case PGA2P: *value = TIRIFIC_DEF_MODERATE_GA2P; break;
    case PGA2D: *value = TIRIFIC_DEF_MODERATE_GA2D; break;
    case PGA3A: *value = TIRIFIC_DEF_MODERATE_GA3A; break;
    case PGA3P: *value = TIRIFIC_DEF_MODERATE_GA3P; break;
    case PGA3D: *value = TIRIFIC_DEF_MODERATE_GA3D; break;
    case PGA4A: *value = TIRIFIC_DEF_MODERATE_GA4A; break;
    case PGA4P: *value = TIRIFIC_DEF_MODERATE_GA4P; break;
    case PGA4D: *value = TIRIFIC_DEF_MODERATE_GA4D; break;
    case PAZ1P: *value = TIRIFIC_DEF_MODERATE_AZ1P; break;
    case PAZ1W: *value = TIRIFIC_DEF_MODERATE_AZ1W; break;
    case PAZ2P: *value = TIRIFIC_DEF_MODERATE_AZ2P; break;
    case PAZ2W: *value = TIRIFIC_DEF_MODERATE_AZ2W; break;
    case PINCL: *value = TIRIFIC_DEF_MODERATE_INCL; break;
    case PPA  : *value = TIRIFIC_DEF_MODERATE_PA  ; break;
    case PXPOS: *value = TIRIFIC_DEF_MODERATE_XPOS; break;
    case PYPOS: *value = TIRIFIC_DEF_MODERATE_YPOS; break;
    case PVSYS: *value = TIRIFIC_DEF_MODERATE_VSYS; break;
    case PSDIS: *value = TIRIFIC_DEF_MODERATE_SDIS; break;
    case PCLNR: *value = TIRIFIC_DEF_MODERATE_CLNR; break;
    case PVM0A: *value = TIRIFIC_DEF_MODERATE_VM0A; break;
    case PVM1A: *value = TIRIFIC_DEF_MODERATE_VM1A; break;
    case PVM1P: *value = TIRIFIC_DEF_MODERATE_VM1P; break;
    case PVM2A: *value = TIRIFIC_DEF_MODERATE_VM2A; break;
    case PVM2P: *value = TIRIFIC_DEF_MODERATE_VM2P; break;
    case PVM3A: *value = TIRIFIC_DEF_MODERATE_VM3A; break;
    case PVM3P: *value = TIRIFIC_DEF_MODERATE_VM3P; break;
    case PVM4A: *value = TIRIFIC_DEF_MODERATE_VM4A; break;
    case PVM4P: *value = TIRIFIC_DEF_MODERATE_VM4P; break;
    case PWM0A: *value = TIRIFIC_DEF_MODERATE_WM0A; break;
    case PWM1A: *value = TIRIFIC_DEF_MODERATE_WM1A; break;
    case PWM1P: *value = TIRIFIC_DEF_MODERATE_WM1P; break;
    case PWM2A: *value = TIRIFIC_DEF_MODERATE_WM2A; break;
    case PWM2P: *value = TIRIFIC_DEF_MODERATE_WM2P; break;
    case PWM3A: *value = TIRIFIC_DEF_MODERATE_WM3A; break;
    case PWM3P: *value = TIRIFIC_DEF_MODERATE_WM3P; break;
    case PWM4A: *value = TIRIFIC_DEF_MODERATE_WM4A; break;
    case PWM4P: *value = TIRIFIC_DEF_MODERATE_WM4P; break;
    case PLS0 : *value = TIRIFIC_DEF_MODERATE_LS0 ; break;
    case PLC0 : *value = TIRIFIC_DEF_MODERATE_LC0 ; break;
    case PRO1A: *value = TIRIFIC_DEF_MODERATE_RO1A; break;
    case PRO1P: *value = TIRIFIC_DEF_MODERATE_RO1P; break;
    case PRO2A: *value = TIRIFIC_DEF_MODERATE_RO2A; break;
    case PRO2P: *value = TIRIFIC_DEF_MODERATE_RO2P; break;
    case PRO3A: *value = TIRIFIC_DEF_MODERATE_RO3A; break;
    case PRO3P: *value = TIRIFIC_DEF_MODERATE_RO3P; break;
    case PRO4A: *value = TIRIFIC_DEF_MODERATE_RO4A; break;
    case PRO4P: *value = TIRIFIC_DEF_MODERATE_RO4P; break;
    case PRA1A: *value = TIRIFIC_DEF_MODERATE_RA1A; break;
    case PRA1P: *value = TIRIFIC_DEF_MODERATE_RA1P; break;
    case PRA2A: *value = TIRIFIC_DEF_MODERATE_RA2A; break;
    case PRA2P: *value = TIRIFIC_DEF_MODERATE_RA2P; break;
    case PRA3A: *value = TIRIFIC_DEF_MODERATE_RA3A; break;
    case PRA3P: *value = TIRIFIC_DEF_MODERATE_RA3P; break;
    case PRA4A: *value = TIRIFIC_DEF_MODERATE_RA4A; break;
    case PRA4P: *value = TIRIFIC_DEF_MODERATE_RA4P; break;
    default: return 1;
    }
    break;
  case TIRIDENT_DELSTART:
    switch (par) {
    case PVROT: *value = TIRIFIC_DEF_DELSTART_VROT; break;
    case PVRAD: *value = TIRIFIC_DEF_DELSTART_VRAD; break;
    case PVVER: *value = TIRIFIC_DEF_DELSTART_VVER; break;
    case PDVRO: *value = TIRIFIC_DEF_DELSTART_DVRO; break;
    case PDVRA: *value = TIRIFIC_DEF_DELSTART_DVRA; break;
    case PDVVE: *value = TIRIFIC_DEF_DELSTART_DVVE; break;
    case PZDRO: *value = TIRIFIC_DEF_DELSTART_ZDRO; break;
    case PZDRA: *value = TIRIFIC_DEF_DELSTART_ZDRA; break;
    case PZDVE: *value = TIRIFIC_DEF_DELSTART_ZDVE; break;
    case PZ0  : *value = TIRIFIC_DEF_DELSTART_Z0  ; break;
    case PSBR : *value = TIRIFIC_DEF_DELSTART_SBR ; break;
    case PSM1A: *value = TIRIFIC_DEF_DELSTART_SM1A; break;
    case PSM1P: *value = TIRIFIC_DEF_DELSTART_SM1P; break;
    case PSM2A: *value = TIRIFIC_DEF_DELSTART_SM2A; break;
    case PSM2P: *value = TIRIFIC_DEF_DELSTART_SM2P; break;
    case PSM3A: *value = TIRIFIC_DEF_DELSTART_SM3A; break;
    case PSM3P: *value = TIRIFIC_DEF_DELSTART_SM3P; break;
    case PSM4A: *value = TIRIFIC_DEF_DELSTART_SM4A; break;
    case PSM4P: *value = TIRIFIC_DEF_DELSTART_SM4P; break;
    case PGA1A: *value = TIRIFIC_DEF_DELSTART_GA1A; break;
    case PGA1P: *value = TIRIFIC_DEF_DELSTART_GA1P; break;
    case PGA1D: *value = TIRIFIC_DEF_DELSTART_GA1D; break;
    case PGA2A: *value = TIRIFIC_DEF_DELSTART_GA2A; break;
    case PGA2P: *value = TIRIFIC_DEF_DELSTART_GA2P; break;
    case PGA2D: *value = TIRIFIC_DEF_DELSTART_GA2D; break;
    case PGA3A: *value = TIRIFIC_DEF_DELSTART_GA3A; break;
    case PGA3P: *value = TIRIFIC_DEF_DELSTART_GA3P; break;
    case PGA3D: *value = TIRIFIC_DEF_DELSTART_GA3D; break;
    case PGA4A: *value = TIRIFIC_DEF_DELSTART_GA4A; break;
    case PGA4P: *value = TIRIFIC_DEF_DELSTART_GA4P; break;
    case PGA4D: *value = TIRIFIC_DEF_DELSTART_GA4D; break;
    case PAZ1P: *value = TIRIFIC_DEF_DELSTART_AZ1P; break;
    case PAZ1W: *value = TIRIFIC_DEF_DELSTART_AZ1W; break;
    case PAZ2P: *value = TIRIFIC_DEF_DELSTART_AZ2P; break;
    case PAZ2W: *value = TIRIFIC_DEF_DELSTART_AZ2W; break;
    case PINCL: *value = TIRIFIC_DEF_DELSTART_INCL; break;
    case PPA  : *value = TIRIFIC_DEF_DELSTART_PA  ; break;
    case PXPOS: *value = TIRIFIC_DEF_DELSTART_XPOS; break;
    case PYPOS: *value = TIRIFIC_DEF_DELSTART_YPOS; break;
    case PVSYS: *value = TIRIFIC_DEF_DELSTART_VSYS; break;
    case PSDIS: *value = TIRIFIC_DEF_DELSTART_SDIS; break;
    case PCLNR: *value = TIRIFIC_DEF_DELSTART_CLNR; break;
    case PVM0A: *value = TIRIFIC_DEF_DELSTART_VM0A; break;
    case PVM1A: *value = TIRIFIC_DEF_DELSTART_VM1A; break;
    case PVM1P: *value = TIRIFIC_DEF_DELSTART_VM1P; break;
    case PVM2A: *value = TIRIFIC_DEF_DELSTART_VM2A; break;
    case PVM2P: *value = TIRIFIC_DEF_DELSTART_VM2P; break;
    case PVM3A: *value = TIRIFIC_DEF_DELSTART_VM3A; break;
    case PVM3P: *value = TIRIFIC_DEF_DELSTART_VM3P; break;
    case PVM4A: *value = TIRIFIC_DEF_DELSTART_VM4A; break;
    case PVM4P: *value = TIRIFIC_DEF_DELSTART_VM4P; break;
    case PWM0A: *value = TIRIFIC_DEF_DELSTART_WM0A; break;
    case PWM1A: *value = TIRIFIC_DEF_DELSTART_WM1A; break;
    case PWM1P: *value = TIRIFIC_DEF_DELSTART_WM1P; break;
    case PWM2A: *value = TIRIFIC_DEF_DELSTART_WM2A; break;
    case PWM2P: *value = TIRIFIC_DEF_DELSTART_WM2P; break;
    case PWM3A: *value = TIRIFIC_DEF_DELSTART_WM3A; break;
    case PWM3P: *value = TIRIFIC_DEF_DELSTART_WM3P; break;
    case PWM4A: *value = TIRIFIC_DEF_DELSTART_WM4A; break;
    case PWM4P: *value = TIRIFIC_DEF_DELSTART_WM4P; break;
    case PLS0 : *value = TIRIFIC_DEF_DELSTART_LS0 ; break;
    case PLC0 : *value = TIRIFIC_DEF_DELSTART_LC0 ; break;
    case PRO1A: *value = TIRIFIC_DEF_DELSTART_RO1A; break;
    case PRO1P: *value = TIRIFIC_DEF_DELSTART_RO1P; break;
    case PRO2A: *value = TIRIFIC_DEF_DELSTART_RO2A; break;
    case PRO2P: *value = TIRIFIC_DEF_DELSTART_RO2P; break;
    case PRO3A: *value = TIRIFIC_DEF_DELSTART_RO3A; break;
    case PRO3P: *value = TIRIFIC_DEF_DELSTART_RO3P; break;
    case PRO4A: *value = TIRIFIC_DEF_DELSTART_RO4A; break;
    case PRO4P: *value = TIRIFIC_DEF_DELSTART_RO4P; break;
    case PRA1A: *value = TIRIFIC_DEF_DELSTART_RA1A; break;
    case PRA1P: *value = TIRIFIC_DEF_DELSTART_RA1P; break;
    case PRA2A: *value = TIRIFIC_DEF_DELSTART_RA2A; break;
    case PRA2P: *value = TIRIFIC_DEF_DELSTART_RA2P; break;
    case PRA3A: *value = TIRIFIC_DEF_DELSTART_RA3A; break;
    case PRA3P: *value = TIRIFIC_DEF_DELSTART_RA3P; break;
    case PRA4A: *value = TIRIFIC_DEF_DELSTART_RA4A; break;
    case PRA4P: *value = TIRIFIC_DEF_DELSTART_RA4P; break;
    default: return 1;
    }
    break;
  case TIRIDENT_DELEND  :
    switch (par) {
    case PVROT: *value = TIRIFIC_DEF_DELEND_VROT; break;
    case PVRAD: *value = TIRIFIC_DEF_DELEND_VRAD; break;
    case PVVER: *value = TIRIFIC_DEF_DELEND_VVER; break;
    case PDVRO: *value = TIRIFIC_DEF_DELEND_DVRO; break;
    case PDVRA: *value = TIRIFIC_DEF_DELEND_DVRA; break;
    case PDVVE: *value = TIRIFIC_DEF_DELEND_DVVE; break;
    case PZDRO: *value = TIRIFIC_DEF_DELEND_ZDRO; break;
    case PZDRA: *value = TIRIFIC_DEF_DELEND_ZDRA; break;
    case PZDVE: *value = TIRIFIC_DEF_DELEND_ZDVE; break;
    case PZ0  : *value = TIRIFIC_DEF_DELEND_Z0  ; break;
    case PSBR : *value = TIRIFIC_DEF_DELEND_SBR ; break;
    case PSM1A: *value = TIRIFIC_DEF_DELEND_SM1A; break;
    case PSM1P: *value = TIRIFIC_DEF_DELEND_SM1P; break;
    case PSM2A: *value = TIRIFIC_DEF_DELEND_SM2A; break;
    case PSM2P: *value = TIRIFIC_DEF_DELEND_SM2P; break;
    case PSM3A: *value = TIRIFIC_DEF_DELEND_SM3A; break;
    case PSM3P: *value = TIRIFIC_DEF_DELEND_SM3P; break;
    case PSM4A: *value = TIRIFIC_DEF_DELEND_SM4A; break;
    case PSM4P: *value = TIRIFIC_DEF_DELEND_SM4P; break;
    case PGA1A: *value = TIRIFIC_DEF_DELEND_GA1A; break;
    case PGA1P: *value = TIRIFIC_DEF_DELEND_GA1P; break;
    case PGA1D: *value = TIRIFIC_DEF_DELEND_GA1D; break;
    case PGA2A: *value = TIRIFIC_DEF_DELEND_GA2A; break;
    case PGA2P: *value = TIRIFIC_DEF_DELEND_GA2P; break;
    case PGA2D: *value = TIRIFIC_DEF_DELEND_GA2D; break;
    case PGA3A: *value = TIRIFIC_DEF_DELEND_GA3A; break;
    case PGA3P: *value = TIRIFIC_DEF_DELEND_GA3P; break;
    case PGA3D: *value = TIRIFIC_DEF_DELEND_GA3D; break;
    case PGA4A: *value = TIRIFIC_DEF_DELEND_GA4A; break;
    case PGA4P: *value = TIRIFIC_DEF_DELEND_GA4P; break;
    case PGA4D: *value = TIRIFIC_DEF_DELEND_GA4D; break;
    case PAZ1P: *value = TIRIFIC_DEF_DELEND_AZ1P; break;
    case PAZ1W: *value = TIRIFIC_DEF_DELEND_AZ1W; break;
    case PAZ2P: *value = TIRIFIC_DEF_DELEND_AZ2P; break;
    case PAZ2W: *value = TIRIFIC_DEF_DELEND_AZ2W; break;
    case PINCL: *value = TIRIFIC_DEF_DELEND_INCL; break;
    case PPA  : *value = TIRIFIC_DEF_DELEND_PA  ; break;
    case PXPOS: *value = TIRIFIC_DEF_DELEND_XPOS; break;
    case PYPOS: *value = TIRIFIC_DEF_DELEND_YPOS; break;
    case PVSYS: *value = TIRIFIC_DEF_DELEND_VSYS; break;
    case PSDIS: *value = TIRIFIC_DEF_DELEND_SDIS; break;
    case PCLNR: *value = TIRIFIC_DEF_DELEND_CLNR; break;
    case PVM0A: *value = TIRIFIC_DEF_DELEND_VM0A; break;
    case PVM1A: *value = TIRIFIC_DEF_DELEND_VM1A; break;
    case PVM1P: *value = TIRIFIC_DEF_DELEND_VM1P; break;
    case PVM2A: *value = TIRIFIC_DEF_DELEND_VM2A; break;
    case PVM2P: *value = TIRIFIC_DEF_DELEND_VM2P; break;
    case PVM3A: *value = TIRIFIC_DEF_DELEND_VM3A; break;
    case PVM3P: *value = TIRIFIC_DEF_DELEND_VM3P; break;
    case PVM4A: *value = TIRIFIC_DEF_DELEND_VM4A; break;
    case PVM4P: *value = TIRIFIC_DEF_DELEND_VM4P; break;
    case PWM0A: *value = TIRIFIC_DEF_DELEND_WM0A; break;
    case PWM1A: *value = TIRIFIC_DEF_DELEND_WM1A; break;
    case PWM1P: *value = TIRIFIC_DEF_DELEND_WM1P; break;
    case PWM2A: *value = TIRIFIC_DEF_DELEND_WM2A; break;
    case PWM2P: *value = TIRIFIC_DEF_DELEND_WM2P; break;
    case PWM3A: *value = TIRIFIC_DEF_DELEND_WM3A; break;
    case PWM3P: *value = TIRIFIC_DEF_DELEND_WM3P; break;
    case PWM4A: *value = TIRIFIC_DEF_DELEND_WM4A; break;
    case PWM4P: *value = TIRIFIC_DEF_DELEND_WM4P; break;
    case PLS0 : *value = TIRIFIC_DEF_DELEND_LS0 ; break;
    case PLC0 : *value = TIRIFIC_DEF_DELEND_LC0 ; break;
    case PRO1A: *value = TIRIFIC_DEF_DELEND_RO1A; break;
    case PRO1P: *value = TIRIFIC_DEF_DELEND_RO1P; break;
    case PRO2A: *value = TIRIFIC_DEF_DELEND_RO2A; break;
    case PRO2P: *value = TIRIFIC_DEF_DELEND_RO2P; break;
    case PRO3A: *value = TIRIFIC_DEF_DELEND_RO3A; break;
    case PRO3P: *value = TIRIFIC_DEF_DELEND_RO3P; break;
    case PRO4A: *value = TIRIFIC_DEF_DELEND_RO4A; break;
    case PRO4P: *value = TIRIFIC_DEF_DELEND_RO4P; break;
    case PRA1A: *value = TIRIFIC_DEF_DELEND_RA1A; break;
    case PRA1P: *value = TIRIFIC_DEF_DELEND_RA1P; break;
    case PRA2A: *value = TIRIFIC_DEF_DELEND_RA2A; break;
    case PRA2P: *value = TIRIFIC_DEF_DELEND_RA2P; break;
    case PRA3A: *value = TIRIFIC_DEF_DELEND_RA3A; break;
    case PRA3P: *value = TIRIFIC_DEF_DELEND_RA3P; break;
    case PRA4A: *value = TIRIFIC_DEF_DELEND_RA4A; break;
    case PRA4P: *value = TIRIFIC_DEF_DELEND_RA4P; break;
    default: return 1;
    }
    break;
  case TIRIDENT_ITESTART:
    switch (par) {
    case PVROT: *value = TIRIFIC_DEF_ITESTART_VROT; break;
    case PVRAD: *value = TIRIFIC_DEF_ITESTART_VRAD; break;
    case PVVER: *value = TIRIFIC_DEF_ITESTART_VVER; break;
    case PDVRO: *value = TIRIFIC_DEF_ITESTART_DVRO; break;
    case PDVRA: *value = TIRIFIC_DEF_ITESTART_DVRA; break;
    case PDVVE: *value = TIRIFIC_DEF_ITESTART_DVVE; break;
    case PZDRO: *value = TIRIFIC_DEF_ITESTART_ZDRO; break;
    case PZDRA: *value = TIRIFIC_DEF_ITESTART_ZDRA; break;
    case PZDVE: *value = TIRIFIC_DEF_ITESTART_ZDVE; break;
    case PZ0  : *value = TIRIFIC_DEF_ITESTART_Z0  ; break;
    case PSBR : *value = TIRIFIC_DEF_ITESTART_SBR ; break;
    case PSM1A: *value = TIRIFIC_DEF_ITESTART_SM1A; break;
    case PSM1P: *value = TIRIFIC_DEF_ITESTART_SM1P; break;
    case PSM2A: *value = TIRIFIC_DEF_ITESTART_SM2A; break;
    case PSM2P: *value = TIRIFIC_DEF_ITESTART_SM2P; break;
    case PSM3A: *value = TIRIFIC_DEF_ITESTART_SM3A; break;
    case PSM3P: *value = TIRIFIC_DEF_ITESTART_SM3P; break;
    case PSM4A: *value = TIRIFIC_DEF_ITESTART_SM4A; break;
    case PSM4P: *value = TIRIFIC_DEF_ITESTART_SM4P; break;
    case PGA1A: *value = TIRIFIC_DEF_ITESTART_GA1A; break;
    case PGA1P: *value = TIRIFIC_DEF_ITESTART_GA1P; break;
    case PGA1D: *value = TIRIFIC_DEF_ITESTART_GA1D; break;
    case PGA2A: *value = TIRIFIC_DEF_ITESTART_GA2A; break;
    case PGA2P: *value = TIRIFIC_DEF_ITESTART_GA2P; break;
    case PGA2D: *value = TIRIFIC_DEF_ITESTART_GA2D; break;
    case PGA3A: *value = TIRIFIC_DEF_ITESTART_GA3A; break;
    case PGA3P: *value = TIRIFIC_DEF_ITESTART_GA3P; break;
    case PGA3D: *value = TIRIFIC_DEF_ITESTART_GA3D; break;
    case PGA4A: *value = TIRIFIC_DEF_ITESTART_GA4A; break;
    case PGA4P: *value = TIRIFIC_DEF_ITESTART_GA4P; break;
    case PGA4D: *value = TIRIFIC_DEF_ITESTART_GA4D; break;
    case PAZ1P: *value = TIRIFIC_DEF_ITESTART_AZ1P; break;
    case PAZ1W: *value = TIRIFIC_DEF_ITESTART_AZ1W; break;
    case PAZ2P: *value = TIRIFIC_DEF_ITESTART_AZ2P; break;
    case PAZ2W: *value = TIRIFIC_DEF_ITESTART_AZ2W; break;
    case PINCL: *value = TIRIFIC_DEF_ITESTART_INCL; break;
    case PPA  : *value = TIRIFIC_DEF_ITESTART_PA  ; break;
    case PXPOS: *value = TIRIFIC_DEF_ITESTART_XPOS; break;
    case PYPOS: *value = TIRIFIC_DEF_ITESTART_YPOS; break;
    case PVSYS: *value = TIRIFIC_DEF_ITESTART_VSYS; break;
    case PSDIS: *value = TIRIFIC_DEF_ITESTART_SDIS; break;
    case PCLNR: *value = TIRIFIC_DEF_ITESTART_CLNR; break;
    case PVM0A: *value = TIRIFIC_DEF_ITESTART_VM0A; break;
    case PVM1A: *value = TIRIFIC_DEF_ITESTART_VM1A; break;
    case PVM1P: *value = TIRIFIC_DEF_ITESTART_VM1P; break;
    case PVM2A: *value = TIRIFIC_DEF_ITESTART_VM2A; break;
    case PVM2P: *value = TIRIFIC_DEF_ITESTART_VM2P; break;
    case PVM3A: *value = TIRIFIC_DEF_ITESTART_VM3A; break;
    case PVM3P: *value = TIRIFIC_DEF_ITESTART_VM3P; break;
    case PVM4A: *value = TIRIFIC_DEF_ITESTART_VM4A; break;
    case PVM4P: *value = TIRIFIC_DEF_ITESTART_VM4P; break;
    case PWM0A: *value = TIRIFIC_DEF_ITESTART_WM0A; break;
    case PWM1A: *value = TIRIFIC_DEF_ITESTART_WM1A; break;
    case PWM1P: *value = TIRIFIC_DEF_ITESTART_WM1P; break;
    case PWM2A: *value = TIRIFIC_DEF_ITESTART_WM2A; break;
    case PWM2P: *value = TIRIFIC_DEF_ITESTART_WM2P; break;
    case PWM3A: *value = TIRIFIC_DEF_ITESTART_WM3A; break;
    case PWM3P: *value = TIRIFIC_DEF_ITESTART_WM3P; break;
    case PWM4A: *value = TIRIFIC_DEF_ITESTART_WM4A; break;
    case PWM4P: *value = TIRIFIC_DEF_ITESTART_WM4P; break;
    case PLS0 : *value = TIRIFIC_DEF_ITESTART_LS0 ; break;
    case PLC0 : *value = TIRIFIC_DEF_ITESTART_LC0 ; break;
    case PRO1A: *value = TIRIFIC_DEF_ITESTART_RO1A; break;
    case PRO1P: *value = TIRIFIC_DEF_ITESTART_RO1P; break;
    case PRO2A: *value = TIRIFIC_DEF_ITESTART_RO2A; break;
    case PRO2P: *value = TIRIFIC_DEF_ITESTART_RO2P; break;
    case PRO3A: *value = TIRIFIC_DEF_ITESTART_RO3A; break;
    case PRO3P: *value = TIRIFIC_DEF_ITESTART_RO3P; break;
    case PRO4A: *value = TIRIFIC_DEF_ITESTART_RO4A; break;
    case PRO4P: *value = TIRIFIC_DEF_ITESTART_RO4P; break;
    case PRA1A: *value = TIRIFIC_DEF_ITESTART_RA1A; break;
    case PRA1P: *value = TIRIFIC_DEF_ITESTART_RA1P; break;
    case PRA2A: *value = TIRIFIC_DEF_ITESTART_RA2A; break;
    case PRA2P: *value = TIRIFIC_DEF_ITESTART_RA2P; break;
    case PRA3A: *value = TIRIFIC_DEF_ITESTART_RA3A; break;
    case PRA3P: *value = TIRIFIC_DEF_ITESTART_RA3P; break;
    case PRA4A: *value = TIRIFIC_DEF_ITESTART_RA4A; break;
    case PRA4P: *value = TIRIFIC_DEF_ITESTART_RA4P; break;
    default: return 1;
    }
    break;
  case TIRIDENT_ITEEND  :
    switch (par) {
    case PVROT: *value = TIRIFIC_DEF_ITEEND_VROT; break;
    case PVRAD: *value = TIRIFIC_DEF_ITEEND_VRAD; break;
    case PVVER: *value = TIRIFIC_DEF_ITEEND_VVER; break;
    case PDVRO: *value = TIRIFIC_DEF_ITEEND_DVRO; break;
    case PDVRA: *value = TIRIFIC_DEF_ITEEND_DVRA; break;
    case PDVVE: *value = TIRIFIC_DEF_ITEEND_DVVE; break;
    case PZDRO: *value = TIRIFIC_DEF_ITEEND_ZDRO; break;
    case PZDRA: *value = TIRIFIC_DEF_ITEEND_ZDRA; break;
    case PZDVE: *value = TIRIFIC_DEF_ITEEND_ZDVE; break;
    case PZ0  : *value = TIRIFIC_DEF_ITEEND_Z0  ; break;
    case PSBR : *value = TIRIFIC_DEF_ITEEND_SBR ; break;
    case PSM1A: *value = TIRIFIC_DEF_ITEEND_SM1A; break;
    case PSM1P: *value = TIRIFIC_DEF_ITEEND_SM1P; break;
    case PSM2A: *value = TIRIFIC_DEF_ITEEND_SM2A; break;
    case PSM2P: *value = TIRIFIC_DEF_ITEEND_SM2P; break;
    case PSM3A: *value = TIRIFIC_DEF_ITEEND_SM3A; break;
    case PSM3P: *value = TIRIFIC_DEF_ITEEND_SM3P; break;
    case PSM4A: *value = TIRIFIC_DEF_ITEEND_SM4A; break;
    case PSM4P: *value = TIRIFIC_DEF_ITEEND_SM4P; break;
    case PGA1A: *value = TIRIFIC_DEF_ITEEND_GA1A; break;
    case PGA1P: *value = TIRIFIC_DEF_ITEEND_GA1P; break;
    case PGA1D: *value = TIRIFIC_DEF_ITEEND_GA1D; break;
    case PGA2A: *value = TIRIFIC_DEF_ITEEND_GA2A; break;
    case PGA2P: *value = TIRIFIC_DEF_ITEEND_GA2P; break;
    case PGA2D: *value = TIRIFIC_DEF_ITEEND_GA2D; break;
    case PGA3A: *value = TIRIFIC_DEF_ITEEND_GA3A; break;
    case PGA3P: *value = TIRIFIC_DEF_ITEEND_GA3P; break;
    case PGA3D: *value = TIRIFIC_DEF_ITEEND_GA3D; break;
    case PGA4A: *value = TIRIFIC_DEF_ITEEND_GA4A; break;
    case PGA4P: *value = TIRIFIC_DEF_ITEEND_GA4P; break;
    case PGA4D: *value = TIRIFIC_DEF_ITEEND_GA4D; break;
    case PAZ1P: *value = TIRIFIC_DEF_ITEEND_AZ1P; break;
    case PAZ1W: *value = TIRIFIC_DEF_ITEEND_AZ1W; break;
    case PAZ2P: *value = TIRIFIC_DEF_ITEEND_AZ2P; break;
    case PAZ2W: *value = TIRIFIC_DEF_ITEEND_AZ2W; break;
    case PINCL: *value = TIRIFIC_DEF_ITEEND_INCL; break;
    case PPA  : *value = TIRIFIC_DEF_ITEEND_PA  ; break;
    case PXPOS: *value = TIRIFIC_DEF_ITEEND_XPOS; break;
    case PYPOS: *value = TIRIFIC_DEF_ITEEND_YPOS; break;
    case PVSYS: *value = TIRIFIC_DEF_ITEEND_VSYS; break;
    case PSDIS: *value = TIRIFIC_DEF_ITEEND_SDIS; break;
    case PCLNR: *value = TIRIFIC_DEF_ITEEND_CLNR; break;
    case PVM0A: *value = TIRIFIC_DEF_ITEEND_VM0A; break;
    case PVM1A: *value = TIRIFIC_DEF_ITEEND_VM1A; break;
    case PVM1P: *value = TIRIFIC_DEF_ITEEND_VM1P; break;
    case PVM2A: *value = TIRIFIC_DEF_ITEEND_VM2A; break;
    case PVM2P: *value = TIRIFIC_DEF_ITEEND_VM2P; break;
    case PVM3A: *value = TIRIFIC_DEF_ITEEND_VM3A; break;
    case PVM3P: *value = TIRIFIC_DEF_ITEEND_VM3P; break;
    case PVM4A: *value = TIRIFIC_DEF_ITEEND_VM4A; break;
    case PVM4P: *value = TIRIFIC_DEF_ITEEND_VM4P; break;
    case PWM0A: *value = TIRIFIC_DEF_ITEEND_WM0A; break;
    case PWM1A: *value = TIRIFIC_DEF_ITEEND_WM1A; break;
    case PWM1P: *value = TIRIFIC_DEF_ITEEND_WM1P; break;
    case PWM2A: *value = TIRIFIC_DEF_ITEEND_WM2A; break;
    case PWM2P: *value = TIRIFIC_DEF_ITEEND_WM2P; break;
    case PWM3A: *value = TIRIFIC_DEF_ITEEND_WM3A; break;
    case PWM3P: *value = TIRIFIC_DEF_ITEEND_WM3P; break;
    case PWM4A: *value = TIRIFIC_DEF_ITEEND_WM4A; break;
    case PWM4P: *value = TIRIFIC_DEF_ITEEND_WM4P; break;
    case PLS0 : *value = TIRIFIC_DEF_ITEEND_LS0 ; break;
    case PLC0 : *value = TIRIFIC_DEF_ITEEND_LC0 ; break;
    case PRO1A: *value = TIRIFIC_DEF_ITEEND_RO1A; break;
    case PRO1P: *value = TIRIFIC_DEF_ITEEND_RO1P; break;
    case PRO2A: *value = TIRIFIC_DEF_ITEEND_RO2A; break;
    case PRO2P: *value = TIRIFIC_DEF_ITEEND_RO2P; break;
    case PRO3A: *value = TIRIFIC_DEF_ITEEND_RO3A; break;
    case PRO3P: *value = TIRIFIC_DEF_ITEEND_RO3P; break;
    case PRO4A: *value = TIRIFIC_DEF_ITEEND_RO4A; break;
    case PRO4P: *value = TIRIFIC_DEF_ITEEND_RO4P; break;
    case PRA1A: *value = TIRIFIC_DEF_ITEEND_RA1A; break;
    case PRA1P: *value = TIRIFIC_DEF_ITEEND_RA1P; break;
    case PRA2A: *value = TIRIFIC_DEF_ITEEND_RA2A; break;
    case PRA2P: *value = TIRIFIC_DEF_ITEEND_RA2P; break;
    case PRA3A: *value = TIRIFIC_DEF_ITEEND_RA3A; break;
    case PRA3P: *value = TIRIFIC_DEF_ITEEND_RA3P; break;
    case PRA4A: *value = TIRIFIC_DEF_ITEEND_RA4A; break;
    case PRA4P: *value = TIRIFIC_DEF_ITEEND_RA4P; break;
     default: return 1;
   }
    break;
  case TIRIDENT_SATDELT :
    switch (par) {
    case PVROT: *value = TIRIFIC_DEF_SATDELT_VROT; break;
    case PVRAD: *value = TIRIFIC_DEF_SATDELT_VRAD; break;
    case PVVER: *value = TIRIFIC_DEF_SATDELT_VVER; break;
    case PDVRO: *value = TIRIFIC_DEF_SATDELT_DVRO; break;
    case PDVRA: *value = TIRIFIC_DEF_SATDELT_DVRA; break;
    case PDVVE: *value = TIRIFIC_DEF_SATDELT_DVVE; break;
    case PZDRO: *value = TIRIFIC_DEF_SATDELT_ZDRO; break;
    case PZDRA: *value = TIRIFIC_DEF_SATDELT_ZDRA; break;
    case PZDVE: *value = TIRIFIC_DEF_SATDELT_ZDVE; break;
    case PZ0  : *value = TIRIFIC_DEF_SATDELT_Z0  ; break;
    case PSBR : *value = TIRIFIC_DEF_SATDELT_SBR ; break;
    case PSM1A: *value = TIRIFIC_DEF_SATDELT_SM1A; break;
    case PSM1P: *value = TIRIFIC_DEF_SATDELT_SM1P; break;
    case PSM2A: *value = TIRIFIC_DEF_SATDELT_SM2A; break;
    case PSM2P: *value = TIRIFIC_DEF_SATDELT_SM2P; break;
    case PSM3A: *value = TIRIFIC_DEF_SATDELT_SM3A; break;
    case PSM3P: *value = TIRIFIC_DEF_SATDELT_SM3P; break;
    case PSM4A: *value = TIRIFIC_DEF_SATDELT_SM4A; break;
    case PSM4P: *value = TIRIFIC_DEF_SATDELT_SM4P; break;
    case PGA1A: *value = TIRIFIC_DEF_SATDELT_GA1A; break;
    case PGA1P: *value = TIRIFIC_DEF_SATDELT_GA1P; break;
    case PGA1D: *value = TIRIFIC_DEF_SATDELT_GA1D; break;
    case PGA2A: *value = TIRIFIC_DEF_SATDELT_GA2A; break;
    case PGA2P: *value = TIRIFIC_DEF_SATDELT_GA2P; break;
    case PGA2D: *value = TIRIFIC_DEF_SATDELT_GA2D; break;
    case PGA3A: *value = TIRIFIC_DEF_SATDELT_GA3A; break;
    case PGA3P: *value = TIRIFIC_DEF_SATDELT_GA3P; break;
    case PGA3D: *value = TIRIFIC_DEF_SATDELT_GA3D; break;
    case PGA4A: *value = TIRIFIC_DEF_SATDELT_GA4A; break;
    case PGA4P: *value = TIRIFIC_DEF_SATDELT_GA4P; break;
    case PGA4D: *value = TIRIFIC_DEF_SATDELT_GA4D; break;
    case PAZ1P: *value = TIRIFIC_DEF_SATDELT_AZ1P; break;
    case PAZ1W: *value = TIRIFIC_DEF_SATDELT_AZ1W; break;
    case PAZ2P: *value = TIRIFIC_DEF_SATDELT_AZ2P; break;
    case PAZ2W: *value = TIRIFIC_DEF_SATDELT_AZ2W; break;
    case PINCL: *value = TIRIFIC_DEF_SATDELT_INCL; break;
    case PPA  : *value = TIRIFIC_DEF_SATDELT_PA  ; break;
    case PXPOS: *value = TIRIFIC_DEF_SATDELT_XPOS; break;
    case PYPOS: *value = TIRIFIC_DEF_SATDELT_YPOS; break;
    case PVSYS: *value = TIRIFIC_DEF_SATDELT_VSYS; break;
    case PSDIS: *value = TIRIFIC_DEF_SATDELT_SDIS; break;
    case PCLNR: *value = TIRIFIC_DEF_SATDELT_CLNR; break;
    case PVM0A: *value = TIRIFIC_DEF_SATDELT_VM0A; break;
    case PVM1A: *value = TIRIFIC_DEF_SATDELT_VM1A; break;
    case PVM1P: *value = TIRIFIC_DEF_SATDELT_VM1P; break;
    case PVM2A: *value = TIRIFIC_DEF_SATDELT_VM2A; break;
    case PVM2P: *value = TIRIFIC_DEF_SATDELT_VM2P; break;
    case PVM3A: *value = TIRIFIC_DEF_SATDELT_VM3A; break;
    case PVM3P: *value = TIRIFIC_DEF_SATDELT_VM3P; break;
    case PVM4A: *value = TIRIFIC_DEF_SATDELT_VM4A; break;
    case PVM4P: *value = TIRIFIC_DEF_SATDELT_VM4P; break;
    case PWM0A: *value = TIRIFIC_DEF_SATDELT_WM0A; break;
    case PWM1A: *value = TIRIFIC_DEF_SATDELT_WM1A; break;
    case PWM1P: *value = TIRIFIC_DEF_SATDELT_WM1P; break;
    case PWM2A: *value = TIRIFIC_DEF_SATDELT_WM2A; break;
    case PWM2P: *value = TIRIFIC_DEF_SATDELT_WM2P; break;
    case PWM3A: *value = TIRIFIC_DEF_SATDELT_WM3A; break;
    case PWM3P: *value = TIRIFIC_DEF_SATDELT_WM3P; break;
    case PWM4A: *value = TIRIFIC_DEF_SATDELT_WM4A; break;
    case PWM4P: *value = TIRIFIC_DEF_SATDELT_WM4P; break;
    case PLS0 : *value = TIRIFIC_DEF_SATDELT_LS0 ; break;
    case PLC0 : *value = TIRIFIC_DEF_SATDELT_LC0 ; break;
    case PRO1A: *value = TIRIFIC_DEF_SATDELT_RO1A; break;
    case PRO1P: *value = TIRIFIC_DEF_SATDELT_RO1P; break;
    case PRO2A: *value = TIRIFIC_DEF_SATDELT_RO2A; break;
    case PRO2P: *value = TIRIFIC_DEF_SATDELT_RO2P; break;
    case PRO3A: *value = TIRIFIC_DEF_SATDELT_RO3A; break;
    case PRO3P: *value = TIRIFIC_DEF_SATDELT_RO3P; break;
    case PRO4A: *value = TIRIFIC_DEF_SATDELT_RO4A; break;
    case PRO4P: *value = TIRIFIC_DEF_SATDELT_RO4P; break;
    case PRA1A: *value = TIRIFIC_DEF_SATDELT_RA1A; break;
    case PRA1P: *value = TIRIFIC_DEF_SATDELT_RA1P; break;
    case PRA2A: *value = TIRIFIC_DEF_SATDELT_RA2A; break;
    case PRA2P: *value = TIRIFIC_DEF_SATDELT_RA2P; break;
    case PRA3A: *value = TIRIFIC_DEF_SATDELT_RA3A; break;
    case PRA3P: *value = TIRIFIC_DEF_SATDELT_RA3P; break;
    case PRA4A: *value = TIRIFIC_DEF_SATDELT_RA4A; break;
    case PRA4P: *value = TIRIFIC_DEF_SATDELT_RA4P; break;
    default: return 1;
    }
    break;
  case TIRIDENT_MINDELTA:
    switch (par) {
    case PVROT: *value = TIRIFIC_DEF_MINDELTA_VROT; break;
    case PVRAD: *value = TIRIFIC_DEF_MINDELTA_VRAD; break;
    case PVVER: *value = TIRIFIC_DEF_MINDELTA_VVER; break;
    case PDVRO: *value = TIRIFIC_DEF_MINDELTA_DVRO; break;
    case PDVRA: *value = TIRIFIC_DEF_MINDELTA_DVRA; break;
    case PDVVE: *value = TIRIFIC_DEF_MINDELTA_DVVE; break;
    case PZDRO: *value = TIRIFIC_DEF_MINDELTA_ZDRO; break;
    case PZDRA: *value = TIRIFIC_DEF_MINDELTA_ZDRA; break;
    case PZDVE: *value = TIRIFIC_DEF_MINDELTA_ZDVE; break;
    case PZ0  : *value = TIRIFIC_DEF_MINDELTA_Z0  ; break;
    case PSBR : *value = TIRIFIC_DEF_MINDELTA_SBR ; break;
    case PSM1A: *value = TIRIFIC_DEF_MINDELTA_SM1A; break;
    case PSM1P: *value = TIRIFIC_DEF_MINDELTA_SM1P; break;
    case PSM2A: *value = TIRIFIC_DEF_MINDELTA_SM2A; break;
    case PSM2P: *value = TIRIFIC_DEF_MINDELTA_SM2P; break;
    case PSM3A: *value = TIRIFIC_DEF_MINDELTA_SM3A; break;
    case PSM3P: *value = TIRIFIC_DEF_MINDELTA_SM3P; break;
    case PSM4A: *value = TIRIFIC_DEF_MINDELTA_SM4A; break;
    case PSM4P: *value = TIRIFIC_DEF_MINDELTA_SM4P; break;
    case PGA1A: *value = TIRIFIC_DEF_MINDELTA_GA1A; break;
    case PGA1P: *value = TIRIFIC_DEF_MINDELTA_GA1P; break;
    case PGA1D: *value = TIRIFIC_DEF_MINDELTA_GA1D; break;
    case PGA2A: *value = TIRIFIC_DEF_MINDELTA_GA2A; break;
    case PGA2P: *value = TIRIFIC_DEF_MINDELTA_GA2P; break;
    case PGA2D: *value = TIRIFIC_DEF_MINDELTA_GA2D; break;
    case PGA3A: *value = TIRIFIC_DEF_MINDELTA_GA3A; break;
    case PGA3P: *value = TIRIFIC_DEF_MINDELTA_GA3P; break;
    case PGA3D: *value = TIRIFIC_DEF_MINDELTA_GA3D; break;
    case PGA4A: *value = TIRIFIC_DEF_MINDELTA_GA4A; break;
    case PGA4P: *value = TIRIFIC_DEF_MINDELTA_GA4P; break;
    case PGA4D: *value = TIRIFIC_DEF_MINDELTA_GA4D; break;
    case PAZ1P: *value = TIRIFIC_DEF_MINDELTA_AZ1P; break;
    case PAZ1W: *value = TIRIFIC_DEF_MINDELTA_AZ1W; break;
    case PAZ2P: *value = TIRIFIC_DEF_MINDELTA_AZ2P; break;
    case PAZ2W: *value = TIRIFIC_DEF_MINDELTA_AZ2W; break;
    case PINCL: *value = TIRIFIC_DEF_MINDELTA_INCL; break;
    case PPA  : *value = TIRIFIC_DEF_MINDELTA_PA  ; break;
    case PXPOS: *value = TIRIFIC_DEF_MINDELTA_XPOS; break;
    case PYPOS: *value = TIRIFIC_DEF_MINDELTA_YPOS; break;
    case PVSYS: *value = TIRIFIC_DEF_MINDELTA_VSYS; break;
    case PSDIS: *value = TIRIFIC_DEF_MINDELTA_SDIS; break;
    case PCLNR: *value = TIRIFIC_DEF_MINDELTA_CLNR; break;
    case PVM0A: *value = TIRIFIC_DEF_MINDELTA_VM0A; break;
    case PVM1A: *value = TIRIFIC_DEF_MINDELTA_VM1A; break;
    case PVM1P: *value = TIRIFIC_DEF_MINDELTA_VM1P; break;
    case PVM2A: *value = TIRIFIC_DEF_MINDELTA_VM2A; break;
    case PVM2P: *value = TIRIFIC_DEF_MINDELTA_VM2P; break;
    case PVM3A: *value = TIRIFIC_DEF_MINDELTA_VM3A; break;
    case PVM3P: *value = TIRIFIC_DEF_MINDELTA_VM3P; break;
    case PVM4A: *value = TIRIFIC_DEF_MINDELTA_VM4A; break;
    case PVM4P: *value = TIRIFIC_DEF_MINDELTA_VM4P; break;
    case PWM0A: *value = TIRIFIC_DEF_MINDELTA_WM0A; break;
    case PWM1A: *value = TIRIFIC_DEF_MINDELTA_WM1A; break;
    case PWM1P: *value = TIRIFIC_DEF_MINDELTA_WM1P; break;
    case PWM2A: *value = TIRIFIC_DEF_MINDELTA_WM2A; break;
    case PWM2P: *value = TIRIFIC_DEF_MINDELTA_WM2P; break;
    case PWM3A: *value = TIRIFIC_DEF_MINDELTA_WM3A; break;
    case PWM3P: *value = TIRIFIC_DEF_MINDELTA_WM3P; break;
    case PWM4A: *value = TIRIFIC_DEF_MINDELTA_WM4A; break;
    case PWM4P: *value = TIRIFIC_DEF_MINDELTA_WM4P; break;
    case PLS0 : *value = TIRIFIC_DEF_MINDELTA_LS0 ; break;
    case PLC0 : *value = TIRIFIC_DEF_MINDELTA_LC0 ; break;
    case PRO1A: *value = TIRIFIC_DEF_MINDELTA_RO1A; break;
    case PRO1P: *value = TIRIFIC_DEF_MINDELTA_RO1P; break;
    case PRO2A: *value = TIRIFIC_DEF_MINDELTA_RO2A; break;
    case PRO2P: *value = TIRIFIC_DEF_MINDELTA_RO2P; break;
    case PRO3A: *value = TIRIFIC_DEF_MINDELTA_RO3A; break;
    case PRO3P: *value = TIRIFIC_DEF_MINDELTA_RO3P; break;
    case PRO4A: *value = TIRIFIC_DEF_MINDELTA_RO4A; break;
    case PRO4P: *value = TIRIFIC_DEF_MINDELTA_RO4P; break;
    case PRA1A: *value = TIRIFIC_DEF_MINDELTA_RA1A; break;
    case PRA1P: *value = TIRIFIC_DEF_MINDELTA_RA1P; break;
    case PRA2A: *value = TIRIFIC_DEF_MINDELTA_RA2A; break;
    case PRA2P: *value = TIRIFIC_DEF_MINDELTA_RA2P; break;
    case PRA3A: *value = TIRIFIC_DEF_MINDELTA_RA3A; break;
    case PRA3P: *value = TIRIFIC_DEF_MINDELTA_RA3P; break;
    case PRA4A: *value = TIRIFIC_DEF_MINDELTA_RA4A; break;
    case PRA4P: *value = TIRIFIC_DEF_MINDELTA_RA4P; break;
    default: return 1;
    }
    break;
  default:
    return 1;
  }
  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
   ------------------------------------------------------------ */

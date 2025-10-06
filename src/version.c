/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file version.c
   @brief Version information for TiRiFiC
*/

#include "version.h"

const char* get_tirific_version(void) {
    return TIRIFIC_VERSION;
}

const char* get_tirific_version_banner(void) {
    return "########################\n"
           "# TiRiFiC v. " TIRIFIC_VERSION " #\n"
           "########################";
}
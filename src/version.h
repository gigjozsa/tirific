/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file version.h
   @brief Version information header for TiRiFiC
*/

#ifndef VERSION_H
#define VERSION_H

#define TIRIFIC_VERSION "2.3.12_fat"

/**
 * @brief Get the version string
 * @return Version string
 */
const char* get_tirific_version(void);

/**
 * @brief Get the formatted version banner
 * @return Formatted banner string
 */
const char* get_tirific_version_banner(void);

#endif /* VERSION_H */
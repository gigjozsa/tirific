/* $Id: qfits_memory.c,v 1.4 2006/08/24 15:01:56 yjung Exp $
 *
 * This file is part of the ESO QFITS Library
 * Copyright (C) 2001-2004 European Southern Observatory
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * $Author: yjung $
 * $Date: 2006/08/24 15:01:56 $
 * $Revision: 1.4 $
 * $Name: qfits-6_2_0 $
 */

/*-----------------------------------------------------------------------------
                                Includes
 -----------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
/* #include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/resource.h> */

/*----------------------------------------------------------------------------*/
/**
  @brief    Map a file's contents to memory as a char pointer.
  @param    name        Name of the file to map
  @param    offs        Offset to the first mapped byte in file.
  @param    size        Returned size of the mapped file in bytes.
  @param    srcname     Name of the source file making the call.
  @param    srclin      Line # where the call was made.
  @return   A pointer to char, to be freed using qfits_memory_free().

  This function takes in input the name of a file. It tries to map the file 
  into memory and if it succeeds, returns the file's contents as a char pointer.
  It also modifies the input size variable to be the size of the mapped file in
  bytes. This function is normally never directly called but through the 
  falloc() macro.

  The offset indicates the starting point for the mapping, i.e. if you are not 
  interested in mapping the whole file but only from a given place.

  The returned pointer ptr must be deallocated with qfits_memory_fdealloc(ptr)
 */
/*----------------------------------------------------------------------------*/
char * qfits_memory_falloc(
        char        *   name,
        size_t          offs,
        size_t      *   size,
        const char  *   srcname,
        int             srclin)
{
  char *t;
  struct stat sta;
  FILE *thefile;

  if (stat(name, &sta)==-1) {
        return NULL;
  }
  if ((t = (char *) malloc((sta.st_size-offs+1)*sizeof(char)))) {
    if ((thefile = fopen(name, "r"))) {
      fseek(thefile, offs, SEEK_SET);
      fread(t, sizeof(char), sta.st_size-offs+1, thefile);
      fclose(thefile);
      if (size!=NULL) {
        (*size) = sta.st_size ;
      }
    }
  }
  return t;
}


/*----------------------------------------------------------------------------*/
/**
  @brief    Free memory that has been allocated with falloc
  @param    ptr         Pointer to free.
  @param    offs        Offset to the first mapped byte in file.
  @param    size        size to unmap
  @param    filename    Name of the file where the dealloc took place.
  @param    lineno      Line number in the file.
  @return   void
 */
/*----------------------------------------------------------------------------*/
void qfits_memory_fdealloc(
        void        *   ptr, 
        size_t          offs,
        size_t          size, 
        const char  *   filename, 
        int             lineno)
{
  free(ptr);
}


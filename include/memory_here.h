/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file memory_here.h
   @brief A module for sophisticated memory handling

  The functions malloc_here, calloc_here, realloc_here, free_here,
  forget, remember, tidyforgetlist, getmaxmem, setmaxmem are
  (modified) functions for dynamic memory allocation. A global
  variable maxmemalloc is provided which controls the maximum of
  dynamical allocated memory. It can be accessed with the get_maxmem()
  and the set_ maxmem() functions. While the functions malloc_here
  realloc_here calloc_here and free_here will mainly act under these
  premises like their stdlib pendants, the speciality here is that the
  user can mark dynamical allocated structures as being dispensible,
  if the maximum of allocated memory is reached. Then, before giving
  up, the allocation routines will first deallocate the marked
  structures in the list and turn the pointers to the structures to
  NULL, to give the user a handle on events. This means memory is
  freed if needed but not before. This is very useful if there is a
  large amount of objects that can be reproduced at any time but
  should be kept in memory up to the largest possible amount to safe
  computing time.  Dynamically allocated pointers will be marked by a
  call of the forget function on the adress of the pointer like
  forget((void **) &anypointer). If in the course of a program the
  mark should be moved away, if an object is needed to stay for a
  while, a call of remember((void **) &anypointer) will do that, if
  the object is not already deallocated. This .h file also delivers
  the possibility to activate a preprocessor substitution of the
  normal memory functions by the local ones. If the symbolic constant
  _MEMORY_HERE_ON is set, this leads to a substitution of the normal
  memory functions with the local ones. Another feature is, that if
  the symbolic constant MEMORY_STAT is defined to a value "ON"
  (#define MEMORY_STAT ON), the additional function memstat() will be
  available, to track down memory leakage.


      $Source: /Volumes/DATA_J_II/data/CVS/tirific/include/memory_here.h,v $
      $Date: 2009/05/26 07:56:40 $
      $Revision: 1.7 $
      $Author: jozsa $
      $Log: memory_here.h,v $
      Revision 1.7  2009/05/26 07:56:40  jozsa
      Left work

      Revision 1.6  2007/08/22 15:58:35  gjozsa
      Left work

      Revision 1.5  2005/04/28 10:13:21  gjozsa
      Some bugfix

      Revision 1.4  2004/12/28 12:03:21  gjozsa
      Added normal allocation and deallocation functions for use in aliasing mode

      Revision 1.3  2004/11/10 12:18:53  gjozsa
      Added memstat functionality

      Revision 1.2  2004/11/09 17:53:56  gjozsa
      Left work

      Revision 1.1.1.1  2004/10/29 11:13:05  gjozsa
      Added to CVS control

*/
/* ------------------------------------------------------------ */

/* Include guard */
#ifndef MEMORY_HERE_H
#define MEMORY_HERE_H



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INCLUDES EXTERNAL */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <stddef.h>



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* DEFINES */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @def MEMORY_STAT
  @brief Turns on the possibility of a memory statistics if set 
  to ON, otherways memory statistics is disabled
*/
/* ------------------------------------------------------------ */
#define MEMORY_STAT ON


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* MACROS */
/* ------------------------------------------------------------ */



/* Protect including files from these macros */
#ifdef _MEMORY_HERE_ON



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def malloc(a)
   @brief substitutes malloc calls with malloc_here

   For portability reasons: substitutes memory function with
   equivalent of the memory_here functions (See memory_here.h) if the
   symbolic constant _MEMORY_HERE_ON is set.

*/
/* ------------------------------------------------------------ */
#define malloc(a) malloc_here(a)



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def calloc(a,b)
   @brief substitutes calloc calls with calloc_here

   For portability reasons: substitutes memory function with
   equivalent of the memory_here functions (See memory_here.h) if the
   symbolic constant _MEMORY_HERE_ON is set.

*/
/* ------------------------------------------------------------ */
#define calloc(a,b) calloc_here(a,b)



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def free(a)
   @brief substitutes free calls with free_here

   For portability reasons: substitutes memory function with
   equivalent of the memory_here functions (See memory_here.h) if the
   symbolic constant _MEMORY_HERE_ON is set.

*/
/* ------------------------------------------------------------ */
#define free(a) free_here(a)



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def realloc(a,b)
   @brief substitutes realloc calls with realloc_here

   For portability reasons: substitutes memory function with
   equivalent of the memory_here functions (See memory_here.h) if the
   symbolic constant _MEMORY_HERE_ON is set.

*/
/* ------------------------------------------------------------ */
#define realloc(a,b) realloc_here(a,b)



#endif



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTIONS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void *malloc_here(size_t size)
  @brief Allocate memory up to a user defined size
  
  Allocates memory of size_t and returns a pointer to the allocated
  memory. The function is under control of a number of external
  variables. malloc_here will return NULL if the maximum of allocable
  memory maxmemalloc (not constant and under control of the user)
  would be reached, however it will first try to deallocate a number
  of dispensable structures, indicated as being dispensable by using
  the forget function. If deallocated the pointer to a previously
  allocated structure will be turned to NULL.
   
  @param size (size_t) size of the object to allocate

  @return (success) void *: pointer to the allocated object\n                            (error)         : NULL
*/
/* ------------------------------------------------------------ */
void *malloc_here(size_t size);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void *malloc_normal(size_t size)
  @brief Ordinary malloc if aliasing is turned on
  
  If aliasing is turned on the allocation functions will be aliased to
  the according functions in this module. This function is the usual
  function that is not aliased.
   
  @param size (size_t) size of the object to allocate

  @return (success) void *: pointer to the allocated object\n                            (error)         : NULL
*/
/* ------------------------------------------------------------ */
void *malloc_normal(size_t size);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void *calloc_here(size_t nmemb, size_t size)
  @brief allocate memory for n obsects of arbitrary size
  
  Allocates memory of nmemb*size_t and returns a pointer to the
  allocated memory. The function is under control of a number of
  external variables. calloc_here will return NULL if the maximum of
  allocable memory maxmemalloc (not constant and under control of the
  user) would be reached, however it will first try to deallocate a
  number of dispensable structures, indicated as being dispensable by
  using the forget function. If deallocated the pointer to a
  previously allocated structure will be turned to NULL.

  @param nmemb (size_t) number of objects to allocate
  @param size (size_t) size of the single object to allocate

  @return (success) void * pointer to the allocated object \n                            (error) NULL     
*/
/* ------------------------------------------------------------ */
void *calloc_here(size_t nmemb, size_t size);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void *calloc_normal(size_t size)
  @brief Ordinary calloc if aliasing is turned on
  
  If aliasing is turned on the allocation functions will be aliased to
  the according functions in this module. This function is the usual
  function that is not aliased.
   
  @param nmemb (size_t) number of objects to allocate
  @param size (size_t) size of the single object to allocate

  @return (success) void * pointer to the allocated object \n                            (error) NULL     
*/
/* ------------------------------------------------------------ */
void *calloc_normal(size_t nmemb, size_t size);




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void free_here(void *ptr)
  @brief Frees memory allocated with malloc_here

  Deallocates allocated memory for the structure ptr. The function is
  under control of a number of external variables and has hidden
  properties. It will control the forget mechanism, from time to time
  invoke a tidying of the forgetlist and in case that a forgotten
  pointer is deallocated turn this pointer into the NULL pointer and
  remove it from the forget list.

  @param ptr (void *) pointer to a structure to be deallocated

  @return void     
*/
/* ------------------------------------------------------------ */
void free_here(void *ptr);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void *free_normal(size_t size)
  @brief Ordinary free if aliasing is turned on
  
  If aliasing is turned on the allocation functions will be aliased to
  the according functions in this module. This function is the usual
  function that is not aliased.
   
  @param ptr (void *) pointer to a structure to be deallocated

  @return void     
*/
/* ------------------------------------------------------------ */
void free_normal(void *ptr);




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void *realloc_here(void *ptr, size_t size)
  @brief Realloc a structure under controllable size of available memory 

  Reallocates memory for the content of ptr. The function is under
  control of a number of external variables. realloc_here will return
  NULL if the maximum of allocable memory maxmemalloc (not constant
  and under control of the user) would be reached, however it will
  first try to deallocate a number of dispensable structures,
  indicated as being dispensable by using the forget function. If
  deallocated the pointer to a previously allocated structure will be
  turned to NULL.

  @param ptr  (void *) Pointer to an ALLOCATED structure
  @param size (size_t) New size of the object to allocate

  @return (success) void *realloc_here: pointer to the allocated object \n
          (error): NULL
*/
/* ------------------------------------------------------------ */
void *realloc_here(void *ptr, size_t size);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void *realloc_normal(size_t size)
  @brief Ordinary free if aliasing is turned on
  
  If aliasing is turned on the allocation functions will be aliased to
  the according functions in this module. This function is the usual
  function that is not aliased.
   
  @param ptr  (void *) Pointer to an ALLOCATED structure
  @param size (size_t) New size of the object to allocate

  @return (success) void *realloc_here: pointer to the allocated object \n
          (error): NULL
*/
/* ------------------------------------------------------------ */
void *realloc_normal(void *ptr, size_t size);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn void forget(void **ptr)
  @brief Puts an allocated structure in a list of structures free for deletion

  Heart function for the use of the forget mechanism. With this call
  on the adress of a pointer to a dynamically allocated structure, the
  structure will be put in a list of dispensable structures and will
  be deallocated if there is the need to adress more memory than
  specified with the maxmemalloc variable during another dynamical
  memory allocation call. If the deallocation will take place the
  pointer will be set to NULL.

  @param ptr (void **) Adress of a dispensible structure

  @return void     
*/
/* ------------------------------------------------------------ */
void forget(void **ptr);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn void *remember(void **ptr)
   @brief Deletes an object from the malloc_here forgetlist, it cannot be deleted anymore
   
   Heart function for the use of the forget mechanism. With this call
   on the adress of a pointer to a dynamically allocated structure,
   the structure will be taken out of a list of dispensable structures
   and will not any more be endangered to be be deallocated if there
   is the need to adress more memory than specified with the
   maxmemalloc variable during another dynamical memory allocation
   call. If the deallocation has already taken place the returned
   pointer will be NULL. It is not an error to try this call on a
   structure not in the forgetlist

   @param ptr (void **) Adress of a structure to safe

   @return (success) void *remember: Pointer to the structure \n
           (error): NULL  
*/
/* ------------------------------------------------------------ */
void *remember(void **ptr);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn size_t get_maxmem(void)
   @brief Get the size of allocable memory
   
   Interface to handle the maximum memory size. Shows the allocable memory

   @return size_t get_maxmem: Size of memory to be allocated
*/
/* ------------------------------------------------------------ */
size_t get_maxmem(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn size_t get_memalloc(void)
   @brief Get the size of allocable memory
   
   Interface to handle the used memory size. Shows the allocated memory

   @return size_t get_maxmem: Size of memory to be allocated
*/
/* ------------------------------------------------------------ */
size_t get_memalloc(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn void set_maxmem(size_t set)
   @brief Set the size of allocable memory
   
   Interface to handle the maximum memory size. Sets the maximum allocable memory

   @param set (size_t) New maximum allocable memory
*/
/* ------------------------------------------------------------ */
void set_maxmem(size_t set);



/* Declare this function only if it is wanted */

#if MEMORY_STAT == ON

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn void memstat(void)
   @brief Report the total number of mallocs and free calls
   
   Very simple function to control memory leakage. If the symbolic
   constant MEMORY_STAT is set to ON in this header file, the module
   memory_here will count every malloc and free call that is made by
   the module. This function puts a simple statistics on the terminal
   that contains all malloc (calloc) and free calls that are made by
   the module. In case of a switched off aliasing of the standard
   memory functions to the one's in the module, i.e., if the symbolic
   constant _MEMORY_HERE is not defined in the code of the calling
   module, only the specific allocation and deallocation calls
   (malloc_here, calloc_here, free_here) are counted.

   @return void This function produces an output to stderr, nothing else
*/
/* ------------------------------------------------------------ */
void memstat(void);

#endif


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: memory_here.h,v $
   Revision 1.7  2009/05/26 07:56:40  jozsa
   Left work

   Revision 1.6  2007/08/22 15:58:35  gjozsa
   Left work

   Revision 1.5  2005/04/28 10:13:21  gjozsa
   Some bugfix

   Revision 1.4  2004/12/28 12:03:21  gjozsa
   Added normal allocation and deallocation functions for use in aliasing mode

   Revision 1.3  2004/11/10 12:18:53  gjozsa
   Added memstat functionality

   Revision 1.2  2004/11/09 17:53:56  gjozsa
   Left work

   Revision 1.1.1.1  2004/10/29 11:13:05  gjozsa
   Added to CVS control


   ------------------------------------------------------------ */

/* Include guard */
#endif


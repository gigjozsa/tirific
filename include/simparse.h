/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file simparse.h
   @brief A utility for parsing files

   All-purpose parsing functions, excluded from the main source for
   portability reasons.
   Usage:
   compiling the module:
   gcc -c -o simparse.o simparse.c -I. --pedantic
   "." denotes the directory simparse.h resides in
   compiling something that uses the module:
   gcc -o bla bla.c simparse.o -I.
   gcc -c -o simparse.o simparse.c -I. -Wall -pedantic -ansi; gcc -o tester tester.c simparse.o -I.

   $Source: /Volumes/DATA_J_II/data/CVS/tirific/include/simparse.h,v $
   $Date: 2010/03/18 15:50:54 $
   $Revision: 1.2 $
   $Author: jozsa $
   $Log: simparse.h,v $
   Revision 1.2  2010/03/18 15:50:54  jozsa
   Left work

   Revision 1.4  2010/02/17 01:21:37  jozsa
   finished decomp complex

   Revision 1.3  2009/03/27 11:17:12  jozsa
   Left work

   Revision 1.2  2009/03/09 12:02:46  jozsa
   included decomposer input functions

   Revision 1.1.1.1  2009/03/06 12:57:04  jozsa
   imported

   Revision 1.5  2007/08/22 15:58:36  gjozsa
   Left work

   Revision 1.4  2004/12/07 10:31:43  gjozsa
   Left work

   Revision 1.3  2004/11/16 18:18:32  gjozsa
   Left work

   Revision 1.2  2004/11/09 17:53:56  gjozsa
   Left work

   Revision 1.1.1.1  2004/10/29 11:13:05  gjozsa
   Added to CVS control

*/
/* ------------------------------------------------------------ */



/* Include guard */
#ifndef SIMPARSE_H
#define SIMPARSE_H


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <stdio.h>
#include <time.h>


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */
#define SIMPARSE_SCN_ERROR_NONE                0
#define SIMPARSE_SCN_ERROR_NULL_PASSED         1
#define SIMPARSE_SCN_ERROR_WRONG_ARRAY         2
#define SIMPARSE_SCN_ERROR_NO_KEY              4
#define SIMPARSE_SCN_ERROR_TYPE                8
#define SIMPARSE_SCN_ERROR_NOT_ENOUGH_ELEMENTS 16
#define SIMPARSE_SCN_ERROR_TOO_MANY_ELEMENTS   32
#define SIMPARSE_SCN_ERROR_MEMORY              64

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* MACROS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* STRUCTS */
/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct simparse_scn_keyvalli
   @brief Struct of keyword-value pairs (both strings) in addition with a line

*/
/* ------------------------------------------------------------ */
typedef struct simparse_scn_keyvalli
{
  /** @brief The key */
  char *key;

  /** @brief The value */
  char *val;

  /** @brief The line */
  int line;
}
simparse_scn_keyvalli;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct simparse_scn_arel
   @brief An array of (pointers to) these structs defines the input to scan for keywords

*/
/* ------------------------------------------------------------ */
typedef struct simparse_scn_arel
{
  /** @brief Name of preamble */
  char *preamblename;

  /** @brief Corresponding preamble, NULL if not existent */
  char *preamble;

  /** @brief Indicator if preamble has been updated (1: yes 0: no) */
  int preamble_is_updated;

  /** @brief Null-terminated key-value pairs belonging to preamble */
  simparse_scn_keyvalli **keyvallipre;

  /** @brief Corresponding file name, NULL if not existent */
  char *orifilename;

  /* @brief Last last-changed time stamp of orifilename */
  time_t timestamp;

  /** @brief Null-terminated key-value pairs belonging to file */
  simparse_scn_keyvalli **keyvallifile;

  /** @brief Indicator if on Error user should be prompted */
  int onerror_prompt;
}
simparse_scn_arel;




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct decomp_listel
   @brief an array of these structs will be returned by decomp_get()

*/
/* ------------------------------------------------------------ */
typedef struct dcp_listel
{
  /** @brief number of elements, negative terminating the list */
  int nuel;

  /** @brief number of input group separated by grse in function decomp_putsep() */
  int grnr;

  /** @brief the list */
  int *poli;
}
decomp_listel;


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct decomp_inlist
   @brief an array of these structs will be returned by decomp_dep()

*/
/* ------------------------------------------------------------ */
typedef struct dcp_inl
{
  /** @brief number of elements */
  int nuel;

  /** @brief list of indexed indices */
  int *ipa;

  /** @brief list of next active indices inside input group, lower index */
  int *inpal;

  /** @brief list of next active indices inside input group, higher index */
  int *inpah;

  /** @brief list of indexed indices, relative position in group, starting at 0 */
  int *ripa;

  /** @brief list of next active indices inside input group, lower index, relative position, starting at 0 */
  int *rinpal;

  /** @brief list of next active indices inside input group, higher index, relative position in group, starting at 0 */
  int *rinpah;

}
decomp_inlist;


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* TYPEDEFS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @typedef decomp_in
   @brief Top layer to internal structs for decomp function

*/
/* ------------------------------------------------------------ */
typedef void *decomp_control;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn char **parsenext(char *fieldseperator, char *lineseperator,char *whitespaces, char *clamper, char *commentsep, char *endcomment, int maxfield, FILE *filepointer, int *line, char sep) 
  @brief Reads in the next words from a file into an array of words
  
  Returns an allocated array of allocated char arrays parsenext (freed
  by freeparsed) containing the words seperated by fieldseperator
  until the next line denoted by lineseperator (or EOF) is reached or
  a number maxfield of output fields is reached. The FILEpointer is
  set to the start of the next line or to the end of the stream. On
  error (memory problems) or if nothing is to be read anymore, NULL is
  returned. Whitespaces are ignored, i.e. kjgh lkfj in a field is
  returned as kjghlkfj, if ' ' is a whitespace. Inside two clamper
  symbols whitespaces, commentsep and fieldseperators are
  ignored. Inside two comment symbols everything but the lineseperator
  is ignored. The last component of parsenext is always NULL to denote
  the end of the array.

  @param fieldseperator (char *) The field seperators seperating
                                   single words
  @param lineseperator  (char *) The line seperators seperating lines
  @param whitespaces    (char *) Whitespaces ignored when reading
  @param clamper        (char *) Inside two clamper symbols whitespaces
                                 are taken literally and fs are ignored
  @param commentsep     (char *) Inside two commentsep's everything 
                                 is ignored but the linesep
  @param endcomment     (char *) These characters end a comment, but they don't start it.
  @param maxfield       (int)    Maximum number of fields to read 
                                 before passing the result
  @param filepointer    (FILE *) The input filepointer
  @param line           (int *)  The line where the search started, 
                                 any number, will be increased by 
                                 the number of encountered '@\ns'.
  @param sep (char) If set (!=0) suppress every empty field.
  
  @return (success) char **parsenext: allocated array of 
                                      allocated char arrays\n
  (error) NULL if EOF is reached or an error occured
*/
/* ------------------------------------------------------------ */
char **parsenext(char *fieldseperator, char *lineseperator, \
                 char *whitespaces, char *clamper, char *commentsep, \
                 char *endcomment, int maxfield, FILE *filepointer, int *line, char sep);


   
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn char **sparsenext(char *fieldseperator, char *lineseperator,char *whitespaces, char *clamper, char *commentsep, char *endcomment, int maxfield, char **string, int *line, char sep, char reset) 
  @brief Reads in the next words from a string into an array of words
  
  Returns an allocated array of allocated char arrays parsenext (freed
  by freeparsed) containing the words seperated by fieldseperator
  until the next line denoted by lineseperator (or \0) is reached or a
  number maxfield of output fields is reached. The content of string
  is set to the start of the next line or to the end of the array. On
  error (memory problems) or if nothing is to be read anymore, NULL is
  returned. Whitespaces are ignored, i.e. kjgh lkfj in a field is
  returned as kjghlkfj, if ' ' is a whitespace. Inside two clamper
  symbols whitespaces, commentsep and fieldseperators are
  ignored. Inside two comment symbols everything but the lineseperator
  is ignored. The last component of parsenext is always NULL to denote
  the end of the array. Note that the pointer returned will point to
  the next line, if reset is set to 0. To free allocated memory you
  have to free the original pointer.

  @param fieldseperator (char *) The field seperators seperating
                                   single words
  @param lineseperator  (char *) The line seperators seperating lines
  @param whitespaces    (char *) Whitespaces ignored when reading
  @param clamper        (char *) Inside two clamper symbols whitespaces
                                 are taken literally and fs are ignored
  @param commentsep     (char *) Inside two commentsep's everything 
                                 is ignored but the linesep
  @param endcomment     (char *) These characters end a comment, but they don't start it.
  @param maxfield       (int)    Maximum number of fields to read 
                                 before passing the result
  @param string         (char **) The input string, the content will be changed to the start of the next line if reset = 0
  @param line           (int *)  The line where the search started, 
                                 any number, will be increased by 
                                 the number of encountered '@\ns'.
  @param sep (char) If set (!=0) suppress every empty field.
  @param reset (char) If set (!=0) do not change the content of string.
  
  @return (success) char **parsenext: allocated array of 
                                      allocated char arrays\n
  (error) NULL if EOF is reached or an error occured
*/
/* ------------------------------------------------------------ */
char **sparsenext(char *fieldseperator, char *lineseperator, \
                 char *whitespaces, char *clamper, char *commentsep, \
		  char *endcomment, int maxfield, char **string, int *line, char sep, char reset);


   
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn void freeparsed(char **parsenext)
  @brief Destroys an char ** array that is created by parsenext
  
  @param parsenext (char **) An allocated array of allocated char 
                             arrays, last member NULL 
*/
/* ------------------------------------------------------------ */
void freeparsed(char **parsenext);

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn void simparse_scn_arellist_dest(simparse_scn_arel **simparse_scn_arellist);
  @brief Deallocate complete simparse_scn_arellist array, including all sub-structures
  
  @param simparse_scn_arellist (simparse_scn_arel **) Pointer to NULL-terminated array

  @return void
*/
/* ------------------------------------------------------------ */
void simparse_scn_arellist_dest(simparse_scn_arel **simparse_scn_arellist);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn simparse_scn_keyvalli **simparse_scn_keyvallilist_gfrfi(char *string)
  @brief Read in a keyvalli list from file
  
  Rules:
  The string will be split into key-value-line triplets, parsing file.

  A key is the last contiguous string (not containing any of ' ',
  '\n', or '\t') before any '=' character. The value of that key is
  anything between that '='-character and the next key, including
  whitespaces. The line is the number of encountered '\n's at the
  position of the '='-character plus one.  Anything starting with '#'
  and ending with '\n' is ignored. Anything starting with '#' and
  ending with '#' gets ignored.

  If NULL is passed, an array containing one element with the value of
  NULL is returned. The same is true if an empty string is returned.

  @param string (char *) Allocated '\0'-terminated char array or NULL

  @return (success) simparse_scn_keyvallilist_gfrfi (simparse_scn_keyvalli **) array of allocated simparse_scn_keyvalli structs
          (error) NULL (allocation problems)
*/
/* ------------------------------------------------------------ */
simparse_scn_keyvalli **simparse_scn_keyvallilist_gfrfi(char *string);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn void simparse_scn_keyvallilist_dest(simparse_scn_keyvalli **simparse_scn_keyvallilist);
  @brief Deallocate complete simparse_scn_keyvallilist array, including strings
  
  @param simparse_scn_keyvallilist (simparse_scn_arel **) Pointer to NULL-terminated array

  @return void
*/
/* ------------------------------------------------------------ */
void simparse_scn_keyvallilist_dest(simparse_scn_keyvalli **simparse_scn_keyvallilist);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn simparse_scn_arel **simparse_scn_arel_insert(simparse_scn_arel **scnarelv, char *preamblename, char *preamble, char *filename)
  @brief Read in a simparse_scn_arel struct (including the keyword-value list)
  
  A simparse_scnarel struct is allocated. 

  Then takes preamble and scans that array for keyword-value pairs. A
  keyword is any string separated by whitespaces (' ', '\t', '\n')
  from the rest before an '='-sign, while a space between the = sign
  and the corresponding key is not required (but does also no
  harm). The corresponding value is the complete string after the '='
  and before the next key. The line is the line in which the key
  resides (counting '\n's). A NULL-terminated array of such
  key-value-line triplets (see keyvalli definition), containing all
  key-value-line triplets from preamble is saved under keyvallipre of
  the allocated simparse_scnarel struct. All strings, all members of
  the array, and the array itself are dynamically allocated. A copy of
  preamblename (dynamically allocated) is saved as preamblename, a
  copy of preamble (dynamically allocated) is saved as
  preamble. preamble_is_updated is set to 0.

  Then takes filename, opens if possible, reads it into a char array,
  closes the file, and saves the timestamp under the timestamp member
  of the simparse_scnarel struct. Then scans the read array for
  keyword-value-line triplets. A keyword is any string separated by
  whitespaces (' ', '\t', '\n') from the rest before an '='-sign,
  while a space between the = sign and the corresponding key is not
  required (but does also no harm). The corresponding value is the
  complete string after the '=' and before the next key. The line is
  the line in which the key resides (counting '\n's). A
  NULL-terminated array of such key-value-line triplets (see keyvalli
  definition), containing all key-value-line triplets from preamble is
  saved under keyvallifile of the allocated simparse_scnarel
  struct. All strings, all members of the array, and the array itself
  are dynamically allocated. A copy of filename (dynamically
  allocated) is saved as orifilename in the simparse_scnarel struct.

  onerror_prompt is set to 1.

  Finally, the simparse_scnarel struct is appended to the
  NULL-terminated input array scnarelv and scnarelv is returned. If
  scnarelv is NULL, the array is created. preamblename, preamble, and
  filename can be NULL.

  @param scnarelv     (simparse_scnarel **) NULL-terminated array of allocated simparse_scnarel structs.
  @param preamblename (char *)              Name of preamble or NULL
  @param preamble     (char *)              Preamble or NULL
  @param filename     (char *)              Filename or NULL

  @return (success) simparse_scnarel **simparse_addscn updated NULL-terminated list
          (error) NULL (allocation problems)
*/
/* ------------------------------------------------------------ */
simparse_scn_arel **simparse_scn_arel_insert(simparse_scn_arel **scnarelv, char *preamblename, char *preamble, char *filename);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_onerror_prompt(simparse_scn_arel **simparse_scn_arellist, int on)
  @brief Switch onerror_prompt on (1) or off (0) for the (NULL-terminated) simparse_scn_arellistv

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list.
  @param on (int) Value to assign onerror_prompt for every member of the array (except for the NULL at the end of course)

  @return (success) int simparse_scn_arel_onerror_prompt(): 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_onerror_prompt(simparse_scn_arel **simparse_scn_arellist, int on);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_cancelkey(simparse_scn_arel **simparse_scn_arellist, char *key)
  @brief Search for key in the simparse_scn_arel reserved for user input (first element) and remove (blank) each entry with the key key

  equivalent to deepcancelkey(,,0)

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list.
  @param key (char *) Key to remove

  @return (success) int simparse_scn_arel_onerror_prompt(): 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_cancelkey(simparse_scn_arel **simparse_scn_arellist, char *key);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_deepcancelkey(simparse_scn_arel **simparse_scn_arellist, char *key, int depth)
  @brief Search for key in the simparse_scn_arel and remove (blank) each entry with the key key

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list.
  @param key (char *) Key to remove
  @param depth (int)  Number of simparse_scn_arellist structs to correct, starting at 0. This can be used to e.g. correct the first two structs (e.g. user input and command line, but not any input file with depth = 1) only.

  @return (success) int simparse_scn_arel_deepcancelkey(): 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_deepcancelkey(simparse_scn_arel **simparse_scn_arellist, char *key, int depth);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_cancelall(simparse_scn_arel **simparse_scn_arellist, int on)
  @brief Remove the complete key-value-line list in the member of simparse_scn_arellist assigned to user dialogue input (the first one)

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list.
  @param on (int) Value to assign onerror_prompt for every member of the array (except for the NULL at the end of course)

  @return (success) int simparse_scn_arel_onerror_prompt(): 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_cancelall(simparse_scn_arel **simparse_scn_arellist);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_readval_gen(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int type, int ndef, void *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, void **returned)
  @brief Generic function to read a value from a key in simparse_scn_arellist
  
  Background:
  
  Input to this function is an (NULL-terminated) array of
  simparse_scn_arel structs, as constructed through subsequent calls
  of the routine simparse_scn_arel_insert(). With that function a set
  of key - value (-line) pairs (triplets) in (NULL-terminated) arrays
  of simparse_scn_keyvalli structs as members of the simparse_scn_arel
  structs is constructed, containing the user input:

  From the user perspective, either a string and/or a file name is
  passed to simparse_scn_arel_insert(). That input is scanned for the
  occurrence of the '=' symbol, which is assumed to separate one
  keyword just before the '=' symbol and separated by whitespaces ('
  ', '\n, '\t') and a value string (including whitespaces). The '#'
  symbol is used as a commenting character.This leads to a
  (NULL-terminated) array of keyword-value-line triplets, where the
  keyword is a string without whitespaces and value a line entered
  after the '=' symbols.

  This function:

  The (NULL-terminated) array of simparse_scn_arel structs
  simparse_scn_arellist is passed to the function. For each member of
  the array, starting with the first, the (NULL-terminated)
  simparse_scn_keyvalli ** (key-value-line triplets) arrays
  keyvallipre and keyvallifile are updated if appropriate: if the
  switch preamble_is_updated is not 0, keyvallipre gets updated, and
  if the time stamp of the file with the name orifilename is later
  than timestamp (member of the simparse_scn_arel struct),
  keyvallifile gets updated. Then both simparse_scn_keyvalli structs
  are searched (in the order keyvallipre, then keyvallifile) for the
  keyword key. The udate-and-search cascade goes through the complete
  array. Once found (first match), the corresponding value val and
  line are evaluated. If not found, an error is issued if keyreq != 0,
  otherwise an empty string is used for the analysis.

  Once found, the corresponding value, at this stage one string, is
  converted to the output returned. returned is a (possibly unterminated)
  array of types as specified with the variable type. Type has the
  values and corresponding types of the array:

  SCN_TYPE_STRING         array type char *   Return a ('\0'-terminated) array containing the complete val, but leaving out any whitespaces (' ', '\n', '\t') at the beginning and at the end. Needs to be deallocated with free().
  SCN_TYPE_STRING_WHITESP array type char *   Return a ('\0'-terminated) array containing the complete val, but leaving out any whitespaces (' ', '\n', '\t') at the end (so this includes leading whitespaces). Needs to be deallocated with free().
  SCN_TYPE_STRINGLIST     array type char **  Return a (NULL-terminated) array of allocated strings (which are allocated and '\0'-terminated), each of which corresponds to a single entity in val, which is separated from the other entities by whitespaces (' ', '\n', '\t'). Needs to be deallocated with freeparsed().
  SCN_TYPE_INT            array type int *    Return an unterminated array, each element of which corresponds to a single entity in val, which is separated from the other entities by whitespaces (' ', '\n', '\t'), converted to int. Needs to be deallocated with free().
  SCN_TYPE_LONG           array type long *   Return an unterminated array, each element of which corresponds to a single entity in val, which is separated from the other entities by whitespaces (' ', '\n', '\t'), converted to long. Needs to be deallocated with free().
  SCN_TYPE_FLOAT          array type float *  Return an unterminated int array, each element of which corresponds to a single entity in val, which is separated from the other entities by whitespaces (' ', '\n', '\t'), converted to float. Needs to be deallocated with free().
  SCN_TYPE_DOUBLE         array type double * Return an unterminated int array, each element of which corresponds to a single entity in val, which is separated from the other entities by whitespaces (' ', '\n', '\t'), converted to double. Needs to be deallocated with free().

  Conversion happens in a hierarchical way, and depending on type, the
  current value is returned. The matched value is converted to a
  single string (as if output SCN_TYPE_STRING), then split into an
  array of strings (as if output SCN_TYPE_STRING), then converted to a
  double array (as if output SCN_TYPE_DOUBLE), then converted to
  either an int, long, or float array (as if output SCN_TYPE_INT,
  SCN_TYPE_LONG, SCN_TYPE_FLOAT) by rounding or downcasting.

  The user can provide a default array *def (provide NULL, if no
  default is provided) of length ndef as specified by the user. If
  more elements than ndef are required the last provided value will be
  applied. With nmin the user specifies the minimum number of elements
  to read in, and with nmax the maximum number of elements to read
  in. If nmax < 0, the read list is unlimited. If enterlowernmax is !=
  0, the number of elements provided by the user must be lower than
  nmax, otherwise the first nmax elements are read out omitting the
  last ones and without issuing an error. If not enough elements are
  provided by the user, the number of elements returned (if possible)
  is nmin.

  The output is one array returned, the number nreturned of elements
  in that array, and the number of user input elements nread. Those
  numbers are not necessarily the same. It might occur that the user
  provides nothing, but the returned list is a coplete copy of
  def. Then, *nread = 0, nreturned = nmin. nreturned is only the
  number of elements read in. In NULL-terminated arrays
  (SCN_TYPE_STRING, SCN_TYPE_STRINGLIST), the actual size of the array
  is that number plus one.

  Errors:

  Several high-level errors can occur. If an error occurs, the program
  will either comment and raise an exception exit(1) or enter the
  interactive mode. If the value of the members onerror_prompt in the
  elements of simparse_scn_arellist is 1, the interactive mode is
  entered. If it is 0, an exception will be raised. In the interactive
  mode, the user is asked to provide the char * input for a certain
  key (for which an error was detected in the values), issuing the
  message errmes, together with other error-related messages. After
  entering the string, the key-value pair is either created or
  replaced in the (NULL-terminated) simparse_scn_keyvalli array
  keyvallipre of the first member of simparse_scn_arellist. Then,
  simparse_scn_arellist is scanned again to either provide a correct
  return or to enter the interactive mode again.  

  Possible errors (which are logically ored on output):

  SIMPARSE_SCN_ERROR_NONE                No error.
  SIMPARSE_SCN_ERROR_NULL_PASSED         NULL is passed for simparse_scn_arellist (get a minimal one with a call to simparse_scn_arellist_start() and then subsequently simparse_scn_arellist_update())
  SIMPARSE_SCN_ERROR_WRONG_ARRAY         An inappropriate array is passed for simparse_scn_arellist (get a minimal one with a call to simparse_scn_arellist_start() and then subsequently simparse_scn_arellist_update())
  SIMPARSE_SCN_ERROR_NO_KEY              The key is not found in simparse_scn_arellist, but keyreq != 0.
  SIMPARSE_SCN_ERROR_TYPE                Some type conversion was impossible.
  SIMPARSE_SCN_ERROR_NOT_ENOUGH_ELEMENTS ndef = 0 or def = NULL and the user provides less than nmin input elements.
  SIMPARSE_SCN_ERROR_TOO_MANY_ELEMENTS   more than nmax elements are provided, but enterlowernmax != 0.
  SIMPARSE_SCN_ERROR_MEMORY              Memory problems.

  Storage of interactive input:

  Note that the (NULL-terminated) simparse_scn_keyvalli array
  keyvallipre of the first member of simparse_scn_arellist always
  contains the user input of the interactive mode. Single
  key-value-line triplets from that array can be removed using the
  function simparse_scn_arel_cancelkey, all key-value-line triplets in
  that array can be removed with the simparse_scn_arel_cancelall
  function.

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list to be searched.
  @param key                   (char *)               Keyword to search for.
  @param errmes                (char *)               Generic message to display on error.
  @param type                  (int)                  Type of output.
  @param ndef                  (int)                  Number of default array elements.
  @param def                   (void *)               Array of default values. Must be type type casted to void *.
  @param nmin                  (int)                  Minimum of elements to read.
  @param nmax                  (int)                  Maximum of elements to read.
  @param keyreq                (int)                  Must the key occur in the simparse_scn_arellist lists (0: no, !0: yes)?
  @param enterlowernmax        (int)                  Is it allowed to enter more than the maximum number of elements (0: yes, !0: no)?
  @param keypres               (int *)                Output: 0: key not entered, 1: key present, 2: key present in arel struct 0 (i.e. entered interactively by user).
  @param nread                 (int *)                Output: number of elements read from first match to key in user input (preambles, files).
  @param nreturned             (int *)                Output: number of elements returned to user. A number between nmin and nmax.
  @param returned              (void **)              Output: the returned array. An array will be allocated (deallocate with free or freeparsed).

  @return (success) int simparse_scn_arel_readval_gen: 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_readval_gen(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int type, int ndef, void *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, void **returned);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_readval_string(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, char *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, char **returned)
  @brief Function to read an int value list from a key in simparse_scn_arellist
  
  See simparse_scn_arel_readval_gen for type = SCN_TYPE_STRING

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list to be searched.
  @param key                   (char *)               Keyword to search for.
  @param errmes                (char *)               Generic message to display on error.
  @param ndef                  (int)                  Number of default array elements.
  @param def                   (char *)               Array of default values. Must be type type casted to void *.
  @param nmin                  (int)                  Minimum of elements to read.
  @param nmax                  (int)                  Maximum of elements to read.
  @param keyreq                (int)                  Must the key occur in the simparse_scn_arellist lists (0: no, !0: yes)?
  @param enterlowernmax        (int)                  Is it allowed to enter more than the maximum number of elements (0: yes, !0: no)?
  @param keypres               (int *)                Output: 0: key not entered, 1: key present, 2: key present in arel struct 0 (i.e. entered interactively by user).
  @param nread                 (int *)                Output: number of elements read from first match to key in user input (preambles, files).
  @param nreturned             (int *)                Output: number of elements returned to user. A number between nmin and nmax.
  @param returned              (char **)              Output: the returned array. An array will be allocated (deallocate with free or freeparsed).

  @return (success) int simparse_scn_arel_readval_string: 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_readval_string(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, char *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, char **returned);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_readval_stringwhitesp(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, char *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, char **returned)
  @brief Function to read an int value list from a key in simparse_scn_arellist
  
  See simparse_scn_arel_readval_gen for type = SCN_TYPE_STRINGWHITESP

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list to be searched.
  @param key                   (char *)               Keyword to search for.
  @param errmes                (char *)               Generic message to display on error.
  @param ndef                  (int)                  Number of default array elements.
  @param def                   (char *)               Array of default values. Must be type type casted to void *.
  @param nmin                  (int)                  Minimum of elements to read.
  @param nmax                  (int)                  Maximum of elements to read.
  @param keyreq                (int)                  Must the key occur in the simparse_scn_arellist lists (0: no, !0: yes)?
  @param enterlowernmax        (int)                  Is it allowed to enter more than the maximum number of elements (0: yes, !0: no)?
  @param keypres               (int *)                Output: 0: key not entered, 1: key present, 2: key present in arel struct 0 (i.e. entered interactively by user).
  @param nread                 (int *)                Output: number of elements read from first match to key in user input (preambles, files).
  @param nreturned             (int *)                Output: number of elements returned to user. A number between nmin and nmax.
  @param returned              (char **)              Output: the returned array. An array will be allocated (deallocate with free or freeparsed).

  @return (success) int simparse_scn_arel_readval_string: 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_readval_stringwhitesp(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, char *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, char **returned);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_readval_stringlist(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, char **def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, char ***returned)
  @brief Function to read an int value list from a key in simparse_scn_arellist
  
  See simparse_scn_arel_readval_gen for type = SCN_TYPE_STRINGLIST

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list to be searched.
  @param key                   (char *)               Keyword to search for.
  @param errmes                (char *)               Generic message to display on error.
  @param ndef                  (int)                  Number of default array elements.
  @param def                   (char **)              Array of default values. Must be type type casted to void *.
  @param nmin                  (int)                  Minimum of elements to read.
  @param nmax                  (int)                  Maximum of elements to read.
  @param keyreq                (int)                  Must the key occur in the simparse_scn_arellist lists (0: no, !0: yes)?
  @param enterlowernmax        (int)                  Is it allowed to enter more than the maximum number of elements (0: yes, !0: no)?
  @param keypres               (int *)                Output: 0: key not entered, 1: key present, 2: key present in arel struct 0 (i.e. entered interactively by user).
  @param nread                 (int *)                Output: number of elements read from first match to key in user input (preambles, files).
  @param nreturned             (int *)                Output: number of elements returned to user. A number between nmin and nmax.
  @param returned              (char ***)               Output: the returned array. An array will be allocated (deallocate with free or freeparsed).

  @return (success) int simparse_scn_arel_readval_stringlist: 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_readval_stringlist(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, char **def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, char ***returned);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_readval_int(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, int *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, int **returned)
  @brief Function to read an int value list from a key in simparse_scn_arellist
  
  See simparse_scn_arel_readval_gen for type = SCN_TYPE_INT

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list to be searched.
  @param key                   (char *)               Keyword to search for.
  @param errmes                (char *)               Generic message to display on error.
  @param ndef                  (int)                  Number of default array elements.
  @param def                   (int *)                Array of default values. Must be type type casted to void *.
  @param nmin                  (int)                  Minimum of elements to read.
  @param nmax                  (int)                  Maximum of elements to read.
  @param keyreq                (int)                  Must the key occur in the simparse_scn_arellist lists (0: no, !0: yes)?
  @param enterlowernmax        (int)                  Is it allowed to enter more than the maximum number of elements (0: yes, !0: no)?
  @param keypres               (int *)                Output: 0: key not entered, 1: key present, 2: key present in arel struct 0 (i.e. entered interactively by user).
  @param nread                 (int *)                Output: number of elements read from first match to key in user input (preambles, files).
  @param nreturned             (int *)                Output: number of elements returned to user. A number between nmin and nmax.
  @param returned              (int **)               Output: the returned array. An array will be allocated (deallocate with free or freeparsed).

  @return (success) int simparse_scn_arel_readval_int: 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_readval_int(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, int *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, int **returned);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_readval_long(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, long *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, long **returned)
  @brief Function to read an int value list from a key in simparse_scn_arellist
  
  See simparse_scn_arel_readval_gen for type = SCN_TYPE_LONG

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list to be searched.
  @param key                   (char *)               Keyword to search for.
  @param errmes                (char *)               Generic message to display on error.
  @param ndef                  (int)                  Number of default array elements.
  @param def                   (long *)                Array of default values. Must be type type casted to void *.
  @param nmin                  (int)                  Minimum of elements to read.
  @param nmax                  (int)                  Maximum of elements to read.
  @param keyreq                (int)                  Must the key occur in the simparse_scn_arellist lists (0: no, !0: yes)?
  @param enterlowernmax        (int)                  Is it allowed to enter more than the maximum number of elements (0: yes, !0: no)?
  @param keypres               (int *)                Output: 0: key not entered, 1: key present, 2: key present in arel struct 0 (i.e. entered interactively by user).
  @param nread                 (int *)                Output: number of elements read from first match to key in user input (preambles, files).
  @param nreturned             (int *)                Output: number of elements returned to user. A number between nmin and nmax.
  @param returned              (long **)               Output: the returned array. An array will be allocated (deallocate with free or freeparsed).

  @return (success) int simparse_scn_arel_readval_int: 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_readval_long(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, long *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, long **returned);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_readval_float(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, float *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, float **returned)
  @brief Function to read an int value list from a key in simparse_scn_arellist
  
  See simparse_scn_arel_readval_gen for type = SCN_TYPE_FLOAT

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list to be searched.
  @param key                   (char *)               Keyword to search for.
  @param errmes                (char *)               Generic message to display on error.
  @param ndef                  (int)                  Number of default array elements.
  @param def                   (float *)                Array of default values. Must be type type casted to void *.
  @param nmin                  (int)                  Minimum of elements to read.
  @param nmax                  (int)                  Maximum of elements to read.
  @param keyreq                (int)                  Must the key occur in the simparse_scn_arellist lists (0: no, !0: yes)?
  @param enterlowernmax        (int)                  Is it allowed to enter more than the maximum number of elements (0: yes, !0: no)?
  @param keypres               (int *)                Output: 0: key not entered, 1: key present, 2: key present in arel struct 0 (i.e. entered interactively by user).
  @param nread                 (int *)                Output: number of elements read from first match to key in user input (preambles, files).
  @param nreturned             (int *)                Output: number of elements returned to user. A number between nmin and nmax.
  @param returned              (float **)               Output: the returned array. An array will be allocated (deallocate with free or freeparsed).

  @return (success) int simparse_scn_arel_readval_int: 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_readval_float(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, float *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, float **returned);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int simparse_scn_arel_readval_double(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, double *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, double **returned)
  @brief Function to read an int value list from a key in simparse_scn_arellist
  
  See simparse_scn_arel_readval_gen for type = SCN_TYPE_DOUBLE

  @param simparse_scn_arellist (simparse_scn_arel **) Allocated simparse_scn_arel NULL-terminated list to be searched.
  @param key                   (char *)               Keyword to search for.
  @param errmes                (char *)               Generic message to display on error.
  @param ndef                  (int)                  Number of default array elements.
  @param def                   (double *)                Array of default values. Must be type type casted to void *.
  @param nmin                  (int)                  Minimum of elements to read.
  @param nmax                  (int)                  Maximum of elements to read.
  @param keyreq                (int)                  Must the key occur in the simparse_scn_arellist lists (0: no, !0: yes)?
  @param enterlowernmax        (int)                  Is it allowed to enter more than the maximum number of elements (0: yes, !0: no)?
  @param keypres               (int *)                Output: 0: key not entered, 1: key present, 2: key present in arel struct 0 (i.e. entered interactively by user).
  @param nread                 (int *)                Output: number of elements read from first match to key in user input (preambles, files).
  @param nreturned             (int *)                Output: number of elements returned to user. A number between nmin and nmax.
  @param returned              (double **)               Output: the returned array. An array will be allocated (deallocate with free or freeparsed).

  @return (success) int simparse_scn_arel_readval_double: 0
          (error): 1
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_readval_double(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, double *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, double **returned);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int putparsed(FILE *filepointer, char **array,
  char fieldseparator, char lineseparator,int current_length
  char whitespace, char clamper, char comment, int fields, 
  int linelength, )
  @brief Append an array of words with a certain format to a FILE

  Takes as an input an array of strings, while the last element is a
  terminating NULL pointer. Each string is understood as a word. The
  basic thing the function does is to append an output of the words to
  the filepointer. 
  
  Each word will be put to the stream separated by a fieldseparator. A
  value of 0 or '\0' for the fieldseparator tells the function that no
  fieldseparation symbols are set. The output will be terminated by a
  lineseparator, if lineseparator is not 0 ('\0'). If a whitespace is
  given (whitespace != 0), the whitespaces will be appended where
  appropriate. If a clamper is given, each word that contains
  separator signs or a comment sign will be put into the clamper
  symbols.  The output will contain up to fields words. A line in the
  output is broken by '\n' if an output exceeds linelength, adding the
  next word. This is only done if '\n' is none of the fieldseparator,
  lineseparator, whitespace, comment, or clamper. Any '\n' in the
  words is counted as a normal character with that respect, so if a
  word contains an '\n' that will spoil the control of the
  linelength. If comment is set, the whole output will be put into
  comment symbols (If whitespace is set separated by a whitespace) and
  any other special sign that is a comment sign will be ignored, while
  the user has to care for himself not to put comment signs into the
  words themselves.


  @param filepointer    (FILE *)  A pointer to an open file
  @param array          (char **) The array to append
  @param fieldseparator (char)    The seperator for single words
  @param lineseparator  (char)    The separator for lines
  @param whitespace     (char)    A whitespace
  @param clamper        (char)    A clamping symbol if words should be
  clamped
  @param comment        (char)    A comment symbol: If not 0, the
  whole output will be put in the comment symbols
  @param fields         (intt)  The maximum number of fields to be
  written, negative if not used
  @param linelength     (int)  The length of one line in the
  output, 0 means no linebreak
  @param current_length (int)  The column that is started with

  @return (success) int testparsenext: 1\n
          (error) 0
*/
/* ------------------------------------------------------------ */
int putparsed(FILE *filepointer, char **array,\
       char fieldseparator, char lineseparator,\
       char whitespace, char clamper, char comment,\
       int fields, int linelength, int current_length);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn char comp(char *constring1, char *constring2, int minmatch)
  @brief Compare two strings up to minmatch characters
  
  Compares constring1 with constring2. If the characters of constring1
  match the first characters of constring2 and the length of
  constring1 is at least minmatch, comp returns true, otherways not.

  @param constring1 (char *) Pointer to a character string
  @param constring2 (char *) Pointer to a character string
  @param minmatch   (int)    Minimum matching characters

  @return (success) char comp: 1 if words match
          (error)   0 if they don't
*/
/* ------------------------------------------------------------ */
char comp(char *constring1, char *constring2, int minmatch);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int comp2words(char *word1, char *word2)
  @brief Check if the first characters of word1 match with word2

  Compares word1 and word2 until the end of word 1 is reached and
  returns 1 for equal words and 0 for unequal words. (I got too far
  now, there is something similar in the standard library.)

  @param word1 (char *) Pointer to a character string
  @param word2 (char *) Pointer to a character string

  @return (success) char comp: 1 if words match
          (error)   0 if they don't
*/
/* ------------------------------------------------------------ */
int comp2words(char *word1, char *word2);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int comp2wordsnocase(char *word1, char *word2)
  @brief Check if the first characters of word1 match with word2

  Compares word1 and word2 until the end of word 1 is reached and
  returns 1 for equal words and 0 for unequal words. Makes no difference between lower and upper case, ignores '='-sign at the end (this is very tirific specific).

  @param word1 (char *) Pointer to a character string
  @param word2 (char *) Pointer to a character string

  @return (success) char comp: 0 if words match
          (error)   1 if they don't
*/
/* ------------------------------------------------------------ */
int comp2wordsnocase(char *word1, char *word2);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int simparse_scn_arel_timestamp_early(simparse_scn_arel *scnarelv)
  @brief Reset the file time stamp to a date sometimes 1901 (so earlier than any possible creation date)

  Basically calculates the time stamp using time.h (function mktime) for a tm struct with:
  tm_sec   = 00;
  tm_min   = 00;
  tm_hour  = 00;
  tm_mday  = 15;
  tm_mon   = 11;
  tm_year  = 1;

  This time stamp should exist on any computer and point to a date in
  November 1901. Then, the time stamp in scnarelv is replaced by this
  time stamp (in effect, the content of the keyvalli array is updated
  on the next update call).

  @param scnarelv     (simparse_scn_arel *) Allocated simparse_scnarel struct.

  @return (success) int simparse_scn_arel_timestamp_early: 0
          (error) 1 (only if inpput struct does not exist...)
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_timestamp_early(simparse_scn_arel *scnarelv);





/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int simparse_scn_arel_timestamp_late(simparse_scn_arel *scnarelv)
  @brief Reset the file time stamp to a date sometimes 1901 (so earlier than any possible creation date)

  Basically calculates the time stamp using time.h (function mktime) for a tm struct with:
  tm_sec   = 00;
  tm_min   = 00;
  tm_hour  = 00;
  tm_mday  = 15;
  tm_mon   = 11;
  tm_year  = 301;

  This time stamp should exist on any computer and point to a date in
  November 2201. Then, the time stamp in scnarelv is replaced by this
  time stamp (in effect, the content of the keyvalli array is not updated
  on the next update call).

  @param scnarelv     (simparse_scn_arel *) Allocated simparse_scnarel struct.

  @return (success) int simparse_scn_arel_timestamp_early: 0
          (error) 1 (only if inpput struct does not exist...)
*/
/* ------------------------------------------------------------ */
int simparse_scn_arel_timestamp_late(simparse_scn_arel *scnarelv);





/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn char charuppercase(char character)
  @brief Convert into uppercase

  Convert into upper case if letter. Return uppercase letter.

  @param characer (char *) Symbol to convert

  @return (success) char charuppercase: upper case char
*/
/* ------------------------------------------------------------ */
int charuppercase(char character);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int checkline(FILE *input)
  @brief Check the number of newlines until the next word is 
  reached

  Checks the number of '\n's until the next real word in input is
  reached and sets the position in input to the first character of the
  new word.

  @param input (FILE *) Pointer to an input file (contains position)

  @return (success) int checkline: number of '\n's 'till next word
          (error) NULL
*/
/* ------------------------------------------------------------ */
int checkline(FILE *input);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn float *nextnumbers(FILE *input, int *line, int numberofnumbers)
  @brief Read the next numberofnumbers numbers into an array

  Creates an array of numberofnumbers numbers (dynamically), reads in
  the next numberofnumbers numbers from *input and changes line
  according to the number of '\n's within the scan.. On error the
  filepointer is set back to the start and line is reset to it's
  original value. The array has to be freed.

  @param input           (FILE *) The input file (-pointer)
  @param line            (int *)  Pointer to the linenumber
  @param numberofnumbers (int)    Number of numbers to be read in

  @return (success) float *nextnumbers Array of read numbers\n
          (error)   NULL
*/
/* ------------------------------------------------------------ */
float *nextnumbers(FILE *input, int *line, int numberofnumbers);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn float *nextnumbers2(FILE *input, int *line, int numberofnumbers)
  @brief Read the next numberofnumbers numbers into an array

  Creates an array of numberofnumbers numbers (dynamically), reads in
  the next numberofnumbers numbers from *input and changes line
  according to the number of '\n's within the scan.. On error the
  filepointer is not set back to the start and line is not reset to
  it's original value. The array has to be freed.

  @param input           (FILE *) The input file (-pointer)
  @param line            (int *)  Pointer to the linenumber
  @param numberofnumbers (int)    Number of numbers to be read in

  @return (success) float *nextnumbers Array of read numbers\n
          (error)   NULL
*/
/* ------------------------------------------------------------ */
float *nextnumbers2(FILE *input, int *line, int numberofnumbers);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn float *nextnumbersint(FILE *input, int *line, int numberofnumbers)
  @brief Read the next numberofnumbers numbers into an array

  Creates an array of numberofnumbers numbers (dynamically), reads in
  the next numberofnumbers numbers from *input and changes line
  according to the number of '\n's within the scan.. On error the
  filepointer is set back to the start and line is reset to it's
  original value. The array has to be freed.

  @param input           (FILE *) The input file (-pointer)
  @param line            (int *)  Pointer to the linenumber
  @param numberofnumbers (int)    Number of numbers to be read in

  @return (success) int *nextnumbers Array of read numbers\n
          (error)   NULL
*/
/* ------------------------------------------------------------ */
int *nextnumbersint(FILE *input, int *line, int numberofnumbers);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn float *nextnumbersint2(FILE *input, int *line, int numberofnumbers)
  @brief Read the next numberofnumbers numbers into an array

  Creates an array of numberofnumbers numbers (dynamically), reads in
  the next numberofnumbers numbers from *input and changes line
  according to the number of '\n's within the scan.. On error the
  filepointer is not set back to the start and line is not reset to
  it's original value. The array has to be freed.

  @param input           (FILE *) The input file (-pointer)
  @param line            (int *)  Pointer to the linenumber
  @param numberofnumbers (int)    Number of numbers to be read in

  @return (success) int *nextnumbers Array of read numbers\n
          (error)   NULL
*/
/* ------------------------------------------------------------ */
int *nextnumbersint2(FILE *input, int *line, int numberofnumbers);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn int checkeof(FILE *inputfile)
  @brief Check if EOF is reached, ignoring whitespaces, leave the 
  FILE pointer unchanged

  Primitive check if the end of inputfile is reached,
  ignoring whitespaces. An Error reading the file will result in EOF.

  @param inputfile (FILE *) Input filepointer

  @return (success) int checkeof: EOF if EOF reached or on error
          (error) 0 if the next non whitespace character is not EOF
*/
/* ------------------------------------------------------------ */
int checkeof(FILE *inputfile);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int countwordsinwarr(char **thearray)

  @brief Counts the number of elements in an array of char arrays 
         before an element is NULL.

  @param thearray (char **) The array.

  @return (success) int countwordsinwarr: Number of words \n
          (error): -1 In case of a NULL pointer passed
*/
/* ------------------------------------------------------------ */
int countwordsinwarr(char **thearray);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn decomp_control decomp_init(void)

  @brief Returns a control structure for decomp functions

  Suppose you have a number of parameters p1, ...,p_i, ..., pn with indices
  1,...,m_1 for p1, 1, ..., m_n. The parameters are stored in a linear
  array

  a    a+1  a+n_1 a+n_1+1 ... a+n_1+n_2+...+n_m
  p1_1 p1_2 p1_n1 p2_1        pn_m_n

  A user wants to name certain parameters and make a list of
  groups. The decomp function group delivers an extremely basic parser
  functionality to return arrays of numbers that the user specifies
  with names and indices.

  Parameter definition:

  The programmer supplies a list of parameters with names, the
  position of the parameter with the starting index 1 in the linear
  array (a+i_1 for the i'th parameter), and the number of indices for
  the parameter (m_i for the ith parameter). This is done with the
  function decomp_inp(). This means that with the parameter name and
  the index the position in the linear array is determined.

  Parsing syntax:

  The programmer supplies a group separator (',' in further
  documentation) and a decompose indicator ('-' in further
  documentation). Group indicator and decompose indicator may not be +
  or : or any digit. Parameter names should neither contain group and
  decompose indicator. If the parameter names are P1, ... , Pn, the
  user indicates the the jth index of parameter Pi with

  Pi j (returns ai_j)

  The user indicates a list from the jth index of parameter Pi to the
  kth index with

  Pi j:k (returns ai_j, ai_j+1, ... , ai_k)

  The user indicates a list from the jth index of parameter Pi to the
  highest index of that parameter with

  Pi j: (returns ai_j, ai_j+1, ... , ai_m_i)

  The user indicates a list from the first index 1 of parameter Pi to the
  index j of that parameter with

  Pi :j (returns ai_1, ai_2, ... , ai_j)

  The user indicates a list from the first index 1 of parameter Pi to the
  highest index of that parameter with

  Pi 1: (returns ai_1, ai_2, ... , ai_m_i) or
  Pi

  The user indicates with a '+l' to jump over parameters

  Pi j:k+l (returns ai_j, ai_j+l, ...)
  Pi :j+l  (returns ai_1, ai_1+l, ...)
  Pi j:+l  (returns ai_j, ai_j_l, ...)
  Pi j+l   (returns ai_j, since this is the only one specified)
  Pi +l    (returns ai_1, ai_1+l, ...)

  The user defines groups separated with a group separator (',')

  Inside a group, several space separated parameter definitions can be
  given according to the syntax given above. Tbc.

  @param grse (char) Separator for groups
  @param dein (char) Indicator for decomposing as group, default is single element

  @return (success) decomp_control decomp_init: decomp control structure\n
          (error): NULL
*/
/* ------------------------------------------------------------ */
decomp_control decomp_init(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
@fn int decomp_putsep(decomp_control decomp_controlv, char grse, char dein, char deco)

  @brief Define group- and decompose indicators

  @param decomp_controlv (decomp_control) decomp control structure
  @param grse            (char)           Separator for groups
  @param dein            (char)           Indicator for decomposing into single elements
  @param deco            (char)           Decoding character for number sequences

  @return (success) 0\n
          (fail)    1: separators are similar, 2: dcp_controlv is empty
*/
/* ------------------------------------------------------------ */
int decomp_putsep(decomp_control decomp_controlv, char grse, char dein, char deco);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn decomp_dest(decomp_control decomp_controlv)

  @brief Destructs decomp_controlv

  @return void
*/
/* ------------------------------------------------------------ */
void decomp_dest(decomp_control decomp_controlv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int decomp_inp(decomp_control decomp_controlv, const char* pana, int papo, int pale)
  @brief Input a parameter with name, position, and length

  @param decomp_controlv (decomp_control) Decomp control structure
  @param pana (const char *) Parameter name
  @param papo (int)          Parameter position in array
  @param pale (int)          Parameter length, zero forbids to specify a length and the parameter has length 1

  @return success: int decomp_inp 0
          fail:    1
*/
/* ------------------------------------------------------------ */
int decomp_inp(decomp_control decomp_controlv, const char* pana, int papo, int pale);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int decomp_index(decomp_control decomp_controlv, int number, int *list)
  @brief Input a list of indexed positions in array

  List contains a list of indexed numbers in the array that will be
  omitted. Array is copied locally. If another list was present, this is replaced.

  @param decomp_controlv (decomp_control) Decomp control structure
  @param number (int *)                     number of elements
  @param list   (int *)                     list 

  @return (success) int dcp_index 0
          (failure) 1: empty decomp_controlv or number < 0
                    2: allocation problems
*/
/* ------------------------------------------------------------ */
int decomp_index(decomp_control decomp_controlv, int number, int *list);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int decomp_get(decomp_control decomp_controlv, char *input, decomp_listel **output, int nega)
  @brief Returns a null terminated list of positions

  A list of groups of array positions is returned decomposing the input array. For each entry in the list, the first number gives the number of entries, the second number the number of the input group, then this is followed by the numbers in the array.

  @param decomp_controlv (decomp_control) Decomp control structure
  @param input (char *)                   input string
  @param output (decomp_listel **)        output (unallocated on input, give address of a pointer (e.g. &decomp_listelv if decomp_listelv == decomp_listel *)!)
  @param nega  (int)                      0: input must be in range 1,...,length as defined in decomp_put()

  @return success: 0: output is filled with the decomp_listel struct
          fail:    1: memory problems
                   2: syntax error (error message via decomp_errmsg)
*/
/* ------------------------------------------------------------ */
int decomp_get(decomp_control decomp_controlv, char *input, decomp_listel **output, int nega);

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn char *decomp_errmsg(decomp_control decomp_controlv)
  @brief Returns the current error message

  On error this returns an allocated error message. Do not try to free
  this, deallocation is done via calling decomp_dest(). On no
  error, error = NULL.

  @param decomp_controlv (decomp_control) Decomp control structure

  @return success: char *decomp_errmsg Error message
          fail:    NULL If no error has occurred
*/
/* ------------------------------------------------------------ */
char *decomp_errmsg(decomp_control decomp_controlv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn decomp_inlist *decomp_inlist_init(void)
  @brief Constructor for a decomp_inlist struct

  Returns an allocated decomp_inlist struct, destroy with decomp_inlist_dest().

  @return success: decomp_inlist *decomp_inlist_init allocated decomp_inlist struct
fail: NULL (memory problems)
*/
/* ------------------------------------------------------------ */
decomp_inlist *decomp_inlist_init(void);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn void decomp_list_dest(decomp_listel *decomp_listelv)
  @brief Destructor for a null terminated array of decomp_listel structs

  @param decomp_listelv (decomp_listel *) null terminated array of decomp_listel structs

  @return void
*/
/* ------------------------------------------------------------ */
void decomp_list_dest(decomp_listel *decomp_listelv);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn void decomp_inlist_dest(decomp_inlist *decomp_inlistv)
  @brief Destructor for a decomp_inlist struct

  @param decomp_inlistv (decomp_inlist *) allocated and properly configured decomp_inlist struct (as returned by decomp_get_inlist())

  @return void
*/
/* ------------------------------------------------------------ */
void decomp_inlist_dest(decomp_inlist *decomp_inlistv);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static char *simparse_copystring(char *instring)

  @brief Return a newly allocated char array with a copy of instring

  @param instring (char) input string

  @return char *copystring: copied array
          NULL: NULL passed or memory problems
*/
/* ------------------------------------------------------------ */
char *simparse_copystring(char *instring);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn int decomp_get_inlist(decomp_control decomp_controlv, decomp_inlist *decomp_inlistv)
  @brief Returns a struct containing the next active indices of indexed indices

  The function provides for each item on the index list the two next
  indices of a input group that are not on the index, one with a
  higher index, one with a lower. Note that allocated members of the
  struct will be destroyed. If a group contains only indexed indices
  to one side, the indices to the other side are duplicated. If a
  group contains only indexed indices, the neighbouring indices are
  identical to the indexed index itself. The same happens if an indexed index is not matched in the input list.

  @param decomp_controlv (decomp_control) Decomp control structure
  @param decomp_inlistv (decomp_inlist *) allocated and properly configured decomp_inlist struct (as returned by decomp_get_inlist())

  @return 0: success
          1: index is not represented in input groups
          2: all parameters indexed inside group
   3: memory problems
*/
/* ------------------------------------------------------------ */
int decomp_get_inlist(decomp_control decomp_controlv, decomp_inlist *decomp_inlistv);


void *malloc_here(size_t size);
void *realloc_here(void *a, size_t size);
void free_here(void *where);
void stats_here(void);

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   $Log: simparse.h,v $
   Revision 1.2  2010/03/18 15:50:54  jozsa
   Left work

   Revision 1.4  2010/02/17 01:21:37  jozsa
   finished decomp complex

   Revision 1.3  2009/03/27 11:17:12  jozsa
   Left work

   Revision 1.2  2009/03/09 12:02:46  jozsa
   included decomposer input functions

   Revision 1.1.1.1  2009/03/06 12:57:04  jozsa
   imported

   Revision 1.5  2007/08/22 15:58:36  gjozsa
   Left work

   Revision 1.4  2004/12/07 10:31:43  gjozsa
   Left work

   Revision 1.3  2004/11/16 18:18:32  gjozsa
   Left work

   Revision 1.2  2004/11/09 17:53:56  gjozsa
   Left work

   Revision 1.1.1.1  2004/10/29 11:13:05  gjozsa
   Added to CVS control


   ------------------------------------------------------------ */

/* Include guard */
#endif

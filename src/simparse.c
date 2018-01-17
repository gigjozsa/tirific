/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file simparse.c
   @brief A utility for parsing files

   All-purpose parsing functions, excluded from the main source for
   portability reasons.

   $Source: /Volumes/DATA_J_II/data/CVS/tirific/src/simparse.c,v $
   $Date: 2011/05/25 22:25:26 $
   $Revision: 1.4 $
   $Author: jozsa $
   $Log: simparse.c,v $
   Revision 1.4  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.3  2011/05/10 00:30:16  jozsa
   Left work

   Revision 1.2  2010/03/18 15:50:55  jozsa
   Left work

   Revision 1.5  2010/02/17 01:21:40  jozsa
   finished decomp complex

   Revision 1.4  2009/06/19 17:31:59  jozsa 
   Bug removal evaluating the sep option in parsenext

   Revision 1.3  2009/03/27 11:17:12  jozsa
   Left work

   Revision 1.2  2009/03/09 12:02:46  jozsa
   included decomposer input functions

   Revision 1.1.1.1  2009/03/06 12:57:04  jozsa
   imported

   Revision 1.5  2007/08/22 15:58:42  gjozsa
   Left work

   Revision 1.4  2004/12/07 10:31:45  gjozsa
   Left work

   Revision 1.3  2004/11/16 18:18:34  gjozsa
   Left work

   Revision 1.2  2004/11/09 15:52:54  gjozsa
   Fixed bug while counting lines in fct parsenext

   Revision 1.1.1.1  2004/10/29 11:13:20  gjozsa
   Added to CVS control

   @todo putparsed seems to have a problem with keeping a maximum length
   of an output line.


*/
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <simparse.h>
#include <maths.h>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */
#define SCN_TYPE_STRING 1
#define SCN_TYPE_STRINGWHITESP 2
#define SCN_TYPE_STRINGLIST 4
#define SCN_TYPE_INT 8
#define SCN_TYPE_LONG 16
#define SCN_TYPE_FLOAT 32
#define SCN_TYPE_DOUBLE 64


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE MACROS */
/* ------------------------------------------------------------ */
/* #define malloc(a) malloc_here(a) */
/* #define free(a)   free_here(a) */
/* #define realloc(a,b) realloc_here(a,b) */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* (PRIVATE) GLOBAL VARIABLES */
/* ------------------------------------------------------------ */
/* static int numberofmallocs = 0; */
/* static int numberoffrees = 0; */
/* static int numberofreallocs = 0; */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE TYPEDEFS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE STRUCTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct dcp_listll
   @brief element of a return linked list

*/
/* ------------------------------------------------------------ */
typedef struct dcp_listll
{
  /** @brief number and position of scanned element */
  int nupo[2];

  /** @brief next element */
  struct dcp_listll *next;

} dcp_listll;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct dcp_pall
   @brief element of a parameter description linked list

*/
/* ------------------------------------------------------------ */
typedef struct dcp_pall
{
  /** @brief parameter name */
  char *name;

  /** @brief position in array */
  int posi;

  /** @brief number of elements with that name */
  int numb;

  /** @brief next element */
  struct dcp_pall *next;
}
  dcp_pall;

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct dcp_control
   @brief Control structure for decomp functions

*/
/* ------------------------------------------------------------ */
typedef struct dcp_control
{
  /** @brief group separator */
  char grse;

  /** @brief decompose indicator */
  char dein;

  /** @brief decompose character of number sequences */
  char deco;

  /** @brief number of indexed indices */
  int ninin;

  /** @brief list of indexed indices */
  int *inin;

  /** @brief linked list of names and positions */
  dcp_pall *pall;

  /** @brief error message */
  char *error;
}
dcp_control;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int *dcp_get(dcp_control *dcp_controlv, char *input, decomp_listel **output, int nega)

  @brief Returns a null terminated list of positions

  @return (success)decomp_listel *dcp_get the info you want to get \n
          (error): NULL
*/
/* ------------------------------------------------------------ */
static int dcp_get(dcp_control *dcp_controlv, char *input, decomp_listel **output, int nega);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static dcp_control dcp_init(void)

  @brief Returns a control structure for decomp functions

  @return (success) decomp_control decomp_init: decomp control structure\n
          (error): NULL
*/
/* ------------------------------------------------------------ */
static dcp_control *dcp_init(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static void dcp_swapll(dcp_listll *first, dcp_listll *after, dcp_listll *beforeafter)

  @brief puts after behind first and mends the ll

@param first (dcp_listll *)        first element
@param after (dcp_listll *)        second element
@param beforeafter (dcp_listll *) element before second element


@return void This is not robust against errors, i.e., passing NULL will fail
*/
/* ------------------------------------------------------------ */
static void dcp_swapll(dcp_listll *first, dcp_listll *after, dcp_listll *beforeafter);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int dcp_putsep(dcp_control *dcp_controlv, char grse, char dein, char deco)

  @brief Define separators
  @param grse            (char)           Separator for groups
  @param dein            (char)           Indicator for decomposing into single elements
  @param deco            (char)           Decoding character for number sequences

  @return (success) 0\n
          (fail)    1: separators are similar, 2: dcp_controlv is empty

*/
/* ------------------------------------------------------------ */
static int dcp_putsep(dcp_control *dcp_controlv, char grse, char dein, char deco);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int dcp_checkiden(char *string, dcp_pall *pall, int *info)
  @brief Check ll pall for occurrence of an identifier in string

  @param string          (char *)         string to be identified
  @param pall            (dcp_pall *)     linked list with identifiers and information
  @param infor           (int *)          information is stored here: info[0]: posi info[1]: numb

  @return (success) 0\n
          (fail)    1: not in list.

*/
/* ------------------------------------------------------------ */
static int dcp_checkiden(char *string, dcp_pall *pall, int *info);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static void dcp_dest(dcp_control *dcp_controlv)

  @brief Destroys dcp_controlv

  @return void
*/
/* ------------------------------------------------------------ */
static void dcp_dest(dcp_control *dcp_controlv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static void bubble(int length, float *array)

  @brief Bubble-sort array of length length

  @return void
*/
/* ------------------------------------------------------------ */
static void bubble(int length, int *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static void dcp_listll_dest(dcp_listll **dcp_listllv)

  @brief destroys the dcp_listll ll and sets *dcp_listllv = NULL

  @param dcp_listllv (dcp_listll **) pointer to a ll (take care to pass some allocated stuff, mostly &bla if bla is the ll)

  @return void
*/
/* ------------------------------------------------------------ */
 static void dcp_listll_dest(dcp_listll **dcp_listllv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static dcp_listll *dcp_listll_app(int *startlength, char *input, int *error, char sep)

  @brief Returns to dcp_listll dcp_listlls elements dechiffering input

  Error codes in error: 
    0: No error
    1: allocation problems

  @param dcp_listllv (dcp_listll *) pointer to a ll or 0;
  @param startlength (int *)        Two-element array 0: start position 1: length
  @param input       (char *)       Input string to decipher
  @param error       (int *)        Container for error code, pass allocated *int
  @param sep         (char)         Separator for fields

  @return success: dcp_listll *dcp_listll_app pointer to last appended element
          error:   NULL, error will be set

*/
/* ------------------------------------------------------------ */
static dcp_listll *dcp_listll_app(int *startlength, char *input, int *error, char sep, int nega);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int dcp_index(dcp_control *dcp_controlv, int number, int *list)

  @brief Replaces the indexed list in dcp_controlv with the list given. Makes a local copy

  @param dcp_controlv (dcp_control *) dcp control object containing the linked list
  @param number       (int *)         number of elements
  @param list         (int *)         list 

  @return (success) int dcp_index 0
          (failure) 1: empty dcp_controlv
                    2: allocation problems

*/
/* ------------------------------------------------------------ */
static int dcp_index(dcp_control *dcp_controlv, int number, int *list);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int *dcp_inp(dcp_control *dcp_controlv, const char* pana, int papo, int pale)

  @brief Creates an element of dcp_pall liked list and appends it to list. pana will be copied loacally.

  @param dcp_controlv (dcp_control *) dcp control object containing the linked list
  @param pana         (const char *)  Parameter name
  @param papo         (int)           Parameter position in array
  @param pale         (int)           Parameter length, zero forbids to specify a length and the parameter has length 1

  @return (success): int dcp_inp 0\n
          (error): 1
*/
/* ------------------------------------------------------------ */
static int dcp_inp(dcp_control *dcp_controlv, const char* pana, int papo, int pale);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static  dcp_pall *dcp_pall_dest(dcp_pall *dcp_pallv)

  @brief Destroys dcp_pallv and returns the next element

  @param dcp_pallv (dcp_pall *) linked list element

  @return dcp_pall *dcp_pall_dest next element

*/
/* ------------------------------------------------------------ */
static  dcp_pall *dcp_pall_dest(dcp_pall *dcp_pallv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn char *dcp_errmsg(dcp_control *dcp_controlv)
  @brief Returns the current error message

  On error this returns an allocated error message. Do not try to free
  this, deallocation is done via calling decomp_dest(). On no
  error, error = NULL.

  @param decomp_controlv (decomp_control) Decomp control structure

  @return success: char *decomp_errmsg Error message
          fail:    NULL If no error has occurred
*/
/* ------------------------------------------------------------ */
static char *dcp_errmsg(dcp_control *dcp_controlv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
   @fn static char isinstring(char *string, char character)`
   @brief Test if a character is in a string
   
   Returns 1 if it is contained in the string string, 0 otherways

   @param string (char *)   A constant string
   @param character (char)  The character in which string is contained
   
   @return (success) 1 if it is found\n
    (error) 0 if it is not found
*/
/* ------------------------------------------------------------ */
static char isinstring(char *string, char character);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static char **intparsenext(char *fieldseperator, char *lineseperator,char *whitespaces, char *clamper, char *commentsep, char *endcomment, int maxfield, char **string, int *line, char sep) 
  @brief The core of sparsenext and parsenext
  
  Returns an allocated array of allocated char arrays parsenext (freed
  by freeparsed) containing the words seperated by fieldseperator
  until the next line denoted by lineseperator (or \0) is reached or
  a number maxfield of output fields is reached. The content of string is
  set to the start of the next line or to the end of the array. On
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
  @param string         (char **) The input string, the content will be changed to the start of the next line
  @param line           (int *)  The line where the search started, 
                                 any number, will be increased by 
                                 the number of encountered '@\ns'.
  @param sep (char) If set (!=0) suppress every empty field.
  @param countout (int *)     The number of chars to proceed (output, must be a pointer to an allocated int)
  
  @return (success) char **parsenext: allocated array of 
                                      allocated char arrays\n
  (error) NULL if EOF is reached or an error occured
*/
/* ------------------------------------------------------------ */
static char **intparsenext(char *fieldseperator, char *lineseperator, \
              char *whitespaces, char *clamper, char *commentsep, \
	      char *endcomment, int maxfield, char **string, int *line, char sep, int *countout);


   
/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int checklinelength(FILE *stream, char yesno, int maxlength, int *current_length, char *word)
  @brief Silly helper for the putparsed function, checking the length of
  a line

  If yesno is 1, then this function will do something, otherways not a
  thing. If current_length plus wordlength exceeds
  maxlength an '\n' is inserted into stream and current_length is set
  to 0. This action takes only place if current_length is not 0. An
  error occurs only if the writing to the stream fails.

  @param stream         (FILE *)
  @param yesno          (char)
  @param maxlength      (int)
  @param current_length (int *)
  @param wordlength     (int)

  @return (success) int checklinelength: 1 except if the writing into
  stream fails\n
          (error) 0
*/
/* ------------------------------------------------------------ */
static int checklinelength(FILE *stream, char yesno, int maxlength,\
			   int *current_length, int wordlength);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static void error_msg_parser(char *call_file, char *call_function, char *call_msg, int line)
  @brief Generic error message in this module

  Generates a generic error message to stderr, reporting the occurance
  of an error, the file, the function, and a message. If an input
  string is empty, no message will be generated concerning the item. A
  similar function can appear elsewhere, but this is kept private for
  prtability reasons.

  @param call_file     (char *) The file from which the call is made
  @param call_function (char *) The function from which the call is made
  @param call_msg      (char *) The error message
  @param line          (int *)  Linenumber printed if not 0 in input

  @return void
*/
/* ------------------------------------------------------------ */
static void error_msg_parser(char *call_file, char *call_function, char *call_msg, int line);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int dcp_get_inlist(dcp_control *dcp_controlv, decomp_inlist *decomp_inlistv)
  @brief Returns a struct containing the next active indices of indexed indices

  Wrapper around decomp_get_inlist

  @param decomp_inlistv (decomp_inlist *) allocated and properly configured decomp_inlist struct (as returned by decomp_get_inlist())
  @param dcp_controlv (dcp_control *) Decomp control structure

  @return 0: success
          1: index is not represented in input groups
          2: all parameters indexed inside group
*/
/* ------------------------------------------------------------ */
static int dcp_get_inlist(dcp_control *dcp_controlv, decomp_inlist *decomp_inlistv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int matchint(int val, int nelem, int *thelist)

  @brief Search an int list for a match

  @param val (int) the value to be searched for
  @param nelem (int) the number of elements in the list
  @param thelist (int) the list

  @return int matchint: 1 found match
                        0 did not find match
*/
/* ------------------------------------------------------------ */
static int matchint(int val, int nelem, int *thelist);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int plen(char ** thelist)

  @brief Return length of NULL-terminated array of char arrays

  @param thelist (char **) the array

  @return int plen (success): number of elements without terminating NULL
                   (error):   -1 NULL passed
*/
/* ------------------------------------------------------------ */
static int plen(char **thelist);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int getlongfromstring(char *input, int n, long *output, int *nmax)

  @brief Get a number if possible from a string

  Interpret instring and get the nth number from a possible
  list. Instring has the format start:end:step or represents a single
  number as string. If a single number is entered the list contains
  only one element. If the format start:end:step is chosen, the list
  is constructed as follows: 
  val_0 = start 
  val_1 = start+step 
  ... 

  as long as val_i <= end The function is limited by memory issues
  (there is a supertiny chance). If this occurs, the function returns
  -1. If n is too small or too large, 1 is returned. If (one of) the
  value(s) cannot be interpreted as number, 2 is returned.
  The input must consist of numbers and colons, nothing else.


  @param input    (char) input string
  @param n        (int) Position in list of possible values, starting with 0 (see description)
  @param output   (long *) output value
  @param nmax     (int *) Maximum n to be requested

  @return int getlongfromstring() (success): 0
          -1: memory problems
           1: n out of range
           2: wrong input format
           3: wrong input numbers (negative where shouldn't or positive where shouldn't)
*/
/* ------------------------------------------------------------ */
static int getlongfromstring(char *input, int n, long *output, int *nmax);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static int getdoublefromstring(char *input, int n, double *output, int *nmax)

  @brief Get a number if possible from a string

  Interpret instring and get the nth number from a possible
  list. Instring has the format start:end:step or represents a single
  number as string. If a single number is entered the list contains
  only one element. If the format start:end:step is chosen, the list
  is constructed as follows: 
  val_0 = start 
  val_1 = start+step 
  ... 

  as long as val_i <= end The function is limited by memory issues
  (there is a supertiny chance). If this occurs, the function returns
  -1. If n is too small or too large, 1 is returned. If (one of) the
  value(s) cannot be interpreted as number, 2 is returned.
  The input must consist of numbers and colons, nothing else.


  @param input    (char) input string
  @param n        (int) Position in list of possible values, starting with 0 (see description)
  @param output   (double *) output value
  @param nmax     (int *) Maximum n to be requested

  @return int getlongfromstring() (success): 0
          -1: memory problems
           1: n out of range
           2: wrong input format
           3: wrong input numbers (negative where shouldn't or positive where shouldn't)
*/
/* ------------------------------------------------------------ */
static int getdoublefromstring(char *input, int n, double *output, int *nmax);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static simparse_scn_keyvalli *simparse_scn_keyvalli_create(void)
  @brief Allocate simparse_scn_keyvalli struct, setting internal pointers to NULL, return pointer
  
  @param simparse_scn_keyvalliv (simparse_scn_keyvalliv *) Pointer to struct

  @return (success) simparse_scn_keyvalli *simparse_scn_keyvalli_create Pointer to struct
          (error) NULL
*/
/* ------------------------------------------------------------ */
static simparse_scn_keyvalli *simparse_scn_keyvalli_create(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn void static simparse_scn_keyvalli_destroy(simparse_scn_keyvalli *simparse_scn_keyvalliv);
  @brief Deallocate simparse_scn_keyvalli struct, the pointer is pointing to, including strings therein
  
  @param simparse_scn_keyvalliv (simparse_scn_keyvalliv *) Pointer to struct

  @return void
*/
/* ------------------------------------------------------------ */
static void simparse_scn_keyvalli_dest(simparse_scn_keyvalli *simparse_scn_keyvalliv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static simparse_scn_keyvalli **simparse_scn_keyvallilist_start(void)
  @brief Allocate an array of pointers to simparse_scn_keyvalli structs with one element, which is NULL
  
  @return (success) simparse_scn_keyvalli **simparse_scn_keyvallilist_start Pointer to array
          (error) NULL
*/
/* ------------------------------------------------------------ */
static simparse_scn_keyvalli **simparse_scn_keyvallilist_start(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static simparse_scn_keyvalli **simparse_scn_keyvalli_insert(simparse_scn_keyvalli **simparse_scn_keyvallilist, char *key, char *value, int line)
  @brief Allocate simparse_scn_keyvalli struct, insert an (allocated) copy of key and and allocated copy of value, allocate new list to insert line, return pointer to new list
  
  @param simparse_scn_keyvallivlist (simparse_scn_keyvalli **) Pointer to NULL-terminated array of pointers to simparse_scn_keyvalli structs
  @param key (char *) The key to pass (string)
  @param value (char *) The value to pass (string)
  @param line (int) The line

  @return (success) simparse_scn_keyvalli **simparse_scn_keyvalli_insert Pointer to new array
          (error) NULL
*/
/* ------------------------------------------------------------ */
static simparse_scn_keyvalli **simparse_scn_keyvalli_insert(simparse_scn_keyvalli **simparse_scn_keyvallilist, char *key, char *value, int line);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static simparse_scn_keyvalli **simparse_scn_keyvallilist_gfrst(char *string)
  @brief Read in a keyvalli list from string
  
  Rules:
  The string will be split into key-value-line triplets, parsing string.

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

  @return (success) simparse_scn_keyvallilist_gfrst (simparse_scn_keyvalli **) array of allocated simparse_scn_keyvalli structs
          (error) NULL (allocation problems)

*/
/* ------------------------------------------------------------ */
static simparse_scn_keyvalli **simparse_scn_keyvallilist_gfrst(char *string);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static simparse_scn_keyvalli **simparse_scn_keyvallilist_gcomm(char **readin)
  @brief Read in a keyvalli list from a NULL-terminated list of strings, helper to gfrfi or gfrst
  
  Rules:
  The string will be split into key-value-line triplets, parsing readin.

  A key is the last contiguous string (not containing any of ' ',
  '\n', or '\t') before any '=' character. The value of that key is
  anything between that '='-character and the next key, including
  whitespaces. The line is the number of encountered '\n's at the
  position of the '='-character plus one.  Anything starting with '#'
  and ending with '\n' is ignored. Anything starting with '#' and
  ending with '#' gets ignored.  It is assumed that whatever needs to
  be analysed has already been split with the '=' symbol as splitting
  character, either with sparsenext or parsenext.

  If NULL is passed, an array containing one element with the value of
  NULL is returned. The same is true if an empty string is returned.

  @param readin (char *) Allocated NULL-terminated char * array or NULL

  @return (success) simparse_scn_keyvallilist_gcomm (simparse_scn_keyvalli **) array of allocated simparse_scn_keyvalli structs
          (error) NULL (allocation problems)
*/
/* ------------------------------------------------------------ */
static simparse_scn_keyvalli **simparse_scn_keyvallilist_gcomm(char **readin);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static simparse_scn_arel *simparse_scn_arel_create(void)
  @brief Allocate simparse_scn_arel struct, setting internal pointers to NULL, return pointer
  
  @return (success) simparse_scn_arel *simparse_scn_arel_create Pointer to struct
          (error) NULL
*/
/* ------------------------------------------------------------ */
static simparse_scn_arel *simparse_scn_arel_create(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static void simparse_scn_arel_dest(simparse_scn_arel *simparse_scn_arelv);
  @brief Deallocate simparse_scn_arel struct and all sub-structures down to strings
  
  @param simparse_scn_arelv (simparse_scn_arel *) Pointer to struct

  @return void
*/
/* ------------------------------------------------------------ */
static void simparse_scn_arel_dest(simparse_scn_arel *simparse_scn_arelv);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
  @fn static simparse_scn_arel **simparse_scn_arellist_start(void)
  @brief Allocate an array of pointers to simparse_scn_arel structs with one element, which is NULL
  
  @return (success) simparse_scn_arel **simparse_scn_arellist_start Pointer to array
          (error) NULL
*/
/* ------------------------------------------------------------ */
simparse_scn_arel **simparse_scn_arellist_start(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION CODE */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* read in the next words from a file into an array of words */

char **parsenext(char *fieldseperator, char *lineseperator, char *whitespace, char *clamper, char *commentsep, char *endcomment, int maxfield, FILE *filepointer, int *line, char sep)

{
  char **parsenext = NULL;
  char *nextline = NULL;
  char nextcharacter;
  fpos_t position ;
  int charcounter = 0, countout = 0, sizer = 0;
  
  /* check if EOF is reached or only whitespaces are found */
  fgetpos(filepointer, &position);

  if (getc(filepointer) == EOF) {
    goto error;
  }

  /* wind back */
  fsetpos(filepointer, &position);

  /* We'll copy it as it will be returned */

  /* First find the number of characters in the next line */
  while ((nextcharacter = fgetc(filepointer)) != EOF && !isinstring(lineseperator, nextcharacter)) {
    ++charcounter;
  }
    
  if (nextcharacter == EOF) {
    ++sizer;
  }

  /* Now read in the line */
  fsetpos(filepointer, &position);

  if (!(nextline = (char *) malloc((charcounter+1)*sizeof(char)))) {
    goto error;
  }
  if (charcounter)
    if (fread(nextline, sizeof(char), charcounter, filepointer))
      ;
  nextline[charcounter] = '\0';

  /* put the filepointer one step forward */
/*   getc(filepointer); */

  /* Do not free nextline, this is done in intparsenext */
  if (!(parsenext = intparsenext(fieldseperator, lineseperator, whitespace, clamper, commentsep, endcomment, maxfield, &nextline, line, sep, &countout)))
    goto error;
  
/* Spool back to the place numberofwords have been read */
  fsetpos(filepointer, &position);
  fseek(filepointer, countout-sizer, SEEK_CUR);

  return parsenext;
  
 error:
  if (nextline)
    free(nextline);
  fsetpos(filepointer, &position);
  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* read in the next words from a string into an array of words */

char **sparsenext(char *fieldseperator, char *lineseperator, char *whitespace, char *clamper, char *commentsep, char *endcomment, int maxfield, char **string, int *line, char sep, char reset)

{
  char **parsenext = NULL;
  char *nextline = NULL;
  char nextcharacter;
  char *position = NULL;
  int charcounter = 0,  countout = 0, sizer = 0;

  if ((string == NULL) || (*string == NULL)) {
    goto error;
  }

  /* check if EOF is reached or only whitespaces are found */
  position = *string;

  /* We'll copy it as it will be returned */

  /* First find the number of characters in the next line */
  while ((nextcharacter = **string) != '\0' && !isinstring(lineseperator, nextcharacter)) {
    ++charcounter;
    ++(*string);
  }
    
  if (nextcharacter == '\0') {
    ++sizer;
  }

  /* Now read in the line */
  *string = position;

  if (!(nextline = (char *) malloc((charcounter+1)*sizeof(char)))) {
    goto error;
  }

  if (charcounter) {
    for (countout = 0; countout < charcounter; ++countout)
      nextline[countout] = (*string)[countout];
  }
  nextline[charcounter] = '\0';

  /* Do not free nextline, this is done in intparsenext */
  if (!(parsenext = intparsenext(fieldseperator, lineseperator, whitespace, clamper, commentsep, endcomment, maxfield, &nextline, line, sep, &countout)))
    goto error;

  if (!reset)
    *string = position+countout-sizer;

  return parsenext;
  
 error:
  if (nextline)
    free(nextline);
  if (position)
    *string = position;
  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* read in the next words from a string into an array of words */

static char **intparsenext(char *fieldseperator, char *lineseperator, char *whitespace, char *clamper, char *commentsep, char *endcomment, int maxfield, char **string, int *line, char sep, int *countout)

{
  char **parsenext = NULL;
  char *nextline = NULL;
  char comment_on = 0, clamper_on = 0;
  int charcounter = 0, numberofwords = 0, charsinword;
  int i;

  nextline = *string;

  /* The string nextline contains everything we want to hack into pieces */
  /* nextline2 = nextline; */

  /* Reset numberofwords and charcounter */
  charcounter = 0;
  numberofwords = 1;
  
  /* Count the number of fields to be stored */
  while (nextline[charcounter] != '\0') {

    /* if a endcomment sign is encountered comment_on will be switched off except in a clamper, BUT the symbol itself is not erased from reading. This is why this stuff is on top */
    if (isinstring(endcomment, nextline[charcounter])) {

      /* except inside a clamper */
      if (clamper_on)
	;
      else
	comment_on = 0;
    }

    /* if a fieldseperator is encountered then the number of fields gets increased */
    if (isinstring(fieldseperator, nextline[charcounter])) {

      /* except inside a comment or a clamper */
      if (comment_on)
	;
      else if (clamper_on)
	;
      else
	++numberofwords;
    }

    /* if a comment sign is encountered comment_on will be switched except in a clamper */
    if (isinstring(commentsep, nextline[charcounter])) {

      /* except inside a clamper */
      if (clamper_on)
	;
      else
	comment_on = !comment_on;
    }

    /* if a clamper sign is encountered clamper_on will be switched except in a comment */
    if (isinstring(clamper, nextline[charcounter])) {

      /* except inside a comment */
      if (comment_on)
	;
      else
	clamper_on = !clamper_on;
    }

    ++charcounter;
  }

  /* Compare the number of the words with the maximum number of words */
  numberofwords = ((maxfield > 0) && (maxfield < numberofwords)) ? maxfield : numberofwords;

  /* Now we know the number of words, so we can allocate for the array */
  if (!(parsenext = (char **) malloc((numberofwords+1)*sizeof(char *))))
    goto error;

  /* We will set everything to NULL for the error handling */
  for (i = 0; i <= numberofwords; ++i) 
    parsenext[i] = NULL;

  /* Now we will allocate memory for the single fields, i.e. first sort out the memory problems */

  /* We will need to come back here */
  comment_on = 0;
  clamper_on = 0;
  charcounter = 0;

  /* count the characters in the words */

  for (i = 0; i < numberofwords; ++i) {
    charsinword = 0;
    while (nextline[charcounter] != '\0') {
      
      /* if a fieldseperator is encountered then go to the next word */
      if (isinstring(fieldseperator, nextline[charcounter])) {

	/* except inside a comment or a clamper */
	if (comment_on) {
	  if (isinstring(endcomment, nextline[charcounter]))
	    break;
	}
	else if (clamper_on)
	  ++charsinword;
	else
	  break;
      }
      
      /* if a comment sign is encountered comment_on will be switched except in a clamper */
      else if (isinstring(commentsep, nextline[charcounter])) {
	
	/* except inside a clamper */
	if (clamper_on)
	  ++charsinword;
	else
	  comment_on = !comment_on;
      }

      /* if a endcomment sign is encountered comment_on will be switched off except in a clamper */
      else if (isinstring(endcomment, nextline[charcounter])) {
	
	/* except inside a clamper */
	if (clamper_on)
	  ++charsinword;
	else {
	  comment_on = 0;
	  ++charsinword;
	}
      }
      
      /* if a clamper sign is encountered clamper_on will be switched except in a comment */
      else if (isinstring(clamper, nextline[charcounter])) {
	
	/* except inside a comment */
	if (comment_on)
	  ;
	else
	  clamper_on = !clamper_on;
      }
      
      /* if a whitespace is encountered it will be ignored except if clamped */
      else if (isinstring(whitespace, nextline[charcounter])) {
	
	/* except inside a clamper */
	if (clamper_on)
	  ++charsinword;
      }

      /* Otherwise we have a normal character */
      else  {
	
	/* except inside a comment */
	if (!comment_on)
	  ++charsinword;
      }


    ++charcounter;
    }

    /* Now allocate memeory */
    if (!(parsenext[i] = (char *) malloc((charsinword+1)*sizeof(char))))
      goto error;
    /* If the memory is allocated make a string out of it */
    *(parsenext[i]+charsinword) = '\0';

    /* Hop over the next sign (we counted the words) */
    charcounter = charcounter+1;
  }

  /* Now that we have safely allocated all the space, we can continue */

  /* Now we can change the lines and put back the FILE pointer */

  /* Count the number of \ns and change line accordingly */

  /* if the lineseperator contains an \n then we increase the number by one */
  if (isinstring(lineseperator, '\n')){
    charsinword = 1;
  }

  /* if not, then count them */
  else {

    charsinword=0;
    for (i = 0; i < charcounter-1; ++i) {

      /* if a \n is encountered then the number of charsinword gets increased */
      if (isinstring("\n", nextline[i]))

	/* We abuse charsinword here for counting lines */
	++charsinword;

    }
  }
  /* Change the linenumber */
  *line = *line + charsinword;
  
  /*****************************/
  /*****************************/

/* Spool back to the place numberofwords have been read */
/*   fsetpos(filepointer, &position); */
/*   fseek(filepointer, charcounter, SEEK_CUR); */

/* Instead of this, we output charcounter */
  *countout = charcounter;

  /*****************************/
  /*****************************/
  /* should be able to define a common function for parsenext and sparsenext for this part */

  /* The last point is to read in all the stuff */
/* We will need to come back here */
  comment_on = 0;
  clamper_on = 0;
  charcounter = 0;

  /* count the characters in the words */
  for (i = 0; i < numberofwords; ++i) {
    charsinword = 0;
    while (nextline[charcounter] != '\0') {
      
      /* if a fieldseperator is encountered then go to the next word */
      if (isinstring(fieldseperator, nextline[charcounter])) {

	/* except inside a comment or a clamper */
	if (comment_on) {
	  if (isinstring(endcomment, nextline[charcounter]))
	    break;
	}
	else if (clamper_on) {
	  *(parsenext[i]+charsinword) = nextline[charcounter];
	  ++charsinword;
	}
	else
	  break;
      }
      
      /* if a comment sign is encountered comment_on will be switched except in a clamper */
      else if (isinstring(commentsep, nextline[charcounter])) {
	
	/* except inside a clamper */
	if (clamper_on) {
	  *(parsenext[i]+charsinword) = nextline[charcounter];
	  ++charsinword;
	}
	else
	  comment_on = !comment_on;
      }
      
      /* if a endcomment sign is encountered comment_on will be switched off except in a clamper */
      else if (isinstring(endcomment, nextline[charcounter])) {
	
	/* except inside a clamper */
	if (clamper_on) {
	  *(parsenext[i]+charsinword) = nextline[charcounter];
	  ++charsinword;
	}
	else {
	  comment_on = 0;
	  *(parsenext[i]+charsinword) = nextline[charcounter];
	  ++charsinword;
	}
      }
      
      /* if a clamper sign is encountered clamper_on will be switched except in a comment */
      else if (isinstring(clamper, nextline[charcounter])) {
	
	/* except inside a comment */
      if (comment_on)
	;
      else
	clamper_on = !clamper_on;
      }
      
      /* if a whitespace is encountered it will be ignored except if clamped */
      else if (isinstring(whitespace, nextline[charcounter])) {
	
	/* except inside a clamper */
	if (clamper_on) {
	  *(parsenext[i]+charsinword) = nextline[charcounter];
	  ++charsinword;
	}
      }

      /* There is the possibility that we encountered a normal character */
      else {
	if (!comment_on) {
	  *(parsenext[i]+charsinword) = nextline[charcounter];
	  ++charsinword;
	}
      }


      ++charcounter;
    }

    /* Hop over the next sign (we counted the words) */
    charcounter = charcounter+1;
  }

  free(nextline);

  /* Now, delete every empty element, if demanded */
  if ((sep)) {
    numberofwords = 0;
    charsinword = 0;
    
    /* As long as there is no NULL check if there is an empty line */
    while ((parsenext[numberofwords])) {
      
      /* If there is an empty line, place everything one step back */
      if (parsenext[numberofwords][0] == '\0') {
	/* Simply delete the string first */
	free(parsenext[numberofwords]);
	
	charsinword = 0;
	while((parsenext[numberofwords+charsinword])) {
	  parsenext[numberofwords+charsinword] = parsenext[numberofwords+charsinword+1];
	  ++charsinword;
	}
      }
      else {
	++numberofwords;
      }
    }
    
    if ((sep)) {
      parsenext = (char **) realloc(parsenext,(numberofwords+1)*sizeof(char *));
    }
  }
  
  return parsenext;
 
 error:
  if (parsenext) {
    for (i = 0; i < numberofwords; ++i)
      free(parsenext[i]);
    free(parsenext);
  }
  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Test if a character is in a string */

static char isinstring(char *string, char character)
{
  char simplearray[2];

  /* Make a string from the character */
  simplearray[0] = character;
  simplearray[1] = '\0';

  /* Then test if it is in the string */
  if (strpbrk(string, simplearray)) {
    return 1;
  }
  else {
    return 0;
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Return a newly allocated char array with a copy of instring */
char *simparse_copystring(char *instring)
{
  char *newstring = NULL;

  if (!(instring))
    return NULL;

  if (!(newstring = (char *) malloc((strlen(instring)+1)*sizeof(char))))
    return NULL;

  strcpy(newstring, instring);

  return newstring;
}

/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destroys an char ** array that is created by parsenext */

void freeparsed(char **parsenext)
{
  int i = 0;

  if (parsenext) {

    while(parsenext[i]) {
      free(parsenext[i]);
      ++i;
    }

    free(parsenext);
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Count the words in a char * array */

int countwordsinwarr(char **thearray)
{
  int countwordsinwarr = 0;

  if (!(thearray)) {
    return -1;
  }

  while (*thearray) {
    ++countwordsinwarr;
    ++thearray;  
  }

  return countwordsinwarr;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* compare two strings up to minmatch characters */

char comp(char *constring1, char *constring2, int minmatch)
{
  char i = 0;

  /* return 0 if the length of the first argument is higher than the
     length of the second or if length of first argument is
     insufficient for identification */

  if (strlen(constring1) > strlen(constring2) || strlen(constring1) < minmatch)
       return 0;

  /* now compare the two strings until the end of first argument is
     reached */

  while (i < strlen(constring1)){
    if (*(constring1+i) != *(constring2+i)){

      /* return 0 if words differ */

      return 0;
    }
    ++i;
  }
  
  /* return 1 for success */

    return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check if the first characters of word1 match with word2 */

int comp2words(char *word1, char *word2)
{ 
  int i = 0;
  if ( strlen(word1) > strlen(word2))
    return 0;
  else
    while (i < strlen(word1)) {
      if (*(word1+i) != *(word2+i)) {
	
	/* return 0 if words differ */
	
	return 0;
      }
      ++i;
    }
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check if the first characters of word1 match with word2 */

int comp2wordsnocase(char *word1, char *word2)
{ 
  int i = 0;
  int strlen1, strlen2;

  strlen1 = strlen(word1);

  if ((strlen1 = strlen(word1))) {
    if ((word1[strlen1-1] == '='))
      --strlen1;
  }
  if ((strlen2 = strlen(word2))) {
    if ((word2[strlen2-1] == '='))
      --strlen2;
  }

  if (strlen1 != strlen2)
    return 1;
  else
    while (i < strlen1) {
      if (toupper(*(word1+i)) != toupper(*(word2+i))) {
	
	/* return 1 if words differ */
	
	return 1;
      }
      ++i;
    }
  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check the number of newlines until the next word is 
  reached */

int checkline(FILE *input)
{
  int checkline = 0;
  char nextchar;
 while ((nextchar = fgetc(input)) == '\t' || nextchar == '\r' || \
	nextchar == '\v' || nextchar == '\f' || nextchar == '\n' || \
	nextchar == ' ')
   if (nextchar == '\n')
     ++checkline;
 ungetc(nextchar,input);
 return checkline;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* read the next numberofnumbers numbers into an array */

float *nextnumbers(FILE *input, int *line, int numberofnumbers)
{
  int testnumber, lineerror, i = 0;
  float number;
  fpos_t fileposition;
  float *nextnumber;
  
  /* allocating space */
  if ((nextnumber = (float *) malloc(numberofnumbers*sizeof(float))))
    ;
  else
    return NULL;

  /* find out about the current line */
  lineerror = *line;

  /* find out about the file pointer */
  fgetpos(input, &fileposition);

  /* find out about the current line just before the next word */
  *line = *line+checkline(input);

  /* read in the numbers */
  while (++i <= numberofnumbers) {

    /* if there is no error */
    if ((testnumber = fscanf(input, "%e", &number))) {
      
      /* and if EOF is not reached */
      if (testnumber != EOF) {
	
	/* read in the number */
	*(nextnumber+i-1) = number;
	
	/* track the line */
	*line = *line+checkline(input);
      }

      /* on any error leave */
      else {
	*line = lineerror;
	fsetpos(input, &fileposition);
	free(nextnumber);
	return NULL;
      }
    }

    /* on any error leave */
    else {
      *line = lineerror;
      free(nextnumber);
      fsetpos(input, &fileposition);
      return NULL;
    }
  }
  
  /* return the array */
    return nextnumber;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Read the next numberofnumbers numbers into an array */

float *nextnumbers2(FILE *input, int *line, int numberofnumbers)
{
  int testnumber, i = 0;
  float number;
  float *nextnumber;
  
  /* allocating space */
  if ((nextnumber = (float *) malloc(numberofnumbers*sizeof(float))))
    ;
  else
    return NULL;

  /* find out about the current line */

  /* find out about the file pointer */

  /* find out about the current line just before the next word */
  *line = *line+checkline(input);

  /* read in the numbers */
  while (++i <= numberofnumbers) {

    /* if there is no error */
    if ((testnumber = fscanf(input, "%e", &number))) {
      
      /* and if EOF is not reached */
      if (testnumber != EOF) {
	
	/* read in the number */
	*(nextnumber+i-1) = number;
	
	/* track the line */
	*line = *line+checkline(input);
      }

      /* on any error leave */
      else {
	free(nextnumber);
	return NULL;
      }
    }

    /* on any error leave */
    else {
      free(nextnumber);
      return NULL;
    }
  }
  
  /* return the array */
    return nextnumber;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Read the next numberofnumbers numbers into an array */

int *nextnumbersint(FILE *input, int *line, int numberofnumbers)
{
  int testnumber, lineerror, i = 0;
  int number;
  fpos_t fileposition;
  int *nextnumber;
  
  /* allocating space */
  if ((nextnumber = (int *) malloc(numberofnumbers*sizeof(int))))
    ;
  else
    return NULL;

  /* find out about the current line */
  lineerror = *line;

  /* find out about the file pointer */
  fgetpos(input, &fileposition);

  /* find out about the current line just before the next word */
  *line = *line+checkline(input);

  /* read in the numbers */
  while (++i <= numberofnumbers) {

    /* if there is no error */
    if ((testnumber = fscanf(input, "%i", &number))) {
      
      /* and if EOF is not reached */
      if (testnumber != EOF) {
	
	/* read in the number */
	*(nextnumber+i-1) = number;
	
	/* track the line */
	*line = *line+checkline(input);
      }

      /* on any error leave */
      else {
	*line = lineerror;
	fsetpos(input, &fileposition);
	free(nextnumber);
	return NULL;
      }
    }

    /* on any error leave */
    else {
      *line = lineerror;
      free(nextnumber);
      fsetpos(input, &fileposition);
      return NULL;
    }
  }
  
  /* return the array */
    return nextnumber;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Read the next numberofnumbers numbers into an array */

int *nextnumbersint2(FILE *input, int *line, int numberofnumbers)
{
  int testnumber, i = 0;
  int number;
  int *nextnumber;
  
  /* allocating space */
  if ((nextnumber = (int *) malloc(numberofnumbers*sizeof(int))))
    ;
  else
    return NULL;

  /* find out about the current line */

  /* find out about the file pointer */

  /* find out about the current line just before the next word */
  *line = *line+checkline(input);

  /* read in the numbers */
  while (++i <= numberofnumbers) {

    /* if there is no error */
    if ((testnumber = fscanf(input, "%i", &number))) {
      
      /* and if EOF is not reached */
      if (testnumber != EOF) {
	
	/* read in the number */
	*(nextnumber+i-1) = number;
	
	/* track the line */
	*line = *line+checkline(input);
      }

      /* on any error leave */
      else {
	free(nextnumber);
	return NULL;
      }
    }

    /* on any error leave */
    else {
      free(nextnumber);
      return NULL;
    }
  }
  
  /* return the array */
    return nextnumber;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* check if EOF is reached, ignoring whitespaces, leave the FILE pointer unchanged */

int checkeof(FILE *inputfile)
{
  fpos_t position;
  char nextchar;

  fgetpos(inputfile, &position);
  while ((nextchar = fgetc(inputfile)) != EOF) {
    if (nextchar != '\t' && nextchar != '\r' && nextchar != '\v' && nextchar != '\f' && nextchar != '\n' && nextchar != ' ') {
	fsetpos(inputfile, &position);
	return 0;
    }
  }
  fsetpos(inputfile, &position);
  return EOF;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Append an array of words with a certain format to a FILE */

int putparsed(FILE *filepointer, char **array,\
	      char fieldseparator, char lineseparator,\
	      char whitespace, char clamper, char comment,\
	      int fields, int linelength, int current_length)
{
  char linebreak = 1;
  fpos_t position;

  fgetpos(filepointer, &position);
  
  /* Check if any of the special characters is a linebreak or if
     linelength is an impossible value */
  if (fieldseparator == '\n' || lineseparator == '\n' || whitespace == '\n' || clamper == '\n' || comment == '\n' || !linelength)
    /* If yes we won't break any line artificially */
    linebreak = 0;
  
  /* If this is a comment then put the comment sign at the start and
     terminate any other sign that is a comment sign */
  if ((comment)) {
    if (fieldseparator == comment)
      fieldseparator = '\0';
    if (lineseparator == comment)
      lineseparator = '\0';
    if (whitespace == comment)
      whitespace = '\0';
    if (clamper == comment)
      clamper = '\0';
    
    /* Put a comment symbol */
    if (!checklinelength(filepointer, linebreak, linelength, &current_length, 1+ !!whitespace))
      goto error;
    if (fputc(comment, filepointer) == EOF)
      goto error;
    ++current_length;
    /* If whitespace is set we use one here */
    if ((whitespace)) {
      if (fputc(whitespace, filepointer) == EOF)
	goto error;
      ++current_length;
    }
  }
  
  if (*array) {
    /* Put out the first word */
    if (fields) {
      /* Check the first word for special symbols to be clamped and take
	 steps */
      if ((clamper)) {
	if (isinstring(*array,fieldseparator) || isinstring(*array, lineseparator) || isinstring(*array, comment) || isinstring(*array, whitespace)) {
	  
	  /* Check for the length of the line */
	  if ((whitespace)) {
	    if (!checklinelength(filepointer, linebreak, linelength, &current_length, 2))
	      goto error;
	  }
	  else
	    {	  if (!checklinelength(filepointer, linebreak, linelength, &current_length, 1))
	      goto error;
	    }
	  if (fputc(clamper,filepointer) == EOF)
	    goto error;
	  else
	    ++current_length;
	  if ((whitespace)) {
	    if (fputc(whitespace,filepointer) == EOF)
	      goto error;
	    else
	      ++current_length;
	  }
	}
      }
      
      /* Now we really put out the word */
      
      /* Check for the linelength, at least a semicolon has to follow,
	 thus '+1'*/
      if (!checklinelength(filepointer, linebreak, linelength, &current_length, strlen(*array)+!!(fieldseparator)))
	goto error;

      /* put the word in the stream */

      /* We have to check for the length of the array, because
	 otherwise fprintf would report an error in case of an empty
	 string */
      if ((strlen(*array))) {
	if (fprintf(filepointer, "%s", *array) < 0)
	  goto error;
	current_length = current_length + strlen(*array);
      }

      /* Now the story with the clamper again */
      if ((clamper)) {
	if (isinstring(*array,fieldseparator) || isinstring(*array, lineseparator) || isinstring(*array, comment) || isinstring(*array, whitespace)) {
	  
	  /* Check for the length of the line */
	  if ((whitespace)) {
	    if (!checklinelength(filepointer, linebreak, linelength, &current_length, 2+!!(fieldseparator)))
	      goto error;
	  }
	  else {
	    if (!checklinelength(filepointer, linebreak, linelength, &current_length, 1+!!(fieldseparator)))
	      goto error;
	  }
	  
	  if ((whitespace)) {
	    if (fputc(whitespace,filepointer) == EOF)
	      goto error;
	    else
	      ++current_length;
	    if (fputc(clamper,filepointer) == EOF)
	      goto error;
	    else
	      ++current_length;
	  }
	}
      }
      --fields;
    }
  }

  ++array;
  /* Now put the content of the rest of the array in the file */
  while ((*array)) {

    /* We put the fieldseparator */
    if ((fieldseparator))
      if (fputc(fieldseparator,filepointer) == EOF)
	goto error;

    /* If the maximum nuber of fields is reached stop */
    if (!(fields))
      break;

    /* Now sort out if we need clamping */
    if ((clamper)) {
      if (isinstring(*array,fieldseparator) || isinstring(*array, lineseparator) || isinstring(*array, comment) || isinstring(*array, whitespace) ) {

	/* Check for the length of the line */
	if ((whitespace)) {
	  if (!checklinelength(filepointer, linebreak, linelength, &current_length, 2))
	      goto error;
	}
	else {
	  if (!checklinelength(filepointer, linebreak, linelength, &current_length, 1))
	    goto error;
	}

	if (fputc(clamper,filepointer) == EOF)
	  goto error;
	else
	  ++current_length;
	if ((whitespace)) {
	  if (fputc(whitespace,filepointer) == EOF)
	    goto error;
	  else
	    ++current_length;
	}
      }
    }

    /* Now put out the word */
    if (!checklinelength(filepointer, linebreak, linelength, &current_length, strlen(*array) + !!whitespace))
      goto error;

    /* If we are at the start of a line we put no blank, if not we do */
    if ((whitespace)) {
      if ((current_length)) {
	if (fputc(whitespace, filepointer) == EOF)
	  goto error;
	current_length = current_length + !!current_length;
      }
    }

    /* We have to check for the length of the array, because otherwise fprintf would report an error in case of an empty string */
    if ((strlen(*array))) {
    if (fprintf(filepointer, "%s", *array) < 0)
      goto error;
    current_length = current_length + strlen(*array);
    }

    /* At the end, again, check for the clamping */
    if ((clamper)) {
      if (isinstring(*array,fieldseparator) || isinstring(*array, lineseparator) || isinstring(*array, comment) || isinstring(*array, whitespace)) {

	/* Check for the length of the line */
	if ((whitespace)) {
	  if (!checklinelength(filepointer, linebreak, linelength, &current_length, 2+!!(fieldseparator)))
	    goto error;
	}
	else {
	  if (!checklinelength(filepointer, linebreak, linelength, &current_length, 1+!!(fieldseparator)))
	    goto error;
	}
	
	if ((whitespace)) {
	  if (fputc(whitespace,filepointer) == EOF)
	    goto error;
	  else
	    ++current_length;
	  if (fputc(clamper,filepointer) == EOF)
	    goto error;
	  else
	    ++current_length;
	}
      }
    }

    ++array;
    --fields;
  }

  /* Now we are through and have to put as a last word a lineseparator
     which we shouldn't have done */

  if ((lineseparator)) {
    /* This should work now */
      if (fputc(lineseparator,filepointer) == EOF)
	goto error;
  }

  /* At the end we search if there should be another comment sign */
  if ((comment)) {

    /* If whitespace is set we use one here */
    if ((whitespace)) {
      if (!checklinelength(filepointer, linebreak, linelength, &current_length, 2))
	goto error;
      if (fputc(whitespace, filepointer) == EOF)
	goto error;
      ++current_length;
    }
    else
      {	  if (!checklinelength(filepointer, linebreak, linelength, &current_length, 1))
	goto error;
      }

    /* Put a comment symbol */
    if (fputc(comment, filepointer) == EOF)
      goto error;
    ++current_length;
  }

  return 1;

  /* The error we may encounter is that we cannot write into stream, which is constantly checked */  
 error:
  error_msg_parser("parser.c","putparsed","Error writing to open file: disk error?",0);
  fsetpos(filepointer, &position);
  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Silly helper for the putparsed function, checking the length of
   a line */

static int checklinelength(FILE *stream, char yesno, int maxlength, \
int *current_length, int wordlength)
{
  if (!(yesno))
    return 1;
  if (*current_length) {
    if (wordlength + *current_length > maxlength) {
      if (fputc('\n',stream) == EOF)
	return 0;
      *current_length = 0;
    }
  }
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Generic error message */

static void error_msg_parser(char *call_file, char *call_function, char *call_msg, int line)
{
  fprintf(stderr,"ERROR REPORTED\n");
  if (*call_file != '\0')
  fprintf(stderr,"FILE: %s\n", call_file);
  if (*call_function != '\0')
  fprintf(stderr,"FUNCTION: %s\n", call_function);
  if (*call_msg != '\0')
  fprintf(stderr,"MESSAGE: %s \n", call_msg);
  if (line != 0)
  fprintf(stderr,"Line: %i\n", line);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns a control structure for decomp functions */
static dcp_control *dcp_init(void)
{
  dcp_control *dcp_init;

  if (!(dcp_init = (dcp_control *) malloc(sizeof(dcp_control))))
    return NULL;

  /* initialise */
  dcp_init -> grse = ',';
  dcp_init -> dein = '*';
  dcp_init -> deco = ':';
  dcp_init -> ninin = 0;
  dcp_init -> inin = NULL;
  dcp_init -> pall = NULL;
  dcp_init -> error = NULL;

  return dcp_init;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns a control structure for decomp functions */
decomp_control decomp_init(void)
{
  return (decomp_control) dcp_init();
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destroys dcp_controlv */
static void dcp_dest(dcp_control *dcp_controlv)
{
  dcp_pall *pall;

  if (dcp_controlv) {
    if (dcp_controlv -> inin)
      free(dcp_controlv -> inin);
    if (dcp_controlv -> error)
      free(dcp_controlv -> error);

    pall = dcp_controlv -> pall;
    while ((pall = dcp_pall_dest(pall)))
      ;
    free(dcp_controlv);
  }

  return;
}
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destructs decomp_controlv */
void decomp_dest(decomp_control decomp_controlv)
{
  dcp_dest((dcp_control *) decomp_controlv);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destructs decomp_controlv */
static char *dcp_errmsg(dcp_control *dcp_controlv)
{
  return dcp_controlv -> error;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destructs decomp_controlv */
char *decomp_errmsg(decomp_control decomp_controlv)
{
return  dcp_errmsg((dcp_control *) decomp_controlv);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Creates an element of dcp_pall liked list and appends it to list. pana will be copied loacally. */
static int dcp_inp(dcp_control *dcp_controlv, const char* pana, int papo, int pale)
{
  dcp_pall *dcp_pall_app = NULL;
  dcp_pall *count;
  int pananlen;

  if (!dcp_controlv)
    goto error;

  /* Allocate the element */
  if (!(dcp_pall_app = (dcp_pall *) malloc(sizeof(dcp_pall))))
    goto error;

  dcp_pall_app -> name = NULL;
  dcp_pall_app -> next = NULL;


  /* Check pana */
  if (!pana)
    goto error;

  /* Count words in pana */
  pananlen = 0;
  while (pana[pananlen++])
    ;

  /* Allocate and copy pana */
  if (!(dcp_pall_app -> name = (char *) malloc(pananlen*sizeof(int))))
    goto error;
  strcpy(dcp_pall_app -> name,pana);

  /* Put papo and pale into place */
  dcp_pall_app -> posi = papo;
  dcp_pall_app -> numb = pale;

  /* link it */
  if ((count = dcp_controlv -> pall)) {
    
    while (count -> next)
      count = count -> next;
    
    count -> next = dcp_pall_app;
  }
  else {
    dcp_controlv -> pall = dcp_pall_app;
  }

  return 0;

 error:
  dcp_pall_dest(dcp_pall_app);
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Input a parameter with name, position, and length */
int decomp_inp(decomp_control decomp_controlv, const char* pana, int papo, int pale)
{
  return dcp_inp((dcp_control *) decomp_controlv, pana, papo, pale);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destroys dcp_pallv and returns the next element */
static  dcp_pall *dcp_pall_dest(dcp_pall *dcp_pallv)
{
  dcp_pall *dcp_pall_dest;

  if (dcp_pallv) {
    if (dcp_pallv -> name)
      free(dcp_pallv -> name);
    dcp_pall_dest = dcp_pallv -> next;
    free(dcp_pallv);
    return dcp_pall_dest;
  }
  return NULL;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Define separators */
static int dcp_putsep(dcp_control *dcp_controlv, char grse, char dein, char deco)
{
  if (!dcp_controlv)
    return 2;

  if (grse == dein || dein == deco || deco == grse)
    return 1;

  dcp_controlv -> grse = grse;
  dcp_controlv -> dein = dein;
  dcp_controlv -> deco = deco;

  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Replaces the indexed list in dcp_controlv with the list given. Makes a local copy */
static int dcp_index(dcp_control *dcp_controlv, int number, int *list)
{
  int *internlist, length;

  if (!dcp_controlv || number < 0)
    return 1;

  if (number > 0) {
    if (!(internlist = (int *) malloc(number*sizeof(int))))
      return 1;
    dcp_controlv -> ninin = number;
  }
  else {
    internlist = NULL;
    dcp_controlv -> ninin = 0;
  }


  if (dcp_controlv -> inin)
    free(dcp_controlv -> inin);

  length = number;

  /* This is new, I think important */

  while(length--)
    internlist[length] = list[length];

  bubble(number, internlist);

  dcp_controlv -> inin = internlist;

  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Replaces the indexed list in decomp_controlv with the list given. Makes a local copy */
int decomp_index(decomp_control decomp_controlv, int number, int *list)
{
  return dcp_index((dcp_control *) decomp_controlv, number, list);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Define separators */
int decomp_putsep(decomp_control decomp_controlv, char grse, char dein, char deco)
{
  return dcp_putsep((dcp_control *) decomp_controlv, grse, dein, deco);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destructor for a null terminated array of decomp_listel structs */
void decomp_list_dest(decomp_listel *decomp_listelv)
{
  int i = 0;

  if (!decomp_listelv)
    return;

  while ((decomp_listelv + i) -> nuel != -1) {
    if ((decomp_listelv + i) -> nuel) {
      if ((decomp_listelv + i) -> poli)
	free((decomp_listelv + i) -> poli);
    }
    ++i;
  }
  
  free(decomp_listelv);
  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns a null terminated list of positions */

#define ERROR_LENGTH 43 /* length of error message */

static int dcp_get(dcp_control *dcp_controlv, char *input, decomp_listel **output, int nega)
{

  decomp_listel *decomp_listelv = NULL;
  char **prs_1 = NULL;
  char **prs_2 = NULL;
  char fs[2];
  char dein = 0;
  int i, action;
  int justname;
  int elinf[2];
  int line = 0;
  int group = 0;
  int elnum_out = 0;
  int minindex = 0, maxindex = 0;
  size_t position;
  dcp_listll *llstart = NULL,*llrun = NULL, *llfirst, *llbefn = NULL, **lltop;
  int errcode = 0;
  int run = 0;
  int listi;
  int nupocur;
  char *inputstart;
  

  if (dcp_controlv -> error)
    free(dcp_controlv -> error);

  if (!(dcp_controlv -> error = (char *) malloc((ERROR_LENGTH+1)*sizeof(char))))
    return 1;

  if ((!dcp_controlv)) {
    sprintf(dcp_controlv -> error,"No decomp_controlv");
    errcode = 2;
    goto error;
  }

  if ((!input)) {
    sprintf(dcp_controlv -> error,"No input");
    errcode = 2;
    goto error;
  }

  /* make a string of the group separator */
  fs[0] = dcp_controlv -> grse;
  fs[1] = '\0';

  inputstart = input;
  
  /* start with checking if the input string contains only whitespaces */
  i = 0;
  listi = 1;

  while (input[i]) {
    if (input[i] != '\t' || input[i] != ' ') {
      listi = 0;
      break;
    }
    ++i;
  }
  
  /* If now we have encountered an empty string, we return a list containing a list of one listel with -1 elements of content (indicating end of list) */

  if ((listi)) { 
    if (!(decomp_listelv = (decomp_listel *) malloc(sizeof(decomp_listel)))) {
	sprintf(dcp_controlv -> error,"Memory allocation problems");
	errcode = 1;
	goto error;
    }

    /* fill this with content */
    decomp_listelv -> nuel = -1;
    decomp_listelv -> grnr = 0;
    decomp_listelv -> poli = NULL;

    *output = decomp_listelv;
    return 0;
  }

  listi = 0;

  /* one time we count, the next time we produce the object */
  while (run < 2) {
    
    input = inputstart;

    /* essential to be here and not out of the loop */
    if (!(prs_1 = sparsenext(fs, "", "", "", "", "", 0, &input, &line, 0, 1))) {
      sprintf(dcp_controlv -> error ,"input is NULL or memory allocation problems");
      errcode = 1;
      goto error;
    }

    while (prs_1[group]) {
      /* look if the first character in the group is a decompose indicator */
      position = 0;
      while (prs_1[group][position] == ' ' || prs_1[group][position] == '\t')
	++position;

      if (prs_1[group][position] == dcp_controlv -> dein) {
	dein = 1;
	prs_1[group][position] = ' ';
      }

      /* Hack this field into chunks, separated by whitespaces */
      if (!(prs_2 = sparsenext(" \t", "", "", "", "", "", 0, prs_1+group, &line, 1, 1))) {
	sprintf(dcp_controlv -> error,"Memory allocation problems");
	errcode = 1;
	goto error;
      }

      /* Check if the first element in the group is reasonable, i.e., must be a known name */
      i = 0;

      if (prs_2[i]) {
	if (dcp_checkiden(prs_2[i], dcp_controlv -> pall, elinf)) {
	  sprintf(dcp_controlv -> error,"Syntax: ");
	  strncpy(dcp_controlv -> error+8, prs_1[group], ERROR_LENGTH-8);
	  errcode = 2;
	  goto error;
	}
      }
      else {
	sprintf(dcp_controlv -> error,"Syntax: ");
	strncpy(dcp_controlv -> error+8, "empty field", ERROR_LENGTH-8);
	errcode = 2;
	goto error;      
      }

      ++i;
      /* Indicates that a name has just been read */
      justname = 1;
      
      /* Now go through the list making a ll */
      
      lltop = &llstart;
      llrun = NULL;

      while(prs_2[i]) {

	action = 0;

	if (dcp_checkiden(prs_2[i], dcp_controlv -> pall, elinf)){
	  
	  /* Check for two consecutive similar parameters */
	  
	  llrun = dcp_listll_app(elinf, prs_2[i], &errcode, dcp_controlv -> deco, nega);
	  action = 1;
	  
	  justname = 0;
	}
	else if (justname) {
	  dcp_checkiden(prs_2[i-1], dcp_controlv -> pall, elinf);
	  
	  llrun = dcp_listll_app(elinf, "", &errcode, dcp_controlv -> deco, nega);
	  dcp_checkiden(prs_2[i], dcp_controlv -> pall, elinf);
	  action = 1;
	  justname = 1;
	}
	else {
	  dcp_checkiden(prs_2[i-1], dcp_controlv -> pall, elinf);
	  
/* 	  llrun = dcp_listll_app(elinf, "", &errcode, dcp_controlv -> deco, nega); */
/* 	  dcp_checkiden(prs_2[i], dcp_controlv -> pall, elinf); */
	  action = 0;
	  justname = 1;
	}
	++i;
	
	if (llrun) {
	  if (action) {
	    *lltop = llrun;
	    while (llrun -> next) {
	      llrun = llrun -> next;
	    }
	    
	    lltop = &(llrun -> next);
	  }
	}
	else {
	  /* print some error message here */
	  if (errcode == 2) {
	    sprintf(dcp_controlv -> error,"Syntax: ");
	    strncpy(dcp_controlv -> error+8, prs_1[group], ERROR_LENGTH-8);
	  }
	  else if (errcode == 1) {
	    sprintf(dcp_controlv -> error,"Memory allocation problems");
	  }
	  goto error;
	}
      }
      
      
      /* The last element has been reached, but it's a name, followed by nothing */
      if (justname) {
	dcp_checkiden(prs_2[i-1], dcp_controlv -> pall, elinf);
	
	llrun = dcp_listll_app(elinf, "", &errcode, dcp_controlv -> deco, nega);
	
	if (llrun) {
	  *lltop = llrun;
	  while (llrun -> next) {
	    llrun = llrun -> next;
	  }
	  
	  lltop = &(llrun -> next);
	}
	else {
	  /* print some error message here */
	  if (errcode ==2) {
	    sprintf(dcp_controlv -> error,"Syntax: ");
	    strncpy(dcp_controlv -> error+8, prs_2[i-1], ERROR_LENGTH-8);
	  }
	  else if (errcode == 1) {
	    sprintf(dcp_controlv -> error,"Memory allocation problems");
	  }
	  goto error;
	}
      }    
      
            
      /* delete things that are on the index */
      llrun = llstart;
      lltop = &llstart;

      /* First all indexed items at the top of the list */	
      while (llrun) {
	for (i = 0; i < dcp_controlv -> ninin; ++i) {
	  if (llrun -> nupo[0] == dcp_controlv -> inin[i]) {
	    llbefn = llrun;
	    llrun = llrun -> next;
	    free(llbefn);
	    llstart = llrun;
	    lltop = &llstart;
	    break;
	  }
	}

	if (i == dcp_controlv -> ninin) {
	  break;
	}
      }


      /* Then delete the rest */
      llrun = llstart;

      while (llrun) {
	for (i = 0; i < dcp_controlv -> ninin; ++i) {
	  if (llrun -> nupo[0] == dcp_controlv -> inin[i]) {
	    break;
	  }
	}
	if (i == dcp_controlv -> ninin) {
	  llbefn = llrun;
	  llrun = llrun -> next;
	}
	else {
	  llbefn -> next = llrun -> next;
	  free(llrun);
	  llrun = llbefn -> next;
	}
      }

      /******* check *****/
      /******************/
/*       fprintf(stderr,"start check for group after delete \n"); */
      
/*       llrun = llstart; */
/*       while (llrun) { */
/* 	fprintf(stderr,"%i ",llrun -> nupo[0]); */
/* 	llrun = llrun -> next; */
/*       } */
/*       fprintf(stderr,"\n"); */
/*       llrun = llstart; */
/*       while (llrun) { */
/* 	fprintf(stderr,"%i ",llrun -> nupo[1]); */
/* 	llrun = llrun -> next; */
/*       } */
/*       fprintf(stderr,"\n\n"); */
      /*******************/

      /* Now sort */

      /* continue here */
      if (!dein) {
	;
      }
      else {
	/* first find start and end number */
	llrun = llstart;
	lltop = &llstart;
	
	if (llrun)
	  minindex = maxindex = llrun -> nupo[1];
	
	while (llrun) {
	  minindex = minindex > llrun -> nupo[1] ? llrun -> nupo[1] : minindex;	
	  maxindex = maxindex < llrun -> nupo[1] ? llrun -> nupo[1] : maxindex;	
	  lltop = &(llrun -> next);
	  llrun = llrun -> next;
	}      

	/* Then go through the list and swap, if necessary */

	for (i = minindex; i <= maxindex; ++i) {
	  llrun = llstart;
	  llfirst = NULL;
	  llbefn = NULL;
	  
	  while (llrun) {
	    if (llrun -> nupo[1] == i) {
	      if (llfirst) {
		dcp_swapll(llfirst,llrun,llbefn);
	      }
	      llfirst = llrun;
	    }
	    llbefn = llrun;
	    llrun = llrun -> next;
	  } 
	}
      }
      
      /* check if there is something to be sent to the output, issue a warning, not an error */
      if (!llstart) {
	sprintf(dcp_controlv -> error,"Warning, on Index: ");
	strncpy(dcp_controlv -> error+19, prs_1[group], ERROR_LENGTH-19);
	/* 	goto error;    */
      }

      /* count memory for the output in a first run */
      if (run == 0) {
	if (dein) {
	  nupocur = minindex - 1;
	  if ((llrun = llstart))
	    ++elnum_out;
	  while (llrun) {
	    llrun = llrun -> next;
	    if (llrun && (llrun -> nupo[1] != nupocur)) {
	      ++elnum_out;
	      nupocur = llrun -> nupo[1];
	    }
	  }
	}
	else
	  ++elnum_out;
      }
      
      /* continue filling the decomp_listel table in a second run */
      else {
	if (dein) {
	  llfirst = llrun = llstart;
	  if (llrun)
	    nupocur = llrun -> nupo[1];
	  while (llrun) {
	    i = 0;

	    while(llrun && (llrun -> nupo[1] == nupocur)) {
	      ++i;
	      llrun = llrun -> next;
	    }

	    if (!((decomp_listelv + listi) -> poli = (int *) malloc(i*sizeof(int)))) {
	      sprintf(dcp_controlv -> error,"Memory allocation problems");
	      errcode = 1;
	      goto error;
	    }
	    llrun = llfirst;

	    (decomp_listelv + listi) -> nuel = i;
	    (decomp_listelv + listi) -> grnr = group;

	    i = 0;
	    while(llrun && (llrun -> nupo[1] == nupocur)) {
	      (decomp_listelv + listi) -> poli[i] = llrun -> nupo[0];
	      llrun = llrun -> next;
	      ++i;
	    }
	    if (llrun)
	      nupocur = llrun -> nupo[1];
	    llfirst = llrun;
	    ++listi;
	  }
	}
	else {
	  llrun = llstart;
	  i = 0;
	  while (llrun) {
	    ++i;
	    llrun = llrun -> next;
	  }
	  if (!((decomp_listelv + listi) -> poli = (int *) malloc(i*sizeof(int)))) {
	    sprintf(dcp_controlv -> error,"Memory allocation problems");
	    errcode = 1;
	    goto error;
	  }
	  (decomp_listelv + listi) -> nuel = i;
	  (decomp_listelv + listi) -> grnr = group;

	  llrun = llstart;
	  i = 0;
	  while (llrun) {
	    (decomp_listelv + listi) -> poli[i] = llrun -> nupo[0];
	    llrun = llrun -> next;
	    ++i;
	  }
	  ++listi;
	}
      }
      freeparsed(prs_2);
      dcp_listll_dest(&llstart);

      dein = 0;

      ++group;
    }
    dein = 0;
    line = 0;
    group = 0;
    dcp_listll_dest(&llstart);
    dcp_listll_dest(&llrun);
    freeparsed(prs_1);

    if ((!run)) {
      if (!(decomp_listelv = (decomp_listel *) malloc((elnum_out+1)*sizeof(decomp_listel)))) {
	sprintf(dcp_controlv -> error,"Memory allocation problems");
	errcode = 1;
	goto error;
      }
      for (i = 0; i < (elnum_out+1);++i) {
	(decomp_listelv + i) -> poli = NULL;
      }
    }
    ++run;
 }

  /* -1-terminate the last element */
  (decomp_listelv + listi) -> nuel = -1;

  dcp_listll_dest(&llstart);
  dcp_listll_dest(&llrun);

  /* finis */
  *output = decomp_listelv;
  return errcode;

 error:
  if (prs_1)
    freeparsed(prs_1);
  if (prs_2)
    freeparsed(prs_2);
  dcp_listll_dest(&llstart);
  dcp_listll_dest(&llrun);
  decomp_list_dest(decomp_listelv);
  return errcode;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns a null terminated list of positions */
int decomp_get(decomp_control decomp_controlv, char *input, decomp_listel **output, int nega)
{
  return dcp_get((dcp_control *) decomp_controlv, input, output, nega);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* puts after behind first and mends the ll */
static void dcp_swapll(dcp_listll *first, dcp_listll *after, dcp_listll *beforeafter) 
{
  dcp_listll *dummy;

  dummy = first -> next;

  if (first == beforeafter) {
    return;
  }

  first -> next = after;
  beforeafter -> next = after -> next;
  after -> next = dummy;

  return;
}

/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Bubble-sort array of length length */

static void bubble(int length, int *array)
{
  int dummy;
  int counter = 0, goon = 1;

  while (goon) {
    counter = 0;
    goon = 0;
    while (++counter < length) {      
      if (array[counter-1] > array[counter]) {
	dummy = array[counter-1];
	array[counter-1] = array[counter];
	array[counter] = dummy;
	goon = 1;
	while (++counter < length) {
	  dummy = array[counter-1];
	  array[counter-1] = array[counter];
	  array[counter] = dummy;
	}
      }
    }
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Check ll pall for occurrence of an identifier in string */
static int dcp_checkiden(char *string, dcp_pall *pall, int *info)
{
  while (pall) {
  
    if (!strcmp(pall -> name, string)) {
      info[0] = pall -> posi;
      info[1] = pall -> numb;
      return 0;
    }
    pall = pall -> next;
  }

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* destroys the dcp_listll ll and sets *dcp_listllv = NULL */
 static void dcp_listll_dest(dcp_listll **dcp_listllv)
{
  dcp_listll *ll, *ll2;
  if (!dcp_listllv)
    return;

  if (!*dcp_listllv)
    return;

  ll = *dcp_listllv;

  while (ll) {
    ll2 = ll -> next;
    free(ll);
    ll = ll2;
  }
  
  *dcp_listllv = NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Appends to dcp_listll dcp_listlls elements dechiffering input */
 static dcp_listll *dcp_listll_app(int *startlength, char *input, int *error, char sep, int nega)
{
  dcp_listll *ll, *llstart = NULL;
  char **input_content;
  char separ[2];
  int line, els;
  int start,end,step;

  separ[0] = sep;
  separ[1] = '\0';

  if (!(input_content = sparsenext(separ, "", "", "", "", "", 0, &input, &line, 0, 1))) {
    *error = 1;
    return NULL;
  }

  els = 0;
  if (input[0]) {
    while (input_content[els])
      ++els;
  }

  if (els > 3) {
    *error = 2;
    goto error;
  }
  
  if (els > 0) {
    if (!startlength[1]) {
      *error = 2;
      goto error;
    }
    
    /* read in the start */
    if (!(sscanf(input_content[0],"%i",&start)>0)) {
      if (input_content[0][0] == '\0') {
	start = 1;
      }
      else {
	*error = 2;
	goto error;
      }
    }
    
    /* read in the end */
    if (els > 1) {
      if (!(sscanf(input_content[1],"%i",&end) > 0)) {
	if (input_content[1][0] == '\0') {
	  end = startlength[1];
	}
	else {
	  *error = 2;
	  goto error;
	}
      }
    }
    else {
      end = start;
    }

    /* read in the step */
    if (els > 2) {
      if (!(sscanf(input_content[2],"%i",&step) > 0)) {
	*error = 2;
	goto error;	
      }
    }
    else {
      if (start > end)
	step = -1;
      else
	step = 1;
    }
  } 
  else {
    start = 1;
    end = startlength[1]?startlength[1]:1;
    step = 1;
  }

  if (!nega) {
    if (start < 1 || end < 1) {
      *error = 2;
      goto error;
    }
    if ((start > startlength[1]) || (end > startlength[1])) {
      if (!((start == 1) && (end == 1) && (startlength[1] == 0))) {
	*error = 2;
	goto error;
      }
    }
  }

  if (!step) {
      *error = 2;
      goto error;
  }

  if (start > end) {
    if (step >= 0) {
      *error = 2;
      goto error;
    }
  }
  else if (end > start) {
    if (step <= 0) {
      *error = 2;
      goto error;
    }
  }


  /* Make the ll */
  if (!(ll = llstart = (dcp_listll *) malloc(sizeof(dcp_listll)))) {
    goto error;
  }
  ll -> next = NULL;
  ll -> nupo[0] = startlength[0]+start-1;
  ll -> nupo[1] = start;
  start = start + step;

  if (step > 0) {
    while (start <= end) {
      
      if (!(ll -> next = (dcp_listll *) malloc(sizeof(dcp_listll)))) {
	goto error;
      }
      ll = ll -> next;
      ll -> next = NULL;
      ll -> nupo[0] = startlength[0]+start-1;
      ll -> nupo[1] = start;    
      start = start + step;
    }
  }
  else {
    while (start >= end) {
      
      if (!(ll -> next = (dcp_listll *) malloc(sizeof(dcp_listll)))) {
	goto error;
      }
      ll = ll -> next;
      ll -> next = NULL;
      ll -> nupo[0] = startlength[0]+start-1;
      ll -> nupo[1] = start;    
      start = start + step;
    }
  }

  ll = llstart;
  while (ll) {
    ll = ll -> next;
  }
  ll = llstart;
  while (ll) {
    ll = ll -> next;
  }
  /*******************/
  /***************/
  
  freeparsed(input_content);
  return llstart;
  
 error:
  dcp_listll_dest(&llstart);
  freeparsed(input_content);
  return NULL;
}
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Constructor for a decomp_inlist struct */
decomp_inlist *decomp_inlist_init(void)
{
  decomp_inlist *decomp_inlist_init;
  if (!(decomp_inlist_init = (decomp_inlist *) malloc(sizeof(decomp_inlist))))
    return NULL;
  decomp_inlist_init -> ipa = NULL;
  decomp_inlist_init -> inpal = NULL;
  decomp_inlist_init -> inpah = NULL;
  decomp_inlist_init -> ripa = NULL;
  decomp_inlist_init -> rinpal = NULL;
  decomp_inlist_init -> rinpah = NULL;
  decomp_inlist_init -> nuel = 0;

  return decomp_inlist_init;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destructor for a decomp_inlist struct */
void decomp_inlist_dest(decomp_inlist *decomp_inlistv)
{
  if (decomp_inlistv) {
    if (decomp_inlistv -> ipa)
      free(decomp_inlistv -> ipa);
    if (decomp_inlistv -> inpal)
      free(decomp_inlistv -> inpal);
    if (decomp_inlistv -> inpah)
      free(decomp_inlistv -> inpah);
    if (decomp_inlistv -> ripa)
      free(decomp_inlistv -> ripa);
    if (decomp_inlistv -> rinpal)
      free(decomp_inlistv -> rinpal);
    if (decomp_inlistv -> rinpah)
      free(decomp_inlistv -> rinpah);
    
    free(decomp_inlistv);
  }
  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns a struct containing the next active indices of indexed indices */
int decomp_get_inlist(decomp_control decomp_controlv, decomp_inlist *decomp_inlistv)
{
  return dcp_get_inlist((dcp_control *) decomp_controlv, decomp_inlistv);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Search an int list for a match */
static int matchint(int val, int nelem, int *thelist)
{
  int i, j = 0;

  for (i = 0; i < nelem; ++i) {
    if (val == thelist[i]) {
      j = 1;
      break;
    }
  }

  return j;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns a struct containing the next active indices of indexed indices */
static int dcp_get_inlist(dcp_control *dcp_controlv, decomp_inlist *decomp_inlistv)
{

  dcp_pall *pall_cur;
  int i, dcp_get_inlist = 0, start, ende, number, found;
  int *ipa = NULL, *inpal = NULL, *inpah = NULL;
  int *ripa = NULL, *rinpal = NULL, *rinpah = NULL;

  /* allocate and deallocate in the inlist struct */
  if (decomp_inlistv -> ipa)
    free(decomp_inlistv -> ipa);
  if (decomp_inlistv -> inpal)
    free(decomp_inlistv -> inpal);
  if (decomp_inlistv -> inpah)
    free(decomp_inlistv -> inpah);
  if (decomp_inlistv -> ripa)
    free(decomp_inlistv -> ripa);
  if (decomp_inlistv -> rinpal)
    free(decomp_inlistv -> rinpal);
  if (decomp_inlistv -> rinpah)
    free(decomp_inlistv -> rinpah);
  
  if ((dcp_controlv -> ninin)) {
    if (!(ipa = (int *) malloc((dcp_controlv -> ninin)*sizeof(int)))) {
      goto error;
    }
    if (!(inpal = (int *) malloc((dcp_controlv -> ninin)*sizeof(int)))) {
      goto error;
    }
    if (!(inpah = (int *) malloc((dcp_controlv -> ninin)*sizeof(int)))) {
      goto error;
    }
    if (!(ripa = (int *) malloc((dcp_controlv -> ninin)*sizeof(int)))) {
      goto error;
    }
    if (!(rinpal = (int *) malloc((dcp_controlv -> ninin)*sizeof(int)))) {
      goto error;
    }
    if (!(rinpah = (int *) malloc((dcp_controlv -> ninin)*sizeof(int)))) {
      goto error;
    }
  }

  decomp_inlistv -> nuel = dcp_controlv -> ninin;
  decomp_inlistv -> ipa = ipa;
  decomp_inlistv -> inpal = inpal;
  decomp_inlistv -> inpah = inpah;
  decomp_inlistv -> ripa = ripa;
  decomp_inlistv -> rinpal = rinpal;
  decomp_inlistv -> rinpah = rinpah;


  /* Go through the indices */
  for (i = 0; i < dcp_controlv -> ninin; ++i) {

    decomp_inlistv -> ipa[i] = dcp_controlv -> inin[i];

    /* Search the group */
    pall_cur = dcp_controlv -> pall;
      found = 0;

    while (pall_cur) {
      if ((dcp_controlv -> inin[i] >= (start = pall_cur -> posi)) && (dcp_controlv -> inin[i] <= (ende = (pall_cur -> posi + pall_cur -> numb-1)))) {
	
	found = 1;

	/* Check if all parameters are indexed */
	number = 0;
	while (start <= ende) {
	  if ((number = !matchint(start, dcp_controlv -> ninin, dcp_controlv -> inin))) {
	    break;
	  }
	  ++start;
	}

	if (!number) {
	  decomp_inlistv -> inpal[i] = dcp_controlv -> inin[i];
	  decomp_inlistv -> inpah[i] = dcp_controlv -> inin[i];
	  dcp_get_inlist = dcp_get_inlist | 2;
	} 
	else {
	  start = pall_cur -> posi;
	  
	  /* Now find the highest element that is not indexed */
	  number = start-1;
	  while (start < dcp_controlv -> inin[i]) {
	    if (!matchint(start, dcp_controlv -> ninin, dcp_controlv -> inin))
	      number = start;
	    ++start;
	  }
	  
	  if (number == (pall_cur -> posi-1)) {
	    decomp_inlistv -> inpal[i] = dcp_controlv -> inin[i];
	  }
	  else {
	    decomp_inlistv -> inpal[i] = number;
	  }
	  
	  /* Now find the lowest element that is not indexed */
	  number = ende+1;
	  while (ende > dcp_controlv -> inin[i]) {
	    if (!matchint(ende, dcp_controlv -> ninin, dcp_controlv -> inin))
	      number = ende;
	    --ende;
	  }
	  
	  if (number == (pall_cur -> posi + pall_cur -> numb)) {
	    decomp_inlistv -> inpah[i] = dcp_controlv -> inin[i];
	  }
	  else {
	    decomp_inlistv -> inpah[i] = number;
	  }
	}
        break;
      }
      pall_cur = pall_cur -> next;
    }
    if (!found) {
      decomp_inlistv -> inpal[i] = decomp_inlistv -> ipa[i];
      decomp_inlistv -> inpah[i] = decomp_inlistv -> ipa[i];
      dcp_get_inlist = dcp_get_inlist | 1;
    }
    if (decomp_inlistv -> inpal[i] == decomp_inlistv -> ipa[i]) {
      decomp_inlistv -> inpal[i] = decomp_inlistv -> inpah[i];
    }
    if (decomp_inlistv -> inpah[i] == decomp_inlistv -> ipa[i]) {
      decomp_inlistv -> inpah[i] = decomp_inlistv -> inpal[i];
    }

    /* Now fill in the relative positions */

    if (found) {
      decomp_inlistv -> ripa[i] = decomp_inlistv -> ipa[i]- pall_cur -> posi;
      decomp_inlistv -> rinpal[i] = decomp_inlistv -> inpal[i] - pall_cur -> posi;
      decomp_inlistv -> rinpah[i] = decomp_inlistv -> inpah[i] - pall_cur -> posi;
    }

    /* here it makes no sense */
    else {
      decomp_inlistv -> ripa[i] = decomp_inlistv -> rinpal[i] = decomp_inlistv -> rinpah[i] = decomp_inlistv -> ipa[i];
    }

  }
  return dcp_get_inlist;

 error:
  if (ipa)
    free(ipa);
  if(inpal)
    free(inpal);
  if (inpah)
    free(inpah);
  if (ripa)
    free(ripa);
  if(rinpal)
    free(rinpal);
  if (rinpah)
    free(rinpah);
  return 3;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Allocate simparse_scn_keyvalli struct, setting internal pointers to NULL, return pointer */
static simparse_scn_keyvalli *simparse_scn_keyvalli_create(void)
{
  simparse_scn_keyvalli *simparse_scn_keyvalli_create = NULL;

  if (!(simparse_scn_keyvalli_create = (simparse_scn_keyvalli *) malloc(sizeof(simparse_scn_keyvalli))))
    return NULL;

  simparse_scn_keyvalli_create -> key = NULL;
  simparse_scn_keyvalli_create -> val = NULL;
  simparse_scn_keyvalli_create -> line = 0;

  return simparse_scn_keyvalli_create;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Deallocate simparse_scn_keyvalli struct, the pointer is pointing to, including strings therein */
static void simparse_scn_keyvalli_dest(simparse_scn_keyvalli *simparse_scn_keyvalliv)
{
  if (!(simparse_scn_keyvalliv))
    return;

  if ((simparse_scn_keyvalliv -> key))
    free(simparse_scn_keyvalliv -> key);

  if ((simparse_scn_keyvalliv -> val))
    free(simparse_scn_keyvalliv -> val);

  /* GJnew */
  free(simparse_scn_keyvalliv);

  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Allocate an array of pointers to simparse_scn_keyvalli structs with one element, which is NULL */
static simparse_scn_keyvalli **simparse_scn_keyvallilist_start(void)
{
  simparse_scn_keyvalli **simparse_scn_keyvallilist_start;

  if (!(simparse_scn_keyvallilist_start = (simparse_scn_keyvalli **) malloc(sizeof(simparse_scn_keyvalli *))))
    return NULL;

  simparse_scn_keyvallilist_start[0] = NULL;

  return simparse_scn_keyvallilist_start;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Read in a keyvalli list from string */
static simparse_scn_keyvalli **simparse_scn_keyvallilist_gfrst(char *string)
{
  char **readin = NULL;
  char blachar = '\0';
  char *blapoi;
  int line = 0;
  simparse_scn_keyvalli **simparse_scn_keyvallilist_gfrst = NULL;

  if ((string)) {
    if (!(readin = sparsenext("=", "", "", "", "#", "\n", -1, &string, &line, 0, 1)))
      goto error;
  }
  else {
    blapoi = &blachar;
    if (!(readin = sparsenext("=", "", "", "", "#", "\n", -1, &blapoi, &line, 0, 1)))
      goto error;
  }

  simparse_scn_keyvallilist_gfrst = simparse_scn_keyvallilist_gcomm(readin);

  if ((readin))
    freeparsed((readin));

  if (simparse_scn_keyvallilist_gfrst) {
    if ((simparse_scn_keyvallilist_gfrst[0])) {
      /* printf("There is a list\n"); */
    }
    else {
      /* printf("There is no list\n"); */
    }
  }

  return simparse_scn_keyvallilist_gfrst;

 error:
  if ((readin))
    freeparsed((readin));
  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Read in a keyvalli list from file */
simparse_scn_keyvalli **simparse_scn_keyvallilist_gfrfi(char *string)
{
  FILE *stream = NULL;
  int line = 0, i = 1;
  char **readin = NULL;
  simparse_scn_keyvalli **simparse_scn_keyvallilist_gfrfi = NULL;

  /* Apparently some OS require some time to get a file opened properly, so we are going into a loop */
  while (++i) {
	 if (!(stream = fopen(string, "r"))) {
		goto error;
	 }

	 if (getc(stream) != EOF) {
		break;
	 }
	 if (i > 10000000) {
		fprintf(stderr,"%s seems to be an empty file. Aborting read\n", string);
		goto error;
	 }
	 fclose(stream);
  }

  /* This should not be necessary */
  rewind(stream);

  /* This is all we need to do */
  if (!(readin = parsenext("=", "", "", "", "#", "\n", -1, stream, &line, 0))) {
    goto error;
  }

  simparse_scn_keyvallilist_gfrfi = simparse_scn_keyvallilist_gcomm(readin);

  if ((readin))
    freeparsed((readin));

  /* Clean up */
  fclose(stream);
      
  return simparse_scn_keyvallilist_gfrfi;

 error:
  if ((stream))
    fclose(stream);
  if ((readin))
    freeparsed((readin));

  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Read in a keyvalli list from string */
static simparse_scn_keyvalli **simparse_scn_keyvallilist_gcomm(char **readin)
{
  simparse_scn_keyvalli **simparse_scn_keyvallilist_gcomm = NULL;
  int citeline;
  int line;
  char *tempkey = NULL;
  char *tempkeynext = NULL;
  char *tempval = NULL;
  char **hacked = NULL;

  int i, j, k, l;

  /* Mind you, the function only returns NULL if there is some allocation error */

  /* OK, let's start with the minimal thing */
  if (!(simparse_scn_keyvallilist_gcomm = simparse_scn_keyvallilist_start()))
    goto error;

  /* If string is NULL, we return the started file */
  /* if (!(string)) { */
  /*   return simparse_scn_keyvallilist_gfrs; */
  /* } */

  /* Then we decompose the input */
  /* if (!(readin = sparsenext("=", "", "", "", "#", -1, &string, &line, 0, 1))) { */
  /*   printf("NULL passed, NULL returned, error invoked...\n"); */
  /*   goto error; */
  /* } */

  /* Then we identify the list of keys */
  i = 0;
  line = 1;
  citeline = 1;

  /* We did that already, but just to be sure */
  tempkey = NULL;

  /* if (!(readin[i])) { */
  /*   printf("yup\n"); */
  /* } */

  while ((readin[i])) {

/* printf("The line to be hacked: |%s|\n", readin[i]); */

    hacked = sparsenext(" \n\t", "", "", "", "", "", -1, &(readin[i]), &line, 1, 1);

    j = 0;
    k = 0;

    while((hacked[j])) {
      /* printf("hack of %i: |%s|\n", i, hacked[j]); */
      k = j;
      ++j;
    }

    if ((hacked[k])) {
      tempkeynext = simparse_copystring(hacked[k]);
      /* printf("got here %s %s\n", tempkeynext, hacked[k]); */
    }
    else {
      tempkeynext = NULL;
      /* printf("hack of %i: no item found\n", i);  */
    }

    /* Now we put a stop to the end of the real line */

    if ((tempkeynext)) {
      k = strlen(readin[i]);
      l = strlen(tempkeynext);
      /* printf("length of %i-th string: %i\n", i, k); */

      /* At the very end we do not expect a keyword, so we do not perform this step */
      if (readin[i+1]) {
	/* This should not fail, if it fails it's a programming error */
	/* original line: */
	/* while ((k+1)) */
	while (1) {
	  if (!strncmp(tempkeynext,readin[i]+k,l)) {
	    readin[i][k] = '\0';
	    break;
	  }
	  --k;
	}
      }
      /* Now read it in and hook all the stuff up */
      if (!(tempval = simparse_copystring(readin[i])))
	goto error;

    }
    else {
      tempval = simparse_copystring("");
    }

    if (!(readin[i+1])) {
      if ((tempval))
	free(tempval);
      if (!(tempval = simparse_copystring(readin[i])))
	goto error;
    }

    if ((tempkey)) {
      /* printf("########################\nkey: |%s| val: |%s| line: |%i|\n", tempkey, tempval, citeline); */

      /* Append to list */
      if (!(simparse_scn_keyvallilist_gcomm = simparse_scn_keyvalli_insert(simparse_scn_keyvallilist_gcomm, tempkey, tempval, citeline)))
	goto error;

      /* GJnew */
      free(tempkey);
    }
    tempkey = tempkeynext;
    tempkeynext = NULL;
    citeline = line;
    /* printf("\n"); */
    freeparsed(hacked);

    /* GJnew */
    free(tempval);
    ++i;
  }
  if ((tempkey))
    free(tempkey);
  return simparse_scn_keyvallilist_gcomm;

 error:
  if ((readin))
    freeparsed(readin);
  if ((hacked))
    freeparsed(hacked);
  if ((tempkey))
    free(tempkey);
  if ((tempkeynext))
    free(tempkeynext);
  if ((tempval))
    free(tempval);
  if (simparse_scn_keyvallilist_gcomm)
    simparse_scn_keyvallilist_dest(simparse_scn_keyvallilist_gcomm);

  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Allocate simparse_scn_keyvalli struct, insert an (allocated) copy of key and and allocated copy of value, allocate new list to insert line, return pointer to new list */
static simparse_scn_keyvalli **simparse_scn_keyvalli_insert(simparse_scn_keyvalli **simparse_scn_keyvallilist, char *key, char *val, int line)
{
  simparse_scn_keyvalli **simparse_scn_keyvallilist_insert = NULL;
  simparse_scn_keyvalli *simparse_scn_keyvalli_toinsert = NULL;
  char *keynew = NULL;
  char *valnew = NULL;
  int i,j;

  /* printf("trying to insert: key: |%s| val: |%s| line: |%i|\n", key, val, line); */

  /* Copy the strings */
  if ((key)) {
    if (!(keynew = (char *) malloc((strlen(key)+1)*sizeof(char))))
      goto error;
    strcpy(keynew, key);
  }

  if ((val)) {
    if (!(valnew = (char *) malloc((strlen(val)+1)*sizeof(char))))
      goto error;
    strcpy(valnew, val);
  }

  /* Create an empty struct and link key and value, set line */
  if (!(simparse_scn_keyvalli_toinsert = simparse_scn_keyvalli_create()))
    goto error;

  simparse_scn_keyvalli_toinsert -> key = keynew;
  simparse_scn_keyvalli_toinsert -> val = valnew;
  simparse_scn_keyvalli_toinsert -> line = line;

  /* printf("trying to insert: key: |%s| val: |%s| line: |%i|\n", simparse_scn_keyvalli_toinsert -> key, simparse_scn_keyvalli_toinsert -> val, simparse_scn_keyvalli_toinsert -> line); */

  /* If not an empty list has been passed, create a list with one element more */
  if ((simparse_scn_keyvallilist)) {

    /* Now make a copy with exactly one element more */
    i = 0;
    while (simparse_scn_keyvallilist[i])
      ++i;

    i = i+2;

    /* allocate an array with one more element */
    if (!(simparse_scn_keyvallilist_insert = (simparse_scn_keyvalli **) malloc(i*sizeof(simparse_scn_keyvalli *))))
      goto error;

    /* Copy all elements */
    for (j=0; j<(i-2); ++j) {
      simparse_scn_keyvallilist_insert[j] = simparse_scn_keyvallilist[j];
    }
    
  }
  else {
    /* Allocate list with exactly two elements */
    if (!(simparse_scn_keyvallilist_insert = (simparse_scn_keyvalli **) malloc(2*sizeof(simparse_scn_keyvalli *))))
      goto error;

    j = 0;
  }

  /* Insert the new one */
  simparse_scn_keyvallilist_insert[j] = simparse_scn_keyvalli_toinsert;
  simparse_scn_keyvallilist_insert[j+1] = NULL;

  /* Now it is safe to deallocate the input list (mind you, the elements stay) */
  free(simparse_scn_keyvallilist);

  return simparse_scn_keyvallilist_insert;

 error:
  if ((keynew))
    free(keynew);
  if ((valnew))
    free(valnew);

  /* Notice: the elements of this struct have been de-allocated already (keynew and valuenew) */
  if ((simparse_scn_keyvalli_toinsert))
    free(simparse_scn_keyvalli_toinsert);
  if ((simparse_scn_keyvallilist_insert))
    free(simparse_scn_keyvallilist_insert);
  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Deallocate complete simparse_scn_keyvallilist array, including strings */
void simparse_scn_keyvallilist_dest(simparse_scn_keyvalli **simparse_scn_keyvallilist)
{
  int i = 0;

  if (!(simparse_scn_keyvallilist))
    return;

  while ((simparse_scn_keyvallilist[i])) {
    simparse_scn_keyvalli_dest(simparse_scn_keyvallilist[i]);
    ++i;
  }

  free(simparse_scn_keyvallilist);
  
  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Allocate simparse_scn_arel struct, setting internal pointers to NULL, return pointer */
static simparse_scn_arel *simparse_scn_arel_create(void)
{
  simparse_scn_arel *simparse_scn_arel_create = NULL;

  if (!(simparse_scn_arel_create = (simparse_scn_arel *) malloc(sizeof(simparse_scn_arel))))
    return NULL;

  simparse_scn_arel_create -> preamblename = NULL;
  simparse_scn_arel_create -> preamble = NULL;
  simparse_scn_arel_create -> preamble_is_updated = 0;
  simparse_scn_arel_create -> keyvallipre = NULL;
  simparse_scn_arel_create -> orifilename = NULL;
  simparse_scn_arel_create -> keyvallifile = NULL;
  simparse_scn_arel_create -> onerror_prompt = 0;

  return simparse_scn_arel_create;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Deallocate simparse_scn_arel struct, the pointer is pointing to, including strings therein */
static void simparse_scn_arel_dest(simparse_scn_arel *simparse_scn_arelv)
{
  if (!(simparse_scn_arelv))
    return;

  if ((simparse_scn_arelv -> preamblename))
    free(simparse_scn_arelv -> preamblename);

  if ((simparse_scn_arelv -> preamble))
    free(simparse_scn_arelv -> preamble);

  simparse_scn_keyvallilist_dest(simparse_scn_arelv -> keyvallipre);

  if ((simparse_scn_arelv -> orifilename))
    free(simparse_scn_arelv -> orifilename);

  simparse_scn_keyvallilist_dest(simparse_scn_arelv -> keyvallifile);

  /* GJnew */
  free(simparse_scn_arelv);

  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Allocate an array of pointers to simparse_scn_arel structs with one element, which is NULL */
simparse_scn_arel **simparse_scn_arellist_start(void)
{
  simparse_scn_arel **simparse_scn_arellist_start;

  if (!(simparse_scn_arellist_start = (simparse_scn_arel **) malloc(sizeof(simparse_scn_arel *))))
    return NULL;

  simparse_scn_arellist_start[0] = NULL;

  return simparse_scn_arellist_start;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Deallocate complete simparse_scn_arellist array, including strings */
void simparse_scn_arellist_dest(simparse_scn_arel **simparse_scn_arellist)
{
  int i = 0;

  if (!simparse_scn_arellist)
    return;

  while ((simparse_scn_arellist[i])) {
    simparse_scn_arel_dest(simparse_scn_arellist[i]);
    ++i;
  }

  free(simparse_scn_arellist);
  
  return;
}

/* ------------------------------------------------------------ */




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Reset the file time stamp to a date sometimes 1901 (so earlier than any possible creation date) */
int simparse_scn_arel_timestamp_early(simparse_scn_arel *scnarelv)
{
  struct tm tmstr;

  if (scnarelv == NULL)
    return 1;

  tmstr.tm_sec   = 00;
  tmstr.tm_min   = 00;
  tmstr.tm_hour  = 00;
  tmstr.tm_mday  = 15;
  tmstr.tm_mon   = 11;
  tmstr.tm_year  = 1;
  tmstr.tm_isdst = -1;

  scnarelv -> timestamp = mktime(&tmstr);

  return 0;
}

/* ------------------------------------------------------------ */




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Reset the file time stamp to a date sometimes 1901 (so earlier than any possible creation date) */
int simparse_scn_arel_timestamp_late(simparse_scn_arel *scnarelv)
{
  struct tm tmstr;

  if (scnarelv == NULL)
    return 1;

  tmstr.tm_sec   = 00;
  tmstr.tm_min   = 00;
  tmstr.tm_hour  = 00;
  tmstr.tm_mday  = 15;
  tmstr.tm_mon   = 11;
  tmstr.tm_year  = 301;
  tmstr.tm_isdst = -1;

  scnarelv -> timestamp = mktime(&tmstr);

  return 0;
}

/* ------------------------------------------------------------ */




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Update one simparse_scn_arel struct (including the keyword-value list), depending on switches */
int simparse_scn_arel_upd(simparse_scn_arel *simparse_scn_arelv)
{
  struct stat fileinfo;
  simparse_scn_keyvalli **keyvallipre = NULL;
  simparse_scn_keyvalli **keyvallifile = NULL;
  time_t newtimestamp;
  int action_pre = 0;
  int action_file = 0;

  /* int jj; */

  /* Safety check */
  if (!(simparse_scn_arelv))
    return 1;

  /* First the preamble */
  if ((simparse_scn_arelv -> preamble_is_updated)) {
    if (!(keyvallipre = simparse_scn_keyvallilist_gfrst(simparse_scn_arelv -> preamble)))
      goto error;
    action_pre = 1;
  }

  /* if (keyvallipre) { */
  /*   printf("created something\n"); */
  /*   printf("*************\n"); */
  /*   jj = 0; */
  /*   while (keyvallipre[jj]) { */
  /*     printf("key: |%s| val: |%s| line: |%i|\n", keyvallipre[jj] -> key, keyvallipre[jj] -> val, keyvallipre[jj] -> line); */
  /*     ++jj; */
  /*   } */
  /*   printf("*************\n"); */
  /* } */
  /* else { */
  /*   printf("nothing created\n"); */
  /* } */

  newtimestamp = simparse_scn_arelv -> timestamp;

  /* Then the file, get file name and time stamp first */
  if (simparse_scn_arelv -> orifilename) {
    
    /* Check existence */
    if (!(stat(simparse_scn_arelv -> orifilename, &fileinfo))) {

      /* Check time stamp, if updated, update everything */
      if (simparse_scn_arelv -> timestamp < fileinfo.st_mtime) {

		  if (!(keyvallifile = simparse_scn_keyvallilist_gfrfi(simparse_scn_arelv -> orifilename)))
			 return 1;

		  /* Update time stamp, but only at the end */
		  newtimestamp = fileinfo.st_mtime;
		  action_file = 1;
      }
    }
    else {
      if (!(keyvallifile = simparse_scn_keyvallilist_start()))
	goto error;
      action_file = 1;
    }
  }
  else {
    if (!(keyvallifile = simparse_scn_keyvallilist_start()))
      goto error;
    action_file = 1;
  }

  /* Do replacing and linking */
  if ((action_pre)) {
    simparse_scn_keyvallilist_dest(simparse_scn_arelv -> keyvallipre);
    simparse_scn_arelv -> keyvallipre = keyvallipre;
    /* Update time stamps */
    simparse_scn_arelv -> preamble_is_updated = 0;
  }
  if ((action_file)) {
    simparse_scn_keyvallilist_dest(simparse_scn_arelv -> keyvallifile);
    simparse_scn_arelv -> keyvallifile = keyvallifile;
    /* Update time stamps */
    simparse_scn_arelv -> timestamp = newtimestamp;
  }
  
  return 0;

 error:
  simparse_scn_keyvallilist_dest(keyvallipre);
  simparse_scn_keyvallilist_dest(keyvallifile);
  return 1;
}

/* ------------------------------------------------------------ */




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Read in a simparse_scn_arel struct (including the keyword-value list) */
simparse_scn_arel **simparse_scn_arel_insert(simparse_scn_arel **simparse_scn_arellist, char *preamblename, char *preamble, char *filename)
{  
  simparse_scn_arel **simparse_scn_arellist_insert = NULL;
  simparse_scn_arel *simparse_scn_arel_toinsert = NULL;
  char *preamblenamenew = NULL;
  char *preamblenew = NULL;
  char *filenamenew = NULL;
  int i,j;

  /* Copy the strings */
  if ((preamblename)) {
    if (!(preamblenamenew = (char *) malloc((strlen(preamblename)+1)*sizeof(char))))
      goto error;
    strcpy(preamblenamenew, preamblename);
  }

  /* Copy the strings */
  if ((preamble)) {
    if (!(preamblenew = (char *) malloc((strlen(preamble)+1)*sizeof(char))))
      goto error;
    strcpy(preamblenew, preamble);
  }

  /* Copy the strings */
  if ((filename)) {
    if (!(filenamenew = (char *) malloc((strlen(filename)+1)*sizeof(char))))
      goto error;
    strcpy(filenamenew, filename);
  }

  /* Create an empty struct and link key and value, set line */
  if (!(simparse_scn_arel_toinsert = simparse_scn_arel_create()))
    goto error;

  simparse_scn_arel_toinsert -> preamblename = preamblenamenew;
  simparse_scn_arel_toinsert -> preamble = preamblenew;
  simparse_scn_arel_toinsert -> orifilename = filenamenew;

  /* Set time stamps */
  simparse_scn_arel_timestamp_early(simparse_scn_arel_toinsert);
  simparse_scn_arel_toinsert -> preamble_is_updated = 1;

  /* See if content ok */
  simparse_scn_arel_upd(simparse_scn_arel_toinsert);

  /* continue here */

  /* If not an empty list has been passed, create a list with one element more */
  if ((simparse_scn_arellist)) {

    /* Now make a copy with exactly one element more */
    i = 0;
    while (simparse_scn_arellist[i])
      ++i;

    i=i+2;

    /* allocate an array with one more element */
    if (!(simparse_scn_arellist_insert = (simparse_scn_arel **) malloc(i*sizeof(simparse_scn_arel *))))
      goto error;

    /* Copy all elements */
    for (j=0; j<(i-2); ++j) {
      simparse_scn_arellist_insert[j] = simparse_scn_arellist[j];
    }
    
  }
  else {
        /* Allocate list with exactly three elements (first one is empty dummy for manual input, second is this one, third is NULL) */
    if (!(simparse_scn_arellist_insert = (simparse_scn_arel **) malloc(3*sizeof(simparse_scn_arel *))))
      goto error;

    /* Terminate for safety */
    simparse_scn_arellist_insert[0] = simparse_scn_arellist_insert[1] = simparse_scn_arellist_insert[2] = NULL;

    if (!(simparse_scn_arellist_insert[0] = simparse_scn_arel_create()))
      goto error;

    /* Name the thingy in the first element 'prompt' */
    if (!(simparse_scn_arellist_insert[0] -> preamblename = simparse_copystring("prompt")))
      goto error; 

    /* Do the thing with the update and the time stamps */
    simparse_scn_arel_timestamp_early(simparse_scn_arellist_insert[0]);
    simparse_scn_arellist_insert[0] -> preamble_is_updated = 1;

    /* if (!(simparse_scn_arellist_insert[1] = simparse_scn_arel_create())) */
    /*   goto error; */

    j = 1;

  }

  /* Now insert the new stuff */
  simparse_scn_arellist_insert[j] = simparse_scn_arel_toinsert;
  simparse_scn_arellist_insert[j+1] = NULL;

  /* Now it is safe to deallocate the input list (mind you, the elements stay) */
  free(simparse_scn_arellist);

  return simparse_scn_arellist_insert;

 error:
  if ((preamblenamenew))
    free(preamblenamenew);
  if ((preamblenew))
    free(preamblenew);
  if ((filenamenew))
    free(filenamenew);

  /* Notice: the elements of this struct have been de-allocated already (keynew and valuenew) */
  if ((simparse_scn_arel_toinsert))
    free(simparse_scn_arel_toinsert);
  if ((simparse_scn_arellist_insert))
    free(simparse_scn_arellist_insert);
  return NULL;

}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Generic function to read a value from a key in simparse_scn_arellist */
int simparse_scn_arel_readval_gen(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int type, int ndef, void *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, void **returned)
{
  int foundkeypre, foundkeyfil;
  int iar, jpre = 0, jfil = 0, i, j, k;
  int lineintern, numberofelements;
  int errorval = 1;
  int nreadintern, nreturnedintern;
  long longout;
  double doubleout;
  int gfrst;
  char *errormes = NULL;
  char *valintern = NULL;
  char *promptmes = NULL;
  char *prinput = NULL;
  char *identintern = NULL;
  char *stringintern = NULL;
  char *stringreturn = NULL;
  char **stringlistintern = NULL;
  char **stringlistreturn = NULL;
  int *intreturn = NULL;
  long *longreturn = NULL;
  float *floatreturn = NULL;
  double *doublereturn = NULL;
  int nmaxcolon;


  /* First check if appropriate input */
  if (!(simparse_scn_arellist)) {
    errormes = simparse_copystring("Function simparse_scn_arel_readval_gen: entered NULL as simparse_scn_arellist\n");
    goto error;
  }

  if (!(simparse_scn_arellist[0])) {
    errormes = simparse_copystring("Function simparse_scn_arel_readval_gen: entered wrong array as simparse_scn_arellist\n.");
    goto error;
  }

  /* Make this an endless loop that we may leave */
  while (errorval) {

    /* Reset the error */
    errorval = 0;
    
    /* Find key */
    iar = 0;
    
    foundkeypre = foundkeyfil = 0;
    
    while ((simparse_scn_arellist[iar])) {     

      jpre = jfil = 0;

      /* First update */
      simparse_scn_arel_upd(simparse_scn_arellist[iar]);

      /* Scan through the lists */
      if ((simparse_scn_arellist[iar] -> keyvallipre)) {
	while (simparse_scn_arellist[iar] -> keyvallipre[jpre]) {
	  if (!comp2wordsnocase(simparse_scn_arellist[iar] -> keyvallipre[jpre] -> key, key)) {
	    foundkeypre = 1;
	    break;
	  }
	  ++jpre;
	} 
	if ((foundkeypre)) {
	  break;
	}
      }

      if ((simparse_scn_arellist[iar] -> keyvallifile)) {
	while (simparse_scn_arellist[iar] -> keyvallifile[jfil]) {
	  if (!comp2wordsnocase(simparse_scn_arellist[iar] -> keyvallifile[jfil] -> key, key)) {
	    foundkeyfil = 1;
	    break;
	  }
	  ++jfil;
	} 
	if ((foundkeyfil)) {
	  break;
	}
      }
      ++iar;      
   }

  /* Return the result of the search */
  *keypres = foundkeypre || foundkeyfil;
  
  /* Okidoki, we have found or haven't found the key */
  if (foundkeypre) {
	 if (!(identintern = simparse_copystring(simparse_scn_arellist[iar] -> preamblename)))
		goto error;
	 if (!(valintern = simparse_copystring(simparse_scn_arellist[iar] -> keyvallipre[jpre] -> val)))
		goto error;
	 lineintern = simparse_scn_arellist[iar] -> keyvallipre[jpre] -> line;
  }
  else if (foundkeyfil) {
	 if (!(identintern = simparse_copystring(simparse_scn_arellist[iar] -> orifilename)))
		goto error;
	 if (!(valintern = simparse_copystring(simparse_scn_arellist[iar] -> keyvallifile[jfil] -> val)))
		goto error;
	 lineintern = simparse_scn_arellist[iar] -> keyvallifile[jfil] -> line;
  }
  else {
	 
	 if (!(identintern = simparse_copystring("")))
		goto error;
	 lineintern = 0;
	 if (!(valintern = simparse_copystring("")))
		goto error;
	 if ((keyreq)) {
		errorval = SIMPARSE_SCN_ERROR_NO_KEY;
		if (!(errormes = simparse_copystring("Input required")))
		  goto error;
		goto exitfromhere;
	 }
  }
  

  /* Now we have the valintern, we clip it to stringintern, this works no matter what (except for memory problems) */
  i = strlen(valintern) - 1;
    j = 1;

    while (i >= 0) {
      if (!strchr(" \n\t", valintern[i])) {
	valintern[i+1] = '\0';
	j = 0;
	break;
      }
      --i;
    }

    if ((j))
      valintern[0] = '\0';

    i = 0;

    /* as long as the character is a whitespace, increase counter, unless this is SCN_TYPE_STRINGWHITESP */
    if (type != SCN_TYPE_STRINGWHITESP) {
      while (strchr(" \n\t", valintern[i]) && (valintern[i] != '\0'))
	++i;
    }

    if (!(stringintern = simparse_copystring(valintern+i)))
      goto error;

    /* If this is what the user wants (and there's no error) we'll jump to the finish */
    if ((type == SCN_TYPE_STRING) || (type == SCN_TYPE_STRINGWHITESP)) {

      numberofelements = strlen(stringintern);

      if ((nmax >= 0) && (numberofelements > nmax) && (enterlowernmax)) {
	errorval = SIMPARSE_SCN_ERROR_TOO_MANY_ELEMENTS;
	if (!(errormes = simparse_copystring("                             ")))
	  goto error;
	if (nmax < 10000)
	  sprintf(errormes, "Too many elements (max. %i)", nmax);
	else
	  sprintf(errormes, "Too many elements (max. 9999)");
	  goto exitfromhere;
      }
      if ((numberofelements < nmin) && ((!def) || (ndef <= 0))) {
	errorval = SIMPARSE_SCN_ERROR_NOT_ENOUGH_ELEMENTS;
	if (!(errormes = simparse_copystring("                              ")))
	  goto error;
	if (nmin < 10000)
	  sprintf(errormes, "Too few elements (min. %i)", nmin);
	else
	  sprintf(errormes, "Too few elements (min. 9999)");
	  goto exitfromhere;
      }
      
      /* Well, that's it, we read it in. First look how many elements will be passed */
      
      /* Possibility one, we don't have enough without def, then it's nmin */
      if (numberofelements < nmin) {
	nreadintern = numberofelements;
	nreturnedintern = nmin;
      }
      /* OK, we have enough, then is nmax switched on, such that we need to check? */
      else if (nmax >= 0) {
	if (numberofelements < nmax) {
	  nreadintern = numberofelements;
	  nreturnedintern = numberofelements;
	}
	else {
	  nreadintern = nmax;
	  nreturnedintern = nmax;	  
	}
      }
      /* nmax < 0 */
      else {
	nreadintern = numberofelements;
	nreturnedintern = numberofelements;
      }
      
      /* Now that this is clarified, make space for the output */
      if (!(stringreturn = (char *) malloc((nreturnedintern+1)*sizeof(char))))
	goto error;
      

      /* Copy string */
      i = 0;
      while (i < nreturnedintern) {
	if (i < nreadintern)
	  stringreturn[i] = stringintern[i];
	else {
	  if (i < ndef) {
	    stringreturn[i] = ((char *) def)[i];
	  }
	  else {
	    stringreturn[i] = ((char *) def)[ndef-1];
	  }
	}
	++i;
      }
      
      /* Terminate string */
      stringreturn[i] = '\0';

      /* Hook up string and fill variables */
      *returned = (void *) stringreturn;
      *nread = nreadintern;
      *nreturned = nreturnedintern;

      /* Thitsit, one of the very few occasions where a global jump seems to make sense. */
      goto exitfromhere;
    }

    /* Next part, split stringintern into single parts. i is used as dummy for line */
    if (!(stringlistintern = sparsenext(" \t\n", "", "", "", "", "", -1, &stringintern, &i, 1, 1)))
      goto error;

    /* If that is the required output, go on with this */
    if (type == SCN_TYPE_STRINGLIST) {
      
      /* There is certainly something more intelligent than this */
      i = 0;
      while(stringlistintern[i])
	++i;

      numberofelements = i;

      if ((nmax >= 0) && (numberofelements > nmax) && (enterlowernmax)) {
	errorval = SIMPARSE_SCN_ERROR_TOO_MANY_ELEMENTS;
	if (!(errormes = simparse_copystring("                             ")))
	  goto error;
	if (nmax < 10000)
	  sprintf(errormes, "Too many elements (max. %i)", nmax);
	else
	  sprintf(errormes, "Too many elements (max. 9999)");
	goto exitfromhere;
      }
      if ((numberofelements < nmin) && ((!def) || (ndef <= 0))) {
	errorval = SIMPARSE_SCN_ERROR_NOT_ENOUGH_ELEMENTS;
	if (!(errormes = simparse_copystring("                              ")))
	  goto error;
	if (nmin < 10000)
	  sprintf(errormes, "Too few elements (min. %i)", nmin);
	else
	  sprintf(errormes, "Too few elements (min. 9999)");
	goto exitfromhere;
      }
      
      /* Well, that's it, we read it in. First look how many elements will be passed */
      
      /* Possibility one, we don't have enough without def, then it's nmin */
      if (numberofelements < nmin) {
	nreadintern = numberofelements;
	nreturnedintern = nmin;
      }
      /* OK, we have enough, then is nmax switched on, such that we need to check? */
      else if (nmax >= 0) {
	if (numberofelements < nmax) {
	  nreadintern = numberofelements;
	  nreturnedintern = numberofelements;
	}
	else {
	  nreadintern = nmax;
	  nreturnedintern = nmax;	  
	}
      }
      /* nmax < 0 */
      else {
	nreadintern = numberofelements;
	nreturnedintern = numberofelements;
      }
      
      /* Now that this is clarified, make space for the output */
      if (!(stringlistreturn = (char **) malloc((nreturnedintern+1)*sizeof(char*))))
	goto error;

      /* NULL it */
      i = 0;
      for (i = 0; i <= nreturnedintern; ++i) {
	stringlistreturn[i] = NULL;
      } 

      /* Copy strings */
      i = 0;
      while (i < nreturnedintern) {
	if (i < nreadintern) {
	  if (!(stringlistreturn[i] = simparse_copystring(stringlistintern[i])))
	    goto error;
	}
	else {
	  if (i < ndef) {
	    if (!(stringlistreturn[i] = simparse_copystring(((char **) def)[i])))
	      goto error;
	  }
	  else {
	    if (!(stringlistreturn[i] = simparse_copystring(((char **) def)[ndef-1])))
	      goto error;
	  }
	}
	++i;
      }
      
      /* To terminate string list is not necessary */
      /* stringreturn[i] = NULL; */

      /* Hook up string and fill variables */
      *returned = (void *) stringlistreturn;
      *nread = nreadintern;
      *nreturned = nreturnedintern;

      /* Thitsit, one of the very few occasions where a global jump seems to make sense. */
      goto exitfromhere;
    }

    /* Still not there? Then we have to read out arrays of numbers */
    if (type == SCN_TYPE_INT) {
      
      /* There is certainly something more intelligent than this */
      i = 0;
      numberofelements = 0;
      while(stringlistintern[i]) {
	if ((gfrst = getlongfromstring(stringlistintern[i], 0, &longout, &j)) > 0) {
	  errorval = SIMPARSE_SCN_ERROR_TYPE;
	  if (!(errormes = simparse_copystring("Could not convert string")))
	    goto error;
	  goto exitfromhere;
	}

	/* Memory problems, terminate. */
	if (gfrst < 0) {
	  goto error;
	}
	numberofelements = numberofelements + (j+1);
	++i;
      }

      if ((nmax >= 0) && (numberofelements > nmax) && (enterlowernmax)) {
	errorval = SIMPARSE_SCN_ERROR_TOO_MANY_ELEMENTS;
	if (!(errormes = simparse_copystring("                             ")))
	  goto error;
	if (nmax < 10000)
	  sprintf(errormes, "Too many elements (max. %i)", nmax);
	else
	  sprintf(errormes, "Too many elements (max. 9999)");
	goto exitfromhere;
      }
      if ((numberofelements < nmin) && ((!def) || (ndef <= 0))) {
	errorval = SIMPARSE_SCN_ERROR_NOT_ENOUGH_ELEMENTS;
	if (!(errormes = simparse_copystring("                              ")))
	  goto error;
	if (nmin < 10000)
	  sprintf(errormes, "Too few elements (min. %i)", nmin);
	else
	  sprintf(errormes, "Too few elements (min. 9999)");
	goto exitfromhere;
      }

      /* Well, that's it, we read it in. First look how many elements will be passed */
      
      /* Possibility one, we don't have enough without def, then it's nmin */
      if (numberofelements < nmin) {
	nreadintern = numberofelements;
	nreturnedintern = nmin;
      }
      /* OK, we have enough, then is nmax switched on, such that we need to check? */
      else if (nmax >= 0) {
	if (numberofelements < nmax) {
	  nreadintern = numberofelements;
	  nreturnedintern = numberofelements;
	}
	else {
	  nreadintern = nmax;
	  nreturnedintern = nmax;	  
	}
      }
      /* nmax <= 0 */
      else {
	nreadintern = numberofelements;
	nreturnedintern = numberofelements;
      }

      /* Now that this is clarified, make space for the output */
      if ((nreturnedintern)) {
	if (!(intreturn = (int *) malloc((nreturnedintern)*sizeof(int))))
	  goto error;
      }
      else {
	intreturn = NULL;
      }

      /* Copy numbers */
      i = 0;
      j = 0;
      k = 0;
      nmaxcolon = 0;
      while (i < nreturnedintern) {
	if (i < nreadintern) {

	  /*  After having had a dry run above, this should cause no memory problems any more... */
	  if (getlongfromstring(stringlistintern[j], k, &longout, &nmaxcolon) < 0)
	    goto error;

	  intreturn[i] = (int) longout;
	  if (nmaxcolon == k) {
	    ++j;
	    k = 0;
	  }
	  else {
	    ++k;
	  }
	}
	else {
	  if (i < ndef) {
	    intreturn[i] = ((int *) def)[i];
	  }
	  else {
	    intreturn[i] = ((int *) def)[ndef];
	  }
	}
	++i;
      }
      
      /* To terminate string list is not necessary */
      /* stringreturn[i] = NULL; */

      /* Hook up string and fill variables */
      *returned = (void *) intreturn;
      *nread = nreadintern;
      *nreturned = nreturnedintern;

      /* Thitsit, one of the very few occasions where a global jump seems to make sense. */
      goto exitfromhere;
    }

    /* Still not there? Then we have to read out arrays of numbers */
    if (type == SCN_TYPE_LONG) {
      
      /* There is certainly something more intelligent than this */
      i = 0;
      numberofelements = 0;
      while(stringlistintern[i]) {
	if ((gfrst = getlongfromstring(stringlistintern[i], 0, &longout, &j)) > 0) {
	  errorval = SIMPARSE_SCN_ERROR_TYPE;
	  if (!(errormes = simparse_copystring("Could not convert string")))
	    goto error;
	  goto exitfromhere;
	}

	/* Memory problems, terminate. */
	if (gfrst < 0) {
	  goto error;
	}
	numberofelements = numberofelements + (j+1);
	++i;
      }

      if ((nmax >= 0) && (numberofelements > nmax) && (enterlowernmax)) {
	errorval = SIMPARSE_SCN_ERROR_TOO_MANY_ELEMENTS;
	if (!(errormes = simparse_copystring("                             ")))
	  goto error;
	if (nmax < 10000)
	  sprintf(errormes, "Too many elements (max. %i)", nmax);
	else
	  sprintf(errormes, "Too many elements (max. 9999)");
	goto exitfromhere;
      }
      if ((numberofelements < nmin) && ((!def) || (ndef <= 0))) {
	errorval = SIMPARSE_SCN_ERROR_NOT_ENOUGH_ELEMENTS;
	if (!(errormes = simparse_copystring("                              ")))
	  goto error;
	if (nmin < 10000)
	  sprintf(errormes, "Too few elements (min. %i)", nmin);
	else
	  sprintf(errormes, "Too few elements (min. 9999)");
	goto exitfromhere;
      }
      
      /* Well, that's it, we read it in. First look how many elements will be passed */
      
      /* Possibility one, we don't have enough without def, then it's nmin */
      if (numberofelements < nmin) {
	nreadintern = numberofelements;
	nreturnedintern = nmin;
      }
      /* OK, we have enough, then is nmax switched on, such that we need to check? */
      else if (nmax >= 0) {
	if (numberofelements < nmax) {
	  nreadintern = numberofelements;
	  nreturnedintern = numberofelements;
	}
	else {
	  nreadintern = nmax;
	  nreturnedintern = nmax;	  
	}
      }
      /* nmax < 0 */
      else {
	nreadintern = numberofelements;
	nreturnedintern = numberofelements;
      }
      
      /* Now that this is clarified, make space for the output */
      if ((nreturnedintern)) {
	if (!(longreturn = (long *) malloc((nreturnedintern)*sizeof(long))))
	  goto error;
      }
      else {
	longreturn = NULL;
      }

      /* Copy numbers */
      i = 0;
      j = 0;
      k = 0;
      nmaxcolon = 0;
      while (i < nreturnedintern) {
	if (i < nreadintern) {

	  /*  After having had a dry run above, this should cause no memory problems any more... */
	  if (getlongfromstring(stringlistintern[j], k, &longout, &nmaxcolon) < 0)
	    goto error;
	  longreturn[i] = longout;
	  if (nmaxcolon == k) {
	    ++j;
	    k = 0;
	  }
	  else {
	    ++k;
	  }
	}
	else {
	  if (i < ndef) {
	    longreturn[i] = ((long *) def)[i];
	  }
	  else {
	    longreturn[i] = ((long *) def)[ndef-1];
	  }
	}
	++i;
      }
      
      /* To terminate string list is not necessary */
      /* stringreturn[i] = NULL; */

      /* Hook up string and fill variables */
      *returned = (void *) longreturn;
      *nread = nreadintern;
      *nreturned = nreturnedintern;

      /* Thitsit, one of the very few occasions where a global jump seems to make sense. */
      goto exitfromhere;
    }

    /* Still not there? Then we have to read out arrays of numbers */
    if (type == SCN_TYPE_FLOAT) {
      
      /* There is certainly something more intelligent than this */
      i = 0;
      numberofelements = 0;
      while(stringlistintern[i]) {
	if ((gfrst = getdoublefromstring(stringlistintern[i], 0, &doubleout, &j)) > 0) {
	  errorval = SIMPARSE_SCN_ERROR_TYPE;
	  if (!(errormes = simparse_copystring("Could not convert string")))
	    goto error;
	  goto exitfromhere;
	}

	/* Memory problems, terminate. */
	if (gfrst < 0) {
	  goto error;
	}
	numberofelements = numberofelements + (j+1);
	++i;
      }

      if ((nmax >= 0) && (numberofelements > nmax) && (enterlowernmax)) {
	errorval = SIMPARSE_SCN_ERROR_TOO_MANY_ELEMENTS;
	if (!(errormes = simparse_copystring("                             ")))
	  goto error;
	if (nmax < 10000)
	  sprintf(errormes, "Too many elements (max. %i)", nmax);
	else
	  sprintf(errormes, "Too many elements (max. 9999)");
	goto exitfromhere;
      }
      if ((numberofelements < nmin) && ((!def) || (ndef <= 0))) {
	errorval = SIMPARSE_SCN_ERROR_NOT_ENOUGH_ELEMENTS;
	if (!(errormes = simparse_copystring("                              ")))
	  goto error;
	if (nmin < 10000)
	  sprintf(errormes, "Too few elements (min. %i)", nmin);
	else
	  sprintf(errormes, "Too few elements (min. 9999)");
	goto exitfromhere;
      }
      
      /* Well, that's it, we read it in. First look how many elements will be passed */
      
      /* Possibility one, we don't have enough without def, then it's nmin */
      if (numberofelements < nmin) {
	nreadintern = numberofelements;
	nreturnedintern = nmin;
      }
      /* OK, we have enough, then is nmax switched on, such that we need to check? */
      else if (nmax >= 0) {
	if (numberofelements < nmax) {
	  nreadintern = numberofelements;
	  nreturnedintern = numberofelements;
	}
	else {
	  nreadintern = nmax;
	  nreturnedintern = nmax;	  
	}
      }
      /* nmax < 0 */
      else {
	nreadintern = numberofelements;
	nreturnedintern = numberofelements;
      }
      
      /* Now that this is clarified, make space for the output */
      if (nreturnedintern) {
	if (!(floatreturn = (float *) malloc((nreturnedintern)*sizeof(float))))
	  goto error;
      }
      else {
	floatreturn = NULL;
      }

      /* Copy numbers */
      i = 0;
      j = 0;
      k = 0;
      nmaxcolon = 0;
      while (i < nreturnedintern) {
	if (i < nreadintern) {

	  /*  After having had a dry run above, this should cause no memory problems any more... */
	  if (getdoublefromstring(stringlistintern[j], k, &doubleout, &nmaxcolon) < 0)
	    goto error;
	  floatreturn[i] = (float) doubleout;
	  if (nmaxcolon == k) {
	    ++j;
	    k = 0;
	  }
	  else {
	    ++k;
	  }
	}
	else {
	  if (i < ndef) {
	    floatreturn[i] = ((float *) def)[i];
	  }
	  else {
	    floatreturn[i] = ((float *) def)[ndef-1];
	  }
	}
	++i;
      }
      
      /* To terminate string list is not necessary */
      /* stringreturn[i] = NULL; */

      /* Hook up string and fill variables */
      *returned = (void *) floatreturn;
      *nread = nreadintern;
      *nreturned = nreturnedintern;

      /* Thitsit, one of the very few occasions where a global jump seems to make sense. */
      goto exitfromhere;
    }

    /* Still not there? Then we have to read out arrays of numbers */
    if (type == SCN_TYPE_DOUBLE) {

      /* There is certainly something more intelligent than this */
      i = 0;
      numberofelements = 0;
      while(stringlistintern[i]) {
	if ((gfrst = getdoublefromstring(stringlistintern[i], 0, &doubleout, &j)) > 0) {
	  errorval = SIMPARSE_SCN_ERROR_TYPE;
	  if (!(errormes = simparse_copystring("Could not convert string")))
	    goto error;
	  goto exitfromhere;
	}

	/* Memory problems, terminate. */
	if (gfrst < 0) {
	  goto error;
	}
	numberofelements = numberofelements + (j+1);
	++i;
      }

      if ((nmax >= 0) && (numberofelements > nmax) && (enterlowernmax)) {
	errorval = SIMPARSE_SCN_ERROR_TOO_MANY_ELEMENTS;
	if (!(errormes = simparse_copystring("                             ")))
	  goto error;
	if (nmax < 10000)
	  sprintf(errormes, "Too many elements (max. %i)", nmax);
	else
	  sprintf(errormes, "Too many elements (max. 9999)");
	goto exitfromhere;
      }
      if ((numberofelements < nmin) && ((!def) || (ndef <= 0))) {
	errorval = SIMPARSE_SCN_ERROR_NOT_ENOUGH_ELEMENTS;
	if (!(errormes = simparse_copystring("                              ")))
	  goto error;
	if (nmin < 10000)
	  sprintf(errormes, "Too few elements (min. %i)", nmin);
	else
	  sprintf(errormes, "Too few elements (min. 9999)");
	goto exitfromhere;
      }
      
      /* Well, that's it, we read it in. First look how many elements will be passed */
      
      /* Possibility one, we don't have enough without def, then it's nmin */
      if (numberofelements < nmin) {
	nreadintern = numberofelements;
	nreturnedintern = nmin;
      }
      /* OK, we have enough, then is nmax switched on, such that we need to check? */
      else if (nmax >= 0) {
	if (numberofelements < nmax) {
	  nreadintern = numberofelements;
	  nreturnedintern = numberofelements;
	}
	else {
	  nreadintern = nmax;
	  nreturnedintern = nmax;	  
	}
      }
      /* nmax <= 0 */
      else {
	nreadintern = numberofelements;
	nreturnedintern = numberofelements;
      }
      
      /* Now that this is clarified, make space for the output */
      if ((nreturnedintern)) {
	if (!(doublereturn = (double *) malloc((nreturnedintern)*sizeof(double))))
	  goto error;
      }
      else {
	doublereturn = NULL;
      }

      /* Copy numbers */
      i = 0;
      j = 0;
      k = 0;
      nmaxcolon = 0;
      while (i < nreturnedintern) {
	if (i < nreadintern) {

	  /*  After having had a dry run above, this should cause no memory problems any more... */
	  if (getdoublefromstring(stringlistintern[j], k, &doubleout, &nmaxcolon) < 0)
	    goto error;
	  doublereturn[i] = doubleout;
	  if (nmaxcolon == k) {
	    ++j;
	    k = 0;
	  }
	  else {
	    ++k;
	  }
	}
	else {
	  if (i < ndef) {
	    doublereturn[i] = ((double *) def)[i];
	  }
	  else {
	    doublereturn[i] = ((double *) def)[ndef-1];
	  }
	}
	++i;
      }
      
      /* To terminate string list is not necessary */
      /* stringreturn[i] = NULL; */

      /* Hook up string and fill variables */
      *returned = (void *) doublereturn;
      *nread = nreadintern;
      *nreturned = nreturnedintern;

      /* Thitsit, one of the very few occasions where a global jump seems to make sense. */
      goto exitfromhere;
    }

    /* Now we've done everything. If no appropriate type has been requested, this is a programming error */
    goto error;

    exitfromhere: 
    /* If there was an error, we might be inclined to correct it */
    if ((errorval)) {
      if (lineintern) {
	if (key[strlen(key)-1] == '=')
	  fprintf(stderr,"%s, line: %i: %s %s\n", identintern, lineintern, key, stringintern);
	else
	  fprintf(stderr,"%s, line: %i: %s = %s\n", identintern, lineintern, key, stringintern);
      }
      if (!(strncmp(identintern, simparse_scn_arellist[0] -> preamblename, strlen(simparse_scn_arellist[0] -> preamblename)))) {
	if (key[strlen(key)-1] == '=')
	  fprintf(stderr,"%s, line: %i: %s %s\n", identintern, lineintern, key, stringintern);
	else 
	  fprintf(stderr,"%s, line: %i: %s = %s\n", identintern, lineintern, key, stringintern);
      }
      if (errorval == SIMPARSE_SCN_ERROR_NO_KEY) {
	if (key[strlen(key)-1] == '=')
	  fprintf(stderr,"%s\n", key);
	else 
	  fprintf(stderr,"%s =\n", key);
      }

      fprintf(stderr,"%s\n",errormes);
      if (simparse_scn_arellist[0] -> onerror_prompt) {
	
	/* Now prompt the user to correct her/his ways */
	
	/* First tell him what should be entered */
	if (errmes) {
	  fprintf(stderr, "Keyword %s %s\n", key, errmes);
	}

	/* Allocate memory for the key */
	if (!(promptmes = (char *) malloc((strlen(key)+6)*sizeof(char)))) {
	  goto error;
	  errorval = errorval | SIMPARSE_SCN_ERROR_MEMORY;
	}
	
	/* Fill it */
	if (key[strlen(key)-1] == '=')
	  sprintf(promptmes,"> %s ", key);
	else
	  sprintf(promptmes,"> %s = ", key);

        rl_bind_key('\t', rl_complete);
	
        if (!(prinput = readline(promptmes)))
	  goto error;
	
	/* This implies that we safe something somewhere, but that's just out of control, hoping that readline has an appropriate garbage control */
        add_history(prinput);
	
	/* Remove the key from the array */
	if (simparse_scn_arellist[0] -> keyvallipre) {
	  i = 0;
	  while (simparse_scn_arellist[0] -> keyvallipre[i]) {
	    if (!(strncmp(simparse_scn_arellist[0] -> keyvallipre[i] -> key, key, strlen(key))))
	      /* This does not delete the keyvalli struct from the list, but kills the keyword */
	      simparse_scn_arellist[0] -> keyvallipre[i] -> key[0] = '\0';
	    ++i;
	  }
	}
	
	/* Now we have to add the key to the array */
	simparse_scn_arellist[0] -> keyvallipre = simparse_scn_keyvalli_insert(simparse_scn_arellist[0] -> keyvallipre, key, prinput, 0);
	
	/* Done, let's see if the user made a better job now */
	
	if (prinput) {
	  free(prinput);
	  prinput = NULL;
	}
	free(promptmes);
	promptmes = NULL;
      }
    }
    
    /* Deallocate stuff */
    /* if (intreturn)
       free(intreturn); */

    if ((valintern))
      free(valintern);
    valintern = NULL;
    
    if ((stringintern))
      free(stringintern);
    stringintern = NULL;
    
    if ((stringlistintern))
      freeparsed(stringlistintern);
    stringlistintern = NULL;
    
    if ((identintern))
      free(identintern);
    identintern = NULL;
    
    if ((errormes))
      free(errormes);
    errormes = NULL;
    
    if ((errorval)) {
      /* If no prompting was requested, we stop, but after deallocating (is not necessary, but cleaner) */
      if (!(simparse_scn_arellist[0] -> onerror_prompt)) {
		  exit(EXIT_FAILURE);
      }
    }
  }

    return 0;

 error:
  /* This is a programming error, so we give some details to the programmer */
    if ((errorval = SIMPARSE_SCN_ERROR_NULL_PASSED) || (errorval = SIMPARSE_SCN_ERROR_WRONG_ARRAY))
      fprintf(stderr,"%s",errormes);
  
  if ((errormes))
    free(errormes);
  if ((valintern))
    free(valintern);
  if ((identintern))
    free(identintern);
  if ((promptmes))
    free(promptmes);
  if ((prinput))
    free(prinput);
  if ((stringintern))
    free(stringintern);
  if ((stringreturn))
    free(stringreturn);
  if ((stringlistintern))
    freeparsed(stringlistintern);
  if ((stringlistreturn))
    freeparsed(stringlistreturn);
  if (intreturn)
    free(intreturn);
  if (longreturn)
    free(longreturn);
   if (floatreturn)
    free(floatreturn);
  if (doublereturn)
    free(doublereturn);

  return errorval;
}
 
/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Switch onerror_prompt on (1) or off (0) for the (NULL-terminated) simparse_scn_arellistv */
int simparse_scn_arel_onerror_prompt(simparse_scn_arel **simparse_scn_arellist, int on)
{
  int i = 0;

  if (on != 0)
    on = 1;

  if (!(simparse_scn_arellist))
    return 1;

  while ((simparse_scn_arellist[i])) {
    simparse_scn_arellist[i] -> onerror_prompt = on;
    ++i;
  }

  return 0;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Search for key in the simparse_scn_arel reserved for user input (first element) and remove (blank) each entry with the key key */
int simparse_scn_arel_cancelkey(simparse_scn_arel **simparse_scn_arellist, char *key) 
{
  int i;

  if (!(simparse_scn_arellist))
    return 1;

  if (!(simparse_scn_arellist[0]))
    return 0;

  if (!(simparse_scn_arellist[0] -> keyvallipre))
    return 0;

  i = 0;
  while ((simparse_scn_arellist[0] -> keyvallipre[i])) {
    if (!comp2wordsnocase(simparse_scn_arellist[0] -> keyvallipre[i] -> key, key)) {

      /* simple trick */
      simparse_scn_arellist[0] -> keyvallipre[i] -> key[0] = '\0';
    }
    ++i;
  }

  return 0;
}

/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Search for key in the simparse_scn_arel and remove (blank) each entry with the key key */
int simparse_scn_arel_deepcancelkey(simparse_scn_arel **simparse_scn_arellist, char *key, int depth) 
{
  int j,i;

  if (!(simparse_scn_arellist))
    return 1;

  if (!(simparse_scn_arellist[0]))
    return 0;

  j = 0;
  while ((simparse_scn_arellist[j]) && j <= depth) {
    i = 0;
    if ((simparse_scn_arellist[j] -> keyvallipre)) {
      while ((simparse_scn_arellist[j] -> keyvallipre[i])) {
	if (!comp2wordsnocase(simparse_scn_arellist[j] -> keyvallipre[i] -> key, key)) {
	  /* simple trick */
	  simparse_scn_arellist[j] -> keyvallipre[i] -> key[0] = '\0';
	}
	++i;
      }
    }
    i = 0;

    if ((simparse_scn_arellist[j] -> keyvallifile)) {
      while ((simparse_scn_arellist[j] -> keyvallifile[i])) {
	if (!comp2wordsnocase(simparse_scn_arellist[j] -> keyvallifile[i] -> key, key)) {
	
	  /* simple trick */
	  simparse_scn_arellist[j] -> keyvallifile[i] -> key[0] = '\0';
	}
	++i;
      }
    }
    ++j;
  }

  return 0;
}

/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Remove the complete key-value-line list in the member of simparse_scn_arellist assigned to user dialogue input (the first one) */
int simparse_scn_arel_cancelall(simparse_scn_arel **simparse_scn_arellist) 
{
  simparse_scn_keyvalli **keyvallipre;

  /* Not passing a list is an error */
  if (!(simparse_scn_arellist))
    return 1;

  /* Passing a list which is immediately terminated is, strictly speaking, no error */
  if (!(simparse_scn_arellist[0]))
    return 0;

  if (!(keyvallipre = simparse_scn_keyvallilist_start()))
    return 1;

  /* If there is no keyvalli list, just put the default one there */
  if (!(simparse_scn_arellist[0] -> keyvallipre)) {
    simparse_scn_arellist[0] -> keyvallipre = keyvallipre;
    return 0;
  }

  /* If there is a keyvalli list, destroy it and replace it */
  simparse_scn_keyvallilist_dest(simparse_scn_arellist[0] -> keyvallipre);
  simparse_scn_arellist[0] -> keyvallipre = keyvallipre;
  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Function to read a value list from a key in simparse_scn_arellist */
int simparse_scn_arel_readval_string(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, char *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, char **returned)
{
  return simparse_scn_arel_readval_gen(simparse_scn_arellist, key, errmes, SCN_TYPE_STRING, ndef, def, nmin, nmax, keyreq, enterlowernmax, keypres, nread, nreturned, (void **) returned);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Function to read a value list from a key in simparse_scn_arellist */
int simparse_scn_arel_readval_stringwhitesp(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, char *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, char **returned)
{
  return simparse_scn_arel_readval_gen(simparse_scn_arellist, key, errmes, SCN_TYPE_STRINGWHITESP, ndef, def, nmin, nmax, keyreq, enterlowernmax, keypres, nread, nreturned, (void **) returned);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Function to read a value list from a key in simparse_scn_arellist */
int simparse_scn_arel_readval_stringlist(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, char **def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, char ***returned){
  return simparse_scn_arel_readval_gen(simparse_scn_arellist, key, errmes, SCN_TYPE_STRINGLIST, ndef, def, nmin, nmax, keyreq, enterlowernmax, keypres, nread, nreturned, (void **) returned);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Function to read a value list from a key in simparse_scn_arellist */
int simparse_scn_arel_readval_int(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, int *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, int **returned)
{
  return simparse_scn_arel_readval_gen(simparse_scn_arellist, key, errmes, SCN_TYPE_INT, ndef, def, nmin, nmax, keyreq, enterlowernmax, keypres, nread, nreturned, (void **) returned);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Function to read a value list from a key in simparse_scn_arellist */
int simparse_scn_arel_readval_long(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, long *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, long **returned)
{
  return simparse_scn_arel_readval_gen(simparse_scn_arellist, key, errmes, SCN_TYPE_LONG, ndef, def, nmin, nmax, keyreq, enterlowernmax, keypres, nread, nreturned, (void **) returned);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Function to read a value list from a key in simparse_scn_arellist */
int simparse_scn_arel_readval_float(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, float *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, float **returned)
{
  return simparse_scn_arel_readval_gen(simparse_scn_arellist, key, errmes, SCN_TYPE_FLOAT, ndef, def, nmin, nmax, keyreq, enterlowernmax, keypres, nread, nreturned, (void **) returned);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Function to read a value list from a key in simparse_scn_arellist */
int simparse_scn_arel_readval_double(simparse_scn_arel **simparse_scn_arellist, char *key, char *errmes, int ndef, double *def, int nmin, int nmax, int keyreq, int enterlowernmax, int *keypres, int *nread, int *nreturned, double **returned)
{
  return simparse_scn_arel_readval_gen(simparse_scn_arellist, key, errmes, SCN_TYPE_DOUBLE, ndef, def, nmin, nmax, keyreq, enterlowernmax, keypres, nread, nreturned, (void **) returned);
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a number if possible from a string */
static int getlongfromstring(char *input, int n, long *output, int *nmax) 
{
  int line = 1; /* dummy */
  char **thinglist = NULL;
  char *pointy;
  int getlongfromstring;
  int i;
  double preoutput[3];
  long theoutput[3];
  int dfail = 0;
  int lfail = 0;

  if (!(thinglist = sparsenext(":", "", "", "", "", "", -1, &input, &line, 1, 1)))
    return -1;

  if (plen(thinglist) == 1) {
    theoutput[0] = strtol(thinglist[0], &pointy, 10);
    if ((*pointy)) {
      lfail = 1;
      preoutput[0] = strtod(thinglist[0], &pointy);
      if ((*pointy)) {
	dfail = 1;
      }
      else {
	theoutput[0] = (long) preoutput[0];
      }
    }

    /* This should be '\0' */
    if (dfail && lfail) {
      getlongfromstring = 2;
      goto error;
    }

    /* n must be 0 */
    if ((n)) {
      getlongfromstring = 1;
      goto error;
    }

    *nmax = 0;
    *output = theoutput[0];
  }
  else if (plen(thinglist) == 3) {
    for (i = 0; i < 3; ++i) {
      lfail = dfail = 0;
      theoutput[i] = strtol(thinglist[i], &pointy, 10);
      if ((*pointy)) {
	lfail = 1;
	preoutput[i] = strtod(thinglist[i], &pointy);
	if ((*pointy)) {
	  dfail = 1;
	}
	else {
	  theoutput[i] = (long) preoutput[i];
	}
      }

    /* This should be '\0' */
      if (dfail && lfail) {
	getlongfromstring = 2;
	goto error;
      }
    }

    /* Now check if the numbers make sense */
    if (theoutput[1] >= theoutput[0]) {
      if (theoutput[2] <= 0) {
	getlongfromstring = 3;
	goto error;
      }
    }
    else {
     if (theoutput[2] >= 0) {
	getlongfromstring = 3;
	goto error;
      }
    }
    *nmax = roundnormal((theoutput[1] - theoutput[0])/theoutput[2]);

    if ((n < 0) || (n > *nmax)) {
      getlongfromstring = 1;
      goto error;
    }

    *output = theoutput[0]+n*theoutput[2];
  }
  else {
    getlongfromstring = 2;
  }

  freeparsed(thinglist);

  return 0;

 error:
  if (thinglist)
    freeparsed(thinglist);
  return getlongfromstring;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a number if possible from a string */
static int getdoublefromstring(char *input, int n, double *output, int *nmax) 
{
  int line = 1; /* dummy */
  char **thinglist = NULL;
  char *pointy;
  int getdoublefromstring;
  int i;
  double theoutput[3];

  if (!(thinglist = sparsenext(":", "", "", "", "", "", -1, &input, &line, 1, 1)))
    return -1;

  if (plen(thinglist) == 1) {
    theoutput[0] = strtod(thinglist[0], &pointy);

    /* This should be '\0' */
    if ((*pointy)) {
      getdoublefromstring = 2;
      goto error;
    }
    /* n must be 0 */
    if ((n)) {
      getdoublefromstring = 1;
      goto error;
    }

    *nmax = 0;
    *output = theoutput[0];
  }
  else if (plen(thinglist) == 3) {
    for (i = 0; i < 3; ++i) {
      theoutput[i] = strtod(thinglist[i], &pointy);
      if ((*pointy)) {
	getdoublefromstring = 2;
	goto error;
      }
    }

    /* Now check if the numbers make sense */
    if (theoutput[1] >= theoutput[0]) {
      if (theoutput[2] <= 0.) {
	getdoublefromstring = 3;
	goto error;
      }
    }
    else {
      if (theoutput[2] >= 0.) {
	getdoublefromstring = 3;
	goto error;
      }
    }
    *nmax = roundnormal((theoutput[1] - theoutput[0])/theoutput[2]);

    if ((n < 0) || (n > *nmax)) {
      getdoublefromstring = 1;
      goto error;
    }

    *output = theoutput[0]+n*theoutput[2];
  }
  else {
    getdoublefromstring = 2;
  }

  freeparsed(thinglist);
  return 0;

 error:
  if (thinglist)
    freeparsed(thinglist);
  return getdoublefromstring;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Return length of NULL-terminated array of char arrays */
static int plen(char **thelist)
{
  int plen = 0;

  if (!(thelist))
    return -1;

  while((thelist[plen]))
    ++plen;

  return plen;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: simparse.c,v $
   Revision 1.4  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.3  2011/05/10 00:30:16  jozsa
   Left work

   Revision 1.2  2010/03/18 15:50:55  jozsa
   Left work

   Revision 1.5  2010/02/17 01:21:40  jozsa
   finished decomp complex

   Revision 1.4  2009/06/19 17:31:59  jozsa
   Bug removal evaluating the sep option in parsenext

   Revision 1.3  2009/03/27 11:17:12  jozsa
   Left work

   Revision 1.2  2009/03/09 12:02:46  jozsa
   included decomposer input functions
   +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* #undef malloc */
/* #undef free */
/* #undef realloc */

/* void *malloc_here(size_t size) */
/* { */
/*   ++numberofmallocs; */
/*   return malloc(size); */
/* } */

/* void free_here(void *where) */
/* { */
/*   ++numberoffrees; */
/*   return free(where); */
/* } */

/* void *realloc_here(void *where, size_t size) */
/* { */
/*   ++numberofreallocs; */
/*   return realloc(where, size); */
/* } */

/* void stats_here() */
/* { */
/*   printf("malloc: %i   free: %i   realloc: %i\n", numberofmallocs, numberoffrees, numberofreallocs); */
/*   return; */
/* } */



/* ------------------------------------------------------------ */

/* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   Revision 1.1.1.1  2009/03/06 12:57:04  jozsa
   imported

   Revision 1.5  2007/08/22 15:58:42  gjozsa
   Left work

   Revision 1.4  2004/12/07 10:31:45  gjozsa
   Left work

   Revision 1.3  2004/11/16 18:18:34  gjozsa
   Left work

   Revision 1.2  2004/11/09 15:52:54  gjozsa
   Fixed bug while counting lines in fct parsenext

   Revision 1.1.1.1  2004/10/29 11:13:20  gjozsa
   Added to CVS control


   ------------------------------------------------------------ */

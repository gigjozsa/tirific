/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file ftstab.c
   @brief A utility for creating fits tables

      The module has been created for use in the tirific environment. It
   is designed to guarantee io of the corresponding fits tables. As it
   is held quite general, it may serve other purposes as well. The
   main feature is that the io of the tables is held at a file io
   level, meaning that the functions are slow, but large tables up to
   2GB can be accessed. 

   Most of the functions can be called from fortran. To do that,
   declare the function in Fortran and call it. Exchange between
   Fortran and c takes place via the use of pointers, while the
   function call in Fortran has to be with the variables (as of course
   there are no pointers). The underscored functions are those which
   take as an input only pointers, and thus are called from Fortran if
   calling the function without underscore.

   The organisation of the module is as follows. The user has access
   to one fits table at a time. To get this access, precautions have
   to take place. Four control structures that are private to the
   module steer the io of the table.

   In principle, it is not difficult to get the module to deliver
   access to a lot of files at the same time. For that, the only thing
   is to pack all the global variables into one struct, make two
   functions, one that constructs and one that destructs this struct,
   and add as a parameter the pointer to this struct to every function
   in this module. Each global variable (exclusively marked with an
   underscore) has to be replaced with a struct -> variable in the
   functions. This is the work of one or two hours. The current
   version has the benifit of one less parameter for each function.

   Each table column gets a number of keywords attached in the header:
   TFOi is the numerical type (fits required keyword) of the column i

   TITi is the name of the column i
   TTYi is the physical type of the column i, e.g. VELO for velocity
   (has to be fits conform) TUNi is the unit of the column i
   TSCi describes the multiplicative factor with which the single
   values are multiplied to get to the real value in the column i (has
   to be fits conform)
   TZEi describes the additional constant which is added to the
   single values to get to the real value in the column i (has to be
   fits conform)
   RADi is the radius (double accuracy) of the column i (free
   keyword)
   GRIi is the grid (accuracy adjusted to TTYi) of the column i
   (free keyword)
   TMAi is the maximum (accuracy adjusted to TTYi) of the column i
   (free keyword)
   TMIi is the minimum (accuracy adjusted to TTYi) of the column i
   (free keyword)

   The keywords TTYi, TUNi, TSCi, TZEi are tied to TITi, as
   a TITi gets a title number and the rest of the keywords is tied
   to the TIT in a header item list (hdl). The hdl contains some
   predefined standard tuples. However, it can always be completely
   changed and analysed with the functions:

   ftstab_putlcoltitl(), ftstab_putlcolunit(), ftstab_putlcoltype(),
   ftstab_putcolunit(), ftstab_putcoltype(), ftstab_putcoltitl(),
   ftstab_gtitln_(), ftstab_glunit(), ftstab_gtzero(),
   ftstab_gtscal(), ftstab_gunit(), ftstab_gltype(), ftstab_gltitl(),
   ftstab_gtitl(), ftstab_hdlreset_(), ftstab_hdladditem()

   The second structure that controls the io is the column descriptor
   array (cda), in which the keywords TFOi, TITi, RADi, GRIi,
   TMAi, TMIi are controlled.  For the creation of a new table or
   the controlled opening of a table, the cda has to be initialised
   with the function ftstab_inithd(), specifying the number of columns
   of the table. It can be filled and accessed with the functions

   ftstab_findminmax(), ftstab_get_colmin(), ftstab_get_colmax(),
   ftstab_get_colgrd(), ftstab_get_colrad(), ftstab_get_coltyp(),
   ftstab_get_coltit(), ftstab_get_maxi_(), ftstab_get_mini_(),
   ftstab_get_radi_(), ftstab_get_grid_(), ftstab_get_type_(),
   ftstab_get_title_(), ftstab_fillhd(), ftstab_inithd()

   The third and fourth structure are qfits header structures that
   contain the "real" headers that are put to the file. One is the
   main header that belongs to the currrently adressed extension, one
   is a "history header" that is always attached to the end of the
   file if initialised. While the cda changes with the acquirement of
   the table, i.e. if the table is enlarged, minimum and maximum is
   changed, the data header will not be changed with respect to the
   minimum and maximum. Before the table gets closed, the current
   maximum and minimum can be updated in the main
   header. Additionally, the possibility exists to directly change the
   headers and append or change cards, which comes convenient for
   saving comments. The functions to adress headers are:

   ftstab_genhd(), ftstab_clearhd(), ftstab_getcard(),
   ftstab_putcard(), ftstab_putminmax_()

   The function ftstab_fopen() opens a table in various ways depending
   on the creation and acquisision of the control objects in the
   module (see the description there). In any case a check against the
   possible header titles will take place in case of trying to open an
   existent extension. This functionality might seem ridiculous but is
   useful when files shall be handled in a single environment that
   should be checked against erroneous data. The function takes care
   of the blocking mechanism, that is, if a file is endangered to
   become corrupted in case of an addition of an item, a table row, or
   a header item, this operation is not allowed. If ftstab_fopen
   encounters a file in which the last extension is meant to be read,
   and it proves that this last extension does not comply with the
   header information concerning the number of rows this number will
   be corrected if possible. This means, if ftstab.c is aborted in the
   course of action, it is likely that the resulting file contains as
   many rows as have been written up to that point, but this
   information is not written in the header. ftstab_fopen recognizes
   such a case and corrects for this error. A history header, however
   will be lost with an abort of the program.

   Once opened the user has the possibility to read from and write to
   the table. The functions

   ftstab_appendrow_(), ftstab_putrow(), ftstab_putval(),
   ftstab_get_row(), ftstab_get_value()
 
   serve at a low level for these purposes, while there are three
   functions that change the table in a more complex way. Mark that
   all functions adress data "as is" without scaling according to tscal
   and tzero.  ftstab_heapsort() sorts the table with a heapsort
   algorithm, ftstab_histo() and ftstab_histo_2d() create fits images
   with a histogram of one or two rows rearranging the table.

   To get information about the status of the table, number of rows,
   columns, extension adressed, use: 

   ftstab_get_rownr_() ,ftstab_get_colnr_(), ftstab_get_extnr_()

   The function ftstab_close() closes the stream caring for the right
   format of the output stream, putting the current header(s) to the
   file. If one wants to adress two different extensions in a file the
   file has to be reopened, i.e. you have to call close(). With
   close(), however, not all information is "lost". All the control
   structures are kept, which affects a possible ftstab_fopen()
   call. To reset the module to its original status, the function
   ftstab_flush() can be used. All acquired information is forgotten.

   Two functions can be used to directly change an opened file or copy a file:

   ftstab_copytohere_() makes a partial copy of the fits file,
   ftstab_deleterest_() deletes the complete information that follows
   the current extension in the file.

   The functions ftstab_get_intarr() and ftstab_get_dblarr() deliver
   int or double arrays of the length of a row.

   Compilation of qfits to be used with this module:

   tar zxvf qfits-4.3.7-mod.tar.gz
   cd qfits-4.3.7-mod
   configure
   make
   
   The underscored functions are safe to use in a fortran code. They
   require a (pointer) variable once. It is not necessary to maintain
   the variable.

   $Source: /Volumes/DATA_J_II/data/CVS/tirific/src/ftstab.c,v $
   $Date: 2011/05/25 22:25:26 $
   $Revision: 1.53 $
   $Author: jozsa $
   $Log: ftstab.c,v $
   Revision 1.53  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.52  2011/05/10 00:30:15  jozsa
   Left work

   Revision 1.51  2011/03/23 22:32:42  jozsa
   Left work

   Revision 1.50  2010/07/28 23:04:19  jozsa
   Left work

   Revision 1.49  2009/05/26 07:56:41  jozsa
   Left work

   Revision 1.48  2007/08/22 15:58:41  gjozsa
   Left work

   Revision 1.47  2006/07/18 09:33:02  gjozsa
   Left work

   Revision 1.46  2005/04/20 13:26:25  gjozsa
   Left work

   Revision 1.45  2005/04/15 15:40:58  gjozsa
   Changed gtitln and gltitln, now an error report also if NULL is passed

   Revision 1.43  2005/04/14 10:32:14  gjozsa
   Left work

   Revision 1.42  2005/04/04 08:42:19  gjozsa
   Left work

   Revision 1.41  2005/03/26 17:52:10  gjozsa
   Another bug at about 4800, wrong adressing of values

   Revision 1.39  2005/03/26 10:06:36  gjozsa
   Another bug at line 4200

   Revision 1.38  2005/03/25 18:17:19  gjozsa
   Left work

   Revision 1.36  2005/03/25 14:41:27  gjozsa
   I introduced the bug, it should be ok now

   Revision 1.35  2005/03/25 12:55:40  gjozsa
   Either removed or created a nasty bug at about line 4302, question when recovering a file

   Revision 1.34  2005/03/22 12:07:52  gjozsa
   Added byteperow_ = 0 in line 4423, (clearhd)

   Revision 1.33  2005/03/19 17:55:52  gjozsa
   Left work

   Revision 1.32  2005/03/17 16:24:10  gjozsa
   bugfix

   Revision 1.31  2005/03/17 08:28:24  gjozsa
   Include documentation also in the source

   Revision 1.30  2005/03/11 17:45:54  gjozsa
   Left work

   Revision 1.29  2005/03/01 17:46:20  gjozsa
   Left work

   Revision 1.28  2005/02/21 11:23:44  gjozsa
   Added the recover functionality to checkin, now finished until some additional thing is needed

   Revision 1.26  2005/02/21 09:44:24  gjozsa
   Stable, tested, and nearly final version, added histogram functions

   Revision 1.25  2005/02/16 18:55:32  gjozsa
   Left work

   Revision 1.24  2005/02/16 13:30:25  gjozsa
   Largely debugged and tested, added histogram function

   Revision 1.23  2005/02/15 18:00:34  gjozsa
   Left work

   Revision 1.22  2005/02/15 15:37:12  gjozsa
   Left work

   Revision 1.21  2005/02/14 17:48:59  gjozsa
   Left work

   Revision 1.20  2005/02/11 17:37:13  gjozsa
   Left work

   Revision 1.19  2005/02/10 17:57:42  gjozsa
   Left work

   Revision 1.18  2005/02/10 16:15:30  gjozsa
   Implemented and tested heapsort

   Revision 1.17  2005/02/10 12:29:08  gjozsa
   Implemented and tested heapsort

   Revision 1.16  2005/02/09 17:52:19  gjozsa
   Left work

   Revision 1.15  2005/02/09 12:06:31  gjozsa
   Added and tested some internal functions

   Revision 1.14  2005/02/08 11:37:13  gjozsa
   Added and tested putval and putrow functions

   Revision 1.13  2005/02/07 16:06:04  gjozsa
   Quite tested version, added lots of functionality

   Revision 1.12  2005/02/05 16:20:51  gjozsa
   Left work

   Revision 1.11  2005/02/04 18:01:12  gjozsa
   Left work

   Revision 1.10  2005/02/03 17:23:56  gjozsa
   Left work

   Revision 1.9  2005/02/02 17:33:42  gjozsa
   Left work

   Revision 1.8  2005/02/01 17:45:53  gjozsa
   Left work

   Revision 1.7  2005/02/01 14:51:20  gjozsa
   Left work

   Revision 1.6  2005/01/28 17:29:03  gjozsa
   Tested and debugged

   Revision 1.5  2005/01/27 17:44:43  gjozsa
   Left work

   Revision 1.4  2005/01/26 17:09:38  gjozsa
   First stable and tested version

   Revision 1.3  2005/01/24 15:48:46  gjozsa
   Left work

   Revision 1.1  2005/01/17 12:16:04  gjozsa
   added to cvs control


*/
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <stdio.h>
#include <string.h>
#include <qfits.h>
/* deleted with qfits */
/* #include <xmemory.h> */
#include <float.h>
#include <limits.h>
#include <math.h>
/* #include <ftsoutput.h> */
#include <cubarithm.h>



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <ftstab.h>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def _MEMORY_HERE_ON
   @brief Controls the use of the memory_here module

   If you don't want to use the memory_here facility comment this
   define, otherways it will be included.

*/
/* ------------------------------------------------------------ */
/* #define _MEMORY_HERE_ON */
/* #include <memory_here.h> */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def COLTITL_DEFAULT @brief Default value for the Column name

   If adding a new item, change also the functions putcoltitl,
   putcolunit, putcoltype, putlcoltitl, putlcolunit, putlcoltype in
   the source code. Those definitions can be deleted, but they are
   kept for testing reasons.

*/
/* ------------------------------------------------------------ */
#define COLTITL_DEFAULT 0


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def COLBYTE_DEFAULT @brief Default value for the Column byte number
*/
/* ------------------------------------------------------------ */
#define COLBYTE_DEFAULT 4
#define COLBYTE_FLOAT   4
#define COLBYTE_CHAR    1
#define COLBYTE_INT     4
#define COLBYTE_DOUBLE  8



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE MACROS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE TYPEDEFS */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE STRUCTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @union multype
   @brief Type containing some basic types

   @li @c char   ch @c A char
   @li @c long   in @c A long
   @li @c float  fl @c A float
   @li @c double do @c A double
*/
/* ------------------------------------------------------------ */
typedef union multype {
  char  c;
  long  i;
  float f;
  double  d;
} multype;


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct Rowdesc
   @brief Descriptor element of a table

   @li @c int     @c title Name of column
   @li @c int     @c type Numerical type of column, CHAR, INT, FLOAT, DOUBLE
   @li @c multype @c radi Radius of column
   @li @c multype @c delt Grid spacing of column
   @li @c multype @c maxi Maximum of column
   @li @c multype @c mini Minimum of column

   The Rowdesc struct contains descriptors to describe a column of the
   table. The header of the fits file will be generated by an array of
   Rowdesc structs.  The name and the type of the column is given by
   an integer, as defined by the symbolic constants for this purpose
   in this module. radi is the radius at which the column items are
   evaluated, delt is the grid spacing for the column.  Internal
   structure of the multype elements is to contain always the type
   specified in type
*/
/* ------------------------------------------------------------ */
typedef struct Rowdesc 
{
  /** @brief Name of column */
  int titl;

  /** @brief Numerical type of column, CHAR, INT, FLOAT, DOUBLE */
  int type;

  /** @brief Radius of column */
  double radi;

  /** @brief Grid spacing of column */
  multype grid;

  /** @brief Maximum of column */
  multype maxi;

  /** @brief Minimum of column */
  multype mini;

} Rowdesc;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct hdrit
   @brief Item that describes the context of a header item

   @li @c char[19]        @c titl  The title (name of the item)
   @li @c char[19]       @c ttype The ttype belonging to item
   @li @c char[19]       @c tunit The tunit belonging to item
   @li @c double         @c tzero The tzero belonging to item
   @li @c double         @c tscal The tscale belonging to item
   @li @c struct hdrit * @c next  The next hdrit in the list   

   Element of a list of header elements that are valid in the current
   module, Belongs to a linked list that is adressed by the user via
   access functions.
*/
/* ------------------------------------------------------------ */
typedef struct hdrit
{
  /** @brief The title (name of the item) */
  char titl[19];
  
  /** @brief The ttype belonging to item */
  char ttype[19];
  
  /** @brief The tunit belonging to item*/
  char tunit[19];
  
  /** @brief The tzero belonging to item*/
  double tzero;
  
  /** @brief The tscale belonging to item*/
  double tscal;
  
  /** @brief The next hdrit in the list*/
  struct hdrit *next;

} hdrit;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @struct hdrlist
   @brief List of header items and their context

   @li @c hdrit @c *first The first item
   @li @c hdrit @c *last The last item for convenience
   @li @c int   @c number The number of items

   Top structure to the list of header items in the module
*/
/* ------------------------------------------------------------ */
typedef struct hdrlist
{
  /** @brief The first item) */
  hdrit *first;
  
  /** @brief The last item) */
  hdrit *last;
  
  /** @brief The number of items */
  int n;

} hdrlist;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* (PRIVATE) GLOBAL VARIABLES */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static Rowdesc *hdrarray
   @brief Array describing the header of the fits file

   Array of Rowdesc structs, not terminated
 */
/* ------------------------------------------------------------ */
static Rowdesc *hdrarray_ = NULL;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static int ncolumns
   @brief Number of columns of the array

 */
/* ------------------------------------------------------------ */
static int ncolumns_ = 0;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static int nrows_
   @brief Number of rows of the array

 */
/* ------------------------------------------------------------ */
static long nrows_ = 0;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static FILE *stream_
   @brief Stream to put the table
 */
/* ------------------------------------------------------------ */
static FILE *stream_ = NULL;


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static int curext_
   @brief The current extension number
 */
/* ------------------------------------------------------------ */
static int curext_ = 0;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static qfits_header *header_
   @brief Qfits header object
 */
/* ------------------------------------------------------------ */
static qfits_header *header_ = NULL;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static qfits_header *lastheader_
   @brief Qfits header object
 */
/* ------------------------------------------------------------ */
static qfits_header *lastheader_ = NULL;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static fpos_t headerstart_
   @brief Start of the binary table
 */
/* ------------------------------------------------------------ */
static fpos_t headerstart_;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static fpos_t tablestart_
   @brief Start of the binary table
 */
/* ------------------------------------------------------------ */
static fpos_t tablestart_;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static int byteperow_
   @brief The bytes in a row
 */
/* ------------------------------------------------------------ */
static long byteperow_ = 0;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static long *byteoffset_ 

   @brief Offset at end of each column in bytes, initialised with
   ftstab_open
 */
/* ------------------------------------------------------------ */
static long *byteoffset_ = NULL;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static char headerblock_ 

   @brief Is the header blocked for adding items? 0 no 1 yes
 */
/* ------------------------------------------------------------ */
static char headerblock_ = 0;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static char headerblock_ 

   @brief Is the table blocked for adding items? 0 no 1 yes
 */
/* ------------------------------------------------------------ */
static char tableblock_ = 0;



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @var static headerlist headerlist_ 

   @brief The list of valid header items and their context
 */
/* ------------------------------------------------------------ */
static hdrlist hdrlist_ = {NULL, NULL, 0};



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static FILE *ftstab_open_(char *filename) 

   @brief Opens a stream for writing to a file with filename and puts
   a fits header generated from the internal column description table

   Function to call after initialising and filling the internal column
   description table. Will return a FILE pointer to an opened file for
   appending the binary table rowwise with function appendrow or
   appendrow_. The FILE pointer will also be stored internally for the
   use with appendrow_.
   This function is robust with respect to a change of filename,
   hopefully.

   @param filename (char *) The name of the file to write to
   
   @return (success)  FILE *opencolfts_: pointer to opened stream\n
           (error) NULL
 */
/* ------------------------------------------------------------ */
static FILE *ftstab_open_(char *filename);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static void hdl_init(void)
  @brief Initializes the standard header context table
*/
/* ------------------------------------------------------------ */
static int hdl_init(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int histo_putarray(int column1, long startrow, long endrow, long bins1, double min1, double max1, int *iarray)
  @brief Initializes the standard header context table
*/
/* ------------------------------------------------------------ */
static int histo_putarray(int column1, long startrow, long endrow, long bins1, double min1, double max1, double delta1, int *iarray);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_hdldelitem(char *titl);
   @brief Delete an item from the header item list

   Deletes an item from the header item list. After successful call,
   it is better to access the item number with the functions in this
   module. Deleting the last element will result in a creation of the
   default in the next step. This function is taken from the user
   interface, because it makes no real sense, nevertheless, we can
   keep it...

   @param title (char *) The titel of the item

   @return (success) int ftstab_hdldelitem: 1
           (error) 0 if the item does not exist
 */
/* ------------------------------------------------------------ */
int ftstab_hdldelitem(char *titl);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int hdlapp(char *titl, char *ttype, char *tunit, double tzero, double tscal)
  @brief Adds an item to the hdr table without checking

  @return 1 for success 0 else
*/
/* ------------------------------------------------------------ */
static int hdlapp(char *titl, char *ttype, char *tunit, double tzero, double tscal);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn static int numberget(qfits_header *header, int item, int column, multype *number)
  @brief Returns the identifyer of an item in the column of a header

  Returns the identyfier or the value in a header. It will check for
  the units and types in use for a given title name.
  
  item = 0: (returns .i) The column name as defined in ftstab.h
  item = 1: (returns .i) The column numerical type as defined in ftstab.h
  item = 2: (returns .d) The radius
  item = 3: (returns .d) The grid spacing
  item = 4: (returns .d) The maximum
  item = 5: (returns .d) The minimum
  
  @param header (qfits_header *) qfits header to parse
  @param item   (int)            item to return info on
  @param column (int)            column to return info on
  @param number (multype)        The place to put the info, must be allocated

  @return int numberget (success): 1
                            (error): 0
*/
/* ------------------------------------------------------------ */
static int numberget(qfits_header *header, int item, int column, multype *number);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static int putrownr(void) 

   @brief Corrects the header in an open table stream with respect to
   the number of rows

   This function records the number of rows in the
   header. The stream is not closed.
   
   @return (success) int finishcolfts_; 1
           (error) 0
 */
/* ------------------------------------------------------------ */
static int putrownr(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static void putrowstay(double *row) 

   @brief Dumps a row but stays at the position

   Dumps the row at the current position of stream_ in the right
   format. No check is done whatsoever, except if the basic operation
   was successful.
   
   @return void
 */
/* ------------------------------------------------------------ */
static void putrowstay(double *row);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static void putquickrow(long firstrow, long rownumber, char *rowtoput) 

   @brief Dumps a row at a given rownumber offset to firstrow

   @return void
 */
/* ------------------------------------------------------------ */
static void putquickrow(long firstrow, long rownumber, char *rowtoput);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static void getquickrow(long firstrow, long rownumber, char *rowtoget) 

   @brief Puts a row at a given rownumber offset to firstrow into the allocated rowtoget 

   @return void
 */
/* ------------------------------------------------------------ */
static void getquickrow(long firstrow, long rownumber, char *rowtoget);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static void putvalstay(int column, double val) 

   @brief Dumps a value but stays at the position

   Dumps the value at the current position of stream_ in the right format for column. No check is done whatsoever, except if the basic operation
   was successful.
   
   @return void
 */
/* ------------------------------------------------------------ */
static void putvalstay(int colnumber, double value);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static void getrowstay(double *row) 

   @brief Dumps a row from a stream into an array but stays at the position

   Dumps the row at the current position of stream_ in the (allocated)
   double array. No check is done whatsoever.
   
   @return void
 */
/* ------------------------------------------------------------ */
static void getrowstay(double *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static double getvalstay(int column, double *val) 

   @brief Reads a value but stays at the position

   Returns in the value at the current position of stream_ in the right format for column. val has to be allocated. No check is done whatsoever. Column  starts at 0.
   
   @return void
 */
/* ------------------------------------------------------------ */
static double getvalstay(int colnr);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static double getquickval(int colnr, long firstval, long rownr) 

   @brief Unsafe function to get a value quickly

   Returns the value in the row rownr (starts with 0) for a given specification of the
   byte offset of the first value belonging to a column in the
   table. This function is unsafe, the operation has to be checked
   before. The file pointer will stay where it is after the operation.

   @param colnr (int) for the identification of the numerical type (starts with 0)
   @param firstval (long) The offset in the file of the first value of the column
   @param rownr (long) The rownumber to be checked
   
   @return double getquickval: The content of the column converted to double
 */
/* ------------------------------------------------------------ */
static double getquickval(int colnr, long firstval, long rownr);




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static void checkminmax(double *row) 

   @brief Changes minima and maxima according to the content of row

   Unsafe. Checks the minima and maxima in the header description
   array according to the content of row.
   
   @return void
 */
/* ------------------------------------------------------------ */
static void checkminmax(double *row);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static void checkminmax_single(int colnumber, double value) 

   @brief Changes minima and maxima according to the content of value

   Unsafe. Checks the minima and maxima in the header description
   array in column colnumber according to the content of value.
   
   @return void
 */
/* ------------------------------------------------------------ */
static void checkminmax_single(int colnumber, double value);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn static void resetminmax(int column) 

   @brief Resets the minima and maxima in the header description array of the column (start with 0) to the highest and lowest possible values
   
   @return void
 */
/* ------------------------------------------------------------ */
static void resetminmax(int column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn static int check_namenumber(int key, int column, qfits_header *header)
  @brief Checks for ftstab consistency for the given key in the header

  Checks if the TUN and TTY are present and conform with the
  ftstab format
 
  @param key    (int)            The COLTITL to check for
  @param column (int)            The column to check
  @param header (qfits_header *) The header to check

  @return int  check_namenumber(success): 1
                               (error): 0
*/
/* ------------------------------------------------------------ */
static int check_namenumber(int key, int column, qfits_header *header);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn static qfits_header *readfilehead(char *filename, int next, Rowdesc *descriptor, int *ncolumns)
  @brief Returns a header array read from a fits file and puts the information in a Rowdes array

  If possible reads the information contained in filename. filename
  must comply with being a fits file and being compatible with ftstab format.

  @param filename   (char *)     The filename
  @param next       (int)        The extension number
  @param descriptor (Rowdesc **) Adress of pointer to a Rowdesc array
  @param ncolumns   (int *)      Allocated output for the number of columns

  @return (success) qfits_header *readfilehead: If the information is present, an allocated header.\n
          (error) NULL
*/
/* ------------------------------------------------------------ */
static qfits_header *readfilehead(char *filename, int next, Rowdesc **descriptor, int *ncolumns);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn static FILE *checkouthistofile(char *filename, qfits_header **header, qfits_header **hheader)
  @brief Opens a file for histogram output

The function opens the filename. If it is a fits file, then the last header will be checked for being a hitory header, put in *hheader and deleted, otherways the content of hheader will be set to 0. If it is not a fits file, it will be deleted if existent and an empty stream will be returned. The filepointer will be set to the end of the file. A header will be filled with a structural header that contains a skeleton header for the histogram.

  @param filename (char *)          The filename
  @param header  (qfits_header **) Pointer to a header pointer
  @param hheader  (qfits_header **) Pointer to a header pointer

  @return (success) FILE *checkouthistofile: The pointer to an open file
          (error) NULL
*/
/* ------------------------------------------------------------ */
static FILE *checkouthistofile(char *filename, qfits_header **header, qfits_header **hheader);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn static int checkfilefts(char *filename, int next, char hheader)
  @brief Check if the file is fits and low level ftstab conforming

  Checks if the file is conforming to ftstab standard format for a fits file, i.e. if it is a fits, the primary header is minimal, if there is only one extension, and if this extension is a table in the ftstab output format.

  @param filename (char *) The filename
  @param next     (int)    The extension number
  @param hheader  (char)   0: remove a last hdu from analysis if history header
                           1: treat a last hdu in any case as if being a normal hdu

  @return (success) int checkfilefts: 0 if it is ok.\n
          (error) 1: file does not exist, 
                  2: file exists and is not a fits file,
		  4: the extension does not exist, a hdu can be attached
5: The number of header items is inconsistent with ftstab format: not in use
6: Memory error or a header item not present
7: TFIELDS value is incorrect if file shall be ftstab conform
8: Some extension descriptor is not ftstab conform

9: Some other error (e.g. memory)
*/
/* ------------------------------------------------------------ */
static int checkfilefts(char *filename, int next, char hheader);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn static int checkhdarray(Rowdesc *temp, int ncolumns)
  @brief Checks if the current hdarray is compatible with temp

  Returns information about the current hdarray_. 

  @param temp     (Rowdesc *) A Rowdesc array or NULL
  @param ncolumns (int *)     Number of columns in the Rowdesc array

  @return int checkhdarray: 0 no hdarray_ existent\n
                            1 hdarray_ existent and compatible
                            2 hdarray_ existent and not compatible
*/
/* ------------------------------------------------------------ */
static int checkhdarray(Rowdesc *temp, int ncolumns);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn static qfits_header *checkhheader(char *filename)
  @brief Checks if the last extension is a history header

  Checks if the last extension is a history header and returns the header if yes.

  @param filename (char *) The filename

  @return (success) qfits_header *checkhheader: The header
          (error) NULL
*/
/* ------------------------------------------------------------ */
static qfits_header *checkhheader(char *filename);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn static int checkin(char *filename; qfits_header *header, Rowdesc *temp, int next, char hheader)
  @brief Checks in the file with an acquired header and Rowdesc table

  Opens stream_ with the file filename and initialises all information
  to start io for the given extension. If a last extension is present
  that is an image with no data this is interpreted as being a
  "history" extension. lastheader_ will be updated with the
  information in this last header. If the desired extension is not
  present, then a new extension will be created that is appended to
  the end of the fits file, in case of no "history" header present. If
  a "history" header is present, the new extension is put in-between
  the last and the one last-before. In any case, the header will be
  put in the file, overwriting any old data of the file at that
  position!!! The user has to care for that fact! hheader controls wheter to treat the last header as a history header or not to do that.

  @param filename (char *)         The file to open
  @param header   (qfits_header *) The header containing the rowdesc information (and probably more)
  @param temp     (Rowdesc *)      The rowdesc array
  @param next     (int)            The extension number requested

  @return (success) int checkin: 1
          (error)   0
*/
/* ------------------------------------------------------------ */
static int checkin(char *filename, qfits_header *header, Rowdesc *temp, int next, char hheader);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION CODE */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Reserves space for the header descriptor */
int ftstab_inithd(int numberofcols)
{
  int i;

  /* Initialise the headerlist */

  if (!(hdrlist_.n)) {
    if (!hdl_init())
      return 0;
  }

  /* Check if numberofcols is sensible */
  if (numberofcols < 1)
    return 0;

  /* First allocate the space */
  if (!(hdrarray_ = (Rowdesc *) malloc(numberofcols * sizeof(Rowdesc))))
    return 0;
  else {
    ncolumns_ = numberofcols;

    /* Allocation has worked, put some default in the structure */
    for (i = 0; i < ncolumns_; ++ i) {
      hdrarray_[i].titl = COLTITL_DEFAULT;
      hdrarray_[i].type = COLTYPE_DEFAULT;
      hdrarray_[i].radi = COLRADI_DEFAULT;
      hdrarray_[i].grid.f = COLGRID_DEFAULT;
      hdrarray_[i].maxi.f = -FLT_MAX;
      hdrarray_[i].mini.f = FLT_MAX;
    }
  }
  return 1;
}
/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Reserves space for the header descriptor */
int ftstab_inithd_(int *numberofcols)
{
  return ftstab_inithd(*numberofcols);
}
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Resets the minima and maxima in the header description array of the
   column (start with 0) to the highest and lowest possible values */

static void resetminmax(int column)
{
  switch(hdrarray_[column].type) {
    case COLTYPE_FLOAT:
      hdrarray_[column].maxi.f = -FLT_MAX;
      hdrarray_[column].mini.f = FLT_MAX;
      break;
    case COLTYPE_CHAR:
      hdrarray_[column].maxi.c = CHAR_MIN;
      hdrarray_[column].mini.c = CHAR_MAX;
      break;
    case COLTYPE_INT:
      hdrarray_[column].maxi.i = INT_MIN;
      hdrarray_[column].mini.i = INT_MAX;
      break;
    case COLTYPE_DOUBLE:
      hdrarray_[column].maxi.d = -DBL_MAX;
      hdrarray_[column].mini.d = DBL_MAX;
      break;
    default:
      hdrarray_[column].maxi.f = -FLT_MAX;
      hdrarray_[column].mini.f = FLT_MAX;
      break;
  }
  return;
}
   


/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Fills column column of the table description object with the given
   values */

int ftstab_fillhd(int column, int title, int type, double radius, double grid)
{

  /* Check if the header array has been initialised */
  if (!(hdrarray_))
    return 0;

  /* Check if the column number is low enough */
  if (column >= ncolumns_)
    return 0;

  /* Fill the struct */
  hdrarray_[column].titl = title;
  hdrarray_[column].type = type;
  hdrarray_[column].radi = radius;

  resetminmax(column);

  switch(hdrarray_[column].type) {
    case COLTYPE_FLOAT:
      hdrarray_[column].grid.f = grid;
      break;
    case COLTYPE_CHAR:
      hdrarray_[column].grid.c = grid;
      break;
    case COLTYPE_INT:
      hdrarray_[column].grid.i = grid;
      break;
    case COLTYPE_DOUBLE:
      hdrarray_[column].grid.d = grid;
      break;
    default:
      hdrarray_[column].grid.f = grid;
      break;
  }
  return 1;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Fills column column of the table description object with the given
   values */

int ftstab_fillhd_(int *column, int *name, int *type, double *radius, double *grid)
{
  return ftstab_fillhd(*column, *name, *type, *radius, *grid);
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Opens a stream for writing to a file with filename and puts a fits
   header generated from the internal column description table */
static FILE *ftstab_open_(char *filename)
{
  int i;
  qfits_header *header;

  /* Check if the column descriptor array has been opened */
  if (!hdrarray_)
    return NULL;

  /* Create and fill the byte offset array */
  if (!(byteoffset_ = (long *) malloc((ncolumns_)*sizeof(long))))
    return NULL;

  byteoffset_[0] = 0;
  for (i = 0; i < ncolumns_-1; ++i) {
    switch(hdrarray_[i].type) {
    case COLTYPE_FLOAT:
      byteoffset_[i+1] = byteoffset_[i]+COLBYTE_FLOAT;
      break;
    case COLTYPE_CHAR:
      byteoffset_[i+1] = byteoffset_[i]+COLBYTE_CHAR;
      break;
    case COLTYPE_INT:
      byteoffset_[i+1] = byteoffset_[i]+COLBYTE_INT;
      break;
    case COLTYPE_DOUBLE:
      byteoffset_[i+1] = byteoffset_[i]+COLBYTE_DOUBLE;
      break;
    default:
      byteoffset_[i+1] = byteoffset_[i]+COLBYTE_DEFAULT;
      break;
    }
  }
  
  /* Open the stream */
  if (!(stream_ = fopen(filename, "w+"))) {
    free(byteoffset_);
    byteoffset_ = NULL;
    return NULL;
  }

  /* Set the buffer to 0 */
/*   setbuf(stream_, NULL); */

  /* Now make a qfits primary header and put it in the stream */
  header = qfits_table_prim_header_default();
  qfits_header_dump(header,stream_);
  qfits_header_destroy(header);

  /* Safe the position where the table header starts */
  fgetpos(stream_, &headerstart_);

  /* It is the user who controls whether a header is right or wrong,
     in any case, if there is none, we generate one, otherways we
     trust in what the user gives */
  if (!(header_)) {
    ftstab_genhd(0);
  }
  /* Will be dumped to the stream */
  qfits_header_dump(header_,stream_);
  
  /* Record the start of the binary table, the size of the header will maybe stay */
  fgetpos(stream_, &tablestart_);
  
  return stream_;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Append a row fo an open table */
int ftstab_appendrow_(double *row) 
{
  /* Check if the file is open */
  if (!stream_)
    return 0;

  /* Check if writing is blocked */
  if (tableblock_)
    return 0;

  /* If this is the first line, the header must be dumped again */
  if (!nrows_) {

    /* Goto the header position */
    fsetpos(stream_, &headerstart_);

    /* Dump the header */
    qfits_header_dump(header_, stream_);

    /* Save the position */
    fgetpos(stream_, &tablestart_);
  }

  /* Now everything should have been done by the user, we expect an array of ncolumns_ columns */

  putrowstay(row);
  checkminmax(row);

  /* Row number increases by 1 */
  ++nrows_;
  
  /* header will be blocked */
  headerblock_ = 1;
  
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Corrects the header in an open table stream with respect to the
   number of rows */

static int putrownr(void)
{
  fpos_t currentpos;
  char key[9], value[21], comment[61];

  /* Check if the stream is present */
  if (!(stream_))
    return 0;

  /* Get the current position */
  fgetpos(stream_, &currentpos);

  /* Correct the header for the rownumber */
  sprintf(key,"NAXIS2");
  sprintf(value,"%li", nrows_);
  comment[0] = '\0';
  qfits_header_mod(header_,key,value,comment);

  /* Wind back to the start of the header */
  fsetpos(stream_, &headerstart_);

  /* Now overwrite the old header with the new one */
  qfits_header_dump(header_,stream_);

  /* At the end, spool back to the current position */
  fsetpos(stream_, &currentpos);

  /* All clear */
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Changes the minmax information to the current value in the header
   of an open fits table */

int ftstab_putminmax_(void) 
{
  char key[9], value[21], comment[61];
  fpos_t currentpos;
  int i;

  /* Check if the stream is present */
  if (!(stream_))
    return 0;

  /* Get the current position */
  fgetpos(stream_, &currentpos);

  /* Correct the header for the mini- and maxima */
  for (i = 1; i <= ncolumns_; ++i) {

  switch(hdrarray_[i-1].type) {
    case COLTYPE_FLOAT:
      /* Internal keyword, not fits restricted */
      sprintf(key,"TMA%i",i);
      sprintf(value,"%.6E", (hdrarray_+(i-1)) -> maxi.f);
      comment[0] = '\0';
      qfits_header_mod(header_,key,value,comment);
      
      /* Internal keyword, not fits restricted */
      sprintf(key,"TMI%i",i);
      sprintf(value,"%.6E", (hdrarray_+(i-1)) -> mini.f);
      comment[0] = '\0';
      qfits_header_mod(header_,key,value,comment);
      
      break;
      
    case COLTYPE_CHAR:
      /* Internal keyword, not fits restricted */
      sprintf(key,"TMA%i",i);
      sprintf(value,"%i", (hdrarray_+(i-1)) -> maxi.c);
      comment[0] = '\0';
      qfits_header_mod(header_,key,value,comment);
      
      /* Internal keyword, not fits restricted */
      sprintf(key,"TMI%i",i);
      sprintf(value,"%i", (hdrarray_+(i-1)) -> mini.c);
      comment[0] = '\0';
      qfits_header_mod(header_,key,value,comment);
      
      break;

    case COLTYPE_INT:
      /* Internal keyword, not fits restricted */
      sprintf(key,"TMA%i",i);
      sprintf(value,"%li", (hdrarray_+(i-1)) -> maxi.i);
      comment[0] = '\0';
      qfits_header_mod(header_,key,value,comment);
      
      /* Internal keyword, not fits restricted */
      sprintf(key,"TMI%i",i);
      sprintf(value,"%li", (hdrarray_+(i-1)) -> mini.i);
      comment[0] = '\0';
      qfits_header_mod(header_,key,value,comment);
      
      break;
      
    case COLTYPE_DOUBLE:
      /* Internal keyword, not fits restricted */
      sprintf(key,"TMA%i",i);
      sprintf(value,"%.12E", (hdrarray_+(i-1)) -> maxi.d);
      comment[0] = '\0';
      qfits_header_mod(header_,key,value,comment);
      
      /* Internal keyword, not fits restricted */
      sprintf(key,"TMI%i",i);
      sprintf(value,"%.12E", (hdrarray_+(i-1)) -> mini.d);
      comment[0] = '\0';
      qfits_header_mod(header_,key,value,comment);
      
      break;
      
    default:
      /* Internal keyword, not fits restricted */
      sprintf(key,"TMA%i",i);
      sprintf(value,"%.6E", (hdrarray_+(i-1)) -> maxi.f);
      comment[0] = '\0';
      qfits_header_mod(header_,key,value,comment);
      
      /* Internal keyword, not fits restricted */
      sprintf(key,"TMI%i",i);
      sprintf(value,"%.6E", (hdrarray_+(i-1)) -> mini.f);
      comment[0] = '\0';
      qfits_header_mod(header_,key,value,comment);
      
      break;
    }
  }


  /* Wind back to the start of the table */
  fsetpos(stream_, &headerstart_);

  /* Now overwrite the old header with the new one */
  qfits_header_dump(header_,stream_);

  /* At the end, spool back to the end */
  fsetpos(stream_, &currentpos);

  /* All clear */
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

int ftstab_close_(void)
{
  long position;
  long i;

  if (!stream_)
    return 0;

  /* This is done under the assumption that ftell delivers the numbers
     of bytes of the whole file, if we are at the end of the file */


  fseek(stream_, 0L, SEEK_END);
  position = (ftell(stream_) + 1L)%2880;

  /* Now, in qfits they use NUL to pad a file, standard is ASCII
     blank, while I don't know whether the description I have is wrong
     or qfits. I believe qfits */

  /* If the table is blocked, we won't do this, because it introduces unwanted whitespaces at the end */
  if (!(tableblock_)) {
    for (i = 0; i <= 2880-position; ++i) {
      fputc(' ', stream_);
    }
  }

  /* Dump the lastheader if present */
  if ((lastheader_))
    qfits_header_dump(lastheader_,stream_);

  putrownr();

  /* Close the stream and set the pointer to NULL */
  fclose(stream_);
  stream_ = NULL;

  /* Now make a cache flash */
  qfits_cache_purge();

  return 1;
}  

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Closes the table stream and destroys all information */

void ftstab_flush_(void)
{
  if (hdrarray_)
    free(hdrarray_);

  hdrarray_ = NULL;

  ncolumns_ = 0;

  nrows_ = 0;

  if (stream_)
    fclose(stream_);

  stream_ = NULL;

  curext_ = 0;

  if (header_)
    qfits_header_destroy(header_);

  header_ = NULL;

  if (lastheader_)
    qfits_header_destroy(lastheader_);

  lastheader_ = NULL;

  byteperow_ = 0;

  if(byteoffset_)
    free(byteoffset_);

  byteoffset_ = NULL;

  headerblock_ = 0;

  tableblock_ = 0;

  qfits_cache_purge();
  
  /* Clear the hdr array, somewhat dangerous, but... */
  ftstab_hdlreset_();

  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the number of rows */

long ftstab_get_rownr_(void)
{
  return nrows_;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the number of columns */

int ftstab_get_colnr_(void)
{
  return ncolumns_;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Generate a header */

int ftstab_genhd(char n)
{
  int i;
  char key[9], value[21], comment[61];

  /* An appendix header that may not have any attached data */
  if (n) {

    /* Check if there is already one header */
    if ((lastheader_))
      return 0;
    
    /* OK, we can create one, the appendix header is a header without
       any data */
    
    lastheader_ = qfits_header_new();
    
    sprintf(key,"XTENSION");
    sprintf(value,"'IMAGE             '");
    comment[0] = '\0';
    qfits_header_append(lastheader_,key,value,comment,"");
    
    sprintf(key,"END");
    value[0] = '\0';
    comment[0] = '\0';
    qfits_header_append(lastheader_,key,value,comment,"");
    
    sprintf(key,"BITPIX");
    sprintf(value,"8");
    comment[0] = '\0';
    qfits_header_add(lastheader_,key,value,comment,"");
    
    sprintf(key,"NAXIS");
    sprintf(value,"0");
    comment[0] = '\0';
    qfits_header_add(lastheader_,key,value,comment,"");
    
    /* PCOUNT is the number of bytes of data after the main table */
    sprintf(key,"PCOUNT");
    sprintf(value,"0");
    comment[0] = '\0';
    qfits_header_add(lastheader_,key,value,comment,"");
    
    sprintf(key,"GCOUNT");
    sprintf(value,"1");
    comment[0] = '\0';
    qfits_header_add(lastheader_,key,value,comment,"");
  }
  else {

    /* If it already exists, we don't do anything */
    if (header_)
      return 0;

    /* We check (again) if the array has been allocated */
    if (!hdrarray_)
      return 0;

    /* Now make a table default header */
    header_ = qfits_header_new();
    sprintf(key,"XTENSION");
    sprintf(value,"'BINTABLE          '");
    comment[0] = '\0';
    qfits_header_append(header_,key,value,comment,"");
    
    sprintf(key,"END");
    value[0] = '\0';
    comment[0] = '\0';
    qfits_header_append(header_,key,value,comment,"");
    
    sprintf(key,"BITPIX");
    sprintf(value,"8");
    comment[0] = '\0';
    qfits_header_add(header_,key,value,comment,"");
    
    sprintf(key,"NAXIS");
    sprintf(value,"2");
    comment[0] = '\0';
    qfits_header_add(header_,key,value,comment,"");
    
    /* The number of rows, at this stage without meaning */
    sprintf(key,"NAXIS2");
    sprintf(value,"%li", nrows_);
    comment[0] = '\0';
    qfits_header_add(header_,key,value,comment,"");
    
    /* PCOUNT is the number of bytes of data after the main table */
    sprintf(key,"PCOUNT");
    sprintf(value,"0");
    comment[0] = '\0';
    qfits_header_add(header_,key,value,comment,"");
    
    sprintf(key,"GCOUNT");
    sprintf(value,"1");
    comment[0] = '\0';
    qfits_header_add(header_,key,value,comment,"");
    
    /* The number of fields in each row (Columns) */
    sprintf(key,"TFIELDS");
    sprintf(value,"%i",ncolumns_);
    comment[0] = '\0';
    qfits_header_add(header_,key,value,comment,"");
    
    /* The format of the fields, IEEE 32bit float, one value, except for
       chisquare */
    for (i = 1; i <= ncolumns_; ++i) {
      sprintf(key,"TFO%i",i);
      switch ((hdrarray_+(i-1)) -> type) {
      case COLTYPE_FLOAT:
	sprintf(value,"'1E                '");
	byteperow_ = byteperow_+4;
	break;
      case COLTYPE_CHAR:
	sprintf(value,"'1B                '");
	byteperow_ = byteperow_+1;
	break;
      case COLTYPE_INT:
	sprintf(value,"'1J                '");
	byteperow_ = byteperow_+4;
	break;
      case COLTYPE_DOUBLE:
	sprintf(value,"'1D                '");
	byteperow_ = byteperow_+8;
      break;
      default:
	sprintf(value,"'1E                '");
	byteperow_ = byteperow_+4;
	break;
      }
      comment[0] = '\0';
      qfits_header_add(header_,key,value,comment,"");
    }
    
    /* The number of bytes in a table row */
    sprintf(key,"NAXIS1");
    sprintf(value,"%li", byteperow_);
    comment[0] = '\0';
    qfits_header_add(header_,key,value,comment,"");
    
    /* The format of the fields, IEEE 32bit float, one value */
    for (i = 1; i <= ncolumns_; ++i) {
      
      /* Private name of the column */
      sprintf(key,"TIT%i",i);
      ftstab_putlcoltitl(value,(hdrarray_+(i-1)) -> titl);
      comment[0] = '\0';
      qfits_header_add(header_,key,value,comment,"");
      
      /* Quasireserved keyword, should be conform with fits */
      sprintf(key,"TTY%i",i);
      ftstab_putlcoltype(value,(hdrarray_+(i-1)) -> titl);
      comment[0] = '\0';
      qfits_header_add(header_,key,value,comment,"");
      
      /* Quasireserved keyword, should be conform with fits */
      sprintf(key,"TUN%i",i);
      ftstab_putlcolunit(value,(hdrarray_+(i-1)) -> titl);
      comment[0] = '\0';
      qfits_header_add(header_,key,value,comment,"");
      
            /* Internal keyword, not fits restricted */
      sprintf(key,"TSC%i",i);
      sprintf(value,"%.12E", ftstab_gtscal((hdrarray_+(i-1)) -> titl));
      comment[0] = '\0';
      qfits_header_add(header_,key,value,comment,"");

      /* Internal keyword, not fits restricted */
      sprintf(key,"TZE%i",i);
      sprintf(value,"%.12E", ftstab_gtzero((hdrarray_+(i-1)) -> titl));
      comment[0] = '\0';
      qfits_header_add(header_,key,value,comment,"");

      /* Internal keyword, not fits restricted */
      sprintf(key,"RAD%i",i);
      sprintf(value,"%.12E", (hdrarray_+(i-1)) -> radi);
      comment[0] = '\0';
      qfits_header_add(header_,key,value,comment,"");
      
      switch(hdrarray_[i-1].type) {
      case COLTYPE_FLOAT:
	/* Internal keyword, not fits restricted */
	sprintf(key,"GRI%i",i);
	sprintf(value,"%.6E", (hdrarray_+(i-1)) -> grid.f);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	/* Internal keyword, not fits restricted */
	sprintf(key,"TMA%i",i);
	sprintf(value,"%.6E", (hdrarray_+(i-1)) -> maxi.f);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	/* Internal keyword, not fits restricted */
	sprintf(key,"TMI%i",i);
	sprintf(value,"%.6E", (hdrarray_+(i-1)) -> mini.f);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	break;
	
      case COLTYPE_CHAR:

	/* Internal keyword, not fits restricted */
	sprintf(key,"GRI%i",i);
	sprintf(value,"%i", (hdrarray_+(i-1)) -> grid.c);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	/* Internal keyword, not fits restricted */
	sprintf(key,"TMA%i",i);
	sprintf(value,"%i", (hdrarray_+(i-1)) -> maxi.c);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	/* Internal keyword, not fits restricted */
	sprintf(key,"TMI%i",i);
	sprintf(value,"%i", (hdrarray_+(i-1)) -> mini.c);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	break;
      case COLTYPE_INT:
	
	/* Internal keyword, not fits restricted */
	sprintf(key,"GRI%i",i);
	sprintf(value,"%li", (hdrarray_+(i-1)) -> grid.i);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	/* Internal keyword, not fits restricted */
	sprintf(key,"TMA%i",i);
	sprintf(value,"%li", (hdrarray_+(i-1)) -> maxi.i);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	/* Internal keyword, not fits restricted */
	sprintf(key,"TMI%i",i);
	sprintf(value,"%li", (hdrarray_+(i-1)) -> mini.i);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	break;
	
      case COLTYPE_DOUBLE:

	/* Internal keyword, not fits restricted */
	sprintf(key,"GRI%i",i);
	sprintf(value,"%.12E", (hdrarray_+(i-1)) -> grid.d);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	/* Internal keyword, not fits restricted */
	sprintf(key,"TMA%i",i);
	sprintf(value,"%.12E", (hdrarray_+(i-1)) -> maxi.d);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	/* Internal keyword, not fits restricted */
	sprintf(key,"TMI%i",i);
	sprintf(value,"%.12E", (hdrarray_+(i-1)) -> mini.d);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	break;
	
      default:

/* Internal keyword, not fits restricted */
	sprintf(key,"GRI%i",i);
	sprintf(value,"%.6E", (hdrarray_+(i-1)) -> grid.f);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	/* Internal keyword, not fits restricted */
	sprintf(key,"TMA%i",i);
	sprintf(value,"%.6E", (hdrarray_+(i-1)) -> maxi.f);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	/* Internal keyword, not fits restricted */
	sprintf(key,"TMI%i",i);
	sprintf(value,"%.6E", (hdrarray_+(i-1)) -> mini.f);
	comment[0] = '\0';
	qfits_header_add(header_,key,value,comment,"");
	
	break;
      }
    }
  }
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a full card image from a header */

int ftstab_getcard(char n, char *key, char *string, int what)
{
  char *line;
  char *grmbfxt;
  qfits_header *header;

  if (n == 0) {
    if (!(header = header_))
      return 0;
  }
  else {
    if (!(header = lastheader_))
      return 0;
  }


  if (!(line = qfits_header_getline(header, key)))
    return 0;

  switch(what) {

  case 0:
    sprintf(string, "%s", line);
    break;
  case 1:
    if (!(grmbfxt = qfits_getvalue(line))) {
    return 0;
    }
    sprintf(string, "%s", grmbfxt);
    break;
  case 2:
    if (!(grmbfxt = qfits_getcomment(line))) {
    return 0;
    }
    sprintf(string, "%s", grmbfxt);
    break;
  default:
   return 0;
    break;
  }
  return 1;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a full card image from a header */

int ftstab_getcard_(char *n, char *key, char *string, int *what)
{
  return ftstab_getcard(*n, key, string, *what);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Generate a header */

int ftstab_genhd_(char *n)
{
  return ftstab_genhd(*n);
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Delivers an int array of the length of the column number */

int *ftstab_get_intarr(void)
{
  int *array;
  if (!(array = (int *) malloc(ncolumns_*sizeof(int))))
    return NULL;
  return array;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Delivers a double array of the length of the column number */

double *ftstab_get_dblarr(void)
{
  double *array;
  if (!(array = (double *) malloc(ncolumns_*sizeof(double))))
    return NULL;
  return array;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the titles of the columns into an array */

int ftstab_get_title_(int *array)
{
  int i;
  for (i = 0; i < ncolumns_; ++i)
    array[i] = hdrarray_[i].titl;
  return i+1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the title number of column column */
int ftstab_get_coltit(int column)
{
  if ((column > 0) && (column <= ncolumns_))
    return hdrarray_[column-1].titl;
  return -1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the title number of column column */
int ftstab_get_coltit_(int *column)
{
  return ftstab_get_coltit(*column);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the identifier of the numerical type of column column */
int ftstab_get_coltyp(int column)
{
  if ((column > 0) && (column <= ncolumns_))
    return hdrarray_[column-1].type;
  return -1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the identifier of the numerical type of column column */
int ftstab_get_coltyp_(int *column)
{
  return ftstab_get_coltyp(*column);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the identifier of the numerical type of column column */
double ftstab_get_colrad(int column)
{
  if ((column > 0) && (column <= ncolumns_))
    return hdrarray_[column-1].radi;
  return DBL_MAX;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the identifier of the numerical type of column column */
double ftstab_get_colrad_(int *column)
{
  return ftstab_get_colrad(*column);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the identifier of the numerical type of column column */
double ftstab_get_colgrd(int column)
{
  if ((column > 0) && (column <= ncolumns_))
      switch(hdrarray_[column-1].type) {
    case COLTYPE_FLOAT:
      return hdrarray_[column-1].grid.f;
    case COLTYPE_CHAR:
      return hdrarray_[column-1].grid.c;
    case COLTYPE_INT:
      return hdrarray_[column-1].grid.i;
    case COLTYPE_DOUBLE:
      return hdrarray_[column-1].grid.d;
    default:
      return hdrarray_[column-1].grid.f;
  }
  return DBL_MAX;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* */
double ftstab_get_colgrd_(int *column)
{
  return ftstab_get_colgrd(*column);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the identifier of the numerical type of column column */
double ftstab_get_colmax(int column)
{
  if ((column > 0) && (column <= ncolumns_))
      switch(hdrarray_[column-1].type) {
    case COLTYPE_FLOAT:
      return hdrarray_[column-1].maxi.f;
    case COLTYPE_CHAR:
      return hdrarray_[column-1].maxi.c;
    case COLTYPE_INT:
      return hdrarray_[column-1].maxi.i;
    case COLTYPE_DOUBLE:
      return hdrarray_[column-1].maxi.d;
    default:
      return hdrarray_[column-1].maxi.f;
  }
  return DBL_MAX;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* */
double ftstab_get_colmax_(int *column)
{
  return ftstab_get_colmax(*column);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the identifier of the numerical type of column column */
double ftstab_get_colmin(int column)
{
  if ((column > 0) && (column <= ncolumns_))
  switch(hdrarray_[column-1].type) {
    case COLTYPE_FLOAT:
      return hdrarray_[column-1].mini.f;
    case COLTYPE_CHAR:
      return hdrarray_[column-1].mini.c;
    case COLTYPE_INT:
      return hdrarray_[column-1].mini.i;
    case COLTYPE_DOUBLE:
      return hdrarray_[column-1].mini.d;
    default:
      return hdrarray_[column-1].mini.f;
  }
  return DBL_MAX;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* */
double ftstab_get_colmin_(int *column)
{
  return ftstab_get_colmin(*column);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the numerical types of the columns into an array */

int ftstab_get_type_(int *array)
{
  int i;
  for (i = 0; i < ncolumns_; ++i)
    array[i] = hdrarray_[i].type;
  return i+1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the grid spacing of the columns into an array */

int ftstab_get_grid_(double *array)
{
  int i;
  for (i = 0; i < ncolumns_; ++i)
    switch(hdrarray_[i].type) {
    case COLTYPE_FLOAT:
      array[i] = hdrarray_[i].grid.f;
      break;
    case COLTYPE_CHAR:
      array[i] = hdrarray_[i].grid.c;
      break;
    case COLTYPE_INT:
      array[i] = hdrarray_[i].grid.i;
      break;
    case COLTYPE_DOUBLE:
      array[i] = hdrarray_[i].grid.d;
      break;
    default:
      array[i] = hdrarray_[i].grid.f;
      break;
    }
  return i+1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the radii of the columns into an array */

int ftstab_get_radi_(double *array)
{
  int i;
  for (i = 0; i < ncolumns_; ++i)
    array[i] = hdrarray_[i].radi;
  return i+1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the maxima of the columns into an array */

int ftstab_get_maxi_(double *array)
{
  int i;
  for (i = 0; i < ncolumns_; ++i)
    switch(hdrarray_[i].type) {
    case COLTYPE_FLOAT:
      array[i] = hdrarray_[i].maxi.f;
      break;
    case COLTYPE_CHAR:
      array[i] = hdrarray_[i].maxi.c;
      break;
    case COLTYPE_INT:
      array[i] = hdrarray_[i].maxi.i;
      break;
    case COLTYPE_DOUBLE:
      array[i] = hdrarray_[i].maxi.d;
      break;
    default:
      array[i] = hdrarray_[i].maxi.f;
      break;
    }
  return i+1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the minima of the columns into an array */

int ftstab_get_mini_(double *array)
{
  int i;
  for (i = 0; i < ncolumns_; ++i)
    switch(hdrarray_[i].type) {
    case COLTYPE_FLOAT:
      array[i] = hdrarray_[i].mini.f;
      break;
    case COLTYPE_CHAR:
      array[i] = hdrarray_[i].mini.c;
      break;
    case COLTYPE_INT:
      array[i] = hdrarray_[i].mini.i;
      break;
    case COLTYPE_DOUBLE:
      array[i] = hdrarray_[i].mini.d;
      break;
    default:
      array[i] = hdrarray_[i].mini.f;
      break;
    }
  return i+1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a whole row into an array */

int ftstab_get_row_(long *rownr, double *array)
{
  return ftstab_get_row(*rownr, array);
}

/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Dumps a row from a stream into an array but stays at the position */

static void getrowstay(double *array)
{
  int i;
  double *value;
  byte oneval[8];

  for (i = 0; i < ncolumns_; ++i) {
    switch(hdrarray_[i].type) {
    case COLTYPE_FLOAT:
      if(fread(oneval, COLBYTE_FLOAT, 1, stream_))
	;
      value = qfits_pixin_double(oneval, 1, BPP_IEEE_FLOAT, 1, 0);
      array[i] = *value;
      free(value);
      break;
    case COLTYPE_CHAR:
      if(fread(oneval, COLBYTE_CHAR, 1, stream_))
	;
      value = qfits_pixin_double(oneval, 1, BPP_8_UNSIGNED, 1, 0);
      array[i] = *value;
      free(value);
      break;
    case COLTYPE_INT:
      if(fread(oneval, COLBYTE_INT, 1, stream_))
	;
      value = qfits_pixin_double(oneval, 1, BPP_32_SIGNED, 1, 0);
      array[i] = *value;
      free(value);
      break;
    case COLTYPE_DOUBLE:
      if(fread(oneval, COLBYTE_DOUBLE, 1, stream_))
	;
      value = qfits_pixin_double(oneval, 1, BPP_IEEE_DOUBLE, 1, 0);
      array[i] = *value;
      free(value);
      break;
    default:
      if(fread(oneval, COLBYTE_DEFAULT, 1, stream_))
	;
      value = qfits_pixin_double(oneval, 1, BPP_IEEE_FLOAT, 1, 0);
      array[i] = *value;
      free(value);
      break;
    }
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a whole row into an array */

int ftstab_get_row(long rownr, double *array)
{
  fpos_t currentpos;

  /* check if the stream is present */
  if (!stream_) 
    return 0;

  /* check if the requested column is present */
  if (nrows_ < rownr || rownr < 1)
    return 0;

  /* Get the current position in the file */
  fgetpos(stream_, &currentpos);


  /* Get to the right position */
  fsetpos(stream_, &tablestart_);

  if (fseek(stream_, byteperow_*(rownr-1), SEEK_CUR)) {
    fsetpos(stream_, &currentpos);
    return 0;
  }

  /* Now go through the rows and put the contents into the array */
  getrowstay(array);
      
  fsetpos(stream_, &currentpos);
  return ncolumns_;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Reads a value but stays at the position */

static double getvalstay(int colnr)
{
  double val, *value;
  byte oneval[8];

    switch(hdrarray_[colnr].type) {
    case COLTYPE_FLOAT:
      if(fread(oneval, COLBYTE_FLOAT, 1, stream_))
	;
      value = qfits_pixin_double(oneval, 1, BPP_IEEE_FLOAT, 1, 0);
      val = *value;
      free(value);
      break;
    case COLTYPE_CHAR:
      if(fread(oneval, COLBYTE_CHAR, 1, stream_))
	;
      value = qfits_pixin_double(oneval, 1, BPP_8_UNSIGNED, 1, 0);
      val = *value;
      free(value);
      break;
    case COLTYPE_INT:
      if(fread(oneval, COLBYTE_INT, 1, stream_))
	;
      value = qfits_pixin_double(oneval, 1, BPP_32_SIGNED, 1, 0);
      val = *value;
      free(value);
      break;
    case COLTYPE_DOUBLE:
      if(fread(oneval, COLBYTE_DOUBLE, 1, stream_))
	;
      value = qfits_pixin_double(oneval, 1, BPP_IEEE_DOUBLE, 1, 0);
      val = *value;
      free(value);
      break;
    default:
      if(fread(oneval, COLBYTE_DEFAULT, 1, stream_))
	;
      value = qfits_pixin_double(oneval, 1, BPP_IEEE_FLOAT, 1, 0);
      val = *value;
      free(value);
      break;
    }
    return val;
}

/* ------------------------------------------------------------ */
      


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a value of a single field as double */

int ftstab_get_value(long rownr, int colnr, double *val)
{
  fpos_t currentpos;

  /* check if the stream is present */
  if (!stream_) 
    return 0;

  /* Get the current position in the file */
  fgetpos(stream_, &currentpos);

  /* check if the requested column is present */
  if (nrows_ < rownr)
    return 0;

  /* check if the requested column is present */
  if (ncolumns_ < colnr)
    return 0;

  /* Get to the right position */
  fsetpos(stream_, &tablestart_);

  if (fseek(stream_, byteperow_*(rownr-1)+byteoffset_[colnr-1], SEEK_CUR)) {
    fsetpos(stream_, &currentpos);
    return 0;
  }


  *val = getvalstay(colnr-1);

  fsetpos(stream_, &currentpos);
  return ncolumns_;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a value of a single field as double */

int ftstab_get_value_(long *rownr, int *colnr, double *val)
{
  return ftstab_get_value(*rownr, *colnr, val);
}
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns a Rowdesc array read from a fits file */

static qfits_header *readfilehead(char *filename, int next, Rowdesc **descriptor, int *ncolumns)
{
  qfits_header *header;
  int i;
  long nbytes = 0;
  multype dummy;

  /* We can read the header */
  if (!(header = qfits_header_readext(filename, next)))
    return NULL;

  /* The column number is this, it has been checked for */
  *ncolumns = qfits_header_getint(header, "TFIELDS", -1);

  /* Reserve the space */
  if (!(*descriptor = (Rowdesc *) malloc(*ncolumns * sizeof(Rowdesc)))) {
    qfits_header_destroy(header);
    return NULL;
  }

  /* Fill it from the header, checking for errors */
  for (i = 1; i <= *ncolumns; ++i) {
    
    /* Each of those must be present and ok */

    /* column name */
    if (!numberget(header, 0, i, &dummy)){
      goto error;
    }
    (*descriptor)[i-1].titl = dummy.i;

    /* numerical type */
    if (!numberget(header, 1, i, &dummy)) {
      goto error;
    }
    (*descriptor)[i-1].type = dummy.i;

    switch((*descriptor)[i-1].type) {
    case COLTYPE_FLOAT:
      nbytes = nbytes+COLBYTE_FLOAT;
      break;
    case COLTYPE_CHAR:
      nbytes = nbytes+COLBYTE_CHAR;
      break;
    case COLTYPE_INT:
      nbytes = nbytes+COLBYTE_INT;
      break;
    case COLTYPE_DOUBLE:
      nbytes = nbytes+COLBYTE_DOUBLE;
      break;
    default:
      nbytes = nbytes+COLBYTE_DEFAULT;
      break;
    }

    /* radius */
    if (!numberget(header, 2, i, &dummy)) {
      goto error;
    }
    (*descriptor)[i-1].radi = dummy.d;

    /* grid spacing */
    if (!numberget(header, 3, i, &dummy)) {
      goto error;
    }
    switch((*descriptor)[i-1].type) {
    case COLTYPE_FLOAT:
      (*descriptor)[i-1].grid.f = dummy.d;
      break;
    case COLTYPE_CHAR:
      (*descriptor)[i-1].grid.c = dummy.d;
      break;
    case COLTYPE_INT:
      (*descriptor)[i-1].grid.i = dummy.d;
      break;
    case COLTYPE_DOUBLE:
      (*descriptor)[i-1].grid.d = dummy.d;
      break;
    default:
      (*descriptor)[i-1].grid.f = dummy.d;
      break;
    }

    /* maximum */
    if (!numberget(header, 4, i, &dummy)) {
      goto error;
    }
    switch((*descriptor)[i-1].type) {
    case COLTYPE_FLOAT:
      (*descriptor)[i-1].maxi.f = dummy.d;
      break;
    case COLTYPE_CHAR:
      (*descriptor)[i-1].maxi.c = dummy.d;
      break;
    case COLTYPE_INT:
      (*descriptor)[i-1].maxi.i = dummy.d;
      break;
    case COLTYPE_DOUBLE:
      (*descriptor)[i-1].maxi.d = dummy.d;
      break;
    default:
      (*descriptor)[i-1].maxi.f = dummy.d;
      break;
    }

    /* minimum */
    if (!numberget(header, 5, i, &dummy)) {
      goto error;
    }
    switch((*descriptor)[i-1].type) {
    case COLTYPE_FLOAT:
      (*descriptor)[i-1].mini.f = dummy.d;
      break;
    case COLTYPE_CHAR:
      (*descriptor)[i-1].mini.c = dummy.d;
      break;
    case COLTYPE_INT:
      (*descriptor)[i-1].mini.i = dummy.d;
      break;
    case COLTYPE_DOUBLE:
      (*descriptor)[i-1].mini.d = dummy.d;
      break;
    default:
      (*descriptor)[i-1].mini.f = dummy.d;
      break;
    }

  }

  /* Now nearly everything seems ok and everything is read, only is this right...? */
  if (nbytes != qfits_header_getint(header, "NAXIS1", -1)) {
    goto error;
  }

  /* We're through */ 
  return header;

 error:
  free((*descriptor));
  qfits_header_destroy(header);
  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Opens a table fits file */

int ftstab_fopen(char *filename, int next, char mode, char hheader)
{
  int i;
  long offset;
  int ncolumns;
  Rowdesc *descriptorarray;
  qfits_header *header;

  /* The very first thing is to initialise the hdr array */
  if (!(hdrlist_.n)) {
    if (!hdl_init())
      return 22;
  }

  /* OK, first thing is to check whether there is a file in procession */
  if (stream_)
    return 1;

  /* Next thing: is there a file with that name */

  /* In case there is none */
  if (!(stream_ = fopen(filename, "r"))) {

    /* Then we create one for a given information present */
    if (!(ftstab_open_(filename)))
      return 2;
  }

  /* there is a file with the given name */
  else {
    fclose(stream_);
    stream_ = NULL;
    
    if (mode == 0)
      return 1;
    
    /* If the user wants the file to be overwritten */
    else if (mode == 1) {
      
      /* Then we create one for a given information present */
      if (!(ftstab_open_(filename)))
	return 3;
    }
    
    /* Now things get complicated, mode is append */
    else if (mode == 2) {
 
 
 /* First we check if there is any information present that has to be checked */
      
      /* The header array is present, so we have to check for consistency */
      if (hdrarray_) {
 
	/* Check the basics, will return 0 if all is well, and 4 if the hdu will have to be attached */
	if (!(i = checkfilefts(filename, next, hheader))) {
	  
	  /* all is well up to now, we have to check further, read the header and the information in a Rowdesc array */
	  if (!(header = readfilehead(filename, next, &descriptorarray, &ncolumns)))
	    return 6;
	  /* Now we have an allocated header and the descriptorarray that can be checked against the present one */
	  if (checkhdarray(descriptorarray, ncolumns) != 1) {
	    qfits_header_destroy(header);
	    free(descriptorarray);
	    return 7;
	  }
	  
	  /* Everything is ok, we can read the file */
	  if (!checkin(filename, header, descriptorarray, next, hheader))
	    return 8;
	}
	else if (i == 4) {
	  
	  /* We don't have to check, the hdu can be appended */
	  if (!(header_)) {
	    ftstab_genhd(0);
	  }
	  header = header_;
	  
	  /* Take care that the headerarray is passed and not identical */
	  descriptorarray = hdrarray_;
	  hdrarray_ = NULL;
	  
	  /* Everything is ok, we can read the file */
	  if (!checkin(filename, header, descriptorarray, next, hheader))
	    return 8;
	  
	}
	else {
	  return 5;
	}
	
      }
      
      /* No header array present, we want to create one */
      else {
 

	/* Check the basics, will return 0 if all is well, and 4 if the hdu will have to be attached */
	if (!(checkfilefts(filename, next, hheader))) {

	  /* all is well up to now, we have to check further, read the header and the information in a Rowdesc array */

	  /* Error occurs here */
	  if (!(header = readfilehead(filename, next, &descriptorarray, &ncolumns)))
	    return 6;

	  /* Everything is ok, we can read the file */
	  if (!checkin(filename, header, descriptorarray, next, hheader))
	    return 8;
	}
	else {
	  return 5;
	}
      }
    }

    /* Mode is append enforce write*/
    else if (mode == 3) {

      /* First we check if there is any information present that has to be checked */

      /* The header array is present, so we have to check for consistency */
      if (hdrarray_) {
	/* Check the basics, will return 0 if all is well, and 4 if the hdu will have to be attached */
	if (!(i = checkfilefts(filename, next, hheader))) {

	  /* all is well up to now, we have to check further, read the header and the information in a Rowdesc array */
	  if (!(header = readfilehead(filename, next, &descriptorarray, &ncolumns))) {
	    if (!(ftstab_open_(filename)))
	      return 9;
	    else return 0;
	  }

	  /* Now we have an allocated header and the descriptorarray that can be checked against the present one */
	  if (checkhdarray(descriptorarray, ncolumns) != 1) {
	    qfits_header_destroy(header);
	    free(descriptorarray);
	    if (!(ftstab_open_(filename)))
	      return 10;
	    else return 0;
	  }
	  /* Everything is ok, we can read the file */
	  if (!checkin(filename, header, descriptorarray, next, hheader))
	    return 8;

	}
	else if (i == 4) {

	  /* We don't have to check, the hdu can be appended */
	  if (!(header_)) {
	    ftstab_genhd(0);
	    header = header_;
	  }
	    header = header_;
	  /* Take care that the headerarray is passed and not identical */
	  descriptorarray = hdrarray_;
	  hdrarray_ = NULL;

	  /* Everything is ok, we can read the file */
	  if (!checkin(filename, header, descriptorarray, next, hheader))
	    return 8;

	}
	else {
	  /* unfortunately, there is an error doing this */
	    if (!(ftstab_open_(filename)))
	      return 11;
	    else return 0;
	}

      }

      /* No header array present, we want to create one */
      else {
	/* Check the basics, will return 0 if all is well, and 4 if the hdu will have to be attached */
	if (!(checkfilefts(filename, next, hheader))) {

	  /* all is well up to now, we have to check further, read the header and the information in a Rowdesc array */
	  if (!(header = readfilehead(filename, next, &descriptorarray, &ncolumns)))
	    return 12;

	  /* Everything is ok, we can read the file */
	  if (!checkin(filename, header, descriptorarray, next, hheader))
	    return 13;
	}
	else 
	  return 14;
      }
    }

    /* Mode is append enforce */
    else if (mode == 4) {

      /* First we check if there is any information present that has to be checked */

      /* The header array is present, so we have to check for consistency */
      if (hdrarray_) {

	/* Check the basics, will return 0 if all is well, and 4 if the hdu will have to be attached */
	if (!(i = checkfilefts(filename, next, hheader))) {

	  /* all is well up to now, we have to check further, read the header and the information in a Rowdesc array */
	  if (!(header = readfilehead(filename, next, &descriptorarray, &ncolumns))) {
	    /* unfortunately, there is an error doing this, nevertheless we will append and delete the rest */
	    if (!(header_)) {
	      ftstab_genhd(0);
	    }
	      header = header_;
	    /* Take care that the headerarray is passed and not identical */
	    descriptorarray = hdrarray_;
	    hdrarray_ = NULL;
	    
	    /* Everything is ok, we can read the file */
	    if (!checkin(filename, header, descriptorarray, next, hheader))
	      return 16;
	    
	    /* Now delete everything at the end */
	    offset = ftell(stream_);
	    if(ftruncate(fileno(stream_), offset))
	      ;
	    clearerr(stream_);

	    /* The blocking can be removed */
	    headerblock_ = 0;
	    tableblock_ = 0;
	    return 0;
	  }

	  /* Now we have an allocated header and the descriptorarray that can be checked against the present one */
	  if (checkhdarray(descriptorarray, ncolumns) != 1) {
	    /* unfortunately, there is an error doing this, nevertheless we will append and delete the rest */
	    if (!(header_)) {
	      ftstab_genhd(0);
	    }
	      header = header_;
	    /* Take care that the headerarray is passed and not identical */
	    descriptorarray = hdrarray_;
	    hdrarray_ = NULL;
	    
	    /* Everything is ok, we can read the file */
	    if (!checkin(filename, header, descriptorarray, next, hheader))
	      return 17;
	    
	    /* Now delete everything at the end */
	    offset = ftell(stream_);
	    if(ftruncate(fileno(stream_), offset))
	      ;
	    clearerr(stream_);

	    /* The blocking can be removed */
	    headerblock_ = 0;
	    tableblock_ = 0;
	    return 0;
	  }

	  /* Everything is ok, we can read the file */
	  if (!checkin(filename, header, descriptorarray, next, hheader))
	    return 18;
	}
	else if (i == 4) {

	  /* We don't have to check, the hdu can be appended */
	  if (!(header_)) {
	    ftstab_genhd(0);
	  }
	    header = header_;

	  /* Take care that the headerarray is passed and not identical */
	  descriptorarray = hdrarray_;
	  hdrarray_ = NULL;

	  /* Everything is ok, we can read the file */
	  if (!checkin(filename, header, descriptorarray, next, hheader))
	    return 8;

	}
	else {

	  /* unfortunately, there is an error doing this, nevertheless we will append and delete the rest */
	    if (!(header_)) {
	    ftstab_genhd(0);
	  }
	    header = header_;
	  /* Take care that the headerarray is passed and not identical */
	  descriptorarray = hdrarray_;
	  hdrarray_ = NULL;

	  /* Everything is ok, we can read the file */
	  if (!checkin(filename, header, descriptorarray, next, hheader))
	    return 15;

	  /* Now delete everything at the end */
	  offset = ftell(stream_);
	  if(ftruncate(fileno(stream_), offset))
	    ;
	  clearerr(stream_);
	  /* The blocking can be removed */
	  headerblock_ = 0;
	  tableblock_ = 0;
	  return 0;
	}

      }

      /* No header array present, we want to create one */
      else {
	/* Check the basics, will return 0 if all is well, and 4 if the hdu will have to be attached */
	if (!(checkfilefts(filename, next, hheader))) {

	  /* all is well up to now, we have to check further, read the header and the information in a Rowdesc array */
	  if (!(header = readfilehead(filename, next, &descriptorarray, &ncolumns)))
	    return 20;

	  /* Everything is ok, we can read the file */
	  if (!checkin(filename, header, descriptorarray, next, hheader))
	    return 21;
	}
	else 
	  return 19;
      }
    
  }
    /* The wrong mode is given */
    else 
      return 4;


  }
  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Opens a table fits file */
static int checkfilefts(char *filename, int next, char hheader)
{
  qfits_header *header;
  char *key;
  int dummy = 0;
/*   int ncolumns; */

  /* First check if there is such a file and if it is a fits */
  if (qfits_is_fits(filename) == -1)
    return 1;

  if (qfits_is_fits(filename) == 0)
    return 2;

  /* Check if there is a history header if that could be a problem */
  if (!(hheader))
    ;
  else {
    if ((header = checkhheader(filename))) {
      qfits_header_destroy(header);
      dummy = 1;
    }
  }

  if (qfits_query_n_ext(filename) < (next+dummy))
    return 4;

  if (!(header = qfits_header_readext(filename, next)))
    return 9;

  /* Is the extension a binary table etc. */
  if (!(key = qfits_header_getstr(header, "XTENSION"))) {
    goto error8;
  }

  if (strcmp("'BINTABLE'", key))
    goto error5;

  if (!(key = qfits_header_findmatch(header, "END")))
    goto error8;

  if (!strcmp("", key))
    goto error5;

  if (!(key = qfits_header_getstr(header, "BITPIX")))
    goto error8;
  if (strcmp("8", key))
    goto error5;

  if (!(key= qfits_header_getstr(header, "NAXIS")))
    goto error8;

  if (strcmp("2", key))
    goto error5;

  if (!(key= qfits_header_getstr(header, "NAXIS2")))
    goto error8;

  /* PCOUNT is the number of bytes of data after the main table */
  if (!(key= qfits_header_getstr(header, "PCOUNT")))
    goto error8;

  if (strcmp("0", key))
    goto error5;

  if (!(key= qfits_header_getstr(header, "GCOUNT")))
    goto error8;

  if (strcmp("1", key))
    goto error5;

  /* The number of fields in each row (Columns) */
  if (!(key= qfits_header_getstr(header, "TFIELDS")))
    goto error8;

  /* If this number doesn't exist or is too low, this is an inconsistency */
  if (qfits_header_getint(header, "TFIELDS", -1) <= 0) {
    qfits_header_destroy(header);
    return 7;
  }

  /* If ncolumns_ is not 0, TFIELDS must be the number of columns */
  else if (ncolumns_) {
    if (qfits_header_getint(header, "TFIELDS", -1) != ncolumns_) {
      qfits_header_destroy(header);
      return 7;
    }
  }

  /* The number of bytes in a table row */
  if (!(key= qfits_header_getstr(header, "NAXIS1")))
    goto error8;

  qfits_header_destroy(header);

  return 0;

 error5:
  qfits_header_destroy(header);
  return 8;

 error8:
  qfits_header_destroy(header);
  return 6;

}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns the identifyer of an item in the column of a header */

static int numberget(qfits_header *header, int item, int column, multype *number)
{
  char *value;
  char keyword[9];

  switch(item) {

    /* The column name as defined in ftstab.h */
  case 0:
    /* Get the value of the key */
    sprintf(keyword,"TIT%i", column);
    if (!(value = qfits_header_getstr(header, keyword))) {
      return 0;
    }

    /* Check if the name is a valid name */
    if ((number -> i = ftstab_gltitln_(value)) < 0) {
      return 0;
    }

    /* Check if the units and types are consistent */
    if (!check_namenumber(number -> i, column, header)) {
      return 0;
    }
    break;

    /* The column numerical type as defined in ftstab.h */
  case 1:
    sprintf(keyword,"TFO%i", column);
    if (!(value = qfits_header_getstr(header, keyword)))
      return 0;
    if (!strcmp("'1J      '", value))
      number -> i = COLTYPE_INT;
    else if (!strcmp("'1E      '", value))
      number -> i = COLTYPE_FLOAT;
    else if (!strcmp("'1D      '", value))
      number -> i = COLTYPE_DOUBLE;
    else if (!strcmp("'1B      '", value))
      number -> i = COLTYPE_CHAR;
    else {
      return 0;
    }
    break;

    /* The radius */
  case 2:
    sprintf(keyword,"RAD%i", column);
    if (!(value = qfits_header_getstr(header, keyword)))
      return 0;
    number -> d = qfits_header_getdouble(header, keyword, -1.0);
    break;

    /* The grid spacing */
  case 3:
    sprintf(keyword,"GRI%i", column);
    if (!(value = qfits_header_getstr(header, keyword)))
      return 0;
    number -> d = qfits_header_getdouble(header, keyword, 0.0);
    break;

    /* The maximum */
  case 4:
    sprintf(keyword,"TMA%i", column);
    if (!(value = qfits_header_getstr(header, keyword)))
      return 0;
    number -> d = qfits_header_getdouble(header, keyword, 0.0);
    break;

    /* The minimum */
  case 5:
    sprintf(keyword,"TMI%i", column);
    if (!(value = qfits_header_getstr(header, keyword)))
      return 0;
    number -> d = qfits_header_getdouble(header, keyword, 0.0);
    break;

  default:
    return 0;
  }

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Checks for ftstab consistency for the given key in the header */

static int check_namenumber(int key, int column, qfits_header *header)
{
  char unit[9], tscalarray[21];
  char *uvalue, *tvalue, *urvalue, *trvalue, *rscal, *rzero;
  double tzeroread, tscalread;
  
  /* If the header is empty this is an error */
  if (!(header)) {
    return 0;
  }

  /* Load the associated values if possible */
  sprintf(unit,"TTY%i", column);
  if (!(tvalue = qfits_header_getstr(header, unit))) {
    return 0;
  }
  sprintf(unit,"TUN%i", column);
  if (!(uvalue = qfits_header_getstr(header, unit))) {
    return 0;
  }
  sprintf(unit,"TSC%i", column);
  if (!(rscal = qfits_header_getstr(header, unit))) {
    return 0;
  }
  sprintf(unit,"TZE%i", column);
  if (!(rzero = qfits_header_getstr(header, unit))) {
    return 0;
  }

  /* This is not most elegant, and also not exact, but it should do */
  tzeroread = ftstab_gtzero(key);
  sprintf(tscalarray,"%.12E", tzeroread);
  if (strcmp(tscalarray, rzero)) {
     return 0;
  }

  tscalread = ftstab_gtscal(key);
  sprintf(tscalarray,"%.12E", tscalread);
  if (strcmp(tscalarray, rscal)) {
    return 0;
  }
  
  if (!(trvalue = ftstab_gltype(key))) {
        return 0;
  }

  if (!(urvalue = ftstab_glunit(key))) {
    free(trvalue);
    return 0;
  }


  if (strcmp(tvalue,trvalue) || strcmp(uvalue,urvalue)) {
    free(trvalue);
    free(urvalue);

    return 0;
  }

  free(trvalue);
  free(urvalue);
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Update an old header with a new card */

int ftstab_putcard(char n, char *keyl, char *valuel)
{
  qfits_header *new, *old;
  int i;
  char headerblock = 1;
  char key[9],value[72],comment[72], appendline[9], *string, string2[9];

  if ((n)) {
    old = lastheader_;
    if (!old)
      return 0;
    if (headerblock_)
      headerblock_ = headerblock = !headerblock_;
  }
  else {
    old = header_;
    if (!old)
      return 0;
  }

  /* Make one header with the value */
  if (!(new = qfits_header_new()))
    return 0;
  qfits_header_append(new, keyl, valuel, NULL, NULL);

  /* Now we think that we can append new to old */
  
  /* check for the first comment in old */
  for (i = 0; i < old -> n; ++i) {
    qfits_header_getitem(old, i, key, NULL, NULL, NULL);
    if (!(strcmp(key,"COMMENT") && strcmp(key,"HISTORY") && strcmp(key,""))) {
      break;
    }
  }
  
  /* If we got to the end we must take one step back */
  if (i == old -> n)
    --i;


  /* This is the line where things are appended if they are not a comment */
  qfits_header_getitem(old, i-1, appendline, NULL, NULL, NULL);

  /* Now go through new */
  for (i = 0; i < new -> n; ++i) {
	qfits_header_getitem(new, i, key, value, comment, NULL);

	if((string = qfits_header_findmatch(old, key))){
	  sprintf(string2,"%s",string);
	}
	else {
	  /* Originally, we had the following line, which has been replaced */
	  /* sprintf(string2, "!@#$%^%$"); */
	  sprintf(string2, "        ");
	}

	/* if we encounter a comment the whole card will be appended to the end of old */
	if (!(strcmp(key,"COMMENT") && strcmp(key,"HISTORY") && strcmp(key,""))) {

	  /* qfits reads the comment as a value */    
	  if (headerblock_) {
	    qfits_header_destroy(new);
	    return 0;
	  }
	  qfits_header_add(old, key, value, NULL, NULL);
	}
	/* Everything else is being modified */

	/* if we find the matching key replace it */
	else if (!strcmp(string2, key)) {
	    qfits_header_mod(old, key, value, comment);
	}

	/* if we don't find it we append it to the body containing valuable information */
	else {
	  if (headerblock_) {
	    qfits_header_destroy(new);
	    return 0;
	  }
	  qfits_header_add_after(old, appendline, key, value, comment, NULL);
	}
  }

  /* That's it, care for headerblock */
  if (!headerblock)
    headerblock_ = !headerblock_;

  qfits_header_destroy(new);
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Add a card to a header */
int ftstab_putcard_(char *n, char *keyl, char *valuel)
{
  return ftstab_putcard(*n, keyl, valuel);
}


/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a char array belonging to the COLTITL titl */

char *ftstab_gtitl(int titl)
{
  char *gtitl;

  if (!(gtitl = (char *) malloc(21*sizeof(char))))
    return NULL;
  
  if (ftstab_putcoltitl(gtitl, titl) == -1) {
  free(gtitl);
  return NULL;
  }
  else
    return gtitl;
}

/* ------------------------------------------------------------ */




/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*Get a char array belonging to the COLTITL titl */
char *ftstab_gltitl(int titl)

{
  char *gtitl;

  if (!(gtitl = (char *) malloc(21*sizeof(char))))
    return NULL;
    if (ftstab_putlcoltitl(gtitl, titl) == -1) {
  free(gtitl);
  return NULL;
  }
  else
    return gtitl;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a char array belonging to the COLTITL titl */

char *ftstab_gtype(int titl)
{
  char *gtitl;

  if (!(gtitl = (char *) malloc(21*sizeof(char))))
    return NULL;
  
  if (ftstab_putcoltype(gtitl, titl) == -1) {
    free(gtitl);
    return NULL;
  }
  else
    return gtitl;
 }
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/*Get a char array belonging to the COLTITL titl*/

char *ftstab_gltype(int titl)
{
  char *gtitl;

  if (!(gtitl = (char *) malloc(21*sizeof(char))))
    return NULL;
  
  if (ftstab_putlcoltype(gtitl, titl) == -1) {
    free(gtitl);
    return NULL;
  }
  else
    return gtitl;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*Get a char array belonging to the COLTITL titl*/

char *ftstab_gunit(int titl)
{
  char *gtitl;

  if (!(gtitl = (char *) malloc(21*sizeof(char))))
    return NULL;
  
  if (ftstab_putcolunit(gtitl, titl) == -1) {
    free(gtitl);
    return NULL;
  }
  else
    return gtitl;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a char array belonging to the COLTITL titl */

char *ftstab_glunit(int titl)
{
  char *gtitl;

  if (!(gtitl = (char *) malloc(21*sizeof(char))))
    return NULL;

  if (ftstab_putlcolunit(gtitl, titl) == -1) {
    free(gtitl);
    return NULL;
  }
  else
    return gtitl;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*Get COLTITL number from string*/
int ftstab_gtitln_(char *titl)
{
  int i;
  char *dummy = NULL;

  if (!hdrlist_.n)
    return -1;

  if(!(titl))
    return -1;

  for (i = 0; i < hdrlist_.n; ++i) {
    if (!(dummy = ftstab_gtitl(i)))
      return -1;
    if (!strcmp(dummy, titl))
      break;
  }
  if (dummy)
    free(dummy);

  if (i == hdrlist_.n)
    return -1;

  return i;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get COLTITL number from string */
 
int ftstab_gltitln_(char *titl)
{
    int i;
  char *dummy = NULL;
  if (!hdrlist_.n)
    return -1;

  if(!(titl))
    return -1;

  for (i = 0; i < hdrlist_.n; ++i) {
    if (!(dummy = ftstab_gltitl(i)))
      return -1;

    if (!strcmp(dummy, titl))
      break;
  }
  if (dummy)
    free(dummy);

  if (i == hdrlist_.n)
    return -1;

  return i;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */

int ftstab_putcoltitl_(char *key, int *coltitle)
{
  return ftstab_putcoltitl(key, *coltitle);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */

int ftstab_putcoltitl(char *key, int coltitle)
{
  hdrit *item;
  int i;

  /* Go to the right key */
  if (!(coltitle < hdrlist_.n) || !(coltitle >= 0))
    return -1;

  item = hdrlist_.first;
  for (i = 0; i < coltitle; ++i)
    item = item -> next;
  
  sprintf(key,"%s", item -> titl);
  return coltitle;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */

int ftstab_putlcoltitl_(char *key, int *coltitle)
{
  return ftstab_putlcoltitl(key, *coltitle);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */

int ftstab_putlcoltitl(char *key, int coltitle)
{
  hdrit *item;
  int i;

  /* Go to the right key */
  if (!(coltitle < hdrlist_.n) || !(coltitle >= 0))
    return -1;

  item = hdrlist_.first;
  for (i = 0; i < coltitle; ++i) {
    item = item -> next;
  }
  sprintf(key,"'%s", item -> titl);
  /* Now it gets a bit tricky, as we have to fill up the rest */
  i = (int) strlen(key);

  /* We point to the terminating char now */
  while (i < 9) {
    key[i] = ' ';
    ++i;
  }
  key[i] = '\'';
  key[i+1] = '\0';

  return coltitle;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */

int ftstab_putcoltype_(char *key, int *coltitle)
{
  return ftstab_putcoltype(key, *coltitle);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */

int ftstab_putcoltype(char *key, int coltitle)
{
  hdrit *item;
  int i;

  /* Go to the right key */
  if (!(coltitle < hdrlist_.n) || !(coltitle >= 0))
    return -1;

  item = hdrlist_.first;
  for (i = 0; i < coltitle; ++i)
    item = item -> next;
  
  sprintf(key,"%s", item -> ttype);
  return coltitle;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */

int ftstab_putlcoltype_(char *key, int *coltitle)
{
  return ftstab_putlcoltype(key, *coltitle);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */

int ftstab_putlcoltype(char *key, int coltitle)
{
  hdrit *item;
  int i;

  /* Go to the right key */
  if (!(coltitle < hdrlist_.n) || !(coltitle >= 0))
    return -1;

  item = hdrlist_.first;
  for (i = 0; i < coltitle; ++i) {
    item = item -> next;
  }
  sprintf(key,"'%s", item -> ttype);
  /* Now it gets a bit tricky, as we have to fill up the rest */
  i = (int) strlen(key);

  /* We point to the terminating char now */
  while (i < 9) {
    key[i] = ' ';
    ++i;
  }
  key[i] = '\'';
  key[i+1] = '\0';

  return coltitle;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */

int ftstab_putcolunit_(char *key, int *coltitle)
{
  return ftstab_putcolunit(key, *coltitle);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */

int ftstab_putcolunit(char *key, int coltitle)
{
  hdrit *item;
  int i;

  /* Go to the right key */
  if (!(coltitle < hdrlist_.n) || !(coltitle >= 0))
    return -1;

  item = hdrlist_.first;
  for (i = 0; i < coltitle; ++i)
    item = item -> next;
  
  sprintf(key,"%s", item -> tunit);
  return coltitle;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */

double ftstab_gtscal_(int *coltitle)
{
  return ftstab_gtscal(*coltitle);
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a tscale belonging to the COLTITL titl */

double ftstab_gtscal(int coltitle)
{
  hdrit *item;
  int i;

  /* Go to the right key */
  if (!(coltitle < hdrlist_.n) || !(coltitle >= 0))
    return DBL_MAX;

  item = hdrlist_.first;
  for (i = 0; i < coltitle; ++i) {
    item = item -> next;
  }

  return item -> tscal;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a tzero belonging to the COLTITL titl */

double ftstab_gtzero_(int *coltitle)
{
  return ftstab_gtzero(*coltitle);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get a tscale belonging to the COLTITL titl */

double ftstab_gtzero(int coltitle)
{
  hdrit *item;
  int i;

  /* Go to the right key */
  if (!(coltitle < hdrlist_.n) || !(coltitle >= 0))
    return DBL_MAX;

  item = hdrlist_.first;
  for (i = 0; i < coltitle; ++i) {
    item = item -> next;
  }

  return item -> tzero;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */
int ftstab_putlcolunit_(char *key, int *coltitle)
{
  return ftstab_putlcolunit(key, *coltitle);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts the string that belongs to coltitle into key */
int ftstab_putlcolunit(char *key, int coltitle)
{
  hdrit *item;
  int i;

  /* Go to the right key */
  if (!(coltitle < hdrlist_.n) || !(coltitle >= 0))
    return -1;

  item = hdrlist_.first;
  for (i = 0; i < coltitle; ++i) {
    item = item -> next;
  }
  sprintf(key,"'%s", item -> tunit);
  /* Now it gets a bit tricky, as we have to fill up the rest */
  i = (int) strlen(key);

  /* We point to the terminating char now */
  while (i < 9) {
    key[i] = ' ';
    ++i;
  }
  key[i] = '\'';
  key[i+1] = '\0';

  return coltitle;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the number of the current extension */

int ftstab_get_extnr_(void)
{
  return curext_;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Get the number of the current extension */

static int checkhdarray(Rowdesc *temp, int ncolumns)
{
  int i;
  
  if (!hdrarray_) {
    return 0;
  }
  
  /* Check for the existence of temp */
  if (!temp) {
    return 2;
  }
  
  /* Check the column numbers */
  if (ncolumns != ncolumns_)
    return 2;
  
  /* Check the items */
  for (i = 0; i < ncolumns_; ++i) {
    
    /* titl must be consistent */
    if (hdrarray_[i].titl != temp[i].titl)
      return 2;
    
    /* numerical type must be consistent */
    if (hdrarray_[i].type != temp[i].type)
      return 2;
    
    /* Radius must be consistent */
    if (hdrarray_[i].radi != temp[i].radi)
      return 2;
    
    /* Grid spacing must be consistent */
    switch(hdrarray_[i].type) {
    case COLTYPE_FLOAT:
      if (hdrarray_[i].grid.f != temp[i].grid.f)
	return 2;
      break;
    case COLTYPE_CHAR:
      if (hdrarray_[i].grid.c != temp[i].grid.c)
	return 2;
      break;
    case COLTYPE_INT:
      if (hdrarray_[i].grid.i != temp[i].grid.i)
	return 2;
      break;
    case COLTYPE_DOUBLE:
      if (hdrarray_[i].grid.d != temp[i].grid.d)
	return 2;
      break;
    }
   }

  /* That was that */
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Checks in the file with an acquired header and Rowdesc table */

static int checkin(char *filename, qfits_header *header, Rowdesc *temp, int next, char hheader)
{
  int nextinfile, i;
  qfits_header *lastheader = NULL;
  int header_startpos;	    
  long offset;

  /* How many extensions does it contain */
  nextinfile = qfits_query_n_ext(filename);

  /* Get the last header */
  if (hheader) {
    lastheader = checkhheader(filename);
  }

  /* We want to get the position information */
  
  /* Check if the desired extension is later than the last one */
  if (nextinfile < next) {
    
    if ((lastheader)) {
      qfits_get_hdrinfo(filename, nextinfile, &header_startpos, NULL);
      next = nextinfile;
    }
    else {
      /* The header will be attached to the end */
      header_startpos = -1;
      next = nextinfile+1;
    }
  }
  else {
    if ((lastheader)) {
      if (next >= (nextinfile -1)) {
	qfits_get_hdrinfo(filename, next, &header_startpos, NULL);
	++next;
	tableblock_ = 0;
      }
      else {
	tableblock_ = 1;
	qfits_get_hdrinfo(filename, next, &header_startpos, NULL);
      } 
    }
    else {
    if (next >= (nextinfile)) {
	qfits_get_hdrinfo(filename, next, &header_startpos, NULL);
	++next;
	tableblock_ = 0;
      }
      else {
	tableblock_ = 1;
	qfits_get_hdrinfo(filename, next, &header_startpos, NULL);
      } 
    }
    
    /* Former content of the else */
/*     if ((lastheader) && (next >= (nextinfile -1))) { */
/*     qfits_get_hdrinfo(filename, next, &header_startpos, NULL); */
/*       ++next; */
/*       tableblock_ = 0; */
/*     } */
/*     else { */
/*       tableblock_ = 1; */
/*       qfits_get_hdrinfo(filename, next, &header_startpos, NULL); */
/*     } */

  }

  /* Now fill all the information from the header and the Rowdesc in the statics */

  ncolumns_ = qfits_header_getint(header, "TFIELDS", 0);

  /* Create and fill the byte offset array */
  if (ncolumns_) {
    if (byteoffset_)
      ;
    else {
      if (!(byteoffset_ = (long *) malloc((ncolumns_)*sizeof(long)))){
	return 0;
      }
    }

  byteoffset_[0] = 0;
  }
  if (ncolumns_) {

	  
    for (i = 0; i < ncolumns_-1; ++i) {
      switch(temp[i].type) {
      case COLTYPE_FLOAT:
	byteoffset_[i+1] = byteoffset_[i]+COLBYTE_FLOAT;
	break;
      case COLTYPE_CHAR:
	byteoffset_[i+1] = byteoffset_[i]+COLBYTE_CHAR;
	break;
      case COLTYPE_INT:
	byteoffset_[i+1] = byteoffset_[i]+COLBYTE_INT;
	break;
      case COLTYPE_DOUBLE:
	byteoffset_[i+1] = byteoffset_[i]+COLBYTE_DOUBLE;
	break;
      default:
	byteoffset_[i+1] = byteoffset_[i]+COLBYTE_DEFAULT;
	break;
      }
    }
  }

  if ((nrows_ = qfits_header_getint(header, "NAXIS2", 0)))
    headerblock_ = 1;

  curext_ = next;

  if (header_)
      qfits_header_update(header_, header);
  else
    header_ = header;

  /* Update the byteperow value to 0 */
  byteperow_ = 0;
  
  /* Open the file, something strange is happening here maybe */
  if (!(stream_ = fopen(filename, "r+"))) {
    if ((lastheader))
      qfits_header_destroy(lastheader);
    return 0;
  }

  /* Set the buffer to 0 */
/*   setbuf(stream_, NULL); */


  /* If the history header is present, attach it to the lastheader */

    if(lastheader) {
      if (!(lastheader_))
	lastheader_ = lastheader;
      else {
	
      /* If one is already present the present one will be updated */
	qfits_header_update(lastheader, lastheader_);
	qfits_header_destroy(lastheader_);
	lastheader_ = lastheader;
      }
    }


    /* Now go to the right positions */
    
    /* Go where the header should be */
    if (header_startpos == -1)
      fseek(stream_, 0L, SEEK_END);
    else {
      fseek(stream_, (long) (header_startpos), SEEK_SET);
    }
    /* Get the position */
    fgetpos(stream_, &headerstart_);
    
    /* Dump the header */
    qfits_header_dump(header_,stream_);
    
    /* Get the position */
    fgetpos(stream_, &tablestart_);
    
    /* Now just copy the descriptor array */
    if (hdrarray_) {
      free(hdrarray_);
      hdrarray_ = temp;
    }
    else
      hdrarray_ = temp;
    
    /* Again do a header update, byteperrow will be updated */
    header = header_;
    header_ = NULL;
    ftstab_genhd(0);
    qfits_header_update(header, header_);
    qfits_header_destroy(header_);
    header_ = header;
    
    /* If we are at the end of the file, we want to destroy all that follows, but first repair a file if necessary */
    if (next >= nextinfile) {
      
      /* There is need to ask if there is a history header */
      offset = ftell(stream_);

      /* We want to know the length of the table array */
      
      /* Now go to the end of the file */
      fseek(stream_, 0, SEEK_END);
      
      /* This is the length of the table */
      offset = ftell(stream_)-offset;
      
      /* If this is either not fits compatible or it doesn't fit to the number of rows, then it is interpreted as a truncated fits file, meaning we will correct the rownumber */
      /* !!!!!!!!!!!!!formerly: if ((((!((nrows_*byteperow_)%2880))?(nrows_*byteperow_):(((nrows_*byteperow_)/2880+1)*2880)) != offset) && !(offset%byteperow_)) */

      /* If the offset is a multiple of byteperow_, then it could be that we have to rescue a file */
      if (!(offset%byteperow_)) {

	/* If the offset is not a multiple of 2880, we can probably tell that the file can maybe be rescued */
	if (offset % 2880) {
	  nrows_ =  offset/byteperow_;
	}

	/* If it is a multiple of 2880, then  we don't rescue at the moment */
	else {
	  /* This is, how it should work ... */
	   if ((((nrows_*byteperow_)/2880+1)*2880) < offset)
	   nrows_ = offset/byteperow_ ;
	}
      }

      /* Go to the position */
      fsetpos(stream_, &tablestart_);
      
      /* Now go to the end of the data array */
      fseek(stream_, byteperow_*(nrows_), SEEK_CUR);
      
      offset = ftell(stream_);
      
      if(ftruncate(fileno(stream_), offset))
	;
      clearerr(stream_);
    }
    
    /* If we have read a history header we will destroy it in the file */
    else if (lastheader) {
      /* Now go to the end of the data array */
      qfits_get_hdrinfo(filename, nextinfile, &header_startpos, NULL);
      fseek(stream_, (long) (header_startpos), SEEK_SET);
      offset = ftell(stream_);
      if(ftruncate(fileno(stream_), offset))
	;
      clearerr(stream_);
    }
    
    /* Now go to the end of the data array */
    fsetpos(stream_, &tablestart_);
    fseek(stream_, byteperow_*(nrows_), SEEK_CUR);
    
    return 1;
}



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Checks if the last extension is a history header */
static qfits_header *checkhheader(char *filename)
{
  qfits_header *lastheader;
  int nextinfile;
  char *value;

  /* Check if there is anything to check */
  if (!(nextinfile = qfits_query_n_ext(filename)))
    return NULL;

  /* Get the last header */
  if (!(lastheader = qfits_header_readext(filename, nextinfile)))
    return NULL;

  /* Check if it is a history header */
  if ((value = qfits_header_getstr(lastheader, "NAXIS"))) {
    if (!strcmp("0", value)){
      if ((value = qfits_header_getstr(lastheader, "XTENSION"))) {
	if (!strcmp("'IMAGE            '", value)){
	  qfits_header_destroy(lastheader);
	  lastheader = NULL;
	}
	else {
	  ;
	}
      }
      else {
	qfits_header_destroy(lastheader);
	lastheader = NULL;
      }
    }
    else {
      qfits_header_destroy(lastheader);
      lastheader = NULL;
    }
  }
  else {
    qfits_header_destroy(lastheader);
    lastheader = NULL;
  }

  return lastheader;
}


/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Opens a table fits file for ftstab use */

int ftstab_fopen_(char *filename, int *next, char *mode, char *hheader)
{
  return(ftstab_fopen(filename, *next, *mode, *hheader));
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destroys or updates a header if possible */
int ftstab_clearhd(char n)
{
  qfits_header *header;

  /* The history header will simply be deleted */
  if (n) {
    if (lastheader_)
      qfits_header_destroy(lastheader_);
    lastheader_ = NULL;
    return 1;
  }

  if (!(header_))
    return 1;

  /* If the header is present, delete or update it */
  else {

    /* If the header is blocked it will be updated */
    if (headerblock_) {

    /* We check (again) if the array has been allocated */
      if (!hdrarray_){
      return 0;
      }
      header = header_;
      header_ = NULL;

      byteperow_ = 0;
      ftstab_genhd(0);

      qfits_header_update(header, header_);

      qfits_header_destroy(header_);
      header_ = header;
      return 1;
    }
    else {
      qfits_header_destroy(header_);
      header_ = NULL;
      
    }

return 1;
  }
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Destroys or updates a header if possible */

int ftstab_clearhd_(char *n)
{
  return ftstab_clearhd(*n);
}

/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Dumps a row but stays at the position */

static void putrowstay(double *row) 
{
  int i;
  byte *buffer;

  for (i = 0; i < ncolumns_; ++i) {
    switch (hdrarray_[i].type) {
    case COLTYPE_FLOAT:
      buffer = qfits_pixdump_double (row+i, 1, BPP_IEEE_FLOAT);
      fwrite(buffer, 4*sizeof(byte), 1, stream_);
      break;
    case COLTYPE_CHAR:
      buffer = qfits_pixdump_double (row+i, 1, BPP_8_UNSIGNED);
      fwrite(buffer, sizeof(byte), 1, stream_);
      break;
    case COLTYPE_INT:
      buffer = qfits_pixdump_double (row+i, 1, BPP_32_SIGNED);
      fwrite(buffer, 4*sizeof(byte), 1, stream_);
      break;
    case COLTYPE_DOUBLE:
      buffer = qfits_pixdump_double (row+i, 1, BPP_IEEE_DOUBLE);
      fwrite(buffer, 8*sizeof(byte), 1, stream_);
      break;
    default:
      buffer = qfits_pixdump_double (row+i, 1, BPP_IEEE_FLOAT);
      fwrite(buffer, 4*sizeof(byte), 1, stream_);
      break;
    }

    free(buffer);
  }
    return;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Changes minima and maxima according to the content of row */

static void checkminmax(double *row)
{
  int i;

    /* Keep track of min and max */
  for (i = 0; i < ncolumns_; ++i) {
    switch(hdrarray_[i].type) {
    case COLTYPE_FLOAT:
      if (hdrarray_[i].mini.f > *(row+i))
	hdrarray_[i].mini.f = *(row+i);
      if (hdrarray_[i].maxi.f < *(row+i))
	hdrarray_[i].maxi.f = *(row+i);
      break;
      
    case COLTYPE_CHAR:
      if (hdrarray_[i].mini.c > *(row+i))
	hdrarray_[i].mini.c = *(row+i);
      if (hdrarray_[i].maxi.c < *(row+i))
	hdrarray_[i].maxi.c = *(row+i);
      break;
      
    case COLTYPE_INT:
      if (hdrarray_[i].mini.i > *(row+i))
	hdrarray_[i].mini.i = *(row+i);
      if (hdrarray_[i].maxi.i < *(row+i))
	hdrarray_[i].maxi.i = *(row+i);
      break;
      
    case COLTYPE_DOUBLE:
      if (hdrarray_[i].mini.d > *(row+i))
	hdrarray_[i].mini.d = *(row+i);
      if (hdrarray_[i].maxi.d < *(row+i))
	hdrarray_[i].maxi.d = *(row+i);
      break;
      
    default:
      if (hdrarray_[i].mini.f > *(row+i))
	hdrarray_[i].mini.f = *(row+i);
      if (hdrarray_[i].maxi.f < *(row+i))
	hdrarray_[i].maxi.f = *(row+i);
      break;
    }
  }
  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Change a row in an open table */

int ftstab_putrow(long rownumber, double *row)
{
  fpos_t currentpos;

  /* Check if a file is open */
  if (!stream_) 
    return 0;

  /* Check if the row exists */
  if ((rownumber > nrows_) || (rownumber < 1))
    return 0;

  fgetpos(stream_, &currentpos);

  /* Now simply go to the position in the file */
  fsetpos(stream_, &tablestart_);

  if (fseek(stream_, byteperow_*(rownumber-1), SEEK_CUR)) {
    fsetpos(stream_, &currentpos);
    return 0;
  }

  /* Now everything should have been done by the user, we expect an array of ncolumns_ columns */

  putrowstay(row);

    /* Keep track of min and max */
  checkminmax(row);

  /* Go back to the origin */
  fsetpos(stream_, &currentpos);
      
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Change a row in an open table */

int ftstab_putrow_(long *rownumber, double *row)
{
  return ftstab_putrow(*rownumber, row);
}
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Dumps a value but stays at the position */

static void putvalstay(int colnumber, double value)
{
  byte *buffer;

  switch (hdrarray_[colnumber].type) {
  case COLTYPE_FLOAT:
    buffer = qfits_pixdump_double (&value, 1, BPP_IEEE_FLOAT);
    fwrite(buffer, 4*sizeof(byte), 1, stream_);
    break;
  case COLTYPE_CHAR:
    buffer = qfits_pixdump_double (&value, 1, BPP_8_UNSIGNED);
    fwrite(buffer, sizeof(byte), 1, stream_);
      break;
  case COLTYPE_INT:
    buffer = qfits_pixdump_double (&value, 1, BPP_32_SIGNED);
    fwrite(buffer, 4*sizeof(byte), 1, stream_);
    break;
  case COLTYPE_DOUBLE:
    buffer = qfits_pixdump_double (&value, 1, BPP_IEEE_DOUBLE);
    fwrite(buffer, 8*sizeof(byte), 1, stream_);
    break;
  default:
    buffer = qfits_pixdump_double (&value, 1, BPP_IEEE_FLOAT);
    fwrite(buffer, 4*sizeof(byte), 1, stream_);
    break;
  }
  free(buffer);
  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Changes minimum and maximum according to the content of value */
static void checkminmax_single(int colnumber, double value)
{
    switch(hdrarray_[colnumber].type) {
    case COLTYPE_FLOAT:
      if (hdrarray_[colnumber].mini.f > value)
	hdrarray_[colnumber].mini.f = value;
      if (hdrarray_[colnumber].maxi.f < value)
	hdrarray_[colnumber].maxi.f = value;
      break;
      
    case COLTYPE_CHAR:
      if (hdrarray_[colnumber].mini.c > value)
	hdrarray_[colnumber].mini.c = value;
      if (hdrarray_[colnumber].maxi.c < value)
	hdrarray_[colnumber].maxi.c = value;
      break;

    case COLTYPE_INT:
      if (hdrarray_[colnumber].mini.i > value)
	hdrarray_[colnumber].mini.i = value;
      if (hdrarray_[colnumber].maxi.i < value)
	hdrarray_[colnumber].maxi.i = value;
      break;
      
    case COLTYPE_DOUBLE:
      if (hdrarray_[colnumber].mini.d > value)
	hdrarray_[colnumber].mini.d = value;
      if (hdrarray_[colnumber].maxi.d < value)
	hdrarray_[colnumber].maxi.d = value;
      break;
      
    default:
      if (hdrarray_[colnumber].mini.f > value)
	hdrarray_[colnumber].mini.f = value;
      if (hdrarray_[colnumber].maxi.f < value)
	hdrarray_[colnumber].maxi.f = value;
      break;
    }
  
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Change a value in an open table */

int ftstab_putval(long rownumber, int colnumber, double value)
{
  fpos_t currentpos;

  /* Check if a file is open */
  if (!stream_) 
    return 0;

  /* Check if the row exists */
  if ((rownumber > nrows_) || (rownumber < 1))
    return 0;
  if ((colnumber > ncolumns_) || (colnumber < 1))
    return 0;

  fgetpos(stream_, &currentpos);

  /* Now simply go to the position in the file */
  fsetpos(stream_, &tablestart_);

  if (fseek(stream_, byteperow_*(rownumber-1)+byteoffset_[colnumber-1], SEEK_CUR)) {
    fsetpos(stream_, &currentpos);
    return 0;
  }

  /* Now everything should have been done by the user, we expect an array of ncolumns_ columns */

  /* BUGFIX: Formerly: putvalstay(colnumber, value); */
  putvalstay(colnumber-1, value);
    
    /* Keep track of min and max */
  checkminmax_single(colnumber, value);


  /* Go back to the origin */
  fsetpos(stream_, &currentpos);
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Change a value in an open table */

int ftstab_putval_(long *rownumber, int *colnumber, double *value)
{
  return ftstab_putval(*rownumber, *colnumber, *value);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Correct the minima and the maxima towards the minima and maxima between begin and end row */

int ftstab_findminmax_(long *begin, long *end)
{
  return ftstab_findminmax(*begin, *end);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Correct the minima and the maxima towards the minima and maxima between begin and end row */

int ftstab_findminmax(long begin, long end)
{
  fpos_t currentpos;
  double *darray;
  int i;

  /* Check for the possibility to do it */
  if (!(begin > 0) && !(end <= nrows_))
    return 0;

  /* Check if the stream is open */
  if (!stream_)
    return 0;

  /* Go to the position described by begin */
  fgetpos(stream_, &currentpos);

    fsetpos(stream_, &tablestart_);

  if (fseek(stream_, byteperow_*(begin-1), SEEK_CUR)) {
    fsetpos(stream_, &currentpos);
    return 0;
  }

  /* Allocate a double array */
  if(!(darray = ftstab_get_dblarr())) {
    fsetpos(stream_, &currentpos);
    return 0;
  }
  
  /* reset the minmax information */
  for (i = 0; i < ncolumns_; ++i)
  resetminmax(i);

  /* Now count through controlling minimum and maximum */
  while(begin <= end) {
    getrowstay(darray);
    checkminmax(darray);
  ++begin;
  }

  /* finish */
  fsetpos(stream_, &currentpos);
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Unsafe function to get a value quickly */

static double getquickval(int colnr, long firstval, long rownr)
{
/* Go to the position in the file */
  fseek(stream_, firstval+rownr*byteperow_,SEEK_SET);

  /* Now get the value */
  return getvalstay(colnr);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Dumps a row at a given rownumber offset to firstrow */
static void putquickrow(long firstrow, long rownumber, char *rowtoput)
{
  /* Go to the position of the first row in the file */
  fseek(stream_, firstrow+rownumber*byteperow_,SEEK_SET);

  /* Put the buffer in the file */
  fwrite(rowtoput, 1, byteperow_, stream_);

  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts a row at a given rownumber offset to firstrow into the
   allocated rowtoget */
static void getquickrow(long firstrow, long rownumber, char *rowtoget)
{
  /* Go to the position of the first row in the file */
  fseek(stream_, firstrow+rownumber*byteperow_,SEEK_SET);

  /* Put the content into the buffer */
  if(fread(rowtoget, 1, byteperow_, stream_))
    ;

  return;
}

/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Sorts ascending the rows in-between begin and end depending on the value of column */

int ftstab_heapsort_(int *column, long *start, long *end)
{
  return ftstab_heapsort( *column, *start, *end);
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Sorts ascending the rows in-between begin and end depending on the value of column */

int ftstab_heapsort(int column, long start, long end)
{

  long i,ir,j,l,n;
  double checkval;
  long offsetfirstcol, offsetfirstrow;
  char *buffer1, *buffer2;
  fpos_t currentpos;

  /* Check if sensible input values are given */
  if ((start < 1) || (end > nrows_))
    return 0;

  if ((column < 1) || (column > ncolumns_))
    return 0;

  --start;
  --end;

  /* Check if the stream is open */
  if (!(stream_))
    return 0;

  /* Check if two buffers can be allocated */
  if (!(buffer1 = (char *) malloc(2*byteperow_*sizeof(char))))
      return 0;

  /* buffer2 can simply be set as a pointer in buffer1 */
  buffer2 = buffer1+byteperow_;

  /* Safe the position in the file */
  fgetpos(stream_, &currentpos);

  /* Go to the start of the array */
  fsetpos(stream_, &tablestart_);

  --column;
  /* Find out the position of the first column */
  offsetfirstcol = ftell(stream_)+byteperow_*start+byteoffset_[column];
  offsetfirstrow = ftell(stream_)+byteperow_*start;

  n = end-start+1L;

  if (n < 2) 
    return 1;
  l=(n >> 1)+1;
  ir=n;
  for (;;) {
    if (l > 1) {
      --l;
      checkval=getquickval(column, offsetfirstcol, l-1);
      getquickrow(offsetfirstrow, l-1, buffer1);
    } 
    else {
      checkval = getquickval(column, offsetfirstcol, ir-1);
      getquickrow(offsetfirstrow, ir-1, buffer1);
      getquickrow(offsetfirstrow, 0, buffer2);
      putquickrow(offsetfirstrow, ir-1, buffer2);
      if (--ir == 1) {
	putquickrow(offsetfirstrow, 0, buffer1);
	break;
      }
    }
    i=l;
    j=l+l;
    while (j <= ir) {
      if (j < ir && getquickval(column, offsetfirstcol, j-1) < getquickval(column, offsetfirstcol, j))
	j++;
      if (checkval < getquickval(column, offsetfirstcol, j-1)) {
      getquickrow(offsetfirstrow, j-1, buffer2);
      putquickrow(offsetfirstrow, i-1, buffer2);
	i=j;
	j = (j << 1);
      } else break;
    }
    putquickrow(offsetfirstrow, i-1, buffer1);
  }

  fsetpos(stream_, &currentpos);
  free(buffer1);
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Make a copy of the file in process to the current extension */

int ftstab_copytohere(char *newfilename)
{
  fpos_t currentpos;
  FILE *stream;
  long offset, position, i;

  /* Check if the stream is ready to be copied */
  if (!stream_)
    return 0;

  /* We intrude, but I hope nobody will notice, I'm too lazy */
  putrownr();

  /* Open the output file */
  if (!(stream = fopen(newfilename, "w")))
    return 0;

  /* Get the current position in the file */
  fgetpos(stream_, &currentpos);

  /* Calculate how many things we have to copy */
  offset = ftell(stream_);

  /* Go to the start of the stream_ */
  fseek(stream_, 0L, SEEK_SET);

  /* Copy the whole content of the stream up to the current position */
  for (i = 0; i <= offset; ++i) {
    fputc(fgetc(stream_), stream);
  }

  /* This is not necessary */
  fsetpos(stream, &currentpos);

  /* This is done under the assumption that ftell delivers the numbers
     of bytes of the whole file, if we are at the end of the file */

  position = (ftell(stream) + 1L)%2880;

  for (i = 0; i <= 2880-position; ++i) {
    fputc(' ', stream);
  }

  /* Dump the lastheader if present */
  if ((lastheader_))
    qfits_header_dump(lastheader_,stream);

  fclose(stream);

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Delete all subsequent extensions */

int ftstab_deleterest_(void)
{
  /* Check if the file is open */
  if (!stream_)
    return 0;

  /* Delete the rest */
  if(ftruncate(fileno(stream_), ftell(stream_)))
    ;
  clearerr(stream_);

  /* Set the permissions */
  tableblock_ = 0;

  if (!nrows_)
    headerblock_ = 0;
  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Opens a file for histogram output */

static FILE *checkouthistofile(char *filename, qfits_header **header, qfits_header **hheader)
{
  FILE *checkouthistofile;
  int next;
  int header_startpos;

  /* First check if there is such a file and if it is a fits */
  if (qfits_is_fits(filename) != 1) {
    if (!(checkouthistofile = fopen(filename, "w+"))) {
      return NULL;
    }

    /* The file cannot have a history header */
    *hheader = NULL;

    /* Make a default primary header */
    if ((*header = qfits_header_default()) == NULL)
      return NULL;
    
    qfits_header_add(*header,"BITPIX","32",NULL,NULL);
    qfits_header_add(*header,"NAXIS","2",NULL,NULL);
    qfits_header_add(*header,"NAXIS1","100",NULL,NULL);
    qfits_header_add(*header,"NAXIS2","1",NULL,NULL);
    qfits_header_add(*header,"EXTEND","T",NULL,NULL);
    
    qfits_header_add(*header,"BSCALE","1",NULL,NULL);
    qfits_header_add(*header,"BZERO","0",NULL,NULL);
    qfits_header_add(*header,"BUNIT","'count'",NULL,NULL);
    qfits_header_add(*header,"BTYPE","' '",NULL,NULL);
    
    qfits_header_add(*header,"CDELT1", "0.0",NULL,NULL);
    qfits_header_add(*header,"CRPIX1", "1.0",NULL,NULL);
    qfits_header_add(*header,"CRVAL1", "0.0",NULL,NULL);
    qfits_header_add(*header,"CTYPE1", "' '",NULL,NULL);
    qfits_header_add(*header,"CUNIT1", "' '",NULL,NULL);
    
    qfits_header_add(*header,"CDELT2", "1.0",NULL,NULL);
    qfits_header_add(*header,"CRPIX2", "1.0",NULL,NULL);
    qfits_header_add(*header,"CRVAL2", "0.0",NULL,NULL);
    qfits_header_add(*header,"CTYPE2", "' '",NULL,NULL);
    qfits_header_add(*header,"CUNIT2", "'Y'",NULL,NULL);

    qfits_header_add(*header, "TITLE1", "' '", NULL, NULL);
    qfits_header_add(*header, "TITLE2", "' '", NULL, NULL);
    qfits_header_add(*header, "RADIUS1", "0.0", NULL, NULL);
    qfits_header_add(*header, "RADIUS2", "0.0", NULL, NULL);
    qfits_header_add(*header, "HISTORY", "'Generated by ftstab'", NULL, NULL);
  }
    
  /* It is a fits file */
  else {

    /* Now check for the number of extensions */
    *hheader = checkhheader(filename);

    /* Get the number of extensions */
    next = qfits_query_n_ext(filename);

    /* Do a refresh immediately */
    qfits_cache_purge();

    if (*hheader)
      /* Get the position to put the fileposition */
      qfits_get_hdrinfo(filename, next, &header_startpos, NULL);

    /* Try to open the file */
    if (!(checkouthistofile = fopen(filename, "r+"))) {
      return NULL;
    }
    /* Go to the right position and delete the rest */

    if (*hheader) {
      fseek(checkouthistofile, (long) header_startpos, SEEK_SET);
      if(ftruncate(fileno(stream_), ftell(checkouthistofile)))
	;
      clearerr(stream_);
    }    
    else
      fseek(checkouthistofile, 0L, SEEK_END);

    /* Now make a default extension header */
    if ((*header = qfits_header_new()) == NULL)
      return NULL;

    qfits_header_append(*header,"XTENSION","'IMAGE             '","","");
    qfits_header_append(*header,"END","","","");

    qfits_header_add(*header,"BITPIX","32",NULL,NULL);
    qfits_header_add(*header,"NAXIS","2",NULL,NULL);
    qfits_header_add(*header,"NAXIS1","100",NULL,NULL);
    qfits_header_add(*header,"NAXIS2","1",NULL,NULL);
    qfits_header_add(*header,"EXTEND","T",NULL,NULL);
    
    qfits_header_add(*header,"BSCALE","1",NULL,NULL);
    qfits_header_add(*header,"BZERO","0",NULL,NULL);
    qfits_header_add(*header,"BUNIT","'count'",NULL,NULL);
    qfits_header_add(*header,"BTYPE","' '",NULL,NULL);
    
    qfits_header_add(*header,"CDELT1", "0.0",NULL,NULL);
    qfits_header_add(*header,"CRPIX1", "1.0",NULL,NULL);
    qfits_header_add(*header,"CRVAL1", "0.0",NULL,NULL);
    qfits_header_add(*header,"CTYPE1", "' '",NULL,NULL);
    qfits_header_add(*header,"CUNIT1", "' '",NULL,NULL);
    
    qfits_header_add(*header,"CDELT2", "1.0",NULL,NULL);
    qfits_header_add(*header,"CRPIX2", "1.0",NULL,NULL);
    qfits_header_add(*header,"CRVAL2", "0.0",NULL,NULL);
    qfits_header_add(*header,"CTYPE2", "' '",NULL,NULL);
    qfits_header_add(*header,"CUNIT2", "'Y'",NULL,NULL);

    qfits_header_add(*header, "TITLE1", "' '", NULL, NULL);
    qfits_header_add(*header, "TITLE2", "' '", NULL, NULL);
    qfits_header_add(*header, "RADIUS1", "0.0", NULL, NULL);
    qfits_header_add(*header, "RADIUS2", "0.0", NULL, NULL);
    qfits_header_add(*header, "HISTORY", "'generated by ftstab'", NULL, NULL);
  }

  return checkouthistofile;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Makes a fits image containing a histogram of a column */

int ftstab_histogram_(char *filename, int *column, long *startrow, long *endrow, double *min, double *max, long *bins, double *delta, int *repet)
{
  return ftstab_histogram(filename, *column, *startrow, *endrow, *min, *max, *bins, *delta, *repet);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Makes a fits image containing a histogram of a column */

int ftstab_histogram(char *filename, int column, long startrow, long endrow, double min, double max, long bins, double delta, int repet)
{
  qfits_header *header, *hheader;
  double adouble;
  int *iarray;
  FILE *stream;
  char key[9];
  char value[21];
  int title;
  double radius;
  long i,j, offsetfirstcol, length;
  fpos_t currentpos;
  byte *buffer;

  /* Make the usual checks */
  if (!(column > 0) || !(column <= ncolumns_))
    return 0;

  if (!stream_)
    return 0;

  /* The startrow and the endrow have to be checked */
  if (startrow > endrow) {
    startrow = 1;
    endrow = nrows_;
  }

  if (!(nrows_) || (startrow < 1) || (endrow > nrows_))
    return 0;

  if (repet < 1)
    return 0;

  /* Now check whether min > max */
  /* The startrow and the endrow have to be checked */
  if (min > max) {

    /* We have to find min and max ourselves */
    ftstab_findminmax(startrow, endrow);
    min = ftstab_get_colmin(column);
    max = ftstab_get_colmax(column);
  }

  /* If there is a number of bins, calculate the delta */
  if (bins > 0) {
    if (!((max - min)/bins)) {
      if (!delta)
      return 0;
    }
    else 
      delta = (max - min)/bins;
  }
  else if (delta <= 0) {
    bins = 100;
    if (!(delta = (max - min)/bins))
      return 0;
  }
  else {
    if (delta > 0.0) {
      bins = (!!((long) modf((max - min)/delta, &adouble)));
bins =bins + (long) adouble;
    }
    else 
      return 0;
  }

  /* Find out about the name type and unit of the column */
  title = ftstab_get_coltit(column);

  /* Find out about the radius */
  radius = ftstab_get_colrad(column);

  /* Now we can proceed to arrange for the array, has to be int, unfortunately */
  if (!(iarray = (int *) malloc(repet*bins*sizeof(int))))
    return 0;

  /* Open the file */
  if (!(stream = checkouthistofile(filename, &header, &hheader)))
    return 0;

  /* Now correct the header */
  /* For the bin number */
  sprintf(key,"NAXIS1");
  sprintf(value,"%li", bins);
  qfits_header_mod(header,key,value,NULL);

  /* For the repetition number */
  sprintf(key,"NAXIS2");
  sprintf(value,"%i", repet);
  qfits_header_mod(header,key,value,NULL);

  /* For the delta */
  sprintf(key,"CDELT1");
/*   sprintf(value,"%.12E", delta); */
  sprintf(value,"%.12E", ftstab_gtscal(title)*delta);
  qfits_header_mod(header,key,value,NULL);

  /* For the reference value, is min plus half a delta */
  sprintf(key,"CRVAL1");
/*   sprintf(value,"%.12E", min+delta/2); */
  sprintf(value,"%.12E", ftstab_gtscal(title)*(min+delta/2)+ftstab_gtzero(title));
  qfits_header_mod(header,key,value,NULL);

  /* For the name of the column */
  sprintf(key,"TITLE");
  ftstab_putlcoltitl(value, title);
  qfits_header_mod(header,key,value,NULL);

  /* For the unit */
  sprintf(key,"CUNIT1");
  ftstab_putlcolunit(value, title);
  qfits_header_mod(header,key,value,NULL);

  /* For the unit */
  sprintf(key,"CTYPE1");
  ftstab_putlcoltype(value, title);
  qfits_header_mod(header,key,value,NULL);

  /* For the name of the column1 */
  sprintf(key,"TITLE1");
  ftstab_putlcoltitl(value, title);
  qfits_header_mod(header,key,value,NULL);


  sprintf(key,"RADIUS1");
  sprintf(value,"%.12E", radius);
  qfits_header_mod(header,key,value,NULL);

  /* Now sort the file */
  if (!ftstab_heapsort(column, startrow, endrow)) {
    free(iarray);
    if (hheader)
      qfits_header_destroy(hheader);
    qfits_header_destroy(header);
    return 0;
  }

  --column;
  length = endrow-startrow+1;

  /* Get the current position in the file */
  fgetpos(stream_, &currentpos);

  fsetpos(stream_, &tablestart_);
  offsetfirstcol = ftell(stream_)+byteperow_*(startrow-1)+byteoffset_[column];


  /* The file is sorted we can fill the array */
  j = 0;

  /* There is only 0's in the array */
  for (i = 0; i < repet*bins; ++i)
    iarray[i] = 0;
  
  /* We search for the first column that is larger than min, before we make a check if there is any number that is in-between min and max */

  ftstab_get_value(endrow, column+1, &adouble);


  if (adouble < min) {
    ;
  }
  else {
    /* Find the first row with a value that is larger or equal min */
    while (getquickval(column, offsetfirstcol, j) < min) {
      ++j;
    }

    /* Now the first bin has to be increased by one */
    min = min+delta;
    
    /* Now the array will be filled */
    i = 0;
    if (bins > 2) {
      while ((i < (bins-1)) && (j < length)) {
	if (getquickval(column, offsetfirstcol, j) <= min) {
	  iarray[i] = iarray[i]+1;
	  ++j;
	}
	else {
	  min = min+delta;
	  ++i;
	}    
      }

      /* We do the last loop seperately because of numerical errors that might have occured, i.e. min+n*delta != max */
      while (j < length) {
	if (getquickval(column, offsetfirstcol, j) <= max) {
	  iarray[i] = iarray[i]+1;
	  ++j;
	}
	else
	  break;
      }
    }
    else {
      while (j < length) {
	if (getquickval(column, offsetfirstcol, j) <= max) {
	  iarray[i] = iarray[i]+1;
	  ++j;
	}
	else
	  break;
      }
    }
  }
  /* That was nearly it, put the file pointer back */
  fsetpos(stream_, &currentpos);

  /* Fill the whole array */
  for (j = 1; j < repet; ++j) {
    for (i = 0; i < bins; ++i) {
      iarray[i+j*bins] = iarray[i];
    }
  }

  /* Now dump the header to the file */
  qfits_header_dump(header, stream);
  qfits_header_destroy(header);

  /* Now I'm afraid there is either some memory requirement or we are slow... As the philosophy of the module is not to spend too much memory... */

  for (i = 0; i < (repet*bins); ++i) {
    if (!(buffer = qfits_pixdump_int(iarray+i, 1, BPP_32_SIGNED))) {  
      free(iarray);
      if (hheader)
	qfits_header_destroy(hheader);
      return 0;
    }
    
    /* Put the buffer in the file */
    fwrite(buffer, 4, 1, stream);
    free(buffer);
  }
  free(iarray);


  /* Stretch the file to the right length */  
  fseek(stream, 0L, SEEK_END);
  j = (ftell(stream) + 1L)%2880;

  /* Now, in qfits they use NUL to pad a file, standard is ASCII
     blank, while I don't know whether the description I have is wrong
     or qfits. I believe qfits */
  
  for (i = 0; i <= 2880-j; ++i) {
    fputc(' ', stream);
  }

  /* Dump the hheader */
  if (hheader) {
    qfits_header_dump(hheader, stream);
    qfits_header_destroy(hheader);
  }

  /* finish */
  fclose(stream);

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Initializes the standard header context table */
static int hdl_init(void)
{
  /* Check if it is already there */
  if ((hdrlist_.n))
    return 0;

  if (!hdlapp("DEFAULT", "NATURAL", " ", 0.0, 1.0))
    return 0;
/*   if (!hdlapp("RADI", "ANGLE", "deg", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("VROT", "VELO", "km/s", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("Z0", "ANGLE", "deg", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("SBR", "FLUX", "Jy/(arcsec**2)", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("INCL", "ANGLE", "deg", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("PA", "ANGLE", "deg", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("XPOS", "ANGLE", "deg", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("YPOS", "ANGLE", "deg", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("VSYS", "VELO", "km/s", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("CONDISP", "VELO", "km/s", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("DISP", "VELO", "km/s", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("CHISQ", "NATURAL", " ", 0.0, 1.0)) */
/*     return 0; */
/*   if (!hdlapp("ITER", "NATURAL", " ", 0.0, 1.0)) */
/*     return 0; */
  return 1;

}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/*  */
static int hdlapp(char *titl, char *ttype, char *tunit, double tzero, double tscal)
{
  hdrit *item;

  /* Brute force, if this doesn't work, forget it all */
  if (!(item = (hdrit *) malloc(sizeof(hdrit))))
    return 0;

  /* Now fill it, it must work */
  strncpy(item -> titl, titl, 18);
  strncpy(item -> ttype, ttype, 18);
  strncpy(item -> tunit, tunit, 18);

  item -> titl[18] = '\0';
  item -> ttype[18] = '\0';
  item -> tunit[18] = '\0';

  item -> tzero = tzero;
  item -> tscal = tscal;

  /* Append it */
  if (hdrlist_.last)
    hdrlist_.last -> next = item;
  else 
    hdrlist_.first = item;
  hdrlist_.last = item;
  ++hdrlist_.n;

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Modify the header item list */

int ftstab_hdladditem(char *titl, char *ttype, char *tunit, double tzero, double tscal)
{
  int i;
  hdrit *item;

  /* If the item list does not exist, create it */
  if (!hdrlist_.n) {
    if (! hdl_init())
      return -1;
  }

  /* Check if the item exists, blanks count fully... */
  item = hdrlist_.first;

  for (i = 0; i < (hdrlist_.n-1); ++i) {
    if (!strcmp(titl, item -> titl))
      break;
    item = item -> next;
  }
  if (strcmp(titl, item -> titl)) {
    i = -1;
    ++i;
  }
  else
    ++i;
  
  /* If the item does not exist, try to append the whole thing */
  if (i == 0) {
    if (!hdlapp(titl, ttype, tunit, tzero, tscal))
      return -1;
  }
  
  /* If the item exists, the content will be changed */
  else {
    strncpy(item -> titl, ttype, 8);
    strncpy(item -> ttype, ttype, 18);
    strncpy(item -> tunit, ttype, 18);
    item -> tzero = tzero;
    item -> tscal = tscal;
  }

  return i;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Resets the hdllist */
int ftstab_hdlreset_(void)
{
  int i;
  hdrit *next, *current;

  current = hdrlist_.first;

  /* Delete the structures */
  for (i = 0; i < hdrlist_.n; ++i) {
    next = current -> next;
    free(current);
    current = next;
  }

  /* Reset the thingies */
  hdrlist_.n = 0;
  hdrlist_.first = hdrlist_.last = NULL;

  return(hdl_init());
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Delete an item from the header item list */

int ftstab_hdldelitem(char *titl)
{
  int i;
  hdrit *item;
  hdrit **bef;

  /* If the item list does not exist, create it */
  if (!hdrlist_.n) {
    if (! hdl_init())
      return -1;
  }

  /* Find the item */
  item = hdrlist_.first;
  bef = &(hdrlist_.first);

  for (i = 0; i < (hdrlist_.n-1); ++i) {
    if (!strcmp(titl, item -> titl))
      break;
    bef = &(item -> next);
    item = item -> next;
  }
  if (strcmp(titl, item -> titl)) {
    return -1;
  }
  else
    ++i;
  
  /* This may be not elegant */
  *bef = item -> next;
  free(item);
  --hdrlist_.n;

  return i;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Makes a fits image containing a histogram of two columns */

int ftstab_histogram_2d_(char *filename, long *startrow, long *endrow, int *column1, double *min1, double *max1, long *bins1, double *delta1, int *column2, double *min2, double *max2, long *bins2, double *delta2)
{
  return ftstab_histogram_2d(filename, *startrow, *endrow, *column1, *min1, *max1, *bins1, *delta1, *column2, *min2, *max2, *bins2, *delta2);
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Makes a fits image containing a histogram of two columns */

int ftstab_histogram_2d(char *filename, long startrow, long endrow, int column1, double min1, double max1, long bins1, double delta1, int column2, double min2, double max2, long bins2, double delta2)
{
  qfits_header *header, *hheader;
  double adouble;
  int *iarray, *iarray2;
  FILE *stream;
  char key[9];
  char value[21];
  int title1, title2;
  double radius1, radius2;
  long i,j,k;
  long offsetfirstcol2;
  fpos_t currentpos;
  byte *buffer;

  /* Make the usual checks */
  if (!(column1 > 0) || !(column1 <= ncolumns_))
    return 0;

  if (!(column2 > 0) || !(column2 <= ncolumns_))
    return 0;

  if (!stream_)
    return 0;

  /* The startrow and the endrow have to be checked */
  if (startrow > endrow) {
    startrow = 1;
    endrow = nrows_;
  }

  if (!(nrows_) || (startrow < 1) || (endrow > nrows_))
    return 0;

  /* Now check whether min1 > max1 */
  /* The startrow and the endrow have to be checked */
  if (min1 > max1) {

    /* We have to find min1 and max ourselves */
    ftstab_findminmax(startrow, endrow);
    min1 = ftstab_get_colmin(column1);
    max1 = ftstab_get_colmax(column1);
  }

  /* If there is a number of bins1, calculate the delta1 */
  if (bins1 > 0) {  
    if (!((max1 - min1)/bins1)) {
      if (!delta1)
	return 0;
    }
    else 
      delta1 = (max1 - min1)/bins1;
  }
  else if (delta1 <= 0) {
    bins1 = 100;
        if (!(delta1 = (max1 - min1)/bins1))
      return 0;
  }
  else {
    if (delta1 > 0.0) {
      bins1 = !!((long) modf((max1 - min1)/delta1, &adouble));
      bins1 = bins1 + (long) adouble;
    }
    else
      return 0;
  }

  /* Now check whether min2 > max2 */
  /* The startrow and the endrow have to be checked */
  if (min2 > max2) {

    /* We have to find min1 and max ourselves */
    min2 = ftstab_get_colmin(column2);
    max2 = ftstab_get_colmax(column2);
  }

  /* If there is a number of bins1, calculate the delta1 */
  if (bins2 > 0) {
    if (!((max2 - min2)/bins2)) {
      if (!delta2)
	return 0;
    }
    else 
      delta2 = (max2 - min2)/bins2;
  }
  else if (delta2 <= 0) {
    bins2 = 100;
    if (!(delta2 = (max2 - min2)/bins2))
      return 0;
  }
  else {
    if (delta2 > 0.0) {
      bins2 = !!((long) modf((max2 - min2)/delta2, &adouble));
      bins2 = bins2 + (long) adouble;
    }
    else
      return 0;
  }

  /* Find out about the name type and unit of the column1 */
  title1 = ftstab_get_coltit(column1);
  title2 = ftstab_get_coltit(column2);

  /* Find out about the radius */
  radius1 = ftstab_get_colrad(column1);
  radius2 = ftstab_get_colrad(column2);

  /* Now we can proceed to arrange for the array, has to be int, unfortunately */
  if (!(iarray = (int *) malloc(bins2*bins1*sizeof(int))))
    return 0;

  /* Open the file */
  if (!(stream = checkouthistofile(filename, &header, &hheader)))
    return 0;

  /* Now correct the header */
  /* For the bin number */
  sprintf(key,"NAXIS1");
  sprintf(value,"%li", bins1);
  qfits_header_mod(header,key,value,NULL);

  /* For the repetition number */
  sprintf(key,"NAXIS2");
  sprintf(value,"%li", bins2);
  qfits_header_mod(header,key,value,NULL);

  /* For the delta1 */
  sprintf(key,"CDELT1");
/*   sprintf(value,"%.12E", delta1); */
  sprintf(value,"%.12E", ftstab_gtscal(title1)*delta1);
  qfits_header_mod(header,key,value,NULL);

  /* For the delta2 */
  sprintf(key,"CDELT2");
/*   sprintf(value,"%.12E", delta2); */
  sprintf(value,"%.12E", ftstab_gtscal(title2)*delta2);
  qfits_header_mod(header,key,value,NULL);

  /* For the reference value, is min1 plus half a delta1 */
  sprintf(key,"CRVAL1");
/*   sprintf(value,"%.12E", min1+delta1/2); */
  sprintf(value,"%.12E", ftstab_gtscal(title1)*(min1+delta1/2)+ftstab_gtzero(title1));
  qfits_header_mod(header,key,value,NULL);

  sprintf(key,"CRVAL2");
/*   sprintf(value,"%.12E", min2+delta2/2); */
  sprintf(value,"%.12E", ftstab_gtscal(title2)*(min2+delta2/2)+ftstab_gtzero(title2));
  qfits_header_mod(header,key,value,NULL);

  /* For the name of the column1 */
  sprintf(key,"TITLE1");
  ftstab_putlcoltitl(value, title1);
  qfits_header_mod(header,key,value,NULL);

  sprintf(key,"TITLE2");
  ftstab_putlcoltitl(value, title2);
  qfits_header_mod(header,key,value,NULL);

  sprintf(key,"RADIUS1");
  sprintf(value,"%.12E", radius1);
  qfits_header_mod(header,key,value,NULL);

  sprintf(key,"RADIUS2");
  sprintf(value,"%.12E", radius2);
  qfits_header_mod(header,key,value,NULL);

  /* For the unit */
  sprintf(key,"CUNIT1");
  ftstab_putlcolunit(value, title1);
  qfits_header_mod(header,key,value,NULL);

  sprintf(key,"CUNIT2");
  ftstab_putlcolunit(value, title2);
  qfits_header_mod(header,key,value,NULL);

  /* For the unit */
  sprintf(key,"CTYPE1");
  ftstab_putlcoltype(value, title1);
  qfits_header_mod(header,key,value,NULL);

  sprintf(key,"CTYPE2");
  ftstab_putlcoltype(value, title2);
  qfits_header_mod(header,key,value,NULL);

  /* Get the current position in the file */
  fgetpos(stream_, &currentpos);
  /* Now sort the file */
  if (!ftstab_heapsort(column2, startrow, endrow)) {
    free(iarray);
    if (hheader)
      qfits_header_destroy(hheader);
    qfits_header_destroy(header);
    return 0;
  }

  --column2;

  fsetpos(stream_, &tablestart_);
  offsetfirstcol2 = ftell(stream_)+byteperow_*(startrow-1)+byteoffset_[column2];

  /* The file is sorted we can fill the array */
  j = 0;

  /* There is only 0's in the array */
  for (i = 0; i < bins2*bins1; ++i)
    iarray[i] = 0;
  
  /* We search for the first column that is larger than min, before we make a check if there is any number that is in-between min and max */

  ftstab_get_value(endrow, column2+1, &adouble);


  if (adouble < min2) {
    ;
  }
  else {
    /* Find the first row with a value that is larger or equal min */
    while (getquickval(column2, offsetfirstcol2, j) < min2) {
      ++j;
    }
    
    /* Now the first bin has to be increased by one */
    min2 = min2+delta2;
    
    /* Now the array will be filled */
    i = 0;
    k = 0;
    iarray2 = iarray;
    
    if (bins2 > 2) {
      while ((i < (bins2-1)) && (j < endrow)) {
	
	/* This serves now to find the range of rows that have to be processed again */
	if (getquickval(column2, offsetfirstcol2, j) <= min2) {
	  /* 	  iarray[i] = iarray[i]+1; */
	  ++j;
	}
	else {

	  /* Now j+startrow-1 takes over the place of endrow and k+startrow the place of startrow */
	  if (!histo_putarray(column1, startrow+k, startrow+j-1,  bins1, min1,  max1, delta1, iarray2)) {
	    free(iarray);
	    if (hheader)
	      qfits_header_destroy(hheader);
	    qfits_header_destroy(header);
	    return 0;
	  }
	  min2 = min2+delta2;
	  
	  iarray2 = iarray2+bins1;
	  k = j;
	  
	  ++i;
	}    
      }
  
      /* We do the last loop seperately because of numerical errors that might have occured, i.e. min+n*delta != max */
      while (j < endrow) {
 
	if (getquickval(column2, offsetfirstcol2, j) <= max2) {
	  /* 	  iarray[i] = iarray[i]+1; */
	  ++j;
 
	}
	else {
	  /* Now j+startrow-1 takes over the place of endrow and k+startrow the place of startrow */
	  if (k != j) {
	    if (!histo_putarray(column1, startrow+k, startrow+j-1, bins1, min1,  max1, delta1, iarray2)) {
	      free(iarray);
	      if (hheader)
		qfits_header_destroy(hheader);
	      qfits_header_destroy(header);
	      return 0;
	    }
	    min2 = min2+delta2;
	    iarray2 = iarray2+bins1;
	    k = j;
	    
	    ++i;
	  }
	  break;
	}
      }

      /* We have to put another loop here in case we got to the end */
      --j;
      if (k != j) { 
	if (!histo_putarray(column1, startrow+k-1, startrow+j-1, bins1, min1,  max1, delta1, iarray2)) {
	  free(iarray);
	  if (hheader)
	    qfits_header_destroy(hheader);
	  qfits_header_destroy(header);
	  return 0;
	}
	min2 = min2+delta2;
	
	iarray2 = iarray2+bins1;
	k = j;
	++i;
      }
      
    }
    else {
      while (j < endrow) {
	if (getquickval(column2, offsetfirstcol2, j) <= max2) {
	  /* 	  iarray[i] = iarray[i]+1; */
	  ++j;
	}
	else {
	  if (k != j) {
	    if (!histo_putarray(column1, startrow+j-1, startrow+k-1, bins1, min1,  max1, delta1, iarray2)) {
	      free(iarray);
	      if (hheader)
		qfits_header_destroy(hheader);
	      qfits_header_destroy(header);
	      return 0;
	    }
	    min2 = min2+delta2;
	    iarray2 = iarray2+bins1;
	    k = j;
	    
	    ++i;
	  }
	  break;
	  
	}
      }
       --j;
      if (k != j) { 
	if (!histo_putarray(column1, startrow+k-1, startrow+j-1, bins1, min1,  max1, delta1, iarray2)) {
	  free(iarray);
	  if (hheader)
	    qfits_header_destroy(hheader);
	  qfits_header_destroy(header);
	  return 0;
	}
	min2 = min2+delta2;
	
	iarray2 = iarray2+bins1;
	k = j;
	++i;
      } 
    }
  }
 
  /* That was nearly it, put the file pointer back */
  fsetpos(stream_, &currentpos);

  /* Now dump the header to the file */
  qfits_header_dump(header, stream);
  qfits_header_destroy(header);

  for (i = 0; i < (bins2*bins1); ++i) {
    if (!(buffer = qfits_pixdump_int(iarray+i, 1, BPP_32_SIGNED))) {  
      free(iarray);
      if (hheader)
	qfits_header_destroy(hheader);
      return 0;
    }
    
    /* Put the buffer in the file */
    fwrite(buffer, 4, 1, stream);
    free(buffer);
  }
  free(iarray);

  /* Stretch the file to the right length */  
  fseek(stream, 0L, SEEK_END);
  j = (ftell(stream) + 1L)%2880;

  /* Now, in qfits they use NUL to pad a file, standard is ASCII
     blank, while I don't know whether the description I have is wrong
     or qfits. I believe qfits */
  
  for (i = 0; i <= 2880-j; ++i) {
    fputc(' ', stream);
  }

  /* Dump the hheader */
  if (hheader) {
    qfits_header_dump(hheader, stream);
    qfits_header_destroy(hheader);
  }

  /* finish */
  fclose(stream);

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Help function to ftstab_histo_2d */

static int histo_putarray(int column1, long startrow, long endrow, long bins1, double min1, double max1, double delta1, int *iarray)
{
  long offsetfirstcol1;
  long i,j;
  double adouble;

  /* Now sort the file */
  if (!ftstab_heapsort(column1, startrow, endrow)) {
    return 0;
  }

  --column1;

  fsetpos(stream_, &tablestart_);
  offsetfirstcol1 = ftell(stream_)+byteperow_*(startrow-1)+byteoffset_[column1];

  /* The file is sorted we can fill the array */
  j = 0;





  /* New experimental line */
  endrow = endrow-startrow+1;
  /* We search for the first column that is larger than min, before we make a check if there is any number that is in-between min and max */

  ftstab_get_value(endrow, column1+1, &adouble);


  if (adouble < min1) {
    ;
  }
  else {
    /* Find the first row with a value that is larger or equal min */
    while (getquickval(column1, offsetfirstcol1, j) < min1) {
      ++j;
    }

    /* Now the first bin has to be increased by one */
    min1 = min1+delta1;

    /* Now the array will be filled */
    i = 0;
    if (bins1 > 2) {
      while ((i < (bins1-1)) && (j < endrow)) {
	if (getquickval(column1, offsetfirstcol1, j) <= min1) {

	  iarray[i] = iarray[i]+1;
	  ++j;
	}
	else {
	  min1 = min1+delta1;
	  ++i;
	}    
      }

      /* We do the last loop seperately because of numerical errors that might have occured, i.e. min+n*delta != max */
      while (j < endrow) {
	if (getquickval(column1, offsetfirstcol1, j) <= max1) {
	  iarray[i] = iarray[i]+1;
	  ++j;
	}
	else
	  break;
      }
    }
    else {
      while (j < endrow) {
	if (getquickval(column1, offsetfirstcol1, j) <= max1) {
	  iarray[i] = iarray[i]+1;
	  ++j;
	}
	else
	  break;
      }
    }
  }

  return 1;
}



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns the current length of the table */
long ftstab_get_curlength_(void)
{
  if (!stream_)
    return 0L;

  return ftell(stream_);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns the current length of a row */
int ftstab_get_byteperow_(void)
{
  return byteperow_;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: ftstab.c,v $
   Revision 1.53  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.52  2011/05/10 00:30:15  jozsa
   Left work

   Revision 1.51  2011/03/23 22:32:42  jozsa
   Left work

   Revision 1.50  2010/07/28 23:04:19  jozsa
   Left work

   Revision 1.49  2009/05/26 07:56:41  jozsa
   Left work

   Revision 1.48  2007/08/22 15:58:41  gjozsa
   Left work

   Revision 1.47  2006/07/18 09:33:02  gjozsa
   Left work

   Revision 1.46  2005/04/20 13:26:25  gjozsa
   Left work

   Revision 1.45  2005/04/15 15:40:58  gjozsa
   Changed gtitln and gltitln, now an error report also if NULL is passed

   Revision 1.43  2005/04/14 10:32:14  gjozsa
   Left work

   Revision 1.42  2005/04/04 08:42:19  gjozsa
   Left work

   Revision 1.41  2005/03/26 17:52:10  gjozsa
   Another bug at about 4800, wrong adressing of values

   Revision 1.39  2005/03/26 10:06:36  gjozsa
   Another bug at line 4200

   Revision 1.38  2005/03/25 18:17:19  gjozsa
   Left work

   Revision 1.36  2005/03/25 14:41:27  gjozsa
   I introduced the bug, it should be ok now

   Revision 1.35  2005/03/25 12:55:40  gjozsa
   Either removed or created a nasty bug at about line 4302, question when recovering a file

   Revision 1.34  2005/03/22 12:07:52  gjozsa
   Added byteperow_ = 0 in line 4423, (clearhd)

   Revision 1.33  2005/03/19 17:55:52  gjozsa
   Left work

   Revision 1.32  2005/03/17 16:24:10  gjozsa
   bugfix

   Revision 1.31  2005/03/17 08:28:24  gjozsa
   Include documentation also in the source

   Revision 1.30  2005/03/11 17:45:54  gjozsa
   Left work

   Revision 1.29  2005/03/01 17:46:20  gjozsa
   Left work

   Revision 1.28  2005/02/21 11:23:44  gjozsa
   Added the recover functionality to checkin, now finished until some additional thing is needed

   Revision 1.26  2005/02/21 09:44:24  gjozsa
   Stable, tested, and nearly final version, added histogram functions

   Revision 1.25  2005/02/16 18:55:32  gjozsa
   Left work

   Revision 1.24  2005/02/16 13:30:25  gjozsa
   Largely debugged and tested, added histogram function

   Revision 1.23  2005/02/15 18:00:34  gjozsa
   Left work

   Revision 1.22  2005/02/15 15:37:12  gjozsa
   Left work

   Revision 1.21  2005/02/14 17:48:59  gjozsa
   Left work

   Revision 1.20  2005/02/11 17:37:13  gjozsa
   Left work

   Revision 1.19  2005/02/10 17:57:42  gjozsa
   Left work

   Revision 1.18  2005/02/10 16:15:30  gjozsa
   Implemented and tested heapsort

   Revision 1.17  2005/02/10 12:29:08  gjozsa
   Implemented and tested heapsort

   Revision 1.16  2005/02/09 17:52:19  gjozsa
   Left work

   Revision 1.15  2005/02/09 12:06:31  gjozsa
   Added and tested some internal functions

   Revision 1.14  2005/02/08 11:37:13  gjozsa
   Added and tested putval and putrow functions

   Revision 1.13  2005/02/07 16:06:04  gjozsa
   Quite tested version, added lots of functionality

   Revision 1.12  2005/02/05 16:20:51  gjozsa
   Left work

   Revision 1.11  2005/02/04 18:01:12  gjozsa
   Left work

   Revision 1.10  2005/02/03 17:23:56  gjozsa
   Left work

   Revision 1.9  2005/02/02 17:33:42  gjozsa
   Left work

   Revision 1.8  2005/02/01 17:45:53  gjozsa
   Left work

   Revision 1.7  2005/02/01 14:51:20  gjozsa
   Left work

   Revision 1.6  2005/01/28 17:29:03  gjozsa
   Tested and debugged

   Revision 1.5  2005/01/27 17:44:43  gjozsa
   Left work

   Revision 1.4  2005/01/26 17:09:38  gjozsa
   First stable and tested version

   Revision 1.3  2005/01/24 15:48:46  gjozsa
   Left work

   Revision 1.1  2
   ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file ftstab.h
   @brief Module to create and change long fits tables

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
   TFORMi is the numerical type (fits required keyword) of the column i

   TITLEi is the name of the column i
   TTYPEi is the physical type of the column i, e.g. VELO for velocity
   (has to be fits conform) TUNITi is the unit of the column i
   TSCALi describes the multiplicative factor with which the single
   values are multiplied to get to the real value in the column i (has
   to be fits conform)
   TZEROi describes the additional constant which is added to the
   single values to get to the real value in the column i (has to be
   fits conform)
   RADIi is the radius (double accuracy) of the column i (free
   keyword)
   GRIDi is the grid (accuracy adjusted to TTYPEi) of the column i
   (free keyword)
   TMAXi is the maximum (accuracy adjusted to TTYPEi) of the column i
   (free keyword)
   TMINi is the minimum (accuracy adjusted to TTYPEi) of the column i
   (free keyword)

   The keywords TTYPEi, TUNITi, TSCALi, TZEROi are tied to TITLEi, as
   a TITLEi gets a title number and the rest of the keywords is tied
   to the TITLE in a header item list (hdl). The hdl contains some
   predefined standard tuples. However, it can always be completely
   changed and analysed with the functions:

   ftstab_putlcoltitl(), ftstab_putlcolunit(), ftstab_putlcoltype(),
   ftstab_putcolunit(), ftstab_putcoltype(), ftstab_putcoltitl(),
   ftstab_gtitln_(), ftstab_glunit(), ftstab_gtzero(),
   ftstab_gtscal(), ftstab_gunit(), ftstab_gltype(), ftstab_gltitl(),
   ftstab_gtitl(), ftstab_hdlreset_(), ftstab_hdladditem()

   The second structure that controls the io is the column descriptor
   array (cda), in which the keywords TFORMi, TITLEi, RADIi, GRIDi,
   TMAXi, TMINi are controlled.  For the creation of a new table or
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
   int or double arrays of the length of a row. The functions ftstab_get_curlength_() and ftstab_get_byteperow_() serve to have control over the size of the table.

   Compilation of qfits to be used with this module:

   tar zxvf qfits-4.3.7-mod.tar.gz
   cd qfits-4.3.7-mod
   configure
   make
   
   The underscored functions are safe to use in a fortran code. They
   require a (pointer) variable once. It is not necessary to maintain
   the variable.

   Code example in testftstab.c

   $Source: /Volumes/DATA_J_II/data/CVS/tirific/include/ftstab.h,v $
   $Date: 2009/05/26 07:56:39 $
   $Revision: 1.28 $
   $Author: jozsa $
   $Log: ftstab.h,v $
   Revision 1.28  2009/05/26 07:56:39  jozsa
   Left work

   Revision 1.27  2007/08/22 15:58:34  gjozsa
   Left work

   Revision 1.26  2005/03/25 18:17:19  gjozsa
   Left work

   Revision 1.25  2005/02/21 11:23:44  gjozsa
   Added the recover functionality to checkin, now finished until some additional thing is needed

   Revision 1.23  2005/02/21 09:44:24  gjozsa
   Stable, tested, and nearly final version, added histogram functions

   Revision 1.22  2005/02/16 18:55:32  gjozsa
   Left work

   Revision 1.21  2005/02/16 13:30:26  gjozsa
   Largely debugged and tested, added histogram function

   Revision 1.20  2005/02/15 18:00:34  gjozsa
   Left work

   Revision 1.19  2005/02/15 15:37:11  gjozsa
   Left work

   Revision 1.18  2005/02/11 17:37:13  gjozsa
   Left work

   Revision 1.17  2005/02/10 17:57:42  gjozsa
   Left work

   Revision 1.16  2005/02/10 12:29:09  gjozsa
   Implemented and tested heapsort

   Revision 1.15  2005/02/09 17:52:19  gjozsa
   Left work

   Revision 1.14  2005/02/08 11:37:14  gjozsa
   Added and tested putval and putrow functions

   Revision 1.13  2005/02/07 16:05:49  gjozsa
   Quite tested version, added lots of functionality

   Revision 1.12  2005/02/04 18:01:12  gjozsa
   Left work

   Revision 1.11  2005/02/03 17:23:55  gjozsa
   Left work

   Revision 1.9  2005/02/01 17:45:53  gjozsa
   Left work

   Revision 1.8  2005/02/01 14:51:20  gjozsa
   Left work

   Revision 1.7  2005/01/28 17:29:23  gjozsa
   Tested and debugged

   Revision 1.6  2005/01/27 17:44:43  gjozsa
   Left work

   Revision 1.5  2005/01/26 17:26:14  gjozsa
   Left work

   Revision 1.4  2005/01/26 17:10:04  gjozsa
   First stable and tested version

   Revision 1.3  2005/01/24 15:48:45  gjozsa
   Left work

   Revision 1.1  2005/01/17 12:16:04  gjozsa
   added to cvs control


*/
/* ------------------------------------------------------------ */



/* Include guard */
#ifndef FTSTAB_H
#define FTSTAB_H


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <stdio.h>



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def COLTYPE_DEFAULT @brief Default value for the Column type
*/
/* ------------------------------------------------------------ */
#define COLTYPE_DEFAULT 0
#define COLTYPE_FLOAT   1
#define COLTYPE_CHAR    2
#define COLTYPE_INT     3
#define COLTYPE_DOUBLE  4


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def COLRADI_DEFAULT @brief Default values for the Column radius
*/
/* ------------------------------------------------------------ */
#define COLRADI_DEFAULT -1.0
#define COLRADI_FLOAT   -1.0
#define COLRADI_CHAR    -1
#define COLRADI_INT     -1
#define COLRADI_DOUBLE  -1.0

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def COLGRID_DEFAULT @brief Default value for the Column delta
*/
/* ------------------------------------------------------------ */
#define COLGRID_DEFAULT 0.0
#define COLGRID_FLOAT   0.0
#define COLGRID_CHAR    0
#define COLGRID_INT     0
#define COLGRID_DOUBLE  0.0


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

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_hdladditem(char *titl, char *ttype, char *tunit, double tzero, double tscal);
   @brief Modify the header item list

   The header item list contains the allowed names to put into the
   header and also the context, i.e. the fits ttype, tunit, tscal,
   tzero that is tied to the name in this module. The function
   searches the list for the item. If it does not find the item, it
   appends it to the list. Whitespaces are fully interpreted as characters.

   @param titl   (char *) The title (name of the item) (18 chars regarded)
   @param ttype	 (char *) The ttype belonging to item  (18 chars regarded)
   @param tunit	 (char *) The tunit belonging to item  (18 chars regarded)
   @param tzero	 (double) The tzero belonging to item  
   @param tscal	 (double) The tscale belonging to item 

   @return (success) int ftstab_hdladditem: Item number
           (error) -1 in case of wrong input
 */
/* ------------------------------------------------------------ */
int ftstab_hdladditem(char *titl, char *ttype, char *tunit, double tzero, double tscal);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_hdlreset(void);
   @brief Resets the hdllist

   Refreshes the header item list, i.e., puts it to the default values.

   @return (success) int ftstab_hdlreset: 1
           (error) 0 if the item does not exist
 */
/* ------------------------------------------------------------ */
int ftstab_hdlreset_(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  char *ftstab_gtitl(int titl)
  @brief Get a char array belonging to the COLTITL titl

  Returns an allocated char array with the title string

  @return char *gtitl: Char array with the title string
*/
/* ------------------------------------------------------------ */
 char *ftstab_gtitl(int titl);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  char *ftstab_gltitl(int titl)
  @brief Get a char array belonging to the COLTITL titl

  Returns an allocated char array with the full title string as is in the fits header.

  @return char *gtitl: Char array with the title string
*/
/* ------------------------------------------------------------ */
 char *ftstab_gltitl(int titl);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  char *ftstab_gtype(int titl)
  @brief Get a char array belonging to the COLTITL titl

  Returns an allocated char array with the type string

  @return char *gtitl: Char array with the type string
*/
/* ------------------------------------------------------------ */
 char *ftstab_gtype(int titl);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  char *ftstab_gltype(int titl)
  @brief Get a char array belonging to the COLTITL titl

  Returns an allocated char array with the full type string as is in the fits header.

  @return char *gltype: Char array with the type string
*/
/* ------------------------------------------------------------ */
 char *ftstab_gltype(int titl);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  char *ftstab_gunit(int titl)
  @brief Get a char array belonging to the COLTITL titl

  Returns an allocated char array with the unit string

  @return char *gunit: Char array with the unit string
*/
/* ------------------------------------------------------------ */
 char *ftstab_gunit(int titl);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  double ftstab_gtscal(int coltitle)
  @brief Get a tscale belonging to the COLTITL titl

  @return (success) double gtscal: The scale belonging to the titl
          (error) DBL_MAX
*/
/* ------------------------------------------------------------ */
 double ftstab_gtscal(int coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  double ftstab_gtscal_(int _coltitle)
  @brief Get a tscale belonging to the COLTITL titl

  @return (success) double gtscal: The scale belonging to the titl
          (error) DBL_MAX
*/
/* ------------------------------------------------------------ */
 double ftstab_gtscal_(int *coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  double ftstab_gtzero(int coltitle)
  @brief Get a tzero belonging to the COLTITL titl

  @return (success) double gtzero: The zero belonging to the titl
          (error) DBL_MAX
*/
/* ------------------------------------------------------------ */
 double ftstab_gtzero(int coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  double ftstab_gtzero_(int *coltitle)
  @brief Get a tscale belonging to the COLTITL titl

  @return (success) double gtzero: The zero belonging to the titl
          (error) DBL_MAX
*/
/* ------------------------------------------------------------ */
 double ftstab_gtzero_(int *coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  char *ftstab_glunit(int titl)
  @brief Get a char array belonging to the COLTITL titl

  Returns an allocated char array with the full unit string as is in the fits header.

  @return char *gunit: Char array with the unit string
*/
/* ------------------------------------------------------------ */
 char *ftstab_glunit(int titl);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_gtitln_(char *titl)
  @brief Get COLTITL number from string

  Returns the number belonging to the short title string

  @return int gtitln: Number belonging to the title string
*/
/* ------------------------------------------------------------ */
 int ftstab_gtitln_(char *titl);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_gltitln_(char *titl)
  @brief Get COLTITL number from string

  Returns the number belonging to the title string as found in a header

  @return int gtitln: Number belonging to the title string
*/
/* ------------------------------------------------------------ */
 int ftstab_gltitln_(char *titl);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn int ftstab_putcoltitl(char *key, int coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key.

  @param key (char *) the string to put the string 
  @param colname (int) integer defined by the symbolic constants in
  the .h

  @return (success) int putcoltitle: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
int ftstab_putcoltitl(char *key, int coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn int ftstab_putcoltitl(char *key, int *coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key.

  @param key     (char *) The string to put the string 
  @param colname (int *)  Integer defined by the symbolic constants in
  the .h

  @return (success) int putcoltitle: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
int ftstab_putcoltitl_(char *key, int *coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_putcoltype(char *key, int coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key.

  @param key (char *) the string to put the string 
  @param colname (int) integer defined by the symbolic constants in
  the .h

  @return (success) int putcoltitle: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
 int ftstab_putcoltype(char *key, int coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_putcoltype_(char *key, int *coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key.

  @param key (char *) the string to put the string 
  @param colname (int *) integer defined by the symbolic constants in
  the .h

  @return (success) int putcoltitle: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
 int ftstab_putcoltype_(char *key, int *coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_putcolunit(char *key, int coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key.

  @param key (char *) the string to put the string 
  @param colname (int) integer defined by the symbolic constants in
  the .h

  @return (success) int putcoltitle: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
 int ftstab_putcolunit(char *key, int coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_putcolunit(char *key, int *coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key.

  @param key (char *) the string to put the string 
  @param colname (int *) integer defined by the symbolic constants in
  the .h

  @return (success) int putcoltitle_: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
 int ftstab_putcolunit_(char *key, int *coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_putlcoltype(char *key, int coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key, as is in the fits header.

  @param key (char *) the string to put the string 
  @param colname (int) integer defined by the symbolic constants in
  the .h

  @return (success) int putcoltitle: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
 int ftstab_putlcoltype(char *key, int coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_putlcoltype_(char *key, int *coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key, as
  is in the fits header.

  @param key (char *) the string to put the string 
  @param colname (int *) integer defined by the symbolic constants in
  the .h

  @return (success) int ftstab_putlcoltype: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
 int ftstab_putlcoltype_(char *key, int *coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_putlcolunit(char *key, int coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key, as
  is in the fits header.

  @param key (char *) the string to put the string 
  @param colname (int) integer defined by the symbolic constants in
  the .h

  @return (success) int putcoltitle: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
 int ftstab_putlcolunit(char *key, int coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_putlcolunit_(char *key, int *coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key, as
  is in the fits header.

  @param key (char *) the string to put the string 
  @param colname (int *) integer defined by the symbolic constants in
  the .h

  @return (success) int ftstab_putlcolunit_: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
 int ftstab_putlcolunit_(char *key, int *coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_putlcoltitl(char *key, int coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key, as is in the fits header.

  @param key (char *) the string to put the string 
  @param colname (int) integer defined by the symbolic constants in
  the .h

  @return (success) int putcoltitle: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
int ftstab_putlcoltitl(char *key, int coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/*
  @fn  int ftstab_putlcoltitl_(char *key, int *coltitle)
  @brief Puts the string that belongs to coltitle into key

  In the fitstable each column gets a title according to the symbolic
  constants defined in the .h. This function puts the string according
  to the number given by the symbolic constant into the string key, as
  is in the fits header.

  @param key (char *) the string to put the string 
  @param colname (int *) integer defined by the symbolic constants in
  the .h

  @return (success) int ftstab_putlcoltitl_: coltitle\n
          (error) -1
*/
/* ------------------------------------------------------------ */
int ftstab_putlcoltitl_(char *key, int *coltitle);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_inithd(int numberofcols)
   @brief Reserves space for the header descriptor

   Initialises the internal struct that contains the information about
   the columns of the table to contain the description of numberofcols
   columns.

   @param numberofcols (int) The number of columns in the table

   @return (success) int initftstbl 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_inithd(int numberofcols);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_inithd_(int *numberofcols)
   @brief Reserves space for the header descriptor

   Initialises the internal struct that contains the information about
   the columns of the table to contain the description of numberofcols
   columns.  This function works with a pointer instead of a given
   direct input. No care has to be taken of the pointer after the
   function call, as it will nevertheless produce a local copy before
   working with the parameter(s).

   @param numberofcols (int *) The number of columns in the table

   @return (success) int initftstbl 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_inithd_(int *numberofcols);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_fillhd(int column, int title, int type, double radius, double grid)
   @brief Fills column column of the table description object with the
   given values

   Tries to fill column column of the table description object with
   the given values for the name, type, radius, and grid spacing in
   the table. Name and type are given by integers defined in the
   corresponding symbolic constants. Returns 0 if not successful,
   i.e. the structure has not been initialised, or if the column is
   non-existent. The change of the numerical type is dangerous if a
   table is open. The module is not safe with respect to that.

   @param column (int)   Column number (starting at 0)
   @param title   (int)   Name of column
   @param type   (int)   Numerical type of column, CHAR, INT, FLOAT, DOUBLE 
   @param radius (double) Radius of column
   @param grid   (double) Grid spacing of column


   @return (success) int fillhd 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_fillhd(int column, int title, int type, double radius, double grid);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_fillhd_(int *column, int *name, int *type, double *radius, double *grid)
   @brief Fills column column of the table description object with the
   given values

   Tries to fill column column of the table description object with
   the given values for the name, type, radius, and grid spacing in
   the table. Name and type are given by integers defined in the
   corresponding symbolic constants. Returns 0 if not successful,
   i.e. the structure has not been initialised, or if the column is
   non-existent.

   This function works with a pointer instead of a given direct
   input. No care has to be taken of the pointer after the function
   call, as it will nevertheless produce a local copy before working
   with the parameter(s).

   @param column (int *)   Column number (starting at 0)
   @param name   (int *)   Name of column
   @param type   (int *)   Numerical type of column, CHAR, INT, FLOAT, DOUBLE 
   @param radius (double *) Radius of column
   @param grid   (double *) Grid spacing of column

   @return (success) int fillhd 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_fillhd_(int *column, int *titl, int *type, double *radius, double *grid);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_title_(int *array) 

   @brief Get the titles of the columns into an array

   The tilte shortcuts as defined above are put into an array that has
   to be of length ftstab_get_colnr_(), as can be allocated using
   ftstab_get_intarr().

   @param array (int *) An array large enough to contain
   ftstab_get_colnr_() numbers

   @return int ftstab_get_title_: Number of columns
 */
/* ------------------------------------------------------------ */
int ftstab_get_title_(int *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_type_(int *array) 

   @brief Get the numerical types of the columns into an array

   The type shortcuts as defined above are put into an array that has
   to be of length ftstab_get_colnr_(), as can be allocated using
   ftstab_get_intarr().

   @param array (int *) An array large enough to contain
   ftstab_get_colnr_() numbers

   @return int ftstab_get_type_: Number of columns
 */
/* ------------------------------------------------------------ */
int ftstab_get_type_(int *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_radi_(double *array) 

   @brief Get the radii of the columns into an array

   The radii of a column are put into an array that has
   to be of length ftstab_get_colnr_(), as can be allocated using
   ftstab_get_dblarr().

   @param array (double *) An array large enough to contain
   ftstab_get_colnr_() numbers

   @return int ftstab_get_radi_: Number of columns
 */
/* ------------------------------------------------------------ */
int ftstab_get_radi_(double *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_grid_(double *array) 

   @brief Get the grid spacing of the columns into an array

   The grid spacings of a column are put into an array that has
   to be of length ftstab_get_colnr_(), as can be allocated using
   ftstab_get_dblarr().

   @param array (double *) An array large enough to contain
   ftstab_get_colnr_() numbers

   @return int ftstab_get_grid_: Number of columns
 */
/* ------------------------------------------------------------ */
int ftstab_get_grid_(double *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_maxi_(double *array) 

   @brief Get the maxima of the columns into an array

   The maxima of a column are put into an array that has
   to be of length ftstab_get_colnr_(), as can be allocated using
   ftstab_get_dblarr().

   @param array (double *) An array large enough to contain
   ftstab_get_colnr_() numbers

   @return int ftstab_get_maxi_: Number of columns
 */
/* ------------------------------------------------------------ */
int ftstab_get_maxi_(double *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_mini_(double *array) 

   @brief Get the radii of the columns into an array

   The minima of a column are put into an array that has
   to be of length ftstab_get_colnr_(), as can be allocated using
   ftstab_get_dblarr().

   @param array (double *) An array large enough to contain
   ftstab_get_colnr_() numbers

   @return int ftstab_get_mini_: Number of columns
 */
/* ------------------------------------------------------------ */
int ftstab_get_mini_(double *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_coltit(int column) 

   @brief Get the title number of column column

   @param column (int) The column (start with 1)

   @return (success) int ftstab_get_coltit_: Number of column
           (error) -1
 */
/* ------------------------------------------------------------ */
int ftstab_get_coltit(int column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_coltit_(int *column) 

   @brief Get the title number of column column

   @param column (int *) The column (start with 1)

   @return (success) int ftstab_get_coltit_: Number of column
           (error) -1
 */
/* ------------------------------------------------------------ */
int ftstab_get_coltit_(int *column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_coltyp(int column) 

   @brief Get the identifier of the numerical type of column column

   @param column (int) The column (start with 1)

   @return (success) int ftstab_get_coltyp: Numerical type of column
           (error) -1
 */
/* ------------------------------------------------------------ */
int ftstab_get_coltyp(int column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_coltyp_(int *column) 

   @brief Get the identifier of the numerical type of column column

   @param column (int) The column (start with 1)

   @return (success) int ftstab_get_coltyp_: Numerical type of column
           (error) -1
 */
/* ------------------------------------------------------------ */
int ftstab_get_coltyp_(int *column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn double ftstab_get_colrad(int column) 

   @brief Get the radius of column column

   @param column (int) The column (start with 1)

   @return (success) double ftstab_get_colrad: Radius of column
           (error) DBL_MAX
 */
/* ------------------------------------------------------------ */
double ftstab_get_colrad(int column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn double ftstab_get_colrad_(int *column) 

   @brief Get the radius of column column

   @param column (int *) The column (start with 1)

   @return (success) double ftstab_get_colrad: Radius of column
           (error) DBL_MAX
 */
/* ------------------------------------------------------------ */
double ftstab_get_colrad_(int *column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn double ftstab_get_colgrd(int column) 

   @brief Get the grid of column column

   @param column (int) The column (start with 1)

   @return (success) double ftstab_get_colgrd: Grid of column
           (error) DBL_MAX
 */
/* ------------------------------------------------------------ */
double ftstab_get_colgrd(int column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn double ftstab_get_colgrd_(int *column) 

   @brief Get the grid of column column

   @param column (int *) The column (start with 1)

   @return (success) double ftstab_get_colgrd: Grid of column
           (error) DBL_MAX
 */
/* ------------------------------------------------------------ */
double ftstab_get_colgrd_(int *column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn double ftstab_get_colmax(int column) 

   @brief Get the current maximum of column column

   The function returnst the current maximum of the column. To be sure
   that this is the right value, call ftstab_get_minmax().

   @param column (int) The column (start with 1)

   @return (success) double ftstab_get_colmax: Maximum of column
           (error) DBL_MAX
 */
/* ------------------------------------------------------------ */
double ftstab_get_colmax(int column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn double ftstab_get_colmax_(int *column) 

   @brief Get the current maximum of column column

   The function returnst the current maximum of the column. To be sure
   that this is the right value, call ftstab_get_minmax().

   @param column (int *) The column (start with 1)

   @return (success) double ftstab_get_colmax: Maximum of column
           (error) DBL_MAX
 */
/* ------------------------------------------------------------ */
double ftstab_get_colmax_(int *column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn double ftstab_get_colmin(int column) 

   @brief Get the current minimum of column column

   The function returnst the current maximum of the column. To be sure
   that this is the right value, call ftstab_get_minmax().

   @param column (int) The column (start with 1)

   @return (success) double ftstab_get_colmin: Minimum of column
           (error) DBL_MAX
 */
/* ------------------------------------------------------------ */
double ftstab_get_colmin(int column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn double ftstab_get_colmin_(int *column) 

   @brief Get the current minimum of column column

   The function returnst the current minimum of the column. To be sure
   that this is the right value, call ftstab_get_minmax().

   @param column (int *) The column (start with 1)

   @return (success) double ftstab_get_colmin: Minimum of column
           (error) DBL_MAX
 */
/* ------------------------------------------------------------ */
double ftstab_get_colmin_(int *column);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_findminmax(long begin, long end) 

   @brief Correct the minima and the maxima towards the minima and maxima between begin and end row

   Searches the minima and maxima for each row in-between begin and
   end (starting with 1) and puts the found values in the header
   description array. They are not applied to the
   header. ftstab_clearhd(0) should follow if one wants to apply the
   information to the file.

   @param start (long) Start row
   @param end   (long) End row

   @return (success) int int ftstab_findminmax: 1
           (error) 0 
 */
/* ------------------------------------------------------------ */
int ftstab_findminmax(long start, long end);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_findminmax_(long *begin, long *end) 

   @brief Correct the minima and the maxima towards the minima and maxima between begin and end row

   Searches the minima and maxima for each row in-between begin and
   end (starting with 1) and puts the found values in the header
   description array. They are not applied to the
   header. ftstab_clearhd(0) should follow if one wants to apply the
   information to the file.

   @param start (long) Start row
   @param end   (long) End row

   @return (success) int int ftstab_findminmax_: 1
           (error) 0 
 */
/* ------------------------------------------------------------ */
int ftstab_findminmax_(long *start, long *end);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_genhd(char n)
   @brief Generate a header

   Generates a header not yet attached to a file that can be
   manipulated. If n is 0, the header will be generated from the
   information passed with ftstab_inithd and ftstab_fillhd to be set
   before the bintable. If n is not 0, a default header will be
   generated that is appropriate to append to the file to store
   additional information. Both headers can be manipulated with
   ftstab_addcard(). It is strongly recommended not to do so if
   information after the header (a table, other hdu's) is present.  If
   a header is already present this will be an error and no changes
   are applied.

   @param n (char)  header to adress, main (0), appendix (1)

   @return (success) int genhd 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_genhd(char n);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_genhd_(char *n)
   @brief Generate a header

   Generates a header not yet attached to a file that can be
   manipulated. If *n is 0, the header will be generated from the
   information passed with ftstab_inithd and ftstab_fillhd to be set
   before the bintable. If *n is 1, a default header will be generated
   that is appropriate to append to the file to store additional
   information. Both headers can be manipulated with
   ftstab_addcard(). It is strongly recommended not to do so if
   information after the header (a table, other hdu's) is present.

   @param n (char *) header to adress, main (0), appendix (1)

   @return (success) int fillhd 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_genhd_(char *n);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_clearhd(char n)
   @brief Destroys or updates a header if possible

   Deletes a header if possible. If n is 0, then the main header will
   be deleted if possible. If the header is blocked, it will be
   updated with all information contained in the header description
   object, but no cards are deleted. If n is 1, the history header
   will be deleted.

   @param n (char) header to adress, main (0), appendix (1)

   @return (success) int  ftstab_clearhd: 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_clearhd(char n);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_clearhd_(char *n)
   @brief Destroys or updates a header if possible

   Deletes a header if possible. If n is 0, then the main header will
   be deleted if possible. If the header is blocked, it will be
   updated with all information contained in the header description
   object, but no cards are deleted. If n is 1, the history header
   will be deleted.

   @param n (char *) header to adress, main (0), appendix (1)

   @return (success) int  ftstab_clearhd: 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_clearhd_(char *n);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_getcard(char n, char *key, char *string, int what)
   @brief Get a full card image from a header

   Returns the content of a header card image. string should have
   reserved space to contain 81 characters (80 + terminator). what can
   be: 0: whole line, 1: value, 2: comment

   @param n      (char)   header to check for, main (0), appendix (other)
   @param key    (char *) The key
   @param string (char *) a char array to write the card in
   @param what   (int)    The desired information
   @return (success) int fillhd 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_getcard(char n, char *key, char *string, int what);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_getcard_(char *n, char *string)
   @brief Generate a header

   Returns the content of a header card image. string should have
   reserved space to contain 81 characters (80 + terminator). what can
   be: 0: whole line, 1: value, 2: comment

   @param n      (char *) header to check for, main (0), appendix (other)
   @param key    (char *) The key
   @param string (char *) a char array to write the card in
   @param what   (int)    The desired information
   @return (success) int fillhd 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_getcard_(char *n, char *key, char *string, int *what);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_putcard(char n, char *key, char *value)
   @brief Add a card to a header

   Adds a card to a header with the value contained in value. Except
   for a blank HISTORY or COMMENT key, the function will update header
   items if they are already present. For a blank, HISTORY, or COMMENT
   key, the values are appended. In case of a blank, HISTORY, or
   COMMENT key, the value may contain 70 characters, in all other
   cases, it may contain 20 characters. For this the user has to
   care. For a blocked header, appending a card is not possible (see
   ftstab_fopen).

   @param n      (char *) header to check for, main (0), appendix (1) 
   @param string (char *) a char array to write the card in

   @return (success) int fillhd 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_putcard(char n, char *keyl, char *valuel);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_putcard_(char *n, char *key, char *value)
   @brief Add a card to a header

   Adds a card to a header with the value contained in value. Except
   for a blank HISTORY or COMMENT key, the function will update header
   items if they are already present. For a blank, HISTORY, or COMMENT
   key, the values are appended. In case of a blank, HISTORY, or
   COMMENT key, the value may contain 70 characters, in all other
   cases, it may contain 20 characters. For this the user has to care.
   For a blocked header, appending a card is not possible.

   @param n      (char *) header to check for, main (0), appendix (1)
   @keyl         (char *) the key to put in 
   @param string (char *) a char array to write the card in

   @return (success) int fillhd 1 \n
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_putcard_(char *n, char *keyl, char *valuel);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_putminmax_(void) 

   @brief Changes the minmax information to the current value in the
   header of an open fits table

   Puts the minimum and maximum information for each column in the
   table as it is tracked actually to the header.
   
   @return (success) int putminmax_; 1
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_putminmax_(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_fopen(char *filename, int next, char mode, char hheader) 

   @brief Opens a table fits file for ftstab use

   Depending on the stage of the information that has already been
   acquired, this function acts differently. The basic function is to
   open a stream for reading and writing an ftstab fits table. Mode
   can be 0, 1, 2, 3, 4. If no file with the given name is present
   ftstab_fopen will create one, provided no other file is processed
   (i.e., if ftstab_fopen() has not been called before without calling
   ftstab_close_() afterwards) and enough information is present (At
   least one successful call of ftstab_inithd() without calling
   ftstab_flush_()). If a file with the filename is present, the
   function will act in four ways:

   mode 0: "nothing" ftstab_fopen returns 0.

   mode 1: "overwrite" Try creating a new file overwriting the old
   one. If this is not possible, return 0 and leave the file
   untouched.

   mode 2: "append" If information is already present (call of
   ftstab_inithd() and subsequent calls of ftstab_fillhd()) the file
   will be checked against the information present in the file.

   If the requested extension is not present (see also documentation
   on the hheader parameter) in the file, the extension will be
   created, if possible (call of ftstab_inithd() without calling
   ftstab_flush_()), while then being the last extension of the file,
   except for a possible history extension that is attached. So, a
   very high extension number (or an impossible extension number)
   results in the attachment of the new extension.

   To load a present extension, the file must contain the same number
   of columns, the same types and names of columns, the same radii and
   grid spacings in the same order. If the file doesn't match these
   criteria, no changes will be applied and a 5 is returned.  If it
   does it will be opened for read and write access.

   If no information is present (no call of ftstab_inithd() or call of
   ftstab_flush), the information will be possibly created from the
   file and the file will be opened. Additional incoming rows will be
   placed at the end of the file according to fits rules. In case of
   appending rows to a intermediate extension, follow-up extensions
   will maybe be overwritten and destroyed, destroying the file.

   mode 3: "append enforce write" acts like "append", with the
   additive that in the case of a mismatch betweeen a present
   description of the table and the file content, the file will be
   destroyed and a new file will be created.

   mode 4: "append enforce" acts like "append", with the additive that
   in the case of a mismatch betweeen a present description of the
   table and the extension header content, the file information
   following the extension adressed (except maybe a history header)
   will be destroyed and a new extension will be created at the
   requested place.

   There is the possibility of a history header to append to the
   file. Aswell, a history header could already be existent at the end
   of a file to open. The keyword hheader controls how to behave if a
   history header (an image extension with naxis=0 and no attached
   data) is present. If 0, the last extension will be treated like a
   normal extension. If not 0, ftstab_fopen updates the header with
   possible information in the present history header (if none was
   created, nothing will be updated) and stores it as the history
   header. Furthermore ftstab_fopen acts as if the last header was not
   present.

   For the write access to tables, the functionality that enlarges the
   content of an item, header, or table, is disabled, if the resulting
   fits file is endangered to be corrupted. If the opened extension is
   not the last one, the table and the header can be changed, but no
   items can be added. If the opened extension is the last one (minus
   possibly a history header), the header will be blocked if there is
   data in the table, while the table can be enlarged, otherways,
   everything is writeable.

   @param filename (char *) The filename
   @param next     (int)    The extension number
   @param mode     (char)   Mode to start ftstab_fopen with.
   @param hheader  (char)   Treatment of a history header

   @return (success) int ftstab_fopen_: 0\n
           (error) 1: stream already opened with a file\n

                   2: Couldn't open a new file: No header descriptor
                   object or not possible\n

                   3: Couldn't overwrite a file: No header descriptor
                   object or not possible\n

		   4: None of the accepted modes was given\n

		   5: mode2, but there are inconsistencies: No real
		   table object adressed or wrong column number\n

		   6: mode2, but there are inconsistencies: table
		   doesn't have ftstab format or NAXIS1 (bytes per
		   row) wrong\n

		   7: mode2, but there are inconsistencies: table does
		   have ftstab format but doesn't comply with the
		   header descriptor information\n

		   8: Allocation error or error opening the file: Some
		   present information can be destroyed, ftsttab_flush
		   and filling everything again recommended\n

		   9: mode3, but there are inconsistencies: table
		   doesn't have ftstab format or NAXIS1 (bytes per
		   row) wrong and overwriting failed\n

		   10: mode3, but there are inconsistencies: table
		   does have ftstab format but doesn't comply with the
		   header descriptor information and overwriting
		   failed\n

		   11: mode3, but there are inconsistencies: No real
		   table object adressed or wrong column number and
		   overwriting failed\n

		   12: mode3, but there are inconsistencies: table
		   doesn't have ftstab format or NAXIS1 (bytes per
		   row) wrong, no further information was given\n

		   13: mode3: Allocation error or error opening the
		   file: Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   14: mode3, but there are inconsistencies: No real
		   table object adressed or wrong column number and no
		   further information\n

		   15: Allocation error or error opening the file:
		   Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   16: mode4, but there are inconsistencies: table
		   doesn't have ftstab format and overwriting failed :
		   Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   17: mode4, but there are inconsistencies: table
		   does have ftstab format but doesn't comply with the
		   header descriptor information and overwriting
		   failed : Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   18: Allocation error or error opening the file:
		   Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   19: mode4, but there are inconsistencies: No real
		   table object adressed or wrong column number and no
		   further information\n

		   20: mode4, but there are inconsistencies: table
		   doesn't have ftstab format or NAXIS1 (bytes per
		   row) wrong, no further information was given\n

		   21: mode4: Allocation error or error opening the
		   file: Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   22: No header item description table could be
		   allocated, sure sign for not enough memory
*/
/* ------------------------------------------------------------ */
int ftstab_fopen(char *filename, int next, char mode, char hheader);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_fopen_(char *filename, int *next, char *mode, char *hheader) 

   @brief Opens a table fits file for ftstab use

   Depending on the stage of the information that has already been
   acquired, this function acts differently. The basic function is to
   open a stream for reading and writing an ftstab fits table. Mode
   can be 0, 1, 2, 3, 4. If no file with the given name is present
   ftstab_fopen will create one, provided no other file is processed
   (i.e., if ftstab_fopen() has not been called before without calling
   ftstab_close_() afterwards) and enough information is present (At
   least one successful call of ftstab_inithd() without calling
   ftstab_flush_()). If a file with the filename is present, the
   function will act in four ways:

   mode 0: "nothing" ftstab_fopen returns 0.

   mode 1: "overwrite" Try creating a new file overwriting the old
   one. If this is not possible, return 0 and leave the file
   untouched.

   mode 2: "append" If information is already present (call of
   ftstab_inithd() and subsequent calls of ftstab_fillhd()) the file
   will be checked against the information present in the file.

   If the requested extension is not present (see also documentation
   on the hheader parameter) in the file, the extension will be
   created, if possible (call of ftstab_inithd() without calling
   ftstab_flush_()), while then being the last extension of the file,
   except for a possible history extension that is attached. So, a
   very high extension number (or an impossible extension number)
   results in the attachment of the new extension.

   To load a present extension, the file must contain the same number
   of columns, the same types and names of columns, the same radii and
   grid spacings in the same order. If the file doesn't match these
   criteria, no changes will be applied and a 5 is returned.  If it
   does it will be opened for read and write access.

   If no information is present (no call of ftstab_inithd() or call of
   ftstab_flush), the information will be possibly created from the
   file and the file will be opened. Additional incoming rows will be
   placed at the end of the file according to fits rules. In case of
   appending rows to a intermediate extension, follow-up extensions
   will maybe be overwritten and destroyed, destroying the file.

   mode 3: "append enforce write" acts like "append", with the
   additive that in the case of a mismatch betweeen a present
   description of the table and the file content, the file will be
   destroyed and a new file will be created.

   mode 4: "append enforce" acts like "append", with the additive that
   in the case of a mismatch betweeen a present description of the
   table and the extension header content, the file information
   following the extension adressed (except maybe a history header)
   will be destroyed and a new extension will be created at the
   requested place.

   There is the possibility of a history header to append to the
   file. Aswell, a history header could already be existent at the end
   of a file to open. The keyword hheader controls how to behave if a
   history header (an image extension with naxis=0 and no attached
   data) is present. If 0, the last extension will be treated like a
   normal extension. If not 0, ftstab_fopen updates the header with
   possible information in the present history header (if none was
   created, nothing will be updated) and stores it as the history
   header. Furthermore ftstab_fopen acts as if the last header was not
   present.

   For the write access to tables, the functionality that enlarges the
   content of an item, header, or table, is disabled, if the resulting
   fits file is endangered to be corrupted. If the opened extension is
   not the last one, the table and the header can be changed, but no
   items can be added. If the opened extension is the last one (minus
   possibly a history header), the header will be blocked if there is
   data in the table, while the table can be enlarged, otherways,
   everything is writeable.

   @param filename (char *) The filename
   @param next     (int *)  The extension number
   @param mode     (char *) Mode to start ftstab_fopen with.
   @param hheader  (char *) Treatment of a history header

   @return (success) int ftstab_fopen_: 0\n
           (error) 1: stream already opened with a file or file present with mode 0\n

                   2: Couldn't open a new file: No header descriptor
                   object or not possible\n

                   3: Couldn't overwrite a file: No header descriptor
                   object or not possible\n

		   4: None of the accepted modes was given\n

		   5: mode2, but there are inconsistencies: No real
		   table object adressed or wrong column number\n

		   6: mode2, but there are inconsistencies: table
		   doesn't have ftstab format or NAXIS1 (bytes per
		   row) wrong\n

		   7: mode2, but there are inconsistencies: table does
		   have ftstab format but doesn't comply with the
		   header descriptor information\n

		   8: Allocation error or error opening the file: Some
		   present information can be destroyed, ftsttab_flush
		   and filling everything again recommended\n

		   9: mode3, but there are inconsistencies: table
		   doesn't have ftstab format or NAXIS1 (bytes per
		   row) wrong and overwriting failed\n

		   10: mode3, but there are inconsistencies: table
		   does have ftstab format but doesn't comply with the
		   header descriptor information and overwriting
		   failed\n

		   11: mode3, but there are inconsistencies: No real
		   table object adressed or wrong column number and
		   overwriting failed\n

		   12: mode3, but there are inconsistencies: table
		   doesn't have ftstab format or NAXIS1 (bytes per
		   row) wrong, no further information was given\n

		   13: mode3: Allocation error or error opening the
		   file: Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   14: mode3, but there are inconsistencies: No real
		   table object adressed or wrong column number and no
		   further information\n

		   15: Allocation error or error opening the file:
		   Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   16: mode4, but there are inconsistencies: table
		   doesn't have ftstab format and overwriting failed :
		   Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   17: mode4, but there are inconsistencies: table
		   does have ftstab format but doesn't comply with the
		   header descriptor information and overwriting
		   failed : Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   18: Allocation error or error opening the file:
		   Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   19: mode4, but there are inconsistencies: No real
		   table object adressed or wrong column number and no
		   further information\n

		   20: mode4, but there are inconsistencies: table
		   doesn't have ftstab format or NAXIS1 (bytes per
		   row) wrong, no further information was given\n

		   21: mode4: Allocation error or error opening the
		   file: Some present information can be destroyed,
		   ftsttab_flush and filling everything again
		   recommended\n

		   22: No header item description table could be
		   allocated, sure sign for not enough memory
*/
/* ------------------------------------------------------------ */
int ftstab_fopen_(char *filename, int *next, char *mode, char *hheader);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_appendrow_(double *row) 

   @brief Append a row to an open table

   Appends a row in binary format to an opened table (initialisation
   be opencolfts) that is private to the module. All input values are
   double format while they will be stored in the user predefined
   numerical type. The specification of the content of a column is
   done by fillhd or fillhd_ respectively.  The user has to care that
   row contains the number of fields specified by initftstbl. The
   function does not work if the table is blocked.

   @param row (double *) A double array with the contents of the row
   
   @return (success) int appendrow_; 1
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_appendrow_(double *row);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_putrow(long rownumber, double *row) 

   @brief Change a row in an open table

   Changes a row in binary format in an opened table (initialisation
   by opencolfts) that is private to the module. All input values are
   double format while they will be stored as the user predefined type. The specification of the content of a column is
   done by fillhd or fillhd_ respectively.  The user has to care that
   row contains the number of fields specified by initftstbl.

   @param row       (double *) A double array with the contents of the row
   @param rownumber (long)     The number of the row to be changed    

   @return (success) int putrow: 1
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_putrow(long rownumber, double *row);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_putval(long rownumber, double *row, double value) 

   @brief Change a row in an open table

   Changes a value in an opened table (initialisation by opencolfts)
   that is private to the module. All input values are double format
   while they will be stored as the user predefined type. The
   specification of the content of a column is done by fillhd or
   fillhd_ respectively.  The user has to care that row contains the
   number of fields specified by initftstbl.

   @param value     (double) The new value
   @param rownumber (long)   The number of the row to be changed    
   @param colnumber (int)    The number of the column to be changed    
   @return (success) int putval; 1
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_putval(long rownumber, int colnumber, double value);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_putval_(long *rownumber, int *colnumber, double *value) 

   @brief Change a row in an open table

   Changes a value in an opened table (initialisation by opencolfts)
   that is private to the module. All input values are double format
   while they will be stored as the user predefined type. The
   specification of the content of a column is done by fillhd or
   fillhd_ respectively.  The user has to care that row contains the
   number of fields specified by initftstbl.

   @param value     (double *)  The new value
   @param rownumber (long *)    The number of the row to be changed    
   @param colnumber (int *)     The number of the column to be changed    
   @return (success) int putrow_; 1
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_putval_(long *rownumber, int *colnumber, double *value);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_putrow_(long *rownumber, double *row) 

   @brief Change a row in an open table

   Changes a row in binary format in an opened table (initialisation
   by opencolfts) that is private to the module. All input values are
   double format while they will be stored as the user predefined
   type. The specification of the content of a column is done by
   fillhd or fillhd_ respectively.  The user has to care that row
   contains the number of fields specified by initftstbl.

   @param row       (double *) A double array with the contents of the row
   @param rownumber (long *)   The number of the row to be changed    

   @return (success) int putrow_; 1
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_putrow_(long *rownumber, double *row);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_row(long rownr, double *row) 

   @brief Get a whole row into an array

   A row is put into an array that has
   to be of length ftstab_get_colnr_(), as can be allocated using
   ftstab_get_dblarr().

   @param rownr (long)     The number of desired row, starting with 1
   @param buffer(double *) An array large enough to contain
   ftstab_get_colnr_() numbers

   @return (success) int ftstab_get_row_: Number of columns
           (error) 0 (if the row dowsn't exist)
 */
/* ------------------------------------------------------------ */
int ftstab_get_row(long rownr, double *row);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_row_(long *rownr, double *array) 

   @brief Get a whole row into an array

   A row is put into an array that has
   to be of length ftstab_get_colnr_(), as can be allocated using
   ftstab_get_dblarr().

   @param rownr (long)     The number of desired row, starting with 1
   @param array (double *) An array large enough to contain
   ftstab_get_colnr_() numbers

   @return (success) int ftstab_get_row_: Number of columns
           (error) 0 (if the row dowsn't exist)
 */
/* ------------------------------------------------------------ */
int ftstab_get_row_(long *rownr, double *array);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_get_value(long rownr, int colnr, double *val) 

   @brief Get a value of a single field as double

   Put the value of the field specified by rownr and colnr into val.

   @param rownr (long)     The number of desired row, starting with 1
   @param colnr (long)     The number of desired column, starting with 1
   @param array (double *) An array large enough to contain
   ftstab_get_colnr_() numbers

   @return (success) int ftstab_get_value_: Number of columns
           (error) 0 (if the row  or column doesn't exist)
 */
/* ------------------------------------------------------------ */
int ftstab_get_value(long rownr, int colnr, double *val);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_get_value_(long *rownr, int *colnr, double *val) 

   @brief Get a value of a single field as double

   Put the value of the field specified by rownr and colnr into val.

   @param rownr (long *)     The number of desired row, starting with 1
   @param colnr (long *)     The number of desired column, starting with 1
   @param array (double *) An array large enough to contain
   ftstab_get_colnr_() numbers

   @return (success) int ftstab_get_value_: Number of columns
           (error) 0 (if the row  or column doesn't exist)
 */
/* ------------------------------------------------------------ */
int ftstab_get_value_(long *rownr, int *colnr, double *val);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_heapsort(int column, long start, long end) 

   @brief Sorts ascending the rows in-between begin and end depending
   on the value of column

   This function works directly on the stream and may be slow. On the
   other hand, memory requirements are not given and handed to the
   operating system. The row and column number starting with 1.

   @param column (int)  The column to sort
   @param start  (long) The start row
   @param end    (long) The end row

   @return (success) int ftstab_heapsort: 1
           (error) 0 in case of wrong input
 */
/* ------------------------------------------------------------ */
int ftstab_heapsort(int column, long start, long end);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_heapsort_(int *column, long *start, long *end) 

   @brief Sorts ascending the rows in-between begin and end depending
   on the value of column

   This function works directly on the stream and may be slow. On the
   other hand, memory requirements are not given and handed to the
   operating system. The row and column number starting with 1.

   @param column (int *)  The column to sort
   @param start  (long *) The start row
   @param end    (long *) The end row

   @return (success) int ftstab_heapsort: 1
           (error) 0 in case of wrong input
 */
/* ------------------------------------------------------------ */
int ftstab_heapsort_(int *column, long *start, long *end);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_histogram(char *filename, int column, long startrow,
   long endrow, double min, double max, long bins, double delta, int
   repet)

   @brief Makes a fits image containing a histogram of a column

   The function will try to open the file filename and checks if it is
   a fits file. If it is not, the file will be deleted and an empty
   file with the name will be opened. If it is, the function checks
   whether the last header in the file is a history header (An empty
   image with no axes), saves it locally and deletes it if it
   is. Then, an fits image is attached to the file that contains a
   histogram in the following way:

   Along one axis the image contains the histogram of the column with
   the number column, this axis will be repeated along the other axis
   repet times (e.g. for access with the Karma software chose 2), such
   that more than repet = 1 is in principle not necessary. The
   histogram will be carried out between startrow and endrow and takes
   into account only values in-between min and max. If startrow >
   endrow, the whole table will be taken into account. If min > max,
   then the minimum and the maximum of the specified rows are
   calculated and taken as min and max. If bins is specified, then the
   length of a bin delta is calculated from bins and min and max,
   min-max being 0 requires a delta to be set. If bins = 0 then the
   number of the bins is calculated from the bin size delta. If bins =
   0 and delta = 0 bins is set to 100.

   With a call of this function the table will be rearranged if
   successful and the min and max values will be changed in any
   case. The user has to be aware of the factand take either
   precautions or has to be able to rearrange the table properly with
   the access functions in this module.

      CAUTION: In this function the header will be created in a way that
   the scaling and the zero value of the item are taken into
   account. If you don't want this, set it to 1.0, or 0.0 respectively
   with ftstab_hdladditem().

   @param filename (char *) The output fits file
   @param column   (int)    The column number of the column to make the histogram of (start with 1)
   @param startrow (long)   The start row number (start with 1)
   @param endrow   (long)   The end row number
   @param min      (double) The minimum value to take into account
   @param max      (double) The maximum value to take into account
   @param bins     (long)   The number of bins
   @param delta    (double) The bin size
   @param repeti   (int)    The number of repetitions of the histogram along the second axis.

   @return (success) int ftstab_histogram: 1
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_histogram(char *filename, int column, long startrow, long endrow, double min, double max, long bins, double delta, int repet);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_histogram_2d(char *filename, long startrow1, long endrow1, int column1, double min1, double max1, long bins1, double delta1, int column2, double min2, double max2, long bins2, double delta2)

   @brief Makes a fits image containing a 2d histogram of a column

   The function will try to open the file filename and checks if it is
   a fits file. If it is not, the file will be deleted and an empty
   file with the name will be opened. If it is, the function checks
   whether the last header in the file is a history header (An empty
   image with no axes), saves it locally and deletes it if it
   is. Then, an fits image is attached to the file that contains a
   histogram in the following way:

   Along axis1 and axis2 the image contains the histogram of column1
   and column2. The histogram will be carried out between startrow and
   endrow and takes into account only values in-between min1/2 and
   max1/2. If startrow > endrow, the whole table will be taken into
   account. If min1/2 > max1/2, then the minimum and the maximum of
   the specified rows are calculated and taken as min and max. If bins
   is specified, then the length of a bin delta is calculated from
   bins and min and max, min1/2-max1/2 being 0 requires a delta1/2 to
   be set.. If bins1/2 = 0 then the number of the bins is calculated
   from the bin size delta1/2. If bins1/2 = 0 and delta1/2 = 0 bins is
   set to 100.

   With a call of this function the table will be rearranged if
   successful and the min and max values will be changed in any
   case. The user has to be aware of the fact and take either
   precautions or has to be able to rearrange the table properly with
   the access functions in this module.

      CAUTION: In this function the header will be created in a way that
   the scaling and the zero value of the item are taken into
   account. If you don't want this, set it to 1.0, or 0.0 respectively
   with ftstab_hdladditem().

   @param filename (char *) The output fits file
   @param startrow (long)   The start row number (start with 1)
   @param endrow   (long)   The end row number
   @param column1   (int)   The column number of the first column to make the histogram of (start with 1)
   @param min1      (double) The minimum value to take into account for first column
   @param max1      (double) The maximum value to take into account for first column
   @param bins1     (long)   The number of bins for first column
   @param delta1    (double) The bin size for first column
   @param column2   (int)   The column number of the second column to make the histogram of (start with 1)
   @param min2      (double) The minimum value to take into account for second column
   @param max2      (double) The maximum value to take into account for second column
   @param bins2     (long)   The number of bins for second column
   @param delta2    (double) The bin size for second column

   @return (success) int ftstab_histogram: 1
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_histogram_2d(char *filename, long startrow1, long endrow1, int column1, double min1, double max1, long bins1, double delta1, int column2, double min2, double max2, long bins2, double delta2);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_histogram_(char *filename, int *column, long *startrow,
   long *endrow, double *min, double *max, long *bins, double *delta, int
   *repet)

   @brief Makes a fits image containing a histogram of a column

   The function will try to open the file filename and checks if it is
   a fits file. If it is not, the file will be deleted and an empty
   file with the name will be opened. If it is, the function checks
   whether the last header in the file is a history header (An empty
   image with no axes), saves it locally and deletes it if it
   is. Then, an fits image is attached to the file that contains a
   histogram in the following way:

   Along one axis the image contains the histogram of the column with
   the number column, this axis will be repeated along the other axis
   repet times (e.g. for access with the Karma software chose 2), such
   that more than repet = 1 is in principle not necessary. The
   histogram will be carried out between startrow and endrow and takes
   into account only values in-between min and max. If startrow >
   endrow, the whole table will be taken into account. If min > max,
   then the minimum and the maximum of the specified rows are
   calculated and taken as min and max. If bins is specified, then the
   length of a bin delta is calculated from bins and min and max,
   min-max being 0 requires a delta to be set. If bins = 0 then the
   number of the bins is calculated from the bin size delta. If bins =
   0 and delta = 0 bins is set to 100.

   With a call of this function the table will be rearranged if
   successful and the min and max values will be changed in any
   case. The user has to be aware of the fact and take either
   precautions or has to be able to rearrange the table properly with
   the access functions in this module.

   CAUTION: In this function the header will be created in a way that
   the scaling and the zero value of the item are taken into
   account. If you don't want this, set it to 1.0, or 0.0 respectively
   with ftstab_hdladditem().

   @param filename (char *) The output fits file
   @param column   (int *)    The column number of the column to make the histogram of (start with 1)
   @param startrow (long *)   The start row number (start with 1)
   @param endrow   (long *)   The end row number
   @param min      (double *) The minimum value to take into account
   @param max      (double *) The maximum value to take into account
   @param bins     (long *)   The number of bins
   @param delta    (double *) The bin size
   @param repeti   (int *)    The number of repetitions of the histogram along the second axis.

   @return (success) int ftstab_histogram: 1
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_histogram_(char *filename, int *column, long *startrow, long *endrow, double *min, double *max, long *bins, double *delta, int *repet);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_histogram_2d_(char *filename, long *startrow1, long *endrow1, int *column1, double *min1, double *max1, long *bins1, double *delta1, int *column2, double *min2, double *max2, long *bins2, double *delta2)

   @brief Makes a fits image containing a 2d histogram of a column

   The function will try to open the file filename and checks if it is
   a fits file. If it is not, the file will be deleted and an empty
   file with the name will be opened. If it is, the function checks
   whether the last header in the file is a history header (An empty
   image with no axes), saves it locally and deletes it if it
   is. Then, an fits image is attached to the file that contains a
   histogram in the following way:

   Along axis1 and axis2 the image contains the histogram of column1
   and column2. The histogram will be carried out between startrow and
   endrow and takes into account only values in-between min1/2 and
   max1/2. If startrow > endrow, the whole table will be taken into
   account. If min1/2 > max1/2, then the minimum and the maximum of
   the specified rows are calculated and taken as min and max. If bins
   is specified, then the length of a bin delta is calculated from
   bins and min and max, min1/2-max1/2 being 0 requires a delta1/2 to
   be set.. If bins1/2 = 0 then the number of the bins is calculated
   from the bin size delta1/2. If bins1/2 = 0 and delta1/2 = 0 bins is
   set to 100.

   With a call of this function the table will be rearranged if
   successful and the min and max values will be changed in any
   case. The user has to be aware of the fact and take either
   precautions or has to be able to rearrange the table properly with
   the access functions in this module.

      CAUTION: In this function the header will be created in a way that
   the scaling and the zero value of the item are taken into
   account. If you don't want this, set it to 1.0, or 0.0 respectively
   with ftstab_hdladditem().

   @param filename (char *) The output fits file
   @param startrow (long *)   The start row number (start with 1)
   @param endrow   (long *)   The end row number
   @param column1   (int *)   The column number of the first column to make the histogram of (start with 1)
   @param min1      (double *) The minimum value to take into account for first column
   @param max1      (double *) The maximum value to take into account for first column
   @param bins1     (long *)   The number of bins for first column
   @param delta1    (double *) The bin size for first column
   @param column2   (int *)   The column number of the second column to make the histogram of (start with 1)
   @param min2      (double *) The minimum value to take into account for second column
   @param max2      (double *) The maximum value to take into account for second column
   @param bins2     (long *)   The number of bins for second column
   @param delta2    (double *) The bin size for second column

   @return (success) int ftstab_histogram_: 1
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_histogram_2d_(char *filename, long *startrow1, long *endrow1, int *column1, double *min1, double *max1, long *bins1, double *delta1, int *column2, double *min2, double *max2, long *bins2, double *delta2);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn long ftstab_get_rownr_(void) 

   @brief Get the number of rows

   @return long ftstab_get_rownr_: Number of rows
 */
/* ------------------------------------------------------------ */
long ftstab_get_rownr_(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_colnr_(void) 

   @brief Get the number of columns

   @return int ftstab_getcolnr_: Number of columns
 */
/* ------------------------------------------------------------ */
int ftstab_get_colnr_(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_get_extnr_(void) 

   @brief Get the number of the current extension

   Returns the extension number of the extension in procession.

   @return (success) int ftstab_get_extnr_: Extension number\n
           (error) 0 
 */
/* ------------------------------------------------------------ */
int ftstab_get_extnr_(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_close_(void) 

   @brief Puts the open stream containing a table in a final condition

   This function appends a number of zeros to the private stream
   making it fits conforming.
   
   @return (success) int  ftstab_close_: 1
           (error) 0
 */
/* ------------------------------------------------------------ */
int ftstab_close_(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn void ftstab_flush_(void) 

   @brief Closes the table stream and destroys all information

   Closes the table stream and destroys all information. Puts the
   module in a status like before the first call.
   
   @return void
 */
/* ------------------------------------------------------------ */
void ftstab_flush_(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_deleterest_(void) 

   @brief Delete all subsequent extensions

   All subsequent extensions are deleted from the file and the
   permissions to attach items (table or header) are adjusted. An
   hheader will still be attached, if present.

   @return (success) int ftstab_copytohere: 1
           (error) 0 in case of wrong input
 */
/* ------------------------------------------------------------ */
int ftstab_deleterest_(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn  int ftstab_copytohere_(char *newfilename) 

   @brief Make a copy of the file in process to the current extension

   Will output a file with the name newfilename that is identical with the file that would be put out if ftstab_close() is called, except that all extensions after the current one are deleted. However if an hheader is present, it will be attached to the file.

   @param newfilename (char *) The filename of the file to copy to.

   @return (success) int ftstab_copytohere: 1
           (error) 0 in case of wrong input
 */
/* ------------------------------------------------------------ */
int ftstab_copytohere_(char *newfilename);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int *ftstab_get_intarr(void) 

   @brief Delivers an int array of the length of the column number

   The array has to be freed with free

   @return (success) int *ftstab_get_intarr: Number of columns\n
           (error) NULL
 */
/* ------------------------------------------------------------ */
int *ftstab_get_intarr(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int *ftstab_get_dblarr(void) 

   @brief Delivers a double array of the length of the column number

   The array has to be freed with free

   @return (success) int *ftstab_get_dblarr: Number of columns
           (error) NULL
 */
/* ------------------------------------------------------------ */
double *ftstab_get_dblarr(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn long ftstab_get_curlength_(void)

   @brief Returns the current length of the table

   @return long ftstab_get_curlength_: Current length of table
 */
/* ------------------------------------------------------------ */
long ftstab_get_curlength_(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int ftstab_get_byteperow_(void)

   @brief Returns the length of a row

   @return long ftstab_get_byteperow_: Current length of a row
 */
/* ------------------------------------------------------------ */
int ftstab_get_byteperow_(void);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

   $Log: ftstab.h,v $
   Revision 1.28  2009/05/26 07:56:39  jozsa
   Left work

   Revision 1.27  2007/08/22 15:58:34  gjozsa
   Left work

   Revision 1.26  2005/03/25 18:17:19  gjozsa
   Left work

   Revision 1.25  2005/02/21 11:23:44  gjozsa
   Added the recover functionality to checkin, now finished until some additional thing is needed

   Revision 1.23  2005/02/21 09:44:24  gjozsa
   Stable, tested, and nearly final version, added histogram functions

   Revision 1.22  2005/02/16 18:55:32  gjozsa
   Left work

   Revision 1.21  2005/02/16 13:30:26  gjozsa
   Largely debugged and tested, added histogram function

   Revision 1.20  2005/02/15 18:00:34  gjozsa
   Left work

   Revision 1.19  2005/02/15 15:37:11  gjozsa
   Left work

   Revision 1.18  2005/02/11 17:37:13  gjozsa
   Left work

   Revision 1.17  2005/02/10 17:57:42  gjozsa
   Left work

   Revision 1.16  2005/02/10 12:29:09  gjozsa
   Implemented and tested heapsort

   Revision 1.15  2005/02/09 17:52:19  gjozsa
   Left work

   Revision 1.14  2005/02/08 11:37:14  gjozsa
   Added and tested putval and putrow functions

   Revision 1.13  2005/02/07 16:05:49  gjozsa
   Quite tested version, added lots of functionality

   Revision 1.12  2005/02/04 18:01:12  gjozsa
   Left work

   Revision 1.11  2005/02/03 17:23:55  gjozsa
   Left work

   Revision 1.9  2005/02/01 17:45:53  gjozsa
   Left work

   Revision 1.8  2005/02/01 14:51:20  gjozsa
   Left work

   Revision 1.7  2005/01/28 17:29:23  gjozsa
   Tested and debugged

   Revision 1.6  2005/01/27 17:44:43  gjozsa
   Left work

   Revision 1.5  2005/01/26 17:26:14  gjozsa
   Left work

   Revision 1.4  2005/01/26 17:10:04  gjozsa
   First stable and tested version

   Revision 1.3  2005/01/24 15:48:45  gjozsa
   Left work

   Revision 1.1  2005/01/17 12:16:04  gjozsa
   added to cvs control


   ------------------------------------------------------------ */

/* Include guard */
#endif

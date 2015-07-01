/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file pgp.h
   @brief Interface to pgplot

   Some usage of pgplot. Functionality goes like this: With filling
   the struct pgp_gdsc the user defines a layout of a plot, which
   consists of a number of viewgraphs with the same x-axis range. A
   default struct of that type is delivered by the routine
   pgp_default. A pgplot device is activated (and must be activated)
   by calling the routine pgp_opendev(). Then, a box is drawn by
   calling the routine pgp_openbox. With a call of that function an
   appropriate pgplot viewport is defined to enable the user to plot
   his viewgraph in the box by calling functions pgp_marker(),
   pgp_lines(), pgp_bars(), and pgp_errby(). With pgp_legend(), a
   string can be plotted to a legend line. The routine pgp_end() will
   terminate the plotting process properly.

   An example can be accessed with testpgp.c.

   $Source: /Volumes/DATA_J_II/data/CVS/tirific/include/pgp.h,v $
   $Date: 2009/05/26 07:56:40 $
   $Revision: 1.7 $
   $Author: jozsa $
   $Log: pgp.h,v $
   Revision 1.7  2009/05/26 07:56:40  jozsa
   Left work

   Revision 1.6  2007/08/22 15:58:36  gjozsa
   Left work

   Revision 1.5  2006/11/03 10:49:03  gjozsa
   Introduced logarithmic scaling, hms dms

   Revision 1.4  2006/02/08 16:13:17  gjozsa
   Extended graphics routines

   Revision 1.3  2005/10/12 09:53:09  gjozsa
   Included polar plot facility

   Revision 1.2  2005/05/24 10:42:23  gjozsa
   Left work

   Revision 1.1  2005/05/17 12:58:16  gjozsa
   Added to cvs control


*/
/* ------------------------------------------------------------ */



/* Include guard */
#ifndef PGP_H
#define PGP_H


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <maths.h>

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @def INTERPTYPE_XXX
   @brief Integer identifyer for interpolation types

   PGP_I_LINEAR: linear interpolation
   PGP_I_CPLINE: natural cubic spline
   PGP_I_AKIMA: natural Akima interpolation

   See gsl documentation.

*/
/* ------------------------------------------------------------ */
#define PGP_I_LINEAR MATHS_I_LINEAR
#define PGP_I_CSPLINE MATHS_I_CSPLINE
#define PGP_I_AKIMA MATHS_I_AKIMA


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* TYPEDEFS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* STRUCTS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/** 
   @struct pagestyler
   @brief Page style struct
   
   A page is constructed of n plots from top to bottom, each plotted
   in a box. Above the top plot and below the bottom plot an axis
   description is plotted and the axes are numbered according to
   xmintop, xmaxtop, xminbottom, xmaxbottom, rlmargin. rlmargin gives
   the margin between a given minimum and the start of the axis in
   relative units (1 being the whole page).
*/
/* ------------------------------------------------------------ */

typedef struct pgp_gdsc
{
  /** @brief The number of plots */
  int nplots;

  /** @brief Scale of axis numbering in pgplot units (1/40 of window) */
  float numberheight;

  /** @brief Scale of symbols in units of numberheight */
  float symbolheight;

  /** @brief Scale of legend characters units of numberheight */
  float legendheight;

  /** @brief Scale of axis description in units of numberheight */
  float axdescheight;

  /** @brief Box and axis marker linewidth in pgplot units */
  int boxlw;

  /** @brief graph line linewidth in pgplot units */
  int graphlw;

  /** @brief Number of lines in the legend */
  int legendcols;

  /** @brief Number of rows in the legend */
  int legendrows;

  /** @brief Vertical border to the last point in units of symbolheight */
  float verbord;

  /** @brief Horizontal border to the last point in units of symbolheight */
  float horbord;

  /** @brief Right hand stop of the axis numbering in units of numberheight */
  float rightnum;

  /** @brief Left hand stop of the axis numbering in units of numberheight */
  float leftnum;

  /** @brief Top graph stop of the axis numbering in units of numberheight */
  float topnum;

  /** @brief Bottom stop of the axis numbering in units of numberheight */
  float botnum;

  /** @brief Margin between left hand axis description and border of the plot */
  float leftmargin;

  /** @brief Margin between right hand axis description and border of the plot */
  float rightmargin;

  /** @brief Margin between top axis description and border of the plot */
  float topmargin;

  /** @brief Margin between bottom axis description and border of the plot */
  float bottommargin;

  /** @brief Plot alternative description at the right hand? 1/0 */
  int altax;

  /** @brief Current viewport scaling x axis 0: normal 1: logarithmic 2: hms 3: dms */
  int logarcsx;

  /** @brief Current viewport scaling y axis 0: normal 1: logarithmic 2: hms 3: dms */
  int logarcsy;

  /** @brief Current viewport interpolation for lines, PGP_I_LINEAR: linear; PGP_I_CSPLINE: cubic natural spline; PGP_I_AKIMA: Akima */
  int interptype_lines;

  /** @brief Any line is a set of linear lines; this is the number of such lines in case of cubic natural spline and Akima interpolation */
  int interp_numlines;

} pgp_gdsc;

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION DECLARATIONS */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int pgp_opendev(char *device_name)
   @brief Opens the pgplot device as specified

   @param device_name (char *) The device name as in pgplot

   @return (success) int pgp_opendev: Device number
           (error)   0
*/
/* ------------------------------------------------------------ */
int pgp_opendev(char *device_name);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn void pgp_end();
   @brief End the plotting and close the device

   @return void
*/
/* ------------------------------------------------------------ */
void pgp_end();


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn void pgp_polar();
   @brief Make a polar diagram on an open device

   This task will draw points with the x and y coordinates given by
   xarray and yarray and lines given by xlarray and ylarray and
   circles about the origin given by radii. The first circles are
   dashed and the last one not dashed. An additional line will be
   drawn from the origin to the non-dashed circle with an angle
   (counterclockwise from y-axis in degree) given by lineangle. The extent of
   the diagram is calculated automatically.

   @param ndots       (int)     number of dots
   @param xarray      (float *) array containing the x coordinates for the plot
   @param yarray      (float *) array containing the y coordinates for the plot
   @param nlines      (int)     number of positions defining the lines
   @param xlarray     (float *) array containing the x coordinates for the lines
   @param ylarray     (float *) array containing the y coordinates for the lines
   @param nradii      (int)     number of radii for rings
   @param radii       (float *) radii for rings
   @param lineangle   (float *) angle for a reference line, if set to NULL, no line
   @param linewidth   (int)     width of the lines, from 1 to 201
   @param circlewidth (int)     width of the circles, the solid circle gets double the value, from 1 to 201
   @param symbolwidth (int)     width of the dots, from 1 to 201
   @param colour      (int)     colour of dots and lines

   @return void
*/
/* ------------------------------------------------------------ */
void pgp_polar(int ndots, float *xarray, float *yarray, int nlines, float *xlarray, float *ylarray, int nradii, float *radii, float *lineangle, int linewidth, int circlewidth, int symbolwidth, int colour);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn pgp_gdsc *pgp_gdsc_default(int nplots, int legendcols, int legendrows, float charheight)
   @brief Returns a default pgp_gdsc struct

   Allocates and fills a standard pgp_gdsc struct. The character
   height is reduced if a proper axis numbering is impossible.

   @param nplots      (int)   Number of plots
   @param legendcols  (int)   Columns of the legend
   @param legendrows  (int)   Rows of the legend
   @param charheight  (float) Character height

   @return (success) pgp_gdsc *pgp_gdsc_default: An allocated struct
           (error)   NULL
*/
/* ------------------------------------------------------------ */
pgp_gdsc *pgp_gdsc_default(int nplots, int legendcols, int legendrows, float charheight);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int pgp_openbox(pgp_gdsc *gdsc, int nplot, float xmin, float xmax, float ymin, float ymax, char *leftdeschi, char *leftdesclo, char *rightdeschi, char *rightdesclo, char *bottomdeschi, char *bottomdesclo, char *topdeschi, char *topdesclo, float lrzero, float lrscale, float btzero, float btscale, int logarcsx, int logarcsy)
   @brief Opens a box in an opened window with a few specifications

   This function will open a box with the following specifications: A
   plot goes from the left border to the right border of the device.
   The page is shared by nplots plots.  There is a space legendsp at
   the bottom of the plot that can hold a few specifications. All axis
   descriptions are in two boxes subdivided by a fraction bar. Axis
   numbering and description will only take place outside the
   viewgraph. The global outlay is steered by filling the pgp_gdsc
   struct that is passed to this function.  Logarithmic scaling on the
   basis of 10 can be achieved by setting logarcsx or logarcsy to 1,
   setting these values to 2 means that the input is interpreted as
   degrees but the lower/left panel labelling is in hms, the same
   accounts for a value of 3, but then the labelling is in dms.

   @param gdsc         (pgp_gdsc *) Global viewgraph descriptor
   @param nplot        (int)        Plot number
   @param xmin         (float)      Minimum of x-axis
   @param xmax         (float)      Maximum of x-axis
   @param ymin         (float)      Minimum of y-axis
   @param ymax         (float)      Minimum of y-axis
   @param leftdeschi   (char *)     Axis description of left axis above the fraction bar
   @param leftdesclo   (char *)     Axis description of left axis below the fraction bar
   @param rightdeschi  (char *)     Axis description of right axis above the fraction bar
   @param rightdesclo  (char *)     Axis description of right axis below the fraction bar
   @param bottomdeschi (char *)     Axis description of bottom axis above the fraction bar
   @param bottomdesclo (char *)     Axis description of bottom axis below the fraction bar
   @param topdeschi    (char *)     Axis description of top axis above the fraction bar
   @param topdesclo    (char *)     Axis description of top axis below the fraction bar
   @param lrzero       (float)      Zero point when scaling the left axis to get the scale of right axis
   @param lrscale      (float)      Scaling factor when scaling the left axis to get the scale of right axis
   @param btzero       (float)      Zero point when scaling the bottom axis to get the scale of top axis
   @param btscale      (float)      Scaling factor when scaling the bottom axis to get the scale of top axis
   @param logarcsx     (int)        if 0, normal axis scaling, 1 logarithmic, 2 hms, 3 dms, input in deg
   @param logarcsy     (int)        if 0, normal axis scaling, 1 logarithmic, 2 hms, 3 dms, input in deg
      
   @return (success) int pgp_openbox: 1
           (error)   0
*/
/* ------------------------------------------------------------ */
int pgp_openbox(pgp_gdsc *gdsc, int nplot, float xmin, float xmax, float ymin, float ymax, char *leftdeschi, char *leftdesclo, char *rightdeschi, char *rightdesclo, char *bottomdeschi, char *bottomdesclo, char *topdeschi, char *topdesclo, float lrzero, float lrscale, float btzero, float btscale, int logarcsx, int logarcsy);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int pgp_legend(pgp_gdsc *gdsc, int nhor, int nver, char *string)
   @brief Puts a string to the specified legend position

   The information on the last box is lost with a call of this function. A call of pgp_openbox is necessary to get an additional box to plot in again.

   @param gdsc   (pgp_gdsc *) Global viewgraph descriptor
   @param nhor   (int)        Horizontal boxnumber
   @param nver   (int)        Vertical box number
   @param string (char *)     String to put in

   @return (success) int pgp_legend: 1\\
           (error)   0
*/
/* ------------------------------------------------------------ */
int pgp_legend(pgp_gdsc *gdsc, int nhor, int nver, char *string);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int pgp_marker(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, int colour, int empty, int symbol, float sizer)
   @brief Puts symbols on a predefined viewport. If the scaling has been modified it makes a local copy of the data arrays xpos and ypos first. Hence the function can fail.

   @param gdsc    (pgp_gdsc *) Global viewgraph descriptor
   @param npoints (int)        Number of points
   @param xpos    (float *)    List of x-positions
   @param ypos    (float *)    List of y-positions
   @param colour  (int)        Colour index of the symbols, preferrably 0-15
   @param empty   (int)        If not set 0, the symbols are not filled
   @param symbol  (int)        The symbols, pgplot convention
   @param sizer   (float)      Size of symbols relative to symbolheight

   @return (success) int pgp_marker: 0 \\
           (error)   1 xarray allocation problems
                     2 yarray allocation problems
*/
/* ------------------------------------------------------------ */
int pgp_marker(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, int colour, int empty, int symbol, float sizer);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int pgp_lines(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, int colour)
   @brief Puts lines to a predefined viewport

   @param gdsc    (pgp_gdsc *) Global viewgraph descriptor
   @param npoints (int)        Number of points
   @param xpos    (float *)    List of x-positions
   @param ypos    (float *)    List of y-positions
   @param colour  (int)        Colour index of the lines, preferrably 0-15

   @return (success) int pgp_lines: 0 \\
           (error)   1 xarray allocation problems
                     2 yarray allocation problems
*/
/* ------------------------------------------------------------ */
int pgp_lines(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, int colour);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int pgp_bars(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, float width, int colour)
   @brief Puts horizontal bars of width width (wu) at the positions 

   @param gdsc    (pgp_gdsc *) Global viewgraph descriptor
   @param npoints (int)        Number of points
   @param xpos    (float *)    List of x-positions
   @param ypos    (float *)    List of y-positions
   @param width   (float)    Width of bar in world units
   @param colour  (int)        Colour index of the bars, preferrably 0-15

   @return (success) int pgp_bars: 0 \\
           (error)   1 xarray allocation problems
                     2 yarray allocation problems
*/
/* ------------------------------------------------------------ */
int pgp_bars(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, float width, int colour);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @fn int pgp_errby(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, float *err, int colour)
   @brief Puts vertical (error)bars of width width (wu) at the positions 

   @param gdsc    (pgp_gdsc *) Global viewgraph descriptor
   @param npoints (int)        Number of points
   @param xpos    (float *)    List of x-positions
   @param ypos    (float *)    List of y-positions
   @param err     (float *)    List of half widths of error bars, i.e. the errors
   @param colour  (int)        Colour index of the bars, preferrably 0-15

   @return (success) int pgp_errby: 0 \\
           (error)   1 xarray allocation problems
                     2 yarray allocation problems
*/
/* ------------------------------------------------------------ */
int pgp_errby(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, float *err, int colour);


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: pgp.h,v $
   Revision 1.7  2009/05/26 07:56:40  jozsa
   Left work

   Revision 1.6  2007/08/22 15:58:36  gjozsa
   Left work

   Revision 1.5  2006/11/03 10:49:03  gjozsa
   Introduced logarithmic scaling, hms dms

   Revision 1.4  2006/02/08 16:13:17  gjozsa
   Extended graphics routines

   Revision 1.3  2005/10/12 09:53:09  gjozsa
   Included polar plot facility

   Revision 1.2  2005/05/24 10:42:23  gjozsa
   Left work

   Revision 1.1  2005/05/17 12:58:16  gjozsa
   Added to cvs control


   ------------------------------------------------------------ */

/* Include guard */
#endif

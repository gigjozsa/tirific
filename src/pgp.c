/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file pgp.c
   @brief Interface to pgplot

   Some usage of pgplot.Functionality goes like this: With filling the
   struct pgp_gdsc the user defines a layout of a plot, which consists
   of a number of viewgraphs with the same x-axis range. A default
   struct of that type is delivered by the routine pgp_default. A
   pgplot device is activated (and must be activated) by calling the
   routine pgp_opendev(). Then, a box is drawn by calling the routine
   pgp_openbox. With a call of that function an appropriate pgplot
   viewport is defined to enable the user to plot his viewgraph in the
   box by calling functions pgp_marker(), pgp_lines(), pgp_bars(), and
   pgp_errby(). With pgp_legend(), a string can be plotted to a legend
   line. The routine pgp_end() will terminate the plotting process
   properly.

   An example can be accessed with testpgp.c.

   $Source: /Volumes/DATA_J_II/data/CVS/tirific/src/pgp.c,v $
   $Date: 2011/05/25 22:25:26 $
   $Revision: 1.14 $
   $Author: jozsa $
   $Log: pgp.c,v $
   Revision 1.14  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.13  2011/05/10 00:30:16  jozsa
   Left work

   Revision 1.12  2009/05/26 07:56:41  jozsa
   Left work

   Revision 1.11  2007/08/22 15:58:43  gjozsa
   Left work

   Revision 1.10  2006/11/22 14:16:21  gjozsa
   Bugfix concerning RASH and horizontal/vertical lines

   Revision 1.9  2006/11/03 10:49:02  gjozsa
   Introduced logarithmic scaling, hms dms

   Revision 1.8  2006/10/05 12:39:58  gjozsa
   nasty bugfix

   Revision 1.7  2006/04/03 11:47:57  gjozsa
   Left work

   Revision 1.6  2006/02/08 16:13:17  gjozsa
   Extended graphics routines

   Revision 1.5  2005/10/12 14:51:25  gjozsa
   First point gets thick in the polar plot

   Revision 1.4  2005/10/12 09:53:09  gjozsa
   Included polar plot facility

   Revision 1.3  2005/06/24 12:00:29  gjozsa
   Left work

   Revision 1.2  2005/05/24 10:42:24  gjozsa
   Left work

   Revision 1.1  2005/05/17 12:58:15  gjozsa
   Added to cvs control


*/
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <stdio.h>
#include <cpgplot.h>
#include <math.h>
#include <string.h>
#include <float.h>
#include <stdlib.h>
#include <gsl/gsl_interp.h>
#include <gsl/gsl_spline.h>


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <pgp.h>
#include <maths.h>

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
   @define DEGTORAD
   @brief Conversion factor from deg to rad
*/
/* ------------------------------------------------------------ */
#define DEGTORAD 0.0174532925199433

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @define LOGFAULT
   @brief If a logarithm is asked for for a negative number or 0 this should be redurned
*/
/* ------------------------------------------------------------ */
#define LOGFAULT -40


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
/**
  @fn int putalabel(char where, char *overfrac, char underfrac, float charheight, float xmin, float xmax)
  @brief Puts a label at a viewgraph as a fraction

  Requires an open viewgraph, and the character height set to the axis
  numbering height. If both overfrac and underfrac are filled, then a fraction will be plotted. If one of both is an empty string (not NULL), then no fraction bar will be plotted.

  @param where      (char)   t, b, l, r position of the label
  @param overfrac   (char *) Text above the fraction bar
  @param underfrac  (char *) Text below the fraction bar
  @param charheight (float)  Character height relative to the actual character height, which should be set to the numbering height.
  @param dist       (float)  Distance from the axis baseline in units of the initial height
  @return (success) int putalabel: 1\\
          (error)   0

*/
/* ------------------------------------------------------------ */
int putalabel(char where, char *overfrac, char *underfrac, float charheight, float dist);

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn float pgp_logf10f(float x)
  @brief Returns the logarithm on basis 10 and LOGFAULT if x <= 0

  @param x (float) number to calculate the logarithm of
  @return logarithm of the number or, if impossible, LOGFAULT

*/
/* ------------------------------------------------------------ */
float pgp_logf10f(float x);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
  @fn static int pgp_get_interparray(float *x, float *y, int npoints, int interptype, int nout, float **xout, float **yout)
  @brief Returns an array of interpolated values.

  Assumes an array of npoints x-y pairs as an input. Checks if x is strictly monotonically increasing. If *xout is not NULL, deallocates *xout. If *yout is not NULL, deallocates *yout. Allocates an array *xout of nout equally-spaced points with xout[0] = x[0] and xout[nout-1] = x[nout-1]. Allocates an array *yout of nout data points where the ith data point is an interpolation between x and y at the position xout[i]. interptype determines the interpolation type, which can be  PGP_I_LINEAR: linear; PGP_I_CSPLINE: cubic natural spline; PGP_I_AKIMA: Akima .

  @param x          (float * ) input data points, abscissae, must be strictly increasing
  @param y 	    (float * ) input data points, ordinate
  @param npoints    (  int   ) input number of data points
  @param interptype (  int   ) input interpolation type: PGP_I_LINEAR: linear; PGP_I_CSPLINE: cubic natural spline; PGP_I_AKIMA: Akima
  @param nout	    (  int   ) input number of output data points
  @param xout 	    (float **) output data array x-values (equally spaced)
  @param yout 	    (float **) output data array y-values

  @return logarithm of the number or, if impossible, LOGFAULT

*/
/* ------------------------------------------------------------ */
 static int pgp_get_interparray(float *x, float *y, int npoints, int interptype, int nout, float **xout, float **yout);



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* FUNCTION CODE */
/* ------------------------------------------------------------ */

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Opens the pgplot device as specified */
int pgp_opendev(char *device_name)
{
  int val;

  if (device_name)
    if (device_name[0]) {
      val = cpgbeg(0,device_name,1,1);

      /* Has to be set 0 for use in gipsy */
      cpgask(1);
      return val;
    }

  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns a default pgp_gdsc struct */
pgp_gdsc *pgp_gdsc_default(int nplots, int legendcols, int legendrows, float charheight)
{
  pgp_gdsc *gdsc;
  /* int i;  Control variable */
  float chrhtx; /* Horizontal character width in x-map units, 1 is whole page */
  float chrhty; /* Vertical character height in y-map units, 1 is whole page */
  float chrhtxref; /* Reference charheight */
  float scalexy; /* scale from x size of numbers to y size of numbers
		    (is the same physical size, but in terms of window
		    size they are different) */
  float pltheight;

  /* Allocate */
  if (!(gdsc = (pgp_gdsc *) malloc(sizeof(pgp_gdsc))))
    return NULL;

  /* Find the reference */
  cpgsch(1.0);
  cpgqcs(0, &chrhtxref, &chrhty);

  /* This is a scale */
  scalexy = chrhty/chrhtxref;

  /* Now, the character height in window units is like this */
  chrhtx = chrhtxref*charheight;

  /* Fill with default values */

  /* Number of plots */
  gdsc -> nplots = nplots;

  /* Symbols and descriptors in charheight, but at most one
     third??? of the plot height until the numbering is finished */

  /* The height of the plot is 1.0 at the start */
  pltheight = 1.0;
  
  /* The number is reduced by the axis description + 1 character */
  pltheight = pltheight-12.3*chrhty;

  /* Then, for each legend line, two characters */
  pltheight = pltheight-2.0*legendrows*chrhty;

  /* Then, each plot has only 1/nplots height */
  pltheight = pltheight/nplots;

  /* At the end, each plot has a margin of 4 heights at the top for the numbering */
  pltheight = pltheight-4.0*chrhty;

  /* We could express this as: */
  /* (1.0-12.3*chrhty-2.0*legendrows*chrhty)/nplots-4.0*chrhty */


  /* If this leaves room for 3 (???) numbers, then we apply the character height else we adjust the height */
  if (chrhty*3 > pltheight)
    chrhty = 1/(12.3+2*legendrows+7.0*nplots);

  chrhtx= chrhty/scalexy;

  gdsc -> numberheight = chrhtx/chrhtxref;
  gdsc -> symbolheight = 1.0;
  gdsc -> legendheight = 1.0;
  gdsc -> axdescheight = 1.0;
  
  /* legend */
  gdsc -> legendcols = legendcols;
  gdsc -> legendrows = legendrows;

  /* Horizontal and vertical borders are 2 characters */
  gdsc -> verbord = 2.0;
  gdsc -> horbord = 2.0;

  /* Stop of the axis numbering is 2 characters, only for the top of the graph */
  gdsc -> leftnum = 0.0;
  gdsc -> rightnum = 0.0;
  gdsc -> botnum = 0.0;
  gdsc -> topnum = 1.0;

  /* Margins */
  gdsc -> leftmargin = 7.0;
  gdsc -> rightmargin = 7.0;
  gdsc -> bottommargin = 1.8;
  gdsc -> topmargin = 2.5;

  /* Linewidths */
  gdsc -> graphlw = 1;
  gdsc -> boxlw = 1;

  /* reserve and return an initialised array for the axis labelling */
/*   if (!(gdsc -> logarcsx = (int *) malloc(gdsc -> nplots*sizeof(int)))){ */
/*     free(gdsc); */
/*     return 0; */
/*   } */
/*   for (i = 0; i < gdsc -> nplots; ++i) */
/*     gdsc -> logarcsx[i] = 0; */

  /* reserve and return an initialised array for the axis labelling */
/*   if (!(gdsc -> logarcsy = (int *) malloc(gdsc -> nplots*sizeof(int)))){ */
/*     free(gdsc -> logarcsx); */
/*     free(gdsc); */
/*     return 0; */
/*   } */
/*   for (i = 0; i < gdsc -> nplots; ++i) */
/*     gdsc -> logarcsy[i] = 0; */
  gdsc -> logarcsx = 0;
  gdsc -> logarcsy = 0;
  gdsc -> interptype_lines = PGP_I_LINEAR;
  gdsc -> interp_numlines = 500;

  return gdsc;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Make a polar diagram on an open device */
void pgp_polar(int ndots, float *xarray, float *yarray, int nlines, float *xlarray, float *ylarray, int nradii, float *radiis, float *lineangle, int linewidth, int circlewidth, int symbolwidth, int colour)
{
  int i;
  float rmax = 0;
  float twopointsx[2];
  float twopointsy[2];
  float xv1,yv1;
  float *radii;
  rmax = 0;

  /* Allocte memory for radii */
  if (!(radii = (float *) malloc(nradii*sizeof(float))))
    return;

  /* First find out about the extent of the graph */
  for (i = 0; i < ndots; ++i) {
    if (rmax < fabs(xarray[i]))
      rmax = fabs(xarray[i]);
    if (rmax < fabs(yarray[i]))
      rmax = fabs(yarray[i]);
  }

  for (i = 0; i < nlines; ++i) {
    if (rmax < fabs(xlarray[i]))
      rmax = fabs(xlarray[i]);
    if (rmax < fabs(ylarray[i]))
      rmax = fabs(ylarray[i]);
  }

  /* We make a local copy of the radii */

  for (i = 0; i < nradii; ++i) {
    radii[i] = fabs(radiis[i]);
    if (rmax < radii[i])
      rmax = radii[i];
  }

  /* Now scale rmax a bit */
  rmax = rmax*1.02;

  /* Define a drawing area, must be quadratic */
  cpgenv(-rmax, rmax, -rmax, rmax, 1, -2);

  /* Check the linewidths */
  if (linewidth < 1)
    linewidth = 1;
  if (linewidth > 201)
    linewidth = 201;
  if (circlewidth < 1)
    circlewidth = 1;
  if (circlewidth > 201)
    circlewidth = 201;
  if (symbolwidth < 1)
    symbolwidth = 1;
  if (symbolwidth > 201)
    symbolwidth = 201;

  /* Set the colour index foreground */
  cpgsci(1);

  /* Draw the circles */
  if (nradii > 0) {
    cpgslw(circlewidth);

    /* Make the circles open */
    cpgsfs(2);

    /* Chose a dashed style */
    cpgsls(4);
    
    for (i = 0; i < (nradii-1); ++i)
      cpgcirc(0,0, radii[i]);

    /* Draw the last circle, solid line with double width, solid */
    cpgsls(1);

    if (circlewidth > 100)
      circlewidth = 201;
    else
      circlewidth = 2*circlewidth;

    cpgslw(circlewidth);

    cpgcirc(0,0, radii[nradii-1]);

    /* Now do the Briggs style thingies */
    twopointsx[0] = 0;
    twopointsx[1] = 0;
    twopointsy[0] = radii[nradii-1];
    twopointsy[1] = radii[nradii-1]-0.05*radii[nradii-1];
    cpgline(2, twopointsx, twopointsy);

    twopointsx[0] = 0;
    twopointsx[1] = 0;
    twopointsy[0] = -radii[nradii-1];
    twopointsy[1] = -radii[nradii-1]+0.05*radii[nradii-1];
    cpgline(2, twopointsx, twopointsy);

    twopointsy[0] = 0;
    twopointsy[1] = 0;
    twopointsx[0] = radii[nradii-1];
    twopointsx[1] = radii[nradii-1]-0.05*radii[nradii-1];
    cpgline(2, twopointsx, twopointsy);

    twopointsy[0] = 0;
    twopointsy[1] = 0;
    twopointsx[0] = -radii[nradii-1];
    twopointsx[1] = -radii[nradii-1]+0.05*radii[nradii-1];
    cpgline(2, twopointsx, twopointsy);

    /* Check whether to plot a point */
    for (i = 0; i < (nradii); ++i) {
      if (radii[i] == 0){
	xv1 = yv1 = 0;
	cpgpt(1, &xv1, &yv1, -1);
      }
    }
    }
    /* Draw the reference thing */
    if ((lineangle)) {
      cpgslw(linewidth);
      twopointsy[0] = 0;
      twopointsy[1] = cos(DEGTORAD*(*lineangle))*radii[nradii-1];
      twopointsx[0] = 0;
      twopointsx[1] = -sin(DEGTORAD*(*lineangle))*radii[nradii-1];
    cpgline(2, twopointsx, twopointsy);
    }

  /* Plot the points */

  /* Set the linewidth */
  cpgslw(symbolwidth);

  /* Set the colour */
  cpgsci(colour);

 /* Plot points, number of points, vectors for x and y, symbol */
  if (ndots > 0)
  cpgpt(ndots, xarray, yarray, -1);

  /* Make a first dot */
  symbolwidth *= 2;
  if (symbolwidth > 201)
    symbolwidth = 210;
  cpgslw(symbolwidth);

  cpgpt(1, xarray, yarray, -1);


  /* Draw the lines */
  /* Set the linewidth */
  if (nlines > 1) {
  cpgslw(linewidth);
  cpgline(nlines, xlarray, ylarray);
  }

  /* Finished */
  return;
}


/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Opens a box in an opened window with a few specifications */
int pgp_openbox(pgp_gdsc *gdsc, int nplot, float xmin, float xmax, float ymin, float ymax, char *leftdeschi, char *leftdesclo, char *rightdeschi, char *rightdesclo, char *bottomdeschi, char *bottomdesclo, char *topdeschi, char *topdesclo, float lrzero, float lrscale, float btzero, float btscale, int logarcsx, int logarcsy)
{
  float plotheight;
  float chrhtxref;    /* Horizontal character width in x-map units, 1 is whole page */
  float chrhtyref;    /* Vertical character height in y-map units, 1 is whole page */
  /* float scalexy; */    /* scale from x size of numbers to y size of numbers (or inverse scale of x to y size of area */
  float dummy1, dummy2;

  int slw; /* Standard linewidth */

  float xlo; /* Box in window coordinates */
  float xhi;
  float ylo;
  float yhi;

  float xulo; /* Box in user coordinates */
  float xuhi;
  float yulo;
  float yuhi;

  float xlo2; /* Another box in window coordinates */
  float xhi2;
  float ylo2;
  float yhi2;

  float xulo2; /* Another box in user coordinates */
  float xuhi2;
  float yulo2;
  float yuhi2;

  float ax, ay, bx, by; /* linear coefficients from window to user coordinates */

  void (* boxingx)(const char *xopt, float xtick, int nxsub, const char *yopt, float ytick, int nysub);
  void (* boxingy)(const char *xopt, float xtick, int nxsub, const char *yopt, float ytick, int nysub);
  char steringxt[8],steringxb[8],steringyl[8],steringyr[8],steribgxt[8],steribgxb[8],steribgyl[8],steribgyr[8];

  /* First calculate the symbol height and width in map units 
		    (is the same physical size, but in terms of window
		    size they are different) */

  /* Check if there is anything to draw */
  if (nplot > gdsc -> nplots || nplot < 1)
    return 0;

  /* Check the scaling */
  if ((logarcsx < 0) || (logarcsx > 3))
    return 0;

  /* per default the function to draw the box is cpgbox */
  boxingx = cpgbox;
  boxingy = cpgbox;

  /* Default numbering style */
  sprintf(steringyr,"MV");
  sprintf(steringyl,"NV");
  sprintf(steringxt,"M");
  sprintf(steringxb,"N");

  sprintf(steribgyr,"CTS");
  sprintf(steribgyl,"BTS");
  sprintf(steribgxt,"CTS");
  sprintf(steribgxb,"BTS");

  /* Default scaling */
  gdsc -> logarcsx = 0;
  gdsc -> logarcsy = 0;

  /* If a different scaling was requested, redefine labelling */
  if (logarcsx == 1) {
    gdsc -> logarcsx = 1;
/*     if (xmin <= 0) */
/*       return 0; */
/*     if (xmax <= 0) */
/*       return 0; */
/*     if ((xmin*btscale+btzero) <= 0) */
/*       return 0; */
/*     if ((xmax*btscale+btzero) <= 0) */
/*       return 0; */

    /* Do a trick */
    dummy1 = pgp_logf10f(xmin*btscale+btzero);
    dummy2 = pgp_logf10f(xmax*btscale+btzero);

    xmin = pgp_logf10f(xmin);
    xmax = pgp_logf10f(xmax);

    if (xmin == xmax) {
      btzero = dummy1-xmin;
      btscale = 0;
    }
    else {
      btscale = (dummy2-dummy1)/(xmax-xmin);
      btzero = dummy2-btscale*xmax;
    }
    sprintf(steringxt,"ML2");
    sprintf(steringxb,"NL2");
    sprintf(steribgxt,"CTSL");
    sprintf(steribgxb,"BTSL");
  }

  if (logarcsy == 1) {
    gdsc -> logarcsy = 1;
/*     if (ymin <= 0) */
/*       return 0; */
/*     if (ymax <= 0) */
/*       return 0; */
/*     if ((ymin*btscale+btzero) <= 0) */
/*       return 0; */
/*     if ((ymax*btscale+btzero) <= 0) */
/*       return 0; */

    /* Do a trick */
    dummy1 = pgp_logf10f(ymin*lrscale+lrzero);
    dummy2 = pgp_logf10f(ymax*lrscale+lrzero);

    ymin = pgp_logf10f(ymin);
    ymax = pgp_logf10f(ymax);

    if (ymin == ymax) {
      lrzero = dummy1-ymin;
      lrscale = 0;
    }
    else {
      lrscale = (dummy2-dummy1)/(ymax-ymin);
      lrzero = dummy2-lrscale*ymax;
    }
    sprintf(steringyr,"MVL2");
    sprintf(steringyl,"NVL2");
    sprintf(steribgyr,"CTSL");
    sprintf(steribgyl,"BTSL");
  }

  /* hms */
  if (logarcsx == 2) {
    gdsc -> logarcsx = 2;
    /* convert to seconds of time, input is interpreted as deg */
    btzero = btzero/240.0;
    btscale = btscale/240.0;
    xmin = xmin*240.0;
    xmax = xmax*240.0;
    boxingx = cpgtbox;
    sprintf(steringxt,"M");
    sprintf(steringxb,"NZH");
    sprintf(steribgxt,"CTS");
    sprintf(steribgxb,"BTSZXYH");
  }
  if (logarcsy == 2) {
    gdsc -> logarcsy = 2;

    /* convert to seconds of time, input is interpreted as deg */
    ymin = ymin*240.0;
    ymax = ymax*240.0;

    /* I'm really not sure why this is not the case */
/*      lrzero = lrzero/240.0; */
     lrscale = lrscale/240.0;
    boxingy = cpgtbox;
    sprintf(steringyr,"MV");
    sprintf(steringyl,"NVZXYH");
    sprintf(steribgyr,"CTS");
    sprintf(steribgyl,"BTSZXYH");
  }

  /* dms */
  if (logarcsx == 3) {
    gdsc -> logarcsx = 3;

    /* convert to seconds of time, input is interpreted as deg */
    xmin = xmin*3600.0;
    xmax = xmax*3600.0;

    /* I'm really not sure why this is not the case */
/*     btzero = btzero/3600.0; */
    btscale = btscale/3600.0;
    boxingx = cpgtbox;
    sprintf(steringxt,"M");
    sprintf(steringxb,"NZD");
    sprintf(steribgxt,"CTS");
    sprintf(steribgxb,"BTSZYD");
  }
  if (logarcsy == 3) {
    gdsc -> logarcsy = 3;

    /* convert to seconds of time, input is interpreted as deg */
    ymin = ymin*3600.0;
    ymax = ymax*3600.0;
/*     lrzero = lrzero/3600.0; */
    lrscale = lrscale/3600.0;
    boxingy = cpgtbox;
    sprintf(steringyr,"MV");
    sprintf(steringyl,"NVZYD");
    sprintf(steribgyr,"CTS");
    sprintf(steribgyl,"BTSZYD");
  }

  /* Ensure that minimum and maximum are different */
  if(xmin == xmax) {
    if (xmin != 0.0) {
      xmin = xmin*0.9;
      xmax = xmax*1.1;
    }
    else {
      xmin = -1.0;
      xmax = 1.0;
    }
  }

  if (logarcsx > 1) {
    if ((xmin-xmax) < 1.0) {
      xmin = xmin-1.0;
      xmax = xmax+1.0;
    }
  }

  if(ymin == ymax) {
    if (ymin != 0.0) {
      ymin = ymin*0.9;
      ymax = ymax*1.1;
    }
    else {
      ymin = -1.0;
      ymax = 1.0;
    }
  }

  if (logarcsy > 1) {
    if ((ymin-ymax) < 1.0) {
      ymin = ymin-1.0;
      ymax = ymax+1.0;
    }
  }

  /**********/
  /* scales */
  /**********/

  /* Standard linewidth */
  cpgqlw(&slw);

  /* Find the reference */
  cpgsch(gdsc -> numberheight);
  cpgqcs(0, &chrhtxref, &chrhtyref);

  /* The physical size in x is scalexy times the physical size in y */
  /* scalexy = chrhtyref/chrhtxref; */

  /* plot height */
  plotheight = (1.0 - 2.0*(gdsc -> legendrows+1)*gdsc -> legendheight*chrhtyref - 6*gdsc -> axdescheight*chrhtyref-6*chrhtyref)/gdsc -> nplots;

  /* Position of the plot, whole box */
  xlo = gdsc -> leftmargin*chrhtxref+3*gdsc -> axdescheight*chrhtxref;

  /* changed from 3*gdsc -> axdescheight to 4*gdsc -> axdescheight */
  xhi = 1.0-4*gdsc -> axdescheight*chrhtxref-gdsc -> rightmargin*chrhtxref;
  ylo = 1.0-4*gdsc -> axdescheight*chrhtyref-gdsc -> topmargin*chrhtyref-plotheight*(nplot);
  yhi = 1.0-4*gdsc -> axdescheight*chrhtyref-gdsc -> topmargin*chrhtyref-plotheight*(nplot-1);

  /* Check if the specification is wrong and return 0 if it is */
  if (xlo >= xhi)
    return 0;
  if (ylo >= yhi)
    return 0;


  /* Define the linear dependencies of the window scaling and the user scaling */
  ax = (xmax-xmin)/((xhi-gdsc -> horbord*gdsc -> symbolheight*chrhtxref) - (xlo+gdsc -> horbord*gdsc -> symbolheight*chrhtxref));
  ay =  (ymax-ymin)/((yhi-gdsc -> verbord*gdsc -> symbolheight*chrhtyref) - (ylo+gdsc -> verbord*gdsc -> symbolheight*chrhtyref));
  bx = xmin-ax*(xlo+gdsc -> horbord*gdsc -> symbolheight*chrhtxref);
  by = ymin-ay*(ylo+gdsc -> verbord*gdsc -> symbolheight*chrhtyref);

  /*************/
  /* Numbering */
  /*************/

  /* Set the symbol size (again) */
  cpgsch(gdsc -> numberheight);

  /* Draw the numbers axis by axis */

  /* First define the plot region */
  xlo2 = gdsc -> leftmargin*chrhtxref+3*gdsc -> axdescheight*chrhtxref+gdsc -> leftnum*chrhtxref;
  xhi2 = 1.0-gdsc -> rightmargin*chrhtxref-4*gdsc -> axdescheight*chrhtxref-gdsc -> rightnum*chrhtxref;
  ylo2 = 1.0-4*gdsc -> axdescheight*chrhtyref-gdsc -> topmargin*chrhtyref-plotheight*(nplot)+gdsc -> botnum*chrhtyref;
  yhi2 = 1.0-4*gdsc -> axdescheight*chrhtyref-gdsc -> topmargin*chrhtyref-plotheight*(nplot-1)-gdsc -> topnum*chrhtyref;

  xulo2 = ax*xlo2+bx;
  xuhi2 = ax*xhi2+bx;
  yulo2 = ay*ylo2+by;
  yuhi2 = ay*yhi2+by;

  /* Top and bottom numbering */
  if (nplot == 1) {
    cpgsvp(xlo2, xhi2, ylo, yhi);
    cpgswin(xulo2*btscale+btzero, xuhi2*btscale+btzero, yulo2*lrscale+lrzero, yuhi2*lrscale+lrzero);

    if ((topdeschi) || topdesclo)
      (*boxingx)(steringxt, 0.0, 0, "", 0.0, 0);
  }

  if (nplot == gdsc -> nplots) {
    cpgsvp(xlo2, xhi2, ylo, yhi);

    /* BUGFIX: changed from (...,yulo, yuhi) to (...,yulo2, yuhi2) */
    cpgswin(xulo2, xuhi2, yulo2, yuhi2);

    if ((bottomdeschi) || bottomdesclo)
      (*boxingx)(steringxb, 0.0, 0, "", 0.0, 0);
  }

  /* Left and right numbering */
  cpgsvp(xlo, xhi, ylo2, yhi2);
  cpgswin(xulo2*btscale+btzero, xuhi2*btscale+btzero, yulo2*lrscale+lrzero, yuhi2*lrscale+lrzero);

  if ((rightdeschi) || (rightdesclo))
    (*boxingy)("", 0.0, 0, steringyr, 0.0, 0);
  cpgsvp(xlo, xhi, ylo2, yhi2);
  cpgswin(xulo2, xuhi2, yulo2, yuhi2);

  if ((leftdeschi) || (leftdesclo))
    (*boxingy)("", 0.0, 0, steringyl, 0.0, 0);

  /***************/  
  /* Box drawing */
  /***************/  
  
  /* Set the line width */
  cpgslw(gdsc -> boxlw);

  /* Define an area of operation PGSVP (XMIN, XMAX, YMIN, YMAX), 0 to 1,  y from bottom to top */
  cpgsvp(xlo, xhi, ylo, yhi);
    
  xulo = ax*xlo+bx;
  xuhi = ax*xhi+bx;
  yulo = ay*ylo+by;
  yuhi = ay*yhi+by;

  cpgswin(xulo, xuhi, yulo, yuhi);

/* draw the box with the tickmarks of the lower and left panel */
  (*boxingx)(steribgxb, 0.0, 0, "", 0.0, 0);
  (*boxingy)("", 0.0, 0, steribgyl, 0.0, 0);

  /* Now change the user coordinates */
  cpgswin(xulo*btscale+btzero, xuhi*btscale+btzero, yulo*lrscale+lrzero, yuhi*lrscale+lrzero);

  /* draw the box with the tickmarks of the upper and right panel */
  (*boxingx)(steribgxt, 0.0, 0, "", 0.0, 0);
  (*boxingy)("", 0.0, 0, steribgyr, 0.0, 0);

  /* Reset the linewidth */
  cpgslw(slw);

  /*************/  
  /* Labelling */
  /*************/  

  /* Set character height (again) to the number value */
  cpgsch(gdsc -> numberheight);

  /* Label top */
  if (nplot == 1)
    putalabel('t', topdeschi, topdesclo, gdsc -> axdescheight, gdsc -> topmargin);

  /* Label bottom */
  if (nplot == gdsc -> nplots)
    putalabel('b', bottomdeschi, bottomdesclo, gdsc -> axdescheight, gdsc -> bottommargin);

  /* Label left and right axis */
  putalabel('l', leftdeschi, leftdesclo, gdsc -> axdescheight, gdsc -> leftmargin);
  putalabel('r', rightdeschi, rightdesclo, gdsc -> axdescheight, gdsc -> rightmargin);

  /* At the end we put the viewport and the window at the right values */
  switch (logarcsx) {
  case 2:
    xmin = xmin/240.0;
    xmax = xmax/240.0;
    break;
  case 3:
    xmin = xmin/3600.0;
    xmax = xmax/3600.0;
    break;
  }
  switch (logarcsy) {
  case 2:
    ymin = ymin/240.0;
    ymax = ymax/240.0;
    break;
  case 3:
    ymin = ymin/3600.0;
    ymax = ymax/3600.0;
    break;
  }

  /* Redefine */
  xlo = gdsc -> leftmargin*chrhtxref+3*gdsc -> axdescheight*chrhtxref;
  xhi = 1.0-4*gdsc -> axdescheight*chrhtxref-gdsc -> rightmargin*chrhtxref;
  ylo = 1.0-4*gdsc -> axdescheight*chrhtyref-gdsc -> topmargin*chrhtyref-plotheight*(nplot);
  yhi = 1.0-4*gdsc -> axdescheight*chrhtyref-gdsc -> topmargin*chrhtyref-plotheight*(nplot-1);

  ax = (xmax-xmin)/((xhi-gdsc -> horbord*gdsc -> symbolheight*chrhtxref) - (xlo+gdsc -> horbord*gdsc -> symbolheight*chrhtxref));
  ay =  (ymax-ymin)/((yhi-gdsc -> verbord*gdsc -> symbolheight*chrhtyref) - (ylo+gdsc -> verbord*gdsc -> symbolheight*chrhtyref));
  bx = xmin-ax*(xlo+gdsc -> horbord*gdsc -> symbolheight*chrhtxref);
  by = ymin-ay*(ylo+gdsc -> verbord*gdsc -> symbolheight*chrhtyref);

  xulo = ax*xlo+bx;
  xuhi = ax*xhi+bx;
  yulo = ay*ylo+by;
  yuhi = ay*yhi+by;

  /* Do the redefinition */
  cpgsvp(xlo, xhi, ylo, yhi);
  cpgswin(xulo, xuhi, yulo, yuhi);

  return 1;
}
/* ------------------------------------------------------------ */

   

/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts a label at a viewgraph as a fraction */

int putalabel(char where, char *overfrac, char *underfrac, float charheight, float dist)
{
  float oheight;    /* Character height at call time */
  int fraclength;
  char *fracbar;
  float ovpos; 
  float unpos; 
  float fracpos;

  char format[3];

  float frlen1;
  float frlen2;
  float xbox;
  float ybox;
  float chrhtxref;
  float chrhtyref;

  /* Check if there is anything to put */
  if ((overfrac == NULL) && (underfrac == NULL))
    return 1;

  /***********/
  /* scaling */
  /***********/

  /* Find the reference character height */
  cpgqch(&oheight);

  /* Set the character height to the required value */
  cpgsch(oheight*charheight);

  /* If the strings are empty we return a success */
  if (!(*overfrac)) {
    if (!(*underfrac))
      return 1;
    /* If only one of them is empty, we make the one not empty overfrac */
    else {
      fracbar = overfrac;
      overfrac = underfrac;
      underfrac = fracbar;
    }
  }

  /* A format string is required */
  switch (where) {
  case 'l':
    sprintf(format,"L");

    /* Calculate the maximum possible distance from the edge of the graph */
    ovpos = dist/charheight+2.0;
    fracpos = dist/charheight+1.4;
    unpos = dist/charheight;
    break;
  case 'r':
    sprintf(format,"R");

    /* Calculate the maximum possible distance from the edge of the graph */
    ovpos = dist/charheight+1;
    fracpos = dist/charheight+1.6;
    unpos = dist/charheight+3;
    break;

  case 't':
    sprintf(format,"T");
    ovpos = dist/charheight+2.0;
    fracpos = dist/charheight/charheight+1.4;
    unpos = dist/charheight/charheight+0.0;
    break;

  case 'b':
    sprintf(format,"B");
    ovpos = dist/charheight+1.0;
    fracpos = dist/charheight+1.6;
    unpos = dist/charheight+3.0;
    break;

  default:
    return 0;
  }

  /* To get the length of the fraction bar, first find a bounding box at an arbitrary position */
  cpglen(2, overfrac, &xbox, &ybox);
  frlen1 = ybox;
  cpglen(2, underfrac, &xbox, &ybox);
  frlen2 = ybox;
  frlen1= (frlen1 > frlen2)?frlen1:frlen2;

  /* Now frlen1 is the length of the string in world coordinates of the x axis, we get the character width of this axis in the same units */
  cpgqcs(2, &chrhtxref, &chrhtyref);

  /* Then, the length of the fraction bar is the uprounded fraction */
  fraclength = ((int) (frlen1/chrhtyref))+3;

  if (!(fracbar = (char *) malloc((fraclength+1)*sizeof(char))))
    return 0;

  fracbar[fraclength] = '\0';

  while (fraclength > 0) {
    fracbar[fraclength-1] = '_';
    --fraclength;
  }


  /************/
  /* plotting */
  /************/

  /* If there is a fraction bar to plot do this */
  if (*underfrac) {
  /* First plot the character string above the fraction bar */
  cpgmtxt(format, ovpos, 0.5, 0.5, overfrac);

  /* Now draw the fraction bar */ 
  cpgmtxt(format, fracpos, 0.5, 0.5, fracbar);

  /* At last put the argument below the fraction bar */
  cpgmtxt(format, unpos, 0.5, 0.5, underfrac);
  }

  /* No fraction bar */
  else {
    cpgmtxt(format, unpos, 0.5, 0.5, overfrac);
  }

  /* Deallocate */
  free(fracbar);

  /* Set the character height to the original value */
  cpgsch(oheight);

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts a string to the specified legend position */
int pgp_legend(pgp_gdsc *gdsc, int nhor, int nver, char *string)
{
  float chrhtxref;    /* Horizontal character width in x-map units, 1 is whole page */
  float chrhtyref;    /* Vertical character height in y-map units, 1 is whole page */
  /* float charwidth; */
  float posix;
  float posiy;
  float plotheight;
  /* float boxwidth; */
  float xlo; /* Box in window coordinates */
  float xhi;
  float ylo;
  float yhi;
  
  /* First check if the intended legend position is available */
  if (nver < 1 || nver > gdsc -> legendrows || nhor < 1 || nhor > gdsc -> legendcols) {
    return 0;
  }

  /* Then open a box that nearly spans the whole surface at the bottom position of the graph */

  /* Find the reference */
  cpgsch(gdsc -> numberheight);
  cpgqcs(0, &chrhtxref, &chrhtyref);

  /* plot height */
  plotheight = (1.0 - 2.0*(gdsc -> legendrows+1)*gdsc -> legendheight*chrhtyref - 6*gdsc -> axdescheight*chrhtyref-6*chrhtyref)/gdsc -> nplots;
  ylo = 1.0-4*gdsc -> axdescheight*chrhtyref-gdsc -> topmargin*chrhtyref-plotheight*(gdsc -> nplots);
  yhi = 1.0-4*gdsc -> axdescheight*chrhtyref-gdsc -> topmargin*chrhtyref-plotheight*(gdsc -> nplots-1);

  /* At the left hand the box has the distance of one symbol to the border of the plotting surface */
  cpgsch(gdsc -> numberheight*gdsc -> legendheight);
  cpgqcs(0, &chrhtxref, &chrhtyref);

  xlo = chrhtxref;
  xhi = 1.0-chrhtxref;

  /* Check if the specification is wrong and return 0 if it is */
  if (xlo >= xhi)
    return 0;
  if (ylo >= yhi)
    return 0;

  /* Define the surface */
  cpgsvp(xlo, xhi, ylo, yhi);

  /* This is the length of a box element in units of the x-axis, spacing is one character between rows */
  /* boxwidth = (1-(gdsc -> legendcols-1)*charwidth)/gdsc -> legendcols; */

  /* This is the position of the string in x along the axis */
  posix = (nhor-0.5)/gdsc -> legendcols;

  /* Now in y */
  posiy = gdsc -> bottommargin/gdsc -> legendheight+3*gdsc -> axdescheight/gdsc -> legendheight+2*nver;

  /* Put it on the graph */
  cpgmtxt("B", posiy, posix, 0.5, string);
  
  /* Reset the character height */
  cpgsch(gdsc -> numberheight);

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts markers on a predefined viewport */
int pgp_marker(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, int colour, int empty, int symbol, float sizer)
{
  int colourbef;
  float chrhtxref, chrhtyref;
  int intlw;
  float *xposd = NULL;
  float *yposd = NULL;
  int i;

  /* Allocate space if necessary */
  if (gdsc -> logarcsx == 1) {
    if (!(xposd = (float *) malloc(npoints*sizeof(float))))
      return 1;
    for (i=0; i < npoints; ++i) {
	xposd[i] = pgp_logf10f(xpos[i]);
    }
    xpos = xposd;
  }
  if (gdsc -> logarcsy == 1) {
    if (!(yposd = (float *) malloc(npoints*sizeof(float)))) {
      if ((xposd))
	free(xposd);
      return 2;
    }
    for (i=0; i < npoints; ++i) {
	yposd[i] = pgp_logf10f(ypos[i]);
    }
    ypos = yposd;
  }

  /* Set the character height */
  if (symbol == -1)
    cpgsch(gdsc -> numberheight*gdsc -> symbolheight*sizer);
  else if (symbol == 3)
    cpgsch(gdsc -> numberheight*gdsc -> symbolheight*2*sizer*1.2);
  else 
    cpgsch(gdsc -> numberheight*gdsc -> symbolheight*2*sizer);

  /* Inquire the character height in inches */
  cpgqcs(1, &chrhtxref, &chrhtyref);

  /* Set the linewidth as an equivalent */
  if (symbol == -1) {
    intlw = chrhtxref*200.0>201.0?201: (int) (chrhtxref*200.0);
  }
  else 
    intlw = 1;
  cpgslw(intlw);

  /* Store the colour index */
  cpgqci(&colourbef);

  /* Set the colour index */
  cpgsci(colour);

 /* Plot points, number of points, vectors for x and y, symbol */
  cpgpt(npoints, xpos, ypos, symbol);

  /* If required, plot the same list on top of that with background colour and with a radius that is smaller */
  if ((empty)) {
    cpgslw(((intlw-4*gdsc -> graphlw) > 0)?(intlw-4*gdsc -> graphlw):0);
    cpgsci(0);
    cpgpt(npoints, xpos, ypos, symbol);
  }

  /* Reset the linewidth */
  cpgslw(gdsc -> graphlw);

  /* Reset the colour index */
  cpgsci(colourbef);

  /* Reset the character height */
  cpgsch(gdsc -> numberheight);

  /* Reset the linewidth */
  cpgslw(gdsc -> boxlw);

  /* Deallocate */

  if (gdsc -> logarcsy == 1) {
    free(yposd);
  }

  if (gdsc -> logarcsx == 1) {
    free(xposd);
  }

  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts lines on a predefined viewport */
int pgp_lines(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, int colour)
{
  int i;
  int colourbef;
  float *xposd = NULL;
  float *yposd = NULL;

  if (gdsc -> interptype_lines != PGP_I_LINEAR) {
    if (pgp_get_interparray(xpos, ypos, npoints, gdsc -> interptype_lines, gdsc -> interp_numlines+1, &xposd, &yposd))
      goto error;
    npoints = gdsc -> interp_numlines+1;
    xpos = xposd;
    ypos = yposd;
  }

  /* Allocate space if necessary */
  if (gdsc -> logarcsx == 1) {
    if (!xposd) {
      if (!(xposd = (float *) malloc(npoints*sizeof(float))))
	goto error;
    }
    for (i=0; i < npoints; ++i) {
	xposd[i] = pgp_logf10f(xpos[i]);
    }
    xpos = xposd;
  }
  if (gdsc -> logarcsy == 1) {
    if (!yposd) {
      if (!(yposd = (float *) malloc(npoints*sizeof(float))))
	goto error;
    }
    for (i=0; i < npoints; ++i) {
	yposd[i] = pgp_logf10f(ypos[i]);
    }
    ypos = yposd;
  }

  /* Store the colour index */
  cpgqci(&colourbef);

  /* Set the colour index */
  cpgsci(colour);

  /* Set the linewidth */
  cpgslw(gdsc -> graphlw);

 /* Plot lines, number of points, vectors for x and y, symbol */
  cpgline(npoints, xpos, ypos);

  /* Reset the linewidth */
  cpgslw(gdsc -> graphlw);

  /* Reset the colour index */
  cpgsci(colourbef);

  /* Reset the linewidth */
  cpgslw(gdsc -> boxlw);

  /* Deallocate */
  if ((xposd))
    free(xposd);
  if ((yposd))
    free(yposd);
  return 0;

 error:
  if ((xposd))
    free(xposd);
  if ((yposd))
    free(yposd);
  return 1;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts lines on a predefined viewport */
int pgp_bars(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, float width, int colour)
{
  int colourbef,i;
  float *xposd = NULL;
  float *yposd = NULL;

  /* Allocate space if necessary */
  if (gdsc -> logarcsx == 1) {
    if (!(xposd = (float *) malloc(npoints*sizeof(float))))
      return 1;
    for (i=0; i < npoints; ++i) {
      xposd[i] = pgp_logf10f(xpos[i]);
    }
    xpos = xposd;
  }
  if (gdsc -> logarcsy == 1) {
    if (!(yposd = (float *) malloc(npoints*sizeof(float)))) {
      if ((xposd))
	free(xposd);
      return 2;
    }
    for (i=0; i < npoints; ++i) {
	yposd[i] = pgp_logf10f(ypos[i]);
    }
    ypos = yposd;
  }

  /* Store the colour index */
  cpgqci(&colourbef);

  /* Set the colour index */
  cpgsci(colour);

  /* Set the linewidth */
  cpgslw(gdsc -> graphlw);

 /* Plot lines, number of points, vectors for x and y, symbol */
  for (i = 0; i < npoints; ++i)
  cpgerr1(5, xpos[i], ypos[i], width/2, 0.0);

  /* Reset the linewidth */
  cpgslw(gdsc -> graphlw);

  /* Reset the colour index */
  cpgsci(colourbef);

  /* Reset the linewidth */
  cpgslw(gdsc -> boxlw);


  /* Deallocate */
  if (gdsc -> logarcsy == 1) {
    free(yposd);
  }

  if (gdsc -> logarcsx == 1) {
    free(xposd);
  }

  return 0;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Puts lines on a predefined viewport */
int pgp_errby(pgp_gdsc *gdsc, int npoints, float *xpos, float *ypos, float *err, int colour)
{
  int colourbef, i;
  float *xposd = NULL;
  float *yposdl = NULL;
  float *yposdr = NULL;

  /* Allocate space if necessary */
  if (gdsc -> logarcsx == 1) {
    if (!(xposd = (float *) malloc(npoints*sizeof(float))))
      return 1;
    for (i=0; i < npoints; ++i) {
	xposd[i] = pgp_logf10f(xpos[i]);
    }
    xpos = xposd;
  }

  if (gdsc -> logarcsy == 1) {
    if (!(yposdl = (float *) malloc(npoints*sizeof(float)))) {
      if ((xposd))
	free(xposd);
      return 1;
    }
    if (!(yposdr = (float *) malloc(npoints*sizeof(float)))) {
      if ((xposd))
	free(xposd);
      free(yposdl);
      return 1;
    }
    for (i=0; i < npoints; ++i) {
	yposdr[i] = pgp_logf10f(ypos[i]+err[i]);
      }
    for (i=0; i < npoints; ++i) {
	yposdl[i] = pgp_logf10f(ypos[i]-err[i]);
    }
  }

  /* Store the colour index */
  cpgqci(&colourbef);

  /* Set the colour index */
  cpgsci(colour);

  /* Set the linewidth */
  cpgslw(gdsc -> graphlw);

 /* Plot lines, number of points, vectors for x and y, symbol */
  if (gdsc -> logarcsy == 1) 
    cpgerry(npoints, xpos, yposdl, yposdr, 2.0);
  else
    cpgerrb(6, npoints, xpos, ypos, err, 2.0);

  /* Reset the linewidth */
  cpgslw(gdsc -> graphlw);

  /* Reset the colour index */
  cpgsci(colourbef);

  /* Reset the linewidth */
  cpgslw(gdsc -> boxlw);

  /* Clean up */
  if (gdsc -> logarcsx == 1)
    free(xposd);
  if (gdsc -> logarcsy == 1) {
    free(yposdl);
    free(yposdr);
  }

  return 0;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* End the plotting and close the device */
void pgp_end()
{
  cpgclos();
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns the logarithm on basis 10 and LOGFAULT if x <= 0 */
float pgp_logf10f(float x)
{
  if (x > 0)
    return log10f(x);
  else
    return LOGFAULT;
}

/* ------------------------------------------------------------ */


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns an array of interpolated values. */

static int pgp_get_interparray(float *x, float *y, int npoints, int interptype, int noutpoints, float **xout, float **yout)
{
  int i, pgp_get_interparray = 0;
  double *xdouble = NULL, *ydouble = NULL; /* input is float, but gsl requires double */
  gsl_interp *gsl_interpv = NULL; /* gsl structs */
  gsl_interp_accel *gsl_interp_accelv = NULL; /* gsl structs */
  const gsl_interp_type * intytype = NULL; /* interpolation type, internal to gsl */
  float dx;

  /* At least two data points */
  if (npoints < 2) {
    pgp_get_interparray |= 1;
    goto error;
  }

  /* Wipe the input, gone ... */
  if (*xout) {
    free(*xout);
    *xout = NULL;
  }  
  if (*yout) {
    free(*yout);
    *yout = NULL;
  }

  /* Allocate arrays */
  if (!(xdouble = (double *) malloc(npoints*sizeof(double)))) {
    pgp_get_interparray |= 2;
    goto error;
  }
  if (!(ydouble = (double *) malloc(npoints*sizeof(double)))) {
    pgp_get_interparray |= 2;
    goto error;
  }
  for (i = 0; i < npoints; ++i) {
    xdouble[i] = x[i];
    ydouble[i] = y[i];
  }
  if (!(*xout = (float *) malloc(noutpoints*sizeof(float)))) {
    pgp_get_interparray |= 2;
    goto error;
  }
  if (!(*yout = (float *) malloc(noutpoints*sizeof(float)))) {
    pgp_get_interparray |= 2;
    goto error;
  }

  switch(interptype) {
    case PGP_I_LINEAR:
      intytype = gsl_interp_linear;
      break;

    case PGP_I_CSPLINE:
      if (npoints > 2)
	intytype = gsl_interp_cspline;
      else {
	printf("Must have at least 3 radii for spline, using linear\n");
	intytype = gsl_interp_linear;
      }
      break;
    
    case PGP_I_AKIMA:
      if (npoints > 4) {
	intytype = gsl_interp_akima;
      }
      else {
	printf("Must have at least 5 radii for Akima\n");
	if (npoints > 2) {
	  printf("Using natural cubic spline\n");
	  intytype = gsl_interp_cspline;
	}
	else {
	  printf("Using linear\n");
	  intytype = gsl_interp_linear;
	}
      }
      break;

    /* No real default, but we set linear */
    default:
      intytype = gsl_interp_linear;
      break;
    }

  /* Attempt to allocate all gsl structures */
  if (!(gsl_interpv = gsl_interp_alloc(intytype, npoints))) {
    pgp_get_interparray |= 2;
    goto error;
  }
  if (!(gsl_interp_accelv = gsl_interp_accel_alloc())) {
    pgp_get_interparray |= 2;
    goto error;
  }

  gsl_interp_init(gsl_interpv, xdouble, ydouble, npoints);
  
  (*xout)[0] = x[0];
  (*yout)[0] = y[0];

  dx = (x[npoints-1]-x[0])/((float)(noutpoints-1));

  /* Bugfix: rounding error could lead to failure of interpolation */
  --noutpoints;

  for (i = 1; i < noutpoints; ++i) {
    (*xout)[i] = (*xout)[i-1]+dx;
    (*yout)[i] = gsl_interp_eval(gsl_interpv, xdouble, ydouble, (*xout)[i], gsl_interp_accelv);
  }
  (*xout)[i] = x[npoints-1];
  (*yout)[i] = y[npoints-1];
  
  /* last points should be identical regardless of rounding errors */
    free(xdouble);
    free(ydouble);
    gsl_interp_free(gsl_interpv);
    gsl_interp_accel_free(gsl_interp_accelv);

  return pgp_get_interparray;

 error:
  if (xdouble)
    free(xdouble);
  if (ydouble)
    free(ydouble);
  if (*xout) {
    free(*xout);
    *xout = NULL;
  }  
  if (*yout) {
    free(*yout);
    *yout = NULL;
  }
  if (gsl_interpv)
    gsl_interp_free(gsl_interpv);
  if (gsl_interp_accelv)
    gsl_interp_accel_free(gsl_interp_accelv);

  return pgp_get_interparray;  
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: pgp.c,v $
   Revision 1.14  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.13  2011/05/10 00:30:16  jozsa
   Left work

   Revision 1.12  2009/05/26 07:56:41  jozsa
   Left work

   Revision 1.11  2007/08/22 15:58:43  gjozsa
   Left work

   Revision 1.10  2006/11/22 14:16:21  gjozsa
   Bugfix concerning RASH and horizontal/vertical lines

   Revision 1.9  2006/11/03 10:49:02  gjozsa
   Introduced logarithmic scaling, hms dms

   Revision 1.8  2006/10/05 12:39:58  gjozsa
   nasty bugfix

   Revision 1.7  2006/04/03 11:47:57  gjozsa
   Left work

   Revision 1.6  2006/02/08 16:13:17  gjozsa
   Extended graphics routines

   Revision 1.5  2005/10/12 14:51:25  gjozsa
   First point gets thick in the polar plot

   Revision 1.4  2005/10/12 09:53:09  gjozsa
   Included polar plot facility

   Revision 1.3  2005/06/24 12:00:29  gjozsa
   Left work

   Revision 1.2  2005/05/24 10:42:24  gjozsa
   Left work

   Revision 1.1  2005/05/17 12:58:15  gjozsa
   Added to cvs control


   ------------------------------------------------------------ */

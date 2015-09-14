/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/**
   @file cubarithm.c
   @brief Simple arithmetics with Cubes

   This module contains all mathematical operations done solely on
   cubes as a stand-alone. All operations that work on cubes alone are
   packed in here, while interfacing between Cubes and other internal
   structures (as Pointsource ll's) is defined and documented
   elsewhere.

   $Source: /Volumes/DATA_J_II/data/CVS/tirific/src/cubarithm.c,v $
   $Date: 2011/05/25 22:25:26 $
   $Revision: 1.6 $
   $Author: jozsa $
   $Log: cubarithm.c,v $
   Revision 1.6  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.5  2011/05/10 00:30:15  jozsa
   Left work

   Revision 1.4  2009/05/26 07:56:40  jozsa
   Left work

   Revision 1.3  2007/08/23 15:23:25  gjozsa
   Left work

   Revision 1.2  2007/08/22 15:58:40  gjozsa
   left work

   Revision 1.1.1.1  2004/10/29 11:13:20  gjozsa
   Added to CVS control


*/
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* EXTERNAL INCLUDES */
/* ------------------------------------------------------------ */
#include <errno.h>
#include <fftw3.h>
#include <time.h>
#include <wcs.h>
#include <wcshdr.h>


/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* INTERNAL INCLUDES */
/* ------------------------------------------------------------ */

#include <qfits.h>
#include <cubarithm.h>
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
#include <maths.h>
#include <wcs.h>



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
/* PRIVATE SYMBOLIC CONSTANTS */
/* ------------------------------------------------------------ */
#define HIRESFREQ 1.420405751786E9
#define SPEEDOFLIGHT 2.99792458E5

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
/* FUNCTION CODE */
/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Setting all pixels in a Cube cube to 0 */

void cuberase(Cube *cube)
{
  int i,j,k, newsize;

  newsize = (*cube).size_x+(*cube).padding;

  /* Do it */
  for (i = 0; i < newsize; ++i)
    for (j = 0; j < (*cube).size_y; ++j)
      for (k = 0; k < (*cube).size_v; ++k)
	(*cube).points[i+newsize*(j+(*cube).size_y*k)] = 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Find the absolute pixel in a Cube */

float *findpixel(Cube *cube, double x, double y, double v)
{
  if (x-(*cube).refpix_x >= 0. && x-(*cube).refpix_x < (*cube).size_x && y-(*cube).refpix_y >= 0. && y-(*cube).refpix_y < (*cube).size_y && v-(*cube).refpix_v >= 0. && v-(*cube).refpix_v < (*cube).size_v)
    return &((*cube).points)[roundnormal(x-(*cube).refpix_x)+((*cube).size_x+(*cube).padding)*(roundnormal(y-(*cube).refpix_y)+(*cube).size_y*roundnormal(v-(*cube).refpix_v))];
  else
    return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Find the relative pixel in a Cube */

float *findpixelrel(Cube *cube, double x, double y, double v, int padding)
{
  if (x >= 0. && x < ((double) (*cube).size_x) && y >= 0. && y < ((double) (*cube).size_y) && v >= 0. && v < ((double) (*cube).size_v))
    return &((*cube).points)[roundnormal(x)+((*cube).size_x+padding)*(roundnormal(y)+(*cube).size_y*roundnormal(v))];
  else
    return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Returns the measure of a cube containing an ellipsoid */

int *calcbordergauss(float sigma_maj, float sigma_min, float sigma_v, float *cossinpa, float n)
{
  float calcbordergauss[3];
  int *calcbordergaussint;
  float t;

  if (fabs(cossinpa[0]) <= 1 && fabs(cossinpa[1]) <= 1 && sigma_maj >= 0 && sigma_min >= 0 && n >= 0 ) {

    if (cossinpa[1] && sigma_min)
      t = atan(-(cossinpa[0]*sigma_maj)/(cossinpa[1]*sigma_min));
    else 
      t =  cossinpa[0]*PI_HERE/(2*fabs(cossinpa[0]));

    *calcbordergauss = n*fabs((cossinpa[1]*sigma_min*cos(t))-cossinpa[0]*sigma_maj*sin(t));

    if (cossinpa[0] && sigma_maj)
      t = atan((cossinpa[1]*sigma_maj)/(cossinpa[0]*sigma_min));
    else
      t =  cossinpa[1]*PI_HERE/(2*fabs(cossinpa[1]));

    *(calcbordergauss+1) = n*fabs((cossinpa[0]*sigma_min*cos(t))+cossinpa[1]*sigma_maj*sin(t));

    *(calcbordergauss+2) = n*sigma_v;

    calcbordergaussint = imax(calcbordergauss);
  }						  
  else {
    calcbordergaussint = NULL;
  }
  return calcbordergaussint;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Change the padding in a cube */

Cube *padcubex(Cube *cube)
{
  int i,j,k;
  size_t counter;
  float *dummy;

  if (cube) {
    /* if the cube contains a padding it should be removed */
    if (cube -> padding) {
      
      /* This is important to do in the right order the lowest index changes most rapid */
      for(k = 0; k < (*cube).size_v; ++k)
		  for(j = 0; j < (*cube).size_y; ++j)
			 for(i = 0; i < (*cube).size_x; ++i)
				*findpixelrel(cube, i, j, k, 0) = *findpixelrel(cube, i, j, k, (*cube).padding);
      /* Now the padding has been removed. Realloc the array */
		/*       realloc((*cube).points, (*cube).size_v*(*cube).size_y*(*cube).size_x*sizeof(float)); */
      if(!(dummy = (float *) fftwf_malloc((*cube).size_v*(*cube).size_y*(*cube).size_x*sizeof(float))))
		  goto error;
      for (counter = 0; counter < cube -> size_v*cube -> size_y*cube -> size_x; ++counter)
		  dummy[counter] = cube -> points[counter];
		
		
      /* This should mean that the old array is not linked elsewhere */
      fftwf_free(cube -> points);
      cube -> points = dummy;
		
      /* Change the padding */
      (*cube).padding = 0;
    }
    
    /* if there's no padding then create some */
    else {
      /* Now realloc first */
		/*       return NULL; */
		/*       realloc(cube -> points, (*cube).size_v*(*cube).size_y*(((*cube).size_x/2)*2+2)*sizeof(float)); */
      if (!(dummy = (float *) fftwf_malloc((*cube).size_v*(*cube).size_y*(((*cube).size_x/2)*2+2)*sizeof(float))))
		  goto error;
      for (counter = 0; counter < cube -> size_v*cube -> size_y*cube -> size_x; ++counter)
		  dummy[counter] = cube -> points[counter];
      fftwf_free(cube -> points);
      cube -> points = dummy;
		
      /* change the padding */
      (*cube).padding = ((*cube).size_x/2)*2+2-(*cube).size_x;
      
      /* Then change the contents according to the padding start with the last pixel and work the way backwards through */
      for(k = (*cube).size_v-1; k >= 0; --k)
		  for(j = (*cube).size_y-1; j >= 0; --j)
			 for(i = (*cube).size_x-1; i >= 0; --i)
				*findpixelrel(cube, i, j, k, (*cube).padding) = *findpixelrel(cube, i, j, k, 0);
      
    }
  }
  return cube;  
  
 error:
  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Change the padding in a cube without reallocating disk space */

Cube *padcubex_norealloc(Cube *cube)
{
  int i,j,k;
  /* size_t counter; */
  /* float *dummy; */

  if (cube) {
    /* if the cube contains a padding it should be removed */
    if (cube -> padding) {
      
      /* This is important to do in the right order the lowest index changes most rapid */
      for(k = 0; k < (*cube).size_v; ++k)
		  for(j = 0; j < (*cube).size_y; ++j)
			 for(i = 0; i < (*cube).size_x; ++i)
				*findpixelrel(cube, i, j, k, 0) = *findpixelrel(cube, i, j, k, (*cube).padding);
      /* Now the padding has been removed. Realloc the array */
      /* if(!(dummy = (float *) fftwf_malloc((*cube).size_v*(*cube).size_y*(*cube).size_x*sizeof(float)))) */
		/*  goto error; */
      /* for (counter = 0; counter < cube -> size_v*cube -> size_y*cube -> size_x; ++counter) */
		/* dummy[counter] = cube -> points[counter]; */
		
		
      /* This should mean that the old array is not linked elsewhere */
      /* fftwf_free(cube -> points); */
      /* cube -> points = dummy; */
		
      /* Change the padding */
      (*cube).padding = 0;
    }
    
    /* if there's no padding then create some */
    else {
      /* Now realloc first not */
      /* if (!(dummy = (float *) fftwf_malloc((*cube).size_v*(*cube).size_y*(((*cube).size_x/2)*2+2)*sizeof(float)))) */
		/*  goto error; */
      /* for (counter = 0; counter < cube -> size_v*cube -> size_y*cube -> size_x; ++counter) */
		/*   dummy[counter] = cube -> points[counter]; */
      /* fftwf_free(cube -> points); */
      /* cube -> points = dummy; */
		
      /* change the padding */
      (*cube).padding = ((*cube).size_x/2)*2+2-(*cube).size_x;
      
      /* Then change the contents according to the padding start with the last pixel and work the way backwards through */
      for(k = (*cube).size_v-1; k >= 0; --k)
		  for(j = (*cube).size_y-1; j >= 0; --j)
			 for(i = (*cube).size_x-1; i >= 0; --i)
				*findpixelrel(cube, i, j, k, (*cube).padding) = *findpixelrel(cube, i, j, k, 0);
      
    }
  }
  return cube;  
  
 /* error: */
 /*  return NULL; */
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Adding cubes in the overlapping region */

Cube *addcubes(Cube *cube1, Cube *cube2)
{
  int xmin, xmax, ymin, ymax, vmin, vmax, i, j, k;
  float *pointertoapixel;
  float scale;

  if (cube1 && cube2 && (*cube1).points && (*cube2).points) {
  /* find the region in which cubes will be added */
    
  /* Start finding the absolute pixel coordinates of the maximum and the minimum in x */
    if ((*cube1).refpix_x <= (*cube2).refpix_x)
      xmin = (*cube2).refpix_x;
    else
      xmin = (*cube1).refpix_x;
    if (((*cube1).refpix_x + (*cube1).size_x - 1) <= ((*cube2).refpix_x + (*cube2).size_x - 1))
      xmax = ((*cube1).refpix_x + (*cube1).size_x - 1);
    else
      xmax = ((*cube2).refpix_x + (*cube2).size_x - 1);
    
  /* stop if the regions prove disjunct */
    if (xmax >= xmin) {
      /* Start finding the absolute pixel coordinates of the maximum and the minimum in y */
      if ((*cube1).refpix_y <= (*cube2).refpix_y)
	ymin = (*cube2).refpix_y;
      else
	ymin = (*cube1).refpix_y;
      if (((*cube1).refpix_y + (*cube1).size_y - 1) <= ((*cube2).refpix_y + (*cube2).size_y - 1))
	ymax = ((*cube1).refpix_y + (*cube1).size_y - 1);
      else
	ymax = ((*cube2).refpix_y + (*cube2).size_y - 1);
      
      /* stop if the regions prove disjunct */
      if (ymax >= ymin) {
	/* Start finding the absolute pixel coordinates of the maximum and the minimum in v */
	if ((*cube1).refpix_v <= (*cube2).refpix_v)
	vmin = (*cube2).refpix_v;
	else
	  vmin = (*cube1).refpix_v;
	if (((*cube1).refpix_v + (*cube1).size_v - 1) <= ((*cube2).refpix_v + (*cube2).size_v - 1))
	  vmax = ((*cube1).refpix_v + (*cube1).size_v - 1);
	else
	  vmax = ((*cube2).refpix_v + (*cube2).size_v - 1);
	
    /* stop if the regions prove disjunct */
	if (vmax >= vmin) {
	  
	  /* the scale of the cubes could be different, so calculate the scale */
	  scale = (*cube2).scale/(*cube1).scale;

	  /* Now add cube2 on cube1 in the overlap region */
	  for (i = xmin; i <= xmax; ++i) {
	    for (j = ymin; j <= ymax; ++j) {
	      for (k = vmin; k <= vmax; ++k) {
		pointertoapixel = findpixel(cube1, i, j, k);
		*pointertoapixel = *pointertoapixel+scale**findpixel(cube2, i, j, k);
	      }
	    }
	  }
	}
      }
    }
    return cube1;
  }
  else
    return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Update an old header with a new one */

qfits_header *qfits_header_update(qfits_header *old, qfits_header *new)
{
  qfits_header *dummy;
  int i;
  char key[9],value[72],comment[72], appendline[9], *string, string2[9];

  /* Ensure that a copy is possible, at least very crudely */
  if (!(new && old && (dummy = qfits_header_copy(new))))
    return NULL;
  else {
    qfits_header_destroy(dummy);
  }
  
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

	/* if we encounter a comment the whole card will be appended to the end of old */
	if (!(strcmp(key,"COMMENT") && strcmp(key,"HISTORY") && strcmp(key,""))) {

	  /* qfits reads the comment as a value */
	  qfits_header_add(old, key, value, NULL, NULL);
	}
	/* Everything else is being modified */

	/* if we find the matching key replace it */
	else if (!strcmp(string2, key)) {
	    qfits_header_mod(old, key, value, comment);
	}

	/* if we don't find it we append it to the body containing valuable information */
	else {
	  qfits_header_add_after(old, appendline, key, value, comment, NULL);
	}
  }

  /* That's it */
  return old;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Update an old header with a new card */

qfits_header *ftsout_putcard(qfits_header *old, char *key, char *value)
{
  qfits_header *new;
  int i;
  char keyl[9],valuel[72],comment[72], appendline[9], *string, string2[9];

  /* If old is NULL, we create old */
  if (!(old)) {
    if (!(old = qfits_header_default()))
      return NULL;
  }

  /* Make one header with the valuel */
  if (!(new = qfits_header_new())) {
    qfits_header_destroy(old);
    return NULL;
  }
  qfits_header_append(new, key, value, NULL, NULL);

  /* Now we think that we can append new to old */
  
  /* check for the first comment in old */
  for (i = 0; i < old -> n; ++i) {
    qfits_header_getitem(old, i, keyl, NULL, NULL, NULL);
    if (!(strcmp(keyl,"COMMENT") && strcmp(keyl,"HISTORY") && strcmp(keyl,""))) {
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
    qfits_header_getitem(new, i, keyl, valuel, comment, NULL);
    
	 /* Initialise string2 */
	 sprintf(string2, "        ");

    if((string = qfits_header_findmatch(old, keyl))){
      sprintf(string2,"%s",string);
    }
    
    /* if we encounter a comment the whole card will be appended to the end of old */
    if (!(strcmp(keyl,"COMMENT") && strcmp(keyl,"HISTORY") && strcmp(keyl,""))) {
      
      /* qfits reads the comment as a valuel */    
      qfits_header_add(old, keyl, valuel, NULL, NULL);
    }
    /* Everything else is being modified */
    
    /* if we find the matching keyl replace it */
    else if (!strcmp(string2, keyl)) {
      qfits_header_mod(old, keyl, valuel, comment);
    }
    
    /* if we don't find it we append it to the body containing valuable information */
    else {
      qfits_header_add_after(old, appendline, keyl, valuel, comment, NULL);
    }
  }

  qfits_header_destroy(new);
  return old;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Synonyme of qfits_header_destroy */
void ftsout_header_destroy(qfits_header *header)
{
  qfits_header_destroy(header);
  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Write a cube using the reference frame in header */

int ftsout_writecube(char *filename, Cube *cubename, qfits_header *header)
{
  FILE *output;
  qfits_header *outheader;
  qfitsdumper qdumper;
  char value[21];
  char *comment;
  int padbef = 0;

  /* quickly check if the cube is there */
  if ((!cubename) || (!(*cubename).points))
    return 0;

  /* open the file and check if writeable */
  if ((output = fopen(filename,"w")) == 0)
    return 0;

  /* Now copy the header */
  outheader = qfits_header_copy(header);

  /* The changes to be made is the size of the cube, and the reference
     pixel. All other stuff has been done before */

  /* size of the cube */
  comment = qfits_header_getcom(outheader,"NAXIS1");
  sprintf(value,"%u",(*cubename).size_x);
  qfits_header_mod(outheader,"NAXIS1",value,comment);
  
  comment = qfits_header_getcom(outheader,"NAXIS2");
  sprintf(value,"%u",(*cubename).size_y);
  qfits_header_mod(outheader,"NAXIS2",value,comment);
  
  comment = qfits_header_getcom(outheader,"NAXIS3");
  sprintf(value,"%u",(*cubename).size_v);
  qfits_header_mod(outheader,"NAXIS3",value,comment);
  
  /* reference pixel */
  comment = qfits_header_getcom(outheader,"CRPIX1");
  sprintf(value,"%0.13E",qfits_header_getdouble(outheader,"CRPIX1",0)-(double)(*cubename).refpix_x);
  qfits_header_mod(outheader,"CRPIX1",value,comment);
  
  comment = qfits_header_getcom(outheader,"CRPIX2");
  sprintf(value,"%0.13E",qfits_header_getdouble(outheader,"CRPIX2",0)-(double)(*cubename).refpix_y);
  qfits_header_mod(outheader,"CRPIX2",value,comment);
  
  comment = qfits_header_getcom(outheader,"CRPIX3");
  sprintf(value,"%0.13E",qfits_header_getdouble(outheader,"CRPIX3",0)-(double)(*cubename).refpix_v);
  qfits_header_mod(outheader,"CRPIX3",value,comment);
  
  comment = qfits_header_getcom(outheader,"BSCALE");
  sprintf(value,"%0.13E", (double)(*cubename).scale);
  qfits_header_mod(outheader,"BSCALE",value,comment);

  /* Now write the modified header in the file and close it */
  qfits_header_dump(outheader,output);
  fclose(output);

  /* check the padding */
  if ((*cubename).padding) {
    padcubex(cubename);
    padbef = 1;
  }

  /* Fill the qdumper fields */
  qdumper.filename = filename;
  qdumper.npix = (*cubename).size_x*(*cubename).size_y*(*cubename).size_v;
  qdumper.ptype = PTYPE_FLOAT;
  qdumper.fbuf = (*cubename).points;
  qdumper.out_ptype = BPP_IEEE_FLOAT;
     
  /* write the cube */
  qfits_pixdump(&qdumper);

  /* now pad the fitsfile with zeros */
  qfits_zeropad(filename);
  
  /* don't forget to destroy the header */
  
  qfits_header_destroy(outheader);

  /* Redo padding if required */
  if ((padbef)) {
    padcubex(cubename);
  }

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Write a cube using the reference frame in header */

int ftsout_writeimage(char *filename, float *array, qfits_header *header, int nx, int ny)
{
  FILE *output;
  qfitsdumper qdumper;
  char value[21];

  /* quickly check if the cube is there */
  if (!array)
    return 0;

  /* open the file and check if writeable */
  if ((output = fopen(filename,"w")) == 0)
    return 0;

  /* The pressing changes to be made is the sizes and the naxis keyword */
  sprintf(value,"%u", nx);
  ftsout_putcard(header, "NAXIS1", value);

  sprintf(value,"%u", ny);
  ftsout_putcard(header, "NAXIS2", value);

  sprintf(value,"%0.12E", 1.0);
  ftsout_putcard(header, "BSCALE", value);

  sprintf(value,"%0.12E", 0.0);
  ftsout_putcard(header, "BZERO", value);

  /* Now write the modified header in the file and close it */
  qfits_header_dump(header,output);
  fclose(output);

  /* Fill the qdumper fields */
  qdumper.filename = filename;
  qdumper.npix = nx*ny;
  qdumper.ptype = PTYPE_FLOAT;
  qdumper.fbuf = array;
  qdumper.out_ptype = BPP_IEEE_FLOAT;

  /* write the cube */
  qfits_pixdump(&qdumper);

  /* now pad the fitsfile with zeros */
  qfits_zeropad(filename);

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Calculate a gaussian */

float cubarithm_fftgaussian (int nx, int ny, int nv, float *expofacs)
{ 
  /* As the trial to safe some time as seen below failed for some reason, we postpone it */
/*   float number; */

  /* If the floating point is low enough, we can simply return 0 */
/*   if ((number = expofacs[0]*nx*nx+expofacs[1]*nx*ny+expofacs[2]*ny*ny+expofacs[3]*nv*nv+expofacs[4]) <= MINEXPONENT) */
/*     return exp(number); */
/*   else */
/*   return 0.0; */
    return expf(expofacs[0]*nx*nx+expofacs[1]*nx*ny+expofacs[2]*ny*ny+expofacs[3]*nv*nv+expofacs[4]);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Calculate a gaussian */

float cubarithm_fftgaussian2d (int nx, int ny, float *expofacs)
{
/*   double number; */

  /* If the floating point is low enough, we can simply return 0 */
/*   if ((number = expf(expofacs[0]*nx*nx+expofacs[1]*nx*ny+expofacs[2]*ny*ny+expofacs[4])) <= MINEXPONENT) */
/*     return (float) exp(number); */
/*   else */
    return (float) expf(expofacs[0]*nx*nx+expofacs[1]*nx*ny+expofacs[2]*ny*ny+expofacs[4]);
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Convolve a cube with a gaussian via fft */

Cube *convolgaussfft(Cube *cube, float *expofacsfft)
{
  int i, j, k, newsize, dummy;
  fftwf_complex *transformed_cube; /* pointer to the floating point array that becomes comlex afterwards */
  fftwf_plan plan, plin;           /* configuration variable for fftw for forwards and backwards transformation */
  int padding;                     /* information about the original padding */
  float expofacs[5];               /* factors in the exponent normalised to unity */
  float expresult;                 /* A dummy */

  /* Does the cube exist and contain an array? */
  if (cube && (*cube).points) {

/*     return NULL; */
    /* Pad the cube if necessary: */
    if ((padding = (*cube).padding) == 0)
      padcubex(cube);

    /* point the trasnsformed cube to the cube itself for an in-place transformation */
    transformed_cube = (fftwf_complex *) (*cube).points;

    /* Calculate the factors in the exponent */
    expofacs[0] = expofacsfft[0]/((*cube).size_x*(*cube).size_x);
    expofacs[1] = expofacsfft[1]/((*cube).size_x*(*cube).size_y);
    expofacs[2] = expofacsfft[2]/((*cube).size_y*(*cube).size_y);
    expofacs[3] = expofacsfft[3]/((*cube).size_v*(*cube).size_v);
    expofacs[4] = logf(expofacsfft[4]/((*cube).size_v*(*cube).size_y*(*cube).size_x));

    /* Calculate the factors in the exponent */
/*     expofacs[0] = expofacsfft[0]; */
/*     expofacs[1] = expofacsfft[1]; */
/*     expofacs[2] = expofacsfft[2]; */
/*     expofacs[3] = expofacsfft[3]; */
/*     expofacs[4] = logf(expofacsfft[4]); */
    /************/
    /************/
    /************/
    /************/
/*   return NULL; */

    /* Convolution in all dimensions or in xy only */
    if ((*cube).size_v > 1) {

      /* fill plan and plin with the necessary information. Take care with the order of the axes, reversed for fftw */
      plan = fftwf_plan_dft_r2c_3d((*cube).size_v, (*cube).size_y, (*cube).size_x, (*cube).points, transformed_cube, FFTW_ESTIMATE);
      plin = fftwf_plan_dft_c2r_3d((*cube).size_v, (*cube).size_y, (*cube).size_x, transformed_cube, (*cube).points, FFTW_ESTIMATE);

      /* Now do the transform */
      fftwf_execute(plan);

      /* multiply with the gaussian, first for nu_v = 0 */
      newsize = (*cube).size_x/2+1;
      for (i = 0; i < newsize; ++i) {
 for (j = 0; j < (*cube).size_y; ++j) {
   
   /* The exponential will be evaluated from 0, ... , N/2 and -1, ..., -N/2 or -N/2 - 1 */
   expresult = cubarithm_fftgaussian2d((i <= (*cube).size_x/2) ? i : (i-(*cube).size_x), (j <= (*cube).size_y/2) ? j : (j-(*cube).size_y), expofacs);
     transformed_cube[i+newsize*j][0] = expresult*transformed_cube[i+newsize*j][0];
     transformed_cube[i+newsize*j][1] = expresult*transformed_cube[i+newsize*j][1];
   }
      }

      /* Check for an extra-axis in v, i.e. if the dimension in v is even, we have to calculate one v-plane separately */
      if (!((*cube).size_v % 2)) {
 /* multiply with the gaussian, first for nu_v = N_v/2 */
 dummy = (*cube).size_v/2;
 for (i = 0; i < newsize; ++i) {
   for (j = 0; j < (*cube).size_y; ++j)
     {
       /* The exponential will be evaluated from 0, ... , N/2 and -1, ..., -N/2 or -N/2 - 1 */
     expresult = cubarithm_fftgaussian((i <= (*cube).size_x/2) ? i : (i-(*cube).size_x), (j <= (*cube).size_y/2) ? j : (j-(*cube).size_y), dummy, expofacs);
     transformed_cube[i+newsize*(j+(*cube).size_y*dummy)][0] = expresult*transformed_cube[i+newsize*(j+(*cube).size_y*dummy)][0];
     transformed_cube[i+newsize*(j+(*cube).size_y*dummy)][1] = expresult*transformed_cube[i+newsize*(j+(*cube).size_y*dummy)][1];
     }
 }
      }
      
      /* Now the rest has to be done, v ranges from 1, ..., N_v-1/2, and using the symmetrics of the gaussian we fill the rest */
      for (i = 0; i < newsize; ++i) {
 for (j = 0; j < (*cube).size_y; ++j) {
   for (k = 1; k <= ((*cube).size_v-1)/2; ++k) {
     expresult = cubarithm_fftgaussian((i <= (*cube).size_x/2) ? i : (i-(*cube).size_x), (j <= (*cube).size_y/2) ? j : (j-(*cube).size_y), k, expofacs);
     transformed_cube[i+newsize*(j+(*cube).size_y*k)][0] = expresult*transformed_cube[i+newsize*(j+(*cube).size_y*k)][0];
     transformed_cube[i+newsize*(j+(*cube).size_y*k)][1] = expresult*transformed_cube[i+newsize*(j+(*cube).size_y*k)][1];

     /* Because of the symmetry, f(v) = f(-v), we can safe quite some calculations */
     transformed_cube[i+newsize*(j+(*cube).size_y*((*cube).size_v-k))][0] = expresult*transformed_cube[i+newsize*(j+(*cube).size_y*((*cube).size_v-k))][0];
     transformed_cube[i+newsize*(j+(*cube).size_y*((*cube).size_v-k))][1] = expresult*transformed_cube[i+newsize*(j+(*cube).size_y*((*cube).size_v-k))][1];
   }
 }
      }

      /* Now do the backtransformation */
      fftwf_execute(plin);    
    }

    /* if the cube has only one plane or only convolution in xy is desired then do this: */
    else {
      plan = fftwf_plan_dft_r2c_2d((*cube).size_y, (*cube).size_x, (*cube).points, transformed_cube, FFTW_ESTIMATE);
      plin = fftwf_plan_dft_c2r_2d((*cube).size_y, (*cube).size_x, transformed_cube, (*cube).points, FFTW_ESTIMATE);    
      
      /* Now do the transform */
      fftwf_execute(plan);
      
      /* multiply with the gaussian, first axis y, second x */
      newsize = (*cube).size_x/2+1; /* The physical size of the cube in x */
      for (i = 0; i < newsize; ++i) {
 for (j = 0; j < (*cube).size_y; ++j) {
   expresult = cubarithm_fftgaussian2d((i <= (*cube).size_x/2) ? i : (i-(*cube).size_x), (j <= (*cube).size_y/2) ? j : (j-(*cube).size_y), expofacs);
   transformed_cube[i+newsize*j][0] = expresult*transformed_cube[i+newsize*j][0];
   transformed_cube[i+newsize*j][1] = expresult*transformed_cube[i+newsize*j][1];

 }
      }
      
      /* Now do the backtransformation */
      fftwf_execute(plin);    
    }
    
    /* For some reason fftw knows if the cube is padded for an in-place transform (i.e. it checks wether in = out) This means that we pad backwards only if there was no padding */
    if (!padding)
      padcubex(cube);

  return cube; 
  }

/* There is either no allocated array in the cube or no cube at all */
  else 
    return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Calculate factors needed by convolgaussfft */

float *expofacsfft(float sigma_maj, float sigma_min, float sigma_v, float *sincosofangle)
{
  float *expofacs;

  if ((sincosofangle && (expofacs = (float *) malloc(5*sizeof(float))))) {

  /* First content is the factor to put before (n_x/N_x)^2 */
  expofacs[0] = -2*PI_HERE*PI_HERE*(sigma_min*sigma_min*sincosofangle[1]*sincosofangle[1]+sigma_maj*sigma_maj*sincosofangle[0]*sincosofangle[0]);

  /* Second content is the factor to put before (n_x/N_x)(n_y/N_y) */
  expofacs[1] = -4*PI_HERE*PI_HERE*sincosofangle[0]*sincosofangle[1]*(sigma_min*sigma_min-sigma_maj*sigma_maj);

  /* Third content is the factor to put before (n_y/N_y)^2 */
  expofacs[2] = -2*PI_HERE*PI_HERE*(sigma_min*sigma_min*sincosofangle[0]*sincosofangle[0]+sigma_maj*sigma_maj*sincosofangle[1]*sincosofangle[1]);

  /* Fourth content is the factor to put before (n_v/N_v)^2 */
  expofacs[3] = -2*(PI_HERE*sigma_v*PI_HERE*sigma_v);

  /* Fifth component is the normalisation factor due to the width of the gaussians. This is not a factor to put in the exponent though. Here we have to care if only one direction conovolution is desired */
    if (sigma_maj == 0) 
      sigma_maj = 1.0/sqrtf(2*PI_HERE);
    
    if (sigma_min == 0)
      sigma_min = 1.0/sqrtf(2*PI_HERE);

  expofacs[4] = 2*PI_HERE*sigma_min*sigma_maj;
  }
  else
    expofacs = NULL;

  return expofacs;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Create a cube */

Cube *cubarithm_cube_create(void)
{
  Cube *cubarithm_create_cube = NULL;
  int i;
  
  if (!(cubarithm_create_cube = (Cube *) malloc(sizeof(Cube))))
    goto error;

  /* Set the pointers correctly */
  cubarithm_create_cube -> type_x = NULL;
  cubarithm_create_cube -> type_y = NULL;
  cubarithm_create_cube -> type_v = NULL;
  cubarithm_create_cube -> epoch = NULL;
  cubarithm_create_cube -> type = NULL;
  cubarithm_create_cube -> unit = NULL;
  cubarithm_create_cube -> header = NULL;
  cubarithm_create_cube -> asciiheader = NULL;
  cubarithm_create_cube -> points = NULL;
  cubarithm_create_cube -> wcs = NULL;

  /* Make the types and the epoch strings of length 80 */
  if (!(cubarithm_create_cube -> type_x = (char *) malloc(81*sizeof(char))))
    goto error;
  if (!(cubarithm_create_cube -> type_y = (char *) malloc(81*sizeof(char))))
    goto error;
  if (!(cubarithm_create_cube -> type_v = (char *) malloc(81*sizeof(char))))
    goto error;
  if (!(cubarithm_create_cube -> epoch = (char *) malloc(81*sizeof(char))))
    goto error;
  if (!(cubarithm_create_cube -> type = (char *) malloc(81*sizeof(char))))
    goto error;
  if (!(cubarithm_create_cube -> unit = (char *) malloc(81*sizeof(char))))
    goto error;

  /* Make them 80 byte empty buffers */
  for (i=0; i < 80; ++i) {
    cubarithm_create_cube -> type_x[i] = ' ';
    cubarithm_create_cube -> type_y[i] = ' ';
    cubarithm_create_cube -> type_v[i] = ' ';
    cubarithm_create_cube -> epoch[i] = ' ';
    cubarithm_create_cube -> type[i] = ' ';
    cubarithm_create_cube -> unit[i] = ' ';
  }
  cubarithm_create_cube -> type_x[i] = '\0';
  cubarithm_create_cube -> type_y[i] = '\0';
  cubarithm_create_cube -> type_v[i] = '\0';
  cubarithm_create_cube -> epoch[i] = '\0';
  cubarithm_create_cube -> type[i] = '\0';
  cubarithm_create_cube -> unit[i] = '\0';

  return cubarithm_create_cube;

 error:
  cubarithm_cube_destroy(cubarithm_create_cube);
  return NULL;
}  

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Read in a cube if possible */

void cubarithm_cube_destroy(Cube *cubev)
{
  if (!cubev)
    return;

  if (cubev -> type_x) free(cubev -> type_x);
  if (cubev -> type_y) free(cubev -> type_y);
  if (cubev -> type_v) free(cubev -> type_v);
  if (cubev -> epoch)  free(cubev -> epoch);
  if (cubev -> type)  free(cubev -> type);
  if (cubev -> unit)  free(cubev -> unit);

  if (cubev -> asciiheader) free(cubev -> asciiheader);
  if (cubev -> points) fftwf_free(cubev -> points);
  if (cubev -> wcs) wcsvfree(&(cubev -> nwcs), (struct wcsprm **) &(cubev -> wcs));
  free(cubev);

  return;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Read in a cube if possible */

int cubarithm_qf_copycard(qfits_header *origin, qfits_header *target, const char *key)
{
  const char *value = NULL;
  const char *comment = NULL;
  char *commentl = "";

  if (!(origin) || !(target))
    return 1;

  /* Now get key, key value, and comment from origin */
  if (!(value = qfits_header_getstr(origin, key))) {
    return 1;
  }

  /* We have to be a bit relaxed about this */
  if (!(comment = qfits_header_getcom(origin, key)))
    comment = commentl;

  /* OK, now either add or replace the value */
  cubarithm_qf_addrcard(target, key, value, comment);

  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Read in a cube if possible */

int cubarithm_qf_addrcard(qfits_header *target, const char *key, const char *value, const char *comment)
{
  const char *dummy = NULL;

  if ((dummy = qfits_header_getstr(target, key))) {
    qfits_header_mod(target, key, value, comment);
  }
  else {
    qfits_header_add(target, key, value, comment, NULL);
  }

  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Read in a cube if possible */

int cubarithm_readcube(const char *filename, Cube **cubename, char *errorstr)
{
  qfits_header *qheader = NULL;
  qfits_header *qhead_intern = NULL;
  char *cardvalue;
  char dummykey[9];
  char dummyval[21];
  char dummycomment[81];
  Cube *cubenamehook = NULL;
  int seghead_start = 0;
  int xtnumdat, segdat_start, segdat_size;
  FILE *filepointer = NULL;
  time_t now;
  struct tm *gregtime;
  int i,j;
  int numheads, numbytes;
  byte *bytebuffer = NULL;
  /* byte bytebuffer[8]; */
  int bitpix;
  int bytepix;
  int npix;
  int naxis;
  float *thefbuffer = NULL;

  /* wcs stuff */
  int nreject = 0;

  /* errors */
  int errorval = CUBARITHM_CUBE_ERROR_NONE;
  char errormes[120];

  errormes[0] = '\0';

  /* The header of the file will be read in first */
  if ((qheader = qfits_header_read(filename)) == NULL) {
    if ((errorstr))
      sprintf(errorstr, "%.40s: Problems reading cube (either not existent or wrong format)", filename);
    return CUBARITHM_CUBE_ERROR_READ;
  }
  if (*cubename == NULL) {
    if (!(cubenamehook = cubarithm_cube_create())) {
      errorval = CUBARITHM_CUBE_ERROR_MEM;
      sprintf(errormes, "%.40s: Memory problems reading cube.", filename);
      goto error;
    }
  }
  else {
    errorval = CUBARITHM_CUBE_ERROR_READ;
    sprintf(errormes, "%.40s: Problems reading cube (either not existent or wrong format)", filename);
    goto error;
  }

  /* Read in and check if the header is ok: first axis RA, second axis DEC, third axis freq, all other axes, if they exist with naxisi = 1 */
  if (!(cardvalue = qfits_header_getstr(qheader, "NAXIS"))) {
    errorval = CUBARITHM_CUBE_ERROR_NONAXIS;
    sprintf(errormes, "%.40s: Keyword NAXIS not found.", filename);
    goto error;
  }
  naxis = atoi(cardvalue);

  if (naxis < 3) {
    errorval = CUBARITHM_CUBE_ERROR_WRONGNAXIS;
    sprintf(errormes, "%.40s: Keyword NAXIS has wrong value (< 3).", filename);
    goto error;
  }

  /* Now check if naxisi = 1 for i > 3 */
  for (i = 4; i <= naxis; ++i) {

    sprintf(dummykey, "NAXIS%i",i);

    if (!(cardvalue = qfits_header_getstr(qheader, dummykey))) {
      errorval = CUBARITHM_CUBE_ERROR_NONAXISI;
      sprintf(errormes, "%.40s: Keyword NAXIS%i not found.", filename, i);
      goto error;
    }
    j = atoi(cardvalue);
    
    if (j > 1) {
      errorval = CUBARITHM_CUBE_ERROR_WRONGNAXISI;
      sprintf(errormes, "%.40s: NAXIS%i has the wrong value (> 1).", filename, i);
      goto error;
    }
  }

  if (!(cardvalue = qfits_header_getstr(qheader, "NAXIS1"))) {
    errorval = CUBARITHM_CUBE_ERROR_NONAXISI;
    sprintf(errormes, "%.40s: Keyword NAXIS%i not found.", filename, 1);
    goto error;
  }
  cubenamehook -> size_x = atoi(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "NAXIS2"))) {
    errorval = CUBARITHM_CUBE_ERROR_NONAXISI;
    sprintf(errormes, "%.40s: Keyword NAXIS%i not found.", filename, 2);
    goto error;
  }
  cubenamehook -> size_y = atoi(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "NAXIS3"))) {
    errorval = CUBARITHM_CUBE_ERROR_NONAXISI;
    sprintf(errormes, "%.40s: Keyword NAXIS%i not found.", filename, 3);
    goto error;
  }
  cubenamehook -> size_v = atoi(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "CRPIX1"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCRPIXI;
    sprintf(errormes, "%.40s: Keyword CRPIX%i not found.", filename, 1);   
    goto error;
  }
  cubenamehook -> refpix_x = atof(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "CRPIX2"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCRPIXI;
    sprintf(errormes, "%.40s: Keyword CRPIX%i not found.", filename, 2);   
    goto error;
  }
  cubenamehook -> refpix_y = atof(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "CRPIX3"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCRPIXI;
    sprintf(errormes, "%.40s: Keyword CRPIX%i not found.", filename, 3);   
    goto error;
  }
  cubenamehook -> refpix_v = atof(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "CRVAL1"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCRVALI;
    sprintf(errormes, "%.40s: Keyword CRVAL%i not found.", filename, 1);   
    goto error;
  }
  cubenamehook -> refval_x = atof(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "CRVAL2"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCRVALI;
    sprintf(errormes, "%.40s: Keyword CRVAL%i not found.", filename, 2);   
    goto error;
  }
  cubenamehook -> refval_y = atof(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "CRVAL3"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCRVALI;
    sprintf(errormes, "%.40s: Keyword CRVAL%i not found.", filename, 3);   
    goto error;
  }
  cubenamehook -> refval_v = atof(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "CDELT1"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCDELTI;
    sprintf(errormes, "%.40s: Keyword CDELT%i not found.", filename, 1);   
    goto error;
  }
  cubenamehook -> delt_x = atof(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "CDELT2"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCDELTI;
    sprintf(errormes, "%.40s: Keyword CDELT%i not found.", filename, 2);   
    goto error;
  }
  cubenamehook -> delt_y = atof(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "CDELT3"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCDELTI;
    sprintf(errormes, "%.40s: Keyword CDELT%i not found.", filename, 3);   
    goto error;
  }
  cubenamehook -> delt_v = atof(cardvalue);

  /* Next check axes 1-3, if right units, read them in */

  if (!(cardvalue = qfits_header_getstr(qheader, "CTYPE1"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCTYPEI;
    sprintf(errormes, "%.40s: Keyword CTYPE%i not found.", filename, 1);   
    goto error;
  }

  if (strlen(cardvalue) < 1) {
    errorval = CUBARITHM_CUBE_ERROR_WRONGCTYPEI;
    sprintf(errormes, "%.40s: CTYPE%i has the wrong value (should be RA...).", filename, 1);   
    goto error;
  }

  if (strncmp("RA",cardvalue+1,2)) {
    errorval = CUBARITHM_CUBE_ERROR_WRONGCTYPEI;
    sprintf(errormes, "%.40s: CTYPE%i has the wrong value (should be RA...).", filename, 1);   
    goto error;
  }

  strcpy(cubenamehook -> type_x, cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "CTYPE2"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCTYPEI;
    sprintf(errormes, "%.40s: Keyword CTYPE%i not found.", filename, 2);   
    goto error;
  }

  if (strlen(cardvalue) < 1) {
    errorval = CUBARITHM_CUBE_ERROR_WRONGCTYPEI;
    sprintf(errormes, "%.40s: CTYPE%i has the wrong value (should be DEC...).", filename, 2);   
    goto error;
  }

  if (strncmp("DEC",cardvalue+1,2)) {
    errorval = CUBARITHM_CUBE_ERROR_WRONGCTYPEI;
    sprintf(errormes, "%.40s: CTYPE%i has the wrong value (should be RA...).", filename, 2);   
    goto error;
  }

  strcpy(cubenamehook -> type_y, cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "CTYPE3"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOCTYPEI;
    sprintf(errormes, "%.40s: Keyword CTYPE%i not found.", filename, 3);   
    goto error;
  }

  if (strlen(cardvalue) < 1) {
    errorval = CUBARITHM_CUBE_ERROR_WRONGCTYPEI;
    sprintf(errormes, "%.40s: CTYPE%i has the wrong value (should be VELO, FELO, or FREQ...).", filename, 3);   
    goto error;
  }

  if (strncmp("VELO",cardvalue+1,4))
    if (strncmp("FELO",cardvalue+1,4))
      if (strncmp("FREQ",cardvalue+1,4)) {
	errorval = CUBARITHM_CUBE_ERROR_WRONGCTYPEI;
	sprintf(errormes, "%.40s: CTYPE%i has the wrong value (should be VELO, FELO, or FREQ...).", filename, 3);   
	goto error;
      }
  
  strcpy(cubenamehook -> type_v, cardvalue);
  
  /* Okidoki, now we want to copy across observatory velocity, rest frequency, epoch */

  if (!(cardvalue = qfits_header_getstr(qheader, "EPOCH"))) {
    errorval = CUBARITHM_CUBE_ERROR_NOEPOCH;
    sprintf(errormes, "%.40s: Keyword EPOCH not found.", filename);   
    goto error;
  }

  if (strlen(cardvalue) < 1) {
    errorval = CUBARITHM_CUBE_ERROR_WRONGEPOCH;
    sprintf(errormes, "%.40s: EPOCH has a wrong value.", filename);   
    goto error;
  }

  strcpy(cubenamehook -> epoch, cardvalue);

  /* rest frequency is not compulsory */
  if (!(cardvalue = qfits_header_getstr(qheader, "FREQ0"))) {
    if (!(cardvalue = qfits_header_getstr(qheader, "RESTFREQ"))) {
      cubenamehook -> restfreq = HIRESFREQ;
    }
    else {
      cubenamehook -> restfreq = atof(cardvalue);
    }
  }
  else {
      cubenamehook -> restfreq = atof(cardvalue);
  }

  /* vobs is not compulsory */
  if (!(cardvalue = qfits_header_getstr(qheader, "VOBS"))) {
    cubenamehook -> vobs = 0.0;
  }
  else {
      cubenamehook -> vobs = atof(cardvalue);
  }

  /* cellscal is not compulsory */
  if (!(cardvalue = qfits_header_getstr(qheader, "CELLSCAL"))) {
    cubenamehook -> cellscal = 0;
  }
  else {
    if (strncmp("1/F",cardvalue+1,3)) {
      cubenamehook -> cellscal = 1;
    }
    else {
      cubenamehook -> cellscal = 0;
    }
  }

  /* Now read in all the values */

  if (!(cardvalue = qfits_header_getstr(qheader, "BMAJ")))
    cubenamehook -> bmaj = -1.;
  else 
    cubenamehook -> bmaj = atof(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "BMIN")))
    cubenamehook -> bmin = -1.;
  else 
    cubenamehook -> bmin = atof(cardvalue);
  
  if (!(cardvalue = qfits_header_getstr(qheader, "BPA")))
    cubenamehook -> bpa = 0.;
  else 
    cubenamehook -> bpa = atof(cardvalue);
  

  if (!(cardvalue = qfits_header_getstr(qheader, "BSCALE")))
    cubenamehook -> scale = 1.;
  else
    cubenamehook -> scale = atof(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "BZERO")))
    cubenamehook -> zero = 0.;
  else
    cubenamehook -> zero = atof(cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "BTYPE")))
    strcpy(cubenamehook -> type, "'intensity'");
  else
    strcpy(cubenamehook -> type, cardvalue);

  if (!(cardvalue = qfits_header_getstr(qheader, "BUNIT")))
    strcpy(cubenamehook -> unit, "'JY/BEAM'");
  else
    strcpy(cubenamehook -> unit, cardvalue);

  if (strlen(cardvalue) < 1){
    errorval = CUBARITHM_CUBE_ERROR_WRONGBUNIT;
    sprintf(errormes, "%.40s: BUNIT has a wrong value.", filename);   
    goto error;
  }

  /* printf("x type: %s size: %i refpix: %f refval: %f delt: %f\n", cubenamehook -> type_x, cubenamehook -> size_x, cubenamehook -> refpix_x, cubenamehook -> refval_x, cubenamehook -> delt_x); */
  /* printf("y type: %s size: %i refpix: %f refval: %f delt: %f\n", cubenamehook -> type_y, cubenamehook -> size_y, cubenamehook -> refpix_y, cubenamehook -> refval_y, cubenamehook -> delt_y); */
  /* printf("v type: %s size: %i refpix: %f refval: %f delt: %f\n", cubenamehook -> type_v, cubenamehook -> size_v, cubenamehook -> refpix_v, cubenamehook -> refval_v, cubenamehook -> delt_v); */

  /* This is always the default */
  cubenamehook -> padding = 0;

  /* Now we create our own header */
  if (!(qhead_intern = qfits_header_default()))  {
      errorval = CUBARITHM_CUBE_ERROR_MEM;
      sprintf(errormes, "%.40s: Memory problems reading cube.", filename);
      goto error;
    }


  /* i tracks the number of cards */
  numheads = 2;

  /* Copy across the relevant parts with some fixed values */
  qfits_header_add(qhead_intern, "BITPIX", "-32", "", NULL);
  ++numheads;
  qfits_header_add(qhead_intern, "NAXIS", "3", "", NULL);
  ++numheads;

  /* These MUST work, otherwise we would have had an error before */
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "NAXIS1"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "NAXIS2"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "NAXIS3"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CRPIX1"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CRPIX2"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CRPIX3"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CRVAL1"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CRVAL2"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CRVAL3"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CDELT1"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CDELT2"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CDELT3"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CTYPE1"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CTYPE2"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "CTYPE3"))) {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "EPOCH")))  {errorval = CUBARITHM_CUBE_ERROR_MEM; sprintf(errormes, "%.40s: Memory problems reading cube.", filename); goto error;} else ++numheads;

  if ((cubarithm_qf_copycard(qheader, qhead_intern, "BUNIT"))) {sprintf(dummykey, "BUNIT"); sprintf(dummyval, "%s", cubenamehook -> unit); sprintf(dummycomment, " "); cubarithm_qf_addrcard(qhead_intern, "BUNIT", dummyval, dummycomment);  ++numheads;}
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "BSCALE"))) {sprintf(dummykey, "BSCALE"); sprintf(dummyval, "%+20.13E", cubenamehook -> scale); sprintf(dummycomment, " "); cubarithm_qf_addrcard(qhead_intern, "BSCALE", dummyval, dummycomment); ++numheads;}
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "BZERO"))) {sprintf(dummykey, "BZERO"); sprintf(dummyval, "%+20.13E", cubenamehook -> zero); sprintf(dummycomment, " "); cubarithm_qf_addrcard(qhead_intern, "BZERO", dummyval, dummycomment); ++numheads;}
  if ((cubarithm_qf_copycard(qheader, qhead_intern, "BTYPE")))  {sprintf(dummykey, "BTYPE"); sprintf(dummyval, "%s", cubenamehook -> type); sprintf(dummycomment, " "); cubarithm_qf_addrcard(qhead_intern, "BTYPE", dummyval, dummycomment); ++numheads;}
  if (!(cubarithm_qf_copycard(qheader, qhead_intern, "CELLSCAL"))) {++numheads;}
  if (!(cubarithm_qf_copycard(qheader, qhead_intern, "RESTFREQ"))) {++numheads;}
  if (!(cubarithm_qf_copycard(qheader, qhead_intern, "FREQ0"))) {++numheads;if ((cubarithm_qf_copycard(qheader, qhead_intern, "RESTFREQ"))) {sprintf(dummyval, "%+20.13E",cubenamehook -> restfreq); sprintf(dummycomment, "REST FREQUENCY: item added by TiRiFiC"); cubarithm_qf_addrcard(qhead_intern, "RESTFREQ", dummyval, dummycomment); ++numheads;}} 
  if (!(cubarithm_qf_copycard(qheader, qhead_intern, "BMAJ"))) {++numheads;}
  if (!(cubarithm_qf_copycard(qheader, qhead_intern, "BMIN"))) {++numheads;}
  if (!(cubarithm_qf_copycard(qheader, qhead_intern, "BPA"))) {++numheads;}

  sprintf(dummykey, "ORIGIN"); sprintf(dummyval, "'TiRiFiC'"); sprintf(dummycomment, " "); cubarithm_qf_addrcard(qhead_intern, "ORIGIN", dummyval, dummycomment); ++numheads;
  if (time(&now)) {
    gregtime = gmtime(&now);
    strftime(dummyval, 40, "'%y-%b-%d %I:%M:%S'", gregtime);
    sprintf(dummykey, "DATE"); 
    sprintf(dummycomment, "UTC");
    cubarithm_qf_addrcard(qhead_intern, "DATE", dummyval, dummycomment);
    ++numheads;
  }

  /* Well, well. Now we want to link the whole new header into the Cube struct and then dump the whole ascii header into the ascii part */
  cubenamehook -> header = qhead_intern;

  /* Now, how much space is needed? */
  numbytes = (roundup(((double) numheads)*80.0/2880.))*2880;
  cubenamehook -> headerbytes = numbytes;

  if (!(cubenamehook -> asciiheader = (char *) malloc((numbytes+1)*sizeof(char)))) {
    errorval = CUBARITHM_CUBE_ERROR_MEM; 
    sprintf(errormes, "%.40s: Memory problems reading cube.", filename); 
    goto error;
  }

  /* Don't know if this is necessary, all blanks in that header */
  for (j = 0; j < numbytes; ++j)
    cubenamehook -> asciiheader[j] = ' ';
  cubenamehook -> asciiheader[j] = '\0';

  /* Read them in one by one */
  for (i = 0; i < qhead_intern -> n; ++i) {
    numbytes = i*80;

    /* Get the key */
    qfits_header_getitem(qhead_intern, i, dummykey, NULL, NULL, NULL);

    /* Get the associated line */
    cardvalue = qfits_header_getline(qhead_intern, dummykey);

    /* fprintf(stderr, "read: key |%s| line |%s|\n", dummykey, cardvalue); */

    /* Put the line into the string */
    memcpy(cubenamehook -> asciiheader+numbytes,cardvalue,80);
  }
  
  /* Now we want to read in the header as a whole and the cube as a float, first find the byte numbers in the fits file */
  /* xtnumhead = 0; */
  xtnumdat = 0;

  /* qfits_get_hdrinfo(filename, xtnumhead, &seghead_start, &seghead_size); */
  qfits_get_datinfo(filename, xtnumdat, &segdat_start, &segdat_size);

  /* Open the rubbish as a binary file, and goto segdata_start */
  if (!(filepointer = fopen(filename, "rb"))) {
    errorval = CUBARITHM_CUBE_ERROR_READ; 
    sprintf(errormes, "%.40s: Memory problems reading cube (either not existent or wrong format).", filename); 
    goto error;
  }

  /* Just to get to the start of the file */
  fseek(filepointer, seghead_start, SEEK_SET);

  /* Then step forward to the right position */
  for (i = 0; i < segdat_start; ++i)
    fgetc(filepointer);

  /* Now we want to know BITPIX from the original header */
  bitpix = qfits_header_getint(qheader, "BITPIX", -512);

  if (bitpix < -64) {
    errorval = CUBARITHM_CUBE_ERROR_WRONGBITPIX; 
    sprintf(errormes, "%.40s: BITPIX has wrong value.", filename); 
    goto error;
  }
  bytepix = (bitpix/8)>0?bitpix/8:-bitpix/8;

  /* Next we want to know the number of pixels */
  cubenamehook -> sumpoints = npix = cubenamehook -> size_x * cubenamehook -> size_y * cubenamehook -> size_v;

  /* Yes, then we know what to allocate, btw byte = char */
  if (!(bytebuffer = (byte *) malloc(bytepix*npix*sizeof(byte)))) {
    errorval = CUBARITHM_CUBE_ERROR_MEM; 
    sprintf(errormes, "%.40s: Memory problems reading cube.", filename); 
    goto error;
  }

  /* Wonderful, now read it in */
  i = 0;

  /* This is sort of programming lazyness. If it turns out that we cannot afford to allocate the memory, we can also read in the values pixel by pixel */
  while (i < (npix*bytepix)) {
    bytebuffer[i] = fgetc(filepointer);
    ++i;
  }
  
  if (!(thefbuffer = (float *) fftwf_malloc(npix*sizeof(float)))) {
    errorval = CUBARITHM_CUBE_ERROR_MEM; 
    sprintf(errormes, "%.40s: Memory problems reading cube.", filename); 
    goto error;
  }

  if ((qfits_pixin_floatbuf(bytebuffer, npix, bitpix, cubenamehook -> scale, cubenamehook -> zero, thefbuffer))) {
    errorval = CUBARITHM_CUBE_ERROR_MEM; 
    sprintf(errormes, "%.40s: Memory problems reading cube.", filename); 
    goto error;
  }
  cubenamehook -> points = thefbuffer;

  /* Unbelievable that this works. We now step to the wcs part */
  cubenamehook -> nwcs = 0;

  /* Enable error messaging */
  wcserr_enable(1);

  if (wcspih(cubenamehook -> asciiheader, cubenamehook -> header -> n, WCSHDR_all, 2, &nreject, &(cubenamehook -> nwcs), (struct wcsprm **) &(cubenamehook -> wcs))) {
    wcsperr((struct wcsprm *) (cubenamehook -> wcs),"WCS error:");
    errorval = CUBARITHM_CUBE_ERROR_WCS; 
    sprintf(errormes, "%.40s: Problems activating WCS.", filename); 
    goto error;
  }
  /* That's f???ing well it. Easy, eh? */

  *cubename = cubenamehook;

  if ((filepointer))
    fclose(filepointer);
  if ((qheader))
    qfits_header_destroy(qheader);
  if ((bytebuffer))
    free(bytebuffer);

  if (errorstr)
    strcpy(errorstr, errormes);

  return errorval;

 error:
  if ((filepointer))
    fclose(filepointer);
  if ((qheader))
    qfits_header_destroy(qheader);
  if ((qhead_intern)) {
    qfits_header_destroy(qhead_intern);
    if ((cubenamehook))
      cubenamehook -> header = NULL;
  }
  if ((bytebuffer))
    free(bytebuffer);
  if ((cubenamehook))
    cubarithm_cube_destroy(cubenamehook);
  if ((thefbuffer))
    fftwf_free(thefbuffer);

  if (errorstr)
    strcpy(errorstr, errormes);

  return errorval;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Convert the coordsin triplet (pixel values, starting at 0) to world coordinates triplet coordsout */

int cubarithm_p2w(void *thewcsthingy, double *coordsin, double *coordsout)
{
  double intermediateworld[3];
  double output[3];
  double intermin[3];
  double phi, theta;
  int stat;
  struct wcsprm *thewcs;

  thewcs = (struct wcsprm *) thewcsthingy;

  if (!(thewcsthingy))
    return 1;

  wcsset(thewcs);

  /* Correct fits -> internal definition */
  intermin[0] = coordsin[0]+1.;
  intermin[1] = coordsin[1]+1.;
  intermin[2] = coordsin[2]+1.;

  if (wcsp2s(thewcs, 1, 3, intermin, intermediateworld, &phi, &theta, output, &stat))
    return 1;

  /* reshuffle */
  coordsout[0] = output[thewcs -> lng];
  coordsout[1] = output[thewcs -> lat];
  coordsout[2] = output[thewcs -> spec];

  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Convert the coordsin triplet (world coordinates, ra, dec, spec) to pixel coordinates triplet (starting at 0) */

int cubarithm_w2p(void *thewcsthingy, double *coordsin, double *coordsout)
{
  double intermediateworld[3];
  double input[3];
  double phi, theta;
  int stat;
  struct wcsprm * thewcs;

  if (!(thewcsthingy)) {
    return 1;
  }

  thewcs = (struct wcsprm *) thewcsthingy;

  if (thewcs == NULL)
    return 1;

  wcsset(thewcs);

  /* reshuffle, first input coordinate is ra, second dec, third spectral */
  input[thewcs -> lng] = coordsin[0];
  input[thewcs -> lat] = coordsin[1];
  input[thewcs -> spec] = coordsin[2];

  if (wcss2p(thewcs, 1, 3, input, &phi, &theta, intermediateworld, coordsout, &stat))
    return 1;

  /* Correct fits -> internal definition */
  coordsout[0] = coordsout[0]-1.;
  coordsout[1] = coordsout[1]-1.;
  coordsout[2] = coordsout[2]-1.;

  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Make a copy of the input cube incubus */

Cube *cubarithm_copycube(Cube *incubus)
{
  Cube *cpc = NULL;
  int i, j, k;
  int nreject = 0;

  if (!(cpc = cubarithm_cube_create()))
    goto error;

  cpc -> size_x = incubus -> size_x;
  cpc -> size_y = incubus -> size_y;
  cpc -> size_v = incubus -> size_v;

  cpc -> refpix_x = incubus -> refpix_x;
  cpc -> refpix_y = incubus -> refpix_y;
  cpc -> refpix_v = incubus -> refpix_v;

  cpc -> refval_x = incubus -> refval_x;
  cpc -> refval_y = incubus -> refval_y;
  cpc -> refval_v = incubus -> refval_v;

  cpc -> delt_x = incubus -> delt_x;
  cpc -> delt_y = incubus -> delt_y;
  cpc -> delt_v = incubus -> delt_v;

  /* This must be copied in person */
  strcpy(cpc -> type_x, incubus -> type_x);
  strcpy(cpc -> type_y, incubus -> type_y);
  strcpy(cpc -> type_v, incubus -> type_v);
  
  strcpy(cpc -> epoch, incubus -> epoch);
  cpc -> restfreq = incubus -> restfreq;
  cpc -> vobs = incubus -> vobs;
  cpc -> cellscal = incubus -> cellscal;

  cpc -> bmaj = incubus -> bmaj;
  cpc -> bmin = incubus -> bmin;
  cpc -> bpa  = incubus -> bpa;

  cpc -> scale = incubus -> scale;
  cpc -> zero  = incubus -> zero;

  strcpy(cpc -> type, incubus -> type);
  strcpy(cpc -> unit, incubus -> unit);

  cpc -> padding  = incubus -> padding;

  if (!(cpc -> header = qfits_header_copy(incubus -> header)))
    goto error;

  cpc -> headerbytes  = incubus -> headerbytes;
  if (!(cpc -> asciiheader = (char *) malloc((cpc -> headerbytes+1)*sizeof(char))))
    goto error;

  /* copy across the header */
  memcpy(cpc -> asciiheader, incubus -> asciiheader, cpc -> headerbytes+1);

  cpc -> nwcs  = 0;

  /* Enable error messaging */
  wcserr_enable(1);

  /* fprintf(stderr,"cpc -> asciiheader:\n|%s|\nincubus -> asciiheader:\n|%s|\nheader -> n %i\nheaderbytes: %i", incubus -> asciiheader, cpc -> asciiheader, cpc -> header -> n, (int) cpc -> headerbytes); */

  /*   if (cpc -> wcs == NULL) */
  /* fprintf(stderr,"The thing is NULL\n"); */


  /* Instead of copying the wcs struct, we re-generate it */
  if (wcspih(cpc -> asciiheader, cpc -> header -> n, WCSHDR_all, 2, &nreject, &(cpc -> nwcs), (struct wcsprm **) &(cpc -> wcs))) {
    wcsperr((struct wcsprm *) (cpc -> wcs),"WCS error:");
    goto error;
  }

  /* if (wcscopy(1,(struct wcsprm *) incubus -> wcs, (struct wcsprm *) cpc -> wcs)) { */
  /*   goto error; */
  /* } */

  cpc -> sumpoints  = incubus -> sumpoints;

  /* last but not least the float array change here */
  if ((cpc -> padding)) {
    if (!(cpc -> points = (float *) fftwf_malloc(cpc -> size_v*cpc -> size_y*((cpc -> size_x/2)*2+2)*sizeof(float))))
	goto error;
    for(k = cpc -> size_v-1; k >= 0; --k)
      for(j = cpc -> size_y-1; j >= 0; --j)
	for(i = cpc -> size_x-1; i >= 0; --i)
	  *findpixelrel(cpc, i, j, k, cpc -> padding) = *findpixelrel(incubus, i, j, k, incubus -> padding);
  }
  else {
    if (!(cpc -> points = (float *) fftwf_malloc(cpc -> sumpoints*sizeof(float))))
      goto error;
  for (i = 0; i < cpc -> sumpoints; ++i)
    cpc -> points[i]  = incubus -> points[i];
  }

  return cpc;

 error:
  cubarithm_cube_destroy(cpc);
  return NULL;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Make a copy of the input cube incubus */

int cubarithm_writecube(Cube *incubus, char *filename, float *replaceflarray)
{
  FILE *output;
  qfitsdumper qdumper;
  float *thearray;
  int padded = 0;
  
  /* quickly check if the cube is there */
  if (((!incubus)) || (!(incubus -> points)))
    return 1;
  
  /* open the file and check if writeable */
  if ((output = fopen(filename,"w")) == NULL)
    return 1;
  
  /* Write the modified header in the file and close it */
  qfits_header_dump(incubus -> header,output);
  fclose(output);
  
  /* check the replace */
  if (replaceflarray) {
    thearray = replaceflarray;
  }
  else {  
    
    /* check the padding */
    if (incubus -> padding) {
      padcubex_norealloc(incubus);
      padded = 1;
    }
    thearray = incubus -> points;
  }
  
  /* Fill the qdumper fields */
  qdumper.filename = filename;
  qdumper.npix = incubus -> size_x*incubus -> size_y*incubus->size_v;
  qdumper.ptype = PTYPE_FLOAT;
  qdumper.fbuf = thearray;
  qdumper.out_ptype = BPP_IEEE_FLOAT;
  
  /* write the cube */
  qfits_pixdump(&qdumper);
  
  /* now pad the fitsfile with zeros */
  qfits_zeropad(filename);
  
  /* Pad back */
  if ((padded))
    padcubex_norealloc(incubus);

     
  return 0;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

/* Write a cube using the reference frame in header */

int writecube(char *filename, Cube *cubename, qfits_header *header)
{
  FILE *output;
  qfits_header *outheader;
  qfitsdumper qdumper;
  char value[20];
  char *comment;

  /* quickly check if the cube is there */
  if ((!cubename) || (!(*cubename).points))
    return 0;

  /* open the file and check if writeable */
  if ((output = fopen(filename,"w")) == 0)
    return 0;

  /* Now copy the header */
  outheader = qfits_header_copy(header);

  /* The changes to be made is the size of the cube, and the reference
     pixel. All other stuff has been done before */

  /* size of the cube */
  comment = qfits_header_getcom(outheader,"NAXIS1");
  sprintf(value,"%u",(*cubename).size_x);
  qfits_header_mod(outheader,"NAXIS1",value,comment);
  
  comment = qfits_header_getcom(outheader,"NAXIS2");
  sprintf(value,"%u",(*cubename).size_y);
  qfits_header_mod(outheader,"NAXIS2",value,comment);
  
  comment = qfits_header_getcom(outheader,"NAXIS3");
  sprintf(value,"%u",(*cubename).size_v);
  qfits_header_mod(outheader,"NAXIS3",value,comment);
  
  /* reference pixel */
  comment = qfits_header_getcom(outheader,"CRPIX1");
  sprintf(value,"%0.13E",qfits_header_getdouble(outheader,"CRPIX1",0)-(double)(*cubename).refpix_x);
  qfits_header_mod(outheader,"CRPIX1",value,comment);

  comment = qfits_header_getcom(outheader,"CRPIX2");
  sprintf(value,"%0.13E",qfits_header_getdouble(outheader,"CRPIX2",0)-(double)(*cubename).refpix_y);
  qfits_header_mod(outheader,"CRPIX2",value,comment);

  comment = qfits_header_getcom(outheader,"CRPIX3");
  sprintf(value,"%0.13E",qfits_header_getdouble(outheader,"CRPIX3",0)-(double)(*cubename).refpix_v);
  qfits_header_mod(outheader,"CRPIX3",value,comment);

  comment = qfits_header_getcom(outheader,"BSCALE");
  sprintf(value,"%0.13E", (double)(*cubename).scale);
  qfits_header_mod(outheader,"BSCALE",value,comment);

  /* Now write the modified header in the file and close it */
  qfits_header_dump(outheader,output);
  fclose(output);

  /* check the padding */
  if ((*cubename).padding)
    padcubex(cubename);
  
  /* Fill the qdumper fields */
  qdumper.filename = filename;
  qdumper.npix = (*cubename).size_x*(*cubename).size_y*(*cubename).size_v;
  qdumper.ptype = PTYPE_FLOAT;
  qdumper.fbuf = (*cubename).points;
  qdumper.out_ptype = BPP_IEEE_FLOAT;
     
  /* write the cube */
  qfits_pixdump(&qdumper);

  /* now pad the fitsfile with zeros */
  qfits_zeropad(filename);
  
  /* don't forget to destroy the header */
  
  qfits_header_destroy(outheader);

  return 1;
}

/* ------------------------------------------------------------ */



/* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 

   $Log: cubarithm.c,v $
   Revision 1.6  2011/05/25 22:25:26  jozsa
   Left work

   Revision 1.5  2011/05/10 00:30:15  jozsa
   Left work

   Revision 1.4  2009/05/26 07:56:40  jozsa
   Left work

   Revision 1.3  2007/08/23 15:23:25  gjozsa
   Left work

   Revision 1.2  2007/08/22 15:58:40  gjozsa
   Left work

   Revision 1.1.1.1  2004/10/29 11:13:20  gjozsa
   Added to CVS control


   ------------------------------------------------------------ */

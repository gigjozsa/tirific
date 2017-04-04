# This is a very simple makefile.

SHELL = /bin/sh
PATH := .:$(PATH)

# Create a target without a file
.PHONY: clean virginal cleanplay document

include settings

# set openmp flags
ifeq ($(OPENMP),YES)
	OPENMPFLAG = -DOPENMPTIR $(OPENMPCOMP)
	OMPENGALINC = -I$(FFTWDIR)
	OMPENGALLIB = -lfftw3f_threads
else
	OPENMPFLAG = 
	OPENMPLIB =
	OMPENGALINC =
	OMPENGALLIB =
endif

# silly patch, but for now ok
OPENMPFFT = NO
ifeq ($(OPENMPFFT),YES)
	OPENMPFFTFLAG = -DOPENMPFFT
else
	OPENMPFFTFLAG = 
endif

# include pbcorr
ifeq ($(PBCORR),YES)
	PBCORRFLAG = -DPBCORR
else
	PBCORRFLAG = 
endif

# external includes
MATHINC = -I$(MATHDIR)
FFTWINC = -I$(FFTWDIR)
GSLINC = -I$(GSLDIR)
PGPINC = -I$(PGPDIR)
WCSINC = -I$(WCSDIR)

# GSL not really required
ifeq ($(GSLDIR),'')
GSLINC = 
GSLFLAG = 
GSLLIBR = 
else
GSLINC = -I$(GSLDIR)
GSLFLAG = -DGFT_GSL_PRESENT
GSLLIBR = $(GSLLIB)
endif

# This has to do with system differences
# ifeq ($(OS),MAC_OS_X)
#	GSLGFTLIB = ""
# else
#	GSLGFTLIB = $(GSLLIB)
# endif

# current directory
DIR := $(shell pwd)/

# Internal includes
# This is the directory in which the gft include file resides
GFTDIR = $(DIR)gft/
GFTINC = -I$(GFTDIR)

#qfits in use here
QFITS = $(DIR)qfits-6.2.0/qloc/
QFITSLIB = -L$(QFITS)lib -lqfits
QFITSDIR = $(QFITS)include/
QFITSINC = -I$(QFITSDIR)

# local include files are in include
LOCINCDIR = $(DIR)include/
LOCINC = -I$(LOCINCDIR)

# Source code and object files are in src
SRC = $(DIR)src/

# The single routine gets into bin
BIN = $(DIR)bin/

PROGS = $(BIN)tirific

#DOCUMENTATION setup here
DOCU = $(DIR)rawdocu/
DOCUSOURCES = $(shell ls) $(DIR)src/*.c $(DIR)include/*.h $(DIR)rawdocu/doxy.start $(DIR)rawdocu/maindocu.h

# This will generate all        
all: $(PROGS)

# Object files

$(SRC)simparse.o: $(SRC)simparse.c $(LOCINCDIR)simparse.h 
	@echo '#####################'
	@echo '# starting simparse.o #'
	@echo '#####################'
	$(CC) $(CFLAGS) -c -o $@ $< $(LOCINC) $(QFITSINC) $(MATHINC) $(FFTWINC)
	@echo '#####################'
	@echo '# simparse.o finished #'
	@echo '#####################'

$(SRC)tirific.o: $(SRC)tirific.c $(LOCINCDIR)engalmod.h $(LOCINCDIR)ftstab.h $(LOCINCDIR)pgp.h $(LOCINCDIR)maths.h $(LOCINCDIR)cubarithm.h $(MATHDIR)math.h $(FFTWDIR)fftw3.h $(GFTDIR)gft.h $(DIR)settings 
	@echo '###########################'
	@echo '# starting tirific.o #'
	@echo '###########################'
	$(CC) $(CFLAGS) -c -o $@ $< $(LOCINC) $(QFITSINC) $(MATHINC) $(FFTWINC) $(GFTINC) $(STDINC) $(WCSINC) -I$(GFTDIR) -D$(OS) -DNDISKS=$(NDISKS) $(OPENMPFLAG) $(PBCORRFLAG)
	@echo '###########################'
	@echo '# tirific.o finished #'
	@echo '###########################'

$(SRC)ftstab.o: $(SRC)ftstab.c $(LOCINCDIR)ftstab.h  $(QFITSDIR)qfits.h $(QFITSDIR)qfits_memory.h $(MATHDIR)math.h
	@echo '#####################'
	@echo '# starting ftstab.o #'
	@echo '#####################'
	$(CC) $(CFLAGS) -c -o $@ $< $(LOCINC) $(QFITSINC) $(MATHINC) $(FFTWINC)
	@echo '#####################'
	@echo '# ftstab.o finished #'
	@echo '#####################'

$(SRC)maths.o: $(SRC)maths.c $(LOCINCDIR)maths.h 
	@echo '####################'
	@echo '# starting maths.o #'
	@echo '####################'
	$(CC) $(CFLAGS) -c -o $@ $< $(LOCINC) $(QFITSINC) $(MATHINC) $(FFTWINC)
	@echo '####################'
	@echo '# maths.o finished #'
	@echo '####################'

$(SRC)cubarithm.o: $(SRC)cubarithm.c $(LOCINCDIR)cubarithm.h  $(LOCINCDIR)maths.h 
	@echo '########################'
	@echo '# starting cubarithm.o #'
	@echo '########################'
	$(CC) $(CFLAGS) -c -o $@ $< $(LOCINC) $(QFITSINC) $(MATHINC) $(FFTWINC) $(WCSINC)
	@echo '########################'
	@echo '# cubarithm.o finished #'
	@echo '########################'

$(SRC)pgp.o: $(SRC)pgp.c $(LOCINCDIR)pgp.h $(PGPDIR)/cpgplot.h
	@echo '##################'
	@echo '# starting pgp.o #'
	@echo '##################'
	$(CC) $(CFLAGS) -c -o $@ $< $(LOCINC) $(PGPINC) $(MATHINC)
	@echo '##################'
	@echo '# pgp.o finished #'
	@echo '##################'

$(SRC)engalmod.o: $(SRC)engalmod.c $(LOCINCDIR)engalmod.h  $(LOCINCDIR)maths.h $(MATHDIR)math.h $(FFTWDIR)fftw3.h
	@echo '#######################'
	@echo '# starting engalmod.o #'
	@echo '#######################'
	$(CC) $(CFLAGS) -c -o $@ $< $(LOCINC) $(QFITSINC) $(MATHINC) $(FFTWINC) $(OMPGALINC) $(OPENMPFLAG) $(OPENMPFFTFLAG) 
	@echo '#######################'
	@echo '# engalmod.o finished #'
	@echo '#######################'

$(SRC)fourat.o: $(SRC)fourat.c $(LOCINCDIR)fourat.h $(FFTWDIR)fftw3.h 
	@echo '#####################'
	@echo '# starting fourat.o #'
	@echo '#####################'
	$(CC) $(CFLAGS) -c -o $@ $< $(LOCINC) $(MATHINC) $(FFTWINC) $(OMPGALINC) $(OPENMPFLAG) 
	@echo '#####################'
	@echo '# fourat.o finished #'
	@echo '#####################'

$(SRC)tirific_defaults.o: $(SRC)tirific_defaults.c $(LOCINCDIR)tirific_defaults.h
	@echo '###############################'
	@echo '# starting tirific_defaults.o #'
	@echo '###############################'
	$(CC) $(CFLAGS) -c -o $@ $< $(LOCINC)
	@echo '###############################'
	@echo '# tirific_defaults.o finished #'
	@echo '###############################'

$(GFTDIR)gft.o: $(GFTDIR)/gft.c $(GFTDIR)/gft.h $(GFTDIR)/golden.h $(GFTDIR)/pswarm.h
	@echo '#########################'
	@echo '# starting gft.o #'
	@echo '#########################'
	$(CC) $(CFLAGS) -c -o $@ -I$(GFTDIR) $< $(GSLINC) $(GSLFLAG)
	@echo '#########################'
	@echo '# gft.o finished #'
	@echo '#########################'

$(GFTDIR)/golden.o: $(GFTDIR)/golden.c $(GFTDIR)/golden.h
	@echo '#####################'
	@echo '# starting golden.o #'
	@echo '#####################'
	$(CC) $(CFLAGS) -c -o $@ -I$(GFTDIR) $<
	@echo '#####################'
	@echo '# golden.o finished #'
	@echo '#####################'

$(GFTDIR)/pswarm.o: $(GFTDIR)/pswarm.c $(GFTDIR)/pswarm.h
	@echo '############'
	@echo '# pswarm.o #'
	@echo '############'
	$(CC) $(CFLAGS) -c -o $@ -I$(GFTDIR) $<
	@echo '#####################'
	@echo '# pswarm.o finished #'
	@echo '#####################'

# executables

OBJTIRIFIC = $(SRC)maths.o\
             $(SRC)tirific.o\
             $(SRC)ftstab.o\
             $(SRC)engalmod.o\
             $(SRC)cubarithm.o\
             $(SRC)simparse.o\
             $(SRC)pgp.o\
             $(SRC)fourat.o\
             $(SRC)tirific_defaults.o\
	     $(GFTDIR)gft.o\
             $(GFTDIR)golden.o\
             $(GFTDIR)pswarm.o


$(BIN)tirific: $(QFITS) $(OBJTIRIFIC) 
	@echo '#########################'
	@echo '# starting tirific #'
	@echo '#########################'
	$(CC) $(CFLAGS) -o $@ $(OBJTIRIFIC) $(WCSLIB) $(FFTWLIB) $(OMPENGALLIB) $(PGPLIB) $(QFITSLIB) $(MATHLIB) $(OPENMPLIB) $(READLINELIB) $(GSLLIBR)
	@echo '#########################'
	@echo '# tirific finished #'
	@echo '#########################'

# generating the documentation
document: $(DOCUSOURCES)
	@echo
	@echo '##############################'
	@echo '# STARTING THE DOCUMENTATION #'
	@echo '##############################'
	@echo
	doxygen $(DOCU)doxy.start
	@echo
	@echo '############################'
	@echo '# ENDING THE DOCUMENTATION #'
	@echo '############################'
	@echo

# generating the qfits library
$(QFITS): $(DIR)qfits-6.2.0
	@echo
	@echo '####################'
	@echo '# REBUILDING QFITS #'
	@echo '####################'
	@echo
	cd $(DIR)qfits-6.2.0;\
	cp configure.pre configure;\
	cp config.h.in.pre config.h.in;\
        configure --prefix=$(DIR)qfits-6.2.0/qloc --enable-shared=no;\
        make;\
	make install
	cd $(DIR)
	@echo
	@echo '########################'
	@echo '# ENDING QFITS REBUILD #'
	@echo '########################'
	@echo

clean:
	touch $(SRC)bla.o; rm -f $(SRC)*.o
	touch $(GFTDIR)bla.o; rm -f $(GFTDIR)*.o
	touch $(DIR)bin/tirific; rm -f $(DIR)bin/tirific
	cd $(DIR)qfits-6.2.0; make clean; rm -rf configure config.h.in Makefile config.h config.log config.status doc/Doxyfile libtool main/Makefile man/Makefile test/Makefile qloc saft/Makefile src/Makefile stamp-h1

virginal: clean
	touch $(DIR)qfits-6.2.0; rm -rf $(DIR)qfits-6.2.0
	touch $(DIR)doc; rm -rf $(DIR)doc
	touch $(DIR)bin/tirific; rm -f $(DIR)bin/tirific
	touch $(DOCUSOURCES)

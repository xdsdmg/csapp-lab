# Modify this line to indicate the default version to build

VERSION=std

# Comment this out if you don't have Tcl/Tk on your system

GUIMODE=-DHAS_GUI

# Modify the following line so that gcc can find the libtcl.so and
# libtk.so libraries on your system. You may need to use the -L option
# to tell gcc which directory to look in. Comment this out if you
# don't have Tcl/Tk.

TKLIBS=-L/usr/lib -ltk -ltcl

# Modify the following line so that gcc can find the tcl.h and tk.h
# header files on your system. Comment this out if you don't have
# Tcl/Tk.

TKINC=-isystem /usr/include/tcl8.6

# Modify these two lines to choose your compiler and compile time
# flags.

CC=gcc
# CFLAGS=-DUSE_INTERP_RESULT -Wall -O2
CFLAGS=-DUSE_INTERP_RESULT -Wall -O2 -fcommon

##################################################
# You shouldn't need to modify anything below here
##################################################

MISCDIR=../misc
HCL2C=$(MISCDIR)/hcl2c
INC=$(TKINC) -I$(MISCDIR) $(GUIMODE)
LIBS=$(TKLIBS) -lm
YAS = ../misc/yas

all: psim drivers

# This rule builds the PIPE simulator
psim: 
	# psim.c sim.h pipe-$(VERSION).hcl $(MISCDIR)/isa.c $(MISCDIR)/isa.h
	# Building the pipe-$(VERSION).hcl version of PIPE
	$(HCL2C) -n pipe-$(VERSION).hcl < pipe-$(VERSION).hcl > pipe-$(VERSION).c
	$(CC) $(CFLAGS) $(INC) -o psim psim.c pipe-$(VERSION).c \
		$(MISCDIR)/isa.c $(LIBS)

# This rule builds driver programs for Part C of the Architecture Lab
drivers: 
	./gen-driver.pl -n 4 -f ncopy.ys > sdriver.ys
	../misc/yas sdriver.ys
	./gen-driver.pl -n 63 -f ncopy.ys > ldriver.ys
	../misc/yas ldriver.ys

# These are implicit rules for assembling .yo files from .ys files.
.SUFFIXES: .ys .yo
.ys.yo:
	$(YAS) $*.ys


clean:
	rm -f psim pipe-*.c *.o *.exe *~ 

build: 
	make VERSION=full
	./psim -t sdriver.yo  									# To test your solution on a small 4-element array.
	./psim -t ldriver.yo									# To test your solution on a larger 63-element array.
	make drivers
	../misc/yis sdriver.yo									# Testing your driver files on the ISA simulator. Make sure that your ncopy.ys function works properly with YIS.
	./correctness.pl										# Testing your code on a range of block lengths with the ISA simulator.	
	cd ../y86-code; make testpsim 							# Testing your pipeline simulator with extensive regression tests.
	cd ../ptest; make SIM=../pipe/psim TFLAGS=-i 			# Testing your pipeline simulator with extensive regression tests.

test:
	./correctness.pl -p
	./benchmark.pl

K=6
test2:
	./gen-driver.pl -f ncopy.ys -n ${K} -rc > driver.ys
	make driver.yo
	../misc/yis driver.yo
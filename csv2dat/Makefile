# -*- makefile -*-

include ../Makefile-common

# Input files
BASEDIR = ..
LOCAL_LIBS =
LIBS =
LFLAGS = -static -static-libgcc -mconsole

# Output files
OUTPUT = csv2dat.exe

include ../Makefile-targets

csv2dat.exe: $(ALLOBJFILES)
	gcc $(LFLAGS) -o $@ $^ -L. $(LIBFLAGS) $(LOCAL_LIBFLAGS)

install: csv2dat.exe
	$(CP) $^ ../../magic_updater/

echo:
	which gcc
	gcc --version

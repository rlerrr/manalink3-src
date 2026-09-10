# -*- makefile -*-

include ../Makefile-common

LFLAGS = -static-libgcc -static-libstdc++

# Input files
BASEDIR = ..
SUBDIRS = .
LIBS = boost_regex-$(BOOST_SUFFIX)

# Output files
OUTPUT = find-missing-art.exe

include ../Makefile-targets

%.exe: $(ALLOBJFILES) $(LOCAL_LIBFILES)
	$(CXX) $(LFLAGS) -o $@ $(ALLOBJFILES) -L. $(LIBFLAGS) $(LOCAL_LIBFLAGS)
	$(OBJCOPY) --only-keep-debug $@ $(patsubst %.exe,%.dbg,$@)
	$(OBJCOPY) --strip-debug $@

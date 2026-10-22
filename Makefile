# -*- makefile -*-

include Makefile-common

# Flags
LFLAGS := $(LFLAGS) -Wl,-Tmanalink.lds --entry _DllEntry

# Input files
SUBDIRS = cards functions
include Makefile-excluded
CFILES := $(filter-out $(EXCLUDED_CFILES),$(CFILES))
AFILES := $(filter-out $(EXCLUDED_AFILES),$(AFILES))
AFLAGS += -DMANALINK_USE_C_HELPERS=1
LIBS = gdi32 msimg32
LOCAL_LIBS = capturestackbacktrace Drawcardlib

# Output files
OUTPUT = ManalinkEh.dll

include Makefile-targets

# The linker script loads this file first to preserve the veneer layout.
# Do not also include it in ALLOBJFILES or it is linked twice with OBJDIR set.
$(OUTPUT): ManalinkEh.ao
ManalinkEh.ao: ManalinkEh.asm ManalinkEh.mac
	$(ASM) $(AFLAGS) -o $@ $<

clean::
	-$(RM) ManalinkEh.ao

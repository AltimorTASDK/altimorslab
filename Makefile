DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
PATH := $(DEVKITPATH)/devkitPPC/bin:$(DEVKITPATH)/tools/bin:$(PATH)

AS := powerpc-eabi-as
CC := powerpc-eabi-gcc
LD := powerpc-eabi-ld

SRCDIR = src
OBJDIR = obj
DEPDIR = dep
SECTIONDIR = sections

SRCCFILES := $(wildcard $(SRCDIR)/*.c)
SRCCOBJFILES := $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCCFILES))
SRCSFILES := $(wildcard $(SRCDIR)/*.S)
SRCSOBJFILES := $(patsubst $(SRCDIR)/%.S, $(OBJDIR)/%.o, $(SRCSFILES))
SECTIONFILES := $(wildcard $(SECTIONDIR)/*.S)
SECTIONOBJFILES := $(patsubst $(SECTIONDIR)/%.S, $(OBJDIR)/%.o, $(SECTIONFILES))
OBJFILES := $(SRCCOBJFILES) $(SRCSOBJFILES) $(SECTIONOBJFILES)

PPCLIBS := $(DEVKITPPC)/powerpc-eabi/lib
LIBOGC := $(DEVKITPRO)/libogc
LINKSCRIPT := -Tmelee.ld
LIBPATHS := -L$(PPCLIBS) -L$(LIBOGC)/lib/cube
LIBS := -lc -lg -nostdlib

CFLAGS = -DGEKKO -mogc -mcpu=750 -meabi -mhard-float
LDFLAGS =

bin/sys/main.dol: $(OBJFILES)
	$(CC) $(LDFLAGS) $(LIBPATHS) $(LIBS) $(LINKSCRIPT) $^ -o $@
	python patch_dol.py

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -MMD -MP -MF $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $@) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.S
	$(AS) $^ -o $@

$(OBJDIR)/%.o: $(SECTIONDIR)/%.S
	$(AS) $^ -o $@

-include $(DEPDIR)/*.d
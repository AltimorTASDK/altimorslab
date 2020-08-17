DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
PATH := $(DEVKITPATH)/devkitPPC/bin:$(DEVKITPATH)/tools/bin:$(PATH)

AS := powerpc-eabi-as
CC := powerpc-eabi-gcc
LD := powerpc-eabi-ld

SOURCES := src src/melee sections
OBJDIR := obj
DEPDIR := dep

CFILES := $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.c))
SFILES := $(foreach dir, $(SOURCES), $(wildcard $(dir)/*.S))

OBJFILES := \
	$(patsubst %.c, $(OBJDIR)/%.o, $(CFILES)) \
	$(patsubst %.S, $(OBJDIR)/%.o, $(SFILES))

DEPFILES := $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $(OBJFILES))

LINKSCRIPT := -Tmelee.ld
LIBS := -nostdlib

CFLAGS = -DGEKKO -mogc -mcpu=750 -meabi -mhard-float
LDFLAGS = -Wl,-Map=output.map

bin/sys/main.dol: $(OBJFILES) GALE01.ld melee.ld patch_dol.py
	$(CC) $(LDFLAGS) $(LIBPATHS) $(LIBS) $(LINKSCRIPT) $(OBJFILES) -o $@
	python patch_dol.py

GALE01.ld: GALE01.map map_to_linker_script.py
	python map_to_linker_script.py

$(OBJDIR)/%.o: %.c
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(dir $@)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(dir $@))
	$(CC) -MMD -MP -MF $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $@) $(CFLAGS) -Isrc -c $< -o $@

$(OBJDIR)/%.o: %.S
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(AS) -mregnames -mgekko $^ -o $@

-include $(DEPFILES)
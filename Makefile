DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
PATH := $(DEVKITPATH)/devkitPPC/bin:$(DEVKITPATH)/tools/bin:$(PATH)

AS  := powerpc-eabi-as
CC  := powerpc-eabi-gcc
CXX := powerpc-eabi-g++
LD  := powerpc-eabi-ld

SOURCES := src sections
OBJDIR  := obj
DEPDIR  := dep

CFILES   := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.c'))
CXXFILES := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.cpp'))
SFILES   := $(foreach dir, $(SOURCES), $(shell find $(dir) -type f -name '*.S'))

OBJFILES := \
    $(patsubst %.c,   $(OBJDIR)/%.o, $(CFILES)) \
    $(patsubst %.cpp, $(OBJDIR)/%.o, $(CXXFILES)) \
    $(patsubst %.S,   $(OBJDIR)/%.o, $(SFILES))

DEPFILES := $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $(OBJFILES))

LIBOGC := $(DEVKITPATH)/libogc

LINKSCRIPT := -Tmelee.ld
LIBRARIES  := -L$(LIBOGC)/lib/cube -logc
LDFLAGS    := -Wl,-Map=output.map -nostdlib $(LIBRARIES)

CFLAGS   := -DGEKKO -mogc -mcpu=750 -meabi -mhard-float -O3 -Wall -Wno-register
CXXFLAGS := $(CFLAGS) -std=c++2a -fconcepts -fno-rtti -fno-exceptions
INCLUDE  := -Isrc -I$(LIBOGC)/include

bin/sys/main.dol: $(OBJFILES) GALE01.ld melee.ld patch_dol.py | clean_unused
	$(CC) $(LDFLAGS) $(LINKSCRIPT) $(OBJFILES) -o $@
	python patch_dol.py

GALE01.ld: GALE01.map map_to_linker_script.py
	python map_to_linker_script.py

$(OBJDIR)/%.o: %.c
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(dir $@)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(dir $@))
	$(CC) -MMD -MP -MF $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $@) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.o: %.cpp
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(dir $@)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(dir $@))
	$(CXX) -MMD -MP -MF $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $@) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.o: %.S
	@[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(AS) -mregnames -mgekko $^ -o $@

.PHONY: clean

clean:
	rm -rf obj/* dep/*

# Remove unused obj/dep files
clean_unused:
	$(foreach file, $(shell find obj -type f), $(if $(filter $(file), $(OBJFILES)),, $(shell rm $(file))))
	$(foreach file, $(shell find dep -type f), $(if $(filter $(file), $(DEPFILES)),, $(shell rm $(file))))
	$(foreach dir, $(shell find obj dep -type d), $(if $(wildcard $(dir)/*),, $(shell rm -r $(dir))))

-include $(DEPFILES)
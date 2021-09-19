DEVKITPATH=$(shell echo "$(DEVKITPRO)" | sed -e 's/^\([a-zA-Z]\):/\/\1/')
PATH := $(DEVKITPATH)/devkitPPC/bin:$(DEVKITPATH)/tools/bin:$(PATH)

AS  := powerpc-eabi-as
CC  := powerpc-eabi-gcc
CXX := powerpc-eabi-g++
LD  := powerpc-eabi-ld

SOURCES   := src sections
OBJDIR    := obj
DEPDIR    := dep
TOOLS     := tools

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
LDFLAGS    := -Wl,-Map=output.map -nostdlib

CFLAGS   := -DGEKKO -mogc -mcpu=750 -meabi -mhard-float -O3 -Wall -Wno-register -Wno-unused-value
CXXFLAGS := $(CFLAGS) -std=c++2b -fconcepts -fno-rtti -fno-exceptions
INCLUDE  := -Isrc -I$(LIBOGC)/include

bin/sys/main.dol: $(OBJFILES) GALE01.ld melee.ld $(TOOLS)/patch_dol.py | resources clean_unused
	$(CC) $(LDFLAGS) $(LINKSCRIPT) $(OBJFILES) -o $@
	python $(TOOLS)/patch_dol.py

GALE01.ld: GALE01.map $(TOOLS)/map_to_linker_script.py
	python $(TOOLS)/map_to_linker_script.py

$(OBJDIR)/%.o: %.c
	@[ -d $(@D) ] || mkdir -p $(@D)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(@D)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(@D))
	$(CC) -MMD -MP -MF $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $@) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.o: %.cpp
	@[ -d $(@D) ] || mkdir -p $(@D)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(@D)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(@D))
	$(CXX) -MMD -MP -MF $(patsubst $(OBJDIR)/%.o, $(DEPDIR)/%.d, $@) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

$(OBJDIR)/%.o: %.S
	@[ -d $(@D) ] || mkdir -p $(@D)
	@[ -d $(subst $(OBJDIR), $(DEPDIR), $(@D)) ] || mkdir -p $(subst $(OBJDIR), $(DEPDIR), $(@D))
	$(AS) -mregnames -mgekko $^ -o $@
	
RESOURCE_DIR_IN  := resources
RESOURCE_DIR_OUT := bin/files/lab
RESOURCES        := $(foreach dir, $(RESOURCE_DIR_IN), $(shell find $(dir) -type f))
RESOURCES        := $(filter-out %.psd, $(RESOURCES))

define get_resource_out
$(patsubst %.png, %.tex, $(subst $(RESOURCE_DIR_IN), $(RESOURCE_DIR_OUT), $1))
endef

define make_resource_rule
$(call get_resource_out, $1): $1 $(TOOLS)/copy_resource.py
	python $(TOOLS)/copy_resource.py $$< $$@
endef

RESOURCES_OUT := $(foreach resource, $(RESOURCES), $(call get_resource_out, $(resource)))

.PHONY: resources
resources: $(RESOURCES_OUT)

$(foreach resource, $(RESOURCES), $(eval $(call make_resource_rule, $(resource))))

.PHONY: clean
clean:
	rm -rf $(OBJDIR)/* $(DEPDIR)/*

# Remove unused obj/dep files
.PHONY: clean_unused
clean_unused:
	$(foreach file, $(shell find $(OBJDIR) -type f), $(if $(filter $(file), $(OBJFILES)),, $(shell rm $(file))))
	$(foreach file, $(shell find $(DEPDIR) -type f), $(if $(filter $(file), $(DEPFILES)),, $(shell rm $(file))))
	$(foreach file, $(shell find $(RESOURCE_DIR_OUT) -type f), $(if $(filter $(file), $(RESOURCES_OUT)),, $(shell rm $(file))))

-include $(DEPFILES)
#################################
# Base Makefile for subprojects #
#################################
#
# No Args

# Common compiler settings

# some GCC magic flags to autogen deps. In order:
# -MT $@ : set name of target in the dep makefile to be the original target
#   name in the main makefile
# -MMD : generate dependency information as a side effect of compilation,
#   ignoring system header deps (use -MD to include systen deps)
# -MP : add a target for each prereq to the list (in generated dep Makefile)
# -MF $(BUILDIR)/$*.mk : write the generated dep file to the listed path
DEPFLAGS       = -MT $@ -MMD -MP -MF $(BUILDIR)/$*.mk

# C compiler settings
CSTD     := -std=c11
CFLAGS   += -Wall -Wextra -pedantic --specs=nano.specs $(VENDOR_CFLAGS)
INCLUDES += -I$(PROJECT_ROOT)/lib/include $(VENDOR_INCLUDES)

# linker config
LDFLAGS += $(VENDOR_LDFLAGS) --specs=nano.specs --specs=nosys.specs\
           -Wl,-Map=$@.map -Wl,--cref -Wl,--warn-section-align
LDLIBS  += $(VENDOR_LDLIBS) -lnosys $(LDLIBS_END)

# Base commands
CC.C = $(CC) $(CSTD) $(CFLAGS) $(DEPFLAGS) $(INCLUDES)
LD.C = $(LD) $(LDFLAGS)

# directory defs
BUILDIR := build

# filexts
C_EXT     := c
MKDEP_EXT := mk
OBJ_EXT   := o

# Common Macros
# Must use deferred assignment to ensure that $(call ...) can replace the
# vars
wrap = $(addprefix $(1)/,$(addsuffix $(2),$(3)))
objpath = $(addprefix $(2)/,$(addsuffix .$(OBJ_EXT),$(1)))

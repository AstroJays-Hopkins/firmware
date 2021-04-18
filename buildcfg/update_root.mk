#################################
# Makefile rel directory config #
#################################

# set future root
PROJECT_ROOT := ../$(REL_ROOT)
CFGDIR        = $(PROJECT_ROOT)/buildcfg
HEXDIR        = $(PROJECT_ROOT)/bin
LIBROOT       = $(PROJECT_ROOT)/vendor
LIBCFG        = $(LIBROOT)/buildcfg
# Update makeflags for next recursive invokation
MAKE_INC     := -I$(CFGDIR) -I$(LIBCFG)
MAKEFLAGS    += $(MAKE_INC)

# Fix paths for current makefile
export PROJECT_ROOT     := $(REL_ROOT)
export REL_ROOT         := ../$(REL_ROOT)

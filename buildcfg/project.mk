###################################################
#       ARM Project Templated Makefile            #
###################################################
# Need to define
# - PROJ_NAME
# - PROJ_OBJS
# before including this
# C.F. http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/)

###################################################
#         BEGIN MAKEFILE INCLUDES                 #
###################################################
include $(CFGDIR)/project-meta.mk

###################################################
#         BEGIN MAKEFILE IMPLEMENTATION           #
###################################################
PROJ_DEPS := $(call objpath,$(PROJ_OBJS),$(BUILDIR))
INCLUDES += -I.


.DEFAULT_GOAL := all
.PHONY: all
all: $(BUILDIR)/$(PROJ_NAME).hex

# hex target
$(BUILDIR)/$(PROJ_NAME).hex: $(BUILDIR)/$(PROJ_NAME).elf
	@rm -f $@
	$(_P_HEX_$(V))$(OBJCOPY) -O ihex $<  $@
	$(_P_SIZE_$(V))$(SIZE) --format=sysv $<
	cp $@ $(HEXDIR)
# elf target
$(BUILDIR)/$(PROJ_NAME).elf: $(PROJ_DEPS) | $(BUILDIR)
	$(_P_LD_$(V))$(LD.C) -o $@  $^ $(LDLIBS)

# object build targets (with the dep Makefiles listed as deps so that the files
# are rebuilt if the dependency files go missing for any reason
$(BUILDIR)/%.$(OBJ_EXT): %.$(C_EXT) $(BUILDIR)/%.$(MKDEP_EXT) | $(BUILDIR)
	$(_P_CC_$(V))$(CC.C) -o $@ -c $<


# phony targets
.PHONY: clean
clean:
	rm -rf $(BUILDIR)
.PHONY: compiledb
compiledb:
	compiledb -o $(PROJECT_ROOT)/compile_commands.json make --always-make --dry-run all


# autogen deps includes and boilerplate to make everything just work (tm)
# target for buildir
$(BUILDIR):
	@mkdir -p $@
# define autogen'd deps files
C_SOURCES := $(shell find . -name '*.c')
SRC_DEP_FILES  := $(addprefix $(BUILDIR)/,$(notdir $(C_SOURCES:.$(C_EXT)=.$(MKDEP_EXT))))
# these empty targets are here to allow make to track the dependency files as
# deps themselves
$(SRC_DEP_FILES):

# include generated dependency files so objects are remade correctly
include $(wildcard $(SRC_DEP_FILES)) 

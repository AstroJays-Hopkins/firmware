###################################################
#            BEGIN MAKEFILE SETTINGS              #
###################################################
CFLAGS     +=
INCLUDES   +=
LDFLAGS    +=
LDLIBS     +=
LDLIBS_END +=

# Add all subdirectories here separated by the : char
vpath %.c .
# Project name (sets outputted bins)
PROJ_NAME := recovery
# Project deps
# add all object dependencies here without the .o extension

PROJ_OBJS := delay main globals handlers init clock i2c uart


###################################################
#            BEGIN MAKEFILE INCLUDES              #
###################################################
include update_root.mk
include $(LIBCFG)/atsamd21g18a.mk
include $(CFGDIR)/project.mk 

############################
# Makefile profiles config #
# ##########################
# 
# Opts:
# 
# CONFIG := debug | release
# NO_LTO - if defined, link time optimization will be disabled

# Profiles
################
# DEBUG configuration profile
CONF_DEBUG_CFLAGS  := -Og
CONF_DEBUG_LDFLAGS :=
CONF_DEBUG_LDLIBS  :=
# RELEASE Configuration profile
CONF_RELEASE_CFLAGS  := -Os -ffunction-sections -fdata-sections
CONF_RELEASE_LDFLAGS := -Os -Wl,--gc-sections
CONF_RELEASE_LDLIBS  :=
# Add additional config profiles here

ifndef NO_LTO # LTO enabled by defautl for release builds
	CONF_RELEASE_CFLAGS  += -flto
	CONF_RELEASE_LDFLAGS += -flto
endif
# set default build config to debug
ifndef CONFIG
	CONFIG := release
endif
# update vars
ifeq ($(CONFIG), debug)
	CFLAGS  += $(CONF_DEBUG_CFLAGS)
	LDFLAGS += $(CONF_DEBUG_LDFLAGS)
	LDLIBS  += $(CONF_DEBUG_LDLIBS)
else ifeq ($(CONFIG), release)
	CFLAGS  += $(CONF_RELEASE_CFLAGS)
	LDFLAGS += $(CONF_RELEASE_LDFLAGS)
	LDLIBS  += $(CONF_RELEASE_LDLIBS)
endif

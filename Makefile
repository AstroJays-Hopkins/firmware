# Root recursive Makefile
include buildcfg/update_root.mk
PROJECT_ROOT  := .
REL_ROOT      := ..
include $(CFGDIR)/verify_version.mk


ARTEFACTS := blink.hex

.DEFAULT_GOAL := all

# metatargets
.PHONY: all
all: aquisition recovery examples

.PHONY: aquisition recovery
aquisition recovery: common_deps
	$(MAKE) -C src $@

.PHONY: examples
# recursive targets
examples: common_deps
	$(MAKE) -C examples

# dep targets
.PHONY: common_deps
common_deps: _check_lib_vers vendor | $(HEXDIR)

.PHONY: vendor
vendor:
	$(MAKE) -C $(LIBROOT)

# utility targets
.PHONY: compiledb
compiledb:
	rm -f compile_commands.json
	$(MAKE) -Bnkw V=0 > build.log
	compiledb < build.log
ifeq ($(PLATFORM), Msys)
# windows pathfix
	@echo "Applying windows fixes to compile_commands.json";sed -i 's|/c/|/|' compile_commands.json
endif
	rm -f build.log
.PHONY: clean
clean:
	rm -rf $(HEXDIR)
	$(call_recursive)
.PHONY: libclean
libclean:
	$(MAKE) -C $(LIBROOT) clean

# special make targets
$(HEXDIR):
	@mkdir -p $@

# utilities
define call_recursive
	$(MAKE) -C examples $@
	$(MAKE) -C src $@
endef

include update_root.mk

.PHONY: all
all: aquisition recovery

.PHONY: aquisition
aquisition:
	$(MAKE) -C $@

.PHONY: recovery
recovery:
	$(MAKE) -C $@

.PHONY: clean compiledb
clean compiledb:
	$(MAKE) -C aquisition $@
	$(MAKE) -C recovery $@

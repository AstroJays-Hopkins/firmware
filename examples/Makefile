include update_root.mk

.PHONY: all
all: blink

.PHONY: blink
blink:
	$(MAKE) -C $@

.PHONY: clean compiledb
clean compiledb:
	$(MAKE) -C blink $@

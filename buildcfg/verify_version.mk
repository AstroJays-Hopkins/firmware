include $(CFGDIR)/defs.mk

########################
# Version verification #
########################
# Edit verify_version.c to set the minimum version for the vendored libraries

.PHONY: _check_lib_vers
_check_lib_vers: $(CFGDIR)/verify_version.c $(LIBROOT)/buildcfg/version.h
	@$(CC) -std=c11 -Wall -I$(PROJECT_ROOT) -o $(<:.c=.o) -c $<
	@rm -f $(<:.c=.o)
	@echo -e "\033[0;32mLibrary Version Checks Passed!\033[0m"

#include "vendor/buildcfg/version.h"

/* Vendored Library check
 *
 * allow any semver between 0.4.0 and 1.0.0
 */
#if _LIB_VENDOR_VERSION_MAJOR_ >= 0 && _LIB_VENDOR_VERSION_MAJOR_ < 1 \
    && _LIB_VENDOR_VERSION_MINOR_ >= 4 \
    && _LIB_VENDOR_VERSION_PATCH_ >= 0
#	define _LIB_VENDOR_OKAY_
#endif

#ifndef _LIB_VENDOR_OKAY_
#	error "Vendor library version incompatible with required version"
#endif

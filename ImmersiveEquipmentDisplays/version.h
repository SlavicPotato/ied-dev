#pragma once

#include "skse_version.h"

#include <ext/macro_helpers.h>

#define PLUGIN_VERSION_MAJOR    1
#define PLUGIN_VERSION_MINOR    6
#define PLUGIN_VERSION_REVISION 2

//#define PLUGIN_VERSION_ALPHA 1
//#define PLUGIN_VERSION_BETA 1

#if defined(PLUGIN_VERSION_ALPHA)
#	define PLUGIN_VERSION_APPEND "a"
#elif defined(PLUGIN_VERSION_BETA)
#	define PLUGIN_VERSION_APPEND "b"
#else
#	define PLUGIN_VERSION_APPEND ""
#endif

#define PLUGIN_VERSION_VERSTRING                                               \
	MK_STRING(PLUGIN_VERSION_MAJOR)                                            \
	"." MK_STRING(PLUGIN_VERSION_MINOR) "." MK_STRING(PLUGIN_VERSION_REVISION) \
		PLUGIN_VERSION_APPEND

#define MAKE_PLUGIN_VERSION(major, minor, rev) \
	(((major & 0xFF) << 16) | ((minor & 0xFF) << 8) | (rev & 0xFF))

#define GET_PLUGIN_VERSION_MAJOR(a) (((a)&0x00FF0000) >> 16)
#define GET_PLUGIN_VERSION_MINOR(a) (((a)&0x0000FF00) >> 8)
#define GET_PLUGIN_VERSION_REV(a)   (((a)&0x000000FF) >> 0)

static inline constexpr std::uint64_t VER_1_6_629 = 0x0001000602750000;

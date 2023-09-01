#pragma once

#include "skse_version.h"

#include <ext/macro_helpers.h>

#define PLUGIN_VERSION_MAJOR    1
#define PLUGIN_VERSION_MINOR    7
#define PLUGIN_VERSION_REVISION 3

#define PLUGIN_VERSION_PRE 0

#if PLUGIN_VERSION_PRE == 3
#	define PLUGIN_VERSION_APPEND "a"
#elif PLUGIN_VERSION_PRE == 2
#	define PLUGIN_VERSION_APPEND "b"
#elif PLUGIN_VERSION_PRE == 1
#	define PLUGIN_VERSION_APPEND "-RC21"
#else
#	define PLUGIN_VERSION_APPEND ""
#endif

#if defined(__AVX2__)
#	define PLUGIN_VERSION_CPUEXT " [AVX2]"
#elif defined(__AVX__) 
#	define PLUGIN_VERSION_CPUEXT " [AVX]"
#else
#	define PLUGIN_VERSION_CPUEXT ""
#endif

#define PLUGIN_VERSION_VERSTRING                                               \
	MK_STRING(PLUGIN_VERSION_MAJOR)                                            \
	"." MK_STRING(PLUGIN_VERSION_MINOR) "." MK_STRING(PLUGIN_VERSION_REVISION) \
		PLUGIN_VERSION_APPEND PLUGIN_VERSION_CPUEXT
			

#define MAKE_PLUGIN_VERSION(major, minor, rev) \
	(((major & 0xFF) << 16) | ((minor & 0xFF) << 8) | (rev & 0xFF))

#define GET_PLUGIN_VERSION_MAJOR(a) (((a)&0x00FF0000) >> 16)
#define GET_PLUGIN_VERSION_MINOR(a) (((a)&0x0000FF00) >> 8)
#define GET_PLUGIN_VERSION_REV(a)   (((a)&0x000000FF) >> 0)

static constexpr std::uint64_t VER_1_6_629 = 0x0001000602750000;

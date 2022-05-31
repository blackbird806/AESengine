#ifndef AES_OS_HPP
#define AES_OS_HPP

#ifdef _WIN32

	#define WINDOWS_LEAN_AND_MEAN
	#define NOMINMAX          // - Macros min(a,b) and max(a,b)
	#define NOOPENFILE        // - OpenFile(), OemToAnsi, AnsiToOem, and OF_*
	#define NOTEXTMETRIC      // - typedef TEXTMETRIC and associated routines
	#define NOKANJI           // - Kanji support stuff.
	#define NOHELP            // - Help engine interface.
	#define NOPROFILER        // - Profiler interface.
	#define NODEFERWINDOWPOS  // - DeferWindowPos routines
	#define NOMCX             // - Modem Configuration Extensions
	#include <windows.h>

	// bruh windows
	#undef far
	#undef near
	#undef RGB

#elif defined(__vita__)

	#include <vitasdk.h>

#endif

#endif

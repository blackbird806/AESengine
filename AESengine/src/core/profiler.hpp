#ifndef PROFILER_HPP
#define PROFILER_HPP

#undef min
#undef max
#include "Tracy.hpp"

#define AES_PROFILE_FUNCTION() ZoneScoped;
#define AES_PROFILE_SCOPE(name) ZoneScopedN(name);

#define AES_PROFILE_FRAME() FrameMark;
#define AES_PROFILE_FRAMEN(name) FrameMarkNamed();

#define AES_PROFILE_MEMORY_NEW(ptr, size) TracyAlloc(ptr, size);
#define AES_PROFILE_MEMORY_DELETE(ptr) TracyFree(ptr);

#endif
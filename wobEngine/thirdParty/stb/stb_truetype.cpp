#include "core/context.hpp"
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(x, u) ((void)(u),wob::context.allocator->allocate(x))
#define STBTT_free(x, u)  ((void)(u),wob::context.allocator->deallocate(x))
#include "stb/stb_truetype.h"
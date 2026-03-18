#include "core/context.hpp"
#define QUOI_MALLOC(x) context.allocator->allocate(x)
#define QUOI_FREE(x) context.allocator->deallocate(x)
#define QOI_IMPLEMENTATION
#include "qoi.h"
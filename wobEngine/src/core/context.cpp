#include "context.hpp"
#include "allocator.hpp"

thread_local wob::Context wob::context{&wob::profilerAlloc};
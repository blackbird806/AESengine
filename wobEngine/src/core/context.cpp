#include "context.hpp"

thread_local wob::Context wob::context{&wob::profilerAlloc};
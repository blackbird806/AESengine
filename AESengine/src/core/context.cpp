#include "context.hpp"

thread_local aes::Context aes::context{&aes::profilerAlloc};
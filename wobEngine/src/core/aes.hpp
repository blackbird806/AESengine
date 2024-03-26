#ifndef AES_HPP
#define AES_HPP

#include <cstdlib>

#include "coreMacros.hpp"

using uint = unsigned int;
using uchar = unsigned char;

#include "assert.hpp"
#include "profiler.hpp"
#include "debug.hpp"

#define AES_FATAL_ERROR(msg) {AES_LOG_ERROR(msg); AES_DEBUG_BREAK(); std::abort();}

#endif


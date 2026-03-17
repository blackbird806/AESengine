#ifndef WOB_RHI_HPP
#define WOB_RHI_HPP

#include "core/wob.hpp"

#ifdef _WIN32
	#define WOB_GRAPHIC_API_D3D11
#elif defined(__vita__)
	#define WOB_GRAPHIC_API_GXM
#endif

#endif

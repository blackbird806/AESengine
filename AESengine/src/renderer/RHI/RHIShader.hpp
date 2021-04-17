#ifndef RHISHADER_HPP
#define RHISHADER_HPP

#include "renderer/RHI/RHI.hpp"

#ifdef AES_GRAPHIC_API_D3D11
#include "renderer/RHI/D3D11/D3D11Shader.hpp"
#elif defined(__vita__)
#include "renderer/RHI/SceGxm/gxmShader.hpp"
#endif

#endif
#ifndef RHISHADER_REFLECTION_HPP
#define RHISHADER_REFLECTION_HPP

#include <string>

namespace wob
{
	struct UniformBufferReflectionInfo
	{
		std::string name;
		uint32_t index;
		size_t size;
	};
}

#endif

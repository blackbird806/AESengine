#include "utility.hpp"

#include <sstream>
#include <fstream>

wob::String wob::readFile(std::string_view file)
{
	WOB_PROFILE_FUNCTION();

	std::ifstream t(file.data(), std::ios::binary | std::ios::ate);
	if (!t.is_open())
	{
		WOB_FATAL_ERROR();
	}
	int const size = t.tellg();
	String buffer;
	buffer.resize(size);
	t.seekg(0);
	t.read(buffer.data(), size);

	return buffer;
}

std::vector<uint8_t> wob::readFileBin(std::string_view file)
{
	WOB_PROFILE_FUNCTION();

	std::ifstream input(file.data(), std::ios::binary);
	return std::vector<uint8_t>(std::istreambuf_iterator<char>(input), {});
}

std::vector<std::string> wob::split(std::string_view a, char sep)
{
	size_t s = 0;
	std::vector<std::string> result;
	for (size_t i = 0; i < a.size(); i++)
	{
		if (a[i] == sep)
		{
			result.push_back(std::string(&a[s], i - s));
			s = ++i;
		}
	}
	if (s != a.size()) 
		result.push_back(std::string(&a[s], a.size() - s));
	return result;
}
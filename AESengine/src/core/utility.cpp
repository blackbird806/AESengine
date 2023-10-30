#include "utility.hpp"

#include <sstream>
#include <fstream>

aes::String aes::readFile(std::string_view file)
{
	AES_PROFILE_FUNCTION();

	std::ifstream t(file.data());
	AES_ASSERT(t.is_open());
	t.seekg(0, std::ios::end);
	int const size = t.tellg();
	String buffer;
	buffer.resize(size);
	t.seekg(0);
	t.read(&buffer[0], size);

	return buffer;
}

std::vector<uint8_t> aes::readFileBin(std::string_view file)
{
	AES_PROFILE_FUNCTION();

	std::ifstream input(file.data(), std::ios::binary);
	return std::vector<uint8_t>(std::istreambuf_iterator<char>(input), {});
}

std::vector<std::string> aes::split(std::string_view a, char sep)
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
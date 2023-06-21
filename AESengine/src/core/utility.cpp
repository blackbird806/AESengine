#include "utility.hpp"

#include <sstream>
#include <fstream>

std::string aes::readFile(StringView file)
{
	std::ifstream const in(file.data());
	std::ostringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

//https://stackoverflow.com/a/18816228
aes::Array<uint8_t> aes::readFileBin(StringView fileName)
{
	std::ifstream file(fileName.data(), std::ios::binary | std::ios::ate);
	std::streamsize const size = file.tellg();
	file.seekg(0, std::ios::beg);

	aes::Array<uint8_t> buffer(globalAllocator);
	buffer.reserve(size);
	if (file.read((char*)buffer.data(), size))
	{
		return buffer;
	}
}

std::vector<std::string> aes::split(StringView a, char sep)
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
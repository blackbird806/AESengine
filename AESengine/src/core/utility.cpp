#include "utility.hpp"

#include <sstream>
#include <fstream>

std::string aes::readFile(std::string const& file)
{
	std::ifstream const in(file);
	std::ostringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

std::vector<uint8_t> aes::readFileBin(std::string const& file)
{
	std::ifstream input(file, std::ios::binary);
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
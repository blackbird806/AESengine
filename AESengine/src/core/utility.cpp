#include "utility.hpp"

#include <sstream>
#include <fstream>

std::string aes::readFile(std::filesystem::path const& file)
{
	std::ifstream in(file);
	std::ostringstream sstr;
	sstr << in.rdbuf();
	return sstr.str();
}

std::vector<unsigned char> aes::readFileBin(std::filesystem::path const& file)
{
	std::ifstream input(file, std::ios::binary);
	return std::vector<unsigned char>(std::istreambuf_iterator<char>(input), {});
}

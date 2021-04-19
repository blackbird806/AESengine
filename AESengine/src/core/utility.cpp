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
	return std::vector<unsigned char>(std::istreambuf_iterator<char>(input), {});
}

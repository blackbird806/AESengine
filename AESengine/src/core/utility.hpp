#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <vector>
#include <filesystem>

namespace aes {
	std::string readFile(std::filesystem::path const& file);
	std::vector<unsigned char> readFileBin(std::filesystem::path const& file);
}

#endif // !UTILITY_HPP

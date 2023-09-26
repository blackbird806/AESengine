#include <iostream>
#include <fstream>
#include "core/aes.hpp"
#include "core/debug.hpp"
#include "test/tests.hpp"

using namespace aes;

int main()
{
#ifdef __vita__
	aes::Logger::instance().addSink(std::make_unique<aes::PsvDebugScreenSink>());
	std::ofstream file("ux0:/log/aeslog.txt");
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(file));
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::cout));
#else
#endif
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::cout));

	test_RHI();

	return 0;
}

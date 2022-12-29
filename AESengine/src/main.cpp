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
	std::ostream file("ux0:/log/")
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>());
#else
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::cout));
#endif

	test_RHI();

	return 0;
}

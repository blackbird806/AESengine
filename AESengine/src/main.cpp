#include <iostream>
#include "core/aes.hpp"
#include "core/debug.hpp"
#include "test/tests.hpp"

int main()
{
#ifdef __vita__
	aes::Logger::instance().addSink(std::make_unique<aes::PsvDebugScreenSink>());
#else
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::cout));
#endif

	//aes::test_fonts();
	//aes::test_imgui();
	aes::test_bsp();

	return 0;
}

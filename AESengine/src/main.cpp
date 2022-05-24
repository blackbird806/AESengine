#include <iostream>
#include "core/debug.hpp"
#include "test/tests.hpp"

int main()
{
	aes::Logger::instance().addSink(std::make_unique<aes::StreamSink>(std::cout));
	aes::test_fonts();
	//aes::test_draw3d();
	return 0;
}

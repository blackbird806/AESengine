#include <iostream>
#include <fstream>
#include "core/aes.hpp"
#include "core/debug.hpp"
#include "core/uniquePtr.hpp"
#include "test/tests.hpp"

using namespace aes;

int main()
{
	auto streamSink = aes::makeUnique<aes::StreamSink>(std::cout);
	aes::Logger::instance().addSink(streamSink.get());
#ifdef __vita__
	auto debugScreenSink = aes::makeUnique<aes::PsvDebugScreenSink>(file);
	aes::Logger::instance().addSink(debugScreenSink.get());
	std::ofstream file("ux0:/log/aeslog.txt");
	auto fileSink = aes::makeUnique<aes::StreamSink>(file);
	aes::Logger::instance().addSink(fileSink.get());
#else
#endif
	AES_LOG("hello {} AAA {} BBB", "world", 12);
	//test_RHI();

	return 0;
}

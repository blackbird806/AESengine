#include <fmt/format.h>
#include "engine.hpp"

int main()
{
	fmt::print("hello {}\n", "world");
	aes::Engine engine({ "aes triangle" });
	engine.init();
	engine.run();
}


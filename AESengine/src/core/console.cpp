#include "console.hpp"

#include <algorithm>

#include <fmt/format.h>

using namespace aes;

Console::Console(std::basic_streambuf<std::ostream::char_type, std::ostream::traits_type>* sb) : outStream(sb)
{
}

void Console::registerCommand(std::string const& name, Command_t&& cmd)
{
	commandTable[name] = std::move(cmd);
}

void Console::sendChar(char c)
{
	switch (c)
	{
	case '\n':
	case '\r':
		if (!currentLine.empty())
		{
			processCurrentLine();
			currentLine.clear();
		}
		break;

	case '\b':
		if (!currentLine.empty())
			currentLine.erase(currentLine.size() - 1);
		break;

	default:
		currentLine += c;
		break;
	}
}

void Console::processCurrentLine()
{
	auto const it = std::find(currentLine.begin(), currentLine.end(), ' ');
	std::string cmdName(currentLine.begin(), it);
	std::string params(it, currentLine.end());
	auto const cmdIt = commandTable.find(cmdName);

	if (cmdIt != commandTable.end())
	{
		outStream << cmdIt->second(std::move(params));
	}
	else
	{
		//outStream << fmt::format("error command \"{}\" not found !\n", cmdName);
	}
}

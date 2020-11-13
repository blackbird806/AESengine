#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <ostream>
#include <unordered_map>
#include <functional>
#include <string>

namespace aes {

	// https://loulou.developpez.com/tutoriels/moteur3d/partie7/
	class Console
	{
	public:
		using Command_t = std::function<std::string(std::string)>;
		using CommandTable = std::unordered_map<std::string, Command_t>;

		Console(std::basic_streambuf<std::ostream::char_type, std::ostream::traits_type>* sb);

		void registerCommand(std::string const& name, Command_t&& cmd);
		void sendChar(char c);

	private:

		void processCurrentLine();
		std::ostream outStream;
		CommandTable commandTable;
		std::string currentLine;
	};
	
}

#endif // !CONSOLE_HPP

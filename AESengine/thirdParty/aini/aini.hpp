#ifndef AINI_HPP
#define AINI_HPP

#include <unordered_map>
#include <string>
#include <variant>

namespace aini {

	using Int_t = long long;
	using Float_t = double;

	enum class ValueType
	{
		Int,
		Float,
		String
	};

	auto constexpr default_section_name = "default";

	struct Reader
	{
		Reader(std::string const& conf);

		bool has_key(const char* key, const char* section = default_section_name);
		ValueType get_value_type(const char* key, const char* section = default_section_name);
		Int_t get_int(const char* key, const char* section = default_section_name);
		Float_t get_float(const char* key, const char* section = default_section_name);
		std::string get_string(const char* key, const char* section = default_section_name);
		std::string get_value_as_string(const char* key, const char* section = default_section_name);

	private:
		using Section = std::unordered_map<std::string, std::string>;

		std::unordered_map<std::string, Section> sections;
	};

	struct Writer
	{
		void set_int(const char* key, Int_t value, const char* section = default_section_name);
		void set_float(const char* key, Float_t value, const char* section = default_section_name);
		void set_string(const char* key, std::string const& value, const char* section = default_section_name);

		std::string write();

	private:
		using Section = std::unordered_map<std::string, std::variant<std::string, Int_t, Float_t>>;

		std::unordered_map<std::string, Section> sections;
	};
}
	
#ifdef AINI_IMPLEMENTATION
#include <ctype.h>
#include <algorithm>

namespace aini
{
	static bool valid_key_char(char c)
	{
		return isalnum(c) || c == '_' || c == '-';
	}
	
	Reader::Reader(std::string const& conf)
	{
		size_t index = 0;
		std::string current_session_name = default_section_name;
		while (index < conf.size())
		{
			// skip spaces
			while (index < conf.size() && isspace(conf[index])) { index++;  }
			
			// skip comments
			if (conf[index] == ';')
			{
				while (index < conf.size() && conf[index] != '\n') { index++; }
				continue;
			}

			// parse section
			if (conf[index] == '[')
			{
				size_t const start = ++index;
				while (index < conf.size() && conf[index] != ']') { index++; }
				current_session_name = std::string(&conf[start], index - start);
				index++; // skip ]
			}
			else if (isalnum(conf[index])) // parse pair
			{
				size_t const start_key = index++;
				while (index < conf.size() && valid_key_char(conf[index])) { index++; }
				std::string const key(&conf[start_key], index - start_key);

				// skip spaces
				while (index < conf.size() && isspace(conf[index])) { index++; }
				if (conf[index++] != '=')
				{
					// TODO error
					continue;
				}

				size_t const start_value = ++index;
				size_t strip = 0;
				if (conf[index] == '"')
				{
					index++;
					while (index < conf.size() && conf[index] != '"') { index++; }
					index++;
					strip = 1;
				}
				else
					while (index < conf.size() && conf[index] != '\n') { index++; }
				
				std::string const value(&conf[start_value + strip], index - start_value - 2 * strip);

				sections[current_session_name][key] = value;
			}
			
		}
	}

	bool Reader::has_key(const char* key, const char* section)
	{
		return sections[section].contains(key);
	}

	ValueType Reader::get_value_type(const char* key, const char* section)
	{
		auto const& value = sections[section][key];
		if (isalnum(value[0]) || value[0] == '-')
		{
			if (std::find(value.begin(), value.end(), '.') != value.end())
				return ValueType::Float;
			return ValueType::Int;
		}
		return ValueType::String;
	}

	Int_t Reader::get_int(const char* key, const char* section)
	{
		return static_cast<Int_t>(std::stoll(sections[section][key]));
	}

	Float_t Reader::get_float(const char* key, const char* section)
	{
		return static_cast<Float_t>(std::stod(sections[section][key]));
	}

	std::string Reader::get_value_as_string(const char* key, const char* section)
	{
		return sections[section][key];
	}
	
	std::string Reader::get_string(const char* key, const char* section)
	{
		return get_value_as_string(key, section);
	}

	void Writer::set_int(const char* key, Int_t value, const char* section)
	{
		sections[section][key] = value;
	}

	void Writer::set_float(const char* key, Float_t value, const char* section)
	{
		sections[section][key] = value;
	}

	void Writer::set_string(const char* key, std::string const& value, const char* section)
	{
		sections[section][key] = value;
	}

	std::string Writer::write()
	{
		std::string data;

		for (auto const& [section_name, section] : sections)
		{
			data += "[" + section_name + "]\n";
			for (auto const& [key, value] : section)
			{
				data += key + " = ";
				
				//https://stackoverflow.com/questions/65629944/how-to-convert-element-of-variant-to-stdstring-c
				std::visit([&data](auto&& arg) {
					using T = std::decay_t<decltype(arg)>;
					if constexpr (std::is_same_v<T, std::string>)
						data += arg;
					else if constexpr (std::is_arithmetic_v<T>)
						data += std::to_string(arg);
					else
						static_assert(always_false_v<T>, "not convertible to string");
					}, value);
				data += "\n";
			}
		}

		return data;
	}
}
#endif // AINI_IMPLEMENTATION


#endif
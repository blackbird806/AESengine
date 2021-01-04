#ifndef REFLECTION_HPP
#define REFLECTION_HPP

#include <vector>
#include <span>
#include <string>
#include <fmt/format.h>
#include <tuple>

namespace aes
{
	template<typename T>
	struct function_traits;

	template<typename R, typename ...Args>
	struct function_traits<R(Args...)>
	{
		static const size_t nargs = sizeof...(Args);

		typedef R result_type;

		template <size_t i>
		using type = typename std::tuple_element<i, std::tuple<Args...>>::type;
	};

	class TypeInfo
	{

	public:

		enum class Type
		{
			Null,
			Int8,
			Int16,
			Int32,
			Int64,

			UInt8,
			UInt16,
			UInt32,
			UInt64,

			Float32,
			Float64,

			Bool,
			String,
		};

		TypeInfo(Type t, const char* n) : name(n), type(t) {}
		
		const char* getName() const noexcept
		{
			return name;
		}

		Type getType() const noexcept
		{
			return type;
		}

	private:

		const char* name;
		Type type;
	};

	const char* to_string(TypeInfo::Type e)
	{
		switch (e)
		{
		case TypeInfo::Type::Null: return "Null";
		case TypeInfo::Type::Int8: return "Int8";
		case TypeInfo::Type::Int16: return "Int16";
		case TypeInfo::Type::Int32: return "Int32";
		case TypeInfo::Type::Int64: return "Int64";
		case TypeInfo::Type::UInt8: return "UInt8";
		case TypeInfo::Type::UInt16: return "UInt16";
		case TypeInfo::Type::UInt32: return "UInt32";
		case TypeInfo::Type::UInt64: return "UInt64";
		case TypeInfo::Type::Float32: return "Float32";
		case TypeInfo::Type::Float64: return "Float64";
		case TypeInfo::Type::Bool: return "Bool";
		case TypeInfo::Type::String: return "String";
		default: return "unknown";
		}
	}

	template<typename T>
	constexpr TypeInfo::Type getEnumType()
	{
		return TypeInfo::Type::Null;
	}

	template<>
	constexpr TypeInfo::Type getEnumType<int8_t>()
	{
		return TypeInfo::Type::Int8;
	}

	template<>
	constexpr TypeInfo::Type getEnumType<int16_t>()
	{
		return TypeInfo::Type::Int16;
	}

	template<>
	constexpr TypeInfo::Type getEnumType<int32_t>()
	{
		return TypeInfo::Type::Int32;
	}

	template<>
	constexpr TypeInfo::Type getEnumType<int64_t>()
	{
		return TypeInfo::Type::Int64;
	}
	
	template<>
	constexpr TypeInfo::Type getEnumType<uint8_t>()
	{
		return TypeInfo::Type::UInt8;
	}

	template<>
	constexpr TypeInfo::Type getEnumType<uint16_t>()
	{
		return TypeInfo::Type::UInt16;
	}

	template<>
	constexpr TypeInfo::Type getEnumType<uint32_t>()
	{
		return TypeInfo::Type::UInt32;
	}

	template<>
	constexpr TypeInfo::Type getEnumType<uint64_t>()
	{
		return TypeInfo::Type::UInt64;
	}

	template<>
	constexpr TypeInfo::Type getEnumType<std::string>()
	{
		return TypeInfo::Type::String;
	}

	template<>
	constexpr TypeInfo::Type getEnumType<float>()
	{
		return TypeInfo::Type::Float32;
	}

	template<>
	constexpr TypeInfo::Type getEnumType<double>()
	{
		return TypeInfo::Type::Float64;
	}
	
	template<>
	constexpr TypeInfo::Type getEnumType<bool>()
	{
		return TypeInfo::Type::Bool;
	}

	
	class Field : public TypeInfo
	{
	public:
		
		Field(Type t, const char* name, size_t off) : TypeInfo(t, name), offset(off)
		{
			
		}

		template<typename T, typename C>
		T& getValue(C& instance) const noexcept
		{
			return *reinterpret_cast<T*>((&instance + offset));
		}
		
	private:
		size_t offset;
	};
	
	class MethodInfo
	{
		
	public:

		MethodInfo(const char* name_, TypeInfo ret_, std::span<TypeInfo> args_)
		: name(name_), ret(ret_), args(args_.begin(), args_.end())
		{
			
		}
		
		const char* getName() const noexcept
		{
			return name;
		}

		TypeInfo getReturnType() const noexcept
		{
			return ret;
		}

		std::vector<TypeInfo> const& getArgs() const noexcept
		{
			return args;
		}
		
	private:
		const char* name;
		TypeInfo ret;
		std::vector<TypeInfo> args;
	};
	
	class ClassInfo
	{
	public:

		ClassInfo(const char* name_, std::span<ClassInfo> parents_, std::span<Field> fields_, std::span<MethodInfo> methods_) :
		name(name_), parents(parents_.begin(), parents_.end()), fields(fields_.begin(), fields_.end()), methods(methods_.begin(), methods_.end())
		{
			
		}
		
		const char* getName() const noexcept
		{
			return name;
		}

		std::vector<Field> const& getFields() const noexcept
		{
			return fields;
		}

		std::vector<Field> const& getMethods() const noexcept
		{
			return fields;
		}

		std::vector<ClassInfo> const& getParents() const noexcept
		{
			return parents;
		}

	private:
		
		const char* name;
		std::vector<ClassInfo> parents;
		std::vector<Field> fields;
		std::vector<MethodInfo> methods;
	};
}

template<>
struct fmt::formatter<aes::TypeInfo::Type>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(aes::TypeInfo::Type const& type, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), "{}", to_string(type));
	}
};

#define AES_REFLECT_BEGIN(className) static aes::ClassInfo& getTypeInfo() noexcept { \
	static aes::ClassInfo clInfo = []() {\
	auto const clName = #className;\
	std::vector<aes::Field> fields;\
	std::vector<aes::MethodInfo> methods;

#define AES_REFLECT_FIELD(className, fieldname) fields.push_back({aes::getEnumType<decltype(fieldname)>(), #fieldname, offsetof(className, fieldname)}); \

#define AES_REFLECT_END() return aes::ClassInfo(clName, {}, fields, methods);}(); return clInfo;}

#endif
